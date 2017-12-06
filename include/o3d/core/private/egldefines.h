/**
 * @file egldefines.h
 * @brief Defines and types for egl.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_EGLDEFINES_H
#define _O3D_EGLDEFINES_H

#include <EGL/egl.h>

typedef EGLDisplay (*EGLNATIVEDISPLAYPROC)(EGLNativeDisplayType display_id);
typedef EGLBoolean (*EGLINITIALIZEPROC)(EGLDisplay dpy, EGLint *major, EGLint *minor);
typedef EGLBoolean (*EGLBINDAPIPROC)(EGLenum api);
typedef EGLBoolean (*EGLCHOOSECONFIGPROC)(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config);
typedef EGLSurface (*EGLCREATEWINDOWSURFACEPROC)(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list);
typedef EGLContext (*EGLCREATECONTEXTPROC)(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list);
typedef EGLBoolean (*EGLMAKECURRENTPROC)(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);
typedef EGLBoolean (*EGLSWAPBUFFERSPROC)(EGLDisplay dpy, EGLSurface surface);
typedef EGLBoolean (*EGLDESTROYCONTEXTPROC)(EGLDisplay dpy, EGLContext ctx);
typedef EGLBoolean (*EGLDESTROYSURFACEPROC)(EGLDisplay dpy, EGLSurface surface);
typedef EGLBoolean (*EGLGETCONFIGATTRIBPROC)(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value);
typedef EGLContext (*EGLGETCURRENTCONTEXTPROC)(void);
typedef EGLBoolean (*EGLTERMINATEPROC)(EGLDisplay dpy);
typedef EGLBoolean (*EGLSWAPINTERVALPROC)(EGLDisplay dpy, EGLint interval);

#endif // _O3D_EGLDEFINES_H
