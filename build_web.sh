#!/bin/sh
# Build The Endless Night web package for itch.io using js-dos.
#
# Requires: endless.exe built first (run ./dosbuild.sh). The js-dos runtime is
#           packed in stuff/js-dos-8.4.1.zip and unpacked into the staging
#           directory at build time.
# Output:   endless-web.zip  (upload to itch.io as an HTML game)
#
# The package contains:
#   index.html    js-dos player page
#   files.json    DOS file manifest used by index.html
#   assets/       game assets + js-dos runtime unpacked from the zip above
#   CWSDPMI.EXE   dosbox.conf   endless.exe   README.md
#
# All staging happens in a temporary directory that is removed on exit.

set -e
cd "$(dirname "$0")"

OUT="$(pwd)/endless-web.zip"
STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT INT TERM

for f in index.html CWSDPMI.EXE dosbox.conf endless.exe README.md; do
    if [ ! -f "$f" ]; then
        echo "ERROR: $f not found."
        if [ "$f" = "endless.exe" ]; then
            echo "Run ./dosbuild.sh first to build endless.exe."
        fi
        exit 1
    fi
done

if [ ! -f "stuff/js-dos-8.4.1.zip" ]; then
    echo "ERROR: stuff/js-dos-8.4.1.zip not found."
    exit 1
fi

echo "Staging web package..."
mkdir -p "$STAGE/assets"
cp -R assets/. "$STAGE/assets/"
cp index.html CWSDPMI.EXE dosbox.conf endless.exe README.md "$STAGE/"

# Unpack the js-dos runtime from stuff/js-dos-8.4.1.zip into the staging assets.
echo "Unpacking js-dos runtime..."
mkdir -p "$STAGE/js-dos-unpack"
unzip -q "stuff/js-dos-8.4.1.zip" -d "$STAGE/js-dos-unpack"
mkdir -p "$STAGE/assets/js-dos"
cp -R "$STAGE/js-dos-unpack/dist/." "$STAGE/assets/js-dos/"
rm -f "$STAGE/assets/js-dos/index.html"
rm -rf "$STAGE/js-dos-unpack"

# Regenerate the DOS file manifest from the staged files (js-dos runtime excluded).
{
    find assets -type f ! -path 'assets/js-dos/*'
    printf '%s\n' CWSDPMI.EXE endless.exe
} | sort | awk 'BEGIN { print "[" } { if (++n > 1) print ","; printf "  \"%s\"", $0 } END { print "\n]" }' > "$STAGE/files.json"

echo "Packaging $OUT..."
rm -f "$OUT"
( cd "$STAGE" && zip -9 -r "$OUT" . >/dev/null )

echo "Done: $OUT"
echo "Upload it to itch.io as an HTML project and tick \"This file will be played in the browser\"."
