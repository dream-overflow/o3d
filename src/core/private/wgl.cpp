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

// ONLY IF O3D_WGL OR O3D_WINDOWS ARE SELECTED
#if defined(O3D_WGL) || defined(O3D_WINDOWS)

#include "o3d/core/dynamiclibrary.h"

#include "o3d/core/private/wgldefines.h"
#include "o3d/core/private/wgl.h"

#include <windows.h>
#include <wingdi.h>

/* WGL 1.x function pointer typedefs */
// typedef PROC
typedef PROC (* WGLGETPROCADDRESSPROC)(LPCSTR lpszProc);
static WGLGETPROCADDRESSPROC _wglGetProcAddress = nullptr;
static PFNSWAPBUFFERSPROC _swapBuffers = nullptr;

using namespace o3d;

DynamicLibrary* WGL::ms_wgl = nullptr;

// PFNSWAPBUFFERSPROC WGL::swapBuffers = nullptr;
PFNWGLSWAPLAYERBUFFERSPROC WGL::swapLayerBuffers = nullptr;
PFNWGLSWAPINTERVALEXTPROC WGL::swapIntervalEXT = nullptr;
PFNWGLGETEXTENSIONSSTRINGARBPROC WGL::getExtensionsStringARB = nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC WGL::createContextAttribsARB = nullptr;
PFNWGLCHOOSEPIXELFORMATARBPROC WGL::choosePixelFormatARB = nullptr;
PFNWGLMAKECURRENTPROC WGL::makeCurrent = nullptr;
PFNWGLCREATECONTEXTPROC WGL::createContext = nullptr;
PFNWGLDELETECONTEXTPROC WGL::deleteContext = nullptr;
PFNWGLGETCURRENTCONTEXTPROC WGL::getCurrentContext = nullptr;
PFNWGLSHARELISTSPROC WGL::shareLists = nullptr;

void WGL::init()
{
    ms_wgl = DynamicLibrary::load("Opengl32.dll");

    _wglGetProcAddress = (WGLGETPROCADDRESSPROC)ms_wgl->getFunctionPtr("wglGetProcAddress");

    _swapBuffers = (PFNSWAPBUFFERSPROC)getProcAddress("SwapBuffers");
    makeCurrent = (PFNWGLMAKECURRENTPROC)getProcAddress("wglMakeCurrent");
    createContext = (PFNWGLCREATECONTEXTPROC)getProcAddress("wglCreateContext");
    deleteContext = (PFNWGLDELETECONTEXTPROC)getProcAddress("wglDeleteContext");
    getCurrentContext = (PFNWGLGETCURRENTCONTEXTPROC)getProcAddress("wglGetCurrentContext");
    shareLists = (PFNWGLSHARELISTSPROC)getProcAddress("wglShareLists");

    // depend from a context
    swapLayerBuffers = nullptr;
    swapIntervalEXT = nullptr;
    getExtensionsStringARB = nullptr;
    createContextAttribsARB = nullptr;
    choosePixelFormatARB = nullptr;
}

void WGL::initContext(_HDC hDC)
{
    if (!ms_wgl || !hDC) {
        return;
    }

    swapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)getProcAddress("wglSwapIntervalEXT");

    getExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)getProcAddress("wglGetExtensionsStringARB");
    createContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)getProcAddress("wglCreateContextAttribsARB");
    choosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)getProcAddress("wglChoosePixelFormatARB");
    swapLayerBuffers = (PFNWGLSWAPLAYERBUFFERSPROC)getProcAddress("wglSwapLayerBuffers");
}

void WGL::quit()
{
    if (ms_wgl) {
        _wglGetProcAddress = nullptr;
        _swapBuffers = nullptr;
        getExtensionsStringARB = nullptr;

        DynamicLibrary::unload(ms_wgl);
        ms_wgl = nullptr;
    }
}

Bool WGL::isValid()
{
    return ms_wgl && _wglGetProcAddress;
}

void* WGL::getProcAddress(const Char *ext)
{   
    void *p = (void*)::_wglGetProcAddress((LPCSTR)ext);
    if (p == 0 || (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) || (p == (void*)-1)) {
        return ms_wgl->getFunctionPtr(ext);
    }

    return p;
}

Bool WGL::isExtensionSupported(const Char *ext, _HDC hDC)
{
    if (!getExtensionsStringARB) {
        return False;
    }

    const Char *extList = getExtensionsStringARB((::HDC)hDC);
    const Char *start;
    const Char *where, *terminator;

    // Extension names should not have spaces.
    where = strchr(ext, ' ');
    if (where || *ext == '\0') {
        return False;
    }

    // It takes a bit of care to be fool-proof about parsing the
    // OpenGL extensions string. Don't be fooled by sub-strings, etc.
    for (start = extList;;) {
        where = strstr(start, ext);

        if (!where) {
            break;
        }

        terminator = where + strlen(ext);

        if (where == start || *(where - 1) == ' ') {
            if (*terminator == ' ' || *terminator == '\0') {
                return True;
            }
        }

        start = terminator;
    }

    return False;
}

Bool WGL::swapBuffers(_HDC hDC)
{
    return ::SwapBuffers((::HDC)hDC);
//    if (swapLayerBuffers) {
//        return swapLayerBuffers((HDC)hDC, WGL_SWAP_MAIN_PLANE);
//    } else if (swapBuffers) {
//        _swapBuffers((HDC)hDC);
//    }
}

#endif // O3D_WGL || O3D_WINDOWS
