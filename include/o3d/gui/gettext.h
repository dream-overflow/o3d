/**
 * @file gettext.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_GETTEXT_H
#define _O3D_GETTEXT_H

#include "abcfont.h"
#include "validator.h"
#include "o3d/core/keyboard.h"
#include "o3d/core/templatearray.h"
#include "o3d/core/smartpointer.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class Texture2D;

/**
 * @brief A single or multi-line text input grabber.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2006-04-29
 * Get the typed text on a keyboard device and convert some specials keys like backspace
 * or arrow key for the cursor pos in the word.
 *
 * About the advance of text using a control key :
 * When moving to left or right, using the control key move per word.
 * Word are seperated by a list of characters, by defaut this is the set of punctuations
 * and any whitespaces. For delete and backspace using control the same separators are
 * used.
 * @todo Should be interesting to have a customisable list, or a regexp. And this could be
 * per direction and navigation or deletion. This is an advanced requirement so its only
 * an idea.
 */
class O3D_API GetText
{
public:

	//! Default constructor.
    GetText(BaseObject *parent, ABCFont* font = nullptr);

	//! Key event.
    //! @return True if the key event is managed.
	Bool keyboardToggled(Keyboard *keyboard, KeyEvent event);
	//! Character event.
    //! @return True if the character event is managed.
	Bool character(Keyboard *keyboard, CharacterEvent event);

	//! Set the text in read only.
	inline void setReadOnly(Bool readOnly = True) { m_readOnly = readOnly; }
	//! Is the text in read only.
	inline Bool isReadOnly() const { return m_readOnly; }

    //! Set the text content and set the cursor position.
    //! @param str New text content
    //! @param cursorPos -1 mean set cusor to end of the text, else use cursorPos
    //! @note The given string is truncated to max length.
    void setText(const String& str, UInt32 cursorPos=-1);
    //! Get the string.
    inline const String& text() const { return m_text; }

    //! Set the font.
    inline void setFont(ABCFont* pFont) { m_font = pFont; }
	//! Get the font (const version).
    inline const ABCFont* font() const { return m_font.get(); }
	//! Get the font.
    inline ABCFont* font() { return m_font.get(); }

	//! Authorize/unauth TAB character.
    inline void setTab(Bool is) { m_isTabEnable = is; }
	//! Get TAB auth status.
    inline Bool isTab()const { return m_isTabEnable; }

	//! Authorize/unauth Return character.
    inline void setReturn(Bool is) { m_isNewLineEnable = is; }
	//! Get Return auth status.
    inline Bool isReturn() const { return m_isNewLineEnable; }

    //! Set the cursor position given its position in character.
    void setCursorPos(UInt32 pos);
	//! Get the cursor position.
    inline UInt32 cursorPos() const { return m_cursorPos; }

    //! Draw the text and cursor.
    //! @deprecated use TextZone to display.
    //! @param pos Position in pixel where to draw the text (baseline).
    //! @param fontHeight -1 mean uses current font height else specific height.
    //! @deprecated Prefere the usage of a TextZone and Caret objects for display.
    void draw(const Vector2i &pos);

    //! Set a character filter list. This is a list of characters that are ignored
    //! during the capture.
    //! @param filter A template array of WChar.
	inline void setFilter(const TemplateArray<WChar> &filter) { m_filter = filter; }

    //! Get the character filter list.
    inline const TemplateArray<WChar>& filter() const { return m_filter; }

	//! Set the capture text length limit
    inline void setLengthLimit(UInt32 limit) { m_maxLength = limit; }

	//! Get the capture text length limit
    inline UInt32 lengthLimit() const { return m_maxLength; }

    //! Set a validator (or nullptr to remove a previous).
    //! A validator is used to force the capture to a valid format.
    //! There is default validator for integers values.
    inline void setValidator(Validator *validator) { m_validator = validator; }
    //! Get the current validator.
    inline const Validator* validator() const { return m_validator.get(); }

private:

    //! used font for check if character exists and by the draw method.
    SmartObject<ABCFont> m_font;

    Bool m_readOnly;         //!< read only
    String m_text;           //!< the text string

    UInt32 m_cursorPos;      //!< the cursor position in character
    UInt32 m_maxLength;      //!< max length of the captured string (default: 65535)

    Bool m_isTabEnable;      //!< is tab '\t' authorized
    Bool m_isNewLineEnable;  //!< is return '\n' authorized

    UInt32 m_lastTime;       //!< cursor toggle time

	TemplateArray<WChar> m_filter;    //!< list of filtered characters (non authorized)

    AutoPtr<Validator> m_validator;
};

} // namespace o3d

#endif // _O3D_GETTEXT_H

