/**
 * @file boxlayout.h
 * @brief A simple vertical or horizontal box layout.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-02-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_BOXLAYOUT_H
#define _O3D_BOXLAYOUT_H

#include "layout.h"

namespace o3d {

class Window;

//---------------------------------------------------------------------------------------
//! @class BoxLayout
//-------------------------------------------------------------------------------------
//! A simple vertical or horizontal box layout.
//! @see Layout
//---------------------------------------------------------------------------------------
class O3D_API BoxLayout : public Layout
{
public:

	O3D_DECLARE_CLASS(BoxLayout)

	//! Orientation of the layout
	enum Orient
	{
		HORIZONTAL_LAYOUT,
		VERTICAL_LAYOUT
	};

	//! Construct from a parent widget.
	BoxLayout(Widget *widget, Orient orient = VERTICAL_LAYOUT);

	//! Construct from a parent window.
	BoxLayout(Window *window, Orient orient = VERTICAL_LAYOUT);

	//! Construct from a parent layout.
	BoxLayout(Layout *layout, Orient orient = VERTICAL_LAYOUT);

	//-----------------------------------------------------------------------------------
	// Orientation
	//-----------------------------------------------------------------------------------

	//! set the orientation
	void setOrientation(Orient orient);
	//! get the orientation
	inline Orient getOrientation(Orient orient) { return m_orient; }

	//-----------------------------------------------------------------------------------
	// Layout processing
	//-----------------------------------------------------------------------------------

	//! (Re)Compute the minimal size of the layout
	virtual Vector2i calcMin();
	//! (Re)Compute the size of the layout
	virtual void computeSizes();

protected:

	//! Default constructor.
	BoxLayout(BaseObject *parent);

	Orient m_orient;   //!< orientation of the layout

	Int32 m_stretchable;

	Int32 m_minWidth;
	Int32 m_minHeight;
	Int32 m_fixedWidth;
	Int32 m_fixedHeight;
};

} // namespace o3d

#endif // _O3D_BOXLAYOUT_H

