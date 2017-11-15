/**
 * @file window.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/window.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"
#include "o3d/gui/widgets/toolbutton.h"
#include "o3d/gui/widgetdrawcompass.h"
#include "o3d/gui/widgets/boxlayout.h"

#include <algorithm>

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Window, GUI_WIDGET_WINDOW, Widget)

/*---------------------------------------------------------------------------------------
  creation constructor
---------------------------------------------------------------------------------------*/
Window::Window(Widget *parent,
    const String &title,
    const Vector2i &pos,
    const Vector2i &size,
    Int32 windowStyle,
    const String &name) :
		Widget(parent, pos, size, name),
        m_windowStyle((WindowStyle)windowStyle),
		m_resize(RESIZE_NONE),
        m_icon(nullptr),
		m_windowState(_STATE_NORMAL),
        m_reduceButton(nullptr),
        m_maximizeButton(nullptr),
        m_closeButton(nullptr),
        m_updatePart(_UPDATE_BOTH),
        m_prevFocusWidget(nullptr)
{
	m_title.setText(title);

	m_pos = pos;
	m_minSize = m_size = size;

	init();
}

//! Default constructor.
Window::Window(BaseObject *parent) :
		Widget(parent),
        m_windowStyle(DEFAULT_STYLE),
		m_resize(RESIZE_NONE),
        m_icon(nullptr),
		m_windowState(_STATE_NORMAL),
        m_reduceButton(nullptr),
        m_maximizeButton(nullptr),
        m_closeButton(nullptr),
        m_updatePart(_UPDATE_BOTH),
        m_prevFocusWidget(nullptr)
{
}

//! Creation constructor as a top-level window
Window::Window(
	WidgetManager *parent,
	const String &title,
	const Vector2i &pos,
	const Vector2i &size,
    Int32 windowStyle,
	const String &name) :
		Widget(parent, pos, size, name),
        m_windowStyle((WindowStyle)windowStyle),
		m_resize(RESIZE_NONE),
        m_icon(nullptr),
		m_windowState(_STATE_NORMAL),
        m_reduceButton(nullptr),
        m_maximizeButton(nullptr),
        m_closeButton(nullptr),
        m_updatePart(_UPDATE_BOTH),
        m_prevFocusWidget(nullptr)
{
	m_title.setText(title);

	m_pos = pos;
	m_minSize = m_size = size;

	init();
}

