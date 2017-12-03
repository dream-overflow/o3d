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

#include "o3d/core/glxdefines.h"
#include "o3d/core/glx.h"

#undef Bool

#include "o3d/engine/context.h"
#include "o3d/core/appwindow.h"
#include "o3d/core/application.h"
#include "o3d/core/debug.h"

using namespace o3d;

static Bool contextErrorOccured = False;
static int contextErrorHandler(Display *display, XErrorEvent *event)
{
    contextErrorOccured = True;
    return 0;
}

static Bool isExtensionSupported(const char *extList, const char *extension)
{
	const char *start;
	const char *where, *terminator;

	// Extension names should not have spaces.
	where = strchr(extension, ' ');
    if (where || *extension == '\0') {
		return False;
    }

	// It takes a bit of care to be fool-proof about parsing the
	// OpenGL extensions string. Don't be fooled by sub-strings, etc.
    for (start = extList;;) {
		where = strstr(start, extension);

        if (!where) {
			break;
        }

		terminator = where + strlen(extension);

        if (where == start || *(where - 1) == ' ') {
            if (*terminator == ' ' || *terminator == '\0') {
				return True;
            }
        }

		start = terminator;
	}

	return False;
}

// Create the OpenGL context.
void Renderer::create(AppWindow *appWindow, Bool debug)
{
    if (m_state.getBit(STATE_DEFINED)) {
        O3D_ERROR(E_InvalidPrecondition("The renderer is already initialized"));
    }

    if (!appWindow || (appWindow->getHWND() == NULL_HWND)) {
		O3D_ERROR(E_InvalidParameter("Invalid application window"));
    }

	O3D_MESSAGE("Creating a new OpenGL context...");

	Display *display = reinterpret_cast<Display*>(Application::getDisplay());
	GLXFBConfig bestFbc = reinterpret_cast<GLXFBConfig>(appWindow->getPixelFormat());

	// Install an X error handler so the application won't exit if GL 3.0+
	// context allocation fails.
	//
	// Note this error handler is global. All display connections in all threads
	// of a process use the same error handler, so be sure to guard against other
	// threads issuing X commands while this code is running.
	contextErrorOccured = False;
	int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&contextErrorHandler);

	// OpenGL version checking
	GLXContext context = 0;

    context = GLX::createNewContext(display, bestFbc, GLX_RGBA_TYPE, 0, True);

    if (contextErrorOccured || !context) {
		// Restore the original error handler
		XSetErrorHandler(oldHandler);

		O3D_ERROR(E_InvalidResult("Unable to create the OpenGL context"));
	}

    GLX::makeCurrent(display, static_cast<GLXDrawable>(appWindow->getHDC()), context);

	Int32 queryMajor = 1;
	Int32 queryMinor = 0;

    // we can retrieve glGetString now
    glGetString = (PFNGLGETSTRINGPROC)GLX::getProcAddress("glGetString");

	const GLubyte *version = glGetString(GL_VERSION);
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

	// Get the default screen's GLX extension list
    const char *glxExts = GLX::queryExtensionsString(display, DefaultScreen(display));

	// It is not necessary to create or make current to a context before calling glXGetProcAddress
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)GLX::getProcAddress("glXCreateContextAttribsARB");

	// Check for the GLX_ARB_create_context extension string and the function.
	// If either is not present, use GLX 1.3 context creation method.
    if (!isExtensionSupported(glxExts, "GLX_ARB_create_context") || !glXCreateContextAttribsARB) {
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
                //queryMajor > 2 ? GLX_CONTEXT_PROFILE_MASK_ARB : (int)None, queryMajor > 2 ? GLX_CONTEXT_CORE_PROFILE_BIT_ARB : (int)None,
				None };

		context = glXCreateContextAttribsARB(display, bestFbc, 0, True, contextAttribs);

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

			context = glXCreateContextAttribsARB(display, bestFbc, 0, True, contextAttribs);
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

	O3D_MESSAGE("Video renderer: " + getRendererName());
	O3D_MESSAGE("OpenGL version: " + getStrVersion());

    GLExtensionManager::init();

	version = glGetString(GL_VERSION);
    if (version && (version[0] == '1')) {
		O3D_WARNING("OpenGL 2.0 or greater is not available, try to found ARB/EXT");
    }

	// compute the gl version
	Int32 glVersion = (version[0] - '0') * 100 + (version[2] - '0') * 10;
	if (glVersion > OGL_420)
		glVersion = OGL_420;

	m_version = Version(glVersion);

	m_appWindow = appWindow;

	m_bpp = appWindow->getBpp();
	m_depth = appWindow->getDepth();
    m_stencil = appWindow->getStencil();
    m_samples = appWindow->getSamples();

    if (debug) {
        initDebug();
    }

	m_glContext = new Context(this);

	doAttachment(m_appWindow);
}

