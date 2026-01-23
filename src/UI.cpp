// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * UI.cpp - Editor UI for Fluida-XT synthesizer
 *
 * Copyright (C) 2026 AnClark Liu <anclarkliu@outlook.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "UI.hpp"
#include <string>    // For std::string

// --------------------------------------------------------------------------------------------------------------------

MeloryUI::MeloryUI()
    : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true)
{
}

// --------------------------------------------------------------------
// DSP/Plugin Callbacks

void MeloryUI::parameterChanged(uint32_t index, float value)
{
    DISTRHO_SAFE_ASSERT_RETURN(index < PARAM_COUNT && index >= 0, )

    // some hosts send parameter change events for output parameters even when nothing changed
    // we catch that here in order to prevent excessive repaints
    if (d_isEqual(fParams[index], value))
        return;

    fParams[index] = value;

#if 0    // For test only
    if (index >= PARAM_INST_ID_CHANNEL_0 && index <= PARAM_INST_ID_CHANNEL_15) {
        uint32_t channel = index - PARAM_INST_ID_CHANNEL_0;
        int instrumentID = static_cast<int>(value);
        d_stderr2("UI: Notified that instrument ID for channel %u changed to %d", channel, instrumentID);
    }
#endif

    repaint();
}

void MeloryUI::stateChanged(const char* key, const char* value)
{
    //d_stderr2("UI stateChanged(). Key: %s", key);

    if (std::strcmp(key, STATE_SOUNDFONT_PATH) == 0)
    {
        // Update local storage of soundfont path
        fSoundfontPath = String(value);

        // Now, let's load and parse the soundfont
        // NOTICE: DPF updates state asynchronously, so this may happen in the audio thread!
        //         UI side may not get the notification immediately after the user selects a file.
        _loadAndParseSoundfont();

        // Inform host about state changes
        // Some hosts like REAPER won't reveal to our state changes, so we need to wake them up!
        _triggerHostUpdate();
    }
    else if (std::strcmp(key, STATE_CHANNEL_INSTRUMENT_LIST) == 0)
    {
        _applyChannelInstrumentList(value);
    }
}

// --------------------------------------------------------------------------------------------------------------------
// Widget Callbacks

