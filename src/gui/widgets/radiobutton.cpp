/**
 * @file radiobutton.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/radiobutton.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"
#include "o3d/gui/widgetmanager.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(RadioButton, GUI_WIDGET_RADIOBUTTON, Widget)

// default constructor
RadioButton::RadioButton(BaseObject *parent) :
	Widget(parent),
	m_pressed(False),
	m_wasPressed(False),
	m_pushed(False),
	m_hover(False),
	m_radioButtonState(False),
    m_previousRadioButton(nullptr),
    m_nextRadioButton(nullptr)
{
}

RadioButton::RadioButton(
		Widget *parent,
		RadioButton *previous,
		const String &label,
		const Vector2i &pos,
		const Vector2i &size,
		const String &name) :
	Widget(parent, pos, size, name),
	m_pressed(False),
	m_wasPressed(False),
	m_pushed(False),
	m_hover(False),
	m_radioButtonState(False),
	m_previousRadioButton(previous),
    m_nextRadioButton(nullptr)
{
	// doubly chained list
	if (m_previousRadioButton)
		m_previousRadioButton->m_nextRadioButton = this;

	m_textZone.setText(label);
	init();
}

// Virtual destructor
RadioButton::~RadioButton()
{
	// doubly chained list
	if (m_previousRadioButton)
		m_previousRadioButton->m_nextRadioButton = m_nextRadioButton;

	if (m_nextRadioButton)
		m_nextRadioButton->m_previousRadioButton = m_previousRadioButton;
}

void RadioButton::init()
{
	m_textZone.setVerticalAlignment(TextZone::MIDDLE_ALIGN);

	Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::RADIO_BUTTON_NORMAL);

	// calc the default radio button size
    Vector2i defaultSize = drawMode->getDefaultSize();

	Vector2i textSize = m_textZone.sizeOf(
		theme->getDefaultFont(),
		theme->getDefaultFontSize());

	// respect margins
	textSize += drawMode->getTopLeftMargin() + drawMode->getBottomRightMargin();

    if (m_minSize.x() < 0) {
        m_minSize.x() = o3d::max(textSize.x(), defaultSize.y());
    }

    if (m_minSize.y() < 0) {
        m_minSize.y() = o3d::max(textSize.y(), defaultSize.y());
    }

	m_size = m_minSize;
	m_textZone.setSize(m_size);

    m_textZone.setPos(drawMode->getTopLeftMargin().x(), -drawMode->getTopLeftMargin().y());

	// text color
	m_textZone.setColor(theme->getWidgetDrawMode(Theme::RADIO_BUTTON_NORMAL)->getDefaultTextColor());
}

// get the recommended widget default size
Vector2i RadioButton::getDefaultSize()
{
	return getUsedTheme()->getDefaultWidgetSize(Theme::RADIO_BUTTON_NORMAL);
}

// Events Management
void RadioButton::sizeChanged()
{
    Theme *theme = getUsedTheme();
    const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::RADIO_BUTTON_NORMAL);

    if (m_size.x() >= 0) {
        m_textZone.setWidth(m_size.x() - drawMode->getTopLeftMargin().x());
    } else {
		m_textZone.setWidth(-1);
    }

    m_textZone.setHeight(m_size.y());
    m_textZone.setPos(drawMode->getTopLeftMargin().x(), -drawMode->getTopLeftMargin().y());

	setDirty();
}

// Check the radio button and uncheck the linked button if necessary.
void RadioButton::setChecked(Bool state)
{
	// check if not
    if (!m_radioButtonState) {
		// unckeck the previous radio button

		// search in previous radio buttons...
		RadioButton *button = m_previousRadioButton;
        while (button) {
            if (button->m_radioButtonState) {
				button->m_radioButtonState = False;
				button->onRadioButtonChanged(False);

				break;
			}

			button = button->m_previousRadioButton;
		}
		// not found in previous ? then iterate in next one's
        if (button == nullptr) {
			button = m_nextRadioButton;
            while (button) {
                if (button->m_radioButtonState) {
					button->m_radioButtonState = False;
					button->onRadioButtonChanged(False);

					break;
				}

				button = button->m_nextRadioButton;
			}
		}

		// finally change the state of the check radio button
		m_radioButtonState = True;
	}
}

// Mouse events management
Bool RadioButton::isTargeted(Int32 x, Int32 y, Widget *&widget)
{
	// no negative part
    if (x < 0 || y < 0) {
		return False;
    }

    if ((x < m_size.x()) && (y < m_size.y())) {
		widget = this;
		return True;
	}

	return False;
}

// Mouse events
Bool RadioButton::mouseLeftPressed(Int32 x, Int32 y)
{
    ((Gui*)getScene()->getGui())->getWidgetManager()->lockWidgetMouse();

    if (m_hover) {
		m_pressed = True;
		setDirty();

		// signal
		onRadioButtonPressed();
	}

	return True;
}

Bool RadioButton::mouseLeftReleased(Int32 x, Int32 y)
{
    ((Gui*)getScene()->getGui())->getWidgetManager()->unlockWidgetMouse();

    if (m_pressed) {
		m_pressed = m_wasPressed = False;

		// check if not
        if (!m_radioButtonState) {
			// unckeck the previous radio button

			// search in previous radio buttons...
			RadioButton *button = m_previousRadioButton;
            while (button) {
                if (button->m_radioButtonState) {
					button->m_radioButtonState = False;
                    button->setDirty();
					button->onRadioButtonChanged(False);

					break;
				}

				button = button->m_previousRadioButton;
			}
			// not found in previous ? then iterate in next one's
            if (button == nullptr) {
				button = m_nextRadioButton;
                while (button) {
                    if (button->m_radioButtonState) {
						button->m_radioButtonState = False;
                        button->setDirty();
						button->onRadioButtonChanged(False);

						break;
					}

					button = button->m_nextRadioButton;
				}
			}

			// finally change the state of the check radio button
			m_radioButtonState = True;

			// signals
			onRadioButtonReleased();
			onRadioButtonChanged(m_radioButtonState);

			setDirty();
		}
    } else {
		m_wasPressed = False;
	}

	return True;
}

Bool RadioButton::mouseMove(Int32 x, Int32 y)
{
    x -= m_pos.x();
    y -= m_pos.y();

    if (m_wasPressed) {
		// no negative part
        if (x < 0 || y < 0) {
			return False;
        }

        if ((x < m_size.x()) && (y < m_size.y())) {
			m_wasPressed = False;
			m_hover = m_pressed = True;

			setDirty();
		}
    } else if (m_pressed) {
		// if move out when pressed
        if (x < 0 || y < 0 || x >= m_size.x() || y >= m_size.y()) {
			m_hover = m_pressed = False;
			m_wasPressed = True;

			setDirty();
		}
	}

	return True;
}

void RadioButton::mouseMoveIn()
{
	m_pressed = False;
	m_hover = True;
	setDirty();

	// signal
	onEnterWidget();
}

void RadioButton::mouseMoveOut()
{
	m_pressed = False;
	m_hover = False;
	setDirty();

	// signal
    onLeaveWidget();
}

Bool RadioButton::keyboardToggled(Keyboard *keyboard, KeyEvent event)
{
    if (event.key() == KEY_SPACE && event.isPressed())
    {
        m_pressed = True;
        setDirty();

        // signal
        onRadioButtonPressed();

        return True;
    }

    if (event.key() == KEY_SPACE && event.isReleased())
    {
        m_pressed = m_wasPressed = False;

        // check if not
        if (!m_radioButtonState)
        {
            // unckeck the previous radio button

            // search in previous radio buttons...
            RadioButton *button = m_previousRadioButton;
            while (button)
            {
                if (button->m_radioButtonState)
                {
                    button->m_radioButtonState = False;
                    button->setDirty();
                    button->onRadioButtonChanged(False);

                    break;
                }

                button = button->m_previousRadioButton;
            }
            // not found in previous ? then iterate in next one's
            if (button == nullptr)
            {
                button = m_nextRadioButton;
                while (button)
                {
                    if (button->m_radioButtonState)
                    {
                        button->m_radioButtonState = False;
                        button->setDirty();
                        button->onRadioButtonChanged(False);

                        break;
                    }

                    button = button->m_nextRadioButton;
                }
            }

            // finally change the state of the check radio button
            m_radioButtonState = True;

            // signals
            onRadioButtonReleased();
            onRadioButtonChanged(m_radioButtonState);

            setDirty();
        }

        return True;
    }

    return False;
}

void RadioButton::focused()
{
    setDirty();

    m_capacities.enable(STATE_FOCUSED);

	// signal
	onFocus();
}

void RadioButton::lostFocus()
{
    setDirty();

    m_capacities.disable(STATE_FOCUSED);

	// signal
	onLostFocus();
}

// draw
void RadioButton::draw()
{
	if (isShown())
        processCache();
}

void RadioButton::updateCache()
{
	Theme *theme = getUsedTheme();

    Int32 center = (m_size.y() - theme->getWidgetHeight(Theme::RADIO_BUTTON_NORMAL)) / 2;

    if (m_hover || m_pressed || isFocused())
        theme->drawWidget(Theme::RADIO_BUTTON_HOVER, 0, center, -1, -1);
	else
		theme->drawWidget(Theme::RADIO_BUTTON_NORMAL, 0, center, -1, -1);

	if (m_radioButtonState)
		theme->drawWidget(Theme::RADIO_BUTTON_TRUE, 0, center, -1, -1);

    theme->getDefaultFont()->setBlendFunc(Blending::DISABLED);
	m_textZone.write(
		theme->getDefaultFont(),
        theme->getDefaultFontSize());
    theme->getDefaultFont()->setBlendFunc(Blending::ONE__ONE_MINUS_SRC_A___ONE__ONE_MINUS_SRC_A);
}