void Window::init()
{
    //m_capacities.disable(STATE_MOVING);
    //m_capacities.disable(STATE_RESIZING);
    m_capacities.enable(CAPS_FIT_TO_CONTENT);
    //m_capacities.disable(CAPS_ALWAYS_ON_TOP);
    //m_capacities.disable(CAPS_MODAL);
    //m_capacities.disable(STATE_FOCUSED);

	// default position
    if (m_pos.x() < 0) m_pos.x() = 0;
    if (m_pos.y() < 0) m_pos.y() = 0;

	// default size
	Vector2i defaultSize = getDefaultSize();
    if (m_size.x() < 0) m_minSize.x() = m_size.x() = defaultSize.x();
    if (m_size.y() < 0) m_minSize.y() = m_size.y() = defaultSize.y();

	// default layout
	m_pLayout = new BoxLayout(this,BoxLayout::VERTICAL_LAYOUT);
	m_pLayout->setParent(this);
	m_pLayout->defineLayoutItem();

	// title
	m_title.setVerticalAlignment(TextZone::MIDDLE_ALIGN);

	Theme *pTheme = getUsedTheme();
	// default title color in non focused mode
	m_title.setColor(pTheme->getWidgetDrawMode(Theme::TITLE_BAR_FALSE)->getDefaultTextColor());

	// buttons
    if (m_windowStyle & REDUCE_BUTTON)
	{
		// TODO
//		m_reduceButton = new ToolButton((Widget*)this, ToolButton::PUSH_BUTTON);
//		((ToolButton*)m_reduceButton)->SetToolButtonThemeStyle(
//			Theme::ReduceButton_False,
//			Theme::ReduceButton_True,
//			Theme::ReduceButton_Hover);
//		m_ReduceButton->setParent(this);

        //static_cast<ToolButton*>(m_reduceButton)->onToolButtonReleased.connect(this, &Window::ReducePressed);
	}

    if (m_windowStyle & MAXIMIZE_RESTORE_BUTTON)
	{
		// TODO
//		m_maximizeButton = new ToolButton((Widget*)this, ToolButton::PUSH_BUTTON);
//		((ToolButton*)m_maximizeButton)->SetToolButtonThemeStyle(
//			Theme::MaximizeButton_False,
//			Theme::MaximizeButton_True,
//			Theme::MaximizeButton_Hover);
//		m_MaximizeButton->setParent(this);

        //static_cast<ToolButton*>(m_maximizeButton)->onToolButtonReleased.connect(this, &Window::MaximizePressed);
    }

    if (m_windowStyle & CLOSE_BUTTON)
	{
		m_closeButton = new ToolButton((Widget*)this, ToolButton::PUSH_BUTTON);
		((ToolButton*)m_closeButton)->setToolButtonThemeStyle(
			Theme::CLOSE_BUTTON_FALSE,
			Theme::CLOSE_BUTTON_TRUE,
            Theme::CLOSE_BUTTON_HOVER,
            (Theme::WidgetDraw)0);
		m_closeButton->setParent(this);

        static_cast<ToolButton*>(m_closeButton)->onToolButtonReleased.connect(this, &Window::closePressed);
	}

	// icon TODO use the theme's default window icon
	//m_icon

	// update the window
	sizeChanged();
}

/*---------------------------------------------------------------------------------------
  destructor
---------------------------------------------------------------------------------------*/
Window::~Window()
{
	deletePtr(m_reduceButton);
	deletePtr(m_maximizeButton);
	deletePtr(m_closeButton);
}

// set the parent layout. By default an HLayout is setup.
void Window::setLayout(Layout *pLayout)
{
	m_pLayout = pLayout;
}

// get the parent layout. By default an HLayout is setup. (const version)
const Layout* Window::getLayout() const
{
	return m_pLayout.get();
}

// get the parent layout. By default an HLayout is setup.
Layout* Window::getLayout()
{
	return m_pLayout.get();
}

/*---------------------------------------------------------------------------------------
  set the widget always on top
---------------------------------------------------------------------------------------*/
void Window::setAlwaysOnTop(Bool onTop)
{
	m_capacities.setBit(CAPS_ALWAYS_ON_TOP, onTop);
    getGui()->getWidgetManager()->setAlwaysOnTop(this, onTop);
}

void Window::center(Window::CenterType center)
{
	Vector2i size;

	if (!(center & CENTER_ON_SCREEN) && getParentWidget())
		size = getParentWidget()->size();
	else
        size.set(getGui()->getWidth(), getGui()->getHeight());

	if (center & CENTER_VERTICALY)
        m_pos.y() = (size.y() - m_size.y()) >> 1;

	if (center & CENTER_HORIZONTALY)
        m_pos.x() = (size.x() - m_size.x()) >> 1;

    positionChanged();
}

void Window::updateTabIndex(Widget *widget)
{
    if (widget->getTabIndex() >= 0 && widget->isFocusable())
    {
        m_tabIndexWidgets.push_back(widget);
        m_tabIndexWidgets.sort([](Widget *a, Widget *b) {
            return (a->getTabIndex() < b->getTabIndex());
        });
    }
}

void Window::onDeleteWidget(BaseObject* object)
{
    if (object == m_prevFocusWidget)
        m_prevFocusWidget = nullptr;
}

