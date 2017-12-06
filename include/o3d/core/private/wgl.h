/**
 * @file wgl.h
 * @brief WGL support.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_WGL_H
#define _O3D_WGL_H

#if defined(O3D_WGL) || defined(O3D_WINDOWS)

#include "o3d/core/types.h"

namespace o3d {

class DynamicLibrary;

class O3D_API WGL
{
public:

    static void init();
    static void quit();

    //! Returns a GL extension pointer address.
    static void* getProcAddress(const Char *ext);

    // @todo
    // swapIntervalEXT

private:

    static DynamicLibrary *ms_wgl;
};

} // namespace o3d

#endif // O3D_WGL || O3D_WINDOWS

#endif // _O3D_WGL_H
