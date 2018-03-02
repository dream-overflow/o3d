/**
 * @file rendererx11.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/renderer.h"

// ONLY IF O3D_X11 IS SELECTED
#ifdef O3D_X11

#include "o3d/engine/glextdefines.h"
#include "o3d/engine/glextensionmanager.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "o3d/core/gl.h"

#include "o3d/core/private/glxdefines.h"
#include "o3d/core/private/glx.h"

#undef Bool

#include "o3d/engine/context.h"
#include "o3d/core/appwindow.h"
#include "o3d/core/application.h"
#include "o3d/core/debug.h"

#ifdef O3D_EGL
  #include "o3d/core/private/egldefines.h"
  #include "o3d/core/private/egl.h"
#endif

using namespace o3d;

static Bool contextErrorOccured = False;
static int contextErrorHandler(Display *display, XErrorEvent *event)
{
    contextErrorOccured = True;
    return 0;
}

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

	O3D_MESSAGE("Creating a new OpenGL context...");

    Display *display = reinterpret_cast<Display*>(Application::getDisplay());

    //
    // GLX implementation
    //
    if (GL::getImplementation() == GL::IMPL_GLX) {
        // Install an X error handler so the application won't exit if GL 3.0+
        // context allocation fails.
        // Note this error handler is global. All display connections in all threads
        // of a process use the same error handler, so be sure to guard against other
        // threads issuing X commands while this code is running.
        contextErrorOccured = False;
        int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&contextErrorHandler);

        GLXFBConfig bestFbc = reinterpret_cast<GLXFBConfig>(appWindow->getPixelFormat());

        // OpenGL version checking
        GLXContext context = 0;
        context = GLX::createNewContext(
                      display,
                      bestFbc,
                      GLX_RGBA_TYPE,
                      sharing != nullptr ? reinterpret_cast<GLXContext>(sharing->m_HGLRC) : 0,
                      True);

        if (contextErrorOccured || !context) {
            // Restore the original error handler
            XSetErrorHandler(oldHandler);

            O3D_ERROR(E_InvalidResult("Unable to create the OpenGL context"));
        }

        GLX::makeCurrent(display, static_cast<GLXDrawable>(appWindow->getHDC()), context);

        Int32 queryMajor = 1;
        Int32 queryMinor = 0;

        // we can retrieve glGetString now
        _glGetString = (PFNGLGETSTRINGPROC)GLX::getProcAddress("glGetString");

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
            GLX::destroyContext(display, context);

            // Restore the original error handler
            XSetErrorHandler(oldHandler);

            O3D_ERROR(E_InvalidPrecondition("OpenGL 1.2 or greater must be available"));
        }

        //
        // Create an OpenGL context with glXCreateContextAttribsARB
        //

        // It is not necessary to create or make current to a context before calling glXGetProcAddress
        // Check for the GLX_ARB_create_context extension string and the function.
        // If either is not present, use GLX 1.3 context creation method.
        if (!GLX::isExtensionSupported("GLX_ARB_create_context") || !GLX::createContextAttribsARB) {
            O3D_MESSAGE("GLX_ARB_create_context is not present use old style");
        } else {
            GLX::destroyContext(display, context);
            context = 0;

            O3D_MESSAGE(String::print("Creating an OpenGL %i.%i context...", queryMajor, queryMinor));

            int flagsARB = 0;

            if (queryMajor > 2) {
                flagsARB |= GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
            }

            if (debug) {
                flagsARB |= GLX_CONTEXT_DEBUG_BIT_ARB;
            }

            int contextAttribs[] = {
                GLX_CONTEXT_MAJOR_VERSION_ARB, queryMajor,
                GLX_CONTEXT_MINOR_VERSION_ARB, queryMinor,
                GLX_CONTEXT_FLAGS_ARB, flagsARB,
                queryMajor > 2 ? GLX_CONTEXT_PROFILE_MASK_ARB : (int)None, queryMajor > 2 ? GLX_CONTEXT_CORE_PROFILE_BIT_ARB : (int)None,
                None };

            context = GLX::createContextAttribsARB(
                          display,
                          bestFbc,
                          sharing != nullptr ? reinterpret_cast<GLXContext>(sharing->m_HGLRC) : 0,
                          True,
                          contextAttribs);

            // Sync to ensure any errors generated are processed.
            XSync(display, False);

            if (contextErrorOccured) {
                // Couldn't create GL 3.0+ context.  Fall back to old-style 2.x context.
                // When a context version below 3.0 is requested, implementations will
                // return the newest context version compatible with OpenGL versions less
                // than version 3.0.
                // GLX_CONTEXT_MAJOR_VERSION_ARB = 1
                contextAttribs[1] = 1;
                // GLX_CONTEXT_MINOR_VERSION_ARB = 0
                contextAttribs[3] = 0;
                // NO GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
                contextAttribs[5] = 0;

                contextErrorOccured = False;

                O3D_MESSAGE(
                            String::print("Failed to create an OpenGL %i.%i context... using old-style GLX context",
                                          queryMajor,
                                          queryMinor));

                context = GLX::createContextAttribsARB(
                              display,
                              bestFbc,
                              sharing != nullptr ? reinterpret_cast<GLXContext>(sharing->m_HGLRC) : 0,
                              True,
                              contextAttribs);
            }
        }

        // Sync to ensure any errors generated are processed.
        XSync(display, False);

        // Restore the original error handler
        XSetErrorHandler(oldHandler);

        if (contextErrorOccured || !context) {
            O3D_ERROR(E_InvalidResult("Unable to create the OpenGL context"));
        }

        // Verifying that context is a direct context
        if (!GLX::isDirect(display, context)) {
            O3D_MESSAGE("Indirect GLX rendering context obtained");
        } else {
            O3D_MESSAGE("Direct GLX rendering context obtained");
        }

        GLX::makeCurrent(display, static_cast<GLXDrawable>(appWindow->getHDC()), context);

        m_HDC = appWindow->getHDC();
        m_HGLRC = reinterpret_cast<_HGLRC>(context);
        m_state.enable(STATE_DEFINED);
    } else if (GL::getImplementation() == GL::IMPL_EGL) {
        //
        // EGL implementation
        //

    #ifdef O3D_EGL
        EGLDisplay eglDisplay = EGL::getDisplay(display);
        EGLSurface eglSurface = reinterpret_cast<EGLSurface>(appWindow->getHDC());
        EGLConfig eglConfig = reinterpret_cast<EGLConfig>(appWindow->getPixelFormat());

        EGLint contextAttributes[] = {
            /*EGL_CONTEXT_MAJOR_VERSION_KHR*/EGL_CONTEXT_CLIENT_VERSION, 3,
            /* EGL_CONTEXT_MINOR_VERSION_KHR, 2,*/
            debug ? EGL_CONTEXT_FLAGS_KHR : EGL_NONE, debug ? EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR : EGL_NONE,
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
        O3D_ERROR(E_UnsuportedFeature("Support for GLX or EGL only"));
    }

    GLExtensionManager::init();

	O3D_MESSAGE("Video renderer: " + getRendererName());
    O3D_MESSAGE("OpenGL version: " + getVersionName());

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
            Display *display = reinterpret_cast<Display*>(Application::getDisplay());

            if (m_state.getBit(STATE_EGL)) {
            #ifdef O3D_EGL
                EGLDisplay eglDisplay = EGL::getDisplay(display);
                EGL::makeCurrent(eglDisplay, 0, 0, 0);

                EGL::destroyContext(eglDisplay, reinterpret_cast<EGLContext>(m_HGLRC));
            #endif
            } else {
                GLX::makeCurrent(display, None, nullptr);
                GLX::destroyContext(display, reinterpret_cast<GLXContext>(m_HGLRC));
            }

			m_HGLRC = NULL_HGLRC;
		}

		m_HDC = NULL_HDC;

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
        return GLX::getProcAddress(ext);
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
        return GLX::getCurrentContext() == reinterpret_cast<GLXContext>(m_HGLRC);
    }
}

