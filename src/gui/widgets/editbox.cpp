/**
 * @file editbox.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/editbox.h"

#include "o3d/gui/widgets/layout.h"
#include "o3d/gui/guitype.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/gui/widgetdrawcompass.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/renderer.h"
#include "o3d/engine/scene/scene.h"

#include "o3d/core/appwindow.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(EditBox, GUI_WIDGET_EDITBOX, Widget)

//---------------------------------------------------------------------------------------
// constructor
//---------------------------------------------------------------------------------------

EditBox::EditBox(BaseObject *parent) :
		Widget(parent),
		m_editBoxStyle(EDITBOX_NORMAL),
		m_hover(False),
		m_editing(False),
        m_getText(this, nullptr),
		m_textZone(),
		m_caret(&m_textZone),
        m_leftPos(0),
        m_visible{0, 0},
        m_selection{0, 0}
{
}

EditBox::EditBox(
	Widget *parent,
	const Vector2i &pos,
	const Vector2i &size,
	EditBoxStyle style,
	const String &name) :
		Widget(parent, pos, size, name),
		m_editBoxStyle(style),
		m_hover(False),
		m_editing(False),
        m_getText(this, nullptr),
		m_textZone(),
		m_caret(&m_textZone),
        m_leftPos(0),
        m_visible{0, 0},
        m_selection{0, 0}
{
	init();
}

void EditBox::init()
{
    m_capacities.enable(CAPS_FOCUSABLE);
    m_capacities.enable(CAPS_COPYABLE);
    m_capacities.enable(CAPS_PASTABLE);

	m_getText.setTab(False);
	m_getText.setReturn(False);

	m_textZone.setVerticalAlignment(TextZone::TOP_ALIGN);

    Theme *theme = getUsedTheme();

	// default color
    m_textZone.setColor(theme->getWidgetDrawMode(Theme::EDITBOX_NORMAL)->getDefaultTextColor());
    m_textZone.setHighlightColor(theme->getDefaultHighlightFontColor());

    // TODO replace with a computed style default
    m_getText.setFont(theme->getDefaultFont());

    m_textZone.setFont(theme->getDefaultFont());
    m_textZone.setFontHeight(theme->getDefaultFontSize());

    Vector2i defaultSize = getDefaultSize();
    if (m_minSize.x() < 0)
        m_minSize.x() = defaultSize.x();
    if (m_minSize.y() < 0)
        m_minSize.y() = defaultSize.y();

    m_size = m_maxSize = m_minSize;

    sizeChanged();
}

/*---------------------------------------------------------------------------------------
  destructor
---------------------------------------------------------------------------------------*/
EditBox::~EditBox()
{
}

/*---------------------------------------------------------------------------------------
  get the recommended widget default size
---------------------------------------------------------------------------------------*/
Vector2i EditBox::getDefaultSize()
{
	return getUsedTheme()->getDefaultWidgetSize(Theme::EDITBOX_NORMAL);
}

// Set text
void EditBox::setText(const String &text)
{
    m_getText.setText(text);
    valueChanged();
}

void EditBox::setPlaceholder(const String &placeholder)
{
    m_placeholder = placeholder;
    valueChanged();
}

// Set the font
void EditBox::setFont(ABCFont* font)
{
    m_textZone.setFont(font);

    if (font)
    {
        m_getText.setFont(font);
        m_textZone.setFont(font);
    }
    else
    {
        Theme *theme = getUsedTheme();

        m_getText.setFont(theme->getDefaultFont());
        m_textZone.setFont(theme->getDefaultFont());
    }

    valueChanged();
}

// Set the font height
void EditBox::setFontHeight(Int32 _fontHeight)
{
    m_textZone.setFontHeight(_fontHeight);
    valueChanged();
}

/*---------------------------------------------------------------------------------------
  Events Management
---------------------------------------------------------------------------------------*/

Bool EditBox::isTargeted(Int32 x, Int32 y, Widget *&widget)
{
	// no negative part
	if (x < 0 || y < 0)
		return False;

    if ((x < m_size.x()) && (y < m_size.y()))
	{
        widget = this;
		return True;
	}

	return False;
}

void EditBox::mouseMoveIn()
{
	m_hover = True;
	setDirty();
}

void EditBox::mouseMoveOut()
{
	m_hover = False;
    setDirty();
}

