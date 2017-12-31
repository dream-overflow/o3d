/**
 * @file menuwidget.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/menuwidget.h"
#include "o3d/gui/widgetdrawmode.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/gui/widgets/boxlayout.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"

// @todo a separate process of layout to adjust the width when an item is removed, to
// the largest item and not as actually to the largest from the creation of the menu

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(MenuLabelItem, GUI_WIDGET_MENU_LABEL, Widget)

MenuLabelItem::MenuLabelItem(BaseObject *parent) :
	Widget(parent)
{
}

MenuLabelItem::MenuLabelItem(
		Widget *parent,
		const String &label,
		const Vector2i &pos,
		const Vector2i &size,
		const String &name) :
	Widget(parent, pos, size, name)
{
	const Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::MENU_LABEL_ITEM_NORMAL);

	m_text.setHorizontalAlignment(TextZone::LEFT_ALIGN);
	m_text.setVerticalAlignment(TextZone::MIDDLE_ALIGN);

	// text color and size
	m_text.setText(label);
	m_text.setColor(drawMode->getDefaultTextColor());

	adjustBestSize();
}

MenuLabelItem::~MenuLabelItem()
{

}

Vector2i MenuLabelItem::getDefaultSize()
{
	return getUsedTheme()->getDefaultWidgetSize(Theme::MENU_LABEL_ITEM_NORMAL);
}

void MenuLabelItem::adjustBestSize()
{
	// calc the menu size
	Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::MENU_LABEL_ITEM_NORMAL);

	Vector2i defaultSize = getDefaultSize();

	Vector2i textSize = m_text.sizeOf(
		theme->getDefaultFont(),
		theme->getDefaultFontSize());

	m_text.setSize(textSize);

	textSize += drawMode->getTopLeftMargin() + drawMode->getBottomRightMargin();

    if (m_minSize.x() < 0) {
        m_minSize.x() = o3d::max(textSize.x(), defaultSize.x());
    }

    if (m_minSize.y() < 0) {
        m_minSize.y() = o3d::max(textSize.y(), defaultSize.y());
    }

	m_size = m_minSize;
	m_text.setPos(drawMode->getTopLeftMargin());
}

void MenuLabelItem::enable(Bool active)
{
    if (m_capacities.getBit(STATE_ACTIVITY) == active) {
		return;
    }

	m_capacities.setBit(STATE_ACTIVITY, active);

	const Theme *theme = getUsedTheme();

	// text color depend of the state
    if (active) {
		m_text.setColor(theme->getWidgetDrawMode(Theme::MENU_LABEL_ITEM_NORMAL)->getDefaultTextColor());
    } else {
		m_text.setColor(theme->getWidgetDrawMode(Theme::MENU_LABEL_ITEM_DISABLED)->getDefaultTextColor());
    }
}

Bool MenuLabelItem::isTargeted(Int32 x, Int32 y, Widget *&pWidget)
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

void MenuLabelItem::sizeChanged()
{
	const Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::MENU_LABEL_ITEM_NORMAL);

	m_text.setSize(m_size - (drawMode->getTopLeftMargin() + drawMode->getBottomRightMargin()));

	// update is needed
	setDirty();
}

// Mouse events
Bool MenuLabelItem::mouseLeftPressed(Int32 x,Int32 y)
{
	if (m_capacities.getBit(STATE_HOVER))
	{
		setDirty();

		// signal
		onSelectMenu();
	}

	// up to the menu widget
	return False;
}

Bool MenuLabelItem::mouseLeftReleased(Int32 x,Int32 y)
{
	return False;
}

Bool MenuLabelItem::mouseMove(Int32 x,Int32 y)
{
/*	x -= m_pos.x();
    y -= m_pos.y();

	// no negative part
	if (x < 0 || y < 0)
		return True;

	if (!isActive())
		return False;

    if ((x < m_size.x()) && (y < m_size.y()))
	{
		return True;
	}
*/
	return False;
}

void MenuLabelItem::mouseMoveIn()
{
	if (isActive())
	{
		m_capacities.enable(STATE_HOVER);
        m_text.setColor(getUsedTheme()->getWidgetDrawMode(Theme::MENU_LABEL_ITEM_HOVER)->getDefaultTextColor());

		setDirty();

		// signal
		onEnterWidget();
	}
}

