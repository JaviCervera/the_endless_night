#!/bin/sh
# Build The Endless Night for DOS (486DX2) using DJGPP cross-compiler and Allegro 4.
#
# Requires: DJGPP cross-compiler at /opt/djgpp/bin
# Output:   endless.exe  (run with CWSDPMI.EXE present in the same directory)
#
# Flags:
#   -march=i486            target 486 instruction set (safe on 486DX2 and newer)
#   -O2 -funroll-loops     release optimisations
#   -ffast-math            faster FP (safe for a game; 486DX2 has built-in FPU)
#   -fomit-frame-pointer   frees an extra register in release mode
#   -fno-exceptions        no C++ exception tables (saves size/speed on DOS)
#   -fno-rtti              no RTTI tables (not used; saves size)
#   -s                     strip debug symbols from the final .exe

set -e
cd "$(dirname "$0")"

DJGPP_BIN=/opt/djgpp/bin
CXX="${DJGPP_BIN}/i586-pc-msdosdjgpp-g++"
ALLEGRO_DIR=./lib/allegro

SOURCES="src/main.cpp src/engine/pal.cpp src/engine/pixmap.cpp src/engine/raycaster.cpp src/engine/screen.cpp"

CXXFLAGS="-std=gnu++17 -march=i486 -O2 -funroll-loops -ffast-math -fomit-frame-pointer"
CXXFLAGS="${CXXFLAGS} -fno-exceptions -fno-rtti"
CXXFLAGS="${CXXFLAGS} -I${ALLEGRO_DIR}/include"

LDFLAGS="-L${ALLEGRO_DIR} -lalleg -s"

echo "Building endless.exe for DOS/486..."
$CXX $CXXFLAGS $SOURCES -o endless.exe $LDFLAGS
echo "Done: endless.exe"
echo ""
echo "Launching DOSBox..."
dosbox -conf dosbox.conf
