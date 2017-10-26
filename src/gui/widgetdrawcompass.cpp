/**
 * @file widgetdrawcompass.cpp
 * @brief Drawing mode based on compass (9 parts).
 * @author RinceWind
 * @author  Frederic SCHERMA
 * @date 2006-10-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgetdrawcompass.h"

#include "o3d/engine/matrix.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"

using namespace o3d;

WidgetDrawCompass::CompassElement WidgetDrawCompass::string2CompassElement(const String &str)
{
		 if (str=="NorthWest")  { return NORTH_WEST; }
	else if (str=="NorthEast")  { return NORTH_EAST; }
	else if (str=="SouthWest")  { return SOUTH_WEST; }
	else if (str=="SouthEast")  { return SOUTH_EAST; }
	else if (str=="North")      { return NORTH; }
	else if (str=="South")      { return SOUTH; }
	else if (str=="West")       { return WEST; }
	else if (str=="East")       { return EAST; }
	else if (str=="Background") { return BACKGROUND; }
	else
		O3D_ERROR(E_InvalidParameter("Unsupported WidgetDrawCompass element " + str));
}

WidgetDrawCompass::WidgetDrawCompass(Context *context, BaseObject *parent) :
    WidgetDrawMode(context, parent)
{

}

WidgetDrawCompass::WidgetDrawCompass(WidgetDrawCompass &dup) :
	WidgetDrawMode(dup)
{
	for (Int32 i = 0; i < 9; ++i)
	{
		m_WidgetSet[i] = dup.m_WidgetSet[i];
	}
}

WidgetDrawCompass::~WidgetDrawCompass()
{

}

Int32 WidgetDrawCompass::getWidgetWidth() const
{
	return m_WidgetSet[NORTH_WEST].width + m_WidgetSet[NORTH].width + m_WidgetSet[NORTH_EAST].width;
}

Int32 WidgetDrawCompass::getWidgetHeight() const
{
	return m_WidgetSet[NORTH_WEST].height + m_WidgetSet[WEST].height + m_WidgetSet[SOUTH_WEST].height;
}

//
// Draw
//
void WidgetDrawCompass::draw(Int32 x, Int32 y, Int32 width, Int32 height)
{
	if (!m_material.getTechnique(0).getPass(0).getAmbientMap() ||
		!m_material.getTechnique(0).getPass(0).getAmbientMap()->isValid())
		return;

	if (width < 0)
		width = m_WidgetSet[NORTH_WEST].width + m_WidgetSet[NORTH].width + m_WidgetSet[NORTH_EAST].width;

	if (height < 0)
		height = m_WidgetSet[NORTH_WEST].height + m_WidgetSet[WEST].height + m_WidgetSet[SOUTH_WEST].height;

	// first row
	drawWidgetElement(m_WidgetSet[NORTH_WEST], x, y, -1, -1);
	repeatWidgetElement(m_WidgetSet[NORTH], x + m_WidgetSet[NORTH_WEST].width, y, width - m_WidgetSet[NORTH_WEST].width - m_WidgetSet[NORTH_EAST].width, -1);
	drawWidgetElement(m_WidgetSet[NORTH_EAST], x + width - m_WidgetSet[NORTH_EAST].width, y, -1, -1);

	// second row
	repeatWidgetElement(m_WidgetSet[WEST], x, y + m_WidgetSet[NORTH_WEST].height, -1, height - m_WidgetSet[NORTH_WEST].height - m_WidgetSet[SOUTH_WEST].height);
	repeatWidgetElement(m_WidgetSet[BACKGROUND], x + m_WidgetSet[WEST].width, y + m_WidgetSet[NORTH].height, width-m_WidgetSet[WEST].width-m_WidgetSet[EAST].width, height - m_WidgetSet[NORTH].height - m_WidgetSet[SOUTH].height);
	repeatWidgetElement(m_WidgetSet[EAST], x + width - m_WidgetSet[EAST].width, y + m_WidgetSet[NORTH_EAST].height, -1, height - m_WidgetSet[NORTH_EAST].height - m_WidgetSet[SOUTH_EAST].height);

	// third and last row
	drawWidgetElement(m_WidgetSet[SOUTH_WEST], x, y + height - m_WidgetSet[SOUTH_WEST].height, -1, -1);
	repeatWidgetElement(m_WidgetSet[SOUTH], x + m_WidgetSet[SOUTH_WEST].width, y + height - m_WidgetSet[SOUTH].height, width - m_WidgetSet[SOUTH_WEST].width - m_WidgetSet[SOUTH].width, -1);
	drawWidgetElement(m_WidgetSet[SOUTH_EAST], x + width - m_WidgetSet[SOUTH_EAST].width, y + height - m_WidgetSet[SOUTH_EAST].height, -1, -1);

	// finally process the rendering
    m_material.processMaterial(*this, nullptr, nullptr, DrawInfo());
}

