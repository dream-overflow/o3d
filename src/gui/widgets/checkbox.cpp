/**
 * @file checkbox.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/checkbox.h"
#include "o3d/gui/widgetdrawmode.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/gui/widgets/layout.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"
#include "o3d/engine/context.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(CheckBox, GUI_WIDGET_CHECKBOX, Widget)

/*---------------------------------------------------------------------------------------
  constructors
---------------------------------------------------------------------------------------*/
CheckBox::CheckBox(BaseObject *parent) :
	Widget(parent),
	m_pressed(False),
	m_wasPressed(False),
	m_pushed(False),
	m_hover(False),
	m_style(TWO_STATES_CHECKBOX),
	m_checkBoxState(NO_CHECKED)
{
}

CheckBox::CheckBox(
	Widget *parent,
	const String &label,
	const Vector2i &pos,
	const Vector2i &size,
	CheckBoxStyle style,
	CheckBoxState checked,
	const String &name) :
		Widget(parent, pos, size, name),
		m_pressed(False),
		m_wasPressed(False),
		m_pushed(False),
		m_hover(False),
		m_style(style),
		m_checkBoxState(checked)
{
	m_textZone.setText(label);
	init();
}

void CheckBox::init()
{
	m_textZone.setVerticalAlignment(TextZone::MIDDLE_ALIGN);

	Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::CHECK_BOX_NORMAL);

	// calc the default checkbox size
    Vector2i defaultSize = drawMode->getDefaultSize();

	Vector2i textSize = m_textZone.sizeOf(
		theme->getDefaultFont(),
		theme->getDefaultFontSize());

	// respect margins
	textSize += drawMode->getTopLeftMargin() + drawMode->getBottomRightMargin();

    if (m_minSize.x() < 0)
        m_minSize.x() = o3d::max(textSize.x(), defaultSize.y());

    if (m_minSize.y() < 0)
        m_minSize.y() = o3d::max(textSize.y(), defaultSize.y());

	m_size = m_minSize;
	m_textZone.setSize(m_size);

    m_textZone.setPos(drawMode->getTopLeftMargin().x(), -drawMode->getTopLeftMargin().y());

	// text color
	m_textZone.setColor(theme->getWidgetDrawMode(Theme::CHECK_BOX_NORMAL)->getDefaultTextColor());
}

/*---------------------------------------------------------------------------------------
  destructor
---------------------------------------------------------------------------------------*/
CheckBox::~CheckBox()
{
}

/*---------------------------------------------------------------------------------------
  get the recommended widget default size
---------------------------------------------------------------------------------------*/
Vector2i CheckBox::getDefaultSize()
{
	return getUsedTheme()->getDefaultWidgetSize(Theme::CHECK_BOX_NORMAL);
}

/*---------------------------------------------------------------------------------------
  Events Management
---------------------------------------------------------------------------------------*/
void CheckBox::sizeChanged()
{
	Theme *theme = getUsedTheme();
    const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::CHECK_BOX_NORMAL);

    if (m_size.x() >= 0)
        m_textZone.setWidth(m_size.x() - drawMode->getTopLeftMargin().x());
	else
		m_textZone.setWidth(-1);

    m_textZone.setHeight(m_size.y());
    m_textZone.setPos(drawMode->getTopLeftMargin().x(), -drawMode->getTopLeftMargin().y());

	setDirty();
}

/*---------------------------------------------------------------------------------------
  Mouse events management
---------------------------------------------------------------------------------------*/
Bool CheckBox::isTargeted(Int32 x,Int32 y,Widget *&pWidget)
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

/*---------------------------------------------------------------------------------------
  Mouse events
---------------------------------------------------------------------------------------*/
Bool CheckBox::mouseLeftPressed(Int32 x,Int32 y)
{
    getGui()->getWidgetManager()->lockWidgetMouse();

	if (m_hover)
	{
		m_pressed = True;
		setDirty();

		// signal
		onCheckBoxPressed();
	}

	return True;
}

