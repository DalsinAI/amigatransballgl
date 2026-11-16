// Unit test of the game's replay packing (src/auxiliar/compression.cpp),
// built with the port's amiga/zlib.h and its 16 KB chunks (__amigaos__;
// TGL_HOST_ZLIB has the host's zlib do z.library's work):
//   test_replays OUT FILE.rpl...
// unpacks every replay given (each must be the game's XML), packs each
// again and writes all the packed bytes to OUT. tests/host/test_all.sh runs
// it built both ways, with upstream's 256 KB chunks too, and compares the
// two OUTs: the files the port writes are the ones the game on a PC writes.
// GPL-3.0-or-later, like Transball GL. Copyright (c) 2026 AmiCradle Team.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "compression.h"

int main(int argc, char **argv)
{
    if (argc < 3) { fprintf(stderr, "usage: test_replays OUT FILE.rpl...\n"); return 2; }
    FILE *out = fopen(argv[1], "wb");
    if (!out) return 2;
    unsigned long total = 0;
    for (int i = 2; i < argc; i++) {
        FILE *fp = fopen(argv[i], "rb");
        if (!fp) { printf("FAIL: %s: cannot open\n", argv[i]); return 1; }
        unsigned len = 0;
        char *xml = decompress(fp, &len);
        fclose(fp);
        if (!xml || len < 16 || xml[0] != '<' || !strstr(xml, "replay")) {
            printf("FAIL: %s: not a replay once unpacked (%u bytes)\n", argv[i], len);
            return 1;
        }
        if (!compress(xml, (int)len, out)) { printf("FAIL: %s: packing again\n", argv[i]); return 1; }
        total += len;
        delete[] xml;
    }
    fclose(out);
    printf("replays: %d unpacked (%lu bytes of XML) and packed again\n", argc - 2, total);
    return 0;
}
