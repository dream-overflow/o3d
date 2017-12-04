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

#ifdef O3D_EGL

#include "o3d/core/types.h"

namespace o3d {

class DynamicLibrary;

class O3D_API EGL
{
public:

    static void init();

    //! Returns a GL extension pointer address.
    static void* getProcAddress(const Char *ext);
/*
    static glXQueryExtensionsStringProc queryExtensionsString;
    static glXCreateContextProc createContext;
    static glXDestroyContextProc destroyContext;
    static glXCreateNewContextProc createNewContext;
    static glXMakeCurrentProc makeCurrent;
    static glXIsDirectProc isDirect;
    static glXGetCurrentContextProc getCurrentContext;
    static glXQueryDrawableProc queryDrawable;

    static glXQueryVersionProc queryVersion;
    static glXChooseFBConfigProc chooseFBConfig;
    static glXGetVisualFromFBConfigProc getVisualFromFBConfig;
    static glXGetFBConfigAttribProc getFBConfigAttrib;
    static eglSwapBuffersProc swapBuffers;
*/
private:

    static DynamicLibrary *ms_egl;
};

} // namespace o3d

#endif // O3D_EGL

#endif // _O3D_EGL_H
