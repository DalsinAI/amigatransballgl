/* Host test stand-in for proto/exec.h: OpenLibrary and CloseLibrary,
 * counted by tests/host/test_zlib_shim.c. */
#ifndef MOCK_PROTO_EXEC_H
#define MOCK_PROTO_EXEC_H
#include <exec/types.h>
struct Library *OpenLibrary(CONST_STRPTR name, ULONG version);
void CloseLibrary(struct Library *lib);
#endif
