/**
 * @file keyboard.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/keyboard.h"

#include "o3d/core/architecture.h"
#include "o3d/core/debug.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Keyboard, CORE_KEYBOARD, Input)

// Default constructor
Keyboard::Keyboard(BaseObject *parent) :
	Input(parent)
{
}

// clear
void Keyboard::clear()
{
	m_keys.clear();
}