Widget *Window::findNextTabIndex(Widget *widget, Int32 direction)
{
   if (direction == 0)
        return widget;

    if (!widget)
    {
        // returns the first ordered widget
        if (direction > 0)
        {
            Widget *first = nullptr;

            if (m_tabIndexWidgets.size() > 0)
            {
                for (IT_WidgetList it = m_tabIndexWidgets.begin(); it != m_tabIndexWidgets.end(); ++it)
                {
                    first = (*it)->findNextTabIndex(nullptr, 1);
                    if (first && first->isFocusable())
                        return first;
                }
            }

            if (getLayout())
            {
                first = getLayout()->findNextTabIndex(nullptr, 1);
                if (first && first->isFocusable())
                    return first;
            }
        }
        else if (direction < 0) // or the last
        {
            Widget *last = nullptr;

            // first get the last from default ordered tree
            if (getLayout())
            {
                last = getLayout()->findNextTabIndex(nullptr, -1);
                if (last && last->isFocusable())
                    return last;
            }

            // if not, is there widgets into the ordrered list
            if (m_tabIndexWidgets.size() > 0)
            {
                for (RIT_WidgetList rit = m_tabIndexWidgets.rbegin(); rit != m_tabIndexWidgets.rend(); ++rit)
                {
                    last = (*rit)->findNextTabIndex(nullptr, -1);
                    if (last && last->isFocusable())
                        return last;
                }
            }
        }

        return nullptr;
    }

    // search first in orderer widgets
    if (widget->getTabIndex() >= 0)
    {
        Widget *next = nullptr;

        if (direction > 0)
        {
            for (IT_WidgetList it = m_tabIndexWidgets.begin(); it != m_tabIndexWidgets.end(); ++it)
            {
                if ((*it)->getTabIndex() > widget->getTabIndex())
                {
                    next = (*it)->findNextTabIndex(widget, 1);
                    if (next)
                        return next;
                }
            }

            // no results then return the first into the default orderer tree
            if (getLayout())
            {
                next = getLayout()->findNextTabIndex(nullptr, 1);
                if (next)
                    return next;
            }
        }
        else if (direction < 0)
        {
            for (RIT_WidgetList rit = m_tabIndexWidgets.rbegin(); rit != m_tabIndexWidgets.rend(); ++rit)
            {
                if ((*rit)->getTabIndex() < widget->getTabIndex())
                {
                    next = (*rit)->findNextTabIndex(widget, -1);
                    if (next)
                        return next;
                }
            }

            // no results then return the last into the default orderer tree
            if (getLayout())
            {
                next = getLayout()->findNextTabIndex(nullptr, -1);
                if (next)
                    return next;
            }
        }
    }
    else
    {
        // look into the tree ordered widgets
        if (getLayout())
        {
            Widget *next = nullptr;

            // search the next sibling and deeply, and finally bubble
            Widget *parent = widget->getParentWidget();
            while (parent)
            {
                if (parent->getLayout())
                    next = parent->getLayout()->findNextTabIndex(widget, direction);
                else
                    next = parent->findNextTabIndex(widget, direction);

                if (next && next->isFocusable())
                    return next;

                if (parent->isTopLevelWidget())
                    break;

                widget = parent;
                parent = parent->getParentWidget();
            }

            // if no result find for first or last
            next = getLayout()->findNextTabIndex(nullptr, direction);
            if (next)
                return next;
        }
    }

    return nullptr;
}

Widget* Window::getPreviouslyFocusedWidget()
{
    return m_prevFocusWidget;
}

void Window::setPreviouslyFocusedWidget(Widget *widget)
{
    if (m_prevFocusWidget)
        m_prevFocusWidget->disconnect(this);
     //   disconnect(m_prevFocusWidget);

    m_prevFocusWidget = widget;

    // follow for safety
    if (m_prevFocusWidget)
        m_prevFocusWidget->onDeleteObject.connect(this, &Window::onDeleteWidget);
}

/*---------------------------------------------------------------------------------------
  is the widget always on top
---------------------------------------------------------------------------------------*/
Bool Window::isAlwaysOnTop() const
{
	return m_capacities.getBit(CAPS_ALWAYS_ON_TOP);
}

