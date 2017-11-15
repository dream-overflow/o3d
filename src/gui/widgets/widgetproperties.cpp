/**
 * @file widgetproperties.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/widgetproperties.h"
#include "o3d/gui/guitype.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(WidgetProperties, GUI_WIDGET_PROPERTIES, SceneEntity)

// default constructor
WidgetProperties::WidgetProperties(BaseObject *parent) :
	SceneEntity(parent),
	m_borderSize(5),
	m_borderElt(BORDER_ALL),
	m_vertAlign(MIDDLE_ALIGN),
	m_horiAlign(CENTER_ALIGN),
	m_stretchFactor(0),
	m_alpha(1.0f)
{
	m_capacities.enable(STATE_ACTIVITY);
	m_capacities.enable(STATE_VISIBILITY);
	//m_capacities.disable(CAPS_SHAPED);
	m_capacities.enable(STATE_DIRTY);
}

