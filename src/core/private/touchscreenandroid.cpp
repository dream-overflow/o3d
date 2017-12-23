/**
 * @file touchscreenandroid.h
 * @brief Touch screen input controller specialization for Android
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-12
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"

#include <math.h>
#include "o3d/core/touchscreen.h"
#include "o3d/core/debug.h"

#ifdef O3D_ANDROID

using namespace o3d;

TouchScreen::TouchScreen(AppWindow *appWindow, Int32 xlimit, Int32 ylimit) :
    Input(),
    m_appWindow(appWindow)
{
    m_pos.set(-1, -1);
    m_oldPos.set(-1, -1);
}

TouchScreen::~TouchScreen()
{

}

void TouchScreen::update()
{

}

void TouchScreen::acquire()
{
    m_pos.set(-1, -1);
    m_oldPos.set(-1, -1);
}

void TouchScreen::release()
{

}

void TouchScreen::setPosition(Int32 index, Float x, Float y)
{
    if (index != 0) {
        return;
    }

    if (m_pos.x() == -1 || m_pos.y() == -1) {
        // first init
        m_pos.set(x, y);
    }

    m_deltaPos.set(x - m_pos.x(), y - m_pos.y());
    m_oldPos = m_pos;
    m_pos.set(x, y);

    O3D_MESSAGE(String::print("Received motion event from pointer: (%.2f, %.2f)", x, y));
    O3D_MESSAGE(String::print("Delta touch: (%.2f, %.2f)", m_deltaPos.x(), m_deltaPos.y()));
}

void TouchScreen::setUp()
{

}

void TouchScreen::setDown()
{
    // @todo reset pos on down and up
}

void TouchScreen::setPointerUp(Float pressure)
{

}

void TouchScreen::setPointerDown(Float pressure)
{

}

#endif
