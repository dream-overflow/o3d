/**
 * @file window.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_WINDOW_H
#define _O3D_WINDOW_H

#include "widget.h"
#include "../textzone.h"

#include <list>

namespace o3d {

class WidgetManager;
class Layout;

/**
 * @brief A top-level window
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-09-18
 * A widget representing a top-level window. This widget can contain a layout
 * definition which can contain some children widgets.
 */
class O3D_API Window : public Widget
{
public:

	O3D_DECLARE_CLASS(Window)

	//! Top-level widget center mode
	enum CenterType
	{
		CENTER_VERTICALY = 1,
		CENTER_HORIZONTALY = 2,
		CENTER_ON_SCREEN = 4,    //!< Center on screen and not from parent
		CENTER_BOTH = CENTER_VERTICALY | CENTER_HORIZONTALY
	};

	//! style that define a window widget
	enum WindowStyle
	{
        EMPTY_STYLE = 0,               //!< Nothing is drawn
        BORDER_STYLE = 1,              //!< Borders and background
        TITLE_ICON = 2,
        TITLE_BAR = 4,
        CLOSE_BUTTON = 8,
        REDUCE_BUTTON = 16,
        MAXIMIZE_RESTORE_BUTTON = 32,
        MOVABLE_WINDOW = 64,
        RESIZABLE_WINDOW = 128,

        DEFAULT_STYLE = BORDER_STYLE | TITLE_ICON | TITLE_BAR | CLOSE_BUTTON |
			REDUCE_BUTTON | MAXIMIZE_RESTORE_BUTTON |
			CLOSE_BUTTON | MOVABLE_WINDOW | RESIZABLE_WINDOW,

		FIXED_WINDOW_STYLE = TITLE_ICON | TITLE_BAR
	};

	//! Creation constructor as a child window
	Window(
		Widget *parent,
		const String &title,
		const Vector2i &pos = DEFAULT_POS,
		const Vector2i &size = DEFAULT_SIZE,
        Int32 windowStyle = DEFAULT_STYLE,
		const String &name = "");

	//! Creation constructor as a top-level window
	Window(
		WidgetManager *parent,
		const String &title,
		const Vector2i &pos = DEFAULT_POS,
		const Vector2i &size = DEFAULT_SIZE,
        Int32 windowStyle = DEFAULT_STYLE,
		const String &name = "");

	//! Virtual destructor.
	virtual ~Window();


	//-----------------------------------------------------------------------------------
	// Layout
	//-----------------------------------------------------------------------------------

	//! set the parent layout. By default an O3DHLayout is setup.
	void setLayout(Layout *layout);

	//! get the parent layout. By default an O3DHLayout is setup. (const version)
    virtual const Layout* getLayout() const override;
	//! get the parent layout. By default an O3DHLayout is setup.
    virtual Layout* getLayout() override;

	//! Initially fit to content. this property affect the next Layout call only
	inline void setFitToContent(Bool fit) { m_capacities.setBit(CAPS_FIT_TO_CONTENT, fit); }
	inline Bool isFitToContent() const { return m_capacities.getBit(CAPS_FIT_TO_CONTENT); }

	//! get the recommended widget default size
    virtual Vector2i getDefaultSize() override;

	//! get the widget client size. the client area is the area which may be drawn on by
	//! the programmer (excluding title bar, border, scrollbars, etc)
    virtual Vector2i getClientSize() const override;


	//-----------------------------------------------------------------------------------
	// Properties
	//-----------------------------------------------------------------------------------

	//! set the title
	void setTitle(const String &title);
	//! get the title
    inline const String& title() const { return m_title.text(); }

	//! set the widget always on top
    virtual void setAlwaysOnTop(Bool onTop = True) override;
	//! is the widget always on top
    virtual Bool isAlwaysOnTop() const override;

	//! set the widget as modal
    virtual void setModal(Bool modal = True) override;
	//! is the widget is modal
    virtual Bool isModal() const override;

	//! Get the current cursor type name for this widget if targeted.
    virtual String getCursorTypeName() const override;

    virtual void show(Bool show) override;

	//-----------------------------------------------------------------------------------
	// Widget
	//-----------------------------------------------------------------------------------

    virtual Vector2i getOrigin() const override;

	//! get the border size of the window depending of its defined theme
	//! the border size is the theme borders size and the title bar height
    virtual Vector2i getWindowBorderSize() const override;

