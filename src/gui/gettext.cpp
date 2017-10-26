/**
 * @file gettext.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/gettext.h"

#include "o3d/engine/texture/texture2d.h"
#include "o3d/gui/texturefont.h"

#include "o3d/core/char.h"

using namespace o3d;

//! Default constructor.
GetText::GetText(BaseObject *parent, ABCFont* font) :
    m_font(parent, font),
    m_readOnly(False),
    m_cursorPos(0),
    m_maxLength(65535),
    m_isTabEnable(False),
    m_isNewLineEnable(False),
    m_lastTime(0)
{
}

Bool GetText::keyboardToggled(Keyboard *keyboard, KeyEvent event)
{
	// only key pressed and repeat
	if (event.isReleased())
		return False;

    Bool ctrl = (keyboard->isKeyDown(KEY_LCONTROL)) || (keyboard->isKeyDown(KEY_RCONTROL));

	// move the cursor to the left to 1 pos
    if (event.key() == KEY_LEFT)
    {
        if (m_cursorPos > 0)
        {
            // to previous word
            if (ctrl)
            {
                Int32 ns = m_text.length() - 1;
                if (m_cursorPos < m_text.length() || WideChar::ponctuationsPlusWs().find(m_text[m_text.length()-1]) != -1)
                    ns = m_text.find(WideChar::ponctuationsPlusWs(), m_cursorPos-1, True, True);

                if (ns > 0)
                {
                    ns = m_text.find(WideChar::ponctuationsPlusWs(), ns-1, True, False);
                    m_cursorPos = ns + 1;
                }
                else
                    m_cursorPos = 0;

                return True;
            }

            m_cursorPos--;
        }

		return True;
	}
	// move the cursor to the right to 1 pos
    else if ((event.key() == KEY_RIGHT))
	{
        if (m_cursorPos < m_text.length())
        {
            // to next word
            if (ctrl)
            {
                Int32 ns = m_text.find(WideChar::ponctuationsPlusWs(), m_cursorPos, False, True);
                if (ns >= (Int32)m_cursorPos)
                    m_cursorPos = ns + 1;
                else
                    m_cursorPos = m_text.length();

                return True;
            }

            m_cursorPos++;
        }

		return True;
	}
	// move the cursor to the first pos
    else if ((event.key() == KEY_HOME))
	{
        if (m_cursorPos != 0)
            m_cursorPos = 0;

		return True;
	}
	// move the cursor to the last pos
    else if ((event.key() == KEY_END))
	{
        if (m_cursorPos != m_text.length())
            m_cursorPos = m_text.length();

		return True;
	}
	else if (event.key() == KEY_UP)
	{
        // TODO search many next \n for multi-line
		return False;//TRUE;
	}
	else if (event.key() == KEY_DOWN)
	{
        // TODO search many previous \n for multi-line
		return False;//TRUE;
	}

	return False;
}

Bool GetText::character(Keyboard *keyboard, CharacterEvent event)
{
	// first check if the character is not filtered
	UInt32 filterLen = m_filter.getSize();
	for (size_t i = 0 ; i < filterLen ; ++i)
	{
		if (event.unicode() == m_filter[i])
			return False;
	}

    Bool ctrl = (keyboard->isKeyDown(KEY_LCONTROL)) || (keyboard->isKeyDown(KEY_RCONTROL));

	// escape
	if (event.unicode() == 27)
	{
		return False;
	}
	// delete
	else if (event.unicode() == 127)
	{
        if (m_text.isValid() && (m_cursorPos < m_text.length()) && !m_readOnly)
		{
			if (ctrl)
            {
                // delete the next word after the cursor
                Int32 ns = m_text.find(WideChar::ponctuationsPlusWs(), m_cursorPos, False, True);
                if (ns >= (Int32)m_cursorPos)
                    m_text.remove(m_cursorPos, ns - m_cursorPos + 1);
                else
                    m_text.remove(m_cursorPos, m_text.length() - m_cursorPos);
			}
			else
			{
				// delete the char after the cursor
                m_text.remove(m_cursorPos, 1);
			}

			return True;
		}
		else
			return False;
	}

	// next operation need write
	if (m_readOnly)
		return False;

	// backspace
	if (event.unicode() == '\b')
	{
        if (m_cursorPos > 0)
		{
			if (ctrl)
			{
                // delete the word before the cursor
                Int32 ns = m_text.length() - 1;
                if (m_cursorPos < m_text.length() || WideChar::ponctuationsPlusWs().find(m_text[m_text.length()-1]) != -1)
                    ns = m_text.find(WideChar::ponctuationsPlusWs(), m_cursorPos-1, True, True);

                if (ns > 0)
                {
                    ns = m_text.find(WideChar::ponctuationsPlusWs(), ns-1, True, False);
                    m_text.remove(ns + 1, m_cursorPos - (ns + 1));
                    m_cursorPos = ns + 1;
                }
                else
                {
                    m_text.remove(0, m_cursorPos);
                    m_cursorPos = 0;
                }
			}
			else
			{
				// delete the char before the cursor
                m_text.remove(m_cursorPos-1, 1);
                m_cursorPos--;
			}
		}
	}
	// tab key
	else if (event.unicode() == '\t')
	{
        if (m_isTabEnable)
		{
			// add 4 spaces for a tab
            m_text += "    ";
            m_cursorPos += 4;
		}
		else
			return False;
	}
	// return/enter key
	else if (event.unicode() == '\n')
	{
        if (m_isNewLineEnable)
		{
			// add '\n'
            m_text += '\n';
            m_cursorPos += 1;
		}
		else
			return False;
	}
	// normal key
	else if (event.unicode() != 0)
	{
		// non writable char for this font
        if (m_font && !m_font->isSupportedChar(event.unicode()))
			return False;

		// max length
        if (m_text.length() >= m_maxLength)
			return False;

        // validator
        if (m_validator.isValid())
        {
            UInt32 prevCursorPos = m_cursorPos;
            String prevStr = m_text;

            // insert character at cursor pos
            if (keyboard->getModifier(MOD_INSERT))
            {
                m_text.insert(event.unicode(), m_cursorPos);
                m_cursorPos++;
            }
            // replace character at cursor pos
            else
            {
                if (m_cursorPos < m_text.length())
                {
                    m_text[m_cursorPos] = event.unicode();
                    m_cursorPos++;
                }
                else
                {
                    m_text += event.unicode();
                    m_cursorPos = m_text.length();
                }
            }

            if (!m_validator->validate(m_text))
            {
                // unvalid, restore previous value
                m_cursorPos = prevCursorPos;
                m_text = prevStr;

                return False;
            }
        }
        else
        {
            // insert character at cursor pos
            if (keyboard->getModifier(MOD_INSERT))
            {
                m_text.insert(event.unicode(), m_cursorPos);
                m_cursorPos++;
            }
            // replace character at cursor pos
            else
            {
                if (m_cursorPos < m_text.length())
                {
                    m_text[m_cursorPos] = event.unicode();
                    m_cursorPos++;
                }
                else
                {
                    m_text += event.unicode();
                    m_cursorPos = m_text.length();
                }
            }
        }
	}

	return True;
}

// Set the string.
void GetText::setText(const String& str, UInt32 cursorPos)
{
    m_text = str;

    if (m_text.length() >= m_maxLength)
        m_text.truncate(m_maxLength);

    if (cursorPos <= m_text.length())
        m_cursorPos = cursorPos;
    else
        m_cursorPos = m_text.length();
}

// set the cursor position
void GetText::setCursorPos(UInt32 pos)
{
    if (pos != m_cursorPos)
        m_cursorPos =  o3d::clamp(pos, (UInt32)0, m_text.length());
}

// draw the text with the specified font
void GetText::draw(const Vector2i &pos)
{
	UInt32 curtime = System::getMsTime();
    UInt32 elapsed = curtime - m_lastTime;

    if (!m_font)
		return;

	if (elapsed < 500)
	{
        m_font->write(pos, m_text, m_cursorPos);
	}
	else if (elapsed >= 500)
	{
		if (elapsed >= 1000)
            m_lastTime = curtime;

        m_font->write(pos, m_text, -1);
    }
}

