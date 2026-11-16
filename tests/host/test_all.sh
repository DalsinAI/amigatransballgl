#!/bin/sh
# Host tests for the Transball GL port. TGL=<upstream checkout at the
# locked commit, unpatched>; the patch is applied to a copy.
#   TGL=... sh tests/host/test_all.sh
# With GLM=<glm checkout> and FONTS=<the lock's fonts> as well, and Xvfb and
# the SDL 2, Mesa and zlib development files on the host, it also builds the
# whole game with the Amiga's settings (GL 2.1 compatibility context, the
# OFL fonts, 16 KB packing chunks) and runs it on Xvfb: it must still be
# running after 10 s, with its first screen drawn (not all one colour).
# GPL-3.0-or-later, like Transball GL. Copyright (c) 2026 AmiCradle Team.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$HERE/../.." && pwd)
: "${TGL:?set TGL to an upstream checkout}"
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
cp -R "$TGL" "$T/tgl"
(cd "$T/tgl" && for p in "$ROOT"/amiga/patches/*.patch; do patch -s -p1 < "$p"; done)
echo "patches: apply"

# the fonts: on the Amiga only the two OFL ones the lock fetches, no others
. "$ROOT/package/transballgl.lock"
c="$T/tgl/src/TGLconstants.h"
for f in RussoOne-Regular.ttf Anton-Regular.ttf; do
    grep -q "fonts/$f" "$c" || { echo "FAIL: $f not in TGLconstants.h"; exit 1; }
    echo "$FONTS_FILES" | grep -q ":$f:" || { echo "FAIL: $f not in the lock"; exit 1; }
done
if grep -rn --include='*.cpp' 'fonts/[A-Za-z]*\.ttf' "$T/tgl/src" | tr -d '\r' | grep -v TGL_FONT; then
    echo "FAIL: a font named outside TGLconstants.h"; exit 1
fi
echo "fonts: titles Russo One, text Anton, both in the lock"
tr -d '\r' < "$T/tgl/src/main.cpp" | grep -A3 '__amigaos__' | grep -q 'SDL_GL_CONTEXT_PROFILE_COMPATIBILITY'
echo "GL: compatibility context asked for"

# amiga/tgl_zlib.c against stand-ins for exec and z.library
cc -Wall -Werror -I"$HERE/mock" "$HERE/test_zlib_shim.c" "$ROOT/amiga/tgl_zlib.c" -o "$T/zshim" -lz
"$T/zshim"

# the replays: the port's packing gives the bytes upstream's does
a="$T/tgl/src/auxiliar"; u="$TGL/src/auxiliar"
c++ -O1 -w -D__amigaos__ -DTGL_HOST_ZLIB -I"$ROOT/amiga" -I"$a" "$HERE/test_replays.cpp" "$a/compression.cpp" -o "$T/rp_amiga" -lz
c++ -O1 -w -I"$u" "$HERE/test_replays.cpp" "$u/compression.cpp" -o "$T/rp_up" -lz
"$T/rp_amiga" "$T/amiga.bin" "$TGL"/tutorials/*.rpl
"$T/rp_up" "$T/up.bin" "$TGL"/tutorials/*.rpl > /dev/null
cmp -s "$T/amiga.bin" "$T/up.bin" || { echo "FAIL: packed replays differ from upstream's"; exit 1; }
echo "replays: packed with 16 KB chunks, the same bytes as upstream's 256 KB"

if [ -n "${GLM:-}" ] && [ -n "${FONTS:-}" ] && command -v Xvfb > /dev/null && pkg-config --exists sdl2 SDL2_image SDL2_ttf SDL2_mixer gl zlib; then
    mkdir -p "$T/obj"
    flags="-O1 -w -std=gnu++11 -D__amigaos__ -DTGL_HOST_ZLIB -DGLM_FORCE_RADIANS -I$ROOT/amiga -I$GLM -I$GLM/glm $(pkg-config --cflags sdl2)"
    for d in src src/auxiliar src/interface src/objects src/states; do flags="$flags -I$T/tgl/$d"; done
    (cd "$T/tgl" && ls src/*.cpp src/*/*.cpp) | sed 's|/|_|g; s|\.cpp$||' > "$T/names"
    (cd "$T/tgl" && ls src/*.cpp src/*/*.cpp) | xargs -P "${JOBS:-2}" -I{} sh -c \
        'o=$(echo "{}" | sed "s|/|_|g; s|\.cpp$|.o|"); c++ '"$flags"' -c "'"$T"'/tgl/{}" -o "'"$T"'/obj/$o"'
    c++ -o "$T/TransballGL" "$T"/obj/*.o $(pkg-config --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer gl zlib) -lm
    D="$T/drawer"; mkdir -p "$D/fonts" "$D/players" "$D/other-players" "$D/replays"
    for d in graphics sound maps mymaps tutorials; do ln -s "$TGL/$d" "$D/$d"; done
    cp "$FONTS/RussoOne-Regular.ttf" "$FONTS/Anton-Regular.ttf" "$D/fonts/"
    n=97; while [ -e "/tmp/.X$n-lock" ]; do n=$((n + 1)); done
    Xvfb ":$n" -screen 0 1024x768x24 > /dev/null 2>&1 & X=$!
    sleep 2
    (cd "$D" && DISPLAY=":$n" SDL_AUDIODRIVER=dummy "$T/TransballGL" > "$T/play.txt" 2>&1) & G=$!
    sleep 10
    kill -0 "$G" || { cat "$T/play.txt"; kill "$X"; echo "FAIL: the game stopped"; exit 1; }
    colours=$(DISPLAY=":$n" import -window root -format '%k' info: 2>/dev/null || echo 0)
    kill "$G" "$X" 2> /dev/null || true
    [ "$colours" -gt 1 ] || { echo "FAIL: nothing drawn ($colours colours)"; exit 1; }
    echo "play: running after 10 s on Xvfb, first screen drawn ($colours colours)"
else
    echo "play: SKIP (set GLM and FONTS; needs Xvfb, ImageMagick, SDL 2, GL and zlib development files)"
fi
python3 -I "$HERE/test_lha.py"
echo PASS
