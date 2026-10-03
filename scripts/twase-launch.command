#!/bin/sh
# Starts Total War: ATTILA with TWASE injected. Steam must be running.
# Lives in the game root, next to "Total War ATTILA.app" and the TWASE folder.

root="$(cd "$(dirname "$0")" && pwd)"
game="$root/Total War ATTILA.app/Contents/MacOS/Total War ATTILA"
dylib="$root/TWASE/libTWASE.dylib"

if [ ! -x "$game" ]; then
    echo "Total War ATTILA not found at: $game" >&2
    exit 1
fi

if [ ! -f "$dylib" ]; then
    echo "TWASE not found at: $dylib" >&2
    exit 1
fi

cd "$root" || exit 1
DYLD_INSERT_LIBRARIES="$dylib" exec "$game" "$@"