void EditBox::themeChanged()
{
   if (!m_textZone.font())
   {
       Theme *theme = getUsedTheme();

       m_getText.setFont(theme->getDefaultFont());

       m_textZone.setFont(theme->getDefaultFont());
       m_textZone.setFontHeight(theme->getDefaultFontSize());
   }
}

// Get the current cursor type name for this widget if targeted.
String EditBox::getCursorTypeName() const
{
	if (m_hover)
		return "Text";
	else
        return "Default";
}

Bool EditBox::cut(Bool primary)
{
    // avoid on hidden chars box
    if (m_editBoxStyle == EDITBOX_HIDDEN_CHAR)
        return False;

    String text = m_getText.text();
    String selection;
    UInt32 cp;

    // is a seletion, cut it
    if (m_selection[1] > m_selection[0])
    {
        selection = text.extract(m_selection[0], m_selection[1]-m_selection[0]);
        cp = m_selection[0];
    }
    else if (m_selection[0] > m_selection[1])
    {
        selection = text.extract(m_selection[1], m_selection[0]-m_selection[1]);
        cp = m_selection[1];
    }
    else
        // empty selection
        return False;

    // invalid the selection
    m_selection[0] = m_selection[1] = 0;

    if (selection.isValid())
    {
        Bool result = getScene()->getRenderer()->getAppWindow()->pasteToClipboard(selection, primary);

        if (result)
        {
            m_getText.setText(text, cp);

            updateText();
            updateCaption();
        }

        return result;
    }

    return False;
}

Bool EditBox::copy(Bool primary)
{
    // avoid on hidden chars box
    if (m_editBoxStyle == EDITBOX_HIDDEN_CHAR)
        return False;

    String text = m_getText.text();
    String selection;

    // is a seletion, copy it
    if (m_selection[1] > m_selection[0])
        selection = text.sub(m_selection[0], m_selection[1]);
    else if (m_selection[0] > m_selection[1])
        selection = text.sub(m_selection[1], m_selection[0]);
    else
        // empty selection
        return False;

    if (selection.isValid())
    {
        return getScene()->getRenderer()->getAppWindow()->pasteToClipboard(selection, primary);
    }

    return False;
}

Bool EditBox::paste(Bool primary)
{
    String clip = getScene()->getRenderer()->getAppWindow()->copyFromClipboard(primary);
    if (clip.isValid())
    {
        String text = m_getText.text();
        UInt32 cp;

        // is a seletion, replace it
        if (m_selection[1] > m_selection[0])
        {
            UInt32 len = m_selection[1] - m_selection[0];

            text.remove(m_selection[0], len);
            text.insert(clip, m_selection[0]);

            cp = m_getText.cursorPos() + clip.length() - len;
        }
        else if (m_selection[0] > m_selection[1])
        {
            UInt32 len = m_selection[0] - m_selection[1];

            text.remove(m_selection[1], len);
            text.insert(clip, m_selection[1]);

            cp = m_getText.cursorPos() + clip.length() - len;
        }
        else
        {
            text.insert(clip, m_getText.cursorPos());
            cp = m_getText.cursorPos() + clip.length();
        }

        m_getText.setText(text, cp);

        // invalid the selection
        m_selection[0] = m_selection[1] = 0;

        updateText();
        updateCaption();

        return True;
    }

    return False;
}

void EditBox::sizeChanged()
{
	const Theme *theme = getUsedTheme();

	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::WINDOW_NORMAL);

	if (drawMode->getType() == WidgetDrawMode::COMPASS_MODE)
	{
		const WidgetDrawCompass *pCompassMode = (WidgetDrawCompass*)drawMode;

		m_textZone.setPos(
			pCompassMode->getEltWidth(WidgetDrawCompass::WEST),
			pCompassMode->getEltHeight(WidgetDrawCompass::NORTH));

        m_textZone.setSize(
            m_size.x() - (pCompassMode->getEltWidth(WidgetDrawCompass::WEST) +
				(pCompassMode->getEltWidth(WidgetDrawCompass::EAST))),
            m_size.y() - (pCompassMode->getEltHeight(WidgetDrawCompass::NORTH) +
				(pCompassMode->getEltHeight(WidgetDrawCompass::SOUTH))));
	}
	else
	{
        m_textZone.setPos(0, 0);
        m_textZone.setSize(m_size.x(), m_size.y());
	}

	setDirty();
}

