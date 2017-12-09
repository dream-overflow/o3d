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

#include "o3d/core/error.h"

#include <string.h>

using namespace o3d;

GL::GetProcAddressCallbackMethod::~GetProcAddressCallbackMethod()
{

}

GL::GetProcAddressCallbackMethod* GL::ms_callback = nullptr;
GL::Impl GL::ms_usedImpl = GL::IMPL_NONE;
GL::Impl GL::ms_nativeImpl = GL::IMPL_NONE;

void GL::init(const Char *library)
{
    ms_usedImpl = IMPL_NONE;

#if defined(O3D_ANDROID)    
  #if defined(O3D_X11)
    try {
        GLX::init();
        ms_nativeImpl = ms_usedImpl = IMPL_GLX_14;
        return;
    } catch(E_BaseException &e) {}
  #endif
  #if defined(O3D_SDL)
    // SDL2::init();
    if (SDL_WasInit(SDL_INIT_VIDEO) == SDL_INIT_VIDEO) {
        ms_nativeImpl = ms_usedImpl = IMPL_SDL_2;
        return;
    }
  #endif
  #if defined(O3D_EGL)
    try {
        EGL::init();
        ms_nativeImpl = ms_usedImpl = IMPL_EGL_15;
        return;
    } catch(E_BaseException &e) {}
  #endif
#elif defined(O3D_LINUX)
  #if defined(O3D_X11)
    try {
        GLX::init();
        ms_nativeImpl = ms_usedImpl = IMPL_GLX_14;
        return;
    } catch(E_BaseException &e) {}
  #endif
  #if defined(O3D_SDL2)
    // SDL2::init();
    if (SDL_WasInit(SDL_INIT_VIDEO) == SDL_INIT_VIDEO) {
        ms_nativeImpl = ms_usedImpl = IMPL_SDL_2;
        return;
    }
  #endif
  #if defined(O3D_EGL)
    try {
        EGL::init();
        ms_nativeImpl = ms_usedImpl = IMPL_EGL_15;
        return;
    } catch(E_BaseException &e) {}
  #endif
#elif defined(O3D_MACOSX)
  #if defined(O3D_X11)
    try {
        GLX::init();
        ms_nativeImpl = ms_usedImpl = IMPL_GLX_14;
        return;
    } catch(E_BaseException &e) {}
  #endif
  #if defined(O3D_SDL2)
    // SDL2::init();
    if (SDL_WasInit(SDL_INIT_VIDEO) == SDL_INIT_VIDEO) {
        ms_nativeImpl = ms_usedImpl = IMPL_SDL_2;
        return;
    }
  #endif
#elif defined(O3D_WINDOWS)
  #if defined(O3D_WIN32) || defined(O3D_WIN64)
    try {
        WGL::init();
        ms_nativeImpl = ms_usedImpl = IMPL_WGL;
        return;
    } catch(E_BaseException &e) {}
  #endif
  #if defined(O3D_SDL2)
    // SDL2::init();
    if (SDL_WasInit(SDL_INIT_VIDEO) == SDL_INIT_VIDEO) {
        ms_nativeImpl = ms_usedImpl = IMPL_SDL_2;
        return;
    }
  #endif
  #if defined(O3D_EGL)
    try {
        EGL::init();
        ms_nativeImpl = ms_usedImpl = IMPL_EGL_15;
        return;
    } catch(E_BaseException &e) {}
  #endif
#endif
}

void GL::quit()
{
    deletePtr(ms_callback);
    ms_nativeImpl = ms_usedImpl = IMPL_NONE;

#if defined(O3D_SDL)
    if (SDL_WasInit(SDL_INIT_VIDEO) == SDL_INIT_VIDEO) {
        // SDL2::quit();
    }
#endif
#if defined(O3D_EGL)
    if (EGL::isValid()) {
        EGL::quit();
    }
#endif
#if defined(O3D_X11)
    if (GLX::isValid()) {
        GLX::quit();
    }
#endif
#if defined(O3D_WIN32) || defined(O3D_WIN64)
    if (WGL::isValid()) {
        WGL::quit();
    }
#endif
}

