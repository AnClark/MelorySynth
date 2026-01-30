# SPDX-License-Identifier: MIT
#
# CMakeLists.txt - CMake build script for Fluida-XT
# Copyright (C) 2024 AnClark Liu <anclarkliu@outlook.com>
# 

cmake_minimum_required (VERSION 3.20)

project (MelorySynth
    VERSION 1.0
    LANGUAGES C CXX
)

#
# Global parameters
#

# Essential for linking against static libraries
if (NOT MSVC)
    add_compile_options (-fPIC)
endif ()

# HACK: Link against static dependency libraries on Win32 (e.g. Glib2, pcre, etc.)
if (WIN32)
    message (WARNING "HACK: You are building on Windows - prefer static dependencies linking")
    # (1/2) Prefer static libraries
    set (CMAKE_FIND_LIBRARY_SUFFIXES .a .lib ${CMAKE_FIND_LIBRARY_SUFFIXES})
    # (2/2) Tell Glib2 (FluidSynth dependency) to prefer static libraries
    add_compile_definitions (GLIB_STATIC_COMPILATION=1)
endif ()

#
# Configure and build FluidSynth
#

# Turn off features unrelevant to plugin development
set (BUILD_SHARED_LIBS OFF)
set (enable-alsa OFF)
set (enable-aufile OFF)
set (enable-dbus OFF)
set (enable-dsound OFF)
set (enable-ipv6 OFF)
set (enable-jack OFF)
set (enable-ladspa OFF)
set (enable-network OFF)
set (enable-oss OFF)
set (enable-pipewire OFF)
set (enable-pulseaudio OFF)
set (enable-readline OFF)
set (enable-sdl3 OFF)
set (enable-systemd OFF)
set (enable-wasapi OFF)
set (enable-waveout OFF)
set (enable-winmidi OFF)
set (enable-libsndfile OFF) # TODO: Need this if we want to support SF3

# Use floats for sample data
# DPF uses floats for audio processing, not doubles
set (enable-floats ON)

# Now include and build FluidSynth
add_subdirectory (deps/fluidsynth)

#
# Build Plugin
#

add_subdirectory (deps/dpf)

dpf_add_plugin (${PROJECT_NAME}
    TARGETS vst2 vst3 lv2 clap jack
    FILES_DSP
        src/Plugin.cpp  # Plugin interface
        src/XSynth.cpp  # Fluida.lv2 FluidSynth APIs (by Hermann Meyer)
    FILES_UI
        src/UI.cpp
        deps/dpf-widgets/opengl/DearImGui.cpp
    USE_FILE_BROWSER
)

target_include_directories (${PROJECT_NAME} PUBLIC
    src
    deps/dpf-widgets/generic
    deps/dpf-widgets/opengl
    deps/nfd/src/include
)

# Both DSP and UI need FluidSynth, since I implement soundfont parsing in UI
# (because DSP does not support sending custom messages to UI yet)
target_link_libraries (${PROJECT_NAME} PUBLIC libfluidsynth) 

# HACK: Link libiconv statically on Windows (required by libintl, a dependency of Glib2)
# NOTICE: This must be done after linking FluidSynth due to GNU linker behavior
if (WIN32)
    find_package (Iconv REQUIRED)    # Check if iconv is available
    target_link_libraries (${PROJECT_NAME} PUBLIC iconv)
endif ()
