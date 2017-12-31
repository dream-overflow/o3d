/**
 * @file toolbutton.h
 * @brief A widget representing a tool button
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-02-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TOOLBUTTON_H
#define _O3D_TOOLBUTTON_H

#include "widget.h"
#include "icon.h"
#include "tooltip.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class ToolButton
//-------------------------------------------------------------------------------------
//! A widget representing a tool button. No text, no icon, it is only a simple icon
//! button used in toolbar, window titlebar and such others. The size is fixed by the
//! theme definition or the icon. Its parent is informed on state change.
//---------------------------------------------------------------------------------------
class O3D_API ToolButton : public Widget
{
public:

	O3D_DECLARE_CLASS(ToolButton)

	//! ToolButton style
	enum ToolButtonStyle
	{
		PUSH_BUTTON,       //!< Default, push button
		TWO_STATES_BUTTON  //!< Two sate button (true/false)
	};

	//! Construct from a parent, icon object and a button style.
	//! @param parent Parent widget.
	//! @param icon Icon to draw in. The size of the icon force the size of the tool button.
	//! @param style Behavior of the button.
	ToolButton(
			Widget *parent,
			Icon *iconFalse,
            Icon *iconTrue = nullptr,
            Icon *iconHover = nullptr,
            Icon *iconDisable = nullptr,
			ToolButtonStyle style = PUSH_BUTTON);

	//! Construct from a parent, icon template, and a button style.
	//! @param parent Parent widget.
	//! @param widgetTrue Button pushed or pressed.
	//! @param widgetFalse Button normal state.
	//! @param widgetHover Button when mouse hover.
	//! @param style Behavior of the button.
	ToolButton(
			Widget *parent,
			Theme::WidgetDraw widgetFalse,
			Theme::WidgetDraw widgetTrue,
			Theme::WidgetDraw widgetHover,
            Theme::WidgetDraw widgetDisabled,
			ToolButtonStyle style = PUSH_BUTTON);

	ToolButton(Widget *parent, ToolButtonStyle style = PUSH_BUTTON);

	//! Virtual destructor.
	virtual ~ToolButton();

	//-----------------------------------------------------------------------------------
	// Properties
	//-----------------------------------------------------------------------------------

	//! Set the False icon. Can be defined by an icon or a ThemeToolButtonStyle.
	//! The icon object is owned by the tool button.
	void setIconFalse(Icon *icon);

	//! Set the False icon. Can be defined by an icon or a ThemeToolButtonStyle.
	//! The icon object is owned by the tool button.
	void setIconTrue(Icon *icon);

	//! Set the False icon. Can be defined by an icon or a ThemeToolButtonStyle.
	//! The icon object is owned by the tool button.
	void setIconHover(Icon *icon);

	//! Set the False icon. Can be defined by an icon or a ThemeToolButtonStyle.
	//! The icon object is owned by the tool button.
	void setIconDisable(Icon *icon);

	//! Get the False icon object.
	inline const Icon* getIconFalse() const { return m_iconFalse; }

	//! Get the True icon object.
	inline const Icon* getIconTrue() const { return m_iconTrue; }

	//! Get the Hover icon object.
	inline const Icon* getIconHover() const { return m_iconHover; }

	//! Get the Disable icon object.
	inline const Icon* getIconDisable() const { return m_iconDisable; }

	//! set the theme button style (@see SetIcon). 0 mean no button theme style
	//! @note Must be a simple type (not compass) and all the three of the same size
	void setToolButtonThemeStyle(
		Theme::WidgetDraw widgetTrue,
		Theme::WidgetDraw widgetFalse,
        Theme::WidgetDraw widgetHover,
        Theme::WidgetDraw widgetDisabled);

	//! get the 'True' version theme tool button style
	inline Theme::WidgetDraw getToolButtonThemeStyleTrue() const { return m_WidgetDrawTrue; }

	//! get the 'False' version theme tool button style
	inline Theme::WidgetDraw getToolButtonThemeStyleFalse() const { return m_WidgetDrawFalse; }

	//! get the 'Hover' version theme tool button style
	inline Theme::WidgetDraw getToolButtonThemeStyleHover() const { return m_WidgetDrawHover; }

	//! get the button style
	inline ToolButtonStyle getToolButtonStyle() const { return m_ButtonStyle; }

	//! is an icon tool button
    inline Bool isIconToolButton() const { return m_iconFalse.get() != nullptr; }

	//! is the button is in push state
	inline Bool isToolButtonPushed() const { return m_pushed; }

    //! Define a tool tip. Default does nothing.
    virtual void setToolTip(Widget *toolTip);

    //! Get the widget tool tip or nullptr if none. (default returns nullptr).
    virtual Widget* getToolTip();

    /**
     * @brief setToolTip Helper that create a tooltip with a simple static text in
     * a single line. It use of the virtual setToolTip method.
     * @param label
     */
    virtual void setToolTip(const String &label);

	//-----------------------------------------------------------------------------------
	// Widget
	//-----------------------------------------------------------------------------------

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

	virtual void focused();
	virtual void lostFocus();

	// Draw
	virtual void draw();
	virtual void updateCache();

public:

	//! Emitted when the tool button is pressed (left mouse click)
    Signal<> onToolButtonPressed{this};

	//! Emitted when the tool button is released (only if it was initially pressed)
    Signal<> onToolButtonReleased{this};

	//! Emitted when the tool button is pushed (only for push button style)
    Signal<> onToolButtonPushed{this};

	//! Emitted when the tool button state has changed (only for two state button style)
    Signal<Bool> onToolButtonStateChanged{this};

protected:

	//! Default constructor.
	ToolButton(BaseObject *parent);

	Bool m_pressed;
	Bool m_waspressed;
	Bool m_pushed;
	Bool m_hover;

	AutoPtr<Icon> m_iconFalse;
	AutoPtr<Icon> m_iconTrue;
	AutoPtr<Icon> m_iconHover;
	AutoPtr<Icon> m_iconDisable;

    AutoPtr<Widget> m_toolTip;

	Theme::WidgetDraw m_WidgetDrawFalse;
	Theme::WidgetDraw m_WidgetDrawTrue;
	Theme::WidgetDraw m_WidgetDrawHover;
    Theme::WidgetDraw m_WidgetDrawDisabled;

	ToolButtonStyle m_ButtonStyle;
};

} // namespace o3d

#endif // _O3D_TOOLBUTTON_H

