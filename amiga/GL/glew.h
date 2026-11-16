/*
 * Transball GL on GFOS: GLEW is not used. GFOpenGPU's libGL exports the
 * whole GL API, so the game's GL calls link straight to it; this header
 * stands in for GLEW's so the game builds unchanged.
 * GPL-3.0-or-later, like Transball GL. Copyright (c) 2026 AmiCradle Team.
 */
#ifndef TGL_AMIGA_GLEW_H
#define TGL_AMIGA_GLEW_H

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif
#include <GL/gl.h>
#include <GL/glext.h>

#define GLEW_OK 0
#define GLEW_VERSION_2_1 1
static GLboolean glewExperimental __attribute__((unused)) = GL_FALSE;
static inline int glewInit(void) { return GLEW_OK; }

/* Apple's name, in code the game builds only on a Mac */
#ifndef glDeleteVertexArraysAPPLE
#define glDeleteVertexArraysAPPLE glDeleteVertexArrays
#endif

#endif
