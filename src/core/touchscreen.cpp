/**
 * @file touchscreen.h
 * @brief Touch screen input controller
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-12
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"

#include <math.h>
#include "o3d/core/touchscreen.h"
#include "o3d/core/debug.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(TouchScreen, CORE_TOUCHSCREEN, Input)

// Default constructor.
TouchScreen::TouchScreen(BaseObject *parent) :
    Input(parent)
{
}

void TouchScreen::commonInit(Int32 xlimit, Int32 ylimit)
{
    m_name = "TouchScreen";

    m_isActive = True;
}

Input::InputType TouchScreen::getInputType() const
{
    return INPUT_TOUCHSCREEN;
}

void TouchScreen::clear()
{

}

// For now, not supported on Windows and Linux desktop
#if defined(O3D_WINAPI) || defined(O3D_X11)

TouchScreen::TouchScreen(AppWindow *appWindow, Int32 xlimit, Int32 ylimit) :
    Input(),
    m_appWindow(appWindow)
{

}

TouchScreen::~TouchScreen()
{

}

void TouchScreen::update()
{

}

void TouchScreen::acquire()
{

}

void TouchScreen::release()
{

}

#endif // O3D_WINAPI || O3D_X11
