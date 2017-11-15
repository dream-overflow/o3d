/**
 * @file widgetmanager.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgetmanager.h"

#include "o3d/core/debug.h"
#include "o3d/gui/widgets/layout.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"
#include "o3d/engine/object/primitive.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/gui/widgets/window.h"
#include "o3d/gui/widgets/button.h"
#include "o3d/gui/widgets/tooltip.h"

#ifdef MessageBox
#undef MessageBox
#endif

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(WidgetManager, GUI_WIDGET_MANAGER, SceneEntity)

// constructor
WidgetManager::WidgetManager(BaseObject *parent) :
	SceneEntity(parent),
	m_console(nullptr),
	m_hoverWidget(nullptr),
	m_focusWidget(nullptr),
	m_focusWindow(nullptr),
    m_toolTip(nullptr),
	m_theme(nullptr),
	m_lockWidgetMouse(False),
	m_preventEvent(False)
{
	m_name = "Scene::Gui::WidgetManager";

    m_console = new Console(this);
    m_console->enable(False);
    m_console->show(False);
}

// destructor
WidgetManager::~WidgetManager()
{
    for (IT_WindowList it = m_windowList.begin(); it != m_windowList.end(); ++it)
	{
		deletePtr(*it);
	}

	// process deferred deletions too
	for (IT_WindowList it = m_deferredDeletion.begin(); it != m_deferredDeletion.end(); ++it)
    {
        deletePtr(*it);
    }

    deletePtr(m_console);
}

void WidgetManager::init()
{
    m_rootWindow = new RootWindow(
                this,
                Vector2i(0, 0),
                Vector2i((Int32)((Gui*)getScene()->getGui())->getWidth(), (Int32)((Gui*)getScene()->getGui())->getHeight()),
                "<WidgetManager::RootWindow>");
}

// delete a window
Bool WidgetManager::deleteWindow(Window *window)
{
	if (window->getParent() != this)
		O3D_ERROR(E_InvalidParameter("The given window is not a child of this manager"));

	if (window == m_hoverWidget)
		m_hoverWidget = nullptr;

	IT_WindowList it = std::find(m_windowList.begin(), m_windowList.end(), window);
	if (it != m_windowList.end())
	{
		m_windowList.erase(it);

		// focused widget
		if ((m_focusWidget != nullptr) && (m_focusWidget->getTopLevelWidget() == window))
		{
			m_lockWidgetMouse = False;

			// lost focus, and no current focused widget
			m_focusWidget->lostFocus();
			m_focusWidget = nullptr;
		}

		// lost mouse hover
		if ((m_hoverWidget != nullptr) && (m_hoverWidget->getTopLevelWidget() == window))
		{
			m_hoverWidget->mouseMoveOut();
			m_hoverWidget = nullptr;
		}

		// focused window
		if (m_focusWindow == window)
		{
			m_focusWindow->lostFocus();
			m_focusWindow = nullptr;

			// set the focus to the previous highest window
			if (!m_windowList.empty())
			{
				for (auto it2 = m_windowList.rbegin(); it2 != m_windowList.rend(); ++it2)
				{
					// next visible window
					if ((*it2)->isShown())
					{
						m_focusWindow = m_focusWidget = *it2;
						m_focusWindow->focused();

                        m_focusWidget = m_focusWindow->getPreviouslyFocusedWidget();
                        if (!m_focusWidget)
                            m_focusWidget = m_focusWindow->findNextTabIndex(nullptr, 1);
                        if (m_focusWidget && m_focusWidget != m_focusWindow)
                            m_focusWidget->focused();

						// simulate a mouse move to target a potential widget
						mouseMove(m_mousePos.x(), m_mousePos.y());

						break;
					}
				}
			}
        }

		// delete the window later to avoid invalid object in some case
		m_deferredDeletion.push_back(window);

		return True;
	}

	return False;
}

// Create a new top-level window
Window* WidgetManager::createTopLevelWindow(
	const String &title,
	const Vector2i &pos,
	const Vector2i &size,
	Window::WindowStyle style,
	const String &name,
	Bool show)
{
	Window *window = new Window(this, title, pos, size, style, name);
	O3D_ASSERT(window);

	addWindowForegound(window, show);
	return window;
}

// Create a message box.
void WidgetManager::messageBox(
	const String &name,
	const String &title,
	const String &message,
	MessageBox::ButtonsStyle buttonStyle,
	MessageBox::IconStyle iconStyle,
	Float alpha)
{
	MessageBox *messageBox = MessageBox::create(this, title, message, buttonStyle, iconStyle);
	messageBox->setName(name);
	messageBox->setAlpha(alpha);

	if (messageBox->getButton(MessageBox::BUTTON_OK))
        messageBox->getButton(MessageBox::BUTTON_OK)->onButtonPushed.connect(this, &WidgetManager::messageBoxOk);

	if (messageBox->getButton(MessageBox::BUTTON_YES))
        messageBox->getButton(MessageBox::BUTTON_YES)->onButtonPushed.connect(this, &WidgetManager::messageBoxYes);

	if (messageBox->getButton(MessageBox::BUTTON_NO))
        messageBox->getButton(MessageBox::BUTTON_NO)->onButtonPushed.connect(this, &WidgetManager::messageBoxNo);

	if (messageBox->getButton(MessageBox::BUTTON_RETRY))
        messageBox->getButton(MessageBox::BUTTON_RETRY)->onButtonPushed.connect(this, &WidgetManager::messageBoxRetry);

	if (messageBox->getButton(MessageBox::BUTTON_CANCEL))
        messageBox->getButton(MessageBox::BUTTON_CANCEL)->onButtonPushed.connect(this, &WidgetManager::messageBoxCancel);

    messageBox->onWindowClose.connect(this, &WidgetManager::messageBoxClose);

	addWindowForegound(messageBox);
}

// add a top-level widget (mainly window or dialog) to the front
void WidgetManager::addWindowForegound(Window* window, Bool show)
{
    if (window == nullptr) {
		return;
    }

    if (window->getParent() != this) {
		O3D_ERROR(E_InvalidParameter("The given window is not a child of this manager"));
    }

    if (show) {
		// current window because it is foreground and visible excepted if the current is modal,
		// or if the new window is modal
		if ((m_focusWindow == nullptr) ||
			(m_focusWindow != nullptr && !m_focusWindow->isModal()) ||
            (window->isModal()) ) {
			// widget lost focus
            if (m_focusWidget != nullptr) {
                if (m_focusWindow && m_focusWidget->getTopLevelWidget() == m_focusWindow) {
                    m_focusWindow->setPreviouslyFocusedWidget(m_focusWidget);
                }

				m_focusWidget->lostFocus();
                m_focusWidget = nullptr;
			}

            // window lost focus
            if (m_focusWindow != nullptr) {
                m_focusWindow->lostFocus();
                m_focusWindow = nullptr;
            }

            m_focusWindow = window;
            m_focusWindow->focused();

            m_focusWidget = m_focusWindow->getPreviouslyFocusedWidget();
            if (!m_focusWidget) {
                m_focusWidget = m_focusWindow->findNextTabIndex(nullptr, 1);
            }
            if (m_focusWidget && m_focusWidget != m_focusWindow) {
                m_focusWidget->focused();
            }
        }
	}

	// insert
    if (window->isAlwaysOnTop() || window->isModal() || m_windowList.empty()) {
		m_windowList.push_back(window);
    } else {
		// insert after always on top and modals
		IT_WindowList it2 = m_windowList.end();
		--it2;
        while (it2 != m_windowList.begin() && ((*it2)->isModal() || (*it2)->isAlwaysOnTop())) {
			--it2;
		}

		++it2;
		m_windowList.insert(it2, window);
	}

    if (show) {
		// simulate a mouse move
        mouseMove(m_mousePos.x(), m_mousePos.y());
	}
}

void WidgetManager::addWindowBackground(Window* window, Bool show)
{
    if (window == nullptr) {
		return;
    }

    if (window->getParent() != this) {
		O3D_ERROR(E_InvalidParameter("The given window is not a child of this manager"));
    }

    if (show) {
        // focus and current only if the window is modal
        if (window->isModal()) {
            if (m_focusWindow != nullptr) {
                // widget lost focus
                if (m_focusWidget != nullptr) {
                    if (m_focusWindow && m_focusWidget->getTopLevelWidget() == m_focusWindow)
                        m_focusWindow->setPreviouslyFocusedWidget(m_focusWidget);

                    m_focusWidget->lostFocus();
                    m_focusWidget = nullptr;
                }

				// window lost focus
				m_focusWindow->lostFocus();
				m_focusWindow = nullptr;
			}

            m_focusWindow = window;
			m_focusWindow->focused();

            m_focusWidget = m_focusWindow->getPreviouslyFocusedWidget();
            if (!m_focusWidget) {
                m_focusWidget = m_focusWindow->findNextTabIndex(nullptr, 1);
            }
            if (m_focusWidget && m_focusWidget != m_focusWindow) {
                m_focusWidget->focused();
            }
		}
	}

	// TODO position

	// add at last window
    m_windowList.push_front(window);

    if (show) {
		// simulate a mouse move to target a potential widget
        mouseMove(m_mousePos.x(), m_mousePos.y());
	}
}

// retrieve a widget by its object name
Widget* WidgetManager::findWidget(const String &name)
{
    Widget *pWidget = nullptr;

	// recursive search
    for (IT_WindowList it = m_windowList.begin(); it != m_windowList.end(); ++it) {
        if ((*it)->getName() == name) {
            return (*it);
        }

        if ((*it)->getLayout()) {
            pWidget = (*it)->getLayout()->findWidget(name);
            if (pWidget) {
                return pWidget;
            }
        }
	}

    return nullptr;
}

const Widget* WidgetManager::findWidget(const String &name) const
{
    const Widget *pWidget = nullptr;

	// recursive search
    for (CIT_WindowList it = m_windowList.begin(); it != m_windowList.end(); ++it) {
       if ((*it)->getName() == name) {
            return (*it);
       }

        if ((*it)->getLayout()) {
			pWidget = (*it)->getLayout()->findWidget(name);
            if (pWidget) {
                return pWidget;
            }
        }
	}

    return nullptr;
}

// search for an existing widget by its pointer
Bool WidgetManager::isExist(Widget* widget) const
{
	// recursive search
    for (CIT_WindowList it = m_windowList.begin(); it != m_windowList.end(); ++it) {
        if ((*it) == widget) {
            return True;
        }

        if ((*it)->getLayout() && (*it)->getLayout()->isWidgetExist(widget)) {
			return True;
        }
	}

	return False;
}

void WidgetManager::showHideWindow(Window *window, Bool show)
{
    if (!window) {
        return;
    }

    if (window->getParent() != this) {
		O3D_ERROR(E_InvalidParameter("The given window is not a child of this manager"));
    }

    if (show) {
        if (m_focusWindow == window) {
			return;
        }

		if (!m_windowList.empty())
		{
			for (auto it = m_windowList.rbegin(); it != m_windowList.rend(); ++it)
			{
				// top window
				if (*it == window)
				{
					m_lockWidgetMouse = False;
                    // TODO show does not include specialy focus

                    if ((m_focusWidget != nullptr))// why that ? && window->isModal())
					{					
                        if (m_focusWindow && m_focusWidget->getTopLevelWidget() == m_focusWindow)
                            m_focusWindow->setPreviouslyFocusedWidget(m_focusWidget);

                        m_focusWidget->lostFocus();
						m_focusWidget = nullptr;
					}

					if (m_focusWindow != nullptr)
					{
						m_focusWindow->lostFocus();
						m_focusWindow = nullptr;
					}					

                    m_focusWindow = window;
					m_focusWindow->focused();

                    m_focusWidget = m_focusWindow->getPreviouslyFocusedWidget();
                    if (!m_focusWidget)
                        m_focusWidget = m_focusWindow->findNextTabIndex(nullptr, 1);
                    if (m_focusWidget && m_focusWidget != m_focusWindow)
                        m_focusWidget->focused();

					// simulate a mouse move
					mouseMove(m_mousePos.x(), m_mousePos.y());

					return;
				}
			}
		}
	}
	else
	{
		// lost keyboard focus
        if ((m_focusWidget != nullptr) && (m_focusWidget->getTopLevelWidget() == window))
		{
            window->setPreviouslyFocusedWidget(m_focusWidget);

			m_lockWidgetMouse = False;

			m_focusWidget->lostFocus();
			m_focusWidget = nullptr;
		}

		// lost mouse hover
        if ((m_hoverWidget != nullptr) && (m_hoverWidget->getTopLevelWidget() == window))
		{
			m_hoverWidget->mouseMoveOut();
			m_hoverWidget = nullptr;
		}

		// top level window lost keyboard focus
		if (m_focusWindow == window)
		{
			m_focusWindow->lostFocus();
			m_focusWindow = nullptr;
		}

		// set the focus to the next highest window
		if (!m_windowList.empty())
		{
			for (auto it = m_windowList.rbegin(); it != m_windowList.rend(); ++it)
			{
				// next visible window
				if ((*it)->isShown())
				{
                    m_focusWindow = *it;
					m_focusWindow->focused();

                    m_focusWidget = m_focusWindow->getPreviouslyFocusedWidget();
                    if (!m_focusWidget)
                        m_focusWidget = m_focusWindow->findNextTabIndex(nullptr, 1);
                    if (m_focusWidget && m_focusWidget != m_focusWindow)
                        m_focusWidget->focused();

					break;
				}
			}
		}

		// simulate a mouse move
		mouseMove(m_mousePos.x(), m_mousePos.y());
	}
}

// set a widget always on top
void WidgetManager::setAlwaysOnTop(Window* window, Bool onTop)
{
	if ((window != nullptr) && !window->isAlwaysOnTop() && !window->isModal())
	{
		if (onTop)
		{
			// already on top
			if (!m_windowList.empty() && m_windowList.front() == window)
				return;

			IT_WindowList it = std::find(m_windowList.begin(), m_windowList.end(), window);
			if (it != m_windowList.end())
			{
				// put on top
				m_windowList.erase(it);
				m_windowList.push_back(window);

				// focus only if necessary
				if (m_focusWindow != window)
				{
                    if (m_focusWidget)
                    {
                        if (m_focusWindow && m_focusWidget->getTopLevelWidget() == m_focusWindow)
                            m_focusWindow->setPreviouslyFocusedWidget(m_focusWidget);

                        m_focusWidget->lostFocus();
                        m_focusWidget = nullptr;
                    }

                    m_focusWindow = window;
					m_focusWindow->focused();

                    m_focusWidget = m_focusWindow->getPreviouslyFocusedWidget();
                    if (!m_focusWidget)
                        m_focusWidget = m_focusWindow->findNextTabIndex(nullptr, 1);
                    if (m_focusWidget && m_focusWidget != m_focusWindow)
                        m_focusWidget->focused();
				}

				// simulate a mouse move
                mouseMove(m_mousePos.x(), m_mousePos.y());
			}
		}
	}
}

// set the widget which have the keyboard focus
void WidgetManager::setFocus(Widget* widget)
{
    // special case for top-level widget
    if (widget && widget->isTopLevelWidget())
    {
        Window *topLevelWidget = (Window*)widget;

        // focus to the window if the current window is different from the top window
        // where the focused widget belong to
        if (topLevelWidget != m_focusWindow)
        {
            // prev widget lost focus
            if (m_focusWidget)
            {
                if (m_focusWindow && m_focusWidget->getTopLevelWidget() == m_focusWindow)
                    m_focusWindow->setPreviouslyFocusedWidget(m_focusWidget);

                m_focusWidget->lostFocus();
                m_focusWidget = nullptr;
            }

            // prev window lost focus
            if (m_focusWindow != nullptr)
            {
                m_focusWindow->lostFocus();
            }

            IT_WindowList it = std::find(m_windowList.begin(), m_windowList.end(), topLevelWidget);
            if (it != m_windowList.end())
            {
                m_windowList.erase(it);

                // if always on top, so simply move it at back
                if (widget->isModal() || widget->isAlwaysOnTop() || m_windowList.empty())
                {
                    m_windowList.push_back(topLevelWidget);
                    m_focusWindow = topLevelWidget;

                    // focus the window first
                    m_focusWindow->focused();
                }
                else
                {
                    // insert after always on top and modals
                    IT_WindowList it2 = m_windowList.end();
                    --it2;
                    while (it2 != m_windowList.begin() && ((*it2)->isModal() || (*it2)->isAlwaysOnTop()))
                    {
                        --it2;
                    }

                    ++it2;
                    m_windowList.insert(it2, topLevelWidget);
                    m_focusWindow = topLevelWidget;

                    // focus the window first
                    m_focusWindow->focused();
                }
            }

            m_focusWidget = m_focusWindow->getPreviouslyFocusedWidget();
            if (!m_focusWidget)
                m_focusWidget = m_focusWindow->findNextTabIndex(nullptr, 1);
            if (m_focusWidget)
                m_focusWidget->focused();
        }

        return;
    }

	// move the focused widget, only if it is different from previous
	if (m_focusWidget != widget)
	{
        Widget *prevWidget = m_focusWidget;

		// the old focused widget lost the focus
		if (m_focusWidget != nullptr)
        {
			m_focusWidget->lostFocus();
            m_focusWidget = nullptr;
        }

        // only valid and focusable widget
        if (!widget)
			return;

        // get the toplevel widget
        Window *topLevelWidget = (Window*)widget->getTopLevelWidget();

        // find a focusable widget if necessary
        if (!widget->isFocusable())
        {
            widget = topLevelWidget->getPreviouslyFocusedWidget();
            if (!widget)
                widget = topLevelWidget->findNextTabIndex(nullptr, 1);
        }

		// focus to the window if the current window is different from the top window
		// where the focused widget belong to
		if (topLevelWidget != m_focusWindow)
		{
			// prev window lost focus
			if (m_focusWindow != nullptr)
				m_focusWindow->lostFocus();

            // prev widget lost focus
            if (prevWidget && m_focusWindow && prevWidget->getTopLevelWidget() == m_focusWindow)
                m_focusWindow->setPreviouslyFocusedWidget(prevWidget);

			IT_WindowList it = std::find(m_windowList.begin(), m_windowList.end(), topLevelWidget);
			if (it != m_windowList.end())
			{
				m_windowList.erase(it);

				// if always on top, so simply move it at back
				if (widget->isModal() || widget->isAlwaysOnTop() || m_windowList.empty())
				{
					m_windowList.push_back(topLevelWidget);
					m_focusWindow = topLevelWidget;

					// focus the window first
					m_focusWindow->focused();
				}
				else
				{
					// insert after always on top and modals
					IT_WindowList it2 = m_windowList.end();
					--it2;
					while (it2 != m_windowList.begin() && ((*it2)->isModal() || (*it2)->isAlwaysOnTop()))
					{
						--it2;
					}

					++it2;
					m_windowList.insert(it2, topLevelWidget);
					m_focusWindow = topLevelWidget;

					// focus the window first
					m_focusWindow->focused();
				}
			}
		}

		m_focusWidget = widget;

		// the new focused widget obtain the focus
        if (m_focusWidget != nullptr)
			m_focusWidget->focused();
	}
}

// Get widget targeted by mouse
Widget* WidgetManager::getTargetedWidget(Int32 x, Int32 y)
{
	Bool widgetMatch = False;
    Widget *pWidget = nullptr;

	RIT_WindowList it = m_windowList.rbegin();
	while (!widgetMatch && it != m_windowList.rend())
	{
		// only if the widget is visible
		if ((*it)->isShown())
		{
			// getting widget's relative coordinates
			if ((*it)->isTargeted(
                    x - (*it)->pos().x(),
                    y - (*it)->pos().y(),
					pWidget))
			{
				widgetMatch = True;
				break;
			}
		}

		// break on visible modal window
		if ((*it)->isModal() && (*it)->isShown())
			break;

		++it;
	}

	if (widgetMatch)
		return pWidget;
	else
        return nullptr;
}

// switch the focus between the console and others widgets
void WidgetManager::setFocusToConsole()
{
    m_console->enable();
    setFocus(nullptr);
}

// switch the focus between the console and others widgets
void WidgetManager::setFocusToWidgets()
{
    m_console->enable(False);
}

// lock the mouse into the current targeted widget
void WidgetManager::lockWidgetMouse()
{
    if (m_lockWidgetMouse)
        return;

    m_lockWidgetMouse = True;

    // lost focus during mouse action
    if (m_focusWidget && m_focusWidget->getTopLevelWidget() == m_focusWindow) {
        m_focusWidget->lostFocus();
    }
}

// unlock the mouse from the current targeted widget.
void WidgetManager::unlockWidgetMouse()
{
    if (!m_lockWidgetMouse) {
        return;
    }

	m_lockWidgetMouse = False;

	// simulate a mouse move to target widget
    mouseMove(m_mousePos.x(), m_mousePos.y());

    // regain focus after mouse action
    if (m_focusWidget && m_focusWidget->getTopLevelWidget() == m_focusWindow)
        m_focusWidget->focused();
}

void WidgetManager::preventEvent(Bool prevent)
{
	if (m_preventEvent != prevent)
	{
		m_preventEvent = prevent;

		// simulate a mouse move to target widget
		if (prevent == False)
		{
            mouseMove(m_mousePos.x(), m_mousePos.y());
		}
		else if (m_hoverWidget != nullptr)
		{
			// mouse out the widget
			m_hoverWidget->mouseMoveOut();
			m_hoverWidget = nullptr;
		}
	}
}

// Is a targeted widget.
Bool WidgetManager::isTargetedWidget()
{
	if (m_preventEvent)
		return False;

    return getTargetedWidget(m_mousePos.x(), m_mousePos.y()) != nullptr;
}

Bool WidgetManager::isHoverWidget()
{
    RIT_WindowList it = m_windowList.rbegin();
    while (it != m_windowList.rend())
    {
        // only if the widget is visible
        if ((*it)->isShown())
        {
            Box2i box((*it)->pos(), (*it)->size());

            // getting widget's relative coordinates
            if (box.isInside(m_mousePos))
                return True;
        }

        ++it;
    }

    return False;
}

//
// events
//

void WidgetManager::mouseButtonDown(Mouse::Buttons button, Int32 x, Int32 y)
{
    if (m_preventEvent)
        return;

    Widget* widget = m_hoverWidget;

    // this mouse event cancel tooltip
    m_toolTip = nullptr;

    // if the widget was hidden since the last operation
    if (widget && !widget->isShown())
    {
        m_hoverWidget = nullptr;
        widget = nullptr;

        m_lockWidgetMouse = False;
    }

    // check for the targeted widget at mouse(x,y) coordinates
    if (!m_lockWidgetMouse)
        widget = getTargetedWidget(x,y);

    // send the mouse event with local coordinates
    if (widget)
    {
        Vector2i pos;

        // bubbling
        Widget *currWidget = widget;
        while (currWidget != nullptr)
        {
            if (currWidget->getParentWidget())
                pos = currWidget->getParentWidget()->getAbsPosOrigin();// - currWidget->getParentWidget()->getScrollPos();

            if ((button == Mouse::LEFT) && currWidget->mouseLeftPressed(x-pos.x(),y-pos.y()))
                break;
            else if ((button == Mouse::MIDDLE) && currWidget->mouseMiddlePressed(x-pos.x(),y-pos.y()))
                break;
            else if ((button == Mouse::RIGHT) && currWidget->mouseRightPressed(x-pos.x(),y-pos.y()))
                break;
            else if ((button == Mouse::X1) && currWidget->mouseX1Pressed(x-pos.x(),y-pos.y()))
                break;
            else if ((button == Mouse::X2) && currWidget->mouseX2Pressed(x-pos.x(),y-pos.y()))
                break;

            currWidget = currWidget->getParentWidget();
        }

        // set the focused widget if the console is not currently visible
        if (!m_console->isShown())
            setFocus(widget);
    }

}

void WidgetManager::mouseButtonUp(Mouse::Buttons button, Int32 x, Int32 y)
{
    if (m_preventEvent)
        return;

    if (m_console->isActive())
    {
        if (button == Mouse::MIDDLE)
            m_console->paste(True);
    }

    Widget* widget = m_hoverWidget;

    // if the widget was hidden since the last operation
    if (widget && !widget->isShown())
    {
        m_hoverWidget = nullptr;
        widget = nullptr;

        m_lockWidgetMouse = False;
    }

    if (!m_lockWidgetMouse)
        widget = getTargetedWidget(x,y);

    if (widget)
    {
        Vector2i pos;

        // bubbling
        Widget *currWidget = widget;
        while (currWidget != nullptr)
        {
            if (currWidget->getParentWidget())
                pos = currWidget->getParentWidget()->getAbsPosOrigin();// - currWidget->getParentWidget()->getScrollPos();

            if ((button == Mouse::LEFT) && currWidget->mouseLeftReleased(x-pos.x(),y-pos.y()))
                break;
            else if ((button == Mouse::MIDDLE) && currWidget->mouseMiddleReleased(x-pos.x(),y-pos.y()))
                break;
            else if ((button == Mouse::RIGHT) && currWidget->mouseRightReleased(x-pos.x(),y-pos.y()))
                break;
            else if ((button == Mouse::X1) && currWidget->mouseX1Released(x-pos.x(),y-pos.y()))
                break;
            else if ((button == Mouse::X2) && currWidget->mouseX2Released(x-pos.x(),y-pos.y()))
                break;

            // paste processing
            if ((button == Mouse::MIDDLE) && currWidget->isPastable() && currWidget->paste(True))
                break;

            // or finally
            currWidget = currWidget->getParentWidget();
        }
    }
}

void WidgetManager::mouseWheel(Int32 x, Int32 y, Int32 z)
{
	if (m_preventEvent)
		return;

	Widget* widget = m_hoverWidget;

	// if the widget was hidden since the last operation
	if (widget && !widget->isShown())
	{
        m_hoverWidget = nullptr;
        widget = nullptr;

		m_lockWidgetMouse = False;
	}

	if (!m_lockWidgetMouse)
		widget = getTargetedWidget(x,y);

	if (widget)
	{
		Vector2i pos;

		// bubbling
		Widget *currWidget = widget;
		while (currWidget != nullptr)
		{
			if (currWidget->getParentWidget())
				pos = currWidget->getParentWidget()->getAbsPosOrigin();// - currWidget->getParentWidget()->getScrollPos();

            if (currWidget->mouseWheel(x-pos.x(),y-pos.y(), z))
				break;

			currWidget = currWidget->getParentWidget();
		}
	}
}

void WidgetManager::mouseMove(Int32 x,Int32 y)
{
	m_mousePos.set(x, y);

	if (m_preventEvent)
		return;

	Widget* widget = m_hoverWidget;

	// if the widget was hidden since the last operation
	if (widget && !widget->isShown())
	{
        m_hoverWidget = nullptr;
        widget = nullptr;

		m_lockWidgetMouse = False;
	}

	if (!m_lockWidgetMouse)
		widget = getTargetedWidget(x,y);

	if (widget != m_hoverWidget)
	{
		if (m_hoverWidget)
        {
			m_hoverWidget->mouseMoveOut();
            m_toolTip = nullptr;
        }

		if (widget)
        {
			widget->mouseMoveIn();

            // tool tip
            if (m_toolTip != widget->getToolTip())
            {
                // reset the tool tip time stamp
                m_toolTipTimeStamp = System::getMsTime();

                m_toolTip = widget->getToolTip();

                if (m_toolTip != nullptr)
                    m_toolTip->show(False);
            }
        }
	}

	if (widget)
	{
		Vector2i pos;

		// bubbling
		Widget *currWidget = widget;
		while (currWidget != nullptr)
		{
			if (currWidget->getParentWidget())
				pos = currWidget->getParentWidget()->getAbsPosOrigin();// - currWidget->getParentWidget()->getScrollPos();

            if (currWidget->mouseMove(x-pos.x(),y-pos.y()))
				break;

			currWidget = currWidget->getParentWidget();
		}

		// mouse cursor
		if (m_theme)
			m_theme->getCursorManager().setCursor(widget->getCursorTypeName(), 32);

        // reset the tool tip time stamp
        if ((m_toolTip != nullptr) && (!m_toolTip->isShown()))
            m_toolTipTimeStamp = System::getMsTime();
	}
	else
	{
		// default mouse cursor
		if (m_theme)
			m_theme->getCursorManager().setCursor("Default", 32);

        m_toolTip = nullptr;
	}

	m_hoverWidget = widget;
}

void WidgetManager::keyboardToggled(Keyboard *keyboard, KeyEvent event)
{
	if (m_preventEvent)
		return;

	// set the keyboard on the console...
    if (m_console->isActive())
	{
        m_console->keyboardToggled(keyboard, event);
	}
	else if (m_focusWidget)
	{
		// send to the focused widget... and bubbling
		Widget *currWidget = m_focusWidget;
		while (currWidget != nullptr && !currWidget->keyboardToggled(keyboard, event))
		{
			currWidget = currWidget->getParentWidget();
		}

		// widget manager send a onKey signal
		if (currWidget == nullptr)
        {
            onKey(keyboard, event);

            // tab-index
            if ((event.key() == KEY_TAB) && (event.isPressed() || event.isRepeat()))
            {
                Int32 dir = 1;

                // set the focus to the next tab index (or previous if shift is down)
                if (keyboard->isKeyDown(KEY_LSHIFT) || keyboard->isKeyDown(KEY_RSHIFT))
                    dir = -1;

                Widget *next = m_focusWindow->findNextTabIndex(m_focusWidget, dir);
                if (next)
                    setFocus(next);
            }
        }
	}
}

void WidgetManager::character(Keyboard *keyboard, CharacterEvent event)
{
	if (m_preventEvent)
		return;

	// set the keyboard on the console
    if (m_console->isActive())
    {
        m_console->character(keyboard, event);

        // special commands
        if (keyboard->isKeyDown(KEY_LCONTROL) || keyboard->isKeyDown(KEY_RCONTROL))
        {
            // cut processing
            if (event.character() == KEY_X)
                m_console->cut(False);
            // copy processing
            if (event.character() == KEY_C)
                m_console->copy(False);
            // paste processing
            else if (event.character() == KEY_V)
                m_console->paste(False);
        }
    }
	else if (m_focusWidget)
	{
		// send to the focused widget... and bubbling
		Widget *currWidget = m_focusWidget;
		while (currWidget != nullptr && !currWidget->character(keyboard, event))
		{
			currWidget = currWidget->getParentWidget();
		}

        // special commands
        if (keyboard->isKeyDown(KEY_LCONTROL) || keyboard->isKeyDown(KEY_RCONTROL))
        {
            // cut processing
            if ((event.character() == KEY_X) && m_focusWidget->isCopyable())
                m_focusWidget->cut(False);
            // copy processing
            if ((event.character() == KEY_C) && m_focusWidget->isCopyable())
                m_focusWidget->copy(False);
            // paste processing
            else if ((event.character() == KEY_V) && m_focusWidget->isPastable())
                m_focusWidget->paste(False);
        }
    }
}

void WidgetManager::reshape(UInt32 width, UInt32 height)
{
    if (m_rootWindow.isValid())
    {
        m_rootWindow->setSize(Vector2i(width, height));
        m_rootWindow->getLayout()->setSize(Vector2i(width, height));
    }
}

// draw the widgets tree
void WidgetManager::draw()
{
    getScene()->getContext()->setAntiAliasing(Context::AA_NONE);

    // background
    if (m_rootWindow.isValid() && m_rootWindow->isShown())
    {
        getScene()->getContext()->modelView().identity();
        m_rootWindow->draw();
    }

	// draw all widgets
	for (IT_WindowList it = m_windowList.begin(); it != m_windowList.end(); ++it)
	{
		if ((*it)->isShown())
		{
			getScene()->getContext()->modelView().identity();
			(*it)->draw();
		}
	}

    // draw tool tip
    UInt32 currTime = System::getMsTime();

    if ((currTime - m_toolTipTimeStamp) >= ToolTip::DELAY_BEFORE_SHOW)
    {
        // the hover widget was replaced or deleted
        if (m_toolTip != nullptr && m_hoverWidget != nullptr && m_toolTip != m_hoverWidget->getToolTip())
        {
            m_toolTip = nullptr;
        }

        if ((m_toolTip != nullptr) && !m_toolTip->isShown())
        {
            // set the tool tip position near the widget
            Vector2i guiSize(((Gui*)getScene()->getGui())->getWidth(), ((Gui*)getScene()->getGui())->getHeight());
            Box2i box(m_mousePos, m_toolTip->size());

            box.y() += getDefaultTheme()->getCursorManager().getCursorSize();

            if (box.x2() > guiSize.x())
                box.x() -= box.x2() - guiSize.x();

            if (box.y2() > guiSize.y())
                box.y() -= box.y2() - guiSize.y();

            m_toolTip->setPos(box.pos() - m_toolTip->getOrigin());

            // and show it
            m_toolTip->show();
        }
    }

    if ((m_toolTip != nullptr) && m_toolTip->isShown())
    {
        getScene()->getContext()->modelView().identity();
        m_toolTip->draw();
    }

	// draw the console
    if (m_console->isShown())
	{
		getScene()->getContext()->modelView().identity();
        m_console->draw();
	}
	
	// draw the mouse
	if (m_theme)
    {
		getScene()->getContext()->modelView().identity();
		m_theme->getCursorManager().draw(Vector2i(m_mousePos.x(), m_mousePos.y()), 1.f);
	}
	
	getScene()->getContext()->setDefaultAntiAliasing();
}

void WidgetManager::update()
{
	// process deferred deletions
	if (!m_deferredDeletion.empty())
	{
		for (IT_WindowList it = m_deferredDeletion.begin(); it != m_deferredDeletion.end(); ++it)
			deletePtr(*it);

		m_deferredDeletion.clear();
	}

    // update the widget on focus
    if (m_focusWidget != nullptr)
        m_focusWidget->update();
}

void WidgetManager::checkPrevTargetedWidget(Widget *prevWidget)
{
    if (m_hoverWidget == prevWidget)
    {
//        if (m_hoverWidget->getParentWidget())
//        {
//            m_hoverWidget = m_hoverWidget->getParentWidget();
//            m_hoverWidget->mouseMoveIn();
//        }
//        else
            m_hoverWidget = nullptr;

        m_lockWidgetMouse = False;
    }

    if (m_focusWidget == prevWidget)
    {
//        if (m_focusWidget->getParentWidget())
//        {
//            m_focusWidget = m_focusWidget->getParentWidget();
//            m_focusWidget->focused();
//        }
//        else
            m_focusWidget = nullptr;
    }

    // simulate a mouse move to target a potential widget
 //   mouseMove(m_mousePos.x(), m_mousePos.y());
}

void WidgetManager::messageBoxClose()
{
	Button *sender = static_cast<Button*>(getSender());
	MessageBox *window = static_cast<MessageBox*>(sender->getTopLevelWidget());

	String name = window->getName();

	// cancel/yes.no.cancel/retry.cancel message box, then cancel
	if (window->getButton(MessageBox::BUTTON_CANCEL) != nullptr)
		onMessageBoxResult(name, MessageBox::BUTTON_CANCEL);
	// yes.no message box, then no
	else if (window->getButton(MessageBox::BUTTON_NO) != nullptr)
		onMessageBoxResult(name, MessageBox::BUTTON_NO);
	// ok message box, then ok
	else if (window->getButton(MessageBox::BUTTON_OK) != nullptr)
		onMessageBoxResult(name, MessageBox::BUTTON_OK);
	else
		O3D_ASSERT(False);

	deleteWindow(window);
}

void WidgetManager::messageBoxOk()
{
	Button *sender = static_cast<Button*>(getSender());
	Window *window = static_cast<Window*>(sender->getTopLevelWidget());

	String name = window->getName();

	onMessageBoxResult(name, MessageBox::BUTTON_OK);

	deleteWindow(window);
}

void WidgetManager::messageBoxCancel()
{
	Button *sender = static_cast<Button*>(getSender());
	Window *window = static_cast<Window*>(sender->getTopLevelWidget());

	String name = window->getName();

	onMessageBoxResult(name, MessageBox::BUTTON_CANCEL);

	deleteWindow(window);
}

void WidgetManager::messageBoxRetry()
{
	Button *sender = static_cast<Button*>(getSender());
	Window *window = static_cast<Window*>(sender->getTopLevelWidget());

	String name = window->getName();

	onMessageBoxResult(name, MessageBox::BUTTON_RETRY);

	deleteWindow(window);
}

void WidgetManager::messageBoxYes()
{
	Button *sender = static_cast<Button*>(getSender());
	Window *window = static_cast<Window*>(sender->getTopLevelWidget());

	String name = window->getName();

	onMessageBoxResult(name, MessageBox::BUTTON_YES);

	deleteWindow(window);
}

void WidgetManager::messageBoxNo()
{
	Button *sender = static_cast<Button*>(getSender());
	Window *window = static_cast<Window*>(sender->getTopLevelWidget());

	String name = window->getName();

	onMessageBoxResult(name, MessageBox::BUTTON_NO);

	deleteWindow(window);
}

