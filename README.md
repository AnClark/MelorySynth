# Melory SF2 Synthesizer

## Description

Melory SF2 Synthesizer (Melory) is a cross-platform virtual instrument, allowing you to load and play Soundfont (SF2) banks. It is based on [FluidSynth](https://www.fluidsynth.org/), the powerful Soundfont synthesizer.

Currently Melory is under active development. More features are on the way here.

## Features

- Supported platforms: Windows, Linux, macOS (including Apple Silicon)
- Supported plugin formats: VST 2.4, VST3, LV2, CLAP
- Supported Soundfont specs: currently SF2 only

## How to Build

### Clone this repository first

```bash
git clone https://github.com/AnClark/MelorySynth.git --recursive
```

In case you have some submodules not fetched properly, run:

```bash
cd MelorySynth
git submodule update --init --recursive
```

> **NOTICE:** You can skip FluidSynth's submodules (SF2-related repos), as they are not relevant to FluidSynth's main components.

### On Linux

#### 1. Install the following prerequisites

- Compiler: GCC
- Build system: CMake
- Builder tool: Ninja (recommended)
- Dependencies: glib2 (may already be installed on your distro)

For example, on Arch Linux:

```bash
sudo pacman -S gcc cmake ninja glib2
```

#### 2. Build

```bash
cd MelorySynth
cmake -S . -B build -GNinja    # Prefer Ninja for better performance. Omit "-GNinja" if you prefer GNU Make.
cmake --build build
```

**Built plugins reside in `build/bin/`.**

### On Windows

Msys2 is required for native build. **UCRT64 environment is recommended on modern Windows.**

#### 1. Install prerequisites in Msys2

```bash
# In Msys2 UCRT64 shell
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-glib2
```

#### 2. Build on Windows

```bash
# In Msys2 UCRT64 shell
cd MelorySynth
cmake -S . -B build -GNinja
cmake --build build --parallel $(nproc)
```

**Built plugins reside in `build/bin/`.**

All those dependency libs are statically linked.

### On Linux, cross-compile for Windows

Melory provided a special CMake toolchain file: `windows-cross-build.cmake`, so you can cross-compile Windows plugins on Linux. This is useful as it's much faster than native Msys2 (on Windows 11).

#### 1. Install prerequisites for cross-compilation

- Compiler: MinGW-w64 cross-compiler
- Build system: CMake
- Builder tool: Ninja (recommended)

Glib2 and its dependencies require their Windows builds. If you are using Arch Linux, you can install via AUR helper:

```bash
yay -S mingw-w64-glib2 --noconfirm
```

On other distros, you may need to manually cross-compile Glib2 and its dependencies: pcre2, libiconv, libintl.

#### 2. Build on Linux

```bash
cd MelorySynth
cmake -S . -B build-win32 -GNinja -DCMAKE_TOOLCHAIN_FILE=windows-cross-build.cmake
cmake --build build-win32
```

**Built plugins reside in `build/bin/`.**

All dependency libs are statically linked as well. You can directly test them on [Wine-powered](https://www.winehq.org) hosts (e.g. [REAPER for Windows](https://reaper.fm)).

## License

Melory is licensed under **GPLv3**.
