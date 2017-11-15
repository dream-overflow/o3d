/**
 * @file input.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/input.h"

#include "o3d/core/keyboard.h"
#include "o3d/core/mouse.h"
#include "o3d/core/debug.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(Input, CORE_INPUT, BaseObject)

// constructor/destructor
Input::Input(BaseObject *parent) :
	BaseObject(parent)
{
    m_isActive = True;
}

