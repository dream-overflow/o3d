/**
 * @file mouse.h
 * @brief Mouse input controller
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-09-05
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MOUSE_H
#define _O3D_MOUSE_H

#include "input.h"
#include "memorydbg.h"
#include "mousesmoother.h"
#include "box.h"

namespace o3d {

class AppWindow;

/**
 * @brief Mouse input manager
 */
class O3D_API Mouse : public Input
{
public:

	O3D_DECLARE_CLASS(Mouse)

	//! Mouse buttons
	enum Buttons
	{
		LEFT = 0,
		RIGHT = 1,
		MIDDLE = 2,
		X1 = 3,
        UNKNOWN = 4,
        X2 = 5,
        LAST_BUTTON = X2
	};

    //! Number of differents supported mouse buttons.
    static const Int32 NUM_BUTTONS = LAST_BUTTON + 1;

	//! Default constructor.
    Mouse(BaseObject *parent = nullptr);

	//! Extended constructor.
	//! @param appWindow A valid application window.
	//! @param width Width of the initial window region.
	//! @param height Height of the initial window region.
	//! @param lock Lock the mouse to the center of the window's application to provide
	//!             an exclusive mode.
	Mouse(
		AppWindow *appWindow,
		Int32 xlimit = 800,
		Int32 ylimit = 600,
		Bool lock = False);

	//! Virtual destructor.
	virtual ~Mouse();

	//-----------------------------------------------------------------------------------
	// Exclusive mode
	//-----------------------------------------------------------------------------------

	//! Grab the mouse to the window.
	//! @note Mouse acceleration and smoothing is available in this mode only.
	void setGrab(Bool grab = True);
	//! Is mouse grabbed.
	Bool isGrabbed() const { return m_grab; }

	//! Set x/y acceleration state.
	//! @note Works only if the mouse is locked.
	inline void setAcceleration(Bool state) { m_accel = state; }
	//! Get x/y acceleration state.
	inline Bool getAcceleration() const { return m_accel; }

	//-----------------------------------------------------------------------------------
	// Global settings
	//-----------------------------------------------------------------------------------

	//! set the mouse double click delay between two clicks (default 500ms)
	inline void setMouseDblClickTime(Int32 time) { m_dblClickTime = time; }
	//! Get the mouse double click delay between two clicks (default 500ms).
	inline Int32 getMouseDlbClickTime() const { return m_dblClickTime; }

	//! set the mouse smoother activity.
	//! @note Works only if the mouse is locked.
	inline void setMouseSmoother(Bool state) { m_isSmoother = state; }
	//! get the mouse smoother activity
	inline Bool isMouseSmoother() const { return m_isSmoother; }

	//! set the mouse smoother sampling period.
	//! @note Works only if the mouse is locked.
	inline void setSmootherPeriod(Float period) { m_smoother.setSamplingPeriod(period); }
	//! get the mouse smoother sampling period.
	inline Bool getSmootherPeriod() const { return m_smoother.getSamplingPeriod(); }

	//! Set the window mouse region.
	inline void setMouseRegion(const Box2i &window)
	{
		m_window = window;

		// compute the mouse centered position
		m_windowPos.x() = ((m_window.width() + m_window.x()) >> 1) - 1;
		m_windowPos.y() = ((m_window.height() + m_window.y()) >> 1) - 1;
	}

	//! Get the window mouse region.
	inline const Box2i& getMouseRegion() { return m_window; }

	//! Enable the system (or SDL) cursor drawing state.
	void enableCursor();
	//! Disable the system (or SDL) cursor drawing state.
	void disableCursor();
	//! Toggle the system (or SDL) cursor drawing state.
	inline void toggleCursor()
	{
        if (m_cursor) {
			disableCursor();
        } else {
			enableCursor();
        }
	}

	//! Set the smoothed wheel speed.
	//! @param delay Attenuation delay (50ms is default).
	//! @param scale A scale factor.
	inline void setWheelSpeed(UInt32 delay, Float scale)
	{
		m_wheelDelay = delay;
		m_wheelScale = scale;
	}

	//! Get the inertia attenuation delay (50ms is default).
	inline UInt32 getWheelDelay() const { return m_wheelDelay; }

	//! Get the wheel sensibility factor (default is 1.).
	inline Float getWheelScale() const { return m_wheelScale; }

