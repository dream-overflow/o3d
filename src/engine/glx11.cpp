/**
 * @file glx11.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include "o3d/engine/glx11.h"

#ifdef O3D_X11

#include <GL/glx.h>

using namespace o3d;

#ifdef __cplusplus
extern "C" {
#endif

O3DGLXextFuncPtr o3d::glxGetProcAddress(const Char *ext)
{
    return ::glXGetProcAddress((const GLubyte*)ext);
}

#ifdef __cplusplus
}
#endif

#endif // O3D_X11

