/**
 * @file mousex11.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/base.h"

// ONLY IF O3D_X11 IS SELECTED
#ifdef O3D_X11

#include <math.h>
#include "o3d/core/mouse.h"
#include "o3d/core/debug.h"
#include "o3d/core/appwindow.h"
#include "o3d/core/application.h"

#include <X11/Xlib.h>

#undef Bool

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

	if (!m_appWindow)
		O3D_ERROR(E_InvalidParameter("Invalid application window"));

	// for windowed mode, start with current absolute mouse position
	if (!lock)
	{
		Display *display = reinterpret_cast<Display*>(Application::getDisplay());
		Window rootWin, childWin;
		unsigned int mask;

		Int32 rootX, rootY, x = 0, y = 0;
		XQueryPointer(
				display,
				static_cast<Window>(appWindow->getHWND()),
				&rootWin,
				&childWin,
				&rootX,
				&rootY,
				&x,
				&y,
				&mask);

		m_windowPos = m_posNoAccel = m_pos = m_oldPos = Vector2i(x, y);
		m_windowPos = m_window.clamp(m_windowPos);

		m_grab = False;
	}
	else
		setGrab();

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
	if (!m_cursor)
	{
		if (!m_appWindow->getHWND())
			O3D_ERROR(E_InvalidOperation("Related application window must be active"));

		Display *display = reinterpret_cast<Display*>(Application::getDisplay());
		Window window = static_cast<Window>(m_appWindow->getHWND());

		// restore the XC_left_ptr font cursor
//		Cursor cursor;
//		cursor = XCreateFontCursor(display, XC_left_ptr/*68*/);
//		XDefineCursor(display, window, cursor);
//		XFreeCursor(display, cursor);

		// simple restore
		XUndefineCursor(display, window);

		m_cursor = True;
	}
}

// hide cursor drawing state
void Mouse::disableCursor()
{
	if (m_cursor)
	{
		if (!m_appWindow->getHWND())
			O3D_ERROR(E_InvalidOperation("Related application window must be active"));

		Display *display = reinterpret_cast<Display*>(Application::getDisplay());
		Window window = static_cast<Window>(m_appWindow->getHWND());

		// create an invisible cursor and define it
		Cursor invisibleCursor;
		Pixmap bitmapNoData;
		XColor black;
		static char noData[] = { 0,0,0,0,0,0,0,0 };
		black.red = black.green = black.blue = 0;

		bitmapNoData = XCreateBitmapFromData(display, window, noData, 8, 8);
		invisibleCursor = XCreatePixmapCursor(
					display, bitmapNoData, bitmapNoData, &black, &black, 0, 0);
		XDefineCursor(display, window, invisibleCursor);
		XFreeCursor(display, invisibleCursor);
        XFreePixmap(display, bitmapNoData);

		m_cursor = False;
	}
}

// lock the mouse position
void Mouse::setGrab(Bool lock)
{
	if (!m_appWindow->getHWND())
		O3D_ERROR(E_InvalidOperation("Related application window must be active"));

	// enable
	if (!m_grab && lock)
	{
		Display *display = reinterpret_cast<Display*>(Application::getDisplay());
		Window window = static_cast<Window>(m_appWindow->getHWND());

		/*int result = */XGrabPointer(
			display,
			window,
			False,
			ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
			GrabModeAsync,
			GrabModeAsync,
			window,
			None,
			CurrentTime);

        //XWarpPointer(display, None, window, 0, 0, 0, 0, 0, 0);

		m_grab = True;
	}

	// disable
	if (m_grab && !lock)
	{
		Display *display = reinterpret_cast<Display*>(Application::getDisplay());

		XUngrabPointer(display, CurrentTime);

		m_grab = False;
	}
}

// update input data (only if acquired)
void Mouse::update()
{
	if (!m_isActive)
		return;

	if (!m_aquired)
		acquire();

	// update the wheel speed
	wheelUpdate();

    // must update the mouse smoother to have a correct effect
    updateSmoother(-1.f);
}

// acquire mouse position and buttons states
void Mouse::acquire()
{
	if (m_isActive && !m_aquired)
	{
		Display *display = reinterpret_cast<Display*>(Application::getDisplay());
		Window rootWin, childWin;
		unsigned int mask;

		// if the mouse was previously grabed, force to grab
		if (m_grab)
		{
			m_grab = False;
			setGrab();
		}

		Int32 rootX, rootY, x = 0, y = 0;
		XQueryPointer(
				display,
				static_cast<Window>(m_appWindow->getHWND()),
				&rootWin,
				&childWin,
				&rootX,
				&rootY,
				&x,
				&y,
				&mask);

		// reset to current mouse pos when the mouse is grabbed
		if (m_grab)
		{
			m_windowPos.set(x, y);
			m_pos = m_oldPos = m_posNoAccel = m_windowPos;
			m_deltaPos.zero();
		}

		m_wheel = 0; // initial wheel position

		m_windowPos = m_window.clamp(m_windowPos);

		m_aquired = True;
	}
}

// un-acquire mouse hardware
void Mouse::release()
{
	if (m_aquired)
	{
		m_wheel = 0; // wheel position is reset

		m_aquired = False;
	}
}

#endif // O3D_X11

