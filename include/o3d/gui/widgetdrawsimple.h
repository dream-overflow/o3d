/**
 * @file widgetdrawsimple.h
 * @brief Simple Drawing Mode for Widgets.
 * @author RinceWind
 * @date 2006-10-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_WIDGETDRAWSIMPLE_H
#define _O3D_WIDGETDRAWSIMPLE_H

#include "widgetdrawmode.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class WidgetDrawSimple
//-------------------------------------------------------------------------------------
//! Simple Drawing Mode for Widgets.
//---------------------------------------------------------------------------------------
class O3D_API WidgetDrawSimple : public WidgetDrawMode
{
public:

	//! default constructor
    WidgetDrawSimple(Context *context, BaseObject *pParent);

	//! copy constructor
	WidgetDrawSimple(WidgetDrawSimple &dup);

	//! destructor
	virtual ~WidgetDrawSimple();

	//! Get the type of the widget mode
	virtual DrawModeType getType() const { return SIMPLE_MODE; }

	//! Set the data set
	inline void setData(Int32 x, Int32 y, Int32 width, Int32 height)
	{
		m_WidgetSet.x = x;
		m_WidgetSet.y = y;

		m_WidgetSet.width = width;
		m_WidgetSet.height = height;
	}

	//! Get the data set
	inline const WidgetDataSet& getData() const { return m_WidgetSet; }

	//! Get base width/height
	Int32 getWidgetWidth() const;
	Int32 getWidgetHeight() const;

	//! draw directly into the current buffer
	void draw(Int32 x, Int32 y, Int32 width, Int32 height);

protected:

	WidgetDataSet m_WidgetSet;
};

} // namespace o3d

#endif // _O3D_WIDGETDRAWSIMPLE_H