void EditBox::updateText()
{
    // by default display the placeholder
    if (m_getText.text().isEmpty() && m_placeholder.isValid())
    {
        if (m_placeholder.isValid())
            m_textZone.setText(m_placeholder);
        else
            m_textZone.setText("");

        m_visible[0] = m_visible[1] = 0;
        m_selection[0] = m_selection[1] = 0;

        m_caret.setPixelPosition(Vector2i(0, 0));
        m_textZone.resetHighlight();
    }
    else
        updateCaption();

    m_textZone.resetHighlight();

    setDirty();
}

ABCFont* EditBox::getUsedFont()
{
    // TODO using computed and local style
    if (m_textZone.font())
    {
        return m_textZone.font();
    }
    else
    {
        Theme *theme = getUsedTheme();
        return theme->getDefaultFont();
    }
}

Int32 EditBox::getUsedFontHeight()
{
    // TODO using computed and local style
    if (m_textZone.font())
    {
        return m_textZone.fontHeight();
    }
    else
    {
        Theme *theme = getUsedTheme();
        return theme->getDefaultFontSize();
    }
}

void EditBox::valueChanged()
{
    updateText();

    // invalid selection
    if (m_editBoxState[STATE_SELECTING])
    {
        m_editBoxState.disable(STATE_MOUSE_SELECT);
        m_editBoxState.disable(STATE_KEYBOARD_SELECT);
        m_editBoxState.disable(STATE_SELECTING);
    }

    if (m_selection[0] != m_selection[1])
        m_selection[0] = m_selection[1] = 0;
}

Bool EditBox::mouseLeftPressed(Int32 x, Int32 y)
{
    if (m_getText.text().isEmpty())
        return True;

    if (!m_editBoxState[STATE_MOUSE_SELECT])
        m_editBoxState.enable(STATE_MOUSE_SELECT);

    // defines the caret position
    Int32 pos = m_getText.font()->nearestChar(m_textZone.text(), x - m_pos.x());
    pos = o3d::clamp(pos, 0, (Int32)m_textZone.text().length());

    // locate the caret
    m_getText.setCursorPos(pos + m_visible[0]);

    m_selection[0] = m_selection[1] = 0;
    m_textZone.resetHighlight();

    updateCaption();

    setDirty();

	return True;
}

Bool EditBox::mouseLeftReleased(Int32 x, Int32 y)
{
    if (m_editBoxState[STATE_MOUSE_SELECT])
    {
        m_editBoxState.disable(STATE_MOUSE_SELECT);
        ((Gui*)getScene()->getGui())->getWidgetManager()->unlockWidgetMouse();
    }

    if (m_editBoxState[STATE_SELECTING] && !m_editBoxState[STATE_KEYBOARD_SELECT])
        m_editBoxState.disable(STATE_SELECTING);

	return True;
}

// Voix ambiguë d'un cœur qui au zéphyr préfère les jattes de kiwis

void EditBox::applySelection()
{
    // defines the highlighted text
    if (m_selection[0] != m_selection[1] && m_getText.text().isValid())
    {
        Int32 min, max;

        if (m_selection[0] > m_selection[1])
        {
            min = m_selection[1];
            max = m_selection[0];
        }
        else if (m_selection[1] > m_selection[0])
        {
            min = m_selection[0];
            max = m_selection[1];
        }

        // intersects with visible part or surround
        if ((min >= m_visible[0]) || (max <= m_visible[1]) || (min < m_visible[0] && max > m_visible[1]))
        {
            m_textZone.setHighlight(
                        o3d::max(0 , min-m_visible[0]),
                        o3d::max(max-m_visible[0], 0));
        }
        else
            m_textZone.resetHighlight();

        setDirty();
    }
    else
    {
        m_textZone.resetHighlight();
        setDirty();
    }
}

