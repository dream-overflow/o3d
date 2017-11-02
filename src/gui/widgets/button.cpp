/**
 * @file button.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/button.h"
#include "o3d/gui/widgetdrawmode.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/gui/widgets/layout.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"
#include "o3d/gui/widgets/statictext.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Button, GUI_WIDGET_BUTTON, Widget)

/*---------------------------------------------------------------------------------------
  constructors
---------------------------------------------------------------------------------------*/
Button::Button(BaseObject *parent) :
	Widget(parent),
	m_buttonStyle(PUSH_BUTTON),
    m_icon(nullptr)
{
}

Button::Button(
	Widget *parent,
	const String &label,
	const Vector2i &pos,
	const Vector2i &size,
	ButtonStyle style,
	const String &name) :
		Widget(parent, pos, size, name),
		m_buttonStyle(style),
        m_icon(nullptr)
{
	m_textZone.setText(label);
	init();
}

void Button::init()
{
	Theme *theme = getUsedTheme();
	//const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::BUTTON_FALSE);

	// because we center the text we dont have a relative position
	//m_textZone.setPos(drawMode->getTopLeftMargin());
	m_textZone.setHorizontalAlignment(TextZone::CENTER_ALIGN);
	m_textZone.setVerticalAlignment(TextZone::MIDDLE_ALIGN);

	adjustBestSize();

	// text color
	m_textZone.setColor(theme->getWidgetDrawMode(Theme::BUTTON_TRUE)->getDefaultTextColor());
}

void Button::adjustBestSize()
{
	// calc the default button size
	Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::BUTTON_FALSE);

	Vector2i defaultSize = getDefaultSize();

	Vector2i textSize = m_textZone.sizeOf(
		theme->getDefaultFont(),
		theme->getDefaultFontSize());

	textSize += drawMode->getTopLeftMargin() + drawMode->getBottomRightMargin();

    if (m_minSize.x() < 0) {
        m_minSize.x() = o3d::max(textSize.x(), defaultSize.x());
    }

    if (m_minSize.y() < 0) {
        m_minSize.y() = o3d::max(textSize.y(), defaultSize.y());
    }

	m_size = m_minSize;
	m_textZone.setSize(m_size);
}

Button::~Button()
{
	deletePtr(m_icon);
}

Vector2i Button::fit()
{
	adjustBestSize();

	return m_size;
}

Vector2i Button::getDefaultSize()
{
	return getUsedTheme()->getDefaultWidgetSize(Theme::BUTTON_FALSE);
}

void Button::setText(const String &text)
{
	m_textZone.setText(text);

    adjustBestSize();

	// update is needed
	setDirty();
}

const String& Button::getText() const
{
	return m_textZone.text();
}

void Button::setIcon(const Icon &icon)
{
	// TODO
}

void Button::enable(Bool active)
{
	if (m_capacities.getBit(STATE_ACTIVITY) == active)
		return;

	m_capacities.setBit(STATE_ACTIVITY, active);

	const Theme *theme = getUsedTheme();

	// text color depend of the state
	if (active)
		m_textZone.setColor(theme->getWidgetDrawMode(Theme::BUTTON_TRUE)->getDefaultTextColor());
	else
		m_textZone.setColor(theme->getWidgetDrawMode(Theme::BUTTON_DISABLED)->getDefaultTextColor());
}

Bool Button::isTargeted(Int32 x, Int32 y, Widget *&widget)
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

void Button::sizeChanged()
{
	m_textZone.setSize(m_size);

	// update is needed
	setDirty();
}

// Mouse events
Bool Button::mouseLeftPressed(Int32 x,Int32 y)
{
    ((Gui*)getScene()->getGui())->getWidgetManager()->lockWidgetMouse();

	if (m_capacities.getBit(STATE_HOVER))
	{
		m_capacities.enable(STATE_PRESSED);
		setDirty();

		// signal
		onButtonPressed();
	}

	return True;
}

Bool Button::mouseLeftReleased(Int32 x,Int32 y)
{
    ((Gui*)getScene()->getGui())->getWidgetManager()->unlockWidgetMouse();

	if (m_capacities.getBit(STATE_PRESSED))
	{
		m_capacities.disable(STATE_PRESSED);
		m_capacities.disable(STATE_WAS_PRESSED);

		if (m_buttonStyle == TWO_STATES_BUTTON)
		{
			m_capacities.toggle(STATE_PUSHED);

			// signals
			onButtonReleased();
			onButtonStateChanged(m_capacities.getBit(STATE_PUSHED));
		}
		else if (m_buttonStyle == PUSH_BUTTON)
		{
			// signals
			onButtonReleased();
			onButtonPushed();
		}

		setDirty();
	}
	else
	{
		m_capacities.disable(STATE_WAS_PRESSED);
	}

	return True;
}

