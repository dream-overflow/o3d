/**
 * @file widgetmanager.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_WIDGETMANAGER_H
#define _O3D_WIDGETMANAGER_H

#include "o3d/core/memorydbg.h"
#include "o3d/core/keyboard.h"
#include "o3d/core/mouse.h"

#include "widgets/window.h"
#include "widgets/rootwindow.h"

#include "console.h"
#include "messagebox.h"

#include <list>

namespace o3d {

/**
 * @brief Window ordering by depth and widgets management.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-07-31
 */
class O3D_API WidgetManager : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(WidgetManager)

	//! default constructor
	WidgetManager(BaseObject *parent);

	//! destructor
	virtual ~WidgetManager();

    //! init after ctor.
    void init();

	//-----------------------------------------------------------------------------------
	// Widgets management
	//-----------------------------------------------------------------------------------

	//! Delete an existing window.
	//! @return TRUE if success.
	Bool deleteWindow(Window* window);

	//! Create a new top-level window
	Window* createTopLevelWindow(
		const String &title,
		const Vector2i &pos = Widget::DEFAULT_POS,
		const Vector2i &size = Widget::DEFAULT_SIZE,
		Window::WindowStyle style = Window::DEFAULT_STYLE,
		const String &name = "",
		Bool show = True);

	//! Create a message box.
	//! @param name Unique name to recognize it at message box terminate message.
	void messageBox(
			const String &name,
			const String &title,
			const String &message,
			MessageBox::ButtonsStyle buttonStyle = MessageBox::OK,
			MessageBox::IconStyle iconStyle = MessageBox::ICON_INFORMATION,
			Float alpha = 1.0f);

	//! Add a top-level widget (mainly window or dialog) to the front
	void addWindowForegound(Window *pWindow,Bool show = True);

	//! Add a top-level widget (mainly window or dialog) to the back
	void addWindowBackground(Window *pWindow,Bool show = True);

	//! Recursively retrieve a widget by its object name
	//! (objects must have unique name for a correct result)
	Widget* findWidget(const String &name);

	//! Recursively retrieve a widget by its object name.
	//! (objects must have unique name for a correct result) (read only).
    const Widget* findWidget(const String &name) const;

	//! Recursively search for an existing widget by its pointer.
	Bool isExist(Widget* widget) const;

	//-----------------------------------------------------------------------------------
	// Events
	// Event are send first to the targeted widget, but if the widget doesn't perform
	// it (returns false), it is sent to its parent, until the widget manager.
	//-----------------------------------------------------------------------------------

	//! Get widget targeted by mouse
	Widget* getTargetedWidget(Int32 x,Int32 y);

	// events
    void mouseButtonDown(Mouse::Buttons button, Int32 x, Int32 y);
    void mouseButtonUp(Mouse::Buttons button, Int32 x, Int32 y);

	void mouseWheel(Int32 x, Int32 y, Int32 z);
	void mouseMove(Int32 x,Int32 y);

	void keyboardToggled(Keyboard *keyboard, KeyEvent event);
	void character(Keyboard *keyboard, CharacterEvent event);

    void reshape(UInt32 width, UInt32 height);

	//-----------------------------------------------------------------------------------
	// Signals
	//-----------------------------------------------------------------------------------

	//! Called when no widget consume the keyboard event.
    Signal<Keyboard*, KeyEvent> onKey{this};

	//-----------------------------------------------------------------------------------
	// Properties and System Console
	//-----------------------------------------------------------------------------------

	//! Prevent the widget to get the events.
	void preventEvent(Bool prevent = True);
	//! Is prevent event.
	Bool isPreventEvent() const { return m_preventEvent; }

	//! switch the focus between the console and others widgets
	void setFocusToConsole();

	//! switch the focus between the console and others widgets
	void setFocusToWidgets();

	//! is the focus is on console
    inline Bool isFocusToConsole() { return m_console->isActive(); }

	//! set console drawing (default is hide)
    inline void showConsole(Bool show = True) { m_console->show(show); }

	//! is the console drawn
    inline Bool isConsoleShown() const { return m_console->isShown(); }

	//! get the console
    inline Console& getConsole() { return *m_console; }

	//! get the console (const version)
    inline const Console& getConsole() const { return *m_console; }

	//! lock the mouse into the current targeted widget
    void lockWidgetMouse();

	//! is the mouse locked into the current targeted widget
	inline Bool isLockWidgetMouse() const { return m_lockWidgetMouse; }

	//! unlock the mouse from the current targeted widget.
	//! and search for the new targeted widget.
	void unlockWidgetMouse();

    //! Get the focused widget (read only).
	inline const Widget* getFocusedWidget() const { return m_focusWidget; }
    //! Get the focused widget.
    inline Widget* getFocusedWidget() { return m_focusWidget; }
	//! Is a focused widget.
    inline Bool isFocusedWidget() const { return m_focusWidget != nullptr; }

    //! Get the hovered widget (read only).
    inline const Widget* getHoveredWidget() const { return m_hoverWidget; }
    //! Get the hovered widget.
    inline Widget* getHoveredWidget() { return m_hoverWidget; }
    //! Is a hovered widget.
    inline Bool isHoveredWidget() const { return m_hoverWidget != nullptr; }

    //! Get the focused window (read only).
	inline const Widget* getFocusedWindow() const { return m_focusWindow; }
    //! Get the focused window.
    inline Widget* getFocusedWindow() { return m_focusWindow; }
	//! Is a focused window.
	inline Bool isFocusedWindow() const { return m_focusWindow != nullptr; }

    /**
     * @brief Is a targeted widget. It is different from isHoverWidget because it returns
     *        true only if the mouse target an active and usable widget (check modal and
     *        activity).
     * @return
     */
	Bool isTargetedWidget();

    /**
     * @brief Is the mouse hover a visible widget even if it is inactive or untargetable
     *        cause of a modal window.
     * @return
     */
    Bool isHoverWidget();

    //
    // Root window
    //

    //! Get the (background) root window
    inline RootWindow* getRootWindow() { return m_rootWindow.get(); }
    //! Get the (background) root window (const version)
    inline const RootWindow* getRootWindow() const { return m_rootWindow.get(); }

	//
	// Default Theme
	//

	//! Set the default theme
	inline void setDefaultTheme(Theme* theme) { m_theme = theme; }

	//! Get the default theme
	inline Theme* getDefaultTheme() { return m_theme; }

	//! Get the default theme (const version)
	inline const Theme* getDefaultTheme() const { return m_theme; }

	//
	// Processing
	//

	//! Draw the widgets tree.
	void draw();

	//! Update some widgets and console and process defered windows deletions.
	void update();