	//! center the widget
	//! @note O3DWinManager::Reshape or Scene::getViewPortManager()->Reshape must be defined with
	//! the correct size to center a toplevel widget or to process a CenterOnScreen.
	void center(CenterType center = CENTER_BOTH);

    //! Get the widget corresponding to next tab index or nullptr if none.
    virtual Widget* findNextTabIndex(Widget *widget, Int32 direction) override;

    //! Get the previously focused widget.
    virtual Widget* getPreviouslyFocusedWidget() override;
    //! Set the previously focused widget.
    virtual void setPreviouslyFocusedWidget(Widget *widget) override;


	//-----------------------------------------------------------------------------------
	// Events
	//-----------------------------------------------------------------------------------

	//! Is widget targeted ?
    virtual Bool isTargeted(Int32 x,Int32 y,Widget *&widget) override;

	//! for move or resize
    virtual Bool mouseLeftPressed(Int32 x,Int32 y) override;
    virtual Bool mouseLeftReleased(Int32 x,Int32 y) override;
    virtual Bool mouseMove(Int32 x,Int32 y) override;

    virtual Bool keyboardToggled(Keyboard *keyboard, KeyEvent event) override;

	//! Others Event
    virtual void focused() override;
    virtual void lostFocus() override;
    virtual void sizeChanged() override;
    virtual void positionChanged() override;


	// Draw
    virtual void draw() override;
    virtual void updateCache() override;

public:

	//! Called on window reduce button pressed
    Signal<> onWindowReduce{this};

	//! Called on window restore button pressed when the window was reduced or maximized
    Signal<> onWindowRestore{this};

	//! Called on window reduce button pressed when the window was reduce or in normal size
    Signal<> onWindowMaximize{this};

	//! Called on window reduce button pressed
    Signal<> onWindowClose{this};

protected:

	//! Creation constructor.
	Window(BaseObject *parent);

	enum Resize
	{
		RESIZE_NONE = 0,
		RESIZE_TOP = 1,
		RESIZE_LEFT = 2,
		RESIZE_RIGHT = 4,
		RESIZE_BOTTOM = 8
	};

	enum _WindowState
	{
		_STATE_NORMAL,
		_STATE_CLOSED,
		_STATE_REDUCED,
		_STATE_MAXIMIZED,
		_STATE_MOVING,
		_STATE_RESIZING
	};

	enum _UpdatePart
	{
		_UPDATE_TITLE_BAR = 1,
		_UPDATE_CLIENT = 2,
		_UPDATE_BOTH = _UPDATE_TITLE_BAR | _UPDATE_CLIENT
	};

	enum CapacitiesWindow
	{
		STATE_MOVING = CAPS_WIDGET_LAST + 1,
		STATE_RESIZING,
		CAPS_FIT_TO_CONTENT,
		CAPS_ALWAYS_ON_TOP,
		CAPS_MODAL
	};

	AutoPtr<Layout> m_pLayout;  //!< Layout used to manage the window content

    WindowStyle m_windowStyle;

	Int32 m_resize;

	Vector2i m_processAction; //!< last pos for move or resize

	TextZone m_title;
	Widget *m_icon;

	_WindowState m_windowState;

	Widget *m_reduceButton;
	Widget *m_maximizeButton;
	Widget *m_closeButton;

	Int32 m_updatePart;

    Widget *m_prevFocusWidget;  //!< when window goes to background

    typedef std::list<Widget*> T_WidgetList;
    typedef T_WidgetList::iterator IT_WidgetList;
    typedef T_WidgetList::reverse_iterator RIT_WidgetList;

    T_WidgetList m_tabIndexWidgets;   //!< list of widgets orderered by tab index

	void init();

	void reducePressed();
	void maximizePressed();
	void closePressed();

    virtual void updateTabIndex(Widget *widget) override;

    void onDeleteWidget(BaseObject *object);
};

typedef std::list<Window*> T_WindowList;
typedef T_WindowList::iterator IT_WindowList;
typedef T_WindowList::const_iterator CIT_WindowList;
typedef T_WindowList::reverse_iterator RIT_WindowList;
typedef T_WindowList::const_reverse_iterator CRIT_WindowList;

} // namespace o3d

#endif // _O3D_WINDOW_H
