/**
 * @file mouseandroid.cpp
 * @brief Implementation of mouse.h (Android part).
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/mouse.h"

// ONLY IF O3D_ANDROID IS SELECTED
#ifdef O3D_ANDROID

#include <math.h>

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

    if (!m_appWindow) {
        O3D_ERROR(E_InvalidParameter("Invalid application window"));
    }

    // for windowed mode, start with current absolute mouse position
    if (!lock) {
        // @todo

        // m_windowPos = m_posNoAccel = m_pos = m_oldPos = Vector2i(mousePos.x, mousePos.y);
        m_windowPos = m_window.clamp(m_windowPos);

        m_grab = False;
    } else {
        setGrab();
    }

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
    if (!m_cursor) {
        // @todo
        m_cursor = True;
    }
}

void Mouse::disableCursor()
{
    if (m_cursor) {
        // @todo
        m_cursor = False;
    }
}

// lock/unlock the mouse position
void Mouse::setGrab(Bool lock)
{
    if (!m_appWindow->getHWND()) {
        O3D_ERROR(E_InvalidOperation("Related application window must be active"));
    }

    // enable
    if (!m_grab && lock) {
        // @todo

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
        // hide @todo
    }

    // disable
    if (m_grab && !lock) {
        // @todo

        m_deltaPos.zero();
        m_smoother.reset();

        // m_windowPos = m_posNoAccel = m_pos = m_oldPos = Vector2i(mousePos.x, mousePos.y);
        m_windowPos = m_window.clamp(m_windowPos);

        m_grab = False;
        // @todo show
    }
}

// update input data (only if acquired)
void Mouse::update()
{
    if (!m_isActive) {
        return;
    }

    if (!m_aquired) {
        acquire();
    }

    // @todo

    // update the wheel speed
    wheelUpdate();

    // must update the mouse smoother to have a correct effect
    updateSmoother(-1.f);

    if (m_grab) {
        wrapPrivate();
    }
}

void Mouse::wrapPrivate()
{
    // make mouse infinite
    // @todo
}

// acquire mouse position and buttons states
void Mouse::acquire()
{
    if (m_isActive && !m_aquired) {
        // @todo
        m_deltaPos.zero();

        m_windowPos = m_window.clamp(m_windowPos);

        m_aquired = True;

        m_wheel = 0; // initial wheel position
    }
}

// release mouse hardware
void Mouse::release()
{
    if (m_aquired) {
        m_wheel = 0; // wheel position is reset

        m_aquired = False;
    }
}

#endif // O3D_ANDROID
