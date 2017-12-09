/**
 * @file keyboardandroid.cpp
 * @brief Implementation of Keyboard.h (WIN32 part).
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/keyboard.h"

/* ONLY IF O3D_ANDROID IS SELECTED */
#ifdef O3D_ANDROID

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

    if (!m_appWindow) {
        O3D_ERROR(E_InvalidParameter("Invalid application window"));
    }

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

void Keyboard::setGrab(Bool exclusive)
{
    if (!m_appWindow->getHWND()) {
        O3D_ERROR(E_InvalidOperation("Related application window must be active"));
    }

    // enable
    if (exclusive && !m_grab) {
        // Insert/replace mode
        // @todo
        m_modifiers.enable(MOD_INSERT);

        this->getKeyboardState();

        m_keys.clear();

        m_aquired = False;
        m_grab = True;
    }

    // disable
    if (!exclusive && m_grab) {
        // Insert/replace mode
        // @todo
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
    // @todo
    Bool found = False;

//    // key state change
//    if (!found && state) {
//        m_keys.insert(keySym);
//    } else if (found && !state) {
//        m_keys.erase(it);
//    }

    return False;  // pressed;
}

// get string name of key
String Keyboard::getKeyName(VKey key)
{
    if (key >= 256) {
        return String("");
    }

    String str;
    // @todo

    return String(str);
}

// get the state
void Keyboard::getKeyboardState()
{
    // caps lock
    // m_modifiers.setBit(MOD_CAPSLOCK, s);

    // num lock
    // m_modifiers.setBit(MOD_NUMLOCK, s);
}

// update input data (only if acquired)
void Keyboard::update()
{
    if (!m_isActive) {
        return;
    }

    if (!m_aquired) {
        acquire();
    }
}

// acquire keyboard hardware
void Keyboard::acquire()
{
    if (m_isActive && !m_aquired) {
        m_keys.clear();

        getKeyboardState();
        m_aquired = True;
    }
}

// release keyboard hardware
void Keyboard::release()
{
    if (m_aquired) {
        m_keys.clear();
        m_aquired = False;
    }
}

#endif // O3D_ANDROID
