/**
 * @file keyboardx11.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/base.h"

/* ONLY IF O3D_X11 IS SELECTED */
#ifdef O3D_X11

#include "o3d/core/keyboard.h"
#include "o3d/core/debug.h"
#include "o3d/core/appwindow.h"
#include "o3d/core/application.h"

#include <X11/Xlib.h>
#include <X11/XKBlib.h>

#undef Bool

using namespace o3d;

static unsigned char getKeySymMask(Display *display, KeySym keySym)
{
	unsigned char result = 0;

	// Find the modifier mask corresponding to CapsLock and NumLock
	KeyCode keyCode = XKeysymToKeycode(display, keySym);
	XModifierKeymap *modmap = XGetModifierMapping(display);
	for (int i = 0; i < 8 * modmap->max_keypermod; ++i)
	{
		if (modmap->modifiermap[i] == keyCode)
		{
			result = (1 << (i / modmap->max_keypermod));
			break;
		}
	}
	XFreeModifiermap(modmap);
	return result;
}

// constructor
Keyboard::Keyboard(AppWindow *appWindow) :
		Input(),
		m_grab(False),
		m_appWindow(appWindow)
{
	m_name = "Keyboard";

	m_isActive = False;

	m_keys.clear();

	if (!m_appWindow)
		O3D_ERROR(E_InvalidParameter("Invalid application window"));

	m_aquired = False;

	m_isActive = True;

	// Insert/replace mode
    m_modifiers.enable(MOD_INSERT);

	m_aquired = False;
	acquire();
}

// destructor
Keyboard::~Keyboard()
{
	setGrab(False);
	release();
}

// get the state
void Keyboard::getKeyboardState()
{
	Display *display = reinterpret_cast<Display*>(Application::getDisplay());
	XkbStateRec state;

	XkbGetState(display, XkbUseCoreKbd, &state);

	// CapsLock
    Bool s = state.locked_mods & getKeySymMask(display, XK_Caps_Lock) ? True : False;
    m_modifiers.setBit(MOD_CAPSLOCK, s);

	// NumLock
    s = state.locked_mods & getKeySymMask(display, XK_Num_Lock) ? True : False;
    m_modifiers.setBit(MOD_NUMLOCK, s);
}

void Keyboard::setGrab(Bool grab)
{
	if (!m_appWindow->getHWND())
		O3D_ERROR(E_InvalidOperation("Related application window must be active"));

	// enable
	if (!m_grab && grab)
	{
		Display *display = reinterpret_cast<Display*>(Application::getDisplay());
		Window window = static_cast<Window>(m_appWindow->getHWND());

		XGrabKeyboard(
				display,
				window,
				True,
				GrabModeAsync,
				GrabModeAsync,
				CurrentTime);

		m_grab = True;
	}

	// disable
	if (m_grab && !grab)
	{
		Display *display = reinterpret_cast<Display*>(Application::getDisplay());

		XUngrabKeyboard(display, CurrentTime);

		m_grab = False;
	}
}

// get string name of key
String Keyboard::getKeyName(VKey key)
{
    return String(XKeysymToString(static_cast<KeySym>(key)));
}

// Update on key change (on keyboard event).
Bool Keyboard::setKeyState(VKey vkey, Bool state)
{
	// pressed mean not released and not repeated
    IT_KeySet it = m_keys.find(vkey);
	Bool found = it != m_keys.end();

	Bool pressed = !found && state ? True : False;

	// toggle insert mode
    if (state && (vkey == KEY_INSERT))
        m_modifiers.toggle((UInt32)MOD_INSERT);

    // caps lock
    if (vkey == KEY_CAPSLOCK)
    {
        Display *display = reinterpret_cast<Display*>(Application::getDisplay());
        XkbStateRec state;

        XkbGetState(display, XkbUseCoreKbd, &state);

        // CapsLock
        Bool s = state.locked_mods & getKeySymMask(display, XK_Caps_Lock) ? True : False;
        m_modifiers.setBit((UInt32)MOD_CAPSLOCK, s);
    }
    // num lock
    else if (vkey == KEY_NUMLOCK)
    {
        Display *display = reinterpret_cast<Display*>(Application::getDisplay());
        XkbStateRec state;

        XkbGetState(display, XkbUseCoreKbd, &state);

        Bool s = state.locked_mods & getKeySymMask(display, XK_Num_Lock) ? True : False;
        m_modifiers.setBit((UInt32)MOD_NUMLOCK, s);
    }

	// key state change
	if (!found && state)
        m_keys.insert(vkey);
	else if (found && !state)
		m_keys.erase(it);

	return pressed;
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
	if (!m_aquired)
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

#endif // O3D_X11

