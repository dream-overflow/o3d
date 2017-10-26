/**
 * @file wraplayout.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_WRAPLAYOUT_H
#define _O3D_WRAPLAYOUT_H

#include "layout.h"
#include "scrollbar.h"

namespace o3d {

/**
 * @brief Wrap layout.
 * It that manage elements horizontaly or verticaly, line by line or colunm by colmun,
 * adding a new line or columun when there is no more space available.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-09-23
 */
class O3D_API WrapLayout : public Layout
{
public:

	O3D_DECLARE_CLASS(WrapLayout)

	//! Orientation of the layout
	enum Orient
	{
		HORIZONTAL_LAYOUT,
		VERTICAL_LAYOUT
	};

	//! Construct from a parent widget.
	WrapLayout(Widget *widget, Orient orient = VERTICAL_LAYOUT);

	//! Construct from a parent window.
	WrapLayout(Window *window, Orient orient = VERTICAL_LAYOUT);

	//! Construct from a parent layout.
	WrapLayout(Layout *layout, Orient orient = VERTICAL_LAYOUT);

	virtual ~WrapLayout();

	//! delete all widgets (and recursively all its sons)
	virtual void deleteAllWidgets();

	//! Get the offset of the scrolling.
	virtual Vector2i getScrollPos() const;

	//! Is widget targeted ?
	virtual Bool isTargeted(Int32 x, Int32 y, Widget *&pWidget);

	//-----------------------------------------------------------------------------------
	// Scrollable
	//-----------------------------------------------------------------------------------

	//! Set the scrollbar single step in pixels (default is -1). -1 mean auto compute.
	void setScrollBarSingleStep(Int32 step);

	//! Set the scrollbar page step in pixels (default is -1). -1 mean auto compute.
	void setScrollBarPageStep(Int32 step);

	//-----------------------------------------------------------------------------------
	// Layout orientation
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

	//-----------------------------------------------------------------------------------
	// Widget processing
	//-----------------------------------------------------------------------------------

	//! Draw the widget and all of its sons
	virtual void draw();

	virtual Bool mouseWheel(Int32 x, Int32 y, Int32 z);

	//! Slot when the scroll bar is moved.
    void sliderMoved(Int32 setPos);

protected:

	//! Default constructor.
	WrapLayout(BaseObject *parent);

	Orient m_orient;   //!< orientation of the layout

	Int32 m_minWidth;
	Int32 m_minHeight;

	Int32 m_scrollbarSingleStep;
	Int32 m_scrollbarPageStep;
	Int32 m_maxEltSize;  //!< Larger element size in the orientation of the layout

    AutoPtr<ScrollBar> m_scrollbar;  //! Horizontal or vertical scroll bar
};

} // namespace o3d

#endif // _O3D_WRAPLAYOUT_H

