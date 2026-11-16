/* Host test stand-in for z.library's proto/z.h: its calls, done by the host
 * zlib (z.library's are zlib's, with capitalised names). */
#ifndef MOCK_PROTO_Z_H
#define MOCK_PROTO_Z_H
#include <zlib.h>
extern struct Library *ZBase;
#define DeflateInit(s, l) deflateInit((s), (l))
#define Deflate(s, f)     deflate((s), (f))
#define DeflateEnd(s)     deflateEnd(s)
#define InflateInit(s)    inflateInit(s)
#define Inflate(s, f)     inflate((s), (f))
#define InflateEnd(s)     inflateEnd(s)
#endif
