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

TouchScreen::Pointer::Pointer() :
    pos(),
    oldPos(),
    deltaPos(),
    pointer(False),
    tap(0),
    pressure(0),
    oldPressure(0),
    deltaPressure(0),
    downTime(0),
    dblTapTime(0)
{
}

// Default constructor.
TouchScreen::TouchScreen(BaseObject *parent) :
    Input(parent)
{
    m_name = "TouchScreen";

    m_isActive = False;
    m_aquired = False;
}

void TouchScreen::commonInit(Int32 xlimit, Int32 ylimit)
{
    m_name = "TouchScreen";

    m_dblTapDelay = DOUBLE_TAP_DELAY;
    m_oldSize = m_deltaSize = 0;

    m_pointers.clear();
    m_pointers.push_back(Pointer());
}

Input::InputType TouchScreen::getInputType() const
{
    return INPUT_TOUCHSCREEN;
}

void TouchScreen::clear()
{

}

Bool TouchScreen::isPointerUp(UInt32 index) const
{
     if (index < m_pointers.size()) {
        return !m_pointers[index].pointer;
     } else {
         return False;
     }
}

Bool TouchScreen::isPointerDown(UInt32 index) const
{
    if (index < m_pointers.size()) {
        return m_pointers[index].pointer;
    } else {
         return False;
     }
}

Bool TouchScreen::getPointerState(UInt32 index) const
{
    if (index < m_pointers.size()) {
        return m_pointers[index].pointer;
    } else {
         return False;
     }
}

Vector2f TouchScreen::getPosition(UInt32 index) const
{
    if (index < m_pointers.size()) {
        return m_pointers[index].pos;
    } else {
         return Vector2f();
     }
}

Float TouchScreen::getPressure(UInt32 index) const
{
     if (index < m_pointers.size()) {
        return m_pointers[index].pressure;
     } else {
         return 0;
     }
}

Float TouchScreen::getPositionX(UInt32 index) const
{
     if (index < m_pointers.size()) {
        return m_pointers[index].pos.x();
     } else {
         return 0;
     }
}

Float TouchScreen::getPositionY(UInt32 index) const
{
     if (index < m_pointers.size()) {
        return m_pointers[index].pos.y();
     } else {
         return 0;
     }
}

Vector2f TouchScreen::getDelta(UInt32 index) const
{
     if (index < m_pointers.size()) {
        return m_pointers[index].deltaPos;
     } else {
         return Vector2f();
     }
}

Float TouchScreen::getDeltaX(UInt32 index) const
{
     if (index < m_pointers.size()) {
        return m_pointers[index].deltaPos.x();
     } else {
         return 0;
     }
}

Float TouchScreen::getDeltaY(UInt32 index) const
{
     if (index < m_pointers.size()) {
        return m_pointers[index].deltaPos.y();
     } else {
         return 0;
     }
}

Bool TouchScreen::isTap() const
{
    if (m_pointers.size() >= 1 && !isSize()) {
        return m_pointers[0].tap == 2;
    } else {
         return False;
     }
}

Bool TouchScreen::isLongTap() const
{
    if (m_pointers.size() >= 1 && !isSize()) {
        return m_pointers[0].tap == 3;
    } else {
         return False;
    }
}

Bool TouchScreen::isDoubleTap() const
{
    if (m_pointers.size() >= 1 && !isSize()) {
        return m_pointers[0].tap == 4;
    } else {
         return False;
    }
}

Bool TouchScreen::isSize() const
{
    // at least 2 pointers down
    return m_pointers.size() >= 2 && m_pointers[0].pointer && m_pointers[1].pointer;
}

Float TouchScreen::getSize() const
{
    if (isSize()) {
        return (m_pointers[1].pos - m_pointers[0].pos).length();
    } else {
        return 0;
    }
}

Float TouchScreen::getDeltaSize() const
{
    return m_deltaSize;
}

void TouchScreen::setDoubleTapDelay(Int32 time)
{
    m_dblTapDelay = time;
}

Int32 TouchScreen::getDoubleTapDelay() const
{
    return m_dblTapDelay;
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
        m_oldSize = m_deltaSize = 0;
        m_pointers.clear();
        m_pointers.push_back(Pointer());
        m_aquired = True;
    }
}

void TouchScreen::release()
{
    if (m_aquired) {
        m_oldSize = m_deltaSize = 0;
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
        if (pointer.tap == 1 || pointer.tap == 2) {
            // possible double tap
            Int64 elapsed = time - pointer.dblTapTime;
            if (elapsed < (Int64)(m_dblTapDelay * 1000 * 1000)) {
                pointer.tap = 4;
            }
        } else {
            // possible tap
            pointer.tap = 1;
        }

        pointer.downTime = time;
        pointer.dblTapTime = 0;
    } else {
        if (pointer.tap == 1 || pointer.tap == 2) {
            Int64 elapsed = time - pointer.downTime;

            // possible double tap
            pointer.dblTapTime = time;

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

#endif // O3D_WINAPI || O3D_X11