void GL::setProcAddress(GetProcAddressCallbackMethod *callback)
{
    if (ms_callback) {
        // delete previous callback
        deletePtr(ms_callback);

        // fallback to system valid implementation
        ms_usedImpl = ms_nativeImpl;
    }

    if (callback) {
        ms_callback = callback;
        ms_usedImpl = IMPL_CUSTOM;
    }
}

void *GL::getProcAddress(const Char *ext)
{
    switch (ms_usedImpl) {
        case IMPL_CUSTOM:
            if (ms_callback) {
                return ms_callback->call(ext);
            }
            break;
        #ifdef O3D_EGL
        case IMPL_EGL_15:
            return EGL::getProcAddress(ext);
        #endif
        #ifdef O3D_X11
        case IMPL_GLX_14:
            return GLX::getProcAddress(ext);
        #endif
        #ifdef O3D_SDL2
        case IMPL_SDL_2:
            return ::SDL_GL_GetProcAddress(ext);
        #endif
        #ifdef O3D_WINDOWS
        case IMPL_WGL:
            return WGL::getProcAddress(ext);
        #endif
        default:
            return nullptr;
    }

    return nullptr;
}

GL::Impl GL::getImplementation()
{
    return ms_usedImpl;
}

const Char *GL::getImplementationName()
{
    switch (ms_usedImpl) {
        case IMPL_CUSTOM:
            if (ms_callback) {
                return "CUSTOM";
            }
            break;
        case IMPL_EGL_15:
            return "EGL";
        case IMPL_GLX_14:
            return "GLX";
        case IMPL_SDL_2:
            return "SDL2";
        case IMPL_WGL:
            return "WGL";
        default:
            return nullptr;
    }

    return nullptr;
}

GL::GLAPIType GL::getType()
{
    switch (ms_usedImpl) {
        case IMPL_CUSTOM:
            if (ms_callback) {
                return GLAPI_CUSTOM;
            }
            break;
        #ifdef O3D_EGL
        case IMPL_EGL_15:
            return EGL::getType();
        #endif
        case IMPL_GLX_14:
            return GLAPI_GL;  // desktop always GL
        #ifdef O3D_SDL2
        case IMPL_SDL_2:
            return GLAPI_GL;  // @todo getType
        #endif
        case IMPL_WGL:
            return GLAPI_GL;  // desktop always GL
        default:
            return GLAPI_UNDEFINED;
    }

    return GLAPI_UNDEFINED;
}

void GL::swapBuffers(_DISP display, _HWND hWnd, _HDC hdc)
{
    switch (ms_usedImpl) {
        case IMPL_CUSTOM:
            if (ms_callback) {
                ms_callback->swapBuffers(reinterpret_cast<void*>(hdc));
            }
            break;
        #ifdef O3D_EGL
        case IMPL_EGL_15:
            {
            #ifdef O3D_X11
                EGLDisplay eglDisplay = EGL::getDisplay(reinterpret_cast<Display*>(display));
                EGL::swapBuffers(eglDisplay, reinterpret_cast<EGLSurface>(hdc));
            #elif defined O3D_ANDROID
                // @todo
                // EGLDisplay eglDisplay = EGL::getDisplay(reinterpret_cast<Display*>(display));
                // EGL::swapBuffers(eglDisplay, reinterpret_cast<EGLSurface>(hdc));
            #endif
            }
            break;
        #endif
        #ifdef O3D_X11
        case IMPL_GLX_14:
            GLX::swapBuffers(reinterpret_cast<Display*>(display), static_cast<Window>(hWnd));
            break;
        #endif
        #ifdef O3D_SDL2
        case IMPL_SDL_2:
            // SDL::swapBuffers(reinterpret_cast<SDL_Window*>(hdc));  // @todo
            SDL_GL_SwapWindow(reinterpret_cast<SDL_Window*>(hdc));
            break;
        #endif
        #ifdef O3D_WINDOWS
        case IMPL_WGL:
            ::SwapBuffers((HDC)hdc);
            // WGL::swapBuffers((HDC)hdc);  // @todo
            break;
        #endif
        default:
            break;
    }
}
