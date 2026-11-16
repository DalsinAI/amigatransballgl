/* The Amiga port's start: the stack, the version string, and the drawer
 * as the current directory (the game opens graphics/, maps/, players/ and
 * the rest by relative paths), set before main().
 * GPL-3.0-or-later, like Transball GL. Copyright (c) 2026 AmiCradle Team. */
#include <unistd.h>

/* A Shell gives 4 KB of stack; SDL 2, GL and the game want more. libnix's
 * startup sets this much up. */
unsigned long __stack = 512 * 1024;
static const char version[] __attribute__((used)) =
    "$VER: TransballGL 1.0 (16.11.2026)";

static void __attribute__((constructor)) amiga_drawer_dir(void)
{
    chdir("PROGDIR:");
}
