/**
 * @file tabbedwidget.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TABBEDWIDGET_H
#define _O3D_TABBEDWIDGET_H

#include "toolbutton.h"
#include "../textzone.h"

namespace o3d {

class ListBoxElement;

/**
 * @brief TabbedWidgetData
 * Inherit you own class to manage it with a tab. This class is deleted at tab destruction.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-10-11
 */
class O3D_API TabbedWidgetData
{
public:

	virtual ~TabbedWidgetData() {}
};

/**
 * @brief A tabbed widget managed many tab and theirs states.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-09-11
 */
class O3D_API TabbedWidget : public Widget
{
public:

	O3D_DECLARE_CLASS(TabbedWidget)

	enum TabState
	{
		TAB_DISABLED,
		TAB_NORMAL,
		TAB_ACTIVE,
		TAB_HOVER
	};

	/**
	 * @brief The Tab class
	 */
	class Tab
	{
	public:

		Tab();
		~Tab();

		Int32 id;           //!< Unique identifier
		UInt32 order;       //!< Position in the list

        Vector2i pos;       //!< Relative to the widget
		Vector2i size;

        TabState state;     //!< Current state for this tab

        TextZone label;     //!< Text of the tab
        Icon *icon;         //!< optional icon

		Bool activable;     //!< If false the tab is not selectionable

		TabbedWidgetData *data; //!< Related data object
	};

	//! Initialization constructor.
	TabbedWidget(
		Widget *parent,
		const Vector2i &pos = DEFAULT_POS,
		const Vector2i &size = DEFAULT_SIZE,
		const String &name = "");

	//! Virtual destructor.
	virtual ~TabbedWidget();

	//-----------------------------------------------------------------------------------
	// Properties
	//-----------------------------------------------------------------------------------

	/**
	 * @brief addTab Add a new tab at the end.
	 * @param label Text of the tab
	 * @param icon Optional icon drawn at left of the text
	 * @param closable If true a close button is drawn (@see onCloseTab signal)
	 * @param data Attached data, deleted at tab destruction
	 * @return the new tab identifier.
	 */
	Int32 addTab(
			const String &label,
			Icon *icon = nullptr,
			Bool closable = False,
			TabbedWidgetData *data = nullptr);

	/**
	 * @brief insertTab Similar to addTab but insert before the specified position.
	 * @param pos if the pos is outside the range an exception is thrown
	 * @param label
	 * @param icon
	 * @param closable
	 * @param data Attached data, deleted at tab destruction
	 * @return the new tab identifier.
	 */
	Int32 insertTab(Int32 setPos,
			const String &label,
			Icon *icon = nullptr,
			Bool closable = False,
			TabbedWidgetData *data = nullptr);

	//! Remove the tab having the specified identifier.
	void removeTab(Int32 tabId);

	//! Remove the tab at the specified position.
	void removeTabAt(UInt32 tabPos);

	//! Define the active tab.
	void setActiveTab(Int32 id);

	//! Get the number of tabs.
	Int32 getNumTabs() const { return m_tabs.size(); }

	//! Get the tab for the tab index.
	const Tab* getTab(Int32 index) const;

	//-----------------------------------------------------------------------------------
	// Properties
	//-----------------------------------------------------------------------------------

	//! get the recommended widget default size
	virtual Vector2i getDefaultSize();

	//! get the current active tab id or -1 if none.
	Int32 getActiveTab() const;

	//! get the current active tab related data or nullptr if none.
	TabbedWidgetData* getActiveTabData();

	//! get the current active tab related data or nullptr if none.
	const TabbedWidgetData *getActiveTabData() const;

//	//! set the button text
//	void setText(Int32 tabId, const String &text);

//	//! get the button text
//	const String &getText(Int32 tabId) const;

//	//! Set the button icon (override the button text).
//	void setIcon(Int32 tabId, const Icon &icon);

	//------------------------------------------------------------------------------------
	// Widget
	//-----------------------------------------------------------------------------------

	//! Is widget targeted ?
	virtual Bool isTargeted(Int32 x, Int32 y, Widget *&widget);

	//! Mouse Events
	virtual Bool mouseLeftPressed(Int32 x,Int32 y);
	virtual Bool mouseLeftReleased(Int32 x,Int32 y);
	virtual Bool mouseMove(Int32 x,Int32 y);
	virtual void mouseMoveIn();
	virtual void mouseMoveOut();

	virtual void focused();
	virtual void lostFocus();

	virtual void sizeChanged();

	// Draw
	virtual void draw();
	virtual void updateCache();

public:

	//! Emitted when the tab is selected (normal to active state) on left mouse click.
    Signal<Int32> onActivateTab{this};

	//! Emitted when a tab is deselected (active to normal state).
    Signal<Int32> onReleaseTab{this};

protected:

	//! Default constructor.
	TabbedWidget(BaseObject *parent);

	enum TabbedWidgetCapacities
	{
		STATE_PRESSED = CAPS_WIDGET_LAST + 1,
		STATE_WAS_PRESSED,
		STATE_HOVER,
		STATE_PUSHED
	};

	Bool m_pressed;
	Bool m_wasPressed;
	Bool m_pushed;
	Bool m_hover;

	Tab *m_activeTab;
	Tab *m_hoverTab;

	typedef std::vector<Tab*> T_TabVector;
	typedef T_TabVector::iterator IT_TabVector;
	typedef T_TabVector::const_iterator CIT_TabVector;

	T_TabVector m_tabs;

	Int32 m_curX;

	AutoPtr<ToolButton> m_dropDownButton;  //!< Button to show the drop down list of hidded tabs

	void init();
	void adjustBestSize();

	//! Slot on drop down button pressed
	void dropDownButton();

	//! Slot on drop down select tab
    void menuSelectTab(Widget* widget);
};

} // namespace o3d

#endif // _O3D_TABBEDWIDGET_H