// Set as current OpenGL context
void Renderer::setCurrent()
{
    if (!m_state.getBit(STATE_DEFINED)) {
        return;
    }

    Display *display = reinterpret_cast<Display*>(Application::getDisplay());

    if (m_state.getBit(STATE_EGL)) {
      #ifdef O3D_EGL
        if (EGL::getCurrentContext() == reinterpret_cast<EGLContext>(m_HGLRC)) {
            return;
        }

        EGLDisplay eglDisplay = EGL::getDisplay(display);

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
        if (GLX::getCurrentContext() == reinterpret_cast<GLXContext>(m_HGLRC)) {
            return;
        }

        if (GLX::makeCurrent(
                display,
                static_cast<GLXDrawable>(m_HDC),
                reinterpret_cast<GLXContext>(m_HGLRC)) == False) {
            O3D_ERROR(E_InvalidResult("Unable to set the current OpenGL context"));
        }
	}
}

Bool Renderer::setVSyncMode(VSyncMode mode)
{
    if (!m_state.getBit(STATE_DEFINED)) {
        return False;
    }

    Display *display = reinterpret_cast<Display*>(Application::getDisplay());

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
        EGLDisplay eglDisplay = EGL::getDisplay(display);
        if (!EGL::swapInterval(eglDisplay, value)) {
            return False;
        }
      #else
        return False;
      #endif
    } else if (GLX::swapIntervalEXT) {
        GLX::swapIntervalEXT(
                    display,
                    static_cast<GLXDrawable>(m_HDC),
                    value);
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

//	unsigned int value = 0;
//    Display *display = reinterpret_cast<Display*>(Application::getDisplay());
//
//    // Query is only supported by GLX
//    if (!m_state.getBit(STATE_EGL)) {
//         GLX::queryDrawable(
//            display,
//			static_cast<GLXDrawable>(m_HDC),
//			GLX_SWAP_INTERVAL_EXT,
//			&value);
//	}
//	return value == 1;

#endif // O3D_X11
