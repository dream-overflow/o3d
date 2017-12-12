/**
 * @file mousesdl.cpp
 * @brief Implementation of Mouse.h (SDL part).
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-09-05
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/base.h"

// ONLY IF O3D_SDL2 IS SELECTED
#ifdef O3D_SDL2

#include <math.h>

#include "o3d/core/mouse.h"
#include "o3d/core/debug.h"
#include "o3d/core/appwindow.h"

#include <SDL2/SDL.h>

using namespace o3d;

// Constructor
Mouse::Mouse(
	AppWindow *appWindow,
	Int32 xlimit,
	Int32 ylimit,
	Bool lock) :
		Input(),
		m_appWindow(appWindow),
		m_grab(False)
{
	commonInit(xlimit, ylimit);

    if (!m_appWindow) {
		O3D_ERROR(E_InvalidParameter("Invalid application window"));
    }

	// for windowed mode, start with current absolute mouse position
    if (!lock) {
		Int32 x, y;
		SDL_GetMouseState(&x, &y);

		m_windowPos = m_posNoAccel = m_pos = m_oldPos = Vector2i(x, y);
		m_windowPos = m_window.clamp(m_windowPos);

		m_grab = False;
    } else {
		setGrab();
    }

	m_isActive = True;
	m_aquired = False;
	m_grab = lock;

	disableCursor();
}

// destructor
Mouse::~Mouse()
{
	setGrab(False);
	enableCursor();
}

// draw cursor drawing state
void Mouse::enableCursor()
{
    if (!m_cursor) {
		SDL_ShowCursor(1);
		m_cursor = True;
	}
}

// hide cursor drawing state
void Mouse::disableCursor()
{
    if (m_cursor) {
		SDL_ShowCursor(0);
		m_cursor = False;
	}
}

// lock the mouse position
void Mouse::setGrab(Bool lock)
{
	// enable
    if (!m_grab && lock) {
        SDL_SetRelativeMouseMode(SDL_TRUE);
		SDL_SetWindowGrab(reinterpret_cast<SDL_Window*>(m_appWindow->getHWND()), SDL_TRUE);
		m_grab = True;
	}

	// disable
    if (m_grab && !lock) {
        SDL_SetRelativeMouseMode(SDL_FALSE);
		SDL_SetWindowGrab(reinterpret_cast<SDL_Window*>(m_appWindow->getHWND()), SDL_FALSE);
		m_grab = False;
	}
}

// update input data (only if acquired)
void Mouse::update()
{
    if (!m_isActive) {
		return;
    }

    if (!m_aquired) {
		acquire();
    }

	// update the wheel speed
	wheelUpdate();

    // must update the mouse smoother to have a correct effect
    updateSmoother(-1.f);

    // if (m_grab) {
    //     wrapPrivate();
    // }
}

void Mouse::wrapPrivate()
{
    // make mouse infinite
    if (m_grab) {
        // nothing todo with relative mode
    }
}

// acquire mouse position and buttons states
void Mouse::acquire()
{
    if (m_isActive && !m_aquired) {
		Int32 x, y;
		SDL_GetMouseState(&x, &y);

		// cursor outside of the window so not acquiered
        //if ((x < 0) || (y < 0)) {
		//	return;
        // }

		m_windowPos.set(x, y);

		m_pos = m_oldPos = m_posNoAccel = m_windowPos;
		m_deltaPos.zero();

		m_wheel = 0; // initial wheel position

		m_windowPos = m_window.clamp(m_windowPos);

		m_aquired = True;
	}
}

// un-acquire mouse hardware
void Mouse::release()
{
    if (m_aquired) {
		m_wheel = 0; // wheel position is reset

		m_aquired = False;
	}
}

#endif // O3D_SDL2
