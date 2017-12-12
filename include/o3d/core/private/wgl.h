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

#ifdef O3D_WINAPI

#include "o3d/core/types.h"

namespace o3d {

class DynamicLibrary;

class O3D_API WGL
{
public:

    static void init();
    static void quit();

    static Bool isValid();

    //! Init context dependants functions
    static void initContext(_HDC hDC);

    //! Returns a GL extension pointer address.
    static void* getProcAddress(const Char *ext);

    static Bool swapBuffers(_HDC hDC);

    //! Is an extension supported (need getExtensionsStringARB to be valid before)
    static Bool isExtensionSupported(const Char *ext, _HDC hDC);

    static PFNWGLSWAPLAYERBUFFERSPROC swapLayerBuffers;
    static PFNWGLSWAPINTERVALEXTPROC swapIntervalEXT;
    static PFNWGLGETEXTENSIONSSTRINGARBPROC getExtensionsStringARB;
    static PFNWGLCREATECONTEXTATTRIBSARBPROC createContextAttribsARB;
    static PFNWGLCHOOSEPIXELFORMATARBPROC choosePixelFormatARB;
    static PFNWGLMAKECURRENTPROC makeCurrent;
    static PFNWGLCREATECONTEXTPROC createContext;
    static PFNWGLDELETECONTEXTPROC deleteContext;
    static PFNWGLGETCURRENTCONTEXTPROC getCurrentContext;
    static PFNWGLSHARELISTSPROC shareLists;

private:

    static DynamicLibrary *ms_wgl;
};

} // namespace o3d

#endif // O3D_WINAPI

#endif // _O3D_WGL_H
