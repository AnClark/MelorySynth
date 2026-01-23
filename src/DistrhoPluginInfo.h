// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * DistrhoPluginInfo.h - DPF metadata definitions
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

#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_BRAND    "AnClark Liu"
#define DISTRHO_PLUGIN_NAME     "Melory SF2 Synthesizer"
#define DISTRHO_PLUGIN_URI      "https://github.com/anclark/fluida-xt"
#define DISTRHO_PLUGIN_CLAP_ID  "anclark.melory.synth"

#define DISTRHO_PLUGIN_IS_SYNTH 1

// To let UI know the changes of parameters on real-time, you must set this flag.
#define DISTRHO_PLUGIN_IS_RT_SAFE       1

#define DISTRHO_PLUGIN_NUM_INPUTS       0
#define DISTRHO_PLUGIN_NUM_OUTPUTS      2
#define DISTRHO_PLUGIN_WANT_PROGRAMS    0
#define DISTRHO_PLUGIN_WANT_STATE       1
#define DISTRHO_PLUGIN_WANT_FULL_STATE  1
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT  1
#define DISTRHO_PLUGIN_WANT_MIDI_OUTPUT 0

#define DISTRHO_PLUGIN_HAS_UI           1
#define DISTRHO_UI_USE_CUSTOM           1
#define DISTRHO_UI_CUSTOM_INCLUDE_PATH  "DearImGui.hpp"
#define DISTRHO_UI_CUSTOM_WIDGET_TYPE   DGL_NAMESPACE::ImGuiTopLevelWidget
#define DISTRHO_UI_DEFAULT_WIDTH        600
#define DISTRHO_UI_DEFAULT_HEIGHT       600
#define DISTRHO_UI_FILE_BROWSER         1    // Enable DPF built-in file browser for soundfont selection

#endif    // DISTRHO_PLUGIN_INFO_H_INCLUDED