void Window::setModal(Bool modal)
{
	m_capacities.setBit(CAPS_MODAL, modal);
}

Bool Window::isModal() const
{
	return m_capacities.getBit(CAPS_MODAL);
}

// Get the current cursor type name for this widget if targeted.
String Window::getCursorTypeName() const
{
	if (m_capacities.getBit(STATE_MOVING))
		return "Move";
	else if (m_resize != RESIZE_NONE)//m_resizing)
	{
		if (m_resize == RESIZE_TOP)
			return "BorderTop";
		else if (m_resize == RESIZE_BOTTOM)
			return "BorderBottom";
		else if (m_resize == RESIZE_LEFT)
			return "BorderLeft";
		else if (m_resize == RESIZE_RIGHT)
			return "BorderRight";
		else if (m_resize == (RESIZE_TOP | RESIZE_LEFT))
			return "BorderTopLeft";
		else if (m_resize == (RESIZE_BOTTOM | RESIZE_RIGHT))
			return "BorderBottomRight";
		else if (m_resize == (RESIZE_TOP | RESIZE_RIGHT))
			return "BorderTopRight";
		else if (m_resize == (RESIZE_BOTTOM | RESIZE_LEFT))
			return "BorderBottomLeft";
		else
			return "Default";
	}
	else
		return "Default";
}

void Window::show(Bool show)
{
	if (show != m_capacities.getBit(STATE_VISIBILITY))
	{
		m_capacities.setBit(STATE_VISIBILITY, show);

        if (getParent() == getGui()->getWidgetManager())
            getGui()->getWidgetManager()->showHideWindow(this, show);
		//else
		// TODO with its parent window
	}
}

// return the top-left absolute position of the widget (containing the local origin)
Vector2i Window::getOrigin() const
{
	const Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::WINDOW_NORMAL);

    if (m_windowStyle & TITLE_BAR)
		return drawMode->getTopLeftMargin() + Vector2i(0, theme->getWidgetHeight(Theme::TITLE_BAR_NORMAL));
    else if (m_windowStyle & BORDER_STYLE)
		return drawMode->getTopLeftMargin();
    else
        return Vector2i(0, 0);
}

// get the recommended widget default size
Vector2i Window::getDefaultSize()
{
	return getUsedTheme()->getDefaultWidgetSize(Theme::WINDOW_NORMAL);
}

// get the extra size of the window depending of its defined theme
Vector2i Window::getWindowBorderSize() const
{
	const Theme *theme = getUsedTheme();
	Vector2i borderSize;

	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::WINDOW_NORMAL);

    if (m_windowStyle & BORDER_STYLE)
        borderSize = drawMode->getTopLeftMargin() + drawMode->getBottomRightMargin();

    if (m_windowStyle & TITLE_BAR)
		borderSize.y() += theme->getWidgetHeight(Theme::TITLE_BAR_NORMAL);

	return borderSize;
}

// Set the title
void Window::setTitle(const String &title)
{
	m_title.setText(title);

	// need a cache update
	m_updatePart |= _UPDATE_TITLE_BAR;
	setDirty();
}