Bool EditBox::mouseMove(Int32 x, Int32 y)
{
    if (m_editBoxState[STATE_MOUSE_SELECT] && m_getText.text().isValid())
    {    
        Int32 pos = m_selection[1];

        if (!m_editBoxState[STATE_SELECTING])
        {
            // begin selection
            m_editBoxState.enable(STATE_SELECTING);

            // exclusive when outside
            ((Gui*)getScene()->getGui())->getWidgetManager()->lockWidgetMouse();

            m_selection[0] = m_getText.cursorPos();

            // start with an empty highlight
            m_textZone.resetHighlight();

            setDirty();
        }

        // slide the text left/right if the mouse goes outside of the widget
        if (x - m_pos.x() < 0)
        {
            if (m_getText.cursorPos() > 0)
            {
                pos = m_getText.cursorPos() - 1;

                m_getText.setCursorPos(pos);
                updateCaption();

                setDirty();
            }
        }
        else if (x - m_pos.x() > m_size.x())
        {
            if (m_getText.cursorPos() < m_getText.text().length())
            {
                pos = m_getText.cursorPos() + 1;

                m_getText.setCursorPos(pos);
                updateCaption();

                setDirty();
            }
        }
        else
        {
            // defines the caret position
            pos = m_getText.font()->nearestChar(m_textZone.text(), x - m_pos.x());
            pos = o3d::clamp(pos, 0, (Int32)m_textZone.text().length());

            // locate the caret
            m_getText.setCursorPos(pos + m_visible[0]);
            updateCaption();

            pos += m_visible[0];
        }

        if (m_selection[1] != pos)
        {
            m_selection[1] = pos;
            applySelection();
        }
    }

	return True;
}

void EditBox::focused()
{
	m_editing = True;
	m_caret.resetVisibilityState();

	setDirty();
}

void EditBox::lostFocus()
{
	m_editing = False;

    // losing focus means loosing selection
    m_selection[0] = m_selection[1] = 0;
    applySelection();

    // it set the dirty flag
}

Bool EditBox::keyboardToggled(Keyboard *keyboard, KeyEvent event)
{
    if (isActive() && keyboard && m_editing && m_getText.text().isValid())
	{
		Bool result = False;

        // select
        if ((event.key() == KEY_LSHIFT) || (event.key() == KEY_RSHIFT))
        {
            if (event.isPressed() && !m_editBoxState[STATE_KEYBOARD_SELECT])
            {
                m_editBoxState.enable(STATE_KEYBOARD_SELECT);

                // set if no current selection else keep current
                if (m_selection[0] == m_selection[1])
                    m_selection[0] = m_selection[1] = m_getText.cursorPos();

                return True;
            }
            else if (event.isReleased() && m_editBoxState[STATE_KEYBOARD_SELECT])
            {
                m_editBoxState.disable(STATE_KEYBOARD_SELECT);

                // terminate to select
                if (!m_editBoxState[STATE_MOUSE_SELECT])
                    m_editBoxState.disable(STATE_SELECTING);

                return True;
            }
        }

        // update the content or the caret position if change
		if (m_getText.keyboardToggled(keyboard, event))
		{
			result = True;

            // display placeholder when text is empty
            if (m_getText.text().isEmpty() && m_placeholder.isValid())
                updateText();
            else
                updateCaption();

            // make caret still visible during navigation
            m_caret.resetVisibilityState();

            if (m_editBoxState[STATE_KEYBOARD_SELECT])
            {
                Int32 pos = (Int32)m_getText.cursorPos();

                if (pos != m_selection[0] && !m_editBoxState[STATE_SELECTING])
                    m_editBoxState.enable(STATE_SELECTING);

                if (m_editBoxState[STATE_SELECTING])
                {
                    m_selection[1] = m_getText.cursorPos();
                    applySelection();
                }

                return True;
            }
            else if (m_selection[0] != m_selection[1])
            {
                // invalidate the selection
                m_selection[0] = m_selection[1] = 0;

                m_textZone.resetHighlight();
                setDirty();

                return True;
            }
		}

		// Enter key
		if (event.isPressed())
		{
            if ((event.key() == KEY_RETURN) || (event.key() == KEY_NUMPAD_ENTER))
			{
                if (m_getText.text().length() > 0)
                    onEnterPressed();

                return True;
			}
		}

		return result;
	}

	return False;
}