void MenuLabelItem::mouseMoveOut()
{
	if (isActive())
	{
		m_capacities.disable(STATE_HOVER);
		m_text.setColor(getUsedTheme()->getWidgetDrawMode(Theme::MENU_LABEL_ITEM_NORMAL)->getDefaultTextColor());

		setDirty();

		// signal
		onLeaveWidget();
	}
}

Bool MenuLabelItem::keyboardToggled(Keyboard *keyboard, KeyEvent event)
{
	if (!m_capacities.getBit(STATE_FOCUSED))
		return False;

	// press the menu
	if (event.isPressed() && m_capacities.getBit(STATE_HOVER) &&
		((event.key() == KEY_RETURN) || (event.key() == KEY_NUMPAD_ENTER)))
	{
		setDirty();

		// signal
		onSelectMenu();

		return True;
	}

	return False;
}

//---------------------------------------------------------------------------------------
// draw
//---------------------------------------------------------------------------------------
void MenuLabelItem::draw()
{
	if (isShown())
		processCache();
}

void MenuLabelItem::updateCache()
{
	Theme *theme = getUsedTheme();

	// Draw label hover
	if (!isActive())
        theme->drawWidget(Theme::MENU_LABEL_ITEM_DISABLED, 0, 0, m_size.x(), m_size.y());
	else if (m_capacities.getBit(STATE_HOVER))
        theme->drawWidget(Theme::MENU_LABEL_ITEM_HOVER, 0, 0, m_size.x(), m_size.y());
	else
        theme->drawWidget(Theme::MENU_LABEL_ITEM_NORMAL, 0, 0, m_size.x(), m_size.y());

	m_text.write(
		theme->getDefaultFont(),
		theme->getDefaultFontSize());
}

//---------------------------------------------------------------------------------------
// MenuWidget
//---------------------------------------------------------------------------------------

O3D_IMPLEMENT_DYNAMIC_CLASS1(MenuWidget, GUI_WIDGET_MENU, Widget)

MenuWidget::MenuWidget(BaseObject *parent) :
	Widget(parent),
	m_menuWidgetStyle(DEFAULT_STYLE),
	m_vertScrollbar(nullptr)
{
}

MenuWidget::MenuWidget(Widget *parent,
		const Vector2i &pos,
		const Vector2i &size,
		MenuWidget::MenuWidgetStyle style,
		const String &name) :
	Widget(
		parent,
		pos,
		size,
		name),
	m_menuWidgetStyle(style),
	m_vertScrollbar(nullptr)
{
	init();
}

MenuWidget::~MenuWidget()
{
	deletePtr(m_vertScrollbar);

	for (Widget *elt : m_sonList)
	{
		deletePtr(elt);
	}

	m_sonList.clear();
}

void MenuWidget::init()
{
	Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::WINDOW_NORMAL);

	// calc the default listbox size
	Vector2i defaultSize = getDefaultSize();

    if (m_minSize.x() < 0)
        m_minSize.x() = defaultSize.x();

    if (m_minSize.y() < 0)
        m_minSize.y() = defaultSize.y();

	// max size is limited by the gui viewport
    m_maxSize.set(((Gui*)getScene()->getGui())->getWidth(), ((Gui*)getScene()->getGui())->getHeight());

	// initial size is min size
	m_size = m_minSize;
	m_limits = m_size;
	m_virtualSize.set(0, 0);

	// borders
	m_limits.x() -= drawMode->getLeftMargin() + drawMode->getRightMargin();
	m_limits.y() -= drawMode->getTopMargin() + drawMode->getBottomMargin();

	// vertical scrollbar
	Vector2i pos(m_limits.x() - getUsedTheme()->getDefaultWidgetSize(Theme::SCROLL_BAR_BORDER).x(), 0);
	Vector2i size(-1, m_limits.y());

	m_vertScrollbar = new LayoutScrollBar(
				this,
				ScrollBar::VERTICAL_ALIGN,
				pos,
				size);

	m_vertScrollbar->setPageStep(m_limits.y() / 10);
	m_vertScrollbar->setSingleStep(1);

	m_vertScrollbar->show(False);
}

