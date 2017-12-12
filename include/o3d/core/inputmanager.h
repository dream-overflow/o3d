/**
 * @file inputmanager.h
 * @brief Manager for inputs like keyboard, mouse, joystick and more.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-09-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_INPUTMANAGER_H
#define _O3D_INPUTMANAGER_H

#include "input.h"
#include "memorydbg.h"

namespace o3d {

class AppWindow;
class Keyboard;
class Mouse;
class TouchScreen;

/**
 * @brief Manager for inputs like keyboard, mouse, joystick and more.
 * @todo Joypad, joystick and camera.
 */
class O3D_API InputManager
{
public:

    //! Default constructor
    InputManager();

	//! destructor
	~InputManager();

    //! Setup the related application window.
    Bool setAppWindow(AppWindow *appWindow);
    //! Destroy all previously created input.
	void destroy();

    //! Init an input support.
    Bool initInput(Input::InputType type);
    //! Release an input support.
    void releaseInput(Input::InputType type);
    //! Is input support init.
    Bool isInput(Input::InputType type) const;
    //! Get input support.
    Input* getInput(Input::InputType type);
    //! Get input support (read-only).
    Input* getInput(Input::InputType type) const;

	//-----------------------------------------------------------------------------------
    // Helpers
	//-----------------------------------------------------------------------------------

	//! Keyboard accessor.
    inline Keyboard* getKeyboard() { return m_keyboard; }
	//! Keyboard accessor (read only).
    inline const Keyboard* getKeyboard() const { return m_keyboard; }

	//! Mouse accessor.
    inline Mouse* getMouse() { return m_mouse; }
	//! Mouse accessor (read only).
    inline const Mouse* getMouse() const { return m_mouse; }

    //! TouchScreen accessor.
    inline TouchScreen* getTouchScreen() { return m_touchScreen; }
    //! TouchScreen accessor (read only).
    inline const TouchScreen* getTouchScreen() const { return m_touchScreen; }

	//-----------------------------------------------------------------------------------
	// Acquisition
	//-----------------------------------------------------------------------------------

    //! Clear input data value.
	void clear();

    //! Acquire all inputs data.
	void acquire();

    //! Release all inputs data.
	void release();

    //! Update all input data.
	void update();

private:

    AppWindow *m_appWindow;      //!< Related application window.

    Keyboard *m_keyboard;        //!< Keyboard
    Mouse *m_mouse;              //!< Mouse
    TouchScreen *m_touchScreen;  //!< Touch-screen
};

} // namespace o3d

#endif // _O3D_INPUTMANAGER_H
