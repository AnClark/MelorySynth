// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * Plugin.cpp - Plugin interface for Melory synthesizer
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

#include "Plugin.hpp"
#include "Definitions.hpp"

#include "extra/ScopedDenormalDisable.hpp"

MeloryPlugin::MeloryPlugin()
    : Plugin(PARAM_COUNT, 0, STATE_COUNT)    // parameters, programs, states
    , fCurrentChannel(fParams[PARAM_CURRENT_CHANNEL])
    , fCurrentInstrument(0)
{
    // Init synth
    fSynth.setup(getSampleRate());
    fSynth.init_synth();

    // Init channel instruments to 0
    for (int i = 0; i < 16; ++i)
        fChannelInstruments[i] = 0;
}

MeloryPlugin::~MeloryPlugin()
{
    // Synth engine (xsynth) will automatically clean up in its destructor.
}

void MeloryPlugin::initParameter(uint32_t index, Parameter& parameter)
{
    parameter.hints = kParameterIsAutomatable;
    switch (index)
    {
    case PARAM_REVERB_LEVEL:
        parameter.name = "Reverb Level";
        parameter.symbol = "reverb_level";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.7f;
        break;
    case PARAM_REVERB_WIDTH:
        parameter.name = "Reverb Width";
        parameter.symbol = "reverb_width";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 100.0f;
        parameter.ranges.def = 10.0f;
        break;
    case PARAM_REVERB_DAMP:
        parameter.name = "Reverb Damp";
        parameter.symbol = "reverb_damp";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.4f;
        break;
    case PARAM_REVERB_SIZE:
        parameter.name = "Reverb Roomsize";
        parameter.symbol = "reverb_size";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.2f;
        parameter.ranges.def = 0.6f;
        break;
    case PARAM_REVERB_ON:
        parameter.name = "Reverb On";
        parameter.symbol = "reverb_on";
        parameter.hints |= kParameterIsBoolean;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;    // Default off
        break;
    case PARAM_CHORUS_TYPE:
        parameter.name = "Chorus Type";
        parameter.symbol = "chorus_type";
        parameter.hints |= kParameterIsInteger;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        break;
    case PARAM_CHORUS_DEPTH:
        parameter.name = "Chorus Depth";
        parameter.symbol = "chorus_depth";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 21.0f;
        parameter.ranges.def = 3.0f;
        break;
    case PARAM_CHORUS_SPEED:
        parameter.name = "Chorus Speed";
        parameter.symbol = "chorus_speed";
        parameter.ranges.min = 0.1f;
        parameter.ranges.max = 5.0f;
        parameter.ranges.def = 0.3f;
        break;
    case PARAM_CHORUS_LEV:
        parameter.name = "Chorus Level";
        parameter.symbol = "chorus_lev";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 10.0f;
        parameter.ranges.def = 3.0f;
        break;
    case PARAM_CHORUS_VOICES:
        parameter.name = "Chorus Voices";
        parameter.symbol = "chorus_voices";
        parameter.hints |= kParameterIsInteger;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 99.0f;
        parameter.ranges.def = 3.0f;
        break;
    case PARAM_CHORUS_ON:
        parameter.name = "Chorus On";
        parameter.symbol = "chorus_on";
        parameter.hints |= kParameterIsBoolean;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;    // Default off
        break;
    case PARAM_CHANNEL_PRESSURE:
        parameter.name = "Channel Pressure";
        parameter.symbol = "channel_pressure";
        parameter.hints |= kParameterIsInteger;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 127.0f;
        parameter.ranges.def = 0.0f;
        break;
    case PARAM_GAIN:
        parameter.name = "Gain";
        parameter.symbol = "gain";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.2f;
        parameter.ranges.def = 0.5f;    // Make it louder than Fluida.lv2
        break;
    case PARAM_FINETUNING:
        parameter.name = "Fine Tuning";
        parameter.symbol = "finetuning";
        parameter.ranges.min = 370.0f;
        parameter.ranges.max = 453.0f;
        parameter.ranges.def = 440.0f;
        break;
    case PARAM_CURRENT_CHANNEL:    // Current selected MIDI channel on UI side
        parameter.name = "Current Channel";
        parameter.symbol = "current_channel";
        parameter.hints |= kParameterIsInteger;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 15.0f;
        parameter.ranges.def = 0.0f;
        break;
    case PARAM_UPDATE_TRIGGER:
        parameter.name = "Update Params";    // User-friendly name
        parameter.symbol = "trigger";
        parameter.hints |= kParameterIsTrigger;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        break;
    case PARAM_ACTIVE_INST_ID_CHANNEL_0:
    case PARAM_ACTIVE_INST_ID_CHANNEL_1:
    case PARAM_ACTIVE_INST_ID_CHANNEL_2:
    case PARAM_ACTIVE_INST_ID_CHANNEL_3:
    case PARAM_ACTIVE_INST_ID_CHANNEL_4:
    case PARAM_ACTIVE_INST_ID_CHANNEL_5:
    case PARAM_ACTIVE_INST_ID_CHANNEL_6:
    case PARAM_ACTIVE_INST_ID_CHANNEL_7:
    case PARAM_ACTIVE_INST_ID_CHANNEL_8:
    case PARAM_ACTIVE_INST_ID_CHANNEL_9:
    case PARAM_ACTIVE_INST_ID_CHANNEL_10:
    case PARAM_ACTIVE_INST_ID_CHANNEL_11:
    case PARAM_ACTIVE_INST_ID_CHANNEL_12:
    case PARAM_ACTIVE_INST_ID_CHANNEL_13:
    case PARAM_ACTIVE_INST_ID_CHANNEL_14:
    case PARAM_ACTIVE_INST_ID_CHANNEL_15:
        char buf_name[32], buf_symbol[32];
        std::snprintf(buf_name, 32, "Inst ID Ch#%d", index - PARAM_ACTIVE_INST_ID_CHANNEL_0);
        parameter.name = String(buf_name);
        std::snprintf(buf_symbol, 32, "inst_id_ch_%d", index - PARAM_ACTIVE_INST_ID_CHANNEL_0);
        parameter.symbol = String(buf_symbol);

        parameter.hints |= kParameterIsOutput;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = MAX_POSSIBLE_INST_ID;
        parameter.ranges.def = 0.0f;
        break;
    default:
        break;
    }

    // Initialize parameter value on synth side
    setParameterValue(index, parameter.ranges.def);
}

