/* Host test stand-in for the NDK's exec/types.h (tests/host/test_zlib_shim.c). */
#ifndef MOCK_EXEC_TYPES_H
#define MOCK_EXEC_TYPES_H
typedef const char *CONST_STRPTR;
typedef unsigned long ULONG;
typedef long LONG;
struct Library { int open_count; };
#endif
