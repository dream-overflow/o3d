/**
 * @file mousewin32.cpp
 * @brief Implementation of Mouse.h (WIN32 part).
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-09-05
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/mouse.h"

// ONLY IF O3D_WIN32 IS SELECTED
#ifdef O3D_WIN32

#include <math.h>
#include "o3d/core/architecture.h"
#include "o3d/core/debug.h"
#include "o3d/core/appwindow.h"

using namespace o3d;

// constructor
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
		POINT mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(reinterpret_cast<HWND>(m_appWindow->getHWND()), &mousePos);

		m_windowPos = m_posNoAccel = m_pos = m_oldPos = Vector2i(mousePos.x, mousePos.y);
		m_windowPos = m_window.clamp(m_windowPos);

		m_grab = False;
	}
	else
		setGrab();

	m_isActive = True;
	m_aquired = False;
}

// destructor
Mouse::~Mouse()
{
	setGrab(False);
	release();
}

// draw/hide cursor drawing state
void Mouse::enableCursor()
{
	if (!m_cursor)
	{
		ShowCursor(True);
		m_cursor = True;
	}
}

void Mouse::disableCursor()
{
	if (m_cursor)
	{
		ShowCursor(False);
		m_cursor = False;
	}
}

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

// lock/unlock the mouse position
void Mouse::setGrab(Bool lock)
{
	if (!m_appWindow->getHWND())
		O3D_ERROR(E_InvalidOperation("Related application window must be active"));

	// enable
	if (!m_grab && lock)
	{
		RAWINPUTDEVICE Rid[1];
		Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; 
		Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE; 
		Rid[0].dwFlags = RIDEV_NOLEGACY | RIDEV_INPUTSINK | RIDEV_CAPTUREMOUSE;   
		Rid[0].hwndTarget = reinterpret_cast<HWND>(m_appWindow->getHWND());
		RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

		m_posNoAccel.zero();
		m_pos.zero();
		m_oldPos.zero();
		m_deltaPos.zero();

		m_smoother.reset();

		m_wheel = 0;
		m_wheelOld = 0;
		m_wheelSpeed = 0;
		m_wheelLastTime = 0;
		m_wheelOldDir = 0;

		m_grab = True;
		ShowCursor(False);
	}
	
	// disable
	if (m_grab && !lock)
	{
		RAWINPUTDEVICE Rid[1];
		Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; 
		Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE; 
		Rid[0].dwFlags = RIDEV_REMOVE;
		Rid[0].hwndTarget = reinterpret_cast<HWND>(m_appWindow->getHWND());
		RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

		POINT mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(reinterpret_cast<HWND>(m_appWindow->getHWND()), &mousePos);

		m_deltaPos.zero();
		m_smoother.reset();

		m_windowPos = m_posNoAccel = m_pos = m_oldPos = Vector2i(mousePos.x, mousePos.y);
		m_windowPos = m_window.clamp(m_windowPos);
		
		m_grab = False;
		ShowCursor(True);
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
		POINT point;
		GetCursorPos(&point);

		RECT rect;
		GetWindowRect(reinterpret_cast<HWND>(m_appWindow->getHWND()), &rect);

		// clamp with a [-4;+4] tolerance
		if (point.x < rect.left)
			point.x = point.x + 4;
		if (point.y < rect.top)
			point.y = point.y + 4;
		if (point.x > rect.right)
			point.x = point.x - 4;
		if (point.y > rect.bottom)
			point.y = point.y - 4;

		ScreenToClient(reinterpret_cast<HWND>(m_appWindow->getHWND()), &point);

		// cursor outside of the window so not acquiered
		if ((point.x < 0) || point.y < 0)
			return;

		m_windowPos.set(point.x, point.y);

		m_pos = m_oldPos = m_posNoAccel = m_windowPos;
		m_deltaPos.zero();

		m_windowPos = m_window.clamp(m_windowPos);

		m_aquired = True;

		m_wheel = 0; // initial wheel position
	}
}

// release mouse hardware
void Mouse::release()
{
	if (m_aquired)
	{
		m_wheel = 0; // wheel position is reset

		m_aquired = False;
	}
}

#endif // O3D_WIN32