void MeloryPlugin::initState(uint32_t index, State& state)
{
    state.hints = kStateIsHostWritable;

    switch (index)
    {
    case 0:    // STATE_SOUNDFONT_PATH
        state.key = STATE_SOUNDFONT_PATH;
        state.defaultValue = "";
        state.hints = kStateIsFilenamePath;
        break;
    case 1:    // STATE_CHANNEL_INSTRUMENT_LIST
        state.key = STATE_CHANNEL_INSTRUMENT_LIST;
        state.defaultValue = "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0";
        break;
    default:
        break;
    }
}

float MeloryPlugin::getParameterValue(uint32_t index) const
{
    DISTRHO_SAFE_ASSERT_RETURN(index < PARAM_COUNT && index >= 0, 0.0f)

    return fParams[index];
}

void MeloryPlugin::setParameterValue(uint32_t index, float value)
{
    DISTRHO_SAFE_ASSERT_RETURN(index < PARAM_COUNT && index >= 0, )

    fParams[index] = value;    // Store parameter value in local copy

    // For synth engine parameters, apply changes to synth engine
    switch (index)
    {
    case PARAM_REVERB_LEVEL:
        fSynth.reverb_level = (double)value;
        fSynth.set_reverb_levels();
        break;
    case PARAM_REVERB_WIDTH:
        fSynth.reverb_width = (double)value;
        fSynth.set_reverb_levels();
        break;
    case PARAM_REVERB_DAMP:
        fSynth.reverb_damp = (double)value;
        fSynth.set_reverb_levels();
        break;
    case PARAM_REVERB_SIZE:
        fSynth.reverb_roomsize = (double)value;
        fSynth.set_reverb_levels();
        break;
    case PARAM_REVERB_ON:
        fSynth.reverb_on = (value >= 1.0f);
        fSynth.set_reverb_on(int(value >= 1.0f));
        break;
    case PARAM_CHORUS_TYPE:
        fSynth.chorus_type = (int)value;
        fSynth.set_chorus_levels();
        break;
    case PARAM_CHORUS_DEPTH:
        fSynth.chorus_depth = (double)value;
        fSynth.set_chorus_levels();
        break;
    case PARAM_CHORUS_SPEED:
        fSynth.chorus_speed = (double)value;
        fSynth.set_chorus_levels();
        break;
    case PARAM_CHORUS_LEV:
        fSynth.chorus_level = (double)value;
        fSynth.set_chorus_levels();
        break;
    case PARAM_CHORUS_VOICES:
        fSynth.chorus_voices = (int)value;
        fSynth.set_chorus_levels();
        break;
    case PARAM_CHORUS_ON:
        fSynth.chorus_on = (value >= 1.0f);
        fSynth.set_chorus_on(int(value >= 1.0f));
        break;
    case PARAM_CHANNEL_PRESSURE:
        fSynth.channel_pressure = (int)value;
        // TODO: Apply channel pressure to all channels or a specific one?
        break;
    case PARAM_GAIN:
        fSynth.volume_level = (double)value;
        fSynth.set_gain();
        break;
    case PARAM_FINETUNING:
        fSynth.finetune(value);
        break;
    case PARAM_UPDATE_TRIGGER:
        // This parameter actually does nothing, just to inform the host to save changes our plugin made.
        break;
    default:
        break;
    }
}

String MeloryPlugin::getState(const char* key) const
{
    if (std::strcmp(key, STATE_SOUNDFONT_PATH) == 0)
    {
        return fSoundfontPath;
    }
    else if (std::strcmp(key, STATE_CHANNEL_INSTRUMENT_LIST) == 0)
    {
        d_stderr2("getState: request STATE_CHANNEL_INSTRUMENT_LIST: %s", _exportChannelInstrumentList().buffer());
        return _exportChannelInstrumentList();
    }

    return String();
}

