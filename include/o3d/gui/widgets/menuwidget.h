/**
 * @file menuwidget.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MENUWIDGET_H
#define _O3D_MENUWIDGET_H

#include "scrollbar.h"
#include "../textzone.h"

namespace o3d {

/**
 * @brief MenuLabelItem
 * A simple targetable and hoverable menu entry containing only a left aligned label.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-10-13
 */
class O3D_API MenuLabelItem : public Widget
{
public:

	O3D_DECLARE_CLASS(MenuLabelItem)

	MenuLabelItem(
			Widget *parent,
			const String &label,
			const Vector2i &pos = DEFAULT_POS,
			const Vector2i &size = DEFAULT_SIZE,
			const String& name = "");

	//! Virtual destructor.
	virtual ~MenuLabelItem();

	//! Get the label content.
	inline const String& getLabel() const { return m_text.text(); }

	//-----------------------------------------------------------------------------------
	// Events and updates
	//-----------------------------------------------------------------------------------

	//! get the recommended widget default size
    virtual Vector2i getDefaultSize() override;

	//! Enable/disable the widget.
    virtual void enable(Bool active = True) override;

	//! check if the widget is targeted and return true and non null widget if a target is found
    virtual Bool isTargeted(Int32 x, Int32 y, Widget *&widget) override;

	// Mouse events. coordinates are relative to parent origin
    virtual Bool mouseLeftPressed(Int32 x, Int32 y) override;
    virtual Bool mouseLeftReleased(Int32 x, Int32 y) override;
    virtual Bool mouseMove(Int32 x,Int32 y) override;
    virtual void mouseMoveIn() override;
    virtual void mouseMoveOut() override;

    virtual Bool keyboardToggled(Keyboard *keyboard, KeyEvent event) override;

    virtual void sizeChanged() override;

    virtual void draw() override;
    virtual void updateCache() override;

	//! Signal on menu selected.
    Signal<> onSelectMenu{this};

protected:

	MenuLabelItem(BaseObject *parent);

	TextZone m_text;

	void adjustBestSize();
};

// TODO add class O3D_API MenuSpacerItem : public Widget

/**
 * @brief MenuWidget
 * Menu form, using of a kind of list box with separators widgets, entry widgets.
 * A menu never have horizontal scrolling. Only vertical scrolling is possible.
 * The width is adjusted to the largest item.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-10-13
 */
class O3D_API MenuWidget : public Widget
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(MenuWidget)

	//! style that define the widget
	enum MenuWidgetStyle
	{
		DEFAULT_STYLE = 0
	};

	//! Create from a parent widget.
	//! @param parent Parent widget.
	MenuWidget(
			Widget *parent,
			const Vector2i &pos = DEFAULT_POS,
			const Vector2i &size = DEFAULT_SIZE,
			MenuWidgetStyle style = DEFAULT_STYLE,
			const String &name = "");

	//! Virtual destructor.
	virtual ~MenuWidget();

	//-----------------------------------------------------------------------------------
	// Children wigdets management
	//-----------------------------------------------------------------------------------

	/**
	 * @brief addWidget insert a widget before a specified position.
	 * @param widget Widget to add
	 * @param index Zero based index or -1 for the back of the list.
	 * @return A new ListBoxElement container
	 */
	void addElement(Widget *widget, Int32 index = -1);

	//! retrieve an element object name (objects must have unique name for a valid result)
	Widget* findElement(const String &name);
	//! retrieve an element object name (objects must have unique name for a valid result) (const version)
	const Widget* findElement(const String &name) const;

	/**
	 * @brief get the nth element.
	 * @param index Zero based index
	 * @return element
	 * @throw E_IndexOutOfRange
	 */
	Widget* getElement(Int32 index);
	/**
	 * @brief get the nth element (const version).
	 * @param index Zero based index
	 * @return element
	 * @throw E_IndexOutOfRange
	 */
	const Widget* getElement(Int32 index) const;

	//! retrieve a widget/layout by its object name (objects must have unique name for a valid result)
	Widget* findWidget(const String &name);
	//! retrieve a widget/layout by its object name (objects must have unique name for a valid result) (const version)
	const Widget* findWidget(const String &name) const;

	//! search for an existing widget/layout by its pointer
	//! @note search recursively if it is not a direct son
	//! @return true if success
	Bool isWidgetExist(Widget *widget) const;

	//! delete a widget/layout (and recursively all its sons)
	//! @return true if success
	Bool deleteWidget(Widget *widget);

	//! delete an element at a specified position (0 based index).
	//! @return true if success
	Bool deleteElement(Int32 index);

	//! delete all widgets elements (and recursively all theirs sons)
	virtual void deleteAllElements();

	//! return the number of children.
	inline UInt32 getNumChildren() const { return (UInt32)m_sonList.size(); }

	//-----------------------------------------------------------------------------------
	// Widget
	//-----------------------------------------------------------------------------------

	//! get the widget client size. the client area is the area which may be drawn on by
	//! the programmer (excluding title bar, border, scrollbars, etc)
    virtual Vector2i getClientSize() const override;

	//! Get the offset of the scrolling.
    virtual Vector2i getScrollPos() const override;

    virtual Vector2i getOrigin() const override;

	//! get the recommended widget default size
    virtual Vector2i getDefaultSize() override;

	//! Is widget targeted ?
    virtual Bool isTargeted(Int32 x, Int32 y, Widget *&widget) override;

	//! Mouse Events
    virtual Bool mouseLeftPressed(Int32 x,Int32 y) override;
    virtual Bool mouseLeftReleased(Int32 x,Int32 y) override;
    virtual Bool mouseMove(Int32 x,Int32 y) override;
    virtual void mouseMoveIn() override;
    virtual void mouseMoveOut() override;

    virtual Bool mouseWheel(Int32 x, Int32 y, Int32 z) override;

	//! Key event.
    virtual Bool keyboardToggled(Keyboard *keyboard, KeyEvent event) override;
	//! Character event.
    virtual Bool character(Keyboard *keyboard, CharacterEvent event) override;

    virtual void focused() override;
    virtual void lostFocus() override;

    virtual void sizeChanged() override;

	// Draw
    virtual void draw() override;
    virtual void updateCache() override;

	//-----------------------------------------------------------------------------------
	// Signals
	//-----------------------------------------------------------------------------------

	//! Called after a widget is selected
    Signal<Widget* /*menu item*/> onSelectMenuItem{this};

protected:

	MenuWidget(BaseObject *parent);

	MenuWidgetStyle m_menuWidgetStyle;

	typedef std::list<Widget*> T_MenuItemList;
	typedef T_MenuItemList::iterator IT_MenuItemList;
	typedef T_MenuItemList::const_iterator CIT_MenuItemList;

	T_MenuItemList m_sonList;    //!< list of elements (children)

	ScrollBar *m_vertScrollbar;

	Vector2i m_limits;             //!< Client area limits coordinates

	void init();
	void updateScrollbar();
};

} // namespace o3d

#endif // _O3D_MENUWIDGET_H