void MenuWidget::updateScrollbar()
{
	if (m_virtualSize.y() > m_limits.y())
	{
		m_vertScrollbar->setRange(0, m_virtualSize.y() - m_limits.y());

		if (!m_vertScrollbar->isShown())
		{
			m_limits.x() -= m_vertScrollbar->size().x();
			m_vertScrollbar->show(True);
		}
	}
	else
	{
		m_vertScrollbar->setRange(0, 0);

		if (m_vertScrollbar->isShown())
		{
			m_limits.x() += m_vertScrollbar->size().x();
			m_vertScrollbar->show(False);
		}
	}
}

Vector2i MenuWidget::getScrollPos() const
{
	Vector2i scrollPos;

	if (m_vertScrollbar->isShown())
		scrollPos.y() = m_vertScrollbar->getSliderPos();

	return scrollPos;
}

Vector2i MenuWidget::getOrigin() const
{
	const Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::WINDOW_NORMAL);

    return drawMode->getTopLeftMargin();
}

Vector2i MenuWidget::getDefaultSize()
{
	Vector2i size = getUsedTheme()->getDefaultWidgetSize(Theme::WINDOW_NORMAL);

	// plus scrollbar
	size.x() += getUsedTheme()->getDefaultWidgetSize(Theme::SCROLL_BAR_BORDER).x();
	size.y() += getUsedTheme()->getDefaultWidgetSize(Theme::SCROLL_BAR_BORDER).y();

	return size;
}

Bool MenuWidget::isTargeted(Int32 x, Int32 y, Widget *&pWidget)
{
	Vector2i scrollPos = getScrollPos();
	Widget *widget;

	// no negative part
	if (x < 0 || y < 0)
		return False;

    if ((x < m_size.x()) && (y < m_size.y()))
	{
		// others children widgets
		Vector2i org = getOrigin();
		pWidget = this;

		// delta by the client origin for children widgets
        x -= org.x();
        y -= org.y();

		// vertical scrollbar
		if (m_vertScrollbar->isShown())
		{
			if (m_vertScrollbar->isTargeted(
                        x - m_vertScrollbar->pos().x(),
                        y - m_vertScrollbar->pos().y(),
						pWidget))
			{
				return True;
			}
		}

		// recursive targeted
		for (IT_MenuItemList it = m_sonList.begin(); it != m_sonList.end(); ++it)
		{
			widget = *it;

			if (widget->isTargeted(
                    x - widget->pos().x() + scrollPos.x(),
                    y - widget->pos().y() + scrollPos.y(),
					pWidget))

				return True;
		}

		return True;
	}

	return False;
}

Bool MenuWidget::mouseLeftPressed(Int32 x, Int32 y)
{
	// others children widgets
	Widget *widget = this;
	Widget *pWidget = nullptr;

	Vector2i scrollPos = getScrollPos();

	x -= m_pos.x();
	y -= m_pos.y();

	// for each elements
	for (IT_MenuItemList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		widget = *it;

		if (widget->isTargeted(
                x - widget->pos().x() + scrollPos.x(),
                y - widget->pos().y() + scrollPos.y(),
				pWidget))
		{
			// signal
			onSelectMenuItem(widget);

			return True;
		}
	}

	return True;
}

Bool MenuWidget::mouseLeftReleased(Int32 x, Int32 y)
{
	return True;
}

Bool MenuWidget::mouseMove(Int32 x, Int32 y)
{
	return False;
}

void MenuWidget::mouseMoveIn()
{

}

void MenuWidget::mouseMoveOut()
{

}

Bool MenuWidget::mouseWheel(Int32 x, Int32 y, Int32 z)
{
	if (z > 0)
	{
		m_vertScrollbar->setSliderPos(m_vertScrollbar->getSliderPos()+m_vertScrollbar->getSingleStep());
		return True;
	}
	else if (z < 0)
	{
		m_vertScrollbar->setSliderPos(m_vertScrollbar->getSliderPos()-m_vertScrollbar->getSingleStep());
		return True;
	}

	return False;
}

