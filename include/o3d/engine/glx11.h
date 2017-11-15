/**
 * @file glx11.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_GLX11_H
#define _O3D_GLX11_H

#ifdef O3D_X11

#include "o3d/core/types.h"

namespace o3d {

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*O3DGLXextFuncPtr)(void);

//! Returns a GL extension pointer address.
extern O3DGLXextFuncPtr glxGetProcAddress(const Char *ext);

#ifdef __cplusplus
}
#endif

} // namespace o3d

#endif // O3D_X11

#endif // _O3D_GLX11_H

