/**
 * @file glx11.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/types.h"

#ifdef O3D_X11

#include "o3d/core/private/glxdefines.h"
#include "o3d/core/private/glx.h"
#include "o3d/core/dynamiclibrary.h"

using namespace o3d;

typedef unsigned char GLubyte;

/* GLX 1.4 function pointer typedefs */
typedef void (*__GLXextFuncPtr)(void);
typedef __GLXextFuncPtr (* PFNGLXGETPROCADDRESSPROC) (const GLubyte *procName);
static PFNGLXGETPROCADDRESSPROC _glXGetProcAddress = nullptr;

DynamicLibrary* GLX::ms_glX = nullptr;

GLXQUERYEXTENSIONSSTRINGPROC GLX::queryExtensionsString = nullptr;
GLXCREATECONTEXTPROC GLX::createContext = nullptr;
GLXDESTROYCONTEXTPROC GLX::destroyContext = nullptr;
GLXCREATENEWCONTEXTPROC GLX::createNewContext = nullptr;
GLXMAKECURRENTPROC GLX::makeCurrent = nullptr;
GLXISDIRECTPROC GLX::isDirect = nullptr;
GLXGETCURRENTCONTEXTPROC GLX::getCurrentContext = nullptr;
GLXQUERYDRAWABLEPROC GLX::queryDrawable = nullptr;

GLXQUERYVERSIONPROC GLX::queryVersion = nullptr;
GLXCHOOSEFBCONFIGPROC GLX::chooseFBConfig = nullptr;
GLXGETVISUALFROMFBCONFIGPROC GLX::getVisualFromFBConfig = nullptr;
GLXGETFBCONFIGATTRIBPROC GLX::getFBConfigAttrib = nullptr;
GLXSWAPBUFFERSPROC GLX::swapBuffers = nullptr;

void GLX::init()
{
    ms_glX = DynamicLibrary::load("libGLX.so");

    _glXGetProcAddress = (PFNGLXGETPROCADDRESSPROC)ms_glX->getFunctionPtr("glXGetProcAddress");

    queryExtensionsString = (GLXQUERYEXTENSIONSSTRINGPROC)getProcAddress("glXQueryExtensionsString");
    createContext = (GLXCREATECONTEXTPROC)getProcAddress("glXCreateContext");
    destroyContext = (GLXDESTROYCONTEXTPROC)getProcAddress("glXDestroyContext");
    createNewContext = (GLXCREATENEWCONTEXTPROC)getProcAddress("glXCreateNewContext");
    makeCurrent = (GLXMAKECURRENTPROC)getProcAddress("glXMakeCurrent");
    isDirect = (GLXISDIRECTPROC)getProcAddress("glXIsDirect");
    getCurrentContext = (GLXGETCURRENTCONTEXTPROC)getProcAddress("glXGetCurrentContext");
    queryDrawable = (GLXQUERYDRAWABLEPROC)getProcAddress("glXQueryDrawable");

    queryVersion = (GLXQUERYVERSIONPROC)getProcAddress("glXQueryVersion");
    chooseFBConfig = (GLXCHOOSEFBCONFIGPROC)getProcAddress("glXChooseFBConfig");
    getVisualFromFBConfig = (GLXGETVISUALFROMFBCONFIGPROC)getProcAddress("glXGetVisualFromFBConfig");
    getFBConfigAttrib = (GLXGETFBCONFIGATTRIBPROC)getProcAddress("glXGetFBConfigAttrib");
    swapBuffers = (GLXSWAPBUFFERSPROC)getProcAddress("glXSwapBuffers");
}

void* GLX::getProcAddress(const Char *ext)
{
    return (void*)::_glXGetProcAddress((const GLubyte*)ext);
}

#endif // O3D_X11