Bool MenuWidget::keyboardToggled(Keyboard *keyboard, KeyEvent event)
{
	if (!m_capacities.getBit(STATE_FOCUSED))
		return False;

	// menu exit
	if (event.isPressed() && (event.key() == KEY_ESCAPE))
	{
		return False;
	}

	// select prev
	else if (event.isPressed() && (event.key() == KEY_UP))
	{
		return True;
	}
	// select next
	else if (event.isPressed() && (event.key() == KEY_DOWN))
	{
		return True;
	}
	// select first
	else if (event.isPressed() && (event.key() == KEY_PGUP))
	{
		return True;
	}
	// select last
	else if (event.isPressed() && (event.key() == KEY_PGDW))
	{
		return True;
	}
	else if (event.isPressed() && (event.key() == KEY_SPACE))
	{
//		// signal
//		onSelectMenuItem(widget);

		return True;
	}

//	// press the menu TODO hover...
	else if (event.isPressed() &&
		((event.key() == KEY_RETURN) || (event.key() == KEY_NUMPAD_ENTER)))
	{
//		// signal
//		onSelectMenuItem(widget);

		return True;
	}

	return True;
}

Bool MenuWidget::character(Keyboard *keyboard, CharacterEvent event)
{
	return True;
}

void MenuWidget::focused()
{
	m_capacities.enable(STATE_FOCUSED);

	// TODO
}

void MenuWidget::lostFocus()
{
	m_capacities.disable(STATE_FOCUSED);

	// TODO
}

void MenuWidget::sizeChanged()
{
	Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::WINDOW_NORMAL);

	m_limits = m_size;

	m_limits.x() -= drawMode->getLeftMargin() + drawMode->getRightMargin();
	m_limits.y() -= drawMode->getTopMargin() + drawMode->getBottomMargin();

	Vector2i pos(
				m_limits.x() - getUsedTheme()->getDefaultWidgetSize(Theme::SCROLL_BAR_BORDER).x(),
				0);

	Vector2i size(-1, m_limits.y());

    m_vertScrollbar->setPos(pos);
    m_vertScrollbar->setSize(size);

	m_vertScrollbar->show(False);

	updateScrollbar();

	setDirty();
}

Vector2i MenuWidget::getClientSize() const
{
	// compute the client size
	const Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::WINDOW_NORMAL);
	if (!drawMode)
		return m_size;

	Vector2i size = m_size;
    size -= drawMode->getTopLeftMargin() + drawMode->getBottomRightMargin();

	return size;
}

void MenuWidget::draw()
{
	if (!isShown())
		return;

	Widget *widget;

	// adjujst items minimal width
	if (isDirty())
	{
		for (IT_MenuItemList it = m_sonList.begin(); it != m_sonList.end(); ++it)
		{
			widget = *it;

			// item have at least the width of the menu
			if (widget->size().x() < m_limits.x())
                widget->setSize(m_limits.x(), widget->size().y());
		}
	}

	Vector2i org = getOrigin();
	Vector2i scrollPos = getScrollPos();

	processCache();

	// translate in local coordinates
	getScene()->getContext()->modelView().translate(
				Vector3((Float)m_pos.x(), (Float)m_pos.y(), 0.f));

	// translate in client area coordinates
	getScene()->getContext()->modelView().translate(
                Vector3((Float)org.x(), (Float)org.y(), 0.f));

	// draw the scrollbar in client area space
	getScene()->getContext()->modelView().push();
	m_vertScrollbar->draw();
	getScene()->getContext()->modelView().pop();

	// scroll space
	getScene()->getContext()->modelView().translate(Vector3(
		(Float)-scrollPos.x(),
		(Float)-scrollPos.y(),
		0.f));

	// draw recursively all sons
	for (IT_MenuItemList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		widget = *it;

		// Draw only if partialy or fully visible
        if ((widget->pos().x()-scrollPos.x() <= m_limits.x()) &&
            (widget->pos().y()-scrollPos.y() <= m_limits.y()) &&
            (widget->pos().x()-scrollPos.x()+widget->size().x() >= 0) &&
            (widget->pos().y()-scrollPos.y()+widget->size().y() >= 0))
		{
			getScene()->getContext()->modelView().push();
			widget->draw();
			getScene()->getContext()->modelView().pop();
		}
	}
}

