@echo off
REM Build The Endless Night web package for itch.io using js-dos.
REM
REM Requires: endless.exe built first (run dosbuild.bat). The js-dos runtime is
REM           packed in stuff\js-dos-8.4.1.zip and unpacked into the staging
REM           directory at build time.
REM Output:   endless-web.zip  (upload to itch.io as an HTML game)
REM
REM The package contains:
REM   index.html    js-dos player page
REM   files.json    DOS file manifest used by index.html
REM   assets\       game assets + js-dos runtime unpacked from the zip above
REM   CWSDPMI.EXE   dosbox.conf   endless.exe   README.md
REM
REM All staging happens in a temporary directory that is removed on exit.

setlocal
cd /d "%~dp0"

set OUT=%CD%\endless-web.zip
set STAGE=%TEMP%\endless-web-build-%RANDOM%%RANDOM%

for %%F in (index.html CWSDPMI.EXE dosbox.conf endless.exe README.md) do (
    if not exist "%%F" (
        echo ERROR: %%F not found.
        if "%%F"=="endless.exe" echo Run dosbuild.bat first to build endless.exe.
        exit /b 1
    )
)

if not exist "stuff\js-dos-8.4.1.zip" (
    echo ERROR: stuff\js-dos-8.4.1.zip not found.
    exit /b 1
)

echo Staging web package in %STAGE%...
mkdir "%STAGE%" || goto :fail

xcopy /E /I /Y /Q assets "%STAGE%\assets" >nul || goto :fail
copy /Y index.html "%STAGE%" >nul || goto :fail
copy /Y CWSDPMI.EXE "%STAGE%" >nul || goto :fail
copy /Y dosbox.conf "%STAGE%" >nul || goto :fail
copy /Y endless.exe "%STAGE%" >nul || goto :fail
copy /Y README.md "%STAGE%" >nul || goto :fail

REM Unpack the js-dos runtime from stuff\js-dos-8.4.1.zip into the staging assets.
echo Unpacking js-dos runtime...
powershell -NoProfile -ExecutionPolicy Bypass -Command "Add-Type -AssemblyName System.IO.Compression.FileSystem; $unpack=Join-Path '%STAGE%' 'js-dos-unpack'; Expand-Archive -LiteralPath 'stuff\js-dos-8.4.1.zip' -DestinationPath $unpack -Force; $dest=Join-Path '%STAGE%' 'assets\js-dos'; New-Item -ItemType Directory -Force -Path $dest | Out-Null; Copy-Item -Path (Join-Path $unpack 'dist\*') -Destination $dest -Recurse -Force; Remove-Item -Force (Join-Path $dest 'index.html'); Remove-Item -Recurse -Force $unpack" || goto :fail

REM Regenerate the DOS file manifest from the staged files (js-dos runtime excluded).
powershell -NoProfile -ExecutionPolicy Bypass -Command "$stage='%STAGE%'; $files=@(Get-ChildItem -Recurse -File (Join-Path $stage 'assets') | Where-Object { $_.FullName -notmatch '\\js-dos\\' } | ForEach-Object { $_.FullName.Substring($stage.Length+1).Replace('\','/') }); $files+='CWSDPMI.EXE','endless.exe'; $files=$files|Sort-Object; [IO.File]::WriteAllText((Join-Path $stage 'files.json'), (ConvertTo-Json $files))" || goto :fail

if exist "%OUT%" del /Q "%OUT%"
echo Packaging %OUT%...
REM Entries are written by hand because ZipFile::CreateFromDirectory uses
REM backslash separators on .NET Framework, which some unzippers reject.
powershell -NoProfile -ExecutionPolicy Bypass -Command "Add-Type -AssemblyName System.IO.Compression.FileSystem; $stage=(Resolve-Path '%STAGE%').Path.TrimEnd('\')+'\'; $zip=[System.IO.Compression.ZipFile]::Open('%OUT%', 'Create'); try { Get-ChildItem -Recurse -File '%STAGE%' | ForEach-Object { [System.IO.Compression.ZipFileExtensions]::CreateEntryFromFile($zip, $_.FullName, $_.FullName.Substring($stage.Length).Replace('\','/'), [System.IO.Compression.CompressionLevel]::Optimal) } | Out-Null } finally { $zip.Dispose() }" || goto :fail

if not exist "%OUT%" goto :fail

rmdir /s /q "%STAGE%" 2>nul

echo.
echo Done: %OUT%
echo Upload it to itch.io as an HTML project and tick "This file will be played in the browser".
exit /b 0

:fail
echo.
echo Build failed!
rmdir /s /q "%STAGE%" 2>nul
exit /b 1
