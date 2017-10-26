/**
 * @file checkbox.h
 * @brief A widget representing a checkbox
 * @author RinceWind
 * @author  Frederic SCHERMA
 * @date 2006-10-13
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CHECKBOX_H
#define _O3D_CHECKBOX_H

#include "widget.h"
#include "../textzone.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class CheckBox
//-------------------------------------------------------------------------------------
//! A widget representing a checkbox
//---------------------------------------------------------------------------------------
class O3D_API CheckBox : public Widget
{
public:

	O3D_DECLARE_CLASS(CheckBox)

	//! Style of the checkbox
	enum CheckBoxStyle
	{
		TWO_STATES_CHECKBOX,
		THREE_STATES_CHECKBOX
	};

	enum CheckBoxState
	{
		NO_CHECKED=0,
		CHECKED,
		THIRD_STATE
	};

	//! initialization constructor
	CheckBox(
		Widget *parent,
		const String &label,
		const Vector2i &pos = DEFAULT_POS,
		const Vector2i &size = DEFAULT_SIZE,
		CheckBoxStyle style = TWO_STATES_CHECKBOX,
		CheckBoxState checked = NO_CHECKED,
		const String &name = "");

	//! Virtual destructor.
	virtual ~CheckBox();

	//! set/get check state
	inline void setChecked(Bool state) { m_checkBoxState = state ? CHECKED : NO_CHECKED; }
	inline Bool isChecked() const { return (m_checkBoxState == CHECKED); }

	//! is the check box in third state
	inline Bool isThirdState() const { return (m_checkBoxState == THIRD_STATE); }

	//! Is widget targeted ?
	Bool isTargeted(Int32 x,Int32 y,Widget *&pWidget);

	//! Set the font.
	inline void setFont(ABCFont* font) { m_textZone.setFont(font); setDirty(); }
	//! Get the font (const version).
	inline const ABCFont* getFont() const { return m_textZone.font(); }
	//! Get the font.
	inline ABCFont* getFont() { return m_textZone.font(); }

	//! Set the font height.
	inline void setFontHeight(UInt32 _fontHeight) { m_textZone.setFontHeight(_fontHeight); setDirty(); }
	//! Get the font height.
	inline UInt32 getFontHeight() const { return m_textZone.fontHeight(); }

	//! Set text.
	inline void setText(const String &text) { m_textZone.setText(text); setDirty(); }
	//! Get text.
	inline const String& getText() const {return m_textZone.text();}

	//! get the recommended widget default size
	virtual Vector2i getDefaultSize();

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

	// Events Management
	virtual void sizeChanged();

	// Draw
	virtual void draw();
	virtual void updateCache();

public:

	//! Emitted when the check box is pressed (left mouse click)
    Signal<> onCheckBoxPressed{this};

	//! Emitted when the check box is released (only if it was initially pressed)
    Signal<> onCheckBoxReleased{this};

	//! Emitted when the check box checked state change (@see CheckState)
    Signal<CheckBox::CheckBoxState> onCheckBoxChanged{this};

protected:

	//! Default constructor.
	CheckBox(BaseObject *parent);

	Bool m_pressed;
	Bool m_wasPressed;
	Bool m_pushed;
	Bool m_hover;

	CheckBoxStyle m_style;
	CheckBoxState m_checkBoxState;

	TextZone m_textZone;

	void init();
};

} // namespace o3d

#endif // _O3D_CHECKBOX_H