void MeloryUI::onImGuiDisplay()
{
    const float          width = getWidth();
    const float          height = getHeight();
    const float          margin = 20.0f * getScaleFactor();

    // Make window fullscreen
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    static constexpr auto window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    if (ImGui::Begin(DISTRHO_PLUGIN_NAME, nullptr, window_flags))
    {
        ImGui::Text("Soundfont path:");
        ImGui::TextWrapped("%s", fSoundfontPath.buffer());

        if (ImGui::Button("Open Soundfont (with DPF built-in FD)"))
        {
            // Tell DPF to open a file dialog to let us choose a soundfont file
            // NOTICE: Plugin sets STATE_SOUNDFONT_PATH asynchronously, so we need to call _triggerHostUpdate() in stateChanged().
            requestStateFile(STATE_SOUNDFONT_PATH);
        }

        {
            if (ImGui::SliderFloat("Active channel", &fParams[PARAM_CURRENT_CHANNEL], 0.0f, 15.0f))
            {
                if (ImGui::IsItemActivated())
                    editParameter(PARAM_CURRENT_CHANNEL, true);

                setParameterValue(PARAM_CURRENT_CHANNEL, fParams[PARAM_CURRENT_CHANNEL]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(PARAM_CURRENT_CHANNEL, false);
            }
        }

        {
            if (ImGui::SliderFloat("Gain (dB)", &fParams[PARAM_GAIN], 0.0f, 1.2f))
            {
                if (ImGui::IsItemActivated())
                    editParameter(PARAM_GAIN, true);

                setParameterValue(PARAM_GAIN, fParams[PARAM_GAIN]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(PARAM_GAIN, false);
            }
        }

        {
            if (ImGui::BeginTable("table1", 3, 0))
            {
                // Submit columns name with TableSetupColumn() and call TableHeadersRow() to create a row with a header in each column.
                // (Later we will show how TableSetupColumn() has other uses, optional flags, sizing weight etc.)
                ImGui::TableSetupColumn("Channel");
                ImGui::TableSetupColumn("Selected Inst ID");
                ImGui::TableSetupColumn("Active Inst ID");
                ImGui::TableHeadersRow();
                for (int row = 0; row <= 15; row++)
                {
                    ImGui::TableNextRow();

                    // Column 0: Channel number
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Channel %d", row + 1);    // Display channel number (1-16)

                    // Column 1: Selected Instrument ID (from parameter)
                    ImGui::TableSetColumnIndex(1);
                    if (ImGui::SliderInt(("##inst_id_sel_" + std::to_string(row)).c_str(),
                            (int*)&fChannelInstruments[row], 0, 127))
                    {
                        // Build up channel inst list, then set state
                        // This will notify the plugin side to update the synth
                        setState(STATE_CHANNEL_INSTRUMENT_LIST, _exportChannelInstrumentList().buffer());

                        // Inform host about state changes
                        _triggerHostUpdate();
                    }

                    // Column 2: Active Instrument ID (from synth)
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%d", (int)fParams[PARAM_ACTIVE_INST_ID_CHANNEL_0 + row]);
                }
                ImGui::EndTable();
            }
        }
    }
    ImGui::End();
}

// --------------------------------------------------------------------------
// Helper and Utilities

void MeloryUI::_loadAndParseSoundfont()
{
    if (fSoundfontPath.isEmpty())
        return;

    if (fSoundfontParser.loadSoundfont(fSoundfontPath.buffer()) == 0)
    {
        fSoundfontParser.parseSoundfont();
        d_stderr2(">>> UI: Soundfont loaded and parsed successfully.");
    }
    else
    {
        d_stderr2(">>> UI: Failed to load soundfont.");
    }
}

String MeloryUI::_exportChannelInstrumentList()
{
    std::string channelInstruments;
    for (int i = 0; i < 16; ++i)
    {
        channelInstruments += std::to_string(fChannelInstruments[i]);
        if (i < 15)
            channelInstruments += ",";
    }
    return String(channelInstruments.c_str());
}

void MeloryUI::_applyChannelInstrumentList(const char* value)
{
    std::string valStr(value);

    // Validate input: must contain exactly 15 commas for 16 values
    size_t      commaCount = 0;
    for (char c : valStr)
    {
        if (c == ',')
            ++commaCount;
    }
    if (commaCount != 15)
    {
        // TODO: Warn user on UI side, then ask user to re-apply instrument settings
        d_stderr2("_applyChannelInstrumentList: Invalid input format, expected 15 commas for 16 values, got %zu commas in '%s'", commaCount, valStr.c_str());
        return;    // Skip applying invalid state
    }

    size_t start = 0;
    size_t end = valStr.find(',');
    int    channel = 0;

    // Parse comma-separated instrument IDs
    while (end != std::string::npos && channel < 16)
    {
        int instrumentId = std::atoi(valStr.substr(start, end - start).c_str());
        fChannelInstruments[channel] = instrumentId;
        start = end + 1;
        end = valStr.find(',', start);
        ++channel;
    }
    // Last instrument
    if (channel < 16)
    {
        int instrumentId = std::atoi(valStr.substr(start).c_str());
        fChannelInstruments[channel] = instrumentId;
    }
}

void MeloryUI::_triggerHostUpdate()
{
    // Let host know we've changed state.
    //
    // Some hosts like REAPER won't react when you simply set a state (but not parameter),
    // so it may not save our new states to project file.
    //
    // Here I set a special plugin parameter as a trigger, to let the host know we've made some changes from plugin.
    editParameter(PARAM_UPDATE_TRIGGER, true);
    setParameterValue(PARAM_UPDATE_TRIGGER, 1.0f);
    editParameter(PARAM_UPDATE_TRIGGER, false);
}

// --------------------------------------------------------------------------------------------------------------------

START_NAMESPACE_DISTRHO

UI* createUI()
{
    return new MeloryUI();
}

END_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------