Bool Button::mouseMove(Int32 x,Int32 y)
{
    x -= m_pos.x();
    y -= m_pos.y();

	if (m_capacities.getBit(STATE_WAS_PRESSED))
	{
		// no negative part
		if (x < 0 || y < 0)
			return True;

        if ((x < m_size.x()) && (y < m_size.y()))
		{
			m_capacities.disable(STATE_WAS_PRESSED);

			m_capacities.enable(STATE_PRESSED);
			m_capacities.enable(STATE_HOVER);

			setDirty();
		}
	}
	else if (m_capacities.getBit(STATE_PRESSED))
	{
		// if move out when pressed
        if (x < 0 || y < 0 || x >= m_size.x() || y >= m_size.y())
		{
			m_capacities.disable(STATE_PRESSED);
			m_capacities.disable(STATE_HOVER);

			m_capacities.enable(STATE_WAS_PRESSED);

			setDirty();
		}
	}

	return True;
}

void Button::mouseMoveIn()
{
	m_capacities.disable(STATE_PRESSED);
	m_capacities.enable(STATE_HOVER);

	setDirty();

	// signal
	onEnterWidget();
}

void Button::mouseMoveOut()
{
	m_capacities.disable(STATE_PRESSED);
	m_capacities.disable(STATE_HOVER);

	setDirty();

	// signal
	onLeaveWidget();
}

Bool Button::keyboardToggled(Keyboard *keyboard, KeyEvent event)
{
	if (!m_capacities.getBit(STATE_FOCUSED))
		return False;

	// press the button
	if (event.isPressed() && !m_capacities.getBit(STATE_PRESSED) &&
		((event.key() == KEY_SPACE) || (event.key() == KEY_RETURN) || (event.key() == KEY_NUMPAD_ENTER)))
	{
		m_capacities.enable(STATE_PRESSED);
		m_capacities.disable(STATE_WAS_PRESSED);

		setDirty();

		// signal
		onButtonPressed();

		return True;
	}

	// cancel pressed
	if (event.isPressed() && m_capacities.getBit(STATE_PRESSED) && (event.key() == KEY_ESCAPE))
	{
		m_capacities.disable(STATE_PRESSED);
		m_capacities.disable(STATE_WAS_PRESSED);

		setDirty();

		return True;
	}

	// release after pressed
	if (event.isReleased() && m_capacities.getBit(STATE_PRESSED) &&
		((event.key() == KEY_SPACE) || (event.key() == KEY_RETURN) || (event.key() == KEY_NUMPAD_ENTER)))
	{
		m_capacities.disable(STATE_PRESSED);
		m_capacities.disable(STATE_WAS_PRESSED);

		setDirty();

		if (m_buttonStyle == TWO_STATES_BUTTON)
		{
			m_capacities.toggle(STATE_PUSHED);

			// signals
			onButtonReleased();
			onButtonStateChanged(m_capacities.getBit(STATE_PUSHED));
		}
		else if (m_buttonStyle == PUSH_BUTTON)
		{
			// signals
			onButtonReleased();
			onButtonPushed();
		}

		return True;
	}

	return False;
}

void Button::focused()
{
	// focused button
    setDirty();

	m_capacities.enable(STATE_FOCUSED);

	// signal
	onFocus();
}

void Button::lostFocus()
{
	// focused button
    setDirty();

	m_capacities.disable(STATE_FOCUSED);

	// signal
	onLostFocus();
}

//---------------------------------------------------------------------------------------
// draw
//---------------------------------------------------------------------------------------
void Button::draw()
{
	if (isShown())
		processCache();
}

void Button::updateCache()
{
	Theme *theme = getUsedTheme();

	// Draw Button
	if (!isActive())
        theme->drawWidget(Theme::BUTTON_DISABLED, 0, 0, m_size.x(), m_size.y());
	else if (m_capacities.getBit(STATE_PUSHED) || m_capacities.getBit(STATE_PRESSED))
        theme->drawWidget(Theme::BUTTON_TRUE, 0, 0, m_size.x(), m_size.y());
    else if (m_capacities.getBit(STATE_HOVER) || isFocused())
        theme->drawWidget(Theme::BUTTON_HOVER, 0, 0, m_size.x(), m_size.y());
	else
        theme->drawWidget(Theme::BUTTON_FALSE, 0, 0, m_size.x(), m_size.y());

    theme->getDefaultFont()->setBlendFunc(Blending::DISABLED);
    m_textZone.write(
		theme->getDefaultFont(),
		theme->getDefaultFontSize());
    theme->getDefaultFont()->setBlendFunc(Blending::ONE__ONE_MINUS_SRC_A___ONE__ONE_MINUS_SRC_A);
}

