/**
 * @file rendererandroid.cpp
 * @brief
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/renderer.h"

// ONLY IF O3D_ANDROID IS SELECTED
#ifdef O3D_ANDROID

#include "o3d/engine/glextdefines.h"
#include "o3d/engine/glextensionmanager.h"

#include "o3d/core/gl.h"

#include "o3d/engine/context.h"
#include "o3d/core/appwindow.h"
#include "o3d/core/application.h"
#include "o3d/core/debug.h"

#ifdef O3D_EGL
  #include "o3d/core/private/egldefines.h"
  #include "o3d/core/private/egl.h"
#endif

using namespace o3d;

// Create the OpenGL context.
void Renderer::create(AppWindow *appWindow, Bool debug, Renderer *sharing)
{
    if (m_state.getBit(STATE_DEFINED)) {
        O3D_ERROR(E_InvalidPrecondition("The renderer is already initialized"));
    }

    if (!appWindow || (appWindow->getHWND() == NULL_HWND)) {
        O3D_ERROR(E_InvalidParameter("Invalid application window"));
    }

    if ((sharing != nullptr) && m_sharing) {
        O3D_ERROR(E_InvalidOperation("A shared renderer cannot be sharing"));
    }

    O3D_MESSAGE("Creating a new OpenGLES context...");

    if (GL::getImplementation() == GL::IMPL_EGL_15) {
        //
        // EGL implementation
        //

    #ifdef O3D_EGL
        EGLDisplay eglDisplay = EGL::getDisplay(reinterpret_cast<EGLNativeDisplayType>(Application::getDisplay()));
        EGLSurface eglSurface = reinterpret_cast<EGLSurface>(appWindow->getHDC());
        EGLConfig eglConfig = reinterpret_cast<EGLConfig>(appWindow->getPixelFormat());

        EGLint contextAttributes[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
        };

        EGLContext eglContext = eglCreateContext(
                        eglDisplay,
                        eglConfig,
                        sharing ? reinterpret_cast<EGLContext>(sharing->getHGLRC()) : EGL_NO_CONTEXT,
                        contextAttributes);

        if (eglContext == EGL_NO_CONTEXT) {
            O3D_ERROR(E_InvalidResult("Unable to create the OpenGLES context"));
        }

        EGL::makeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

        m_HDC = appWindow->getHDC();
        m_HGLRC = reinterpret_cast<_HGLRC>(eglContext);
        m_state.enable(STATE_DEFINED);
        m_state.enable(STATE_EGL);
    #else
        O3D_ERROR(E_UnsuportedFeature("Support for EGL is missing"));
    #endif
    } else {
        O3D_ERROR(E_UnsuportedFeature("Support for EGL only"));
    }

    GLExtensionManager::init();

    O3D_MESSAGE("Video renderer: " + getRendererName());
    O3D_MESSAGE("OpenGL version: " + getStrVersion());

    const GLubyte *version = _glGetString(GL_VERSION);
    if (version && (version[0] == '1')) {
        O3D_WARNING("OpenGLES 2.0 or greater is not available, try to found ARB/EXT");
    }

    // compute the gl version
    Int32 glVersion = (version[0] - '0') * 100 + (version[2] - '0') * 10;
    if (glVersion > OGL_460) {
        glVersion = OGL_460;
    }

    m_version = Version(glVersion);

    m_appWindow = appWindow;

    m_bpp = appWindow->getBpp();
    m_depth = appWindow->getDepth();
    m_stencil = appWindow->getStencil();
    m_samples = appWindow->getSamples();

    if (sharing) {
        m_sharing = sharing;
        m_sharing->m_shareCount++;
    }

    if (debug) {
        initDebug();
    }

    m_glContext = new Context(this);

    doAttachment(m_appWindow);
}

// delete the renderer
void Renderer::destroy()
{
    if (m_state.getBit(STATE_DEFINED)) {
        if (m_refCount > 0) {
            O3D_ERROR(E_InvalidPrecondition("Unable to destroy a referenced renderer"));
        }

        if (m_shareCount > 0) {
            O3D_ERROR(E_InvalidPrecondition("All shared renderer must be destroyed before"));
        }

        // unshare
        if (m_sharing) {
            m_sharing->m_shareCount--;

            if (m_sharing->m_shareCount < 0) {
                O3D_ERROR(E_InvalidResult("Share counter reference is negative"));
            }

            m_sharing = nullptr;
        }

        deletePtr(m_glContext);

        if (m_HGLRC && m_appWindow) {
            if (m_state.getBit(STATE_EGL)) {
            #ifdef O3D_EGL
                EGLDisplay eglDisplay = EGL::getDisplay(reinterpret_cast<EGLNativeDisplayType>(Application::getDisplay()));
                EGL::makeCurrent(eglDisplay, 0, 0, 0);

                EGL::destroyContext(eglDisplay, reinterpret_cast<EGLContext>(m_HGLRC));
            #endif
            }

            m_HGLRC = NULL_HGLRC;
        }

        m_HDC = NULL_HDC;

        m_depth = m_bpp = m_stencil = m_samples = 0;

        m_state.zero();
        m_version = OGL_UNDEFINED;

        if (m_appWindow) {
            disconnect(m_appWindow);
            m_appWindow = nullptr;
        }

        m_glErrno = GL_NO_ERROR;
    }
}

void *Renderer::getProcAddress(const Char *ext) const
{
    return EGL::getProcAddress(ext);
}

// Is it the current OpenGL context.
Bool Renderer::isCurrent() const
{
    if (!m_state.getBit(STATE_DEFINED)) {
        return False;
    }

    if (m_state.getBit(STATE_EGL)) {
    #ifdef O3D_EGL
        return EGL::getCurrentContext() == reinterpret_cast<EGLContext>(m_HGLRC);
    #endif
    } else {
        return False;
    }
}

// Set as current OpenGL context
void Renderer::setCurrent()
{
    if (!m_state.getBit(STATE_DEFINED)) {
        return;
    }

    if (m_state.getBit(STATE_EGL)) {
    #ifdef O3D_EGL
        if (EGL::getCurrentContext() == reinterpret_cast<EGLContext>(m_HGLRC)) {
            return;
        }

        EGLDisplay eglDisplay = EGL::getDisplay(reinterpret_cast<EGLNativeDisplayType>(Application::getDisplay()));

        if (EGL::makeCurrent(
                eglDisplay,
                reinterpret_cast<EGLSurface>(m_HDC),
                reinterpret_cast<EGLSurface>(m_HDC),
                reinterpret_cast<EGLContext>(m_HGLRC))) {
            O3D_ERROR(E_InvalidResult("Unable to set the current OpenGL context"));
        }
    #else
        O3D_ERROR(E_InvalidResult("Unable to set the current OpenGL context"));
    #endif
    } else {
        O3D_ERROR(E_InvalidResult("Unable to set the current OpenGL context"));
    }
}

Bool Renderer::setVSyncMode(VSyncMode mode)
{
    if (!m_state.getBit(STATE_DEFINED)) {
        return False;
    }

    int value = 0;

    if (mode == VSYNC_NONE) {
        value = 0;
    } else if (mode == VSYNC_YES) {
        value = 1;
    } else if (mode == VSYNC_ADAPTIVE) {
        value = -1;
    }

    if (m_state.getBit(STATE_EGL)) {
    #ifdef O3D_EGL
        EGLDisplay eglDisplay = EGL::getDisplay(reinterpret_cast<EGLNativeDisplayType>(Application::getDisplay()));
        if (!EGL::swapInterval(eglDisplay, value)) {
            return False;
        }
    #else
        return False;
    #endif
    } else {
        return False;
    }

    if (mode == VSYNC_NONE) {
        m_state.setBit(STATE_VSYNC, False);
        m_state.setBit(STATE_ADAPTIVE_VSYNC, False);
    } else if (mode == VSYNC_YES) {
        m_state.setBit(STATE_VSYNC, True);
        m_state.setBit(STATE_ADAPTIVE_VSYNC, False);
    } else if (mode == VSYNC_ADAPTIVE) {
        m_state.setBit(STATE_VSYNC, True);
        m_state.setBit(STATE_ADAPTIVE_VSYNC, True);
    }

    return True;
}

#endif // O3D_ANDROID