void MenuWidget::updateCache()
{
	Theme *theme = getUsedTheme();

	// Draw borders
    theme->drawWidget(Theme::WINDOW_NORMAL, 0, 0, m_size.x(), m_size.y());
}

void MenuWidget::addElement(Widget *widget, Int32 index)
{
	if (widget->getParent() != this)
		O3D_ERROR(E_InvalidParameter("The given widget is not a child of this widget"));

	widget->defineLayoutItem();

	// append
	if ((index == -1) || (index >= (Int32)m_sonList.size()) || m_sonList.empty())
	{
		// compute the widget pos
		Int32 posY = 0;
		if (!m_sonList.empty())
		{
			Widget *prevElt = m_sonList.back();
            posY = prevElt->pos().y() + prevElt->size().y();
		}

        widget->setPos(0, posY);
		m_sonList.push_back(widget);
	}
	// prepend
	else if (index == 0)
	{
		// set the widget pos
        widget->setPos(0, 0);

		// Update position of the following elements
		Int32 n = 1;
		for (IT_MenuItemList it = m_sonList.begin(); it != m_sonList.end(); ++it, ++n)
		{
			Widget *nextElt = *it;
            Vector2i pos = nextElt->pos();

            nextElt->setPos(pos.x(), pos.y() + widget->size().y());
		}

		m_sonList.push_front(widget);
	}
	// insert before index
	else
	{
		// compute the widget pos
        Int32 n = 0, height = 0;
		IT_MenuItemList it = m_sonList.begin();
		for (; it != m_sonList.end(); ++it, ++n)
		{
			if (n == index)
			{
				Widget *prevElt = *it;
                height = prevElt->pos().y() + prevElt->size().y();
				break;
			}
		}

        widget->setPos(0, height);
		m_sonList.insert(it, widget);

		// Update position of the following elements
		++n;
		for (; it != m_sonList.end(); ++it, ++n)
		{
			Widget *nextElt = *it;
            Vector2i pos = nextElt->pos();

            nextElt->setPos(pos.x(), pos.y() + widget->pos().y());
		}
	}

	m_virtualSize.y() += widget->size().y();
	m_virtualSize.x() = o3d::max(m_virtualSize.x(), widget->size().x());

	// auto adjust the width of the menu
	if (m_virtualSize.x() > m_limits.x())
		m_size.x() += m_virtualSize.x() - m_limits.x();

	// and the height
	if (m_virtualSize.y() > m_limits.y())
		m_size.y() += m_virtualSize.y() - m_limits.y();

	// limits to max size
	if (m_size.x() > m_maxSize.x())
		m_size.x() = m_maxSize.x();

	if (m_size.y() > m_maxSize.y())
		m_size.y() = m_maxSize.y();

	m_minSize = m_size;

	// reset the menu to its initial size
	Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::WINDOW_NORMAL);

	// borders
	m_limits.x() = m_size.x() - (drawMode->getLeftMargin() + drawMode->getRightMargin());
	m_limits.y() = m_size.y() - (drawMode->getTopMargin() + drawMode->getBottomMargin());

	updateScrollbar();

	setDirty();
}

Widget* MenuWidget::findElement(const String &name)
{
	for (IT_MenuItemList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		if ((*it)->getName() == name)
			return *it;
	}

	return nullptr;
}

const Widget *MenuWidget::findElement(const String &name) const
{
	for (CIT_MenuItemList cit = m_sonList.begin(); cit != m_sonList.end(); ++cit)
	{
		if ((*cit)->getName() == name)
			return *cit;
	}

	return nullptr;
}

Widget *MenuWidget::getElement(Int32 index)
{
	Int32 n = 0;
	for (IT_MenuItemList it = m_sonList.begin(); it != m_sonList.end(); ++it, ++n)
	{
		if (n == index)
			return *it;
	}

	return nullptr;
}

const Widget *MenuWidget::getElement(Int32 index) const
{
	Int32 n = 0;
	for (CIT_MenuItemList cit = m_sonList.begin(); cit != m_sonList.end(); ++cit, ++n)
	{
		if (n == index)
			return *cit;
	}

	return nullptr;
}

