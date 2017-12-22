/**
 * @file rendererwin32.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/renderer.h"

// ONLY IF O3D_WINAPI IS SELECTED
#ifdef O3D_WINAPI

using namespace o3d;

#include "o3d/engine/context.h"
#include "o3d/engine/glextdefines.h"
#include "o3d/engine/glextensionmanager.h"

#include "o3d/core/appwindow.h"
#include "o3d/core/debug.h"
#include "o3d/core/architecture.h"

#include "o3d/core/gl.h"
#include "o3d/core/private/wgldefines.h"
#include "o3d/core/private/wgl.h"

#ifdef O3D_EGL
  #include "o3d/core/private/egldefines.h"
  #include "o3d/core/private/egl.h"
#endif

// Use the best GPU with Optimus
#ifdef _MSC_VER
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#else
extern "C" {
    DWORD NvOptimusEnablement = 0x00000001;
}
#endif

// Create the OpenGL context.
void Renderer::create(AppWindow *appWindow, Bool debug, Renderer *sharing)
{
    if (m_state.getBit(STATE_DEFINED)) {
		O3D_ERROR(E_InvalidOperation("The renderer is already initialized"));
    }

    if (!appWindow || !appWindow->isSet()) {
		O3D_ERROR(E_InvalidParameter("Invalid application window"));
    }

    if (appWindow->getHWND() == NULL_HWND) {
		m_appWindow = appWindow;

		O3D_MESSAGE("Using external OpenGL renderer");
		return;
	}

    if ((sharing != nullptr) && m_sharing) {
        O3D_ERROR(E_InvalidOperation("A shared renderer cannot be sharing"));
    }

	O3D_MESSAGE("Creating a new OpenGL context...");

	m_HDC = (_HDC)GetDC(reinterpret_cast<HWND>(appWindow->getHWND()));

    if (GL::getImplementation() == GL::IMPL_WGL) {
        //
        // WGL implementation
        //

        if ((m_HGLRC = (_HGLRC)WGL::createContext((HDC)m_HDC)) == nullptr) {
            O3D_ERROR(E_InvalidResult("Unable to create the OpenGL context"));
        }

        if (sharing != nullptr) {
            if (!WGL::shareLists((HGLRC)sharing->getHGLRC(), (HGLRC)m_HGLRC))
                O3D_ERROR(E_InvalidResult("Unable to share the OpenGL context"));
        }

        if (WGL::makeCurrent((HDC)m_HDC, (HGLRC)m_HGLRC) == False) {
            O3D_ERROR(E_InvalidResult("Unable to set the new OpenGL context as current"));
        }

        Int32 queryMajor = 1;
        Int32 queryMinor = 0;

        // we can retrieve glGetString now
        _glGetString = (PFNGLGETSTRINGPROC)WGL::getProcAddress("glGetString");

        const GLubyte *version = _glGetString(GL_VERSION);
        if (version && (version[0] == '3')) {
            // try an OpenGL 3.0+ context.
            queryMajor = 3;
            queryMinor = version[2] - '0';
        } else if (version && (version[0] == '4')) {
            // try an OpenGL 4.0+ context.
            queryMajor = 4;
            queryMinor = version[2] - '0';
        } else if (version && (version[0] > '4')) {
            // try an OpenGL 4.5 context.
            queryMajor = 4;
            queryMinor = 5;
        }

        // we need at least OpenGL 1.2
        if (version && (version[0] == '1') && (version[2] < '2')) {
            WGL::makeCurrent((HDC)m_HDC, 0);
            WGL::deleteContext((HGLRC)m_HGLRC);

            m_HDC = nullptr;
            m_HGLRC = nullptr;

            O3D_ERROR(E_InvalidPrecondition("OpenGL 1.2 or greater must be available"));
        }

        //
        // Create an OpenGL context with wglGetExtensionsStringARB
        //

        // Check for the WGL_ARB_create_context extension string and the function.
        // If either is not present, use old style context creation method.
        if (!WGL::getExtensionsStringARB || !WGL::isExtensionSupported("WGL_ARB_create_context", m_HDC) || !WGL::createContextAttribsARB) {
            O3D_MESSAGE("WGL_ARB_create_context is not present use old style");
        } else {
            O3D_MESSAGE(String::print("Creating an OpenGL %i.%i context...", queryMajor, queryMinor));

            int flagsARB = 0;

            if (queryMajor > 2) {
                flagsARB |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
            }

            if (debug) {
                flagsARB |= WGL_CONTEXT_DEBUG_BIT_ARB;
            }

            // Ask for the more higher supported GL context
            int attribList[] = {
                WGL_CONTEXT_MAJOR_VERSION_ARB, queryMajor,
                WGL_CONTEXT_MINOR_VERSION_ARB, queryMinor,
                WGL_CONTEXT_FLAGS_ARB, flagsARB,
                //queryMajor > 2 ? WGL_CONTEXT_PROFILE_MASK_ARB : 0, queryMajor > 2 ? WGL_CONTEXT_CORE_PROFILE_BIT_ARB : 0,
                0,
                0 };

            _HGLRC prevContext = m_HGLRC;

            // And create the new context with attributes
            if ((m_HGLRC = (_HGLRC)WGL::createContextAttribsARB(
                     (HDC)m_HDC,
                     sharing != nullptr ? (HGLRC)sharing->getHGLRC() : 0,
                     attribList)) == nullptr) {

                // Delete the previous context
                WGL::makeCurrent((HDC)m_HDC, 0);
                WGL::deleteContext((HGLRC)prevContext);

                O3D_ERROR(E_InvalidResult(String("Unable to create the OpenGL ") +
                                          reinterpret_cast<const Char*>(version) + " context"));
            }

            // Delete the previous context
            WGL::makeCurrent((HDC)m_HDC, 0);
            WGL::deleteContext((HGLRC)prevContext);

            if (WGL::makeCurrent((HDC)m_HDC, (HGLRC)m_HGLRC) == False) {
                O3D_ERROR(E_InvalidResult(String("Unable to set the new OpenGL ") +
                                          reinterpret_cast<const Char*>(version) + " context as current"));
            }
        }

        m_state.enable(STATE_DEFINED);
    } else if (GL::getImplementation() == GL::IMPL_EGL_15) {
        //
        // EGL implementation
        //

    #ifdef O3D_EGL
        EGLDisplay eglDisplay = EGL::getDisplay(EGL_DEFAULT_DISPLAY);
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
            O3D_ERROR(E_InvalidResult("Unable to create the OpenGL context"));
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
        O3D_ERROR(E_UnsuportedFeature("Support for WGL or EGL only"));
    }

    GLExtensionManager::init();

	O3D_MESSAGE("Video renderer: " + getRendererName());
	O3D_MESSAGE("OpenGL version: " + getVersion());

    computeVersion();
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
			O3D_ERROR(E_InvalidPrecondition("Unable to destroy a bound renderer"));
        }

        if (m_shareCount > 0) {
			O3D_ERROR(E_InvalidPrecondition("All shared renderer must be destroyed before"));
        }

		// unshare
        if (m_sharing) {
			m_sharing->m_shareCount--;

			if (m_sharing->m_shareCount < 0)
				O3D_ERROR(E_InvalidResult("Share counter reference is negative"));

            m_sharing = nullptr;
		}

		deletePtr(m_glContext);

        if (m_HGLRC != NULL_HGLRC) {
            if (m_state.getBit(STATE_EGL)) {
              #ifdef O3D_EGL
                EGLDisplay eglDisplay = EGL::getDisplay(EGL_DEFAULT_DISPLAY);
                EGL::makeCurrent(eglDisplay, 0, 0, 0);

                EGL::destroyContext(eglDisplay, reinterpret_cast<EGLContext>(m_HGLRC));
              #endif
            } else {
                WGL::makeCurrent((HDC)m_HDC, 0);
                WGL::deleteContext((HGLRC)m_HGLRC);
            }

            m_HGLRC = nullptr;
		}

        if (m_HDC != NULL_HDC) {
			ReleaseDC(reinterpret_cast<HWND>(m_appWindow->getHWND()), (HDC)m_HDC);
            m_HDC = nullptr;
		}

        m_depth = m_bpp = m_stencil = m_samples = 0;

        m_state.zero();
        m_version = 0;

        if (m_appWindow) {
			disconnect(m_appWindow);
            m_appWindow = nullptr;
		}

		m_glErrno = GL_NO_ERROR;
	}
}

void *Renderer::getProcAddress(const Char *ext) const
{
    if (m_state.getBit(STATE_EGL)) {
      #ifdef O3D_EGL
        return EGL::getProcAddress(ext);
     #else
        return nullptr;
      #endif
    } else {
        return WGL::getProcAddress(ext);
    }
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
      #else
        return False;
      #endif
    } else {
        return WGL::getCurrentContext() == (HGLRC)m_HGLRC;
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

        EGLDisplay eglDisplay = EGL::getDisplay(EGL_DEFAULT_DISPLAY);

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
        if (WGL::getCurrentContext() == (HGLRC)m_HGLRC) {
            return;
        }

        if (WGL::makeCurrent((HDC)m_HDC,(HGLRC)m_HGLRC) == False) {
            O3D_ERROR(E_InvalidResult("Unable to set the current OpenGL context"));
        }
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
        EGLDisplay eglDisplay = EGL::getDisplay(EGL_DEFAULT_DISPLAY);
        if (!EGL::swapInterval(eglDisplay, value)) {
            return False;
        }
      #else
        return False;
      #endif
    } else if (WGL::swapIntervalEXT) {
        if (!WGL::swapIntervalEXT(value)) {
            return False;
        }
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

#endif // O3D_WINAPI
