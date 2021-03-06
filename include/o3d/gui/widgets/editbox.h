/**
 * @file editbox.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_EDITBOX_H
#define _O3D_EDITBOX_H

#include "widget.h"
#include "../abcfont.h"
#include "../textzone.h"
#include "../gettext.h"
#include "../caret.h"

namespace o3d {

/**
 * @brief A widget representing a single line text entry box
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-02-02
 */
class O3D_API EditBox : public Widget
{
public:

	O3D_DECLARE_CLASS(EditBox)

	//! Style relative to the edit box widget
	enum EditBoxStyle
	{
		EDITBOX_NORMAL,       //!< normal edit box (default)
		EDITBOX_HIDDEN_CHAR  //!< replace each char by a wildchar '*'
	};

	//! creation constructor
	EditBox(
		Widget *parent,
        const Vector2i &pos = DEFAULT_POS,
        const Vector2i &size = DEFAULT_SIZE,
		EditBoxStyle style = EDITBOX_NORMAL,
		const String &name = "");

	//! destructor
	virtual ~EditBox();

	//! get the recommended widget default size
    virtual Vector2i getDefaultSize() override;

	//! Set read-only text
	inline void setReadOnly(Bool readOnly = True) { m_getText.setReadOnly(readOnly); }
	//! Is read-only text
	inline Bool isReadOnly() const { return m_getText.isReadOnly(); }

	//! Set text
    void setText(const String &text);
	//! Get text
    inline const String& getText() const { return m_getText.text();}

    //! Defines the placeholder.
    void setPlaceholder(const String &placeholder);
    //! Get the placeholder.
    inline const String& getPlaceholder() const { return m_placeholder; }

    //! Set the font. A null font mean use the default.
    void setFont(ABCFont* font);
	//! Get the font (const version)
    inline const ABCFont* getFont() const { return m_textZone.font(); }
	//! Get the font
    inline ABCFont* getFont() { return m_textZone.font(); }

    //! Set the font height. -1 mean use default font height.
    void setFontHeight(Int32 _fontHeight);
    //! Get the font height
    inline Int32 getFontHeight() const { return m_textZone.fontHeight(); }

	//! Set/Get the horizontal alignment
	inline void setTextHorizontalAlignment(TextZone::HorizontalAlignmentMode align)
	{
		m_textZone.setHorizontalAlignment(align);
	}

	inline TextZone::HorizontalAlignmentMode getTextHorizontalAlignment() const
	{
		return m_textZone.horizontalAlignment();
	}

	//! Set/Get the vertical alignment
	inline void setTextVerticalAlignment(TextZone::VerticalAlignmentMode align)
	{
		m_textZone.setVerticalAlignment(align);
	}

	inline TextZone::VerticalAlignmentMode getTextVerticalAlignment() const
	{
		return m_textZone.verticalAlignment();
	}

	//! Set the capture text length limit
    inline void setTextLimit(UInt32 limit) { m_getText.setLengthLimit(limit); }

	//! get the capture text length limit
    inline UInt32 getTextLimit() const { return m_getText.lengthLimit(); }

    //! Set a validator (or nullptr to remove a previous).
    void setValidator(Validator *validator) { m_getText.setValidator(validator); }
    //! Get the current validator.
    const Validator* getValidator() const { return m_getText.validator(); }

	//! Events Management
    virtual Bool isTargeted(Int32 x, Int32 y, Widget *&widget) override;

    virtual Bool mouseLeftPressed(Int32 x,Int32 y) override;
    virtual Bool mouseLeftReleased(Int32 x,Int32 y) override;
    virtual Bool mouseMove(Int32 x,Int32 y) override;

	//! Others Event
    virtual void focused() override;
    virtual void lostFocus() override;
    virtual void sizeChanged() override;
    virtual void valueChanged() override;

    virtual void mouseMoveIn() override;
    virtual void mouseMoveOut() override;

    virtual void themeChanged() override;

	//! A keyboard key toggle event occurred
    virtual Bool keyboardToggled(Keyboard *keyboard, KeyEvent event) override;
	//! Character event.
    virtual Bool character(Keyboard *keyboard, CharacterEvent event) override;

	//! Update for keyboard entry (GetText)
	void updateCaption();

	// Draw
    virtual void draw() override;
    virtual void updateCache() override;

	// Get the current cursor type name for this widget if targeted.
    virtual String getCursorTypeName() const override;

    virtual Bool cut(Bool primary=False) override;
    virtual Bool copy(Bool primary=False) override;
    virtual Bool paste(Bool primary=False) override;

public:

	//-----------------------------------------------------------------------------------
	// User events. This event can be used directly by the user program
	//-----------------------------------------------------------------------------------

	//! The text has change
    Signal<> onValueChange{this};

	//! When the enter key is pressed
    Signal<> onEnterPressed{this};

protected:

    enum EditBoxState
    {
        STATE_HOVER = 0,        //!< True when the pointer hover him.
        STATE_EDITING,          //!< True when keyboard edit him.
        STATE_SELECTING,        //!< True when an input select text.
        STATE_MOUSE_SELECT,     //!< True when the mouse select text.
        STATE_KEYBOARD_SELECT   //!< True when the keyboard select text.
    };

	//! Default constructor.
	EditBox(BaseObject *parent);

	EditBoxStyle m_editBoxStyle;

	Bool m_hover;      //!< True when the mouse hover him.
	Bool m_editing;    //!< True when keyboard edit him.

    BitSet8 m_editBoxState;

    GetText m_getText;     //!< Managed text content with cursor
    TextZone m_textZone;   //!< Visual part of the text

    Caret m_caret;         //!< Displayable cursor
    Int32 m_leftPos;       //!< Displayed text left offset in pixels

    Int32 m_visible[2];    //!< Left and right visible characters index.
    Int32 m_selection[2];  //!< Selected index (start, end) of text.

    String m_placeholder;  //!< Placeholder texte when no value.

	void init();

    //! internal text update
    void updateText();

    ABCFont *getUsedFont();
    Int32 getUsedFontHeight();

    void applySelection();
};

} // namespace o3d

#endif // _O3D_EDITBOX_H
