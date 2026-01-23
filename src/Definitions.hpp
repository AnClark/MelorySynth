// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * Definitions.hpp - Common definitions (enums, etc.)
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

#pragma once

//-------------------------------------------------------------------
// Parameters

enum PluginParams {
    // Synth engine parameters
    PARAM_REVERB_LEVEL,
    PARAM_REVERB_WIDTH,
    PARAM_REVERB_DAMP,
    PARAM_REVERB_SIZE,
    PARAM_REVERB_ON,
    PARAM_CHORUS_TYPE,
    PARAM_CHORUS_DEPTH,
    PARAM_CHORUS_SPEED,
    PARAM_CHORUS_LEV,
    PARAM_CHORUS_VOICES,
    PARAM_CHORUS_ON,
    PARAM_CHANNEL_PRESSURE,
    PARAM_GAIN,
    PARAM_FINETUNING,

    // Runtime data (not related to synth engine)
    PARAM_CURRENT_CHANNEL,    // Currently selected MIDI channel on UI side
    PARAM_UPDATE_TRIGGER,     // This parameter notify the host we've changed parameters.
                              // (since some hosts like REAPER won't save changes when you set a state on plugin UI)
                              // @see MeloryUI::_triggerHostUpdate(), MeloryPlugin::setParameter()

    // Current active instrument ID per channel (output parameters)
    // They show the actual instruments on the synth side, since MIDI CCs can change instruments at real-time
    PARAM_ACTIVE_INST_ID_CHANNEL_0,
    PARAM_ACTIVE_INST_ID_CHANNEL_1,
    PARAM_ACTIVE_INST_ID_CHANNEL_2,
    PARAM_ACTIVE_INST_ID_CHANNEL_3,
    PARAM_ACTIVE_INST_ID_CHANNEL_4,
    PARAM_ACTIVE_INST_ID_CHANNEL_5,
    PARAM_ACTIVE_INST_ID_CHANNEL_6,
    PARAM_ACTIVE_INST_ID_CHANNEL_7,
    PARAM_ACTIVE_INST_ID_CHANNEL_8,
    PARAM_ACTIVE_INST_ID_CHANNEL_9,
    PARAM_ACTIVE_INST_ID_CHANNEL_10,
    PARAM_ACTIVE_INST_ID_CHANNEL_11,
    PARAM_ACTIVE_INST_ID_CHANNEL_12,
    PARAM_ACTIVE_INST_ID_CHANNEL_13,
    PARAM_ACTIVE_INST_ID_CHANNEL_14,
    PARAM_ACTIVE_INST_ID_CHANNEL_15,

    // Total number of parameters
    PARAM_COUNT
};

//-------------------------------------------------------------------
// States

constexpr int         STATE_COUNT = 2;
constexpr const char* STATE_SOUNDFONT_PATH = "soundfont_path";
constexpr const char* STATE_CHANNEL_INSTRUMENT_LIST = "channel_instrument_list";    // Comma-separated list of instrument IDs per channel

//-------------------------------------------------------------------
// Global constants

constexpr float       MAX_POSSIBLE_INST_ID = 1023.0f;    // Maximum possible instrument ID (for parameter range)
