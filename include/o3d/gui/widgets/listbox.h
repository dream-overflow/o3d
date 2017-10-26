/**
 * @file listbox.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_LISTBOX_H
#define _O3D_LISTBOX_H

#include "widget.h"
#include "boxlayout.h"
#include "scrollbar.h"

namespace o3d {

class ListBox;

/**
 * @brief List box element.
 * Contains a reference to a widget element, a meta-data, and reference to its list-box.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-10-01
 */
class O3D_API ListBoxElement
{
public:

	ListBoxElement(ListBox* listBox, Widget *widget);

	~ListBoxElement();

	//! Get the parent listbox object.
	ListBox* getListBox() { return m_listBox; }
	//! Get the parent listbox object (const version).
	const ListBox* getListBox() const { return m_listBox; }

	//! Get the widget object.
	Widget* getWidget() { return m_widget; }
	//! Get the widget object (const version).
	const Widget* getWidget() const { return m_widget; }

	//! Link a generic metadata object.
	void setMetadata(BaseObject *metadata);
	//! Get the related metadata object.
	BaseObject* getMetadata();
	//! Get the related metadata object (const version).
	const BaseObject* getMetadata() const;

	//! Set the position (0 based index) into the listbox.
	void setIndex(Int32 index) { m_index = index; }
	//! Get the position (0 based index) into the listbox.
	Int32 getIndex() const { return m_index; }

	//! Set a integer id for a own usage.
	void setId(Int32 id) { m_id = id; }
	//! Get the integer id for a own usage.
	Int32 getId() const { return m_id; }

	//! Set the widget selection into the list box.
	void setSelected(Bool selected) { m_selected = selected; }
	//! Is the widget selected into the list box.
	Int32 isSelected() const { return m_selected; }

private:

	Int32 m_index;
	Bool m_selected;

	ListBox *m_listBox;
	Widget *m_widget;

	Int32 m_id;
	SmartObject<BaseObject> m_metadata;
};

/**
 * @brief List box widget
 * The borders of the list box is based on the border of the Window of the theme.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-09-30
 */
class O3D_API ListBox : public Widget
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(ListBox)

	//! style that define the widget
	enum ListBoxStyle
	{
		EMPTY_STYLE = 0,             //!< Nothing
		BORDER_STYLE = 1,            //!< Draw borders
		VERTICALY_SCROLLABLE = 2,    //!< Can manage a vertical scrollbar
		HORIZONTALY_SCROLLABLE = 4,  //!< Can manager an horizontal scrollbar
		SELECTION = 8,               //!< Selection is allowed
		MULTI_SELECTION = 16,        //!< Using CTRL key multiple selection is allowed
		DEFAULT_STYLE = BORDER_STYLE | VERTICALY_SCROLLABLE | HORIZONTALY_SCROLLABLE
	};

	//! Create from a parent widget.
	//! @param parent Parent widget.
	ListBox(
			Widget *parent,
			const Vector2i &pos = DEFAULT_POS,
			const Vector2i &size = DEFAULT_SIZE,
			ListBoxStyle style = DEFAULT_STYLE,
			const String &name = "");

	//! Virtual destructor.
	virtual ~ListBox();

	//-----------------------------------------------------------------------------------
	// Children wigdets management
	//-----------------------------------------------------------------------------------

	/**
	 * @brief addWidget insert a widget before a specified position.
	 * @param widget Widget to add
	 * @param index Zero based index or -1 for the back of the list.
	 * @return A new ListBoxElement container
	 */
	ListBoxElement* addElement(Widget *widget, Int32 index = -1);

	//! retrieve an element object name (objects must have unique name for a valid result)
	ListBoxElement* findElement(const String &name);
	//! retrieve an element object name (objects must have unique name for a valid result) (const version)
	const ListBoxElement* findElement(const String &name) const;

	/**
	 * @brief get the nth element.
	 * @param index Zero based index
	 * @return element
	 * @throw E_IndexOutOfRange
	 */
	ListBoxElement* getElement(Int32 index);
	/**
	 * @brief get the nth element (const version).
	 * @param index Zero based index
	 * @return element
	 * @throw E_IndexOutOfRange
	 */
	const ListBoxElement* getElement(Int32 index) const;

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
	// Scrolling
	//-----------------------------------------------------------------------------------

	//! Set the vertical scroll bar page step.
	void setVerticalPageStep(Int32 step);
	//! Get the vertical scrollbar page step.
	Int32 getVerticalPageStep() const;

	//! Set the vertical scroll bar single step.
	void setVerticalSingleStep(Int32 step);
	//! Get the vertical scrollbar single step.
	Int32 getVerticalSingleStep() const;

	//! Set the horizontal scroll bar page step.
	void setHorizontalPageStep(Int32 step);
	//! Get the horizontal scrollbar page step.
	Int32 getHorizontalPageStep() const;

	//! Set the horizontal scroll bar single step.
	void setHorizontalSingleStep(Int32 step);
	//! Get the vertical scrollbar single step.
	Int32 getHorizontalSingleStep() const;

	//! Set the vertical position. -1 mean bottom.
	void setVerticalPos(Int32 y);
	//! Set the horizontal position. -1 mean right.
	void setHorizontalPos(Int32 x);

	//-----------------------------------------------------------------------------------
	// Widget
	//-----------------------------------------------------------------------------------

	//! Get the offset of the scrolling.
	virtual Vector2i getScrollPos() const;

	virtual Vector2i getOrigin() const;

	//! get the recommended widget default size
	virtual Vector2i getDefaultSize();

	//! Is widget targeted ?
	virtual Bool isTargeted(Int32 x, Int32 y, Widget *&widget);

	//! Mouse Events
	virtual Bool mouseLeftPressed(Int32 x,Int32 y);
	virtual Bool mouseLeftReleased(Int32 x,Int32 y);
	virtual Bool mouseMove(Int32 x,Int32 y);
	virtual void mouseMoveIn();
	virtual void mouseMoveOut();

	virtual Bool mouseWheel(Int32 x, Int32 y, Int32 z);

	//! Key event.
	virtual Bool keyboardToggled(Keyboard *keyboard, KeyEvent event);

	virtual void focused();
	virtual void lostFocus();

	virtual void sizeChanged();

	// Draw
	virtual void draw();
	virtual void updateCache();

	//-----------------------------------------------------------------------------------
	// Signals
	//-----------------------------------------------------------------------------------

	//! Called after a widget is selected
    Signal<ListBoxElement* /*element*/> onSelectElement{this};
	//! Called after a widget is deselected
    Signal<ListBoxElement* /*element*/> onDeselectElement{this};

	//! Called when after a widget position changed
    Signal<ListBoxElement* /*element*/> onElementPosChanged{this};

	//! Called after a widget is added
    Signal<ListBoxElement* /*element*/> onElementAdded{this};
	//! Called before a widget is removed
    Signal<ListBoxElement* /*element*/> onRemoveElement{this};

protected:

	ListBox(BaseObject *parent);

	ListBoxStyle m_listBoxStyle;

	typedef std::list<ListBoxElement*> T_ListBoxEltList;
	typedef T_ListBoxEltList::iterator IT_ListBoxEltList;
	typedef T_ListBoxEltList::const_iterator CIT_ListBoxEltList;

	T_ListBoxEltList m_sonList;    //!< list of elements (children)

	ScrollBar *m_horiScrollbar;
	ScrollBar *m_vertScrollbar;

	Vector2i m_limits;             //!< Client area limits coordinates
	ListBoxElement *m_lastSelection;

	void init();

	void updateScrollbars();
};

} // namespace o3d

#endif // _O3D_LISTBOX_H