// Share the OpenGL rendering using a given window handle. Same as create but with sharing.
void Renderer::share(Renderer *sharing, AppWindow *appWindow, Bool debug)
{
    if (!sharing) {
		O3D_ERROR(E_InvalidParameter("Sharing renderer must be valid"));
    }

    if (m_sharing) {
		O3D_ERROR(E_InvalidOperation("A shared renderer cannot be sharing"));
    }

    if (m_state.getBit(STATE_DEFINED)) {
		O3D_ERROR(E_InvalidOperation("The renderer is already initialized"));
    }

    if (!appWindow || (appWindow->getHWND() == NULL_HWND)) {
		O3D_ERROR(E_InvalidParameter("Invalid application window"));
    }

	O3D_MESSAGE("Creating a new OpenGL context...");

	Display *display = reinterpret_cast<Display*>(Application::getDisplay());
	GLXFBConfig bestFbc = reinterpret_cast<GLXFBConfig>(appWindow->getPixelFormat());

	// Install an X error handler so the application won't exit if GL 3.0+
	// context allocation fails.
	//
	// Note this error handler is global. All display connections in all threads
	// of a process use the same error handler, so be sure to guard against other
	// threads issuing X commands while this code is running.
	contextErrorOccured = False;
	int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&contextErrorHandler);

	// OpenGL version checking
	GLXContext context = 0;

    context = GLX::createNewContext(
			display,
			bestFbc,
			GLX_RGBA_TYPE,
			reinterpret_cast<GLXContext> (sharing->m_HGLRC),
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
    glGetString = (PFNGLGETSTRINGPROC)GLX::getProcAddress("glGetString");

	const GLubyte *version = glGetString(GL_VERSION);
    if (version && (version[0] == '3')) {
		// try an OpenGL 3.0+ context.
		queryMajor = 3;
		queryMinor = version[2] - '0';
    } else if (version && (version[0] == '4')) {
		// try an OpenGL 4.0+ context.
		queryMajor = 4;
		queryMinor = version[2] - '0';
    } else if (version && (version[0] > '4')) {
		// try an OpenGL 4.2 context.
		queryMajor = 4;
		queryMinor = 2;
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

	// Get the default screen's GLX extension list
    const char *glxExts = GLX::queryExtensionsString(display, DefaultScreen(display));

	// It is not necessary to create or make current to a context before calling glXGetProcAddress
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)GLX::getProcAddress("glXCreateContextAttribsARB");

	// Check for the GLX_ARB_create_context extension string and the function.
	// If either is not present, use GLX 1.3 context creation method.
    if (!isExtensionSupported(glxExts, "GLX_ARB_create_context") || !glXCreateContextAttribsARB) {
		O3D_MESSAGE("GLX_ARB_create_context is not present use old style");
    } else {
        GLX::destroyContext(display, context);
		context = 0;

		O3D_MESSAGE(String::print("Creating an OpenGL %i.%i context...", queryMajor, queryMinor));

        int flagsARB = 0;

        if (queryMajor > 2)
            flagsARB |= GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;

        if (debug)
            flagsARB |= GLX_CONTEXT_DEBUG_BIT_ARB;

		int contextAttribs[] = {
				GLX_CONTEXT_MAJOR_VERSION_ARB, queryMajor,
				GLX_CONTEXT_MINOR_VERSION_ARB, queryMinor,
                GLX_CONTEXT_FLAGS_ARB, flagsARB,
				//queryMajor > 2 ? GLX_CONTEXT_PROFILE_MASK_ARB : (int)None, queryMajor > 2 ? GLX_CONTEXT_CORE_PROFILE_BIT_ARB : (int)None,
				None };

		context = glXCreateContextAttribsARB(
				display,
				bestFbc,
				reinterpret_cast<GLXContext>(sharing->m_HGLRC),
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

			context = glXCreateContextAttribsARB(
					display,
					bestFbc,
					reinterpret_cast<GLXContext>(sharing->m_HGLRC),
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

	O3D_MESSAGE("Video renderer: " + getRendererName());
	O3D_MESSAGE("OpenGL version: " + getStrVersion());

    GLExtensionManager::init();

	version = glGetString(GL_VERSION);
    if (version && (version[0] == '1')) {
		O3D_WARNING("OpenGL 2.0 or greater is not available, try to found ARB/EXT");
    }

	// compute the gl version
	Int32 glVersion = (version[0] - '0') * 100 + (version[2] - '0') * 10;
    if (glVersion > OGL_450) {
        glVersion = OGL_450;
    }

    m_version = Version(glVersion);

	m_appWindow = appWindow;

	m_bpp = appWindow->getBpp();
	m_depth = appWindow->getDepth();
    m_stencil = appWindow->getStencil();
    m_samples = appWindow->getSamples();

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
            GLX::makeCurrent(
					reinterpret_cast<Display*>(Application::getDisplay()),
					None,
					NULL);
            GLX::destroyContext(
					reinterpret_cast<Display*>(Application::getDisplay()),
					reinterpret_cast<GLXContext>(m_HGLRC));

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

// Is it the current OpenGL context.
Bool Renderer::isCurrent() const
{
    return (m_state.getBit(STATE_DEFINED) &&
            (GLX::getCurrentContext() == reinterpret_cast<GLXContext>(m_HGLRC)));
}

// Set as current OpenGL context
void Renderer::setCurrent()
{
    if (m_state.getBit(STATE_DEFINED) &&
        (m_appWindow != nullptr) &&
        (GLX::getCurrentContext() != reinterpret_cast<GLXContext>(m_HGLRC))) {

        if (GLX::makeCurrent(
				reinterpret_cast<Display*>(Application::getDisplay()),
				static_cast<GLXDrawable>(m_HDC),
                reinterpret_cast<GLXContext>(m_HGLRC)) == False) {

			O3D_ERROR(E_InvalidResult("Unable to set the current OpenGL context"));
		}
	}
}

void Renderer::setVerticalRefresh(Bool use)
{
    static glXSwapIntervalEXTProc glXSwapIntervalEXT = nullptr;

    if (glXSwapIntervalEXT == nullptr) {
        glXSwapIntervalEXT = (glXSwapIntervalEXTProc)GLX::getProcAddress("glXSwapIntervalEXT");
	}

    if ((m_HDC != NULL_HDC) && (glXSwapIntervalEXT != nullptr))	{
		glXSwapIntervalEXT(
			reinterpret_cast<Display*>(Application::getDisplay()),
			static_cast<GLXDrawable>(m_HDC),
			use ? 1 : 0);
	}
}

Bool Renderer::isVerticalRefresh() const
{
	unsigned int value = 0;

    if (m_HDC != NULL_HDC) {
        GLX::queryDrawable(
			reinterpret_cast<Display*>(Application::getDisplay()),
			static_cast<GLXDrawable>(m_HDC),
			GLX_SWAP_INTERVAL_EXT,
			&value);
	}

	return value == 1;
}

#endif // O3D_X11
