/**
 * @file textzone.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/textzone.h"

#include "o3d/core/stringtokenizer.h"
#include "o3d/gui/gui.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/gl.h"
#include "o3d/engine/object/primitivemanager.h"
#include "o3d/core/templatemanager.h"

using namespace o3d;

TextZone::TextZone() :
    m_font(nullptr),           // default comes from the parameters
    m_fontHeight(-1),          // default comes from the parameters
    m_color(0.f, 0.f, 0.f),    // black by default
    m_highlightColor(0.25f, 0.35f, 1.0f),  // kind of blue
	m_hTextAlign(LEFT_ALIGN),
	m_vTextAlign(TOP_ALIGN),
	m_multiLine(False),
    m_highlight{0, 0},
	m_text("")
{
}

TextZone::~TextZone()
{
}

void TextZone::createMultiLineList(ABCFont *pCurFont)
{
	// Clear previous list
	m_textMultiLined.clear();

	// Preconditions
	if (!pCurFont || (pCurFont->getTextHeight() <= 0) || (m_box.width() <= 0))
		return;

	StringTokenizer strToken(m_text, L" \n\t");
	String currLine = "";
	String nextWord;
	Bool remainWord = False;

	while (strToken.hasMoreElements() || remainWord)
	{
		// Do we need to read a new word
		if (!remainWord)
			nextWord = strToken.nextElement();
		else
			remainWord = False;

		// Can the current line contain the new word
		if (pCurFont->sizeOf(currLine) + pCurFont->sizeOf(nextWord) <= m_box.width())
		{
			currLine.concat(nextWord);
			currLine.concat(" ");
		}
		else
		{
			remainWord = True;
			if (currLine == "")
			{
				// The first word is too long, we need to cut it
				currLine = nextWord.getData()[0];
				nextWord = nextWord.sub(1);

                while (pCurFont->sizeOf(currLine) + pCurFont->sizeOf(nextWord.getData()[0]) <= m_box.width())
				{
					currLine.concat(nextWord.getData()[0]);
					nextWord = nextWord.sub(1);
				}
			}
			else
			{
				// Add the currLine to the list of lines
				m_textMultiLined.push_back(currLine);
				currLine = "";
			}
		}
	}

	// Add the last line
	m_textMultiLined.push_back(currLine);

	setClean();
}

const String& TextZone::getTextLine(UInt32 row) const
{
    if (!m_multiLine) {
		return m_text;
    } else {
        if (row < m_textMultiLined.size()) {
			return m_textMultiLined[row];
        } else {
			return m_text;
        }
	}
}

void TextZone::write(ABCFont *defaultFont, Int32 defaultFontSize)
{
	// Preconditions
	ABCFont *curFont = m_font;
    if (!curFont) {
		curFont = defaultFont;
		setDirty();
	}

    if (!curFont) {
		return;
    }

	Int32 curFontSize = m_fontHeight;
    if (curFontSize < 0) {
		curFontSize = defaultFontSize;
		setDirty();
	}

    if (curFontSize < 0) {
		return;
    }

    if (m_text.isEmpty()) {
        return;
    }

	Bool prevScissorTest = False;
	Box2i prevScissor;

	// Set the Scissor rectangle
    if ((m_box.width() >= 0) && (m_box.height() >= 0)) {
		Int32 posX, posY, bottom;
		Matrix4 modlv,proj;

		modlv = curFont->getScene()->getContext()->modelView().get();
		proj = curFont->getScene()->getContext()->projection().get();

		bottom = (Int32)(-2.f / proj.m22());

        posX = m_box.x() + (Int32)modlv.getTranslation().x();
        posY = bottom - 1 - (m_box.y() + (Int32)modlv.getTranslation()[Y] + m_box.height() + 1);

		// a current scissor test
		if (curFont->getScene()->getContext()->getScissorTest())
		{
			prevScissorTest = True;
			prevScissor = curFont->getScene()->getContext()->getScissor();

            Box2i textScissor(posX, posY, m_box.width(), m_box.height());
			textScissor = prevScissor.clamp(textScissor);

			curFont->getScene()->getContext()->setScissor(textScissor);
		}
		else
		{
			curFont->getScene()->getContext()->enableScissorTest();
            curFont->getScene()->getContext()->setScissor(posX, posY, m_box.width(), m_box.height());
		}
	}

	// Font setup
	curFont->setTextHeight(curFontSize);
	curFont->setColor(m_color);

	if (isDirty() && m_multiLine)
		createMultiLineList(curFont);

	// Get the whole text height
	Int32 textHeight = curFontSize;
	if (!m_multiLine)
		textHeight = curFontSize;
	else
		textHeight = (Int32)m_textMultiLined.size() * (textHeight + curFont->getInterline());

    // Get the height to vertically offset the text
    Int32 firstLineOfs = 0;
	switch (m_vTextAlign)
	{
	case TOP_ALIGN:
        firstLineOfs = curFontSize;
		break;
	case MIDDLE_ALIGN:
        firstLineOfs = curFontSize + (((m_box.height() < 0 ? 0 : m_box.height()) - textHeight) / 2);
		break;
	case BOTTOM_ALIGN:
        firstLineOfs = (m_box.height() < 0 ? 0 : m_box.height()) - textHeight;
		break;
	}

	// Write text
	if (!m_multiLine)
	{
        // Get the width to horizontally offset the text
        Int32 firstColOfs = 0;
		switch (m_hTextAlign)
		{
		case LEFT_ALIGN:
            firstColOfs = 0;
			break;
		case CENTER_ALIGN:
            firstColOfs = ((m_box.width() < 0 ? 0 : m_box.width()) - curFont->sizeOf(m_text)) / 2;
			break;
		case RIGHT_ALIGN:
			// TODO :
            firstColOfs = (m_box.width() < 0 ? 0 : m_box.width()) - curFont->sizeOf(m_text);
			break;
		}

        // draw the highlighted selection area
        if (m_highlight[0] != m_highlight[1])
        {
            // selection has the same height as the caret
            Int32 selHeight = curFontSize + 1;
            Int32 row = selHeight;
            Int32 top = (selHeight*4)/5;

            // compute the position of the start/end characters
            // always from start of text because of kerning
            Int32 begin = curFont->sizeOf(m_text.sub(0, m_highlight[0]));
            Int32 end = curFont->sizeOf(m_text.sub(0, m_highlight[1]));

            PrimitiveAccess pa = PrimitiveAccess(curFont->getScene()->getPrimitiveManager());

            pa->setModelviewProjection(curFont->getScene()->getContext()->modelViewProjection());
            pa->setColor(m_highlightColor);

            pa->beginDraw(P_TRIANGLE_STRIP);

            pa->addVertex(Vector3(
                              m_box.x() + firstColOfs + begin,    // bottom-left
                              m_box.y() + firstLineOfs + row - top,
                              0.f));
            pa->addVertex(Vector3(
                              m_box.x() + firstColOfs + end,      // bottom-right
                              m_box.y() + firstLineOfs + row - top,
                              0.f));
            pa->addVertex(Vector3(
                              m_box.x() + firstColOfs + begin,
                              m_box.y() + firstLineOfs - top,  // top-left
                              0.f));
            pa->addVertex(Vector3(
                              m_box.x() + firstColOfs + end,
                              m_box.y() + firstLineOfs - top,  // top-right
                              0.f));

            pa->endDraw();
        }

        curFont->write(Vector2i(m_box.x() + firstColOfs, m_box.y() + firstLineOfs), m_text);
	}
	else
	{
        Int32 yOfs = 0;

		for (std::vector<String>::iterator it = m_textMultiLined.begin() ; it != m_textMultiLined.end() ; ++it)
		{
            Int32 firstColOfs = 0;
            // Get the width to horizontally offset the text
			switch (m_hTextAlign)
			{
			case LEFT_ALIGN:
                firstColOfs = 0;
				break;
			case CENTER_ALIGN:
                firstColOfs = ((m_box.width() < 0 ? 0 : m_box.width()) - curFont->sizeOf(*it)) / 2;
				break;
			case RIGHT_ALIGN:
				// TODO :
                firstColOfs = (m_box.width() < 0 ? 0 : m_box.width()) - curFont->sizeOf(*it);
				break;
			}

            curFont->write(Vector2i(m_box.x() + firstColOfs, m_box.y() + firstLineOfs + yOfs), *it);

            // TODO draw the highlighted selection area

            yOfs += curFontSize + curFont->getInterline();
		}
	}

	// Restore scissor test
    if ((m_box.width() >= 0) && (m_box.height() >= 0))
	{
		if (prevScissorTest)
			curFont->getScene()->getContext()->setScissor(prevScissor);
		else
			curFont->getScene()->getContext()->disableScissorTest();
	}
}

Vector2i TextZone::sizeOf(ABCFont *pDefaultFont, Int32 defaultFontSize)
{
	Vector2i size;

	// Preconditions
	ABCFont *pCurFont = m_font;
	if (!pCurFont)
	{
		pCurFont = pDefaultFont;
		setDirty();
	}

	if (!pCurFont)
		return size;

	Int32 curFontSize = m_fontHeight;
	if (curFontSize < 0)
	{
		curFontSize = defaultFontSize;
		setDirty();
	}

	if (curFontSize < 0)
		return size;

	// Font setup
	pCurFont->setTextHeight(curFontSize);

	if (isDirty() && m_multiLine)
		createMultiLineList(pCurFont);

	// Get the whole text height
	Int32 textHeight = curFontSize;
	if (!m_multiLine)
		textHeight = curFontSize;
	else
		textHeight = (Int32)m_textMultiLined.size() * (textHeight + pCurFont->getInterline());

    // Get the height to vertically offset the text
    Int32 firstLineOfs = 0;
//    switch (m_vTextAlign)
//    {
//    case TOP_ALIGN:
//        firstLineOfs = curFontSize;
//        break;
//    case MIDDLE_ALIGN:
//        firstLineOfs = curFontSize + (((m_box.height() < 0 ? 0 : m_box.height()) - textHeight) / 2);
//        break;
//    case BOTTOM_ALIGN:
//        // TODO : checkRange(0, (m_box.height() < 0 ? 0 : m_box.height()))
//        firstLineOfs = (m_box.height() < 0 ? 0 : m_box.height()) - textHeight;
//        break;
//    }

	// Write text
	if (!m_multiLine)
	{
        // Get the width to horizontally offset the text
        Int32 firstColOfs = 0;
//        switch (m_hTextAlign)
//        {
//        case LEFT_ALIGN:
//            firstColOfs = 0;
//            break;
//        case CENTER_ALIGN:
//            firstColOfs = ((m_box.width() < 0 ? 0 : m_box.width()) - pCurFont->sizeOf(m_text)) / 2;
//            break;
//        case RIGHT_ALIGN:
//            // TODO :
//            firstColOfs = (m_box.width() < 0 ? 0 : m_box.width()) - pCurFont->sizeOf(m_text);
//            break;
//        }

        size.x() = firstColOfs + pCurFont->sizeOf(m_text);
        size.y() = firstLineOfs + pCurFont->getTextHeight();
	}
	else
	{
		for (std::vector<String>::iterator it = m_textMultiLined.begin() ; it != m_textMultiLined.end() ; ++it)
		{
            Int32 firstColOfs = 0;
            // Get the width to horizontally offset the text
//            switch (m_hTextAlign)
//            {
//            case LEFT_ALIGN:
//                firstColOfs = 0;
//                break;
//            case CENTER_ALIGN:
//                firstColOfs = ((m_box.width() < 0 ? 0 : m_box.width()) - pCurFont->sizeOf(*it)) / 2;
//                break;
//            case RIGHT_ALIGN:
//                // TODO :
//                firstColOfs = (m_box.width() < 0 ? 0 : m_box.width()) - pCurFont->sizeOf(*it);
//                break;
//            }

            size.x() = o3d::max(size.x(), firstColOfs + pCurFont->sizeOf(*it));
			size.y() += pCurFont->getTextHeight() + pCurFont->getInterline();
		}

        size.y() += firstLineOfs;
	}

	return size;
}

void TextZone::setHighlight(Int32 begin, Int32 end)
{
    m_highlight[0] = begin;
    m_highlight[1] = end;
    setDirty();
}

void TextZone::resetHighlight()
{
    m_highlight[0] = m_highlight[1] = 0;
    setDirty();
}
