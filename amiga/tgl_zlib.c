/*
 * Transball GL on GFOS: zlib's stream calls through z.library (amiga/zlib.h).
 * The library is opened on first use and closed at exit; without it a
 * call fails with Z_VERSION_ERROR, and the game shows a replay as unreadable.
 * GPL-3.0-or-later, like Transball GL. Copyright (c) 2026 AmiCradle Team.
 */
#include <stdlib.h>
#include <exec/types.h>
#include <proto/exec.h>
#include <libraries/z.h>
#include <proto/z.h>

struct Library *ZBase;

static void close_z(void)
{
    if (ZBase) { CloseLibrary(ZBase); ZBase = NULL; }
}

static int open_z(void)
{
    if (!ZBase) {
        ZBase = OpenLibrary((CONST_STRPTR)"z.library", 53);
        if (!ZBase) return 0;
        atexit(close_z);
    }
    return 1;
}

int tgl_deflateInit(z_stream *strm, int level) { return open_z() ? (int)DeflateInit(strm, level) : Z_VERSION_ERROR; }
int tgl_deflate(z_stream *strm, int flush)     { return open_z() ? (int)Deflate(strm, flush) : Z_VERSION_ERROR; }
int tgl_deflateEnd(z_stream *strm)             { return open_z() ? (int)DeflateEnd(strm) : Z_VERSION_ERROR; }
int tgl_inflateInit(z_stream *strm)            { return open_z() ? (int)InflateInit(strm) : Z_VERSION_ERROR; }
int tgl_inflate(z_stream *strm, int flush)     { return open_z() ? (int)Inflate(strm, flush) : Z_VERSION_ERROR; }
int tgl_inflateEnd(z_stream *strm)             { return open_z() ? (int)InflateEnd(strm) : Z_VERSION_ERROR; }
