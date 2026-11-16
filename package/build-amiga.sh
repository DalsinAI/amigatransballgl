#!/bin/sh
# The release archive and the GFOpenUp part, for Cradle's stove_build
# (target package):
#  1. upstream Transball GL and GLM, fetched at the commits
#     package/transballgl.lock names (neither is kept in this repository),
#     with amiga/patches applied to the game, and the two OFL fonts that
#     stand in for upstream's own, checked against their sha256;
#  2. the game built by amiga/Makefile.amigaos;
#  3. the drawer, its icon and the Installer script beside it packed as
#     NAME-VERSION-68040.lha (tools/pack_dist.py), with its .sha256 and a
#     short .readme;
#  4. OUT/Parts/TransballGL, a GFOpenUp part: its Part file and the drawer's
#     files for SYS:Games/TransballGL, installed with
#       GFOpenUpTool INSTALL Parts/TransballGL
#     It needs the GFOpenGPU part, whose SDL2.module and GL draw the game.
# Nothing is published from here.
# Copyright (c) 2026 AmiCradle Team. GPL-3.0-or-later, like Transball GL.
set -eu

HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$HERE/.." && pwd)
. "$HERE/package.env"
. "$HERE/transballgl.lock"
OUT=${OUT:-"$ROOT/build-amiga-package"}
mkdir -p "$OUT"
OUT=$(CDPATH= cd -- "$OUT" && pwd)
STOVE=${STOVE:-"$HOME/AmiCradle/stoves/os32-gcc16-open6"}
# outside the checkout: Cradle cleans it (git clean -ffdx) before every job
WORK=${WORK:-"$HOME/.cache/amigatransballgl"}
cd "$ROOT"

fetch() { # dir url commit
    if [ "$(git -C "$1" rev-parse HEAD 2>/dev/null)" != "$3" ]; then
        rm -rf "$1"; mkdir -p "$1"
        git -C "$1" init -q
        git -C "$1" fetch -q --depth 1 "$2" "$3"
        git -C "$1" checkout -q FETCH_HEAD
    fi
    git -C "$1" checkout -q -- . && git -C "$1" clean -qfdx
}

# 1. upstream at the locked commits, patched afresh every time; the fonts
T="$WORK/transballgl"; G="$WORK/glm"; F="$WORK/fonts"
fetch "$T" "$TGL_URL" "$TGL_COMMIT"
fetch "$G" "$GLM_URL" "$GLM_COMMIT"
for p in "$ROOT"/amiga/patches/*.patch; do
    git -C "$T" apply "$p"
done
mkdir -p "$F"
echo "$FONTS_FILES" | while IFS=: read -r path name sum; do
    if [ "$(sha256sum "$F/$name" 2>/dev/null | cut -d' ' -f1)" != "$sum" ]; then
        curl -fsSL -o "$F/$name" "$FONTS_RAW/$FONTS_COMMIT/$path"
        [ "$(sha256sum "$F/$name" | cut -d' ' -f1)" = "$sum" ] || { echo "$name: sha256 differs from the lock" >&2; exit 1; }
    fi
done

# 2. the game (a stove's compiler is in its prefix/, or STOVE is the prefix)
if [ -x "$STOVE/prefix/bin/m68k-amigaos-gcc" ]; then PREFIX="$STOVE/prefix"; else PREFIX="$STOVE"; fi
make -f amiga/Makefile.amigaos TGL="$T" GLM="$G" FONTS="$F" STOVE="$STOVE" PREFIX="$PREFIX" -j4 $TARGET \
    > "$OUT/build-output.txt" 2>&1 || { tail -n 60 "$OUT/build-output.txt"; exit 1; }

# 3. the archive
python3 "$ROOT/tools/pack_dist.py" --dist "$DIST" --name "$NAME" --out "$OUT"

# 4. the GFOpenUp part
sh "$ROOT/package/write-part.sh" "$DIST/$NAME" "$OUT/Parts/$NAME" "$NAME" "$PORT_VERSION" \
    "Transball GL: fly a thrust ship through caverns to steal back the energy spheres"
echo "part: $(grep -c '^copy' "$OUT/Parts/$NAME/Part") files, $(du -sk "$OUT/Parts/$NAME" | cut -f1) KB"
ls -l "$DIST/$NAME/$NAME"
