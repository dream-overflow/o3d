/**
 * @file abcfont.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/abcfont.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"
#include "o3d/core/debug.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS2(ABCFont, GUI_FONT_ABC, SceneEntity, Text2D)

//! default constructor
ABCFont::ABCFont(
	BaseObject *pParent,
	CharSet charSet,
	FontStyle style) :
		SceneEntity(pParent),
		m_style(style),
		m_CharSet(charSet),
		m_nChar(0),
		m_charH(16),
		m_color(1.f,1.f,1.f),
        m_blendFunc(Blending::ONE__ONE_MINUS_SRC_A___ONE__ONE_MINUS_SRC_A)
{
}

ABCFont::~ABCFont()
{

}

void ABCFont::setTextHeight(Int32 y)
{
    m_charH = y;
}

Int32 ABCFont::getTextHeight() const
{
    return m_charH;
}

// Write a string at the n° row and a cursor at curpos position (-1 mean no cursor)
Int32 ABCFont::writeAtRow(Int32 row, Int32 x, const String& text, Int32 curspos)
{
    return write(Vector2i(x, row*m_charH), text, curspos);
}

// set the text area size (in number of char per row/col)
void ABCFont::setNumCharHeight(Int32 y)
{
    m_charH = ((Gui*)getScene()->getGui())->getHeight() / y;
}

// get the text area size (in number of char per row/col)
Int32 ABCFont::getNumCharHeight() const
{
    return ((Gui*)getScene()->getGui())->getHeight() * m_charH;
}

void ABCFont::setColor(const Color &col)
{
    m_color = col;
}

const Color &ABCFont::getColor() const
{
    return m_color;
}

void ABCFont::setBlendFunc(Blending::FuncProfile func)
{
    m_blendFunc = func;
}

Blending::FuncProfile ABCFont::getBlendFunc() const
{
    return m_blendFunc;
}
