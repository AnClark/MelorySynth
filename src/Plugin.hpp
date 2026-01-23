// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * Plugin.hpp - Plugin interface for Melory synthesizer (header file)
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

#include "DistrhoPlugin.hpp"
#include "XSynth.h"

#include "Definitions.hpp"

using DISTRHO::Plugin;

class MeloryPlugin : public Plugin
{
public:
    MeloryPlugin();
    ~MeloryPlugin() override;

protected:
    //-------------------------------------------------------------------
    // Information

    const char* getLabel() const override
    {
        return DISTRHO_PLUGIN_NAME;
    }

    const char* getDescription() const override
    {
        return "Next-generation Soundfont audio plugin based on FluidSynth";
    }

    const char* getMaker() const override
    {
        return DISTRHO_PLUGIN_BRAND;
    }

    const char* getHomePage() const override
    {
        return DISTRHO_PLUGIN_URI;
    }

    const char* getLicense() const override
    {
        return "GPLv3";
    }

    uint32_t getVersion() const override
    {
        return d_version(1, 0, 0);
    }

    int64_t getUniqueId() const override
    {
        return d_cconst('M', 'L', 'Y', '1');
    }

    //-------------------------------------------------------------------
    // Init

    void   initParameter(uint32_t index, Parameter& parameter) override;
    void   initState(uint32_t index, State& state) override;

    //-------------------------------------------------------------------
    // Internal data

    float  getParameterValue(uint32_t index) const override;
    void   setParameterValue(uint32_t index, float value) override;

    String getState(const char* key) const override;
    void   setState(const char* key, const char* value) override;

    //-------------------------------------------------------------------
    // Process

    void   run(const float** inputs, float** outputs, uint32_t frames,
          const MidiEvent* midiEvents, uint32_t midiEventCount) override;

    //--------------------------------------------------------------------
    // Callbacks

    void   sampleRateChanged(double newSampleRate) override
    {
        // TODO: Test stability on Win32!
        d_stderr2("Sample rate changed: %f -> %f", getSampleRate(), newSampleRate);
        fSynth.change_sample_rate(static_cast<unsigned int>(newSampleRate));
    }

    //-------------------------------------------------------------------

private:
    //--------------------------------------------------------------------
    // Variables and Instances

    float                fParams[PARAM_COUNT];    // Local copy of parameter values
    String               fSoundfontPath;          // Path to the loaded soundfont

    float&               fCurrentChannel;            // Current active MIDI channel on UI side (if I design the UI as BassMidiVSTi does)
    [[maybe_unused]] int fCurrentInstrument;         // Current instrument ID (current active channel)
    int                  fChannelInstruments[16];    // Current instrument IDs per channel

    xsynth::XSynth       fSynth;    // Fluidsynth instance

    String               fInstrumentList;    // Cached instrument list string

    //-------------------------------------------------------------------
    // Utilities

    // Load soundfont in the audio thread (blocking)
    void                 _loadSoundfontSync();

    // Export channel instrument list as a comma-separated string
    String               _exportChannelInstrumentList() const;

    // Apply channel instrument list from a comma-separated string
    void                 _applyChannelInstrumentList(const char* value);

    // Apply all channel instrument settings to the synth
    void                 _setAllChannelInstruments();

    // Update active instrument ID parameter for a channel
    void                 _updateInstIdParam(uint32_t channel);

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeloryPlugin)
};