public:

	//! INTERNAL : previous targeted widget checking deletion prevention
	//! @warning Avoid to use them
    void checkPrevTargetedWidget(Widget *prevWidget);

	//! INTERNAL : set a widget always on top
	//! @warning Avoid to use them
	void setAlwaysOnTop(Window* pWindow, Bool onTop);

	//! INTERNAL : set the widget which have the keyboard focus
	void setFocus(Widget* pWidget);

	//! INTERNAL : show/hide a window
	void showHideWindow(Window *window, Bool show);

public:

	//! Called when a message box terminate.
    Signal<String, MessageBox::ButtonType> onMessageBoxResult{this};

protected:

	void messageBoxClose();
	void messageBoxOk();
	void messageBoxCancel();
	void messageBoxRetry();
	void messageBoxYes();
	void messageBoxNo();

	T_WindowList m_deferredDeletion;

    Console *m_console;         //!< a console

	T_WindowList m_windowList;  //!< the top-level widgets list

    AutoPtr<RootWindow> m_rootWindow;  //!< root window
    AutoPtr<Widget> m_glassPane;       //!< glass pane

	Widget* m_hoverWidget; //!< current targeted widget
	Widget* m_focusWidget; //!< current widget under focus
	Widget* m_focusWindow; //!< current window of the widget having the focus

    Widget* m_toolTip;     //!< valid if a tool tip is shown
    UInt32 m_toolTipTimeStamp; //!< timestamp when the tool tip is set (must wait a delay before show)

	Theme* m_theme;        //!< default theme for widgets

	Vector2i m_mousePos;   //!< position of the mouse

	Bool m_lockWidgetMouse;
	Bool m_preventEvent;
};

} // namespace o3d

#endif // _O3D_WIDGETMANAGER_H

