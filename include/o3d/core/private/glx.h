/**
 * @file glx.h
 * @brief GLX manager.
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

class DynamicLibrary;

class O3D_API GLX
{
public:

    static void init();
    static void quit();

    static Bool isValid();

    //! Returns a GL extension pointer address.
    static void* getProcAddress(const Char *ext);

    static GLXQUERYEXTENSIONSSTRINGPROC queryExtensionsString;
    static GLXCREATECONTEXTPROC createContext;
    static GLXDESTROYCONTEXTPROC destroyContext;
    static GLXCREATENEWCONTEXTPROC createNewContext;
    static GLXMAKECURRENTPROC makeCurrent;
    static GLXISDIRECTPROC isDirect;
    static GLXGETCURRENTCONTEXTPROC getCurrentContext;
    static GLXQUERYDRAWABLEPROC queryDrawable;
    static GLXQUERYVERSIONPROC queryVersion;
    static GLXCHOOSEFBCONFIGPROC chooseFBConfig;
    static GLXGETVISUALFROMFBCONFIGPROC getVisualFromFBConfig;
    static GLXGETFBCONFIGATTRIBPROC getFBConfigAttrib;
    static GLXSWAPBUFFERSPROC swapBuffers;
    static GLXSWAPINTERVALEXTPROC swapIntervalEXT;

private:

    static DynamicLibrary *ms_glX;
};

} // namespace o3d

#endif // O3D_X11

#endif // _O3D_GLX11_H
