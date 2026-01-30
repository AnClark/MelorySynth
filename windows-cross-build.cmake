#  amsynth windows-cross-build.cmake
#
#  Copyright (C) 2022 AnClark Liu
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

# Cross-build Windows edition on Linux / WSL.
# Useful for debugging with Wine. (Recommend REAPER for Windows)
#
# Usage:
#   cmake -S . -B build-win32 -DCMAKE_TOOLCHAIN_FILE=windows-cross-build.cmake
#   cmake --build build-win32

SET(CMAKE_SYSTEM_NAME Windows)
SET(CMAKE_SYSTEM_PROCESSOR x86_64)

SET(CMAKE_C_COMPILER /usr/bin/x86_64-w64-mingw32-gcc)
SET(CMAKE_CXX_COMPILER /usr/bin/x86_64-w64-mingw32-g++)
SET(CMAKE_RC_COMPILER /usr/bin/x86_64-w64-mingw32-windres)

# This PREFIX_PATH will be specified by -DPREFIX_PATH when invoking cmake
SET(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32/ ${PREFIX_PATH})

#
# The following settings ensure that CMake only searches for libraries and headers
# within the target environment.
# (Thanks Claude Sonnet 4.5 for hints!)
#

# (1/3) Essential: Limit CMake to only search within the target platform paths
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# (2/3) Configure pkg-config to use Windows .pc files
SET(ENV{PKG_CONFIG_LIBDIR} "/usr/x86_64-w64-mingw32/lib/pkgconfig:/usr/x86_64-w64-mingw32/share/pkgconfig")
SET(ENV{PKG_CONFIG_PATH} "")

# (3/3) Disable pkg-config's system paths to avoid mixing in Linux system libraries
SET(ENV{PKG_CONFIG_SYSTEM_INCLUDE_PATH} "/usr/x86_64-w64-mingw32/include")
SET(ENV{PKG_CONFIG_SYSTEM_LIBRARY_PATH} "/usr/x86_64-w64-mingw32/lib")
