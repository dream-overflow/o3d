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

// ONLY IF O3D_WIN32 IS SELECTED
#ifdef O3D_WIN32

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

// @see WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_PROFILE_MASK_ARB
// @todo complete WGL class

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

// wglGetExtensionsStringARB prototype.
typedef const char * (APIENTRYP PFNWGLGETEXTENSIONSSTRINGARBPROC) (HDC hdc);

// wglCreateContextAttribsARB prototype.
typedef HGLRC (APIENTRYP PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC, HGLRC, const int*);

#define WGL_CONTEXT_MAJOR_VERSION_ARB   0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB   0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB     0x2093
#define WGL_CONTEXT_FLAGS_ARB           0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB    0x9126

#define WGL_CONTEXT_DEBUG_BIT_ARB       0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB    0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

#define ERROR_INVALID_VERSION_ARB       0x2095
#define ERROR_INVALID_PROFILE_ARB       0x2096

static Bool isExtensionSupported(const char *extList, const char *extension)
{
	const char *start;
	const char *where, *terminator;

	// Extension names should not have spaces.
	where = strchr(extension, ' ');
	if (where || *extension == '\0')
		return False;

	// It takes a bit of care to be fool-proof about parsing the
	// OpenGL extensions string. Don't be fooled by sub-strings, etc.
	for (start = extList;;)
	{
		where = strstr(start, extension);

		if (!where)
			break;

		terminator = where + strlen(extension);

		if (where == start || *(where - 1) == ' ')
			if (*terminator == ' ' || *terminator == '\0')
				return True;

		start = terminator;
	}

	return False;
}

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

        if ((m_HGLRC = (_HGLRC)wglCreateContext((HDC)m_HDC)) == nullptr) {
            O3D_ERROR(E_InvalidResult("Unable to create the OpenGL context"));
        }

        if (sharing != nullptr) {
            if (!wglShareLists((HGLRC)sharing->getHGLRC(), (HGLRC)m_HGLRC))
                O3D_ERROR(E_InvalidResult("Unable to share the OpenGL context"));
        }

        if (wglMakeCurrent((HDC)m_HDC,(HGLRC)m_HGLRC) == False) {
            O3D_ERROR(E_InvalidResult("Unable to set the new OpenGL context as current"));
        }

        Int32 queryMajor = 1;
        Int32 queryMinor = 0;

        // we can retrieve glGetString now
        _glGetString = (PFNGLGETSTRINGPROC)wglGetProcAddress("glGetString");

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
            wglMakeCurrent((HDC)m_HDC, 0);
            wglDeleteContext((HGLRC)m_HGLRC);

            m_HDC = NULL;
            m_HGLRC = NULL;

            O3D_ERROR(E_InvalidPrecondition("OpenGL 1.2 or greater must be available"));
        }

        //
        // Create an OpenGL context with wglGetExtensionsStringARB
        //

        PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
        wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");

        PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
        wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

        const char *wglExts = nullptr;

        if (wglGetExtensionsStringARB)
            wglExts = wglGetExtensionsStringARB((HDC)m_HDC);

        // Check for the WGL_ARB_create_context extension string and the function.
        // If either is not present, use old style context creation method.
        if (!wglExts || !isExtensionSupported(wglExts, "WGL_ARB_create_context") || !wglCreateContextAttribsARB) {
            O3D_MESSAGE("WGL_ARB_create_context is not present use old style");
        } else {
            O3D_MESSAGE(String::print("Creating an OpenGL %i.%i context...", queryMajor, queryMinor));

            int flagsARB = 0;

            if (queryMajor > 2)
                flagsARB |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;

            if (debug)
                flagsARB |= WGL_CONTEXT_DEBUG_BIT_ARB;

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
            if ((m_HGLRC = (_HGLRC)wglCreateContextAttribsARB(
                     (HDC)m_HDC,
                     sharing != nullptr ? (HGLRC)sharing->getHGLRC() : 0,
                     attribList)) == NULL) {
                // Delete the previous context
                wglMakeCurrent((HDC)m_HDC, 0);
                wglDeleteContext((HGLRC)prevContext);

                O3D_ERROR(E_InvalidResult(String("Unable to create the OpenGL ") +
                                          reinterpret_cast<const Char*>(version) + " context"));
            }

            // Delete the previous context
            wglMakeCurrent((HDC)m_HDC, 0);
            wglDeleteContext((HGLRC)prevContext);

            if (wglMakeCurrent((HDC)m_HDC, (HGLRC)m_HGLRC) == False) {
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
        // @todo

    #else
        O3D_ERROR(E_UnsuportedFeature("Support for EGL is missing"));
    #endif
    } else {
        O3D_ERROR(E_UnsuportedFeature("Support for WGL or EGL only"));
    }

    GLExtensionManager::init();

	O3D_MESSAGE("Video renderer: " + getRendererName());
	O3D_MESSAGE("OpenGL version: " + getVersion());

    const GLubyte *version = _glGetString(GL_VERSION);
    if (version && (version[0] == '1')) {
		O3D_WARNING("OpenGL 2.0 or greater is not available, try to found ARB/EXT");
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

        #ifdef O3D_EGL

        #else
        if (m_HGLRC != NULL_HGLRC) {
			wglMakeCurrent((HDC)m_HDC, 0);
			wglDeleteContext((HGLRC)m_HGLRC);

            m_HGLRC = nullptr;
		}
        #endif

        if (m_HDC != NULL_HDC) {
			ReleaseDC(reinterpret_cast<HWND>(m_appWindow->getHWND()), (HDC)m_HDC);
			m_HDC = NULL;
		}

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
#ifdef O3D_EGL

#else
    // return WGL::getProcAddress(ext);
    return (void*)wglGetProcAddress((LPCSTR)ext);
#endif
}

// Is it the current OpenGL context.
Bool Renderer::isCurrent() const
{

    return (m_state.getBit(STATE_DEFINED) && (wglGetCurrentContext() == (HGLRC)m_HGLRC));
}

// Set as current OpenGL context
void Renderer::setCurrent()
{
    #ifdef O3D_EGL

    #else
    if (m_state.getBit(STATE_DEFINED) && (wglGetCurrentContext() != (HGLRC)m_HGLRC)) {
        if (wglMakeCurrent((HDC)m_HDC,(HGLRC)m_HGLRC) == False) {
			O3D_ERROR(E_InvalidResult("Unable to set the current OpenGL context"));
        }
    }
    #endif
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
        EGLDisplay eglDisplay = EGL::getDisplay(display);
        if (!EGL::swapInterval(eglDisplay, value)) {
            return False;
        }
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

#endif // O3D_WIN32 
