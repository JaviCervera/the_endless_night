@echo off
REM Build The Endless Night for DOS (486DX2) using DJGPP cross-compiler and Allegro 4.
REM
REM Requires: DJGPP cross-compiler extracted to C:\djgpp  (override with DJGPP_ROOT)
REM Output:   endless.exe  (run with CWSDPMI.EXE present in the same directory)
REM
REM Flags:
REM   -march=i486            target 486 instruction set (safe on 486DX2 and newer)
REM   -O2 -funroll-loops     release optimisations
REM   -ffast-math            faster FP (safe for a game; 486DX2 has built-in FPU)
REM   -fomit-frame-pointer   frees an extra register in release mode
REM   -fno-exceptions        no C++ exception tables (saves size/speed on DOS)
REM   -fno-rtti              no RTTI tables (not used; saves size)
REM   -s                     strip debug symbols from the final .exe

setlocal
cd /d "%~dp0"

if "%DJGPP_ROOT%"=="" set DJGPP_ROOT=C:\djgpp

set DJGPP_BIN=%DJGPP_ROOT%\bin
set CXX=%DJGPP_BIN%\i586-pc-msdosdjgpp-g++.exe

if not exist "%CXX%" (
    echo ERROR: DJGPP compiler not found at %CXX%
    echo.
    echo Please extract djgpp-mingw-gcc1220.zip to C:\djgpp,
    echo or set DJGPP_ROOT to your installation directory.
    exit /b 1
)

REM DJGPP needs DJDIR pointing to its root to locate its own libraries/headers
set DJDIR=%DJGPP_ROOT%

set ALLEGRO_DIR=.\lib\allegro

set SOURCES=src/main.cpp src/engine/pal.cpp src/engine/pixmap.cpp src/engine/raycaster.cpp src/engine/screen.cpp src/engine/texts.cpp

set CXXFLAGS=-std=gnu++17 -march=i486 -O2 -funroll-loops -ffast-math -fomit-frame-pointer
set CXXFLAGS=%CXXFLAGS% -fno-exceptions -fno-rtti
set CXXFLAGS=%CXXFLAGS% -I%ALLEGRO_DIR%/include

set LDFLAGS=-L%ALLEGRO_DIR% -lalleg -s

echo Building endless.exe for DOS/486...
"%CXX%" %CXXFLAGS% %SOURCES% -o endless.exe %LDFLAGS% 2>&1
if errorlevel 1 (
    echo.
    echo Build failed!
    exit /b 1
)

echo Done: endless.exe
echo.
echo Launching DOSBox...
dosbox -conf dosbox.conf
