/**
 * @file gl.cpp
 * @brief GL configuration entry point.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/gl.h"

#if defined(O3D_ANDROID) || defined(O3D_EGL)
#include "o3d/core/egldefines.h"
#include "o3d/core/egl.h"
#elif defined(O3D_X11)
#include "o3d/core/glxdefines.h"
#include "o3d/core/glx.h"
#elif defined(O3D_SDL2)
#include <SDL2/SDL.h>
#elif defined(O3D_WINDOWS)
// #include "o3d/core/wgldefines.h"
// #include "o3d/core/wgl.h"
#endif

using namespace o3d;

void GL::init(const Char *library)
{
#if defined(O3D_ANDROID) || defined(O3D_EGL)
    EGL::init();
#elif defined(O3D_X11)
    GLX::init();
#elif defined(O3D_SDL2)
    // SDL_Init(SDL_INIT_VIDEO); done at application level
#elif defined(O3D_WINDOWS)
    WGL::init();
#endif
}

void *GL::getProcAddress(const Char *ext)
{
#if defined(O3D_ANDROID) || defined(O3D_EGL)
    return EGL::getProcAddress(ext);
#elif defined(O3D_X11)
    return GLX::getProcAddress(ext);
#elif defined(O3D_SDL2)
    return SDL_GL_GetProcAddress(ext);
#elif defined(O3D_WINDOWS)
    return WGL::getProcAddress(ext);
#endif
}
