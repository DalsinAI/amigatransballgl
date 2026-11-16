Transball GL for GFOS
=====================

A port of Transball GL, Santiago Ontanon Villar's cavern flying game
(https://github.com/santiontanon/transballgl, commit
066195e9e0d6a778a051f147f4c72ba76b9b7c46, version 1.0), for GFOS:
AmigaOS 3.2 with GFOpenUp, on a 68040 or 68060 with an FPU, on GFOpenUp's
SDL 2 and OpenGL (GFOpenGPU).

Fly a thrust ship down through the caverns, find the energy sphere, take
it on your tractor beam and carry it back to the top, past cannons, tanks,
lasers and doors, with gravity pulling and the fuel running down. Twelve
ships to unlock, three level packs, replays of your best runs, and a level
editor.

Status: built in the os32-gcc16-open6 stove (TransballGL 1,716,080 bytes,
SDL2_ttf, FreeType and GLM linked in); not yet run in a lab.


What you need
-------------

- GFOS: AmigaOS 3.2 with GFOpenUp 0.8.26 or later, which installs
  LIBS:GFOpenGPU/SDL2.module, SDL2_image.module and SDL2_mixer.module and
  OpenGL, and z.library (GFOpenZlib) in LIBS:, which packs the replays.
- A 68040 or 68060 with an FPU (not an LC040 or EC040), an RTG screen with
  room for a 640 x 480 window, about 48 MB of free memory and 22 MB of disk.
- Nothing else: the graphics, sounds, levels, tutorials and fonts are in
  the drawer.


How to play
-----------

Install it one of three ways:

- GFOpenUp: `GFOpenUpTool INSTALL Parts/TransballGL` puts it in
  SYS:Games/TransballGL.
- Run Install-TransballGL from the archive (it offers Work:Games).
- Or copy the TransballGL drawer where you want it.

Then double-click TransballGL. It opens in a 640 x 480 window; Options
gives it a screen of its own. Make a player profile first (type a name,
then Create). Q thrusts, O and P turn, Space fires, Return is the tractor
beam, Esc is the menu, F12 quits; every key can be changed in Options, and
the drawer's Controls file has them all. Profiles are kept in the drawer's
players drawer, replays in replays and your own levels in mymaps, which
Install-TransballGL and GFOpenUp keep when they update the game.


Building
--------

    sh package/build-amiga.sh        (STOVE=/path/to/os32-gcc16-open6)

or, in Cradle, stove_build with the os32-gcc16-open6 stove and target
package. It fetches upstream and GLM at the commits package/transballgl.lock
names and the two fonts (checked against their sha256), applies
amiga/patches, builds with amiga/Makefile.amigaos and packs
TransballGL-VERSION-68040.lha and the GFOpenUp part Parts/TransballGL.

Host tests: `TGL=<upstream checkout> sh tests/host/test_all.sh` applies the
patch; checks the game names only the lock's two OFL fonts and asks for a
compatibility GL context; runs amiga/tgl_zlib.c against stand-ins for exec
and z.library (no library: every call fails cleanly; with it: opened once,
for version 53, a deflate and inflate round trip, closed at exit); and
unpacks all 24 shipped replays and packs them again with the port's 16 KB
chunks, byte for byte the same as upstream's 256 KB ones. With GLM= and
FONTS= set, and Xvfb and the SDL 2, Mesa and zlib development files on the
host, it also builds the whole game with the Amiga's settings and runs it
on Xvfb: still running after 10 s, its first screen drawn.


What was changed
----------------

- amiga/patches/0001-amigaos-fonts-gl-stack.patch:
  - TGLconstants.h, TGL.cpp, TGLapp.cpp: the fonts by name
    (TGL_FONT_TITLE, TGL_FONT_IMPACT). Upstream ships Krungthep and Impact,
    which are not free to give away; on __amigaos__ they are Russo One and
    Anton, both SIL OFL, fetched at build time from Google Fonts.
  - main.cpp: on __amigaos__, a GL 2.1 compatibility context. The game's
    shaders are GLSL 1.10 with no #version; GFOpenGPU gives a 4.3 core
    context unless asked for compatibility.
  - compression.cpp: on __amigaos__, 16 KB packing chunks for upstream's
    256 KB. The packer keeps two of them on the stack, 512 KB, the whole of
    an Amiga program's; the packed bytes are the same.
- amiga/GL/glew.h, amiga/GL/glu.h: the game includes GLEW and GLU and calls
  only glewInit; GFOpenGPU's libGL has the whole GL API, so these stand in.
- amiga/zlib.h, amiga/tgl_zlib.c: the replays' zlib calls go to z.library
  (GFOpenZlib), opened at the first call and closed at exit, not to a
  compiled-in zlib. amiga/sdk carries z.library's C headers (MIT), which
  the stove's SDK does not have yet.
- amiga/amiga_start.c: 512 KB of stack, the $VER string, and the drawer as
  the current directory (the game finds everything by relative paths).
- amiga/: the Makefile, the GlowIcons (our own art), the Installer script
  and its spec. package/: the build, the release archive and the GFOpenUp
  part.

The AmiCradle Team.