	//-----------------------------------------------------------------------------------
	// Current mouse state
	//-----------------------------------------------------------------------------------

	//! Get the system (or SDL) cursor drawing state.
	inline Bool isVisible() const { return m_cursor; }

	//! Is left button down.
	inline Bool isLeftDown() const { return O3D_ISKEYDOWN(m_data.rgbButtons[0]); }
	//! Is left button up.
	inline Bool isLeftUp() const { return !O3D_ISKEYDOWN(m_data.rgbButtons[0]);  }

	//! Is right button down.
	inline Bool isRightDown() const { return O3D_ISKEYDOWN(m_data.rgbButtons[1]); }
	//! Is right button up.
	inline Bool isRightUp() const { return !O3D_ISKEYDOWN(m_data.rgbButtons[1]);  }

	//! Is middle button down.
	inline Bool isMiddleDown() const { return O3D_ISKEYDOWN(m_data.rgbButtons[2]); }
	//! Is middle button up.
	inline Bool isMiddleUp() const { return !O3D_ISKEYDOWN(m_data.rgbButtons[2]);  }

	//! Is X1 button down.
	inline Bool isX1Down() const { return O3D_ISKEYDOWN(m_data.rgbButtons[3]); }
	//! Is X1 button up.
	inline Bool isX1Up() const { return !O3D_ISKEYDOWN(m_data.rgbButtons[3]);  }
	
	//! Is X2 button down.
	inline Bool isX2Down() const { return O3D_ISKEYDOWN(m_data.rgbButtons[4]); }
	//! Is X2 button up.
	inline Bool isX2Up() const { return !O3D_ISKEYDOWN(m_data.rgbButtons[4]);  }
	
	//! Is N button down.
	inline Bool isButtonDown(Buttons n) const { return O3D_ISKEYDOWN(m_data.rgbButtons[n]); }
	//! Is N button up.
	inline Bool isButtonUp(Buttons n) const { return !O3D_ISKEYDOWN(m_data.rgbButtons[n]);  }

	//! Get the mouse absolute coordinates.
	inline Vector2f getPosition() const
	{
		return Vector2f((Float)m_pos.x(), (Float)m_pos.y());
	}

	//! Get the X mouse absolute coordinates.
	inline Float getPositionX() const { return (Float)m_pos.x(); }
	//! Get the Y mouse absolute coordinates.
	inline Float getPositionY() const { return (Float)m_pos.y(); }

	//! Get the mouse coordinates mapped to window region.
	inline const Vector2i& getMappedPosition() const { return m_windowPos; }

	//! Get the smoothed mouse coordinates.
	inline const Vector2f& getSmootherPosition() const { return m_smoother.getLastPos(); }

	//! Get the mouse delta position since the last update.
	inline Vector2f getDelta() const
	{
		return Vector2f((Float)m_deltaPos.x(), (Float)m_deltaPos.y());
	}

	//! Get the X mouse absolute coordinates.
	inline Float getDeltaX() const { return (Float)m_deltaPos.x(); }
	//! Get the Y mouse absolute coordinates.
	inline Float getDeltaY() const { return (Float)m_deltaPos.y(); }

	//! Get the delta smoothed mouse.
	inline const Vector2f& getSmoothedDelta() const { return m_smoother.getDeltaPos(); }

	//! Get wheel coordinate.
	//! @note Wheel unit is 120.
	inline Int32 getWheel() const { return m_wheel; }

	//! Get delta wheel delta.
	//! @note Wheel unit is 120.
	inline Int32 getWheelDelta() const { return m_wheelDelta; }

	//! Get the smoothed wheel speed according to a delay and a scale.
	//! @note Wheel unit is 120.
	inline Float getSmoothedWheel() const
	{
        if (m_wheelSpeed < 0) {
			return -120.f;
        } else if (m_wheelSpeed > 0) {
			return 120.f;
        } else {
			return 0.f;
        }
	}

	//-------------------------------------------------------------------------------
	// virtual
	//-------------------------------------------------------------------------------

	//! Clear current mouse states.
	virtual void clear();

	//! Update input data (only if acquired).
	virtual void update();

	//! Acquire mouse position and buttons states.
	virtual void acquire();

	//! Release mouse hardware.
	virtual void release();

