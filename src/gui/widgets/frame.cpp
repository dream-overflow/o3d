/**
 * @file frame.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/frame.h"

#include "o3d/gui/precompiled.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"
#include "o3d/gui/widgetdrawcompass.h"
#include "o3d/gui/widgets/boxlayout.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Frame, GUI_WIDGET_FRAME, Widget)

// Default constructor
Frame::Frame(BaseObject *parent) :
        Widget(parent),
        m_style(DEFAULT_STYLE)
{
}

// Creation constructor
Frame::Frame(
	Widget *parent,
	const Vector2i &pos,
	const Vector2i &size,
	FrameStyle style,
	const String &name) :
		Widget(parent, pos, size, name),
		m_style(style)
{
	m_pos = pos;
	m_minSize = m_size = size;

	init();
}

void Frame::init()
{
    m_capacities.enable(CAPS_CONTAINER);

	// default position
    if (m_pos.x() < 0) m_pos.x() = 0;
    if (m_pos.y() < 0) m_pos.y() = 0;

	// default size
	Vector2i defaultSize = getDefaultSize();
    if (m_size.x() < 0) m_minSize.x() = m_size.x() = defaultSize.x();
    if (m_size.y() < 0) m_minSize.y() = m_size.y() = defaultSize.y();

	// default layout
	m_pLayout = new BoxLayout(this, BoxLayout::VERTICAL_LAYOUT);
	m_pLayout->setParent(this);
	m_pLayout->defineLayoutItem();

	// update the window
	sizeChanged();
}

Frame::~Frame()
{
}

void Frame::setLayout(Layout *pLayout)
{
	m_pLayout = pLayout;
}

const Layout* Frame::getLayout() const
{
	return m_pLayout.get();
}

Layout* Frame::getLayout()
{
	return m_pLayout.get();
}

Vector2i Frame::getOrigin() const
{
	if ((m_style & BORDER_STYLE) != 0)
	{
		const Theme *theme = getUsedTheme();
		const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::FRAME_NORMAL);

		return drawMode->getTopLeftMargin();
	}
	else
        return Vector2i(0, 0);
}

Widget *Frame::findNextTabIndex(Widget *widget, Int32 direction)
{
    if (getLayout())
        return getLayout()->findNextTabIndex(widget, direction);

    return widget;
}

Vector2i Frame::getDefaultSize()
{
	return getUsedTheme()->getDefaultWidgetSize(Theme::FRAME_NORMAL);
}

Bool Frame::isTargeted(Int32 x, Int32 y, Widget *&pWidget)
{
	// no negative part
	if (x < 0 || y < 0)
		return False;

    if ((x < m_size.x()) && (y < m_size.y()))
	{
		// others childs widgets
		Vector2i org = getOrigin();
		pWidget = this;

		// delta by the client origin for childs widgets
		// title bar buttons are in childs space, using
		// negatives coordinates.
        x -= org.x();
        y -= org.y();

		// recursive targeted
		if (getLayout() && getLayout()->isTargeted(
            x - getLayout()->pos().x(),
            y - getLayout()->pos().y(),
			pWidget))
		{
			return True;
		}

		return True;
	}

	return False;
}

void Frame::focused()
{

}

void Frame::lostFocus()
{

}

Vector2i Frame::getClientSize() const
{
	// compute the client size
	const Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::FRAME_NORMAL);
	if (!drawMode)
		return m_size;

	Vector2i size = m_size;

	if ((m_style & BORDER_STYLE) != 0)
     size -= drawMode->getTopLeftMargin() + drawMode->getBottomRightMargin();

	return size;
}

void Frame::sizeChanged()
{
	const Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::FRAME_NORMAL);
	if (!drawMode)
		return;

	// need to update the cache
	setDirty();
}

void Frame::draw()
{
	if (isShown())
	{
		// is need to recompute the layout
		if (getLayout() && getLayout()->isDirty())
			getLayout()->layout();

		Vector2i org = getOrigin();

		processCache();

		// translate in local coordinates
		getScene()->getContext()->modelView().translate(
                Vector3((Float)m_pos.x(), (Float)m_pos.y(), 0.f));

		// translate in client area local coordinates
		getScene()->getContext()->modelView().translate(
                Vector3((Float)org.x(), (Float)org.y(),0.f));

		// finally draw its content into the client area
		if (getLayout())
		{
			getScene()->getContext()->modelView().push();
			getLayout()->draw();
			getScene()->getContext()->modelView().pop();
		}
	}
}

void Frame::updateCache()
{
	if ((m_style & BORDER_STYLE) != 0)
	{
		Theme *theme = getUsedTheme();

		// Draw borders
        theme->drawWidget(Theme::FRAME_NORMAL, 0, 0, m_size.x(), m_size.y());
	}

    setClean();
}