Bool CheckBox::mouseLeftReleased(Int32 x,Int32 y)
{
    getGui()->getWidgetManager()->unlockWidgetMouse();

	if (m_pressed)
	{
		m_pressed = m_wasPressed = False;

		// two states
		if (m_style == TWO_STATES_CHECKBOX)
		{
			if (m_checkBoxState == CHECKED)
				m_checkBoxState = NO_CHECKED;
			else
				m_checkBoxState = CHECKED;

			// signals
			onCheckBoxReleased();
			onCheckBoxChanged(m_checkBoxState);
		}
		// three states
		else if (m_style == THREE_STATES_CHECKBOX)
		{
			if (m_checkBoxState == CHECKED)
				m_checkBoxState = THIRD_STATE;
			else if (m_checkBoxState == NO_CHECKED)
				m_checkBoxState = CHECKED;
			else
				m_checkBoxState = NO_CHECKED;

			// signals
			onCheckBoxReleased();
			onCheckBoxChanged(m_checkBoxState);
		}

		setDirty();
	}
	else
	{
		m_wasPressed = False;
	}

	return True;
}

Bool CheckBox::mouseMove(Int32 x,Int32 y)
{
    x -= m_pos.x();
    y -= m_pos.y();

	if (m_wasPressed)
	{
		// no negative part
		if (x < 0 || y < 0)
			return True;

        if ((x < m_size.x()) && (y < m_size.y()))
		{
			m_wasPressed = False;
			m_hover = m_pressed = True;
		}
	}
	else if (m_pressed)
	{
		// if move out when pressed
        if (x < 0 || y < 0 || x >= m_size.x() || y >= m_size.y())
		{
			m_hover = m_pressed = False;
			m_wasPressed = True;
		}
	}

	return True;
}

void CheckBox::mouseMoveIn()
{
	m_pressed = False;
	m_hover = True;
	setDirty();

	// signal
	onEnterWidget();
}

void CheckBox::mouseMoveOut()
{
	m_pressed = False;
	m_hover = False;
	setDirty();

	// signal
    onLeaveWidget();
}

Bool CheckBox::keyboardToggled(Keyboard *keyboard, KeyEvent event)
{
    if (event.key() == KEY_SPACE && event.isPressed())
    {
        m_pressed = True;

        setDirty();
        return True;
    }

    if (event.key() == KEY_SPACE && event.isReleased())
    {
        m_pressed = False;
        m_wasPressed = True;

        // two states
        if (m_style == TWO_STATES_CHECKBOX)
        {
            if (m_checkBoxState == CHECKED)
                m_checkBoxState = NO_CHECKED;
            else
                m_checkBoxState = CHECKED;

            // signals
            onCheckBoxReleased();
            onCheckBoxChanged(m_checkBoxState);
        }
        // three states
        else if (m_style == THREE_STATES_CHECKBOX)
        {
            if (m_checkBoxState == CHECKED)
                m_checkBoxState = THIRD_STATE;
            else if (m_checkBoxState == NO_CHECKED)
                m_checkBoxState = CHECKED;
            else
                m_checkBoxState = NO_CHECKED;

            // signals
            onCheckBoxReleased();
            onCheckBoxChanged(m_checkBoxState);
        }

        setDirty();
        return True;
    }

    return False;
}

void CheckBox::focused()
{
    setDirty();

	m_capacities.enable(STATE_FOCUSED);

	// signal
	onFocus();
}

void CheckBox::lostFocus()
{
    setDirty();

	m_capacities.disable(STATE_FOCUSED);

	// signal
	onLostFocus();
}

//---------------------------------------------------------------------------------------
// draw
//---------------------------------------------------------------------------------------
void CheckBox::draw()
{
	if (isShown())
        processCache();
}

void CheckBox::updateCache()
{
	Theme *theme = getUsedTheme();

    Int32 center = (m_size.y() - theme->getWidgetHeight(Theme::CHECK_BOX_NORMAL)) / 2;

    if (m_hover || m_pressed || isFocused())
        theme->drawWidget(Theme::CHECK_BOX_HOVER, 0, center, -1, -1);
	else
		theme->drawWidget(Theme::CHECK_BOX_NORMAL, 0, center, -1, -1);

	if (m_checkBoxState == CHECKED)
		theme->drawWidget(Theme::CHECK_BOX_TRUE, 0, center, -1, -1);
	else if (m_checkBoxState == THIRD_STATE)
		theme->drawWidget(Theme::CHECK_BOX_THIRD, 0, center, -1, -1);

    // when using cache, disable text blending because it will be processed when
    // the cache is rendered
    theme->getDefaultFont()->setBlendFunc(Blending::DISABLED);
	m_textZone.write(
		theme->getDefaultFont(),
		theme->getDefaultFontSize());
    theme->getDefaultFont()->setBlendFunc(Blending::ONE__ONE_MINUS_SRC_A___ONE__ONE_MINUS_SRC_A);
}

