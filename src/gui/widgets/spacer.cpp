/**
 * @file spacer.cpp
 * @brief Implementation of Space.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-03-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"

#include "o3d/gui/widgets/spacer.h"
#include "o3d/gui/guitype.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Spacer, GUI_LAYOUT_SPACER, Widget)

// Constructor
Spacer::Spacer(BaseObject *parent) :
	Widget(parent)
{
}

Spacer::Spacer(Widget *parent) :
	Widget(parent)
{
}

// get the recommended widget default size
Vector2i Spacer::getDefaultSize()
{
	return Vector2i(1,1);
}

void Spacer::sizeChanged()
{

}

