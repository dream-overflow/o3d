/**
 * @file inputmanager.h
 * @brief Manager for inputs like keyboard, mouse, joystick and more.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2003-09-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_INPUTMANAGER_H
#define _O3D_INPUTMANAGER_H

#include "base.h"
#include "input.h"
#include "memorydbg.h"
#include "keyboard.h"
#include "mouse.h"
#include "file.h"

namespace o3d {

class AppWindow;

//---------------------------------------------------------------------------------------
//! @class InputManager
//-------------------------------------------------------------------------------------
//! Manager for inputs like keyboard, mouse, joystick and more.
//---------------------------------------------------------------------------------------
class O3D_API InputManager
{
public:

	//! constructor
	InputManager() :
        m_Keyboard(nullptr),
        m_Mouse(nullptr),
        m_appWindow(nullptr)
	{}

	//! destructor
	~InputManager();

	//! create directinput object
	Bool initInput(AppWindow *appWindow);
	//! release directinput object
	void releaseInput();

	//! destroy keyboard/mouse (and directx object)
	void destroy();

	//! init/release/get keyboard
	Bool initKeyboard();
	inline void releaseKeyboard() { o3d::deletePtr(m_Keyboard); }
    inline Bool isKeyboard() const { return (m_Keyboard != nullptr); }

	//! init/release/get mouse
	Bool initMouse(Int32 xlimit = 800, Int32 ylimit = 600, Bool lock = False);
	inline void releaseMouse() { o3d::deletePtr(m_Mouse); }
    inline Bool isMouse() const { return (m_Mouse != nullptr); }


	//-----------------------------------------------------------------------------------
	// Getter
	//-----------------------------------------------------------------------------------

	//! Keyboard accessor.
	inline Keyboard* getKeyboard() { return m_Keyboard; }
	//! Keyboard accessor (read only).
	inline const Keyboard* getKeyboard() const { return m_Keyboard; }
	//! Mouse accessor.
	inline Mouse* getMouse() { return m_Mouse; }
	//! Mouse accessor (read only).
	inline const Mouse* getMouse() const { return m_Mouse; }

	//-----------------------------------------------------------------------------------
	// Acquisition
	//-----------------------------------------------------------------------------------

	//! force input data value
	void clear();

	//! acquire input data
	void acquire();

	//! release input data
	void release();

	//! update input data
	void update();

private:

	Keyboard *m_Keyboard;    //!< keyboard
	Mouse    *m_Mouse;       //!< mouse

	AppWindow *m_appWindow;  //!< Related application window.
};

} // namespace o3d

#endif // _O3D_INPUTMANAGER_H

