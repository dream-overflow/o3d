/**
 * @file widgetdrawsimple.cpp
 * @brief Base class for drawing widgets.
 * @author RinceWind
 * @date 2006-10-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgetdrawsimple.h"

#include "o3d/engine/matrix.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"

using namespace o3d;

/*---------------------------------------------------------------------------------------
  default constructor
---------------------------------------------------------------------------------------*/
WidgetDrawSimple::WidgetDrawSimple(Context *context, BaseObject *parent) :
    WidgetDrawMode(context, parent)
{
}

WidgetDrawSimple::WidgetDrawSimple(WidgetDrawSimple &dup) :
	WidgetDrawMode(dup),
	m_WidgetSet(dup.m_WidgetSet)
{
}

/*---------------------------------------------------------------------------------------
  destructor
---------------------------------------------------------------------------------------*/
WidgetDrawSimple::~WidgetDrawSimple()
{
}

/*---------------------------------------------------------------------------------------
  Get base width/height
---------------------------------------------------------------------------------------*/
Int32 WidgetDrawSimple::getWidgetWidth() const
{
	return m_WidgetSet.width;
}

Int32 WidgetDrawSimple::getWidgetHeight() const
{
	return m_WidgetSet.height;
}

//---------------------------------------------------------------------------------------
// Draw
//---------------------------------------------------------------------------------------
void WidgetDrawSimple::draw(
		Int32 x,
		Int32 y,
		Int32 width,
		Int32 height)
{
	if (!m_material.getTechnique(0).getPass(0).getAmbientMap() ||
		!m_material.getTechnique(0).getPass(0).getAmbientMap()->isValid())
		return;

	if ((width > m_WidgetSet.width) || (height > m_WidgetSet.height))
			repeatWidgetElement(m_WidgetSet, x, y, width, height);
	else
		drawWidgetElement(m_WidgetSet, x, y, width, height);

	// finally process the rendering
    m_material.processMaterial(*this, nullptr, nullptr, DrawInfo());
}