Widget* MenuWidget::findWidget(const String &name)
{
	if (getName() == name)
		return this;

	// recursive search
	for (IT_MenuItemList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
        if ((*it)->getName() == name)
            return *it;

		// the child widget is a container ?
		if ((*it)->getLayout())
		{
			Widget *pWidget = (*it)->getLayout()->findWidget(name);
			if (pWidget)
				return pWidget;
		}
	}

    return nullptr;
}

const Widget* MenuWidget::findWidget(const String &name) const
{
	if (getName() == name)
		return this;

	// recursive search
	for (CIT_MenuItemList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
        if ((*it)->getName() == name)
            return *it;

		// the child widget is a container ?
		if ((*it)->getLayout())
		{
			const Widget *pWidget = (*it)->getLayout()->findWidget(name);
			if (pWidget)
				return pWidget;
		}
	}

    return nullptr;
}

Bool MenuWidget::isWidgetExist(Widget *pWidget) const
{
	if (this == pWidget)
		return True;

	// recursive search
	for (CIT_MenuItemList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
        if (*it == pWidget)
            return True;

		// the child widget is a container ?
		if ((*it)->getLayout())
		{
			if ((*it)->getLayout()->isWidgetExist(pWidget))
				return True;
		}
	}

	return False;
}

Bool MenuWidget::deleteWidget(Widget *pWidget)
{
	// recursive search
	for (IT_MenuItemList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		if (*it == pWidget)
		{
            getGui()->getWidgetManager()->checkPrevTargetedWidget(pWidget);

			Widget *elt = *it;

			// compute the position and index of the following elements
			Int32 height = elt->size().y();

			IT_MenuItemList rmit = it++;
			m_sonList.erase(rmit);

			for (; it != m_sonList.end(); ++it)
			{
				Widget *elt = *it;

                elt->setPos(
                            elt->pos().x(),
                            elt->pos().y() - height);
			}

			// update virtual size
			m_virtualSize.y() -= height;

			// and auto adjust height
			if (m_limits.y()-m_virtualSize.y() >= height)
			{
				m_size.y() -= height;
				m_limits.y() -= height;
			}

			m_minSize = m_size;

			updateScrollbar();

			deletePtr(elt);

			setDirty();
			return True;
		}

		// recurse on children
		if ((*it)->getLayout())
		{
			if ((*it)->getLayout()->deleteWidget(pWidget))
			{
				setDirty();
				return True;
			}
		}
	}

	return False;
}

Bool MenuWidget::deleteElement(Int32 index)
{
	Int32 n = 0;
	for (IT_MenuItemList it = m_sonList.begin(); it != m_sonList.end(); ++it, ++n)
	{
		if (n == index)
		{
			Widget *elt = *it;

			// compute the position and index of the following elements
			Int32 height = elt->size().y();

			IT_MenuItemList rmit = it++;
			m_sonList.erase(rmit);

			for (; it != m_sonList.end(); ++it)
			{
				Widget *elt = *it;

                elt->setPos(
                            elt->pos().x(),
                            elt->pos().y() - height);
			}

			// update virtual size
			m_virtualSize.y() -= height;

			// and auto adjust height
			if (m_limits.y()-m_virtualSize.y() >= height)
			{
				m_size.y() -= height;
				m_limits.y() -= height;
			}

			m_minSize = m_size;

			updateScrollbar();

			deletePtr(elt);

			return True;
		}
	}

	return False;
}

void MenuWidget::deleteAllElements()
{
	for (IT_MenuItemList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
        getGui()->getWidgetManager()->checkPrevTargetedWidget(*it);
		deletePtr(*it);
	}

	// reset the menu to its initial size
	Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::WINDOW_NORMAL);

	// initial size is min size
	m_size = m_minSize = getDefaultSize();
	m_limits = m_size;
	m_virtualSize.set(0, 0);

	// borders
	m_limits.x() -= drawMode->getLeftMargin() + drawMode->getRightMargin();
	m_limits.y() -= drawMode->getTopMargin() + drawMode->getBottomMargin();

	updateScrollbar();

	m_sonList.clear();
	setDirty();
}

