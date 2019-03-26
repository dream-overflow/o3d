/**
 * @file mouse.cpp
 * @brief Implementation of Mouse.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-09-05
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include <math.h>
#include "o3d/core/mouse.h"
#include "o3d/core/architecture.h"
#include "o3d/core/debug.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Mouse, CORE_MOUSE, Input)

// Default constructor.
Mouse::Mouse(BaseObject *parent) :
    Input(parent)
{
}

#if defined(O3D_DUMMY)
Mouse::Mouse(AppWindow *appWindow, Int32 xlimit, Int32 ylimit, Bool lock)
{

}

Mouse::~Mouse()
{

}

void Mouse::enableCursor()
{

}

void Mouse::disableCursor()
{

}

void Mouse::acquire()
{

}

void Mouse::release()
{

}

void Mouse::update()
{

}

void Mouse::setGrab(Bool)
{

}
#endif // O3D_DUMMY

void Mouse::commonInit(Int32 xlimit, Int32 ylimit)
{
	m_name = "Mouse";

	m_isActive = True;

	m_center.x() = (xlimit >> 1) - 1;
	m_center.y() = (ylimit >> 1) - 1;

	m_posNoAccel.zero();
	m_pos.zero();
	m_oldPos.zero();
	m_deltaPos.zero();

	m_wheel = 0;
	m_wheelOld = 0;
	m_wheelSpeed = 0;
	m_wheelScale = 1.0f;
	m_wheelLastTime = 0;
	m_wheelDelay = 50;
	m_wheelOldDir = 0;

	m_accel = True;
	m_cursor = True;

	m_aquired = False;

    m_dblClickDelay = DOUBLE_CLICK_DELAY;

	// default window rect
	m_window.set(0, 0, xlimit, ylimit);

	// default window centered position
	m_windowPos.x() = ((m_window.width() + m_window.x()) >> 1) - 1;
	m_windowPos.y() = ((m_window.height() + m_window.y()) >> 1) - 1;

    m_isSmoother = False;
	m_isSmootherUseAccel = False;

	Float prog = 1.f;

	// acceleration map
    for (UInt32 s = 0 ; s < 512 ; ++s) {
		m_accelTable[s] = (Int32)(s*prog);
		if (s == 5) prog = 1.05f;
		else if (s == 10) prog = 1.1f;
		else if (s == 30) prog = 1.2f;
		else if (s == 60) prog = 1.3f;
		else if (s == 90) prog = 1.4f;
		else if (s == 150) prog = 1.5f;
	}

    for (Int32 i = 0 ; i < 8 ; ++i) {
		m_lastDblClickTime[i] = 0;
		m_dblClick[i] = 0;
	}

	memset(&m_data, 0, sizeof(MouseData));
}

// Set the delta change (on mouse event).
void Mouse::setMouseDelta(Int32 x, Int32 y)
{
	m_data.lX = x;
	m_data.lY = y;

	updateCoords();
}

// Set the mouse position.
void Mouse::setMousePos(Int32 x, Int32 y)
{
	// when non grabbed uses the direct coordinates
    if (!m_grab) {
		m_deltaPos.set(x - m_pos.x(), y - m_pos.y());
		m_data.lX = m_deltaPos.x();
		m_data.lY = m_deltaPos.y();

		m_pos.set(x, y);
		m_windowPos.set(x, y);
		m_windowPos = m_window.clamp(m_windowPos);

		m_posNoAccel = m_pos;

        // a mouse move cancel dbl click
        if (m_deltaPos.x() || m_deltaPos.y()) {
            for (Int32 i = 0; i < 8; ++i) {
                m_lastDblClickTime[i] = 0;
                m_dblClick[i] = 0;
            }
        }
    } else {
		// managed coordinates
		m_data.lX = x - m_pos.x();
		m_data.lY = y - m_pos.y();

		updateCoords();
	}
}

// Set mouse wheel.
void Mouse::setMouseWheel(Int32 dir)
{
    if (dir < 0) {
        m_data.lZ = -WHEEL_UNIT_STEPS;
    } else if (dir > 0) {
        m_data.lZ = WHEEL_UNIT_STEPS;
    }

	wheelUpdate();
}

Bool Mouse::setMouseButton(Buttons button, Bool pressed)
{
	m_data.rgbButtons[button] = pressed ? 0xFF : 0x00;
	return checkDblClick(button);
}

Bool Mouse::checkDblClick(Buttons button)
{
	UInt32 curTime = System::getMsTime();

    if ((curTime - m_lastDblClickTime[button]) > m_dblClickDelay) {
		m_dblClick[button] = 0;
    }

    if (m_data.rgbButtons[button]) {
        if (m_dblClick[button] == 0) {
			m_lastDblClickTime[0] = curTime;
			m_dblClick[button] = 1;
        } else if (m_dblClick[button] == 1) {
			m_dblClick[button] = 2;
        } else if (m_dblClick[button] == 2) {
			m_dblClick[button] = 0;
        }
    } else if (m_dblClick[button] == 2) {
		m_dblClick[button] = 0;
    }

	// return TRUE if a double click occur
	return m_dblClick[button] == 2;
}

void Mouse::updateCoords()
{
	// use acceleration only if the mouse is locked
    if (m_accel && m_grab) {
        if (o3d::abs(m_data.lX) < 512 && o3d::abs(m_data.lY) < 512) {
			m_pos.x() += (Int32)(m_data.lX>0?m_accelTable[m_data.lX]:-m_accelTable[o3d::abs(m_data.lX)]);
			m_pos.y() += (Int32)(m_data.lY>0?m_accelTable[m_data.lY]:-m_accelTable[o3d::abs(m_data.lY)]);
		}
    } else {
		m_pos.x() += m_data.lX;
		m_pos.y() += m_data.lY;
	}

	m_posNoAccel.x() += m_data.lX;
	m_posNoAccel.y() += m_data.lY;

	// delta compute
	m_deltaPos = m_pos - m_oldPos;

    // a mouse move cancel dbl click
    if (m_deltaPos.x() || m_deltaPos.y()) {
        for (Int32 i = 0; i < 8; ++i) {
            m_lastDblClickTime[i] = 0;
            m_dblClick[i] = 0;
        }
    }

	// old positions
	m_oldPos = m_pos;

	// mapped mouse position
	m_windowPos += m_deltaPos;
		
	// let the mouse go outside of the client
    if (!m_grab) {
        if (!m_window.isInside(m_windowPos)) {
			release();
        }
    }

	// re-map the window coordinates if necessary
	m_windowPos = m_window.clamp(m_windowPos);
}

void Mouse::wheelUpdate()
{
	// wheel speed
	UInt32 curtime = System::getMsTime();

	m_wheel += m_data.lZ;

	m_wheelDelta = m_wheel - m_wheelOld;
	m_wheelOld = m_wheel;

    if (m_wheelLastTime == 0) {
		m_wheelLastTime = curtime;
    }

	// reset the speed when change of direction occurs
    if (m_wheelDelta != 0) {
        if ((m_wheelOldDir < 0 && m_wheelDelta > 0) || (m_wheelOldDir > 0 && m_wheelDelta < 0)) {
			m_wheelOldDir = m_wheelSpeed = 0;
		}

        m_wheelSpeed += (m_wheelDelta / WHEEL_UNIT_STEPS);
		m_wheelOldDir = m_wheelDelta;
	}

	// compute the speed
    if ((curtime - m_wheelLastTime) > m_wheelDelay) {
        if (o3d::abs(m_wheelSpeed) > 1) {
			m_wheelSpeed = (Int32)((Float)m_wheelSpeed * m_wheelScale);
        } else {
			m_wheelSpeed = 0;
        }

		m_wheelLastTime = curtime;
    }
}

void Mouse::updateSmoother(Float time)
{
    if (time < 0.f) {
        time = (Float)System::getTime() / (Float)System::getTimeFrequency();
    }

    // use smoother only if the mouse is locked
    if (m_isSmoother && m_grab) {
        if (m_isSmootherUseAccel) {
            m_smoother.samplePos(
                        Vector2f((Float)m_pos.x(), (Float)m_pos.y()),
                        time);
        } else {
            m_smoother.samplePos(
                        Vector2f((Float)m_posNoAccel.x(), (Float)m_posNoAccel.y()),
                        time);
        }
    }
}

void Mouse::setWheelSpeed(UInt32 delay, Float scale)
{
    m_wheelDelay = delay;
    m_wheelScale = scale;
}

void Mouse::setMouseRegion(const Box2i &window)
{
    m_window = window;

    // compute the mouse centered position
    m_windowPos.x() = ((m_window.width() + m_window.x()) >> 1) - 1;
    m_windowPos.y() = ((m_window.height() + m_window.y()) >> 1) - 1;
}

Float Mouse::getSmoothedWheel() const
{
    if (m_wheelSpeed < 0) {
        return -(Float)WHEEL_UNIT_STEPS;
    } else if (m_wheelSpeed > 0) {
        return (Float)WHEEL_UNIT_STEPS;
    } else {
        return 0.f;
    }
}

Input::InputType Mouse::getInputType() const
{
    return INPUT_MOUSE;
}

void Mouse::clear()
{
    for (Int32 i = 0; i < 8; ++i) {
		m_lastDblClickTime[i] = 0;
		m_dblClick[i] = 0;
	}

	m_wheelDelta = 0;
}
