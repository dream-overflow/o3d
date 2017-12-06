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
#include "o3d/core/application.h"
#include "o3d/core/debug.h"

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
EGLTERMINATEPROC EGL::terminate = nullptr;
EGLSWAPINTERVALPROC EGL::swapInterval = nullptr;

DynamicLibrary* EGL::ms_egl = nullptr;
GL::GLAPIType EGL::ms_type = GL::GLAPI_UNDEFINED;

void EGL::init()
{
    if (ms_egl) {
        return;
    }

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
    swapInterval = (EGLSWAPINTERVALPROC)getProcAddress("eglSwapInterval");

    EGLNativeDisplayType display = reinterpret_cast<EGLNativeDisplayType>(Application::getDisplay());
    EGLDisplay eglDisplay = EGL::getDisplay(display);

    // Initialize EGL for this display (need 1.4+)
    EGLint eglVersionMajor, eglVersionMinor;
    if (!EGL::initialize(eglDisplay, &eglVersionMajor, &eglVersionMinor) ||
        ((eglVersionMajor == 1) && (eglVersionMinor < 4)) || (eglVersionMajor < 1)) {

        O3D_ERROR(E_InvalidResult("Invalid EGL version. Need 1.4+"));
    }

    // Selection of the GL API (GL on Desktop, GLES on mobile)
#if defined(O3D_LINUX) || defined(O3D_WINDOWS) || defined(O3D_MACOSX)
    EGL::bindAPI(EGL_OPENGL_API);
    ms_type = GL::GLAPI_GL;
#elif defined(O3D_ANDROID)
    EGL::bindAPI(EGL_OPENGL_ES_API);
    ms_type = GL::GLAPI_GLES3;
#endif
}

void EGL::quit()
{
    if (!ms_egl) {
        return;
    }

    if (terminate) {
        EGLNativeDisplayType display = reinterpret_cast<EGLNativeDisplayType>(Application::getDisplay());
        EGLDisplay eglDisplay = getDisplay(display);

        terminate(eglDisplay);
    }

    _eglGetProcAddress = nullptr;

    DynamicLibrary::unload(ms_egl);
    ms_egl = nullptr;
}

void* EGL::getProcAddress(const Char *ext)
{
    return (void*)::_eglGetProcAddress((const char*)ext);
}

GL::GLAPIType EGL::getType()
{
    return ms_type;
}

#endif // O3D_EGL