Bool EditBox::character(Keyboard *keyboard, CharacterEvent event)
{
    if (isActive() && keyboard && m_editing)
	{
		// escape
		if (event.unicode() == 27)
			return False;

		// update the content if change
		if (m_getText.character(keyboard, event))
		{
            // make caret still visible during typing
            m_caret.resetVisibilityState();

            // terminate any selection
            if (m_editBoxState[STATE_SELECTING])
            {
                m_editBoxState.disable(STATE_SELECTING);
                m_editBoxState.disable(STATE_MOUSE_SELECT);
                m_editBoxState.disable(STATE_KEYBOARD_SELECT);
            }

            if (m_selection[0] != m_selection[1])
            {
                // cut the selection before insert or replace
                String text = m_getText.text();
                UInt32 cp = m_getText.cursorPos();

                // is a seletion, cut it
                if (m_selection[1] > m_selection[0])
                {               
                    text.remove(m_selection[0], m_selection[1]-m_selection[0]);
                    cp = m_selection[0] + 1;
                }
                else if (m_selection[0] > m_selection[1])
                {
                    text.remove(m_selection[1]+1, m_selection[0]-m_selection[1]);
                }

                // invalid the selection
                m_selection[0] = m_selection[1] = 0;

                m_getText.setText(text, cp);
                m_textZone.resetHighlight();
            }

			updateCaption();

            // display placeholder when text is empty
            if (m_getText.text().isEmpty() && m_placeholder.isValid())
                updateText();

			return True;
		}
	}

	return False;
}

// Update for keyboard entry (GetText)
void EditBox::updateCaption()
{
    ABCFont *font = getUsedFont();
    Int32 fontHeight = getUsedFontHeight();

    font->setTextHeight(fontHeight);

    UInt32 caretPos = m_getText.cursorPos();

    String dstText;
    Int32 caretPixelPos;

    Int32 limits[2]{m_leftPos, m_leftPos + m_textZone.box().width()-1};

    if (m_editBoxStyle == EDITBOX_HIDDEN_CHAR)
    {
        String srcText;
        srcText.setFill('*', m_getText.text().length());

        caretPixelPos = caretPos * font->sizeOf('*');
        caretPixelPos = font->adjust(srcText, limits, caretPixelPos, m_visible, m_leftPos, dstText);
    }
    else
    {
        const String &srcText = m_getText.text();
        caretPixelPos = font->sizeOf(srcText.sub(0, caretPos));
        caretPixelPos = font->adjust(srcText, limits, caretPixelPos, m_visible, m_leftPos, dstText);
    }
/*
    const String &srcText = m_getText.text();

    if (m_editBoxStyle == EDITBOX_HIDDEN_CHAR)
        caretPixelPos = caretPos * font->sizeOf('*');
    else
        caretPixelPos = font->sizeOf(srcText.sub(0, caretPos));

    //
    // readjust the previous window according to the new caret location
    // using the previous left coordinates in pixels
    //

    // fill n char from leftPos to width
    Int32 pos = 0;
    UInt32 firstCharIndex = 0;

    Int32 rightPos = m_leftPos + m_textZone.box().width();
    Int32 leftChar = 0;

    // caret is at left out-screen
    if (caretPixelPos < m_leftPos)
    {
        // shift to caret position
        m_leftPos = o3d::max(caretPixelPos - 4, 0);
    }

    // caret is at right out-screen
    if (caretPixelPos > rightPos)
    {
        // shift to contain the caret at the last character
        m_leftPos = caretPixelPos - m_textZone.box().width();
        rightPos = m_leftPos + m_textZone.box().width();
    }

    //
    // now update the content TODO optimizations
    //

    Int32 charWidth;

    // TODO does not use per character increment because of the kerning
    // or the initial left bearing (if no kerning), so uses of a global approach

    // first, search for the first character
    if (m_editBoxStyle == EDITBOX_HIDDEN_CHAR)
    {
        charWidth = font->sizeOf('*');

        while ((pos < m_leftPos) && (firstCharIndex < srcText.length()))
        {
            pos += charWidth;
            ++firstCharIndex;
        }
    }
    else
    {
        while ((pos < m_leftPos) && (firstCharIndex < srcText.length()))
        {
            pos += font->sizeOf(srcText[firstCharIndex]);
            ++firstCharIndex;
        }
    }

    m_leftPos = pos;
    leftChar = firstCharIndex;

    // next, fill enough characters to fill the text zone in width
    UInt32 lastCharIndex = firstCharIndex;
    Int32 width = 0;

    if (m_editBoxStyle == EDITBOX_HIDDEN_CHAR)
    {
        charWidth = font->sizeOf('*');

        while ((pos < rightPos) && (lastCharIndex < srcText.length()))
        {
            pos += charWidth;
            width += charWidth;

            dstText.append('*');

            ++lastCharIndex;
        }
    }
    else
    {
        while ((pos < rightPos) && (lastCharIndex < srcText.length()))
        {
            charWidth = font->sizeOf(srcText[lastCharIndex]);

            pos += charWidth;
            width += charWidth;

            dstText.append(srcText[lastCharIndex]);

            ++lastCharIndex;
        }
    }

    // last char is partially display, does not count into visible
    if (pos > rightPos)
    {
        --lastCharIndex;
    }

    // free space at right and some characters out of screen at left
    // then shift of some char to right
    if ((width < m_textZone.box().width()) && (m_leftPos > 0))
    {
        if (m_editBoxStyle == EDITBOX_HIDDEN_CHAR)
        {
            charWidth = font->sizeOf('*');

            while ((width < m_textZone.box().width()) && (firstCharIndex > 0))
            {
                --firstCharIndex;

                if (charWidth + width > m_textZone.box().width())
                {
                    // restore previous index because we want an exact value for m_visible
                    ++firstCharIndex;
                    break;
                }

                dstText.append('*');

                width += charWidth;
                m_leftPos -= charWidth;
                --leftChar;
            }
        }
        else
        {
            while ((width < m_textZone.box().width()) && (firstCharIndex > 0))
            {
                --firstCharIndex;

                charWidth = font->sizeOf(srcText[firstCharIndex]);

                if (charWidth + width > m_textZone.box().width())
                {
                    // restore previous index because we want an exact value for m_visible
                    ++firstCharIndex;
                    break;
                }

                dstText.insert(srcText[firstCharIndex], 0);

                width += charWidth;
                m_leftPos -= charWidth;
                --leftChar;
            }
        }
    }

    m_visible[0] = firstCharIndex;
    m_visible[1] = lastCharIndex;

    // new caret position set after defining the new text zone content
    caretPixelPos -= m_leftPos;
*/
    m_textZone.setText(dstText);
    m_caret.setPixelPosition(Vector2i(caretPixelPos, 0));

    // defines the highlighted text
    applySelection();

    setDirty();
}