void MeloryPlugin::setState(const char* key, const char* value)
{
    if (std::strcmp(key, STATE_SOUNDFONT_PATH) == 0)
    {
        fSoundfontPath = String(value);
        // TODO: Load SF2 asynchronously to avoid blocking the audio thread (by using a separate thread)
        _loadSoundfontSync();
    }
    else if (std::strcmp(key, STATE_CHANNEL_INSTRUMENT_LIST) == 0)
    {
        _applyChannelInstrumentList(value);
    }
}

void MeloryPlugin::run(const float** inputs, float** outputs, uint32_t frames,
    const MidiEvent* midiEvents, uint32_t midiEventCount)
{
    ScopedDenormalDisable p_disableDenormals;    // Prevent denormalized floats for performance

    // Handle MIDI events for synth
    for (uint32_t i = 0; i < midiEventCount; ++i)
    {
        const MidiEvent&     midiEvent(midiEvents[i]);
        const uint8_t* const msg = midiEvent.data;
        const uint8_t        channel = msg[0] & 0x0f;

        switch (midiEvent.data[0] & 0xF0)
        {
        case 0x80:    // Note Off
            fSynth.synth_note_off(channel, msg[1]);
            break;
        case 0x90:    // Note On
            if (msg[2] == 0)
                fSynth.synth_note_off(channel, msg[1]);
            else
                fSynth.synth_note_on(channel, msg[1], msg[2]);
            break;
        case 0xB0:    // Control Change
            switch (msg[1])
            {
            case 0x78:    // All Sounds Off
            case 0x7B:    // All Notes Off
                fSynth.panic();
                break;
            case 0x20:    // Bank Select LSB
            case 0x00:    // Bank Select MSB
                // TODO: Notify UI on bank change (maintain an output-only parameter "current bank LSB/MSB"?)
                //       Method: to edit parameter, so UI can see the change via parameterChanged() callback.
                fSynth.synth_bank_changed(channel, msg[2]);
                _updateInstIdParam(channel);
                break;
            case 0x06:    // Data Entry MSB (for RPN/NRPN, but simplified)
            case 0x26:    // Data Entry LSB
                // TODO: Handle RPN/NRPN if needed
                break;
            default:
                fSynth.synth_send_cc(channel, msg[1], msg[2]);
                break;
            }
            break;
        case 0xE0:    // Pitch Bend
            fSynth.synth_send_pitch_bend(channel, (msg[2] << 7) | msg[1]);
            break;
        case 0xC0:    // Program Change
            fSynth.synth_pgm_changed(channel, msg[1]);
            _updateInstIdParam(channel);
            break;
        default:
            // Ignore other MIDI messages
            break;
        }
    }

    // Generate audio
    fSynth.synth_process(frames, outputs[0], outputs[1]);
}

void MeloryPlugin::_loadSoundfontSync()
{
    if (fSynth.load_soundfont(fSoundfontPath) == 0)
    {
        if (fCurrentInstrument < (int)fSynth.instruments.size())
        {
            fSynth.synth_pgm_changed(fCurrentChannel, fCurrentInstrument);
        }
        else
        {
            fCurrentInstrument = 0;
        }

        // Apply current channel instrument settings
        // NOTE: At this point, fChannelInstruments[] should already be set correctly (from state)
        _setAllChannelInstruments();
    }
    else
    {
        fSoundfontPath.clear();
    }
}

String MeloryPlugin::_exportChannelInstrumentList() const
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

void MeloryPlugin::_applyChannelInstrumentList(const char* value)
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
        fSynth.set_instrument_on_channel(channel, instrumentId);

        // Remember to update the parameter as well
        _updateInstIdParam(channel);                    // Active instrument ID parameter
        fChannelInstruments[channel] = instrumentId;    // User selection of instrument ID

        start = end + 1;
        end = valStr.find(',', start);

        ++channel;
    }
    // Last instrument
    if (channel < 16)
    {
        int instrumentId = std::atoi(valStr.substr(start).c_str());
        fSynth.set_instrument_on_channel(channel, instrumentId);

        // Remember to update the parameter as well
        _updateInstIdParam(channel);                    // Active instrument ID parameter
        fChannelInstruments[channel] = instrumentId;    // User selection of instrument ID
    }
}

void MeloryPlugin::_setAllChannelInstruments()
{
    for (int channel = 0; channel < 16; ++channel)
    {
        int instrumentId = fChannelInstruments[channel];
        fSynth.set_instrument_on_channel(channel, instrumentId);

        // Remember to update the parameter as well
        _updateInstIdParam(channel);    // Active instrument ID parameter
    }
}

void MeloryPlugin::_updateInstIdParam(uint32_t channel)
{
    fParams[PARAM_ACTIVE_INST_ID_CHANNEL_0 + channel] = fSynth.get_instrument_for_channel(channel);
}

// --------------------------------------------------------------------------------------------------------------------

START_NAMESPACE_DISTRHO

Plugin* createPlugin()
{
    return new MeloryPlugin();
}

END_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------
