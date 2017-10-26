/**
 * @file inputmanager.cpp
 * @brief Implementation of InputManager.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2003-09-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/inputmanager.h"

#include "o3d/core/classfactory.h"
#include "o3d/core/debug.h"

using namespace o3d;

/*---------------------------------------------------------------------------------------
  destructor
---------------------------------------------------------------------------------------*/
InputManager::~InputManager()
{
	destroy();
}

/*---------------------------------------------------------------------------------------
  destroy keyboard/mouse and directx object
---------------------------------------------------------------------------------------*/
void InputManager::destroy()
{
	o3d::deletePtr(m_Keyboard);
	o3d::deletePtr(m_Mouse);

	releaseInput();
}

/*---------------------------------------------------------------------------------------
  create directinput object
---------------------------------------------------------------------------------------*/
Bool InputManager::initInput(AppWindow *appWindow)
{
	m_appWindow = appWindow;
	return True;
}

/*---------------------------------------------------------------------------------------
  release directinput object
---------------------------------------------------------------------------------------*/
void InputManager::releaseInput()
{
}

/*---------------------------------------------------------------------------------------
  init keyboard
---------------------------------------------------------------------------------------*/
Bool InputManager::initKeyboard()
{
	m_Keyboard = new Keyboard(m_appWindow);
	return True;
}

/*---------------------------------------------------------------------------------------
  init mouse
---------------------------------------------------------------------------------------*/
Bool InputManager::initMouse(Int32 xlimit, Int32 ylimit, Bool lock)
{
	m_Mouse = new Mouse(m_appWindow, xlimit, ylimit, lock);
	return True;
}

/*---------------------------------------------------------------------------------------
  acquire input data
---------------------------------------------------------------------------------------*/
void InputManager::acquire()
{
	if (m_Mouse)
		m_Mouse->acquire();

	if (m_Keyboard)
		m_Keyboard->acquire();
}

/*---------------------------------------------------------------------------------------
  unacquire input data
---------------------------------------------------------------------------------------*/
void InputManager::release()
{
	if (m_Mouse)
		m_Mouse->release();

	if (m_Keyboard)
		m_Keyboard->release();
}

/*---------------------------------------------------------------------------------------
  update input data
---------------------------------------------------------------------------------------*/
void InputManager::update()
{
	if (m_Mouse)
		m_Mouse->update();

	if (m_Keyboard)
		m_Keyboard->update();
}

/*---------------------------------------------------------------------------------------
  force input data value
---------------------------------------------------------------------------------------*/
void InputManager::clear()
{
	if (m_Mouse)
		m_Mouse->clear();

	if (m_Keyboard)
		m_Keyboard->clear();
}

