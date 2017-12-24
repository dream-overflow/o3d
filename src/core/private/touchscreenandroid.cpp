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
    commonInit(xlimit, ylimit);

    if (!m_appWindow) {
        O3D_ERROR(E_InvalidParameter("Invalid application window"));
    }

    m_isActive = True;
    m_aquired = False;
}

TouchScreen::~TouchScreen()
{

}

void TouchScreen::update()
{
    if (!m_isActive) {
        return;
    }

    if (!m_aquired) {
        acquire();
    }
}

void TouchScreen::acquire()
{
    if (!m_aquired) {
        m_pointers.clear();
        m_pointers.push_back(Pointer());
        m_aquired = True;
    }
}

void TouchScreen::release()
{
    if (m_aquired) {
        m_pointers.clear();
        m_pointers.push_back(Pointer());
        m_aquired = False;
    }
}

void TouchScreen::setPosition(UInt32 index, Float x, Float y, Float pressure, Int64 time)
{
    if (index >= m_pointers.size()) {
        m_pointers.resize(index+1);
    }

    Pointer &pointer = m_pointers[index];

    if (pointer.oldPos.x() < 0) {
        // first init
        pointer.pos.set(x, y);
        pointer.oldPos.x() = 0;
        pointer.deltaPos.zero();
    } else if (pointer.oldPos.y() < 0) {
        // wait second move
        pointer.pos.set(x, y);
        pointer.oldPos = pointer.pos;
        pointer.deltaPos.zero();
    } else {
        // now we can compute delta
        pointer.deltaPos.set(x - pointer.pos.x(), y - pointer.pos.y());
        pointer.oldPos = pointer.pos;
        pointer.pos.set(x, y);

        if (pointer.tap > 0) {
            // cancel tap on motion
            pointer.tap = 0;
        }

        if (isSize()) {
            Float size = getSize();
            m_deltaSize = size - m_oldSize;
            m_oldSize = size;
        } else {
            m_oldSize = m_deltaSize = 0;
        }
    }

    if (pointer.pressure < 0) {
        pointer.pressure = pressure;
    }

    pointer.deltaPressure = pressure - pointer.oldPressure;
    pointer.oldPressure = pointer.pressure;
    pointer.pressure = pressure;

    pointer.time = time;
}

void TouchScreen::setPointerState(UInt32 index, Bool state, Float x, Float y, Float pressure, Int64 time)
{
    if (index >= m_pointers.size()) {
        m_pointers.resize(index+1);
    }

    Pointer &pointer = m_pointers[index];

    pointer.pressure = pointer.oldPressure = pressure;
    pointer.deltaPressure = 0;
    pointer.pointer = state;

    pointer.pos.set(x, y);
    pointer.oldPos.set(-1, -1);
    pointer.deltaPos.zero();

    m_oldSize = m_deltaSize = 0;

    if (state) {
        // possible tap
        pointer.tap = 1;
        pointer.downTime = time;
    } else {
        if (pointer.tap == 1) {
            Int64 elapsed = time - pointer.downTime;

            // tap occurs
            if (elapsed > (Int64)(LONG_TAP_DURATION * 1000 * 1000)) {
                pointer.tap = 3;  // a long tap
            } else {
                pointer.tap = 2; // a short tap
            }
        } else {
            // reset no tap
            pointer.tap = 0;
        }
    }

    pointer.time = time;
}

#endif
