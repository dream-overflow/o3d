/**
 * @file wgl.cpp
 * @brief WGL support
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/types.h"

#if defined(O3D_WGL) || defined(O3D_WINDOWS)

#include "o3d/core/dynamiclibrary.h"

// #include "o3d/core/private/wgldefines.h"
#include "o3d/core/private/wgl.h"

#include <windows.h>
#include <wingdi.h>

/* WGL 1.x function pointer typedefs */
// typedef PROC
typedef PROC (* WGLGETPROCADDRESSPROC)(LPCSRT lpszProc);
static WGLGETPROCADDRESSPROC _wglGetProcAddress = nullptr;

using namespace o3d;

DynamicLibrary* WGL::ms_wgl = nullptr;

void WGL::init()
{
    // ms_wgl = DynamicLibrary::load("Opengl32.dll");

    // _wglGetProcAddress = (WGLGETPROCADDRESSPROC)ms_wgl->getFunctionPtr("wglGetProcAddress");

    // @todo
}

void WGL::quit()
{
    if (ms_wgl) {
        _wglGetProcAddress = nullptr;

        DynamicLibrary::unload(ms_wgl);
        ms_wgl = nullptr;
    }
}

void* WGL::getProcAddress(const Char *ext)
{
    // return (void*)::_wglGetProcAddress((const char*)ext);
    return (void*)::wglGetProcAddress((const char*)ext);
}

#endif // O3D_WGL || O3D_WINDOWS
