/**
 * @file caret.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/caret.h"

using namespace o3d;

// default constructor. take an TextZone object
Caret::Caret(TextZone *textZone) :
    m_textZone(textZone),
    m_isVisible(True),
    m_blindTime(500)
{
    O3D_ASSERT(m_textZone != nullptr);
}

void Caret::resetVisibilityState()
{
    m_blindTime.reset();
    m_isVisible = True;
}

void Caret::draw(const Vector2i &pos)
{
    m_blindTime.update();

    if (m_blindTime.check())
        m_isVisible = !m_isVisible;

    if (m_isVisible && m_textZone && m_textZone->font())
	{
        ABCFont *font = m_textZone->font();
        Int32 textHeight = m_textZone->fontHeight();
        font->setTextHeight(textHeight);

        font->write(Vector2i(
            m_textZone->pos().x(),
            m_textZone->pos().y() + textHeight) + pos + m_pixelPos,
			"",0);
	}
}

void Caret::setPixelPosition(const Vector2i &pos)
{
    m_pixelPos = pos;
}