	//! Set a mouse button change (on mouse event).
	//! @param button The mouse button.
	//! @param pressed TRUE mean pressed, FALSE mean released.
	//! @return True if a double click occur.
	Bool setMouseButton(Buttons button, Bool pressed);

	//! Set the delta change (on mouse event).
	void setMouseDelta(Int32 x, Int32 y);

	//! Set the mouse position (on mouse event).
	void setMousePos(Int32 x, Int32 y);

	//! Set mouse wheel (on mouse event).
	void setMouseWheel(Int32 dir);

protected:

	Int32 m_wheelOldDir;       //!< old wheel direction
	Float m_wheelScale;        //!< wheel sensibility factor
	Int32 m_wheelSpeed;        //!< num of turns per second
	UInt32 m_wheelLastTime;    //!< last time of a wheel event
	UInt32 m_wheelDelay;       //!< inertia delay (in ms : default 50ms)

	Vector2i m_pos;             //!< Mouse absolute position.
	Vector2i m_oldPos;          //!< Mouse previous absolute position.
	Vector2i m_posNoAccel;      //!< Mouse position without acceleration.
	Vector2i m_deltaPos;        //!< Mouse delta position since last update.
	Vector2i m_center;          //!< Mouse centering position.

	Int32 m_wheel;             //!< wheel coords (]-oo,-120] down,0 null,[120,+oo[ up)
	Int32 m_wheelOld;          //!< old wheel coords
	Int32 m_wheelDelta;        //!< delta wheel

	Bool m_accel;              //!< use x/y acceleration

	Bool m_aquired;            //!< input acquired by direct input

	AppWindow *m_appWindow;        //!< Related application window.
	Bool m_cursor;             //!< cursor drawing state

	UInt8 m_dblClick[8];          //!< double clicked buttons arrays
	UInt32 m_dblClickTime;        //!< max time between to click for have a double click
	UInt32 m_lastDblClickTime[8]; //!< last time that a button have been clicked

	MouseData m_data;              //!< mouse data

	Int32 m_accelTable[512];   //!< accelerator array

	Bool m_grab;               //!< Locked mouse position.

	Box2i m_window;             //!< Mapping region.
	Vector2i m_windowPos;       //!< Mapped to window coordinates.

	Bool m_isSmoother;         //!< Is the smoother enabled
	Bool m_isSmootherUseAccel; //!< Is the smoother use accelerator
	MouseSmoother m_smoother;      //!< mouse smoother

	//! Called by the constructor.
	void commonInit(Int32 xlimit, Int32 ylimit);

	//! Check for a double click.
	Bool checkDblClick(Buttons button);
	
	//! Update coordinates according to the last update of m_data.lX/m_data.lY.
	void updateCoords();
	
	//! Update coordinates according to the last update of m_data.lZ and current time.
	void wheelUpdate();

    //! Update the mouse smoother using a specified time, or current time if negative.
    void updateSmoother(Float time);

    //! Internally wrap mouse.
    void wrapPrivate();
};

/**
 * @brief Mouse button event.
 */
class ButtonEvent
{
public:

	//! Constructor.
	//! @param button Mouse button.
	//! @param state TRUE if the button is pressed, FALSE if the button is released.
	ButtonEvent(Mouse::Buttons button, Bool pressed, Bool dblClick) :
		m_button(button)
	{
		m_state = (pressed ? PRESSED : RELEASED) | (dblClick ? DBL_CLICK : 0);
	}
	
	//! Get the mouse button.
	inline Mouse::Buttons button() const { return m_button; }

	//! Is the button is up.
	inline Bool isUp() const { return m_state & UP; }
	//! Is the button is pressed.
	inline Bool isPressed() const { return m_state & PRESSED; }

	//! Is the button is down.
	inline Bool isDown() const { return m_state & DOWN; }
	//! Is the button is down.
	inline Bool isReleased() const { return m_state & RELEASED; }

	//! Is a double click.
	inline Bool isDblClick() const { return m_state & DBL_CLICK; }

private:

	//! Key states.
	enum States
	{
		RELEASED = 0x01,
		PRESSED = 0x02,
		DOWN = PRESSED,
		UP = RELEASED,
		DBL_CLICK = 0x04
	};

	Mouse::Buttons m_button;      //!< Mouse button.
	Int32 m_state;            //!< Button state.
};

} // namespace o3d

#endif // _O3D_MOUSE_H
