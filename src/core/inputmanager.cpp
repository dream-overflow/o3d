/**
 * @file inputmanager.cpp
 * @brief Implementation of InputManager.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-09-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/inputmanager.h"

#include "o3d/core/classfactory.h"
#include "o3d/core/debug.h"

#include "o3d/core/appwindow.h"
#include "o3d/core/touchscreen.h"

using namespace o3d;

InputManager::InputManager() :
    m_appWindow(nullptr),
    m_keyboard(nullptr),
    m_mouse(nullptr),
    m_touchScreen(nullptr)
{
}

InputManager::~InputManager()
{
	destroy();
}

void InputManager::destroy()
{
    deletePtr(m_keyboard);
    deletePtr(m_mouse);
    deletePtr(m_touchScreen);
}

Bool InputManager::initInput(Input::InputType type)
{
    switch (type) {
        case Input::INPUT_KEYBOARD:
            m_keyboard = new Keyboard(m_appWindow);
            return True;
        case Input::INPUT_MOUSE:
            m_mouse = new Mouse(m_appWindow, m_appWindow->getSize().x(), m_appWindow->getSize().y(), False);
            return True;
        case Input::INPUT_TOUCHSCREEN:
            m_touchScreen = new TouchScreen(m_appWindow, m_appWindow->getSize().x(), m_appWindow->getSize().y());
            return True;
        default:
            return False;
    }
}

void InputManager::releaseInput(Input::InputType type)
{
    switch (type) {
        case Input::INPUT_KEYBOARD:
            o3d::deletePtr(m_keyboard);
            break;
        case Input::INPUT_MOUSE:
            o3d::deletePtr(m_mouse);
            break;
        case Input::INPUT_TOUCHSCREEN:
            o3d::deletePtr(m_touchScreen);
            break;
        default:
            break;
    }
}

Bool InputManager::isInput(Input::InputType type) const
{
    switch (type) {
        case Input::INPUT_KEYBOARD:
            return m_keyboard != nullptr;
        case Input::INPUT_MOUSE:
            return m_mouse != nullptr;
        case Input::INPUT_TOUCHSCREEN:
            return m_touchScreen != nullptr;
        default:
            return False;
    }
}

Input *InputManager::getInput(Input::InputType type)
{
   switch (type) {
        case Input::INPUT_KEYBOARD:
            return m_keyboard;
        case Input::INPUT_MOUSE:
            return m_mouse;
        case Input::INPUT_TOUCHSCREEN:
            return m_touchScreen;
        default:
            return nullptr;
    }
}

Input *InputManager::getInput(Input::InputType type) const
{
   switch (type) {
        case Input::INPUT_KEYBOARD:
            return m_keyboard;
        case Input::INPUT_MOUSE:
            return m_mouse;
        case Input::INPUT_TOUCHSCREEN:
            return m_touchScreen;
        default:
            return nullptr;
    }
}

Bool InputManager::setAppWindow(AppWindow *appWindow)
{
	m_appWindow = appWindow;
	return True;
}

void InputManager::acquire()
{
    if (m_mouse) {
        m_mouse->acquire();
    }

    if (m_keyboard) {
        m_keyboard->acquire();
    }

    if (m_touchScreen) {
        m_touchScreen->acquire();
    }
}

void InputManager::release()
{
    if (m_mouse) {
        m_mouse->release();
    }

    if (m_keyboard) {
        m_keyboard->release();
    }

    if (m_touchScreen) {
        m_touchScreen->release();
    }
}

void InputManager::update()
{
    if (m_mouse) {
        m_mouse->update();
    }

    if (m_keyboard) {
        m_keyboard->update();
    }

    if (m_touchScreen) {
        m_touchScreen->update();
    }
}

void InputManager::clear()
{
    if (m_mouse) {
        m_mouse->clear();
    }

    if (m_keyboard) {
        m_keyboard->clear();
    }

    if (m_touchScreen) {
        m_touchScreen->clear();
    }
}