/*---------------------------------------------------------------------------------------
  Mouse events management
---------------------------------------------------------------------------------------*/
Bool Window::isTargeted(Int32 x,Int32 y,Widget *&pWidget)
{
	// no negative part
	if (x < 0 || y < 0)
		return False;

    if ((x < m_size.x()) && (y < m_size.y()))
	{
		// others children widgets
		Vector2i org = getOrigin();
		pWidget = this;

		// delta by the client origin for children widgets
		// title bar buttons are in children space, using
		// negatives coordinates.
        x -= org.x();
        y -= org.y();

		// reduce button
		if (m_reduceButton && m_reduceButton->isTargeted(
            x - m_reduceButton->pos().x(),
            y - m_reduceButton->pos().y(),
			pWidget))
		{
			return True;
		}

		// maximize button
		if (m_maximizeButton && m_maximizeButton->isTargeted(
            x - m_maximizeButton->pos().x(),
            y - m_maximizeButton->pos().y(),
			pWidget))
		{
			return True;
		}

		// close button
		if (m_closeButton && m_closeButton->isTargeted(
            x - m_closeButton->pos().x(),
            y - m_closeButton->pos().y(),
			pWidget))
		{
			return True;
		}

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

Bool Window::mouseLeftPressed(Int32 x,Int32 y)
{
	// check if the event appear on a border : RESIZE (only if resizable window)
    if ((m_windowStyle & RESIZABLE_WINDOW) == RESIZABLE_WINDOW)
	{
		if (m_resize != RESIZE_NONE)
		{
			m_capacities.enable(STATE_RESIZING);
			m_processAction.set(x,y);

            getGui()->getWidgetManager()->lockWidgetMouse();
			return True;
		}
	}

	// check if the event appear in the title bar : MOVE (only if a title bar is present)
    if ((m_windowStyle & TITLE_BAR) == TITLE_BAR)
	{
        if (y < m_pos.y() + getUsedTheme()->getWidgetHeight(Theme::TITLE_BAR_NORMAL))
		{
			m_capacities.enable(STATE_MOVING);
			m_processAction.set(x,y);

            getGui()->getWidgetManager()->lockWidgetMouse();
			return True;
		}
	}

	return False;
}

Bool Window::mouseLeftReleased(Int32 x,Int32 y)
{
	if (m_capacities.getBit(STATE_MOVING))
	{
		m_capacities.disable(STATE_MOVING);
        getGui()->getWidgetManager()->unlockWidgetMouse();

		return True;
	}
	else if (m_capacities.getBit(STATE_RESIZING))
	{
		m_capacities.disable(STATE_RESIZING);
        getGui()->getWidgetManager()->unlockWidgetMouse();

		return True;
	}

	return False;
}

Bool Window::mouseMove(Int32 x,Int32 y)
{
	if (m_capacities.getBit(STATE_MOVING))
	{
		Vector2i pos = m_pos;
		Int32 titleBarHeight = getUsedTheme()->getWidgetHeight(Theme::TITLE_BAR_NORMAL);

		m_pos.x() += x - m_processAction.x();
		m_pos.y() += y - m_processAction.y();

		// limits to top
		if (m_pos.y() < 0)
		{
			m_pos.y() = 0;
			m_processAction.y() += m_pos.y() - pos.y();
		}
		// limits to down
        else if ((m_pos.y() + titleBarHeight) >= getGui()->getHeight())
		{
            m_pos.y() = getGui()->getHeight() - titleBarHeight;
			m_processAction.y() += m_pos.y() - pos.y();
		}
		else
			m_processAction.y() = y;

		m_processAction.x() = x;

		if (pos != m_pos)
			positionChanged();

		return True;
	}
	else if (m_capacities.getBit(STATE_RESIZING) && m_resize != RESIZE_NONE)
	{
		Vector2i pos = m_pos;
		Vector2i size = m_size;

		if ((m_resize & RESIZE_TOP) == RESIZE_TOP)
		{
			m_size.y() -= y - m_processAction.y();
			m_pos.y() += y - m_processAction.y();

			if (m_size.y() < m_minSize.y())
			{
				m_pos.y() -= m_minSize.y() - m_size.y();
				m_size.y() = m_minSize.y();
				m_processAction.y() += m_pos.y() - pos.y();
			}
			// clamp to top desktop border
			else if (m_pos.y() < 0)
			{
				m_size.y() += m_pos.y();
				m_pos.y() = 0;
				m_processAction.y() += m_pos.y() - pos.y();
			}
			else
				m_processAction.y() = y;
		}
		else if ((m_resize & RESIZE_BOTTOM) == RESIZE_BOTTOM)
		{
			m_size.y() += y - m_processAction.y();

			if (m_size.y() < m_minSize.y())
			{
				m_processAction.y() = y + m_minSize.y()-m_size.y();
				m_size.y() = m_minSize.y();
			}
			// clamp to bottom desktop border
            else if (m_pos.y() + m_size.y() >= getGui()->getHeight())
			{
                m_size.y() -= m_pos.y() + m_size.y() - getGui()->getHeight();
				m_processAction.y() += m_size.y() - size.y();
			}
			else
				m_processAction.y() = y;
		}
		else
			m_processAction.y() = y;

		if ((m_resize & RESIZE_LEFT) == RESIZE_LEFT)
		{
			m_size.x() -= x - m_processAction.x();
			m_pos.x() += x - m_processAction.x();

			if (m_size.x() < m_minSize.x())
			{
				m_pos.x() -= m_minSize.x() - m_size.x();
				m_size.x() = m_minSize.x();
				m_processAction.x() += m_pos.x() - pos.x();
			}
			// clamp to left desktop border
			else if (m_pos.x() < 0)
			{
				m_size.x() += m_pos.x();
				m_pos.x() = 0;
				m_processAction.x() += m_pos.x() - pos.x();
			}
			else
				m_processAction.x() = x;
		}
		else if ((m_resize & RESIZE_RIGHT) == RESIZE_RIGHT)
		{
			m_size.x() += x - m_processAction.x();

			if (m_size.x() < m_minSize.x())
			{
				m_processAction.x() = x + m_minSize.x()-m_size.x();
				m_size.x() = m_minSize.x();
			}
			// clamp to right desktop border
            else if (m_pos.x() + m_size.x() >= getGui()->getWidth())
			{
                m_size.x() -= m_pos.x() + m_size.x() - getGui()->getWidth();
				m_processAction.x() += m_size.x() - size.x();
			}
			else
				m_processAction.x() = x;
		}
		else
			m_processAction.x() = x;

		// send events
		if (pos != m_pos)
			positionChanged();

		if (size != m_size)
		{
			// layout
			layout();
			sizeChanged();
		}

		return True;
	}
	else
	{
		m_resize = RESIZE_NONE;
		m_windowState = _STATE_NORMAL;

        if ((m_windowStyle & RESIZABLE_WINDOW) == RESIZABLE_WINDOW)
		{
			// top resize
			if (hasTopBorder() && (y >= m_pos.y()) && (y < m_pos.y()+getBorderSize()))
				m_resize = RESIZE_TOP;

			// bottom resize
			if (hasBottomBorder() && (y < m_pos.y()+m_size.y()) && (y > m_pos.y()+m_size.y()-getBorderSize()))
				m_resize = RESIZE_BOTTOM;

			// left resize
			if (hasLeftBorder() && (x >= m_pos.x()) && (x < m_pos.x()+getBorderSize()))
				m_resize = RESIZE_LEFT;

			// right resize
			if (hasRightBorder() && (x < m_pos.x()+m_size.x()) && (x > m_pos.x()+m_size.x()-getBorderSize()))
				m_resize = RESIZE_RIGHT;

			// top-left resize
			if (hasLeftBorder() && hasTopBorder() &&
				((y >= m_pos.y()) && (y < m_pos.y()+2*getBorderSize())) &&
				((x >= m_pos.x()) && (x < m_pos.x()+2*getBorderSize())))
				m_resize = RESIZE_TOP | RESIZE_LEFT;

			// top-right resize
			if (hasRightBorder() && hasTopBorder() &&
				((y >= m_pos.y()) && (y < m_pos.y()+2*getBorderSize())) &&
				((x < m_pos.x()+m_size.x()) && (x > m_pos.x()+m_size.x()-2*getBorderSize())))
				m_resize = RESIZE_TOP | RESIZE_RIGHT;

			// bottom-left resize
			if (hasLeftBorder() && hasBottomBorder() &&
				((y < m_pos.y()+m_size.y()) && (y > m_pos.y()+m_size.y()-2*getBorderSize())) &&
				((x >= m_pos.x()) && (x < m_pos.x()+2*getBorderSize())))
				m_resize = RESIZE_BOTTOM | RESIZE_LEFT;

			// bottom-right resize
			if (hasRightBorder() && hasBottomBorder() &&
				((y < m_pos.y()+m_size.y()) && (y > m_pos.y()+m_size.y()-2*getBorderSize())) &&
				((x < m_pos.x()+m_size.x()) && (x > m_pos.x()+m_size.x()-2*getBorderSize())))
				m_resize = RESIZE_BOTTOM | RESIZE_RIGHT;
		}

		return True;
	}
}

void Window::positionChanged()
{
	// Nothing
}

void Window::focused()
{
	if (!m_capacities.getBit(STATE_FOCUSED))
	{
		m_capacities.enable(STATE_FOCUSED);

        if ((m_windowStyle & TITLE_BAR) == TITLE_BAR)
		{
			m_updatePart |= _UPDATE_TITLE_BAR;
			setDirty();
		}

		// signal
		onFocus();
	}
}

void Window::lostFocus()
{
	if (m_capacities.getBit(STATE_FOCUSED))
	{
		m_capacities.disable(STATE_FOCUSED);

        if ((m_windowStyle & TITLE_BAR) == TITLE_BAR)
		{
			m_updatePart |= _UPDATE_TITLE_BAR;
			setDirty();
		}

		// signal
		onLostFocus();
	}
}

Vector2i Window::getClientSize() const
{
	// compute the client size
	const Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::WINDOW_NORMAL);
	if (!drawMode)
		return m_size;

    Vector2i size = m_size;

    if (m_windowStyle & BORDER_STYLE)
        size -= drawMode->getTopLeftMargin() + drawMode->getBottomRightMargin();

    if (m_windowStyle & TITLE_BAR)
		size.y() -= getUsedTheme()->getWidgetHeight(Theme::TITLE_BAR_NORMAL);

	return size;
}

void Window::sizeChanged()
{
	const Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::WINDOW_NORMAL);
	if (!drawMode)
		return;

	// compute the position of the widgets into the title bar
    if (m_windowStyle & TITLE_BAR)
	{
		Vector2i origin = getOrigin();
		Int32 TitleHeight = theme->getWidgetHeight(Theme::TITLE_BAR_NORMAL);

		Int32 xOfs = 0, yOfs = 0;

		xOfs = drawMode->getLeftMargin();
		yOfs = 0;//drawMode->getTopMargin();

        if (m_icon && (m_windowStyle & TITLE_ICON))
			xOfs += m_icon->size().x();

		// get the font height (default if necessary)
        Int32 fontHeight = m_title.fontHeight();
		if (fontHeight == -1)
			fontHeight = theme->getDefaultFontSize();

		// title position start after the hypothetical icon
		m_title.setPos(xOfs, -((TitleHeight - fontHeight) >> 1));

        xOfs = m_size.x() - drawMode->getRightMargin();

        xOfs -= origin.x();
        yOfs -= origin.y();

		// close button
        if ((m_windowStyle & CLOSE_BUTTON) && m_closeButton)
		{
            xOfs -= m_closeButton->size().x();

			m_closeButton->setPos(
                        xOfs, yOfs + ((TitleHeight - m_closeButton->size().y()) >> 1));

			xOfs -= 2;
		}

		// maximize button
        if ((m_windowStyle & MAXIMIZE_RESTORE_BUTTON) && m_maximizeButton)
		{
            xOfs -= m_maximizeButton->size().x();

			m_maximizeButton->setPos(
                        xOfs, yOfs + ((TitleHeight - m_maximizeButton->size().y()) >> 1));

			xOfs -= 2;
		}

		// reduce button
        if ((m_windowStyle & REDUCE_BUTTON) && m_reduceButton)
		{
            xOfs -= m_reduceButton->size().x();

			m_reduceButton->setPos(
                        xOfs, yOfs + ((TitleHeight - m_reduceButton->size().y()) >> 1));

			xOfs -= 2;
		}

		// title size is defined depend of the rest of the width
		m_title.setSize(xOfs, TitleHeight);
	}

	// need to update the cache
	m_updatePart |= _UPDATE_BOTH;
	setDirty();
}

