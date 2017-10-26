/**
 * @file pane.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/pane.h"
#include "o3d/gui/widgetdrawmode.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/gui/widgets/boxlayout.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Pane, GUI_WIDGET_PANE, Widget)

Pane::Pane(BaseObject *parent) :
	Widget(parent),
	m_paneStyle(DEFAULT_STYLE)
{
}

Pane::Pane(
	Widget *parent,
	const String &label,
	const Vector2i &pos,
	const Vector2i &size,
	PaneStyle style,
	const String &name) :
		Widget(parent, pos, size, name),
		m_paneStyle(style)
{
	m_label.setText(label);
	init();
}

void Pane::init()
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

	Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::FRAME_NORMAL);

	// because we center the text we dont have a relative position

	m_label.setHorizontalAlignment(TextZone::LEFT_ALIGN);
	m_label.setVerticalAlignment(TextZone::MIDDLE_ALIGN);

	m_textSize = m_label.sizeOf(
		theme->getDefaultFont(),
		theme->getDefaultFontSize());

	m_label.setSize(m_textSize);

	Vector2i textPos(drawMode->getLeftMargin()*2+theme->getDefaultFontSize(), theme->getDefaultFontSize()/2);
	m_label.setPos(textPos);

	// text color
	m_label.setColor(theme->getWidgetDrawMode(Theme::FRAME_NORMAL)->getDefaultTextColor());

	// label rect
	m_labelRect.set(
				Vector2i(drawMode->getLeftMargin()*2, 0),
				Vector2i(m_textSize.x()+2*theme->getDefaultFontSize(), 2*theme->getDefaultFontSize()));

	// border rect
	if ((m_paneStyle & LABEL_STYLE) != 0)
		m_borderRect.set(Vector2i(0, textPos.y()), m_size - Vector2i(0, textPos.y()));
	else
		m_borderRect.set(Vector2i(0, 0), m_size);

    m_pLayout->setSize(getClientSize());
}

Pane::~Pane()
{
}

Vector2i Pane::getDefaultSize()
{
	return getUsedTheme()->getDefaultWidgetSize(Theme::FRAME_NORMAL);
}

Vector2i Pane::getClientSize() const
{
	// compute the client size
	const Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::FRAME_NORMAL);
	if (!drawMode)
		return m_size;

	Vector2i size = m_size;

	if ((m_paneStyle & LABEL_STYLE) != 0)
	{
        size.x() -= drawMode->getLeftMargin() + drawMode->getRightMargin();
		size.y() -= m_labelRect.y2();
	}
	else if ((m_paneStyle & BORDER_STYLE) != 0)
	{
        size -= drawMode->getTopLeftMargin() + drawMode->getBottomRightMargin();
	}

	return size;
}

void Pane::setLabel(const String &label)
{
	m_label.setText(label);

	Theme *theme = getUsedTheme();

	m_textSize = m_label.sizeOf(
		theme->getDefaultFont(),
		theme->getDefaultFontSize());

	// label rect
	m_labelRect.width() = min(
				m_textSize.x()+2*theme->getDefaultFontSize(),
				m_size.x()-2*theme->getDefaultFontSize());

	// and the label size
	m_label.setSize(m_labelRect.width() - 2*theme->getDefaultFontSize(), m_textSize.y());

	// update is needed
	setDirty();
}

const String &Pane::getLabel() const
{
	return m_label.text();
}

void Pane::setLayout(Layout *pLayout)
{
	m_pLayout = pLayout;
	if (m_pLayout.isValid())
        m_pLayout->setSize(getClientSize());
}

const Layout* Pane::getLayout() const
{
	return m_pLayout.get();
}

Layout* Pane::getLayout()
{
	return m_pLayout.get();
}

Vector2i Pane::getOrigin() const
{
	if ((m_paneStyle & BORDER_STYLE) != 0)
	{
		const Theme *theme = getUsedTheme();
		const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::FRAME_NORMAL);

		Vector2i org;

		if ((m_paneStyle & LABEL_STYLE) != 0)
		{
			org.x() = drawMode->getLeftMargin();
			org.y() = m_labelRect.y2();
		}
		else if ((m_paneStyle & BORDER_STYLE) != 0)
		{
			org = drawMode->getTopLeftMargin();
		}

		return org;
	}
	else
		return Vector2i(0, 0);
}

Bool Pane::isTargeted(Int32 x, Int32 y, Widget *&pWidget)
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

void Pane::sizeChanged()
{
	Vector2i textPos = m_label.pos();

	Theme *theme = getUsedTheme();

	// label rect
	m_labelRect.width() = min(
				m_textSize.x()+2*theme->getDefaultFontSize(),
				m_size.x()-2*theme->getDefaultFontSize());

	// and the label size
	m_label.setSize(m_labelRect.width() - 2*theme->getDefaultFontSize(), m_textSize.y());

	// border rect
	if ((m_paneStyle & LABEL_STYLE) != 0)
		m_borderRect.setSize(m_size - Vector2i(0, textPos.y()));
	else
		m_borderRect.setSize(m_size);

	if (getLayout())
        getLayout()->setSize(getClientSize());

	// update is needed
    setDirty();
}

Widget *Pane::findNextTabIndex(Widget *widget, Int32 direction)
{
    if (getLayout())
        return getLayout()->findNextTabIndex(widget, direction);

    return widget;
}

void Pane::mouseMoveIn()
{
	m_capacities.enable(STATE_HOVER);

	setDirty();

	// signal
	onEnterWidget();
}

void Pane::mouseMoveOut()
{
	m_capacities.disable(STATE_HOVER);

	setDirty();

	// signal
	onLeaveWidget();
}

void Pane::focused()
{
	// focused
	//setDirty();

	m_capacities.enable(STATE_FOCUSED);

	// signal
	onFocus();
}

void Pane::lostFocus()
{
	// focused
	//setDirty();

	m_capacities.disable(STATE_FOCUSED);

	// signal
	onLostFocus();
}

//---------------------------------------------------------------------------------------
// draw
//---------------------------------------------------------------------------------------
void Pane::draw()
{
	if (!isShown())
		return;

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

void Pane::updateCache()
{
	Theme *theme = getUsedTheme();

	// Draw borders
	if ((m_paneStyle & BORDER_STYLE) != 0)
	{
		theme->drawWidget(
					Theme::FRAME_NORMAL,
					m_borderRect.x(),
					m_borderRect.y(),
					m_borderRect.width(),
					m_borderRect.height());

		// and the label background
		if ((m_paneStyle & LABEL_STYLE) != 0)
		{
			theme->drawWidget(
						Theme::PANE_LABEL,
						m_labelRect.x(),
						m_labelRect.y(),
						m_labelRect.width(),
						m_labelRect.height());

			Theme *theme = getUsedTheme();

			m_label.write(
						theme->getDefaultFont(),
						theme->getDefaultFontSize());
		}
	}

    setClean();
}

