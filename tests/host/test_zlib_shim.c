/* Unit test of amiga/tgl_zlib.c, the game's zlib calls through z.library,
 * built against tests/host/mock (OpenLibrary counted, z.library's calls
 * done by the host zlib):
 *  - without z.library every call is Z_VERSION_ERROR and nothing is left open;
 *  - with it, the library is opened once, at the first call, for version 53;
 *  - a deflate and inflate round trip through the shim gives the data back;
 *  - at exit the library is closed (checked by the atexit handler below,
 *    registered before the shim's, so it runs after it).
 * GPL-3.0-or-later, like Transball GL. Copyright (c) 2026 AmiCradle Team. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

struct Library { int open_count; };
int tgl_deflateInit(z_stream *, int);
int tgl_deflate(z_stream *, int);
int tgl_deflateEnd(z_stream *);
int tgl_inflateInit(z_stream *);
int tgl_inflate(z_stream *, int);
int tgl_inflateEnd(z_stream *);
extern struct Library *ZBase;

static struct Library zlib_base;
static int library_present, opens, closes, last_version;
static const char *last_name;

struct Library *OpenLibrary(const char *name, unsigned long version)
{
    last_name = name; last_version = (int)version;
    if (!library_present) return NULL;
    opens++;
    return &zlib_base;
}
void CloseLibrary(struct Library *lib) { if (lib == &zlib_base) closes++; }

static int failures;
#define CHECK(c) do { if (!(c)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); failures++; } } while (0)

static void after_shim_exit(void)
{
    if (closes != 1 || ZBase != NULL) { printf("FAIL: z.library not closed at exit (%d)\n", closes); _exit(1); }
    printf("zlib shim: closed at exit\n");
}

int main(void)
{
    z_stream s;
    static unsigned char in[100000], packed[120000], out[100000];
    unsigned i;

    atexit(after_shim_exit);

    /* no z.library */
    memset(&s, 0, sizeof s);
    CHECK(tgl_deflateInit(&s, Z_DEFAULT_COMPRESSION) == Z_VERSION_ERROR);
    CHECK(tgl_inflateInit(&s) == Z_VERSION_ERROR);
    CHECK(tgl_inflate(&s, Z_NO_FLUSH) == Z_VERSION_ERROR);
    CHECK(opens == 0 && ZBase == NULL);
    CHECK(last_name && strcmp(last_name, "z.library") == 0 && last_version == 53);

    /* z.library there: a round trip */
    library_present = 1;
    for (i = 0; i < sizeof in; i++) in[i] = (unsigned char)((i * 7) ^ (i >> 5));
    memset(&s, 0, sizeof s);
    CHECK(tgl_deflateInit(&s, Z_DEFAULT_COMPRESSION) == Z_OK);
    s.next_in = in; s.avail_in = sizeof in;
    s.next_out = packed; s.avail_out = sizeof packed;
    CHECK(tgl_deflate(&s, Z_FINISH) == Z_STREAM_END);
    unsigned packed_len = (unsigned)(sizeof packed - s.avail_out);
    CHECK(tgl_deflateEnd(&s) == Z_OK);

    memset(&s, 0, sizeof s);
    CHECK(tgl_inflateInit(&s) == Z_OK);
    s.next_in = packed; s.avail_in = packed_len;
    s.next_out = out; s.avail_out = sizeof out;
    CHECK(tgl_inflate(&s, Z_NO_FLUSH) == Z_STREAM_END);
    CHECK(s.total_out == sizeof in && memcmp(in, out, sizeof in) == 0);
    CHECK(tgl_inflateEnd(&s) == Z_OK);
    CHECK(opens == 1 && ZBase == &zlib_base && closes == 0);

    if (failures) { printf("zlib shim: %d failures\n", failures); _exit(1); }
    printf("zlib shim: %u bytes -> %u and back, z.library opened once\n", (unsigned)sizeof in, packed_len);
    return 0;
}
