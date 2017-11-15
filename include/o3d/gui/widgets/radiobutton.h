/**
 * @file radiobutton.h
 * @brief A widget representing a radio button.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-10-12
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_RADIOBUTTON_H
#define _O3D_RADIOBUTTON_H

#include "widget.h"
#include "../textzone.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class RadioButton
//-------------------------------------------------------------------------------------
//! A widget representing a radio button. Radio buttons are grouped by setting the
//! previous radio button for any radio button, and NULL when it is the first.
//! When a radio button is checked it received the signal OnRadioButtonChanged(True).
//! The previous check radio button receive the signal OnRadioButtonChanged(False).
//---------------------------------------------------------------------------------------
class O3D_API RadioButton : public Widget
{
public:

	O3D_DECLARE_CLASS(RadioButton)

	//! Construct given a parent widget and previous radio button.
	//! @param parent Parent widget.
	//! @param previous Previous radio button or NULL if first.
	//! @param label Text message.
	//! @param pos Position.
	//! @param size Size.
	//! @param name Single widget name or empty.
	RadioButton(
			Widget *parent,
			RadioButton *previous,
			const String &label,
			const Vector2i &pos = Widget::DEFAULT_POS,
			const Vector2i &size = Widget::DEFAULT_SIZE,
			const String &name = "");

	//! Virtual destructor.
	virtual ~RadioButton();

	//! get the recommended widget default size.
	Vector2i getDefaultSize();

	//! Check the radio button and uncheck the linked button if necessary.
	//! A signal OnRadioButtonChanged is emitted to the previous checked button.
	void setChecked(Bool state);

	//! Is the radio button checked.
	inline Bool isChecked() const { return m_radioButtonState; }

	//! Is widget targeted ?
	Bool isTargeted(Int32 x, Int32 y, Widget *&widget);

	//! Set the font.
	inline void setFont(ABCFont* font) { m_textZone.setFont(font); setDirty(); }
	//! Get the font (const version).
    inline const ABCFont* font() const { return m_textZone.font(); }
	//! Get the font.
    inline ABCFont* font() { return m_textZone.font(); }

	//! Set the font height.
	inline void setFontHeight(UInt32 _fontHeight) { m_textZone.setFontHeight(_fontHeight); setDirty(); }
	//! Get the font height.
    inline UInt32 fontHeight() const { return m_textZone.fontHeight(); }

	//! Set text.
	inline void setText(const String &text) { m_textZone.setText(text); setDirty(); }
	//! Get text.
    inline const String& text() const {return m_textZone.text();}

	//! Mouse Events
	virtual Bool mouseLeftPressed(Int32 x,Int32 y);
	virtual Bool mouseLeftReleased(Int32 x,Int32 y);
	virtual Bool mouseMove(Int32 x,Int32 y);
	virtual void mouseMoveIn();
	virtual void mouseMoveOut();

    //! A keyboard key toggle event occurred
    virtual Bool keyboardToggled(Keyboard *keyboard, KeyEvent event);

	virtual void focused();
	virtual void lostFocus();

	//! Events Management
	virtual void sizeChanged();

	// Draw
	virtual void draw();
	virtual void updateCache();

public:

	//! Emitted when the radio button is pressed (left mouse click).
    Signal<> onRadioButtonPressed{this};

	//! Emitted when the radio button is released (only if it was initially pressed)
    Signal<> onRadioButtonReleased{this};

	//! Emitted when the radio button checked state change (@see IsChecked).
    Signal<Bool> onRadioButtonChanged{this};

protected:

	//! default constructor
	RadioButton(BaseObject *parent);

	Bool m_pressed;
	Bool m_wasPressed;
	Bool m_pushed;
	Bool m_hover;

	Bool m_radioButtonState;

	RadioButton *m_previousRadioButton;
	RadioButton *m_nextRadioButton;

	TextZone m_textZone;

	void init();
};

} // namespace o3d

#endif // _O3D_RADIOBUTTON_H

