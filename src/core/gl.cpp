/**
 * @file gl.cpp
 * @brief GL configuration entry point.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/gl.h"

#if defined(O3D_EGL)
  #include "o3d/core/private/egldefines.h"
  #include "o3d/core/private/egl.h"
#endif
#if defined(O3D_SDL2)
  #include <SDL2/SDL.h>
#endif
#if defined(O3D_X11)
  #include "o3d/core/private/glxdefines.h"
  #include "o3d/core/private/glx.h"
#endif
#if defined(O3D_WINDOWS)
  #include "o3d/core/private/wgldefines.h"
  #include "o3d/core/private/wgl.h"
#endif

#include <string.h>

using namespace o3d;

GL::GetProcAddressCallbackMethod::~GetProcAddressCallbackMethod()
{

}

GL::GetProcAddressCallbackMethod* GL::ms_callback = nullptr;

void GL::init(const Char *library)
{
    // qt manage for us
    if (library && (strcasecmp(library, "qt") == 0)) {
        return;
    }

#if defined(O3D_ANDROID)
  #if defined(O3D_SDL)
    // SDL2::init();
  #endif
  #if defined(O3D_EGL)
    EGL::init();
  #endif
#elif defined(O3D_LINUX)
  #if defined(O3D_SDL2)
    // SDL2::init();
  #endif
  #if defined(O3D_EGL)
    EGL::init();
  #endif
  #if defined(O3D_X11)
    GLX::init();
  #endif
#elif defined(O3D_MACOSX)
  #if defined(O3D_SDL2)
    // SDL2::init();
  #endif
  #if defined(O3D_EGL)
    EGL::init();
  #endif
  #if defined(O3D_X11)
    GLX::init();
  #endif
#elif defined(O3D_WINDOWS)
  #if defined(O3D_SDL2)
    // SDL2::init();
  #endif
  #if defined(O3D_EGL)
    EGL::init();
  #endif
  #if defined(O3D_WIN32) || defined(O3D_WIN64)
    WGL::init();
  #endif
#endif
}

// @todo remove me
#ifdef O3D_EGL
#undef O3D_EGL
#endif

void GL::quit()
{
    deletePtr(ms_callback);
}

void GL::setProcAddress(GetProcAddressCallbackMethod *callback)
{
    if (ms_callback) {
        // delete previous callback
        delete ms_callback;
    }

    ms_callback = callback;
}

void *GL::getProcAddress(const Char *ext)
{
    if (ms_callback) {
        return ms_callback->call(ext);
    }

    // SDL2 take precedence on EGL thats is take precedence over GLX or WGL
#if defined(O3D_SDL2)
    return SDL_GL_GetProcAddress(ext);
#elif defined(O3D_EGL)
    return EGL::getProcAddress(ext);
#elif defined(O3D_X11)
    return GLX::getProcAddress(ext);
#elif defined(O3D_WINDOWS)
    return WGL::getProcAddress(ext);
#endif

    return nullptr;
}

const Char *GL::getImplementation()
{
    if (ms_callback) {
        return "CUSTOM";
    }

    // SDL2 take precedence on EGL thats is take precedence over GLX or WGL
#if defined(O3D_SDL2)
    return "SDL2";
#elif defined(O3D_EGL)
    return "EGL";
#elif defined(O3D_X11)
    return "GLX";
#elif defined(O3D_WINDOWS)
    return "WGL";
#endif

    return nullptr;
}

void GL::swapBuffers(_DISP display, _HWND hWnd, _HDC hdc)
{
    if (ms_callback) {
        ms_callback->swapBuffers(reinterpret_cast<void*>(hdc));
    } else {
        // SDL2 take precedence on EGL thats is take precedence over GLX or WGL
    #if defined(O3D_SDL2)
        // SDL::swapBuffers(reinterpret_cast<SDL_Window*>(hdc));
        SDL_GL_SwapWindow(reinterpret_cast<SDL_Window*>(hdc));
    #elif defined(O3D_EGL)
        EGL::swapBuffers(reinterpret_cast<EGLDisplay>(display), reinterpret_cast<EGLSurface>(hdc));
    #elif defined(O3D_X11)
        GLX::swapBuffers(reinterpret_cast<Display*>(display), static_cast<Window>(hWnd));
    #elif defined(O3D_WINDOWS)
        ::SwapBuffers((HDC)hdc);
        WGL::swapBuffers((HDC)hdc);
    #endif
    }
}
