/**
 * @file wgldefines.h
 * @brief Defines and types for wgl.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_WGLDEFINES_H
#define _O3D_WGLDEFINES_H

#include "o3d/core/architecture.h"

#define WGL_DRAW_TO_WINDOW_ARB         0x2001
#define WGL_ACCELERATION_ARB           0x2003

#define WGL_SUPPORT_OPENGL_ARB         0x2010
#define WGL_DOUBLE_BUFFER_ARB          0x2011
#define WGL_STEREO_ARB                 0x2012
#define WGL_PIXEL_TYPE_ARB             0x2013

#define WGL_COLOR_BITS_ARB             0x2014
#define WGL_RED_BITS_ARB               0x2015
#define WGL_RED_SHIFT_ARB              0x2016
#define WGL_GREEN_BITS_ARB             0x2017
#define WGL_GREEN_SHIFT_ARB            0x2018
#define WGL_BLUE_BITS_ARB              0x2019
#define WGL_BLUE_SHIFT_ARB             0x201A
#define WGL_ALPHA_BITS_ARB             0x201B
#define WGL_ALPHA_SHIFT_ARB            0x201C
#define WGL_TYPE_RGBA_ARB              0x202B

#define WGL_DEPTH_BITS_ARB             0x2022
#define WGL_STENCIL_BITS_ARB           0x2023

#define WGL_NO_ACCELERATION_ARB        0x2025
#define WGL_GENERIC_ACCELERATION_ARB   0x2026
#define WGL_FULL_ACCELERATION_ARB      0x2027

#define WGL_SAMPLE_BUFFERS_ARB         0x2041
#define WGL_SAMPLES_ARB                0x2042

#define WGL_CONTEXT_MAJOR_VERSION_ARB   0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB   0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB     0x2093
#define WGL_CONTEXT_FLAGS_ARB           0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB    0x9126

#define WGL_CONTEXT_DEBUG_BIT_ARB       0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB    0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

#define ERROR_INVALID_VERSION_ARB       0x2095
#define ERROR_INVALID_PROFILE_ARB       0x2096

// @see WGL_CONTEXT_PROFILE_MASK_ARB

typedef BOOL (*PFNSWAPBUFFERSPROC)(HDC hdc);
typedef BOOL (*PFNWGLSWAPINTERVALEXTPROC)(int interval);
typedef int (*PFNWGLGETSWAPINTERVALEXTPROC)(void);
typedef BOOL (*PFNWGLMAKECURRENTPROC)(HDC hDc, HGLRC newContext);
typedef HGLRC (*PFNWGLCREATECONTEXTPROC)(HDC hDc);
typedef BOOL (*PFNWGLDELETECONTEXTPROC)(HGLRC oldContext);
typedef HGLRC (*PFNWGLGETCURRENTCONTEXTPROC)(void);
typedef BOOL (*PFNWGLSHARELISTSPROC)(HGLRC hrcSrvShare, HGLRC hrcSrvSource);

// wglGetExtensionsStringARB prototype.
typedef const char* (*PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC hdc);

// wglGetExtensionsStringARB prototype.
typedef BOOL (*PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC hdc,
                                              const int *piAttribIList,
                                              const FLOAT *pfAttribFList,
                                              UINT nMaxFormats,
                                              int *piFormats,
                                              UINT *nNumFormats);

// wglCreateContextAttribsARB prototype.
typedef HGLRC (*PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);

#endif // _O3D_WGLDEFINES_H
