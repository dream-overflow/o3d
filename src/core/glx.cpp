/**
 * @file glx11.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include "o3d/core/glxdefines.h"
#include "o3d/core/glx.h"
#include "o3d/core/dynamiclibrary.h"

#ifdef O3D_X11

typedef unsigned char GLubyte;

/* GLX 1.4 function pointer typedefs */
typedef void (*__GLXextFuncPtr)(void);
typedef __GLXextFuncPtr (* PFNGLXGETPROCADDRESSPROC) (const GLubyte *procName);
static PFNGLXGETPROCADDRESSPROC glXGetProcAddress = nullptr;

using namespace o3d;

DynamicLibrary* GLX::ms_glX = nullptr;

glXQueryExtensionsStringProc GLX::queryExtensionsString = nullptr;
glXCreateContextProc GLX::createContext = nullptr;
glXDestroyContextProc GLX::destroyContext = nullptr;
glXCreateNewContextProc GLX::createNewContext = nullptr;
glXMakeCurrentProc GLX::makeCurrent = nullptr;
glXIsDirectProc GLX::isDirect = nullptr;
glXGetCurrentContextProc GLX::getCurrentContext = nullptr;
glXQueryDrawableProc GLX::queryDrawable = nullptr;

glXQueryVersionProc GLX::queryVersion = nullptr;
glXChooseFBConfigProc GLX::chooseFBConfig = nullptr;
glXGetVisualFromFBConfigProc GLX::getVisualFromFBConfig = nullptr;
glXGetFBConfigAttribProc GLX::getFBConfigAttrib = nullptr;
glXSwapBuffersProc GLX::swapBuffers = nullptr;

void GLX::init()
{
    ms_glX = DynamicLibrary::load("libGLX.so");
    glXGetProcAddress = (PFNGLXGETPROCADDRESSPROC)ms_glX->getFunctionPtr("glXGetProcAddress");

    queryExtensionsString = (glXQueryExtensionsStringProc)getProcAddress("glXQueryExtensionsString");
    createContext = (glXCreateContextProc)getProcAddress("glXCreateContext");
    destroyContext = (glXDestroyContextProc)getProcAddress("glXDestroyContext");
    createNewContext = (glXCreateNewContextProc)getProcAddress("glXCreateNewContext");
    makeCurrent = (glXMakeCurrentProc)getProcAddress("glXMakeCurrent");
    isDirect = (glXIsDirectProc)getProcAddress("glXIsDirect");
    getCurrentContext = (glXGetCurrentContextProc)getProcAddress("glXGetCurrentContext");
    queryDrawable = (glXQueryDrawableProc)getProcAddress("glXQueryDrawable");

    queryVersion = (glXQueryVersionProc)getProcAddress("glXQueryVersion");
    chooseFBConfig = (glXChooseFBConfigProc)getProcAddress("glXChooseFBConfig");
    getVisualFromFBConfig = (glXGetVisualFromFBConfigProc)getProcAddress("glXGetVisualFromFBConfig");
    getFBConfigAttrib = (glXGetFBConfigAttribProc)getProcAddress("glXGetFBConfigAttrib");
    swapBuffers = (glXSwapBuffersProc)getProcAddress("glXSwapBuffers");
}

void* GLX::getProcAddress(const Char *ext)
{
    return (void*)::glXGetProcAddress((const GLubyte*)ext);
}

#endif // O3D_X11
