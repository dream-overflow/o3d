/**
 * @file renderersdl.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/renderer.h"

#include "o3d/engine/context.h"

// ONLY IF O3D_SDL IS SELECTED
#ifdef O3D_SDL

#include "o3d/engine/glextensionmanager.h"

#include "o3d/core/appwindow.h"
#include "o3d/core/debug.h"

#include <SDL2/SDL.h>

using namespace o3d;

// create the renderer
void Renderer::create(AppWindow *appWindow, Bool debug)
{
    if (m_state.getBit(STATE_DEFINED))
		O3D_ERROR(E_InvalidPrecondition("The renderer is already initialized"));

	if (!appWindow || (appWindow->getHWND() == NULL_HWND))
		O3D_ERROR(E_InvalidParameter("Invalid application window"));

	O3D_MESSAGE("Creating a new OpenGL context...");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_Window *appWindowSDL = reinterpret_cast<SDL_Window*>(appWindow->getHDC());

	m_HGLRC = reinterpret_cast<_HGLRC>(SDL_GL_CreateContext(appWindowSDL));

	if (!m_HGLRC)
		O3D_ERROR(E_InvalidResult("Unable to create the OpenGL context"));

	Int32 queryMajor = 1;
	Int32 queryMinor = 0;

	const GLubyte *version = glGetString(GL_VERSION);
	if (version && (version[0] == '3'))
	{
		// try an OpenGL 3.0+ context.
		queryMajor = 3;
		queryMinor = version[2] - '0';
	}
	else if (version && (version[0] == '4'))
	{
		// try an OpenGL 4.0+ context.
		queryMajor = 4;
		queryMinor = version[2] - '0';
	}
	else if (version && (version[0] > '4'))
	{
        // try an OpenGL 4.5 context.
		queryMajor = 4;
        queryMinor = 5;
	}

	// we need at least OpenGL 1.2
	if (version && (version[0] == '1') && (version[2] < '2'))
	{
		SDL_GL_DeleteContext(reinterpret_cast<SDL_GLContext>(m_HGLRC));

		m_HGLRC = NULL;

		O3D_ERROR(E_InvalidPrecondition("OpenGL 1.2 or greater must be available"));
	}

	//
	// Create an OpenGL context with CreateContextAttribsARB
	//

	O3D_MESSAGE(String::print("Creating an OpenGL %i.%i context...", queryMajor, queryMinor));

	SDL_GL_DeleteContext(reinterpret_cast<SDL_GLContext>(m_HGLRC));

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, queryMajor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, queryMinor);

    int contextFlags = 0;

    if (queryMajor > 2)
        contextFlags |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;

    if (debug)
        contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags),

    // now create GL context
	m_HGLRC = reinterpret_cast<_HGLRC>(SDL_GL_CreateContext(appWindowSDL));

	if (m_HGLRC != NULL_HGLRC)
	{
		O3D_MESSAGE(
				String::print("Failed to create an OpenGL %i.%i context... using old-style GLX context",
				queryMajor,
				queryMinor));

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

		m_HGLRC = reinterpret_cast<_HGLRC>(SDL_GL_CreateContext(appWindowSDL));

		if (!m_HGLRC)
			O3D_ERROR(E_InvalidResult("Unable to create the OpenGL context"));
	}

    m_state.enable(STATE_DEFINED);

	O3D_MESSAGE("Video renderer: " + getRendererName());
	O3D_MESSAGE("OpenGL version: " + getVersion());

	version = glGetString(GL_VERSION);
	if (version && (version[0] == '1'))
		O3D_WARNING("OpenGL 2.0 or greater is not available, try to found ARB/EXT");

	GLExtensionManager::initialize();

	// compute the gl version
	Int32 glVersion = (version[0] - '0') * 100 + (version[2] - '0') * 10;
	if (glVersion > OGL_420)
		glVersion = OGL_420;

	m_version = Version(glVersion);
	
	m_appWindow = appWindow;

	m_HDC = appWindow->getHDC();

	m_bpp = appWindow->getBpp();
	m_depth = appWindow->getDepth();
    m_stencil = appWindow->getStencil();
    m_samples = appWindow->getSamples();

    if (debug)
        initDebug();

	m_glContext = new Context(this);

	doAttachment(m_appWindow);
}

// Share the OpenGL rendering using a given window handle. Same as create but with sharing.
void Renderer::share(Renderer *shared, AppWindow *appWindow, Bool debug)
{
	O3D_ERROR(E_InvalidOperation("Not yet supported"));
}

// delete the renderer
void Renderer::destroy()
{
    if (m_state.getBit(STATE_DEFINED))
	{
		if (m_refCount > 0)
			O3D_ERROR(E_InvalidPrecondition("Unable to destroy a referenced renderer"));

		deletePtr(m_glContext);

		SDL_GL_DeleteContext(reinterpret_cast<SDL_GLContext>(m_HGLRC));

		m_HDC = NULL_HDC;
		m_HGLRC = NULL_HGLRC;

        m_depth = m_bpp = m_stencil = m_samples = 0;

        m_state.zero();
		m_version = OGL_UNDEFINED;

		if (m_appWindow)
		{
			disconnect(m_appWindow);
			m_appWindow = NULL;
		}

		m_glErrno = GL_NO_ERROR;
	}
}

// Is it the current OpenGL context.
Bool Renderer::isCurrent() const
{
    return (m_state.getBit(STATE_DEFINED));// && (SDL_GL_GetCurrentContext() == reinterpret_cast<SDL_GLContext>(m_HGLRC)));
}

// Set as current OpenGL context
void Renderer::setCurrent()
{
//	if (m_state.getBit(STATE_DEFINED) && (SDL_GL_GetCurrentContext() != reinterpret_cast<SDL_GLContext>(m_HGLRC))
//	{
//		if (SDL_GL_MakeCurrent(
//				reinterpret_cast<SDL_GLContext>(m_HDC),
//				reinterpret_cast<SDL_GLContext>(m_HGLRC)) != 0)
//			O3D_ERROR(E_InvalidResult("Unable to set the current OpenGL context"));
//	}
}

void Renderer::setVerticalRefresh(Bool use)
{
	SDL_GL_SetSwapInterval(use ? 1 : 0);
}

Bool Renderer::isVerticalRefresh() const
{
	return SDL_GL_GetSwapInterval() == 1;
}

#endif // O3D_SDL
