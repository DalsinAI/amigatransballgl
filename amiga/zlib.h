/*
 * Transball GL on GFOS: the zlib calls the game makes (replays are stored
 * deflated), done by z.library (GFOpenZlib) instead of a compiled-in zlib.
 * amiga/tgl_zlib.c opens the library when first needed.
 * GPL-3.0-or-later, like Transball GL. Copyright (c) 2026 AmiCradle Team.
 */
#ifndef TGL_AMIGA_ZLIB_H
#define TGL_AMIGA_ZLIB_H

/* TGL_HOST_ZLIB: the host tests build the Amiga paths with the host zlib */
#if defined(__amigaos__) && !defined(TGL_HOST_ZLIB)
#include <libraries/z.h>       /* z_stream, Z_OK, Z_FINISH, ... */
#ifndef Z_NULL
#define Z_NULL 0
#endif
#else
#include_next <zlib.h>         /* host tests: the real zlib */
#endif

#if defined(__amigaos__) && !defined(TGL_HOST_ZLIB)
#ifdef __cplusplus
extern "C" {
#endif
int tgl_deflateInit(z_stream *strm, int level);
int tgl_deflate(z_stream *strm, int flush);
int tgl_deflateEnd(z_stream *strm);
int tgl_inflateInit(z_stream *strm);
int tgl_inflate(z_stream *strm, int flush);
int tgl_inflateEnd(z_stream *strm);
#ifdef __cplusplus
}
#endif
#undef deflateInit
#undef inflateInit
#define deflateInit tgl_deflateInit
#define deflate     tgl_deflate
#define deflateEnd  tgl_deflateEnd
#define inflateInit tgl_inflateInit
#define inflate     tgl_inflate
#define inflateEnd  tgl_inflateEnd
#endif

#endif
