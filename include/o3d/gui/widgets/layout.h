/**
 * @file layout.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_LAYOUT_H
#define _O3D_LAYOUT_H

#include "widget.h"
#include "spacer.h"

namespace o3d {

class Window;
class LayoutItem;

/**
 * @brief A layout represent the way of organize widgets into a frame or a window.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-02-21
 * A layout represent the way of organize widgets into a frame or a window.
 * Some layout align horizontally, some others vertically, and some as grid.
 * O3DLayout is an abstract class.
 * Each frame or window own a default layout, (horizontal layout), that can be changed.
 * A layout can contain another layout. Widgets are inserted into a layout and not
 * directly into its parent widget.
 */
class O3D_API Layout : public Widget
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(Layout)

	//! default constructor
	Layout(Widget *widget);

	//! default constructor
	Layout(Window *window);

	//! construct as child layout
	Layout(Layout *layout);

	//! Virtual destructor.
	virtual ~Layout();

	//! Events Management
	virtual void sizeChanged();

	//! Is widget targeted ?
	virtual Bool isTargeted(Int32 x, Int32 y, Widget *&pWidget);

	// Draw
	virtual void draw();
	virtual void updateCache() {}

	virtual Vector2i getDefaultSize() { return Vector2i(-1,-1); }

	//! get the minimal widget size
    virtual Vector2i getMinSize();

	//-----------------------------------------------------------------------------------
	// Layout
	//-----------------------------------------------------------------------------------

	//! process the layout calculation
	void layout();

	//! Fit the window to the size of the content of the layout
	virtual Vector2i fit();

	//! Similar to Fit, but sizes the interior (virtual) size of a window
	virtual void fitInside();

	void setSizeHints();

	void setVirtualSizeHints();

	//! (Re)Compute the minimal size of the layout
	virtual Vector2i calcMin() = 0;
	//! (Re)Compute the size of the layout
	virtual void computeSizes() = 0;

	//! set the dimensions of the layout and perform the layout
	void setDimension(const Vector2i &pos, const Vector2i &size);

	//-----------------------------------------------------------------------------------
	// Children wigdets management
	//-----------------------------------------------------------------------------------

	//! insert a widget/layout in last position
	void addWidget(Widget *widget);

    //! insert a widget/layout before another
    void insertWidget(Widget *widget, const Widget *before);

	//! retrieve a widget/layout by its object name (objects must have unique name for a valid result)
	Widget* findWidget(const String &name);
	//! retrieve a widget/layout by its object name (objects must have unique name for a valid result) (const version)
	const Widget* findWidget(const String &name) const ;

	//! search for an existing widget/layout by its pointer
	//! @note search recursively if it is not a direct son
	//! @return true if success
	Bool isWidgetExist(Widget *widget) const;

	//! delete a widget/layout (and recursively all its sons)
	//! @return true if success
	Bool deleteWidget(Widget *widget);

	//! delete all widgets (and recursively all its sons)
	virtual void deleteAllWidgets();

	//! return the number of children.
    inline UInt32 getNumChildren() const { return (UInt32)m_sonList.size(); }

    //! Get the widget corresponding to next tab index or nullptr if none.
    virtual Widget* findNextTabIndex(Widget *widget, Int32 direction);

protected:

	//! default constructor
	Layout(BaseObject *parent);

	typedef std::list<Widget*> T_WidgetList;
	typedef T_WidgetList::iterator IT_WidgetList;
    typedef T_WidgetList::reverse_iterator RIT_WidgetList;
	typedef T_WidgetList::const_iterator CIT_WidgetList;

    Window *m_window;         //!< window containing this layout
    T_WidgetList m_sonList;   //!< list of widgets

	Vector2i getMinClientSize();
	Vector2i getMaxClientSize();
	Vector2i getMinWindowSize();
	Vector2i getMaxWindowSize();

	Vector2i virtualFitSize();
};

/**
 * @brief Widget/layout child element.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-02-21
 */
class O3D_API LayoutItem : NonCopyable<>
{
public:

	//! Item type.
	enum ItemType
	{
		ITEM_NONE,
		ITEM_WIDGET,
		ITEM_LAYOUT,
		ITEM_SPACER
	};

	//! Number of item type.
	static const Int32 NUM_ITEM_TYPE = 4;

	//! widget constructor
	LayoutItem(Widget *widget);

	//! layout constructor
	LayoutItem(Layout *layout);

	//! spacer constructor
	LayoutItem(Spacer *spacer);

	//-----------------------------------------------------------------------------------
	// Type
	//-----------------------------------------------------------------------------------

	//! is the item is a layout
	inline Bool isLayout() const { return m_itemType == ITEM_LAYOUT; }

	//! is the item is a window
	inline Bool isWidget() const { return m_itemType == ITEM_WIDGET; }

	//! is the item is a spacer
	inline Bool isSpacer() const { return m_itemType == ITEM_SPACER; }

	//-----------------------------------------------------------------------------------
	// Position
	//-----------------------------------------------------------------------------------

	//! get the top-left corner border surrounded position (const version)
	inline const Vector2i& getPos() const { return m_pos; }
	//! get the corner border surrounded position
	inline Vector2i& getPos() { return m_pos; }

	//-----------------------------------------------------------------------------------
	// Screen Rect
	//-----------------------------------------------------------------------------------

	//! get the on screen rect (const version)
	inline const Box2i& getRect() const { return m_rect; }
	//! get the on screen rect
	inline Box2i& getRect() { return m_rect; }

	//-----------------------------------------------------------------------------------
	// Ratio
	//-----------------------------------------------------------------------------------

	//! set the ratio
	inline void setRatio(Int32 width, Int32 height)
		{ m_ratio = (width && height) ? ((Float)width / (Float)height) : 1; }

	//! set the ratio
	inline void setRatio(const Vector2i size)
        { m_ratio = (size.x() && size.y()) ? ((Float)size.x() / (Float)size.y()) : 1; }

	//! set the ratio
	inline void setRatio(Float ratio) { m_ratio = ratio; }

	//! get the ratio
	inline Float getRatio() const { return m_ratio; }

	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	//! return the minimal size of the widget item
	Vector2i calcMin();

	//! set the layout item dimensions
	void setDimension(const Vector2i &pos, const Vector2i &size);

	//! get the minimal widget size including borders
	Vector2i getMinSizeIncludeBorders() const;

private:

	ItemType m_itemType;

	union
	{
		Widget *m_pWidget;
		Layout *m_pLayout;
		Spacer *m_pSpacer;
	};

	Vector2i m_minSize;     //!< computed widget min size
	Vector2i m_pos;         //!< top-left corner of the surrounding border

	Box2i m_rect;           //!< on screen rectangle

	//! Aspect ratio can always be calculated from m_size, but this would cause precision
	//! loss when the window is shrunk. It is safer to preserve the initial value.
	Float m_ratio;

	//! define the widget
	void setWidget(Widget *widget);

	//! define the layout
	void setLayout(Layout *layout);

	//! define the space
	void setSpacer(Spacer *spacer);
};

} // namespace o3d

#endif // _O3DLAYOUT_H

