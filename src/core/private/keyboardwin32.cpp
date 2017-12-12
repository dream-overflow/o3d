/**
 * @file keyboardwin32.cpp
 * @brief Implementation of Keyboard.h (WIN32 part).
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-09-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/keyboard.h"

/* ONLY IF O3D_WINAPI IS SELECTED */
#ifdef O3D_WINAPI

#include "o3d/core/architecture.h"
#include "o3d/core/debug.h"
#include "o3d/core/appwindow.h"

using namespace o3d;

// constructor
Keyboard::Keyboard(AppWindow *appWindow) :
		Input(),
		m_grab(False),
		m_appWindow(appWindow)
{
	m_name = "Keyboard";

	m_isActive = True;

	if (!m_appWindow)
		O3D_ERROR(E_InvalidParameter("Invalid application window"));

    // Insert/replace mode
    m_modifiers.enable(MOD_INSERT);

	m_keys.clear();

	m_aquired = False;
	acquire();
}

// destructor
Keyboard::~Keyboard()
{
	setGrab(False);
	release();
}

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_KEYBOARD
#define HID_USAGE_GENERIC_KEYBOARD     ((USHORT) 0x06)
#endif

void Keyboard::setGrab(Bool exclusive)
{
	if (!m_appWindow->getHWND())
		O3D_ERROR(E_InvalidOperation("Related application window must be active"));

	// enable
	if (exclusive && !m_grab)
	{
		// No legacy because we want WM_CHAR/WM_UNICHAR messages
		RAWINPUTDEVICE Rid[1];
		Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; 
		Rid[0].usUsage = HID_USAGE_GENERIC_KEYBOARD; 
		Rid[0].dwFlags = /*RIDEV_NOLEGACY |*/ RIDEV_INPUTSINK/* | RIDEV_APPKEYS*/ | RIDEV_NOHOTKEYS;
		Rid[0].hwndTarget = reinterpret_cast<HWND>(m_appWindow->getHWND());
		RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

        // Insert/replace mode
        m_modifiers.enable(MOD_INSERT);

		this->getKeyboardState();

		m_keys.clear();

		m_aquired = False;
		m_grab = True;
	}
	
	// disable
	if (!exclusive && m_grab)
	{
		RAWINPUTDEVICE Rid[1];
		Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; 
		Rid[0].usUsage = HID_USAGE_GENERIC_KEYBOARD; 
		Rid[0].dwFlags = RIDEV_REMOVE;
		Rid[0].hwndTarget = reinterpret_cast<HWND>(m_appWindow->getHWND());
		RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

        // Insert/replace mode
        m_modifiers.enable(MOD_INSERT);

		this->getKeyboardState();

		m_keys.clear();

		m_aquired = False;
		m_grab = False;
	}
}

// Update on key change (on keyboard event).
Bool Keyboard::setKeyState(VKey keySym, Bool state)
{
	// left or right alt
    if (keySym == VK_MENU)
	{
		if (LOBYTE(GetAsyncKeyState(VK_LMENU)))
            keySym = KEY_LALT;
		else if (LOBYTE(GetAsyncKeyState(VK_RMENU)))
            keySym = KEY_RALT;
	}
	// left or right shift
    else if (keySym == VK_SHIFT)
	{
		if (LOBYTE(GetAsyncKeyState(VK_LSHIFT)))
            keySym = KEY_LSHIFT;
		else if (LOBYTE(GetAsyncKeyState(VK_RSHIFT)))
            keySym = KEY_RSHIFT;
	}
	// left or right control
    else if (keySym == VK_CONTROL)
	{
		if (LOBYTE(GetAsyncKeyState(VK_LCONTROL)))
            keySym = KEY_LCONTROL;
		else if (LOBYTE(GetAsyncKeyState(VK_RCONTROL)))
            keySym = KEY_RCONTROL;
	}

    // pressed mean not released and not repeated
    IT_KeySet it = m_keys.find(keySym);
    Bool found = it != m_keys.end();

    Bool pressed = !found && state ? True : False;

    // toggle insert mode
    if (state && (keySym == KEY_INSERT))
        m_modifiers.toggle((UInt32)MOD_INSERT);

    // caps lock
    if ((keySym == VK_CAPITAL) && pressed)
    {
        SHORT ks = GetKeyState(VK_CAPITAL);

        if (!LOBYTE(ks) && state)
            m_modifiers.enable((UInt32)MOD_CAPSLOCK);
        else if (state)
            m_modifiers.disable((UInt32)MOD_CAPSLOCK);
    }
    // num lock
    else if ((keySym == VK_NUMLOCK) && pressed)
    {
        SHORT ks = GetKeyState(VK_NUMLOCK);

        if (!LOBYTE(ks) && state)
            m_modifiers.enable((UInt32)MOD_NUMLOCK);
        else if (state)
            m_modifiers.disable((UInt32)MOD_NUMLOCK);
    }

	// key state change
	if (!found && state)
        m_keys.insert(keySym);
	else if (found && !state)
		m_keys.erase(it);

	return pressed;
}

// get string name of key
String Keyboard::getKeyName(VKey key)
{
	if (key >= 256)
		return String("");

	WChar str[16] = { 0 };
	LONG lParam = (key << 16);
	GetKeyNameTextW(lParam, str, 16);

	return String(str);
}

// get the state
void Keyboard::getKeyboardState()
{
	// caps lock
	SHORT ks = GetKeyState(VK_CAPITAL);
    Bool s = LOBYTE(ks) ? True : False;
    m_modifiers.setBit(MOD_CAPSLOCK, s);

	// num lock
	ks = GetKeyState(VK_NUMLOCK);
    s = LOBYTE(ks) ? True : False;
    m_modifiers.setBit(MOD_NUMLOCK, s);
}

// update input data (only if acquired)
void Keyboard::update()
{
	if (!m_isActive)
		return;

	if (!m_aquired)
		acquire();
}

// acquire keyboard hardware
void Keyboard::acquire()
{
	if (m_isActive && !m_aquired)
	{
		m_keys.clear();

		getKeyboardState();
		m_aquired = True;
	}
}

// release keyboard hardware
void Keyboard::release()
{
	if (m_aquired)
	{
		m_keys.clear();
		m_aquired = False;
	}
}

#endif // O3D_WINAPI
