/**
 * @file widgetdrawcompass.h
 * @brief Drawing mode based on compass (9 parts).
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-10-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_WIDGETDRAWCOMPASS_H
#define _O3D_WIDGETDRAWCOMPASS_H

#include "widgetdrawmode.h"

namespace o3d {

/**
 * @brief Drawing mode based on compass (9 parts).
 */
class O3D_API WidgetDrawCompass : public WidgetDrawMode
{
public:

	enum CompassElement
	{
		NORTH_WEST,
		NORTH_EAST,
		SOUTH_WEST,
		SOUTH_EAST,

		NORTH,
		SOUTH,
		WEST,
		EAST,

		BACKGROUND
	};

	//! Convers a compass element string to its enum type.
	static CompassElement string2CompassElement(const String &str);

public:

	//! default constructor
    WidgetDrawCompass(Context *context, BaseObject *parent);

	//! copy constructor
	WidgetDrawCompass(WidgetDrawCompass &dup);

	//! destructor
	virtual ~WidgetDrawCompass();

	//! Get the type of the widget mode
	virtual DrawModeType getType() const { return COMPASS_MODE; }

	//! Set a data set
	inline void setData(CompassElement elmt, Int32 x, Int32 y, Int32 width, Int32 height)
	{
		m_WidgetSet[elmt].x = x;
		m_WidgetSet[elmt].y = y;
		m_WidgetSet[elmt].width = width;
		m_WidgetSet[elmt].height = height;
	}
	//! Get a data set
	inline const WidgetDataSet& getData(CompassElement elmt) const { return m_WidgetSet[elmt]; }

	//! Get a data set width.
	inline Int32 getEltWidth(CompassElement elmt) const { return m_WidgetSet[elmt].width; }

	//! Get a data set height.
	inline Int32 getEltHeight(CompassElement elmt) const { return m_WidgetSet[elmt].height; }

	//! Get a data set x position.
	inline Int32 getElementX(CompassElement elmt) const { return m_WidgetSet[elmt].x; }

	//! Get a data set y position.
	inline Int32 getElementY(CompassElement elmt) const { return m_WidgetSet[elmt].y; }

	//! Get base width/height
	Int32 getWidgetWidth() const;
	Int32 getWidgetHeight() const;

	//! draw directly into the current buffer
	void draw(Int32 x, Int32 y, Int32 width, Int32 height);

protected:

	WidgetDataSet m_WidgetSet[9];
};

} // namespace o3d

#endif // _O3D_WIDGETDRAWCOMPASS_H