void Window::reducePressed()
{
	if (m_windowState != _STATE_REDUCED)
	{
		// TODO reduce
		m_windowState = _STATE_REDUCED;

		// signal
		onWindowReduce();
	}
}

void Window::maximizePressed()
{
	// restore
	if (m_windowState == _STATE_MAXIMIZED)
	{
		// TODO restore
		m_windowState = _STATE_NORMAL;

		// signal
		onWindowRestore();
	}
	// maximize
	else
	{
		// TODO maximize
		m_windowState = _STATE_MAXIMIZED;

		// signal
		onWindowMaximize();
	}
}

void Window::closePressed()
{
	// signal
    onWindowClose();
}

Bool Window::keyboardToggled(Keyboard *keyboard, KeyEvent event)
{
	if (event.isPressed() && (event.key() == KEY_ESCAPE))
	{
		// signal
		onWindowClose();

		return True;
	}

	return False;
}

/*---------------------------------------------------------------------------------------
  draw
---------------------------------------------------------------------------------------*/
void Window::draw()
{
	if (isShown())
	{
		// is need to recompute the layout
		if (getLayout() && getLayout()->isDirty())
			getLayout()->layout();

		Vector2i org = getOrigin();

		processCache(m_updatePart == _UPDATE_BOTH ? True : False);

		// translate in window coordinates
		getScene()->getContext()->modelView().translate(
                Vector3((Float)m_pos.x(), (Float)m_pos.y(), 0.f));

		// translate in client area window coordinates
		getScene()->getContext()->modelView().translate(
                Vector3((Float)org.x(), (Float)org.y(),0.f));

		// first draw title bar button with negative client area window coordinates
        if ((m_windowStyle & TITLE_BAR) == TITLE_BAR)
		{
			// reduce button
            if (m_reduceButton && (m_windowStyle & REDUCE_BUTTON))
				m_reduceButton->updateCache();

			// maximize button
            if (m_maximizeButton && (m_windowStyle & MAXIMIZE_RESTORE_BUTTON))
				m_maximizeButton->updateCache();

			// close button
            if (m_closeButton && (m_windowStyle & CLOSE_BUTTON))
				m_closeButton->updateCache();
		}

		// finally draw its content into the client area
		if (getLayout())
		{
			getScene()->getContext()->modelView().push();
			getLayout()->draw();
			getScene()->getContext()->modelView().pop();
		}
	}
}