// draw
void EditBox::draw()
{
	if (isShown())
	{
        processCache();

		// draw the caret if focused
        if (isActive() && m_editing)
            m_caret.draw(m_pos);
	}
}

void EditBox::updateCache()
{
    Theme *theme = getUsedTheme();

	// Draw EditBox
	if (!isActive())
        theme->drawWidget(Theme::EDITBOX_NORMAL, 0, 0, m_size.x(), m_size.y());
	else if (m_hover || m_editing)
        theme->drawWidget(Theme::EDITBOX_HOVER, 0, 0, m_size.x(), m_size.y());
	else
        theme->drawWidget(Theme::EDITBOX_NORMAL, 0, 0, m_size.x(), m_size.y());

	// text content
    const WidgetDrawMode *pDrawMode = theme->getWidgetDrawMode(Theme::WINDOW_NORMAL);
	if (pDrawMode->getType() == WidgetDrawMode::COMPASS_MODE)
	{
		const WidgetDrawCompass *pCompassMode = (WidgetDrawCompass*)pDrawMode;

		m_textZone.setPos(
			pCompassMode->getEltWidth(WidgetDrawCompass::WEST),
			pCompassMode->getEltHeight(WidgetDrawCompass::NORTH));

        m_textZone.setSize(
			m_size.x() - (pCompassMode->getEltWidth(WidgetDrawCompass::WEST) +
				(pCompassMode->getEltWidth(WidgetDrawCompass::EAST))),
			m_size.y() - (pCompassMode->getEltWidth(WidgetDrawCompass::NORTH) +
				(pCompassMode->getEltHeight(WidgetDrawCompass::SOUTH))));
	}
	else
	{
        m_textZone.setPos(0, 0);
        m_textZone.setSize(m_size.x(), m_size.y());
	}

    if (m_getText.text().isValid())
        m_textZone.write(getUsedFont(), getUsedFontHeight());
    else
    {
        // color of the placeholder is 50% of the inverse normal
        Color color = m_textZone.color();

        m_textZone.setColor(Color(
                                (1.0f - color.r()) * 0.5f,
                                (1.0f - color.g()) * 0.5f,
                                (1.0f - color.b()) * 0.5f,
                                color.a()));

        m_textZone.write(getUsedFont(), getUsedFontHeight());

        m_textZone.setColor(color);
    }

}

