/**
 * @file egl.cpp
 * @brief EGL support
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/types.h"

#ifdef O3D_EGL

#include "o3d/core/private/egldefines.h"
#include "o3d/core/private/egl.h"

#include "o3d/core/dynamiclibrary.h"

/* EGL 1.5 function pointer typedefs */
typedef void (*__eglMustCastToProperFunctionPointerType)(void);
typedef __eglMustCastToProperFunctionPointerType (* PFNEGLGETPROCADDRESSPROC) (const char *procName);
static PFNEGLGETPROCADDRESSPROC _eglGetProcAddress = nullptr;

using namespace o3d;

EGLNATIVEDISPLAYPROC EGL::getDisplay = nullptr;
EGLINITIALIZEPROC EGL::initialize = nullptr;
EGLBINDAPIPROC EGL::bindAPI = nullptr;
EGLCHOOSECONFIGPROC EGL::chooseConfig = nullptr;
EGLCREATEWINDOWSURFACEPROC EGL::createWindowSurface = nullptr;
EGLCREATECONTEXTPROC EGL::createContext = nullptr;
EGLMAKECURRENTPROC EGL::makeCurrent = nullptr;
EGLSWAPBUFFERSPROC EGL::swapBuffers = nullptr;
EGLDESTROYCONTEXTPROC EGL::destroyContext = nullptr;
EGLDESTROYSURFACEPROC EGL::destroySurface = nullptr;
EGLGETCONFIGATTRIBPROC EGL::getConfigAttrib = nullptr;
EGLGETCURRENTCONTEXTPROC EGL::getCurrentContext = nullptr;

DynamicLibrary* EGL::ms_egl = nullptr;

void EGL::init()
{
    ms_egl = DynamicLibrary::load("libEGL.so");

    _eglGetProcAddress = (PFNEGLGETPROCADDRESSPROC)ms_egl->getFunctionPtr("eglGetProcAddress");

    getDisplay = (EGLNATIVEDISPLAYPROC)getProcAddress("eglGetDisplay");
    initialize = (EGLINITIALIZEPROC)getProcAddress("eglInitialize");
    bindAPI = (EGLBINDAPIPROC)getProcAddress("eglBindAPI");
    chooseConfig = (EGLCHOOSECONFIGPROC)getProcAddress("eglChooseConfig");
    createWindowSurface = (EGLCREATEWINDOWSURFACEPROC)getProcAddress("eglCreateWindowSurface");
    createContext = (EGLCREATECONTEXTPROC)getProcAddress("eglCreateContext");
    makeCurrent = (EGLMAKECURRENTPROC)getProcAddress("eglMakeCurrent");
    swapBuffers = (EGLSWAPBUFFERSPROC)getProcAddress("eglSwapBuffers");
    destroyContext = (EGLDESTROYCONTEXTPROC)getProcAddress("eglDestroyContext");
    destroySurface = (EGLDESTROYSURFACEPROC)getProcAddress("eglDestroySurface");
    getConfigAttrib = (EGLGETCONFIGATTRIBPROC)getProcAddress("eglGetConfigAttrib");
    getCurrentContext = (EGLGETCURRENTCONTEXTPROC)getProcAddress("eglGetCurrentContext");
}

void* EGL::getProcAddress(const Char *ext)
{
    return (void*)::_eglGetProcAddress((const char*)ext);
}

#endif // O3D_EGL
