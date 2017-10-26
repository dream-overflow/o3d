/**
 * @file button.h
 * @brief A widget representing a simple button.
 * @author RinceWind
 * @author  Frederic SCHERMA
 * @date 2006-06-26
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_BUTTON_H
#define _O3D_BUTTON_H

#include "widget.h"
#include "icon.h"
#include "../textzone.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Button
//-------------------------------------------------------------------------------------
//! A widget representing a simple button.
//! TODO Button with an icon.
//---------------------------------------------------------------------------------------
class O3D_API Button : public Widget
{
public:

	O3D_DECLARE_CLASS(Button)

	//! Button style
	enum ButtonStyle
	{
		PUSH_BUTTON,       //!< Default, push button
		TWO_STATES_BUTTON  //!< Two sate button (true/false)
	};

	//! Initialization constructor.
	Button(
		Widget *parent,
		const String &label,
		const Vector2i &pos = DEFAULT_POS,
		const Vector2i &size = DEFAULT_SIZE,
		ButtonStyle style = PUSH_BUTTON,
		const String &name = "");

	//! Virtual destructor.
	virtual ~Button();

	//-----------------------------------------------------------------------------------
	// Properties
	//-----------------------------------------------------------------------------------

	//! fit to the size of the content
	virtual Vector2i fit();

	//! get the recommended widget default size
	virtual Vector2i getDefaultSize();

	//! get the button style
	inline ButtonStyle getButtonStyle() const { return m_buttonStyle; }

	//! is the button is in push state
	inline Bool isButtonPushed() const { return m_capacities.getBit(STATE_PUSHED); }

	//! set the button text
	void setText(const String &text);

	//! get the button text
	const String &getText() const;

	//! Set the horizontal alignment
	inline void setHorizontalAlignment(TextZone::HorizontalAlignmentMode _align)
	{
		m_textZone.setHorizontalAlignment(_align);
		setDirty();
	}
	//! Get the horizontal alignment
	inline TextZone::HorizontalAlignmentMode getHorizontalAlignment() const
	{
		return m_textZone.horizontalAlignment();
	}

	//! Set the vertical alignment
	inline void setVerticalAlignment(TextZone::VerticalAlignmentMode _align)
	{
		m_textZone.setVerticalAlignment(_align);
		setDirty();
	}
	//! Get the vertical alignment
	inline TextZone::VerticalAlignmentMode getVerticalAlignment() const
	{
		return m_textZone.verticalAlignment();
	}

	//! Set the button icon (override the button text).
	void setIcon(const Icon &icon);

	//------------------------------------------------------------------------------------
	// Widget
	//-----------------------------------------------------------------------------------

	//! Enable/disable the widget.
	virtual void enable(Bool active = True);

	//! Is widget targeted ?
	virtual Bool isTargeted(Int32 x, Int32 y, Widget *&widget);

	//! Mouse Events
	virtual Bool mouseLeftPressed(Int32 x,Int32 y);
	virtual Bool mouseLeftReleased(Int32 x,Int32 y);
	virtual Bool mouseMove(Int32 x,Int32 y);
	virtual void mouseMoveIn();
	virtual void mouseMoveOut();

	//! Key event.
	virtual Bool keyboardToggled(Keyboard *keyboard, KeyEvent event);

	virtual void focused();
	virtual void lostFocus();

	virtual void sizeChanged();

	// Draw
	virtual void draw();
	virtual void updateCache();

public:

	//! Emitted when the button is pressed (left mouse click)
    Signal<> onButtonPressed{this};

	//! Emitted when the button is released (only if it was initially pressed)
    Signal<> onButtonReleased{this};

	//! Emitted when the button is pushed (only for push button style)
    Signal<> onButtonPushed{this};

	//! Emitted when the button state has changed (only for two state button style)
    Signal<Bool> onButtonStateChanged{this};

protected:

	enum CapacitesButton
	{
		STATE_PRESSED = CAPS_WIDGET_LAST + 1,
		STATE_WAS_PRESSED,
		STATE_PUSHED
	};

	//! Default constructor.
	Button(BaseObject *parent);

	ButtonStyle m_buttonStyle;

	TextZone m_textZone;
	Widget *m_icon;

	void init();
	void adjustBestSize();
};

} // namespace o3d

#endif // _O3DBUTTON_H

