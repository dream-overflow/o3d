/**
 * @file tabbedpane.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TABBEDPANE_H
#define _O3D_TABBEDPANE_H

#include "widget.h"
#include "tabbedwidget.h"

namespace o3d {

class TabbedPane;

/**
 * @brief TabbedPaneElement
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-10-11
 */
class O3D_API TabbedPaneElement : public TabbedWidgetData
{
public:

	TabbedPaneElement(TabbedPane *pane, Widget *widget);

	~TabbedPaneElement();

	//! Get the parent tabbed pane object.
	TabbedPane* getTabbedPane() { return m_tabbedPane; }
	//! Get the parent tabbed pane object (const version).
	const TabbedPane* getTabbedPane() const { return m_tabbedPane; }

	//! Get the widget object.
	Widget* getWidget() { return m_widget; }
	//! Get the widget object (const version).
	const Widget* getWidget() const { return m_widget; }

	//! Link a generic metadata object.
	void setMetadata(NullClass *metadata);
	//! Get the related metadata object.
	NullClass* getMetadata();
	//! Get the related metadata object (const version).
	const NullClass* getMetadata() const;

	//! Set the position (0 based index) into the tabbed pane.
	void setIndex(Int32 index) { m_index = index; }
	//! Get the position (0 based index) into the tabbed pane.
	Int32 getIndex() const { return m_index; }

private:

	Int32 m_index;

	TabbedPane *m_tabbedPane;
	Widget *m_widget;

	AutoPtr<NullClass> m_metadata;
};

/**
 * @brief TabbedPane
 * Component containing a pane with a tabbed component. A single tab is shown at time.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-10-11
 */
class O3D_API TabbedPane : public Widget
{
public:

	O3D_DECLARE_CLASS(TabbedPane)

	enum TabbedPaneStyle
	{
		EMPTY_STYLE = 0,
		BORDER_STYLE = 1,
		DEFAULT_STYLE = BORDER_STYLE
	};

	TabbedPane(
			Widget *parent,
			const Vector2i &pos = DEFAULT_POS,
			const Vector2i &size = DEFAULT_SIZE,
			TabbedPaneStyle style = DEFAULT_STYLE,
			const String &name = "");

	virtual ~TabbedPane();

	//-----------------------------------------------------------------------------------
	// Properties
	//-----------------------------------------------------------------------------------

	//! get the tabbed pane style
	inline TabbedPaneStyle getTabbedPaneStyle() const { return m_tabbedPaneStyle; }

	/**
	 * @brief addTab Add a new tab at the end.
	 * @param widget Widget to draw with this tab
	 * @param label Text of the tab
	 * @param icon Optional icon drawn at left of the text
	 * @param closable If true a close button is drawn (@see onCloseTab signal)
	 * @return the new tab element.
	 */
	TabbedPaneElement* addTab(
			Widget *widget,
			const String &label,
			Icon *icon = nullptr,
			Bool closable = False);

	/**
	 * @brief insertTab Similar to addTab but insert before the specified position.
	 * @param widget Widget to draw with this tab
	 * @param pos if the pos is outside the range an exception is thrown
	 * @param label
	 * @param icon
	 * @param closable
	 * @return the new tab element.
	 */
	TabbedPaneElement* insertTab(
			Widget *widget,
			Int32 setPos,
			const String &label,
			Icon *icon = nullptr,
			Bool closable = False);

	//! Remove the tab having the specified identifier.
	void removeTab(Int32 tabId);

	//! Remove the tab at the specified position.
	void removeTabAt(UInt32 tabPos);

	//! Define the active tab.
	void setActiveTab(Int32 id);

	//! get the current active tab id or -1 if none.
	Int32 getActiveTab() const;

	//! get the current active tab element or nullptr if none.
	TabbedPaneElement* getActiveTabElt() const;

	//! get the number of tab.
	Int32 getNumTabs() const { return m_tabs->getNumTabs(); }

	//! get the tab for the tab index.
	const TabbedPaneElement* getTab(Int32 index) const;
	//! get the tab for the tab index.
	TabbedPaneElement* getTab(Int32 index);

	//------------------------------------------------------------------------------------
	// Widget
	//-----------------------------------------------------------------------------------

	//! get the recommended widget default size
	virtual Vector2i getDefaultSize();

	//! get the widget client size. the client area is the area which may be drawn on
	virtual Vector2i getClientSize() const;

	virtual Vector2i getOrigin() const;

	//! Is widget targeted ?
	virtual Bool isTargeted(Int32 x, Int32 y, Widget *&widget);

	//! Mouse Events
    virtual Bool mouseWheel(Int32 x, Int32 y, Int32 z);
	virtual void mouseMoveIn();
	virtual void mouseMoveOut();

	virtual void focused();
	virtual void lostFocus();

	virtual void sizeChanged();

    //! Get the widget corresponding to next tab index or nullptr if none.
    virtual Widget* findNextTabIndex(Widget *widget, Int32 direction);


	// Draw
	virtual void draw();
	virtual void updateCache();

	// When a tab is selected
    void activateTab(Int32 tabIndex);

	// When a tab is released
    void releaseTab(Int32 tabIndex);

public:

	//! Emitted when the tab is selected (normal to active state) on left mouse click.
    Signal<TabbedPaneElement*> onActivateTab{this};

	//! Emitted when a tab is deselected (active to normal state).
    Signal<TabbedPaneElement*> onReleaseTab{this};

private:

	TabbedPane(BaseObject *parent);

	TabbedPaneStyle m_tabbedPaneStyle;
	AutoPtr<TabbedWidget> m_tabs;

	Box2i m_borderRect;

	void init();
};

} // namespace o3d

#endif // _O3D_TABBEDPANE_H

