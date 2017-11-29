/**
 * @file toolbar.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/toolbar.h"

#include "o3d/gui/widgets/layout.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"
#include "o3d/gui/widgetdrawcompass.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(ToolBar, GUI_WIDGET_TOOLBAR, Widget)

// constructors
ToolBar::ToolBar(BaseObject *parent) :
	Widget(parent),
	m_orientation(HORIZONTAL_ALIGN),
	m_space(0)
{
    m_capacities.enable(CAPS_CONTAINER);
}

ToolBar::ToolBar(Widget *parent,
		const Vector2i &buttonSize,
		UInt32 numButtons,
		Int32 space,
		Orientation orientation) :
	Widget(parent),
	m_orientation(orientation),
	m_toolButtonSize(buttonSize),
	m_space(space)
{
    m_capacities.enable(CAPS_CONTAINER);
	setNumButtons(numButtons);
}

ToolBar::~ToolBar()
{
	for (IT_ToolButtonVector it = m_buttons.begin(); it != m_buttons.end(); ++it)
		if (*it)
			deletePtr(*it);
}

// Change the toolbar orientation.
void ToolBar::setOrientation(Orientation orientation)
{
	if (orientation != m_orientation)
	{
		Vector2i pos;
		m_size = getUsedTheme()->getDefaultWidgetSize(Theme::TOOL_BAR);

		// recompute the position of each tool button.
		if (orientation == HORIZONTAL_ALIGN)
		{
			for (UInt32 i = 0; i < m_buttons.size(); ++i)
			{
				if (m_buttons[i])
				{
					pos = m_pos + getOrigin();
                    pos.x() += i * ((Int32)m_toolButtonSize.x() + m_space);

					m_buttons[i]->setPos(pos);
				}
			}

            m_size.x() += m_buttons.size() * (m_toolButtonSize.x() + m_space);
            m_size.y() += m_toolButtonSize.y();
		}
		else if (orientation == VERTICAL_ALIGN)
		{
			for (UInt32 i = 0; i < m_buttons.size(); ++i)
			{
				if (m_buttons[i])
				{
					pos = m_pos + getOrigin();
                    pos.y() += i * ((Int32)m_toolButtonSize.y() + m_space);

					m_buttons[i]->setPos(pos);
				}
			}

            m_size.x() += m_toolButtonSize.x();
            m_size.y() += m_buttons.size() * (m_toolButtonSize.y() + m_space);
		}

		m_minSize = m_maxSize = m_size;
		m_orientation = orientation;

		setDirty();
	}
}

// properties
void ToolBar::setButton(UInt32 index, ToolButton *button)
{
	if (!button)
		return;

	if (button->size() != m_toolButtonSize)
		O3D_ERROR(E_InvalidParameter("The tool button is not of the excepted size"));

	m_buttons.at(index) = button;

	// Position the button
	Vector2i pos = getOrigin();
	if (m_orientation == HORIZONTAL_ALIGN)
        pos.x() += index * ((Int32)m_toolButtonSize.x() + m_space);
	else if (m_orientation == VERTICAL_ALIGN)
        pos.y() += index * ((Int32)m_toolButtonSize.y() + m_space);

	button->setPos(pos);
}

ToolButton* ToolBar::getButton(UInt32 index)
{
	return m_buttons.at(index);
}

// Set the number of buttons slots.
void ToolBar::setNumButtons(UInt32 numButtons)
{
	if (numButtons < m_buttons.size())
	{
		for (UInt32 i = numButtons; i < m_buttons.size(); ++i)
			deletePtr(m_buttons[i]);

		m_buttons.resize(numButtons);

	}
	else if (numButtons > m_buttons.size())
	{
		UInt32 oldSize = m_buttons.size();
		m_buttons.resize(numButtons);

		for (UInt32 i = oldSize; i < m_buttons.size(); ++i)
            m_buttons[i] = nullptr;
	}
	else
		return;

	m_size = getUsedTheme()->getDefaultWidgetSize(Theme::TOOL_BAR);

	if (m_orientation == HORIZONTAL_ALIGN)
	{
        m_size.x() += numButtons * (m_toolButtonSize.x() + m_space);
        m_size.y() += m_toolButtonSize.y();
	}
	else if (m_orientation == VERTICAL_ALIGN)
	{
        m_size.x() += m_toolButtonSize.x();
        m_size.y() += numButtons * (m_toolButtonSize.y() + m_space);
	}

	m_minSize = m_maxSize = m_size;

	setDirty();
}

// GetDefaultSize
Vector2i ToolBar::getDefaultSize()
{
	return getUsedTheme()->getDefaultWidgetSize(Theme::TOOL_BAR);
}

Vector2i ToolBar::getOrigin() const
{
	const Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::TOOL_BAR);

	return drawMode->getTopLeftMargin();
}

// Mouse events management
Bool ToolBar::isTargeted(Int32 x, Int32 y, Widget *&widget)
{
	// no negative part
	if (x < 0 || y < 0)
		return False;

    if ((x < m_size.x()) && (y < m_size.y()))
	{
		Vector2i org = getOrigin();
		widget = this;

		// delta by the client origin for children widgets
        x -= org.x();
        y -= org.y();

		// for each tool button
		for (IT_ToolButtonVector it = m_buttons.begin(); it != m_buttons.end(); ++it)
		{
			if ((*it) && (*it)->isTargeted(
                x - (*it)->pos().x(),
                y - (*it)->pos().y(),
				widget))
			{
				return True;
			}
		}

		return True;
	}

    return False;
}

Widget *ToolBar::findNextTabIndex(Widget *widget, Int32 direction)
{
    // TODO
    return widget;
}

// Draw
void ToolBar::draw()
{
	if (!isShown())
		return;

	processCache();

	getScene()->getContext()->modelView().push();
	getScene()->getContext()->modelView().translate(Vector3(
            (Float)m_pos.x(),
            (Float)m_pos.y(), 0.f));

	for (IT_ToolButtonVector it = m_buttons.begin(); it != m_buttons.end(); ++it)
		if (*it)
			(*it)->draw();

	getScene()->getContext()->modelView().pop();
}

void ToolBar::updateCache()
{
	Theme *theme = getUsedTheme();
    theme->drawWidget(Theme::TOOL_BAR, 0, 0, m_size.x(), m_size.y());
}
