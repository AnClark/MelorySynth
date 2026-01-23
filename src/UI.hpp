// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * UI.hpp - Editor UI for Melory synthesizer (header file)
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

#include "DistrhoUI.hpp"
#include "Definitions.hpp"

#include <fluidsynth.h>

// --------------------------------------------------------------------------------------------------------------------
// FluidSoundfontParser - Parse soundfont files to extract information without starting audio engine
//
// Since DPF does not support sending custom data from Plugin to UI, we use this class to parse.
//

class FluidSoundfontParser
{
    fluid_synth_t*    fFluidInstance;    // A minimum FluidSynth instance (for parsing soundfonts only)
    fluid_settings_t* fFluidSettings;    // Dummy settings for parsing only
    int               fSoundfontId;

public:
    std::vector<String> fInstrumentList;    // List of instruments found in the soundfont

    FluidSoundfontParser()
        : fFluidInstance(NULL)
        , fFluidSettings(NULL)
        , fSoundfontId(FLUID_FAILED)
    {
        fFluidSettings = new_fluid_settings();    // A dummy, empty settings object
        fFluidInstance = new_fluid_synth(fFluidSettings);
    }
    ~FluidSoundfontParser()
    {
        if (fSoundfontId != -1)
        {
            fluid_synth_sfunload(fFluidInstance, fSoundfontId, 0);
            fSoundfontId = -1;
        }
        if (fFluidInstance)
            delete_fluid_synth(fFluidInstance);
        if (fFluidSettings)
            delete_fluid_settings(fFluidSettings);
    }

public:
    // Load soundfont file on UI side for parsing.
    // TODO: Handle errors properly: Inform user on UI side if loading fails (via message box or banner).
    // TIPS:
    // - Configure my own log function fluid_set_log_function()
    // - Set error flag in log function, then onImGuiDisplay() can poll the error flag at real-time
    // - Store error message when needed
    int loadSoundfont(const char* path)
    {
        d_stderr2(">>> UI: Loading soundfont: %s", path);

        if (!fFluidInstance)
            return FLUID_FAILED;
        if (fSoundfontId != FLUID_FAILED)
            fluid_synth_sfunload(fFluidInstance, fSoundfontId, 0);

        fSoundfontId = fluid_synth_sfload(fFluidInstance, path, 0);
        if (fSoundfontId == FLUID_FAILED)
        {
            return 1;
        }
        return FLUID_OK;
    }

    // Parse loaded soundfont to extract instrument list.
    // TODO: Handle error properly: Inform user on UI side if parsing fails.
    int parseSoundfont()
    {
        if (fSoundfontId == FLUID_FAILED)
        {
            d_stderr2(">>> UI: Cannot parse soundfont, not loaded\n");
            return FLUID_FAILED;
        }

        fInstrumentList.clear();
        fluid_sfont_t* p_sfont = fluid_synth_get_sfont_by_id(fFluidInstance, fSoundfontId);
        int            p_offset = fluid_synth_get_bank_offset(fFluidInstance, fSoundfontId);

        if (p_sfont == NULL)
        {
            d_stderr2(">>> UI: Failed to get soundfont by ID\n");
            return FLUID_FAILED;
        }

        // NOTE: In my plugin I embed FluidSynth 2.x only, not 1.x.
        // So I only implement the 2.x API here.
        fluid_preset_t* p_preset;
        fluid_sfont_iteration_start(p_sfont);

        while ((p_preset = fluid_sfont_iteration_next(p_sfont)) != NULL)
        {
            char inst_name[100];
            snprintf(inst_name, 100, "%03d %03d %s", fluid_preset_get_banknum(p_preset) + p_offset,
                fluid_preset_get_num(p_preset), fluid_preset_get_name(p_preset));
            fInstrumentList.push_back(String(inst_name));
        }

        return FLUID_OK;
    }
};

using DISTRHO::UI;

// --------------------------------------------------------------------------------------------------------------------

class MeloryUI : public UI
{
public:
    MeloryUI();

protected:
    // ----------------------------------------------------------------------------------------------------------------
    // DSP/Plugin Callbacks

    void parameterChanged(uint32_t index, float value) override;
    void stateChanged(const char* key, const char* value) override;

    // ----------------------------------------------------------------------------------------------------------------
    // Widget Callbacks

    void onImGuiDisplay() override;

private:
    //--------------------------------------------------------------------
    // Parameters (local copies) and Instances

    float                fParams[PARAM_COUNT];    // Local copy of parameter values
    String               fSoundfontPath;          // Path to the loaded soundfont

    uint32_t             fActiveChannel;             // Currently selected MIDI channel
    int                  fChannelInstruments[16];    // Current instrument IDs per channel

    FluidSoundfontParser fSoundfontParser;    // Soundfont parser instance

    //--------------------------------------------------------------------
    // Helpers and Utilities

    void                 _loadAndParseSoundfont();
    void                 _applyChannelInstrumentList(const char* value);
    String               _exportChannelInstrumentList();

    void                 _triggerHostUpdate();

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeloryUI)
};
