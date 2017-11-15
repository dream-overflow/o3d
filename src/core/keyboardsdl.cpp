/**
 * @file keyboardsdl.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/base.h"

/* ONLY IF O3D_SDL IS SELECTED */
#ifdef O3D_SDL

#include "o3d/core/keyboard.h"

#include <SDL2/SDL.h>

#include "o3d/core/debug.h"

using namespace o3d;

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
    // CapsLock
    Bool s = SDL_GetModState()&KMOD_CAPS ? True : False;
    m_modifiers.setBit(MOD_CAPSLOCK, s);

    // NumLock
    s = SDL_GetModState()&KMOD_NUM ? True : False;
    m_modifiers.setBit(MOD_NUMLOCK, s);
}

void Keyboard::setGrab(Bool exclusive)
{
	m_grab = exclusive;
}

// get string name of key
String Keyboard::getKeyName(VKey key)
{
	return String(SDL_GetKeyName(static_cast<SDL_Keycode>(key)));
}

// Update on key change (on keyboard event).
Bool Keyboard::setKeyState(VKey keySym, Bool state)
{
	// pressed mean not released and not repeated
    IT_KeySet it = m_keys.find(keySym);
	Bool found = it != m_keys.end();

	Bool pressed = !found && state ? True : False;

    // toggle insert mode
    if (state && (keySym == KEY_INSERT))
        m_modifiers.toggle((UInt32)MOD_INSERT);

    // caps lock
    if (keySym == KEY_CAPSLOCK)
    {
        Bool s = SDL_GetModState()&KMOD_CAPS ? True : False;
        m_modifiers.setBit((UInt32)MOD_CAPSLOCK, s);
    }
    // num lock
    else if (keySym == KEY_NUMLOCK)
    {
        Bool s = SDL_GetModState()&KMOD_NUM ? True : False;
        m_modifiers.setBit((UInt32)MOD_NUMLOCK, s);
    }

	// key state change
	if (!found && state)
        m_keys.insert(keySym);
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

#endif // O3D_SDL

