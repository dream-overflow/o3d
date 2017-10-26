/**
 * @file statictext.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/statictext.h"

#include "o3d/gui/widgets/layout.h"
#include "o3d/gui/guitype.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(StaticText, GUI_WIDGET_STATICTEXT, Widget)

// direct rendering (1) or render to target (0)
#define DIRECT_RENDERING 0

/*---------------------------------------------------------------------------------------
  default constructor
---------------------------------------------------------------------------------------*/
StaticText::StaticText(BaseObject *parent) :
	Widget(parent),
	m_textZone()
{
}

StaticText::StaticText(Widget *parent,
	const String &label,
	const Vector2i &pos,
	const Vector2i &size,
	Bool multiLine,
	const String &name) :
		Widget(parent, pos, size, name),
		m_textZone()
{
	m_textZone.setText(label);
	m_textZone.setMultiLine(multiLine);

	init();
}

void StaticText::init()
{
    m_capacities.disable(CAPS_FOCUSABLE);

	m_texture.setFiltering(Texture::NO_FILTERING);

    // calc the default size
    Theme *theme = getUsedTheme();

    if ((m_minSize.x() <= 0) && m_textZone.isMultiLine())
        O3D_ERROR(E_InvalidParameter("Multiline static text must at least have a defined width."));

    m_textZone.setVerticalAlignment(TextZone::MIDDLE_ALIGN);

    adjustBestSize();

    // TODO adjust to text zone position to be entire text
/*
#if DIRECT_RENDERING == 1
    m_textZone.setPos(m_pos);
#else
    m_textZone.setPos(0, 0);
#endif

    m_textZone.setWidth(m_minSize.x());

	Vector2i defaultSize = getDefaultSize();

    if (m_minSize.x() < 0)
        m_minSize.x() = defaultSize.x();

    if (m_minSize.y() < 0)
        m_minSize.y() = defaultSize.y();

	m_size = m_minSize;
    m_textZone.setSize(m_size.x(), m_size.y());
*/
	// default color
    m_textZone.setColor(theme->getWidgetDrawMode(Theme::STATIC_TEXT_FALSE)->getDefaultTextColor());
}

StaticText::~StaticText()
{
}

Vector2i StaticText::fit()
{
    // force no default size to recompute it
    m_minSize.set(-1, -1);

    adjustBestSize();
    return m_size;
}

void StaticText::adjustBestSize()
{
    // calc the default button size
    Theme *theme = getUsedTheme();
    const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::STATIC_TEXT_FALSE);

    Vector2i defaultSize = getDefaultSize();

    Vector2i textSize = m_textZone.sizeOf(
        theme->getDefaultFont(),
        theme->getDefaultFontSize());

    textSize += drawMode->getTopLeftMargin() + drawMode->getBottomRightMargin();

    if (m_minSize.x() < 0)
        m_minSize.x() = o3d::max(textSize.x(), defaultSize.x());

    if (m_minSize.y() < 0)
        m_minSize.y() = o3d::max(textSize.y(), defaultSize.y());

    m_size = m_minSize;
    m_textZone.setSize(m_size);

#if DIRECT_RENDERING == 1
    m_textZone.setPos(m_pos);
#else
    m_textZone.setPos(0, 0);
#endif
}

void StaticText::enable(Bool active)
{
    if (m_capacities.getBit(STATE_ACTIVITY) != active)
    {
        m_capacities.setBit(STATE_ACTIVITY, active);

        if (active)
            m_textZone.setColor(getUsedTheme()->getWidgetDrawMode(Theme::STATIC_TEXT_FALSE)->getDefaultTextColor());
        else
            m_textZone.setColor(getUsedTheme()->getWidgetDrawMode(Theme::STATIC_TEXT_DISABLED)->getDefaultTextColor());
    }
}

void StaticText::setMultiline(Bool multiLine)
{
	m_textZone.setMultiLine(multiLine);
	setDirty();
}

/*---------------------------------------------------------------------------------------
  get the recommended widget default size
---------------------------------------------------------------------------------------*/
Vector2i StaticText::getDefaultSize()
{
//    Theme *pTheme = getUsedTheme();

//    return m_textZone.sizeOf(
//        pTheme->getDefaultFont(),
//        pTheme->getDefaultFontSize());

    return getUsedTheme()->getDefaultWidgetSize(Theme::STATIC_TEXT_FALSE);
}

/*---------------------------------------------------------------------------------------
  Events Management
---------------------------------------------------------------------------------------*/
void StaticText::positionChanged()
{
#if DIRECT_RENDERING == 1
    m_textZone.setPos(m_pos.x(), m_pos.y());
#endif
}

void StaticText::sizeChanged()
{
    m_textZone.setSize(m_size.x(), m_size.y());
	setDirty();
}

Bool StaticText::isTargeted(Int32 x, Int32 y, Widget *&pWidget)
{
	// no negative part
	if (x < 0 || y < 0)
		return False;

    if ((x < m_size.x()) && (y < m_size.y()))
	{
		pWidget = this;
		return True;
	}

	return False;
}

//---------------------------------------------------------------------------------------
// draw
//---------------------------------------------------------------------------------------
void StaticText::draw()
{
	if (!isShown())
		return;

#if DIRECT_RENDERING == 1
    if (isDirty())
    {
        m_textZone.setPos(m_pos.x(), m_pos.y());
        m_textZone.setSize(m_size.x(), m_size.y());

        setClean();
    }

    updateCache();
#else
    processCache();
#endif
}

void StaticText::updateCache()
{
    Theme *theme = getUsedTheme();

#if DIRECT_RENDERING == 0
    m_textZone.setPos(0, -2);
    //m_textZone.setSize(m_size.x(), m_size.y());
    theme->getDefaultFont()->setBlendFunc(Blending::DISABLED);
#endif
	m_textZone.write(
        theme->getDefaultFont(),
        theme->getDefaultFontSize());
#if DIRECT_RENDERING == 0
    theme->getDefaultFont()->setBlendFunc(Blending::ONE__ONE_MINUS_SRC_A___ONE__ONE_MINUS_SRC_A);
#endif
}

void StaticText::layout()
{
    adjustBestSize();
    m_minSize = m_size;
}

