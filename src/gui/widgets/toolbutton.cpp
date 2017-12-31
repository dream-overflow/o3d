/**
 * @file toolbutton.cpp
 * @brief A widget representing a simple button
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-02-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/toolbutton.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/gui/widgets/layout.h"
#include "o3d/core/templatemanager.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(ToolButton, GUI_WIDGET_TOOLBUTTON, Widget)

// default constructor
ToolButton::ToolButton(BaseObject *parent) :
	Widget(parent),
	m_pressed(False),
	m_waspressed(False),
	m_pushed(False),
	m_hover(False),
	m_WidgetDrawFalse((Theme::WidgetDraw)0),
	m_WidgetDrawTrue((Theme::WidgetDraw)0),
	m_WidgetDrawHover((Theme::WidgetDraw)0),
    m_WidgetDrawDisabled((Theme::WidgetDraw)0),
	m_ButtonStyle(PUSH_BUTTON)
{
}

ToolButton::ToolButton(
		Widget *parent,
		ToolButtonStyle style) :
	Widget(parent),
	m_pressed(False),
	m_waspressed(False),
	m_pushed(False),
	m_hover(False),
	m_WidgetDrawFalse((Theme::WidgetDraw)0),
	m_WidgetDrawTrue((Theme::WidgetDraw)0),
	m_WidgetDrawHover((Theme::WidgetDraw)0),
    m_WidgetDrawDisabled((Theme::WidgetDraw)0),
	m_ButtonStyle(style)
{
}

ToolButton::ToolButton(
		Widget *parent,
		Icon *iconFalse,
		Icon *iconTrue,
		Icon *iconHover,
		Icon *iconDisable,
		ToolButtonStyle style) :
	Widget(parent),
	m_pressed(False),
	m_waspressed(False),
	m_pushed(False),
	m_hover(False),
	m_WidgetDrawFalse((Theme::WidgetDraw)0),
	m_WidgetDrawTrue((Theme::WidgetDraw)0),
	m_WidgetDrawHover((Theme::WidgetDraw)0),
    m_WidgetDrawDisabled((Theme::WidgetDraw)0),
	m_ButtonStyle(style)
{
    O3D_ASSERT(iconFalse != nullptr);

	if (iconFalse)
		setIconFalse(iconFalse);
	if (iconTrue)
		setIconTrue(iconTrue);
	if (iconHover)
		setIconHover(iconHover);
	if (iconDisable)
		setIconDisable(iconDisable);
}

ToolButton::ToolButton(Widget *parent,
        Theme::WidgetDraw widgetFalse,
        Theme::WidgetDraw widgetTrue,
        Theme::WidgetDraw widgetHover,
        Theme::WidgetDraw widgetDisabled,
        ToolButtonStyle style) :
	Widget(parent),
	m_pressed(False),
	m_waspressed(False),
	m_pushed(False),
	m_hover(False),
	m_WidgetDrawFalse(widgetFalse),
	m_WidgetDrawTrue(widgetTrue),
	m_WidgetDrawHover(widgetHover),
    m_WidgetDrawDisabled(widgetDisabled),
	m_ButtonStyle(style)
{
    setToolButtonThemeStyle(widgetTrue, widgetFalse, widgetHover, widgetDisabled);
}

// destructor
ToolButton::~ToolButton()
{
}

// get the recommended widget default size
Vector2i ToolButton::getDefaultSize()
{
	if (m_iconFalse.get())
		return m_iconFalse->size();
	else
		return Vector2i(16, 16);
}

// set the theme button style (@see SetIcon)
void ToolButton::setToolButtonThemeStyle(Theme::WidgetDraw widgetTrue,
    Theme::WidgetDraw widgetFalse,
    Theme::WidgetDraw widgetHover,
    Theme::WidgetDraw widgetDisabled)
{
    m_WidgetDrawTrue = Theme::WIDGET_NONE;
    m_WidgetDrawFalse = Theme::WIDGET_NONE;
    m_WidgetDrawHover = Theme::WIDGET_NONE;
    m_WidgetDrawDisabled = Theme::WIDGET_NONE;

	Theme *theme = getUsedTheme();

	// check for validity
    if ((widgetTrue != Theme::WIDGET_NONE) &&
        (theme->getWidgetDrawMode(widgetTrue)->getType() != WidgetDrawMode::SIMPLE_MODE))
        O3D_ERROR(E_InvalidParameter("Invalid widget draw type. Must be a simple mode only."));

    if ((widgetFalse != Theme::WIDGET_NONE) &&
        (theme->getWidgetDrawMode(widgetFalse)->getType() != WidgetDrawMode::SIMPLE_MODE))
        O3D_ERROR(E_InvalidParameter("Invalid widget draw type. Must be a simple mode only."));

    if ((widgetHover != Theme::WIDGET_NONE) &&
        (theme->getWidgetDrawMode(widgetHover)->getType() != WidgetDrawMode::SIMPLE_MODE))
        O3D_ERROR(E_InvalidParameter("Invalid widget draw type. Must be a simple mode only."));

    if ((widgetDisabled != Theme::WIDGET_NONE) &&
        (theme->getWidgetDrawMode(widgetDisabled)->getType() != WidgetDrawMode::SIMPLE_MODE))
        O3D_ERROR(E_InvalidParameter("Invalid widget draw type. Must be a simple mode only."));

	m_WidgetDrawTrue = widgetTrue;
	m_WidgetDrawFalse = widgetFalse;
	m_WidgetDrawHover = widgetHover;
    m_WidgetDrawDisabled = widgetDisabled;

    if (m_WidgetDrawTrue != Theme::WIDGET_NONE)
	{
        m_minSize.x() = theme->getWidgetWidth(m_WidgetDrawTrue);
        m_minSize.y() = theme->getWidgetHeight(m_WidgetDrawTrue);
	}
    else if (m_WidgetDrawFalse != Theme::WIDGET_NONE)
	{
        m_minSize.x() = theme->getWidgetWidth(m_WidgetDrawFalse);
        m_minSize.y() = theme->getWidgetHeight(m_WidgetDrawFalse);
	}
    else if (m_WidgetDrawHover != Theme::WIDGET_NONE)
	{
        m_minSize.x() = theme->getWidgetWidth(m_WidgetDrawHover);
        m_minSize.y() = theme->getWidgetHeight(m_WidgetDrawHover);
	}
    else if (m_WidgetDrawDisabled != Theme::WIDGET_NONE)
    {
        m_minSize.x() = theme->getWidgetWidth(m_WidgetDrawDisabled);
        m_minSize.y() = theme->getWidgetHeight(m_WidgetDrawDisabled);
    }

    m_size = m_minSize;
}

void ToolButton::setToolTip(Widget *toolTip)
{
    m_toolTip = toolTip;
}

Widget *ToolButton::getToolTip()
{
    return m_toolTip;
}

void ToolButton::setToolTip(const String &label)
{
    Widget::setToolTip(label);
}

// set the button icon
void ToolButton::setIconFalse(Icon *icon)
{
	m_iconFalse = icon;

	if (icon != nullptr)
		m_size = m_minSize = m_maxSize = icon->size();
}

// set the button icon
void ToolButton::setIconTrue(Icon *icon)
{
	m_iconTrue = icon;

	if (icon != nullptr)
		m_size = m_minSize = m_maxSize = icon->size();
}

// set the button icon
void ToolButton::setIconHover(Icon *icon)
{
	m_iconHover = icon;

	if (icon != nullptr)
		m_size = m_minSize = m_maxSize = icon->size();
}

// set the button icon
void ToolButton::setIconDisable(Icon *icon)
{
	m_iconDisable= icon;

	if (icon != nullptr)
		m_size = m_minSize = m_maxSize = icon->size();
}

// Mouse events management
Bool ToolButton::isTargeted(Int32 x, Int32 y, Widget *&widget)
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

// Mouse events
Bool ToolButton::mouseLeftPressed(Int32 x, Int32 y)
{
    ((Gui*)getScene()->getGui())->getWidgetManager()->lockWidgetMouse();

    if (!isActive()) {
        return True;
    }

    if (m_hover) {
		m_pressed = True;
		setDirty();

		// signal
		onToolButtonPressed();

		return True;
	}

	return False;
}

Bool ToolButton::mouseLeftReleased(Int32 x, Int32 y)
{
    ((Gui*)getScene()->getGui())->getWidgetManager()->unlockWidgetMouse();

    if (!isActive()) {
        return True;
    }

    if (m_pressed) {
		m_pressed = m_waspressed = False;

        if (m_ButtonStyle == TWO_STATES_BUTTON) {
			m_pushed = !m_pushed;

			// signals
			onToolButtonReleased();
			onToolButtonStateChanged(m_pushed);
        } else if (m_ButtonStyle == PUSH_BUTTON) {
			// signals
			onToolButtonReleased();
			onToolButtonPushed();
		}

		setDirty();
    } else {
		m_waspressed = False;
	}

	return True;
}

Bool ToolButton::mouseMove(Int32 x, Int32 y)
{
    x -= m_pos.x();
    y -= m_pos.y();

    if (!isActive()) {
        return True;
    }

    if (m_waspressed) {
		// no negative part
        if (x < 0 || y < 0) {
			return True;
        }

        if ((x < m_size.x()) && (y < m_size.y())) {
			m_waspressed = False;
			m_hover = m_pressed = True;
		}

		return True;
    } else if (m_pressed) {
		// if move out when pressed
        if (x < 0 || y < 0 || x >= m_size.x() || y >= m_size.y())
		{
			m_hover = m_pressed = False;
			m_waspressed = True;
		}

		return True;
	}

	return True;
}

void ToolButton::mouseMoveIn()
{
	m_pressed = False;
	m_hover = True;
	setDirty();

	// signal
	onEnterWidget();
}

void ToolButton::mouseMoveOut()
{
	m_pressed = False;
	m_hover = False;
	setDirty();

	// signal
	onLeaveWidget();
}

void ToolButton::focused()
{
	// TODO focused button
	//setDirty();

	// signal
	onFocus();
}

void ToolButton::lostFocus()
{
	// TODO focused button
	//setDirty();

	// signal
	onLostFocus();
}

// draw
void ToolButton::draw()
{
	// The cache is never used to draw directly a such object
	if (isShown())
		updateCache();
}

void ToolButton::updateCache()
{
	Theme *theme = getUsedTheme();

	// Draw Button
	if (m_iconFalse.isValid())
	{
        if (!isActive() && m_iconDisable.get())
		{
			m_iconDisable->setPos(m_pos);
			m_iconDisable->draw();
		}
        else if ((m_pushed || m_pressed) && m_iconTrue.isValid())
		{
			m_iconTrue->setPos(m_pos);
			m_iconTrue->draw();
		}
		else if (m_hover && m_iconHover.isValid())
		{
			m_iconHover->setPos(m_pos);
			m_iconHover->draw();
		}
        else
		{
			m_iconFalse->setPos(m_pos);
			m_iconFalse->draw();
		}
	}
	else
	{
        if (!isActive() && m_WidgetDrawDisabled != Theme::WIDGET_NONE)
            theme->drawWidget(m_WidgetDrawDisabled, m_pos.x(), m_pos.y(), -1, -1);
        else if ((m_pushed || m_pressed) && (m_WidgetDrawTrue != Theme::WIDGET_NONE))
            theme->drawWidget(m_WidgetDrawTrue, m_pos.x(), m_pos.y(), -1, -1);
        else if ((m_hover) && (m_WidgetDrawHover != Theme::WIDGET_NONE))
            theme->drawWidget(m_WidgetDrawHover, m_pos.x(), m_pos.y(), -1, -1);
        else if (m_WidgetDrawFalse != Theme::WIDGET_NONE)
            theme->drawWidget(m_WidgetDrawFalse, m_pos.x(), m_pos.y(), -1, -1);
	}
}

