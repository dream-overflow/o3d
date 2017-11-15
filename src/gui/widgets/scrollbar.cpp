/**
 * @file scrollbar.cpp
 * @brief Implementation of Scrollbar.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2009-11-16
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/scrollbar.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/gui/widgets/layout.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"
#include "o3d/gui/widgetdrawcompass.h"
#include "o3d/engine/context.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(ScrollBar, GUI_WIDGET_SCROLLBAR, Widget)

// default constructor
ScrollBar::ScrollBar(BaseObject *parent) :
	Widget(parent),
	m_orientation(HORIZONTAL_ALIGN),
	m_hover(False),
	m_pushed(False),
	m_part(SCROLLBAR_PART_NONE),
	m_minimum(0),
	m_maximum(0),
	m_singleStep(0),
	m_pageStep(0),
	m_sliderPos(0),
	m_tracking(True),
	m_oldSliderPos(0),
	m_scrollThreshold(1),
	m_beforeDelay(500),
	m_repeatDelay(100),
	m_lastTime(0),
	m_repeat(False)
{
}

// Construct given a parent widget.
ScrollBar::ScrollBar(
		Widget *parent,
		Orientation orientation,
		const Vector2i &pos,
		const Vector2i &size,
		const String &name) :
			Widget(parent, pos, size, name),
			m_orientation(orientation),
			m_hover(False),
			m_pushed(False),
			m_part(SCROLLBAR_PART_NONE),
			m_minimum(0),
			m_maximum(0),
			m_singleStep(0),
			m_pageStep(0),
			m_sliderPos(0),
			m_tracking(True),
			m_oldSliderPos(0),
			m_scrollThreshold(1),
			m_beforeDelay(500),
			m_repeatDelay(100),
			m_lastTime(0),
			m_repeat(False)
{
	init();
}

// Virtual destructor.
ScrollBar::~ScrollBar()
{
}

void ScrollBar::init()
{
	Theme *theme = getUsedTheme();

	Vector2i defaultSize = getDefaultSize();

	const WidgetDrawCompass *border = (const WidgetDrawCompass*)theme->getWidgetDrawMode(Theme::SCROLL_BAR_BORDER);

	Int32 borderUp = border->getEltWidth(WidgetDrawCompass::NORTH);
	Int32 borderDown = border->getEltWidth(WidgetDrawCompass::SOUTH);
	Int32 borderLeft = border->getEltWidth(WidgetDrawCompass::WEST);
	Int32 borderRight = border->getEltWidth(WidgetDrawCompass::EAST);

	if (m_orientation == HORIZONTAL_ALIGN)
	{
        m_minSize.y() = m_size.y() = m_maxSize.y() = defaultSize.y();

		Int32 scrollHeight = border->getDefaultSize().y() - borderUp - borderDown;

		// compute buttons position
		Int32 buttonLeftW = theme->getWidgetWidth(Theme::SCROLL_BAR_BUTTON_LEFT_FALSE);
		Int32 buttonRightW = theme->getWidgetWidth(Theme::SCROLL_BAR_BUTTON_RIGHT_FALSE);

		m_buttonUp.set(borderLeft, borderUp, buttonLeftW, scrollHeight);
		m_buttonDown.set(m_size.x() - borderRight - buttonRightW, borderUp, buttonRightW, scrollHeight);
	}
	else if (m_orientation == VERTICAL_ALIGN)
	{
        m_minSize.x() = m_size.x() = m_maxSize.x() = defaultSize.x();

		Int32 scrollWidth = border->getDefaultSize().x() - borderLeft - borderRight;

		// compute buttons position
		Int32 buttonUpH = theme->getWidgetHeight(Theme::SCROLL_BAR_BUTTON_UP_FALSE);
		Int32 buttonDownH = theme->getWidgetHeight(Theme::SCROLL_BAR_BUTTON_DOWN_FALSE);

		m_buttonUp.set(borderLeft, borderUp, scrollWidth, buttonUpH);
		m_buttonDown.set(borderLeft, m_size.y() - borderDown - buttonDownH, scrollWidth, buttonDownH);
	}

	computeSliderSize();
}

void ScrollBar::updatePos()
{
	Theme *theme = getUsedTheme();
	const WidgetDrawCompass *border = (const WidgetDrawCompass*)theme->getWidgetDrawMode(Theme::SCROLL_BAR_BORDER);

	Int32 borderUp = border->getEltWidth(WidgetDrawCompass::NORTH);
	Int32 borderDown = border->getEltWidth(WidgetDrawCompass::SOUTH);
	Int32 borderLeft = border->getEltWidth(WidgetDrawCompass::WEST);
	Int32 borderRight = border->getEltWidth(WidgetDrawCompass::EAST);

	if (m_orientation == HORIZONTAL_ALIGN)
	{
		Int32 scrollHeight = border->getDefaultSize().y() - borderUp - borderDown;

		// compute buttons position
		Int32 buttonLeftW = theme->getWidgetWidth(Theme::SCROLL_BAR_BUTTON_LEFT_FALSE);
		Int32 buttonRightW = theme->getWidgetWidth(Theme::SCROLL_BAR_BUTTON_RIGHT_FALSE);

		m_buttonUp.set(borderLeft, borderUp, buttonLeftW, scrollHeight);
		m_buttonDown.set(m_size.x() - borderRight - buttonRightW, borderUp, buttonRightW, scrollHeight);
	}
	else if (m_orientation == VERTICAL_ALIGN)
	{
		Int32 scrollWidth = border->getDefaultSize().x() - borderLeft - borderRight;

		// compute buttons position
		Int32 buttonUpH = theme->getWidgetHeight(Theme::SCROLL_BAR_BUTTON_UP_FALSE);
		Int32 buttonDownH = theme->getWidgetHeight(Theme::SCROLL_BAR_BUTTON_DOWN_FALSE);

		m_buttonUp.set(borderLeft, borderUp, scrollWidth, buttonUpH);
		m_buttonDown.set(borderLeft, m_size.y() - borderDown - buttonDownH, scrollWidth, buttonDownH);
	}
}

void ScrollBar::sizeChanged()
{
	computeSliderSize();
	updatePos();
	setDirty();
}

void ScrollBar::computeSliderSize()
{
	Theme *theme = getUsedTheme();

	//Vector2i defaultSize = getDefaultSize();

	const WidgetDrawCompass *border = (const WidgetDrawCompass*)theme->getWidgetDrawMode(Theme::SCROLL_BAR_BORDER);

	Int32 borderUp = border->getEltHeight(WidgetDrawCompass::NORTH);
	Int32 borderDown = border->getEltHeight(WidgetDrawCompass::SOUTH);
	Int32 borderLeft = border->getEltWidth(WidgetDrawCompass::WEST);
	Int32 borderRight = border->getEltWidth(WidgetDrawCompass::EAST);

	if (m_orientation == HORIZONTAL_ALIGN)
	{
		Int32 buttonLeftWidth = theme->getWidgetWidth(Theme::SCROLL_BAR_BUTTON_LEFT_FALSE);
		Int32 buttonRightWidth = theme->getWidgetWidth(Theme::SCROLL_BAR_BUTTON_RIGHT_FALSE);

		Int32 scrollHeight = border->getDefaultSize().x() - borderUp - borderDown;
		Int32 minSliderWidth = theme->getWidgetWidth(Theme::SCROLL_BAR_SLIDER_FALSE);

		m_sliderZoneMin = buttonLeftWidth + borderLeft;
		m_sliderZoneMax = m_size.x() - buttonRightWidth - borderRight;

		// slider take the full space
		if (m_minimum == m_maximum)
		{
			m_scrollThreshold = 0;
			m_slider.set(m_sliderZoneMin, borderUp, m_sliderZoneMax - m_sliderZoneMin, scrollHeight);

			m_emptyUp.set(m_sliderZoneMin, borderUp, 0, scrollHeight);
			m_emptyDown.set(m_sliderZoneMax, borderUp, 0, scrollHeight);
		}
		// slider size is related to page step
		else
		{
			Int32 sliderRange = m_sliderZoneMax - m_sliderZoneMin + 1; // 100% pixel
			Int32 valueRange = m_maximum - m_minimum;     // virtual document range

			// current slider width in pixel
			Int32 sliderWidth = Int32(Float(sliderRange) / (Float(valueRange + m_pageStep) / m_pageStep));
			sliderWidth = o3d::max(sliderWidth, minSliderWidth); // minimal slider size

			// virtual document size
			sliderRange -= sliderWidth;
			Float ratio = Float(sliderRange) / valueRange;   // x pixels per range

			// minimal slider move
			m_scrollThreshold = o3d::max(1, Int32(ratio * m_singleStep));

			// current slider position in pixel
			Int32 sliderPos = Int32(ratio * m_sliderPos);

			m_slider.set(m_sliderZoneMin + sliderPos, borderUp, sliderWidth, scrollHeight);

			updateSliderPos(0);
		}
	}
	else if (m_orientation == VERTICAL_ALIGN)
	{
		Int32 buttonUpHeight = theme->getWidgetHeight(Theme::SCROLL_BAR_BUTTON_UP_FALSE);
		Int32 buttonDownHeight = theme->getWidgetHeight(Theme::SCROLL_BAR_BUTTON_DOWN_FALSE);

		Int32 scrollWidth = border->getDefaultSize().x() - borderLeft - borderRight;
		Int32 minSliderHeight = theme->getWidgetHeight(Theme::SCROLL_BAR_SLIDER_FALSE);

		m_sliderZoneMin = buttonUpHeight + borderUp;
		m_sliderZoneMax = m_size.y() - buttonDownHeight - borderDown;

		// slider take the full space
		if (m_minimum == m_maximum)
		{
			m_scrollThreshold = 0;
			m_slider.set(borderLeft, m_sliderZoneMin, scrollWidth, m_sliderZoneMax - m_sliderZoneMin);

			m_emptyUp.set(borderLeft, m_sliderZoneMin, scrollWidth, 0);
			m_emptyDown.set(borderLeft, m_sliderZoneMax, scrollWidth, 0);
		}
		// slider size is related to page step
		else
		{
			Int32 sliderRange = m_sliderZoneMax - m_sliderZoneMin + 1; // 100% pixel
			Int32 valueRange = m_maximum - m_minimum;     // virtual document range

			// current slider height in pixel
			Int32 sliderHeight = Int32(Float(sliderRange) / (Float(valueRange + m_pageStep) / m_pageStep));
			sliderHeight = o3d::max(sliderHeight, minSliderHeight); // minimal slider size

			// virtual document size
			sliderRange -= sliderHeight;
			Float ratio = Float(sliderRange) / valueRange;   // x pixels per range

			// minimal slider move
			m_scrollThreshold = o3d::max(1, Int32(ratio * m_singleStep));

			// current slider position in pixel
			Int32 sliderPos = Int32(ratio * m_sliderPos);

			m_slider.set(borderLeft, m_sliderZoneMin + sliderPos, scrollWidth, sliderHeight);

			updateSliderPos(0);
		}
	}
}

// Set the maximum position. Minimum is adjusted if necessary.
void ScrollBar::setMaximum(Int32 max)
{
	if (max < m_minimum)
		m_minimum = max;

	if (m_sliderPos > max)
		m_sliderPos = max;

	m_maximum = max;
	computeSliderSize();
}

// Set the minimum position. Maximum is adjusted if necessary.
void ScrollBar::setMinimum(Int32 min)
{
	if (min > m_maximum)
		m_maximum = min;

	if (m_sliderPos < min)
		m_sliderPos = min;

	m_minimum = min;
	computeSliderSize();
}

// Set the range, (minimal and maximal value). Minimal must be lesser or equal to maximal.
void ScrollBar::setRange(Int32 min, Int32 max)
{
	if (min > max)
		max = min;

	m_minimum = min;
	m_maximum = max;

	if (m_sliderPos < min)
		m_sliderPos = min;

	if (m_sliderPos > max)
		m_sliderPos = max;

	computeSliderSize();
}

// Set the single step (when hit a button or arrow key).
void ScrollBar::setSingleStep(Int32 step)
{
	m_singleStep = step;

	if (m_pageStep < m_singleStep)
		setPageStep(step);

	computeSliderSize();
}

// Set the page step (when drag the empty zone of the scroll bar or page button key).
void ScrollBar::setPageStep(Int32 pageStep)
{
	m_pageStep = pageStep;
	computeSliderSize();
}

// Define the slider current position in min and max value.
void ScrollBar::setSliderPos(Int32 pos)
{
	if (pos > m_maximum)
		pos = m_maximum;

	if (pos < m_minimum)
		pos = m_minimum;

	m_sliderPos = pos;
	computeSliderSize();
}

// Enable/disable slider tracking.
void ScrollBar::setTracking(Bool tracking)
{
	m_tracking = True;
}

// get the recommended widget default size.
Vector2i ScrollBar::getDefaultSize()
{
	return getUsedTheme()->getDefaultWidgetSize(Theme::SCROLL_BAR_BORDER);
}

// Is widget targeted ?
Bool ScrollBar::isTargeted(Int32 x, Int32 y, Widget *&widget)
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

// Mouse Events
Bool ScrollBar::mouseLeftPressed(Int32 x, Int32 y)
{
    ((Gui*)getScene()->getGui())->getWidgetManager()->lockWidgetMouse();

	if ((m_minimum < m_maximum) && m_hover && (m_part != SCROLLBAR_PART_NONE))
	{
		// In some case we need to have to check again the scroll bar part
		ScrollBar::mouseMove(x, y);

		// move up/left button.
		if (m_part == SCROLLBAR_PART_BUTTON_UP)
		{
			if (m_sliderPos > m_minimum)
			{
				Int32 oldSliderPos = m_sliderPos;

				//m_sliderPos -= m_singleStep;
				setSliderPos(m_sliderPos - m_singleStep);

				onSliderMoved(m_sliderPos - oldSliderPos);

				m_pushed = True;

				setDirty();
				startRepeatAction();
			}
		}
		// move down/right button.
		else if (m_part == SCROLLBAR_PART_BUTTON_DOWN)
		{
			if (m_sliderPos < m_maximum)
			{
				Int32 oldSliderPos = m_sliderPos;

				//m_sliderPos += m_singleStep;
				setSliderPos(m_sliderPos + m_singleStep);

				onSliderMoved(m_sliderPos - oldSliderPos);

				m_pushed = True;

				setDirty();
				startRepeatAction();
			}
		}
		// click on the empty up/left part
		else if (m_part == SCROLLBAR_PART_EMPTY_UP)
		{
			if (m_sliderPos > m_minimum)
			{
				Int32 oldSliderPos = m_sliderPos;

				//m_sliderPos -= m_pageStep;
				setSliderPos(m_sliderPos - m_pageStep);

				onSliderMoved(m_sliderPos - oldSliderPos);

				m_pushed = True;

				setDirty();
				startRepeatAction();
			}
		}
		// click on the empty down/right part
		else if (m_part == SCROLLBAR_PART_EMPTY_DOWN)
		{
			if (m_sliderPos < m_maximum)
			{
				Int32 oldSliderPos = m_sliderPos;

				//m_sliderPos += m_pageStep;
				setSliderPos(m_sliderPos + m_pageStep);

				onSliderMoved(m_sliderPos - oldSliderPos);

				m_pushed = True;

				setDirty();
				startRepeatAction();
			}
		}
		// click on the slider
		else if (m_part == SCROLLBAR_PART_SLIDER)
		{
			m_pushed = True;
			m_oldSliderPos = m_sliderPos;

			m_lastMousePos.set(x, y);

			setDirty();
		}
	}

	return True;
}

Bool ScrollBar::mouseLeftReleased(Int32 x, Int32 y)
{
    ((Gui*)getScene()->getGui())->getWidgetManager()->unlockWidgetMouse();

	if ((m_minimum < m_maximum) && m_pushed && (m_part != SCROLLBAR_PART_NONE))
	{
		// move up/left button.
		if (m_part == SCROLLBAR_PART_BUTTON_UP)
		{
			m_pushed = False;
			setDirty();

			stopRepeatAction();
		}
		// move down/right button.
		else if (m_part == SCROLLBAR_PART_BUTTON_DOWN)
		{
			m_pushed = False;
			setDirty();

			stopRepeatAction();
		}
		// click on the empty up/left part
		else if (m_part == SCROLLBAR_PART_EMPTY_UP)
		{
			m_pushed = False;
			setDirty();

			stopRepeatAction();
		}
		// click on the empty down/right part
		else if (m_part == SCROLLBAR_PART_EMPTY_DOWN)
		{
			m_pushed = False;
			setDirty();

			stopRepeatAction();
		}
		// click on the slider
		else if (m_part == SCROLLBAR_PART_SLIDER)
		{
			m_pushed = False;

			// if no tracking return the amount of change since the slider button pressed
			if (!m_tracking)
				onSliderMoved(m_sliderPos - m_oldSliderPos);

			setDirty();
		}

		// In some case we need to have to check again the scroll bar part
		mouseMove(x, y);
	}

	return True;
}

Bool ScrollBar::mouseMove(Int32 x, Int32 y)
{
	// hover on targeted part of the scroll bar
	if ((m_minimum < m_maximum) && !m_pushed)
	{
		x -= m_pos.x();
		y -= m_pos.y();

		// up/left button targeted
		if (m_buttonUp.isInside(Vector2i(x,y)))
		{
			m_part = SCROLLBAR_PART_BUTTON_UP;
			setDirty();
		}
		// down/right button targeted
		else if (m_buttonDown.isInside(Vector2i(x,y)))
		{
			m_part = SCROLLBAR_PART_BUTTON_DOWN;
			setDirty();
		}
		// empty up/left
		else if (m_emptyUp.isInside(Vector2i(x,y)))
		{
			m_part = SCROLLBAR_PART_EMPTY_UP;
			setDirty();
		}
		// empty down/right
		else if (m_emptyDown.isInside(Vector2i(x,y)))
		{
			m_part = SCROLLBAR_PART_EMPTY_DOWN;
			setDirty();
		}
		// slider
		else if (m_slider.isInside(Vector2i(x,y)))
		{
			m_part = SCROLLBAR_PART_SLIDER;
			setDirty();
		}
		// dead zone
		else
		{
			m_part = SCROLLBAR_PART_NONE;
			setDirty();
		}
	}
	// move the slider
	else if (m_pushed && (m_part == SCROLLBAR_PART_SLIDER))
	{
		if (m_orientation == HORIZONTAL_ALIGN)
		{
			// the mouse is outside of the slider position.
			if (((m_sliderPos == m_minimum) && (x <= m_lastMousePos.x())) ||
				((m_sliderPos == m_maximum) && (x >= m_lastMousePos.x())))
			{
				// out of boundaries
			}
			else if (x - m_lastMousePos.x())
			{
				Int32 oldPosition = m_sliderPos;
				Int32 delta = x - m_lastMousePos.x();

				// delta seek out of the minimal bound
				if ((m_slider.x() + delta) < m_sliderZoneMin)
				{
					m_slider.x() = m_sliderZoneMin;
					updateSliderPos(0);
					m_lastMousePos.set(x, y);

					m_sliderPos = m_minimum;

					if (m_tracking)
						onSliderMoved(m_sliderPos - oldPosition);

					setDirty();
				}
				// delta seek out of the maximal bound
				else if ((m_slider.x() + delta) > m_sliderZoneMax-m_slider.width())
				{
					m_slider.x() = m_sliderZoneMax - m_slider.width();
					updateSliderPos(0);
					m_lastMousePos.set(x, y);

					m_sliderPos = m_maximum;

					if (m_tracking)
						onSliderMoved(m_sliderPos - oldPosition);

					setDirty();
				}
				// delta is equal or greater than the minimal threshold
				else if (o3d::abs(delta) >= m_scrollThreshold)
				{
					updateSliderPos(delta);
					m_lastMousePos.set(x, y);

					// compute the logical slider position according the slider position in pixels
					Int32 sliderRange = m_sliderZoneMax - m_slider.width() - m_sliderZoneMin + 1;
					Int32 valueRange = m_maximum - m_minimum;
					Float ratio = Float(sliderRange) / valueRange;

					// round to the greater integer value (ceil)
					m_sliderPos = Int32(ceil(Float(m_slider.x()-m_sliderZoneMin) / ratio));
					m_sliderPos = o3d::clamp(m_sliderPos, m_minimum, m_maximum);

					if (m_tracking)
						onSliderMoved(m_sliderPos - oldPosition);

					setDirty();
				}
			}
		}
		else if (m_orientation == VERTICAL_ALIGN)
		{
			// the mouse is outside of the slider position.
			if (((m_sliderPos == m_minimum) && (y <= m_lastMousePos.y())) ||
				((m_sliderPos == m_maximum) && (y >= m_lastMousePos.y())))
			{
				// out of boundaries
			}
			else if (y - m_lastMousePos.y())
			{
				Int32 oldPosition = m_sliderPos;
				Int32 delta = y - m_lastMousePos.y();

				// delta seek out of the minimal bound
				if ((m_slider.y() + delta) < m_sliderZoneMin)
				{
					m_slider.y() = m_sliderZoneMin;
					updateSliderPos(0);
					m_lastMousePos.set(x, y);

					m_sliderPos = m_minimum;

					if (m_tracking)
						onSliderMoved(m_sliderPos - oldPosition);

					setDirty();
				}
				// delta seek out of the maximal bound
				else if ((m_slider.y() + delta) > m_sliderZoneMax-m_slider.height())
				{
					m_slider.y() = m_sliderZoneMax - m_slider.height();
					updateSliderPos(0);
					m_lastMousePos.set(x, y);

					m_sliderPos = m_maximum;

					if (m_tracking)
						onSliderMoved(m_sliderPos - oldPosition);

					setDirty();
				}
				// delta is equal or greater than the minimal threshold
				else if (o3d::abs(delta) >= m_scrollThreshold)
				{
					updateSliderPos(delta);
					m_lastMousePos.set(x, y);

					// compute the logical slider position according the slider position in pixels
					Int32 sliderRange = m_sliderZoneMax - m_slider.height() - m_sliderZoneMin + 1;
					Int32 valueRange = m_maximum - m_minimum;
					Float ratio = Float(sliderRange) / valueRange;

					// round to the greater integer value (ceil)
					m_sliderPos = Int32(ceil(Float(m_slider.y()-m_sliderZoneMin) / ratio));
					m_sliderPos = o3d::clamp(m_sliderPos, m_minimum, m_maximum);

					if (m_tracking)
						onSliderMoved(m_sliderPos - oldPosition);

					setDirty();
				}
			}
		}
	}

	return True;
}

Bool ScrollBar::mouseWheel(Int32 x, Int32 y, Int32 z)
{
	if ((m_minimum < m_maximum) && m_hover && (m_part != SCROLLBAR_PART_NONE))
	{
		if (z > 0 && m_sliderPos < m_maximum)
		{
			Int32 oldSliderPos = m_sliderPos;

			setSliderPos(m_sliderPos + m_singleStep);

			onSliderMoved(m_sliderPos - oldSliderPos);

			setDirty();
		}
		else if (z < 0 && m_sliderPos > m_minimum)
		{
			Int32 oldSliderPos = m_sliderPos;

			setSliderPos(m_sliderPos - m_singleStep);

			onSliderMoved(m_sliderPos - oldSliderPos);

			setDirty();
		}
	}

	return True;
}

void ScrollBar::mouseMoveIn()
{
	m_hover = True;
	setDirty();

	// signal
	onEnterWidget();
}

void ScrollBar::mouseMoveOut()
{
	m_hover = False;
	setDirty();

	// signal
	onLeaveWidget();
}

void ScrollBar::focused()
{
	m_capacities.enable(STATE_FOCUSED);

	// signal
	onFocus();
}

void ScrollBar::lostFocus()
{
	m_capacities.disable(STATE_FOCUSED);

	// signal
	onLostFocus();
}

// Draw
void ScrollBar::draw()
{
	 if (isShown())
		processCache(True);
}

void ScrollBar::updateCache()
{
	Theme *theme = getUsedTheme();

	// draw borders
    theme->drawWidget(Theme::SCROLL_BAR_BORDER, 0, 0, m_size.x(), m_size.y());

	// horizontal style
	if (m_orientation == HORIZONTAL_ALIGN)
	{
		Theme::WidgetDraw button;

		// left button
		if (!isActive() || (m_minimum == m_maximum) || (m_sliderPos == m_minimum))
			button = Theme::SCROLL_BAR_BUTTON_LEFT_DISABLED;
		else if (m_pushed && (m_part == SCROLLBAR_PART_BUTTON_UP))
			button = Theme::SCROLL_BAR_BUTTON_LEFT_TRUE;
		else if (m_hover && (m_part == SCROLLBAR_PART_BUTTON_UP))
            button = Theme::SCROLL_BAR_BUTTON_LEFT_HOVER;
		else
			button = Theme::SCROLL_BAR_BUTTON_LEFT_FALSE;

		theme->drawWidget(button, m_buttonUp.x(), m_buttonUp.y(), -1, -1);

		// right button
		if (!isActive() || (m_minimum == m_maximum) || (m_sliderPos == m_maximum))
			button = Theme::SCROLL_BAR_BUTTON_RIGHT_DISABLED;
		else if (m_pushed && (m_part == SCROLLBAR_PART_BUTTON_DOWN))
			button = Theme::SCROLL_BAR_BUTTON_RIGHT_TRUE;
		else if (m_hover && (m_part == SCROLLBAR_PART_BUTTON_DOWN))
            button = Theme::SCROLL_BAR_BUTTON_RIGHT_HOVER;
		else
			button = Theme::SCROLL_BAR_BUTTON_RIGHT_FALSE;

		theme->drawWidget(button, m_buttonDown.x(), m_buttonDown.y(), -1, -1);

		// empty left
		if (!isActive() || (m_minimum == m_maximum))
			button = Theme::SCROLL_BAR_HORIZONTAL_EMPTY_DISABLED;
		else if (m_pushed && (m_part == SCROLLBAR_PART_EMPTY_UP))
			button = Theme::SCROLL_BAR_HORIZONTAL_EMPTY_TRUE;
		else if (m_hover && (m_part == SCROLLBAR_PART_EMPTY_UP))
            button = Theme::SCROLL_BAR_HORIZONTAL_EMPTY_HOVER;
		else
			button = Theme::SCROLL_BAR_HORIZONTAL_EMPTY_FALSE;

		if (m_emptyUp.width())
			theme->drawWidget(button, m_emptyUp.x(), m_emptyUp.y(), m_emptyUp.width(), m_emptyUp.height());

		// slider
		if (!isActive() || (m_minimum == m_maximum))
			button = Theme::SCROLL_BAR_SLIDER_DISABLED;
		else if (m_pushed && (m_part == SCROLLBAR_PART_SLIDER))
			button = Theme::SCROLL_BAR_SLIDER_TRUE;
		else if (m_hover && (m_part == SCROLLBAR_PART_SLIDER))
            button = Theme::SCROLL_BAR_SLIDER_HOVER;
		else
			button = Theme::SCROLL_BAR_SLIDER_FALSE;

		theme->drawWidget(button, m_slider.x(), m_slider.y(), m_slider.width(), m_slider.height());

		// empty right
		if (!isActive() || (m_minimum == m_maximum))
			button = Theme::SCROLL_BAR_HORIZONTAL_EMPTY_DISABLED;
		else if (m_pushed && (m_part == SCROLLBAR_PART_EMPTY_DOWN))
			button = Theme::SCROLL_BAR_HORIZONTAL_EMPTY_TRUE;
		else if (m_hover && (m_part == SCROLLBAR_PART_EMPTY_DOWN))
            button = Theme::SCROLL_BAR_HORIZONTAL_EMPTY_HOVER;
		else
			button = Theme::SCROLL_BAR_HORIZONTAL_EMPTY_FALSE;

		if (m_emptyDown.width())
			theme->drawWidget(button, m_emptyDown.x(), m_emptyDown.y(), m_emptyDown.width(), m_emptyDown.height());
	}
	// vertical style
	else if (m_orientation == VERTICAL_ALIGN)
	{
		Theme::WidgetDraw button;

		// up button
		if (!isActive() || (m_minimum == m_maximum) || (m_sliderPos == m_minimum))
			button = Theme::SCROLL_BAR_BUTTON_UP_DISABLED;
		else if (m_pushed && (m_part == SCROLLBAR_PART_BUTTON_UP))
			button = Theme::SCROLL_BAR_BUTTON_UP_TRUE;
		else if (m_hover && (m_part == SCROLLBAR_PART_BUTTON_UP))
            button = Theme::SCROLL_BAR_BUTTON_UP_HOVER;
		else
			button = Theme::SCROLL_BAR_BUTTON_UP_FALSE;

		theme->drawWidget(button, m_buttonUp.x(), m_buttonUp.y(), -1, -1);

		// down button
		if (!isActive() || (m_minimum == m_maximum) || (m_sliderPos == m_maximum))
			button = Theme::SCROLL_BAR_BUTTON_DOWN_DISABLED;
		else if (m_pushed && (m_part == SCROLLBAR_PART_BUTTON_DOWN))
			button = Theme::SCROLL_BAR_BUTTON_DOWN_TRUE;
		else if (m_hover && (m_part == SCROLLBAR_PART_BUTTON_DOWN))
            button = Theme::SCROLL_BAR_BUTTON_DOWN_HOVER;
		else
			button = Theme::SCROLL_BAR_BUTTON_DOWN_FALSE;

		theme->drawWidget(button, m_buttonDown.x(), m_buttonDown.y(), -1, -1);

		// empty up
		if (!isActive() || (m_minimum == m_maximum))
			button = Theme::SCROLL_BAR_VERTICAL_EMPTY_DISABLED;
		else if (m_pushed && (m_part == SCROLLBAR_PART_EMPTY_UP))
			button = Theme::SCROLL_BAR_VERTICAL_EMPTY_TRUE;
		else if (m_hover && (m_part == SCROLLBAR_PART_EMPTY_UP))
            button = Theme::SCROLL_BAR_VERTICAL_EMPTY_HOVER;
		else
			button = Theme::SCROLL_BAR_VERTICAL_EMPTY_FALSE;

		if (m_emptyUp.height())
			theme->drawWidget(button, m_emptyUp.x(), m_emptyUp.y(), m_emptyUp.width(), m_emptyUp.height());

		// slider
		if (!isActive() || (m_minimum == m_maximum))
			button = Theme::SCROLL_BAR_SLIDER_DISABLED;
		else if (m_pushed && (m_part == SCROLLBAR_PART_SLIDER))
			button = Theme::SCROLL_BAR_SLIDER_TRUE;
		else if (m_hover && (m_part == SCROLLBAR_PART_SLIDER))
            button = Theme::SCROLL_BAR_SLIDER_HOVER;
		else
			button = Theme::SCROLL_BAR_SLIDER_FALSE;

		theme->drawWidget(button, m_slider.x(), m_slider.y(), m_slider.width(), m_slider.height());

		// empty down
		if (!isActive() || (m_minimum == m_maximum))
			button = Theme::SCROLL_BAR_VERTICAL_EMPTY_DISABLED;
		else if (m_pushed && (m_part == SCROLLBAR_PART_EMPTY_DOWN))
			button = Theme::SCROLL_BAR_VERTICAL_EMPTY_TRUE;
		else if (m_hover && (m_part == SCROLLBAR_PART_EMPTY_DOWN))
            button = Theme::SCROLL_BAR_VERTICAL_EMPTY_HOVER;
		else
			button = Theme::SCROLL_BAR_VERTICAL_EMPTY_FALSE;

		if (m_emptyDown.height())
			theme->drawWidget(button, m_emptyDown.x(), m_emptyDown.y(), m_emptyDown.width(), m_emptyDown.height());
	}
}

// Update for repeated action.
void ScrollBar::update()
{
	if (m_lastTime > 0)
	{
		UInt32 curTime = System::getMsTime();

		// wait before to repeat
		if (!m_repeat)
		{
			if ((curTime - m_lastTime) >= m_beforeDelay)
				m_repeat = True;
			else
				return;
		}

		// wait before repeat again
		if ((curTime - m_lastTime) >= m_repeatDelay)
		{
			m_lastTime = curTime;

			// process the current action
			switch (m_part)
			{
				case SCROLLBAR_PART_BUTTON_UP:
					if (m_sliderPos > m_minimum)
					{
						Int32 oldSliderPos = m_sliderPos;

						setSliderPos(m_sliderPos - m_singleStep);
						setDirty();

						onSliderMoved(m_sliderPos - oldSliderPos);
					}
					else
						m_lastTime = 0;
					break;

				case SCROLLBAR_PART_BUTTON_DOWN:
					if (m_sliderPos < m_maximum)
					{
						Int32 oldSliderPos = m_sliderPos;

						setSliderPos(m_sliderPos + m_singleStep);
						setDirty();

						onSliderMoved(m_sliderPos - oldSliderPos);
					}
					else
						m_lastTime = 0;
					break;

				case SCROLLBAR_PART_EMPTY_UP:
					if (m_sliderPos > m_minimum)
					{
						Int32 oldSliderPos = m_sliderPos;

						setSliderPos(m_sliderPos - m_pageStep);
						setDirty();

						onSliderMoved(m_sliderPos - oldSliderPos);
					}
					else
						m_lastTime = 0;

					break;

				case SCROLLBAR_PART_EMPTY_DOWN:
					if (m_sliderPos < m_maximum)
					{
						Int32 oldSliderPos = m_sliderPos;

						setSliderPos(m_sliderPos + m_pageStep);
						setDirty();

						onSliderMoved(m_sliderPos - oldSliderPos);
					}
					else
						m_lastTime = 0;
					break;

				default:
					break;
			}
		}
	}
}

void ScrollBar::startRepeatAction()
{
	m_lastTime = System::getMsTime();
	m_repeat = False;
}

void ScrollBar::stopRepeatAction()
{
	m_lastTime = 0;
	m_repeat = False;
}

// Update the slider position.
void ScrollBar::updateSliderPos(Int32 amount)
{
	if (m_orientation == HORIZONTAL_ALIGN)
	{
		m_slider.x() += amount;

		if (m_slider.x() < m_sliderZoneMin)
			m_slider.x() = m_sliderZoneMin;

		if (m_slider.x()+m_slider.width() > m_sliderZoneMax)
			m_slider.x() = m_sliderZoneMax - m_slider.width();

		// update the empty left
		m_emptyUp.width() = m_slider.x() - m_sliderZoneMin;

		// update the empty right
		m_emptyDown.x() = m_slider.x() + m_slider.width();
		m_emptyDown.width() = m_emptyDown.x() >= m_sliderZoneMax ? 0 : m_sliderZoneMax - m_emptyDown.x();
	}
	else if (m_orientation == VERTICAL_ALIGN)
	{
		m_slider.y() += amount;

		if (m_slider.y() < m_sliderZoneMin)
			m_slider.y() = m_sliderZoneMin;

		if (m_slider.y()+m_slider.height() > m_sliderZoneMax)
			m_slider.y() = m_sliderZoneMax - m_slider.height();

		// update the empty left
		m_emptyUp.height() = m_slider.y() - m_sliderZoneMin;

		// update the empty right
		m_emptyDown.y() = m_slider.y() + m_slider.height();
		m_emptyDown.height() = m_emptyDown.y() >= m_sliderZoneMax ? 0 : m_sliderZoneMax - m_emptyDown.y();
	}

	setDirty();
}

LayoutScrollBar::LayoutScrollBar(
		Widget *parent,
		ScrollBar::Orientation orientation,
		const Vector2i &pos,
		const Vector2i &size,
		const String &name) :
	ScrollBar(parent, orientation, pos, size, name)
{
}

Bool LayoutScrollBar::isTargeted(Int32 x, Int32 y, Widget *&widget)
{
	return ScrollBar::isTargeted(
				x,
				y,
				widget);
}

Bool LayoutScrollBar::mouseLeftPressed(Int32 x, Int32 y)
{
	return ScrollBar::mouseLeftPressed(
				x - getParentWidget()->getScrollPos().x(),
				y - getParentWidget()->getScrollPos().y());
}

Bool LayoutScrollBar::mouseLeftReleased(Int32 x, Int32 y)
{
	return ScrollBar::mouseLeftReleased(
				x - getParentWidget()->getScrollPos().x(),
				y - getParentWidget()->getScrollPos().y());
}

Bool LayoutScrollBar::mouseMove(Int32 x, Int32 y)
{
	return ScrollBar::mouseMove(
				x - getParentWidget()->getScrollPos().x(),
				y - getParentWidget()->getScrollPos().y());
}

Bool LayoutScrollBar::mouseWheel(Int32 x, Int32 y, Int32 z)
{
	return ScrollBar::mouseWheel(
				x - getParentWidget()->getScrollPos().x(),
				y - getParentWidget()->getScrollPos().y(),
				z);
}

