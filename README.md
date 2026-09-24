# The Endless Night

![Cover](stuff/cover.png)

An alien horror adventure in glorious 3D - Every night is the night of reckoning! Only for MS-DOS

## Installing DJGPP on Windows 11 and compiling

Download: https://github.com/andrewwutw/build-djgpp/releases/download/v3.4/djgpp-mingw-gcc1220.zip

1. Extract the package
  * Expand-Archive djgpp-mingw-gcc1220.zip -DestinationPath C:\djgpp
  * Or right-click the ZIP → "Extract All" → type `C:\djgpp` as the destination.
  * You should end up with `C:\djgpp\bin\i586-pc-msdosdjgpp-g++.exe`.
2. Add DJGPP to your PATH (optional but convenient)
  * Open Settings → System → About → Advanced system settings → Environment Variables.
  * Under System variables, edit `Path` and add: `C:\djgpp\bin`
3. Set `DJDIR` (required by the linker)
  * In Environment Variables create a new System variable:
    - Variable name: `DJDIR`
    - Variable value: `C:\djgpp`
  * `dosbuild.bat` sets `DJDIR` automatically for its own session, so this is only necessary if you want to call the DJGPP tools directly from a terminal.
4. Build for DOS
  * Run `dosbuild.bat`
   This produces `endless.exe` and runs it in DOSBox (which must have been previously installed).

![Poster](stuff/poster.png)

## Premise

A shimmering alien mothership lands in a field near a quiet village just after sunset. Every few minutes, an eerie pulse of light resets the world to the same moment of arrival. Objects you move, damage, or collect persist between loops. Your mission: Find or build the 3 generators needed to send an electrical pulse to the alien mothership and destroy it, ending the cycle.

The executable file ENDLESS.EXE requires MS-DOS 6.22, a 486DX2 CPU at 66 MHz and 16 MB of RAM. A DOSBox-based "The Endless Night.exe" executable is provided for Windows.

CONTROLS

* Up / Down: Walk.
* Left / Right: Turn.
* Space: Execute action / select.
* Esc: Pause.

INFORMATION

The game has been written in C++17 and Allegro 4, compiled under DJGPP. Graphics have been created with ChatGPT. Ambient sounds and FX taken from the sound library of DIV Games Studio.
