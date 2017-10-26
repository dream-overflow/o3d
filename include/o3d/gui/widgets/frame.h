/**
 * @file frame.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FRAME_H
#define _O3D_FRAME_H

#include "widget.h"

namespace o3d {

class WidgetManager;
class Layout;

 /**
 * @brief Frame.
 * A widget representing a frame. This widget can contain a layout definition which can
 * contain some children widgets. A frame is not a top level widget.
 * The borders of the frame is based on the border of the Window of the theme.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-10-01
 */
class O3D_API Frame : public Widget
{
public:

	O3D_DECLARE_CLASS(Frame)

	//! style that define a frame widget
	enum FrameStyle
	{
		EMPTY_STYLE = 0,
		BORDER_STYLE = 1,
		DEFAULT_STYLE = BORDER_STYLE
	};

	//! Construct as a child frame.
	Frame(
		Widget *parent,
		const Vector2i &pos = DEFAULT_POS,
		const Vector2i &size = DEFAULT_SIZE,
		FrameStyle style = DEFAULT_STYLE,
		const String &name = "");

	//! Virtual destructor.
	virtual ~Frame();


	//-----------------------------------------------------------------------------------
	// Layout
	//-----------------------------------------------------------------------------------

	//! set the parent layout. By default a vertical BoxLayout is setup.
	void setLayout(Layout *layout);

	//! get the parent layout. By default a vertical BoxLayout is setup. (const version)
	virtual const Layout* getLayout() const;
	//! get the parent layout. By default a vertical BoxLayout is setup.
	virtual Layout* getLayout();

	//! get the recommended widget default size
	virtual Vector2i getDefaultSize();

	//! get the widget client size. the client area is the area which may be drawn on by
	//! the programmer (excluding title bar, border, scrollbars, etc)
	virtual Vector2i getClientSize() const;

	//-----------------------------------------------------------------------------------
	// Widget
	//-----------------------------------------------------------------------------------

	virtual Vector2i getOrigin() const;

    //! Get the widget corresponding to next tab index or nullptr if none.
    virtual Widget* findNextTabIndex(Widget *widget, Int32 direction);


	//-----------------------------------------------------------------------------------
	// Events
	//-----------------------------------------------------------------------------------

	//! Is widget targeted ?
	Bool isTargeted(Int32 x, Int32 y, Widget *&widget);

	//! Others Event
	void focused();
	void lostFocus();
	void sizeChanged();

	// Draw
	virtual void draw();
	virtual void updateCache();

protected:

	//! Default constructor.
	Frame(BaseObject *parent);

	AutoPtr<Layout> m_pLayout;  //!< Layout used to manage the frame content

	FrameStyle m_style;

	void init();
};

} // namespace o3d

#endif // _O3D_FRAME_H

