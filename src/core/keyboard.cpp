/**
 * @file keyboard.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/keyboard.h"

#include "o3d/core/architecture.h"
#include "o3d/core/debug.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Keyboard, CORE_KEYBOARD, Input)

Keyboard::Keyboard(BaseObject *parent) :
	Input(parent)
{
}

Input::InputType Keyboard::getInputType() const
{
    return INPUT_KEYBOARD;
}

void Keyboard::clear()
{
	m_keys.clear();
}

#if defined(O3D_DUMMY)
Keyboard::Keyboard(AppWindow *appWindow) :
        Input(),
        m_grab(False),
        m_appWindow(appWindow)
{
    m_name = "Keyboard";

    m_isActive = False;

    m_keys.clear();

    if (!m_appWindow) {
        O3D_ERROR(E_InvalidParameter("Invalid application window"));
    }

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

void Keyboard::setGrab(Bool)
{

}

void Keyboard::acquire()
{

}

void Keyboard::release()
{

}

void Keyboard::update()
{

}
#endif // #if O3D_DUMMY
