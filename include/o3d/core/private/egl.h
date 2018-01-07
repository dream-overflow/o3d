/**
 * @file egl.h
 * @brief EGL support.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_EGL_H
#define _O3D_EGL_H

#include "o3d/core/types.h"

#ifdef O3D_EGL

#include "o3d/core/gl.h"

namespace o3d {

class DynamicLibrary;

class O3D_API EGL
{
public:

    static void init();
    static void quit();

    static Int32 majorVersion();
    static Int32 minorVersion();

    static Bool isValid();

    //! Returns a GL extension pointer address.
    static void* getProcAddress(const Char *ext);

    static EGLNATIVEDISPLAYPROC getDisplay;
    static EGLINITIALIZEPROC initialize;
    static EGLBINDAPIPROC bindAPI;
    static EGLCHOOSECONFIGPROC chooseConfig;
    static EGLCREATEWINDOWSURFACEPROC createWindowSurface;
    static EGLCREATECONTEXTPROC createContext;
    static EGLQUERYSURFACEPROC querySurface;
    static EGLMAKECURRENTPROC makeCurrent;
    static EGLSWAPBUFFERSPROC swapBuffers;
    static EGLDESTROYCONTEXTPROC destroyContext;
    static EGLDESTROYSURFACEPROC destroySurface;
    static EGLGETCONFIGATTRIBPROC getConfigAttrib;
    static EGLGETCURRENTCONTEXTPROC getCurrentContext;
    static EGLTERMINATEPROC terminate;
    static EGLSWAPINTERVALPROC swapInterval;

    //! Get instancied GL API type.
    static GL::GLAPIType getType();

private:

    static DynamicLibrary *ms_egl;
    static GL::GLAPIType ms_type;
    static Int32 ms_version[2];
};

} // namespace o3d

#endif // O3D_EGL

#endif // _O3D_EGL_H