void Window::updateCache()
{
	Theme *pTheme = getUsedTheme();

	Int32 titleBarHeight = 0;

	// Draw Titlebar
    if ((m_updatePart & _UPDATE_TITLE_BAR) && (m_windowStyle & TITLE_BAR))
	{
		titleBarHeight = pTheme->getWidgetHeight(Theme::TITLE_BAR_NORMAL);

		pTheme->drawWidget(
					m_capacities.getBit(STATE_FOCUSED) ? Theme::TITLE_BAR_NORMAL : Theme::TITLE_BAR_FALSE,
					0,
					0,
                    m_size.x(),
					-1);

		m_title.setColor(
					pTheme->getWidgetDrawMode(
					m_capacities.getBit(STATE_FOCUSED) ? Theme::TITLE_BAR_NORMAL : Theme::TITLE_BAR_FALSE)->getDefaultTextColor());

		// with its title
		m_title.write(
			pTheme->getDefaultFont(),
			pTheme->getDefaultFontSize());
	}

	// Draw Frame
    if (m_windowStyle & BORDER_STYLE)
    {
        if ((m_updatePart & _UPDATE_CLIENT) == _UPDATE_CLIENT)
            pTheme->drawWidget(Theme::WINDOW_NORMAL, 0, 0+titleBarHeight, m_size.x(), m_size.y()-titleBarHeight);
    }

	// Clear the update part to none
	m_updatePart = 0;
}

