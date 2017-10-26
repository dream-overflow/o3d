/**
 * @file listbox.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/listbox.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/gui/guitype.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"

#include "o3d/gui/precompiled.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(ListBox, GUI_WIDGET_LISTBOX, Widget)

ListBox::ListBox(BaseObject *parent) :
	Widget(parent),
	m_listBoxStyle(DEFAULT_STYLE),
	m_horiScrollbar(nullptr),
	m_vertScrollbar(nullptr),
	m_lastSelection(nullptr)
{
}

ListBox::ListBox(Widget *parent,
		const Vector2i &pos,
		const Vector2i &size,
		ListBox::ListBoxStyle style,
		const String &name) :
	Widget(
		parent,
		pos,
		size,
		name),
	m_listBoxStyle(style),
	m_horiScrollbar(nullptr),
	m_vertScrollbar(nullptr),
	m_lastSelection(nullptr)
{
	init();
}

ListBox::~ListBox()
{
	deletePtr(m_horiScrollbar);
	deletePtr(m_vertScrollbar);

	for (ListBoxElement *elt : m_sonList)
	{
		deletePtr(elt);
	}

	m_sonList.clear();
}

void ListBox::init()
{
	Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::WINDOW_NORMAL);

	// calc the default listbox size
	Vector2i defaultSize = getDefaultSize();

    if (m_minSize.x() < 0)
        m_minSize.x() = defaultSize.x();

    if (m_minSize.y() < 0)
        m_minSize.y() = defaultSize.y();

	m_size = m_minSize;
	m_limits = m_size;
	m_virtualSize.set(0, 0);

	if ((m_listBoxStyle & BORDER_STYLE) != 0)
	{
		m_limits.x() -= drawMode->getLeftMargin() + drawMode->getRightMargin();
		m_limits.y() -= drawMode->getTopMargin() + drawMode->getBottomMargin();
	}

	if ((m_listBoxStyle & VERTICALY_SCROLLABLE) != 0)
	{
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

	if ((m_listBoxStyle & HORIZONTALY_SCROLLABLE) != 0)
	{
		Vector2i pos(0, m_limits.y() - getUsedTheme()->getDefaultWidgetSize(Theme::SCROLL_BAR_BORDER).y());
		Vector2i size(m_limits.x(), -1);

		m_horiScrollbar = new LayoutScrollBar(
					this,
					ScrollBar::HORIZONTAL_ALIGN,
					pos,
					size);

		m_horiScrollbar->setPageStep(m_limits.x() / 10);
		m_horiScrollbar->setSingleStep(1);

		m_horiScrollbar->show(False);
	}
}

void ListBox::updateScrollbars()
{
	Bool verticalToggle = False;

	// hide or show and range, in two passes
	if (m_vertScrollbar != nullptr)
	{
		if (m_virtualSize.y() > m_limits.y())
		{
			m_vertScrollbar->setRange(0, m_virtualSize.y() - m_limits.y());

			if (!m_vertScrollbar->isShown())
			{
				m_limits.x() -= m_vertScrollbar->size().x();
				m_vertScrollbar->show(True);
				verticalToggle = True;
			}
		}
		else
		{
			m_vertScrollbar->setRange(0, 0);

			if (m_vertScrollbar->isShown())
			{
				m_limits.x() += m_vertScrollbar->size().x();
				m_vertScrollbar->show(False);
				verticalToggle = True;
			}
		}
	}

	if (m_horiScrollbar != nullptr)
	{
		if (m_virtualSize.x() > m_limits.x())
		{
			m_horiScrollbar->setRange(0, m_virtualSize.x() - m_limits.x());

			if (!m_horiScrollbar->isShown())
			{
				m_limits.y() -= m_horiScrollbar->size().y();
				m_horiScrollbar->show(True);

				// update the vertical scroll bar
				if ((m_vertScrollbar != nullptr) && m_vertScrollbar->isShown())
				{
                    m_vertScrollbar->setSize(
								m_vertScrollbar->size().x(),
								m_limits.y());

					m_vertScrollbar->setRange(0, m_virtualSize.y() - m_limits.y());
				}
			}
		}
		else
		{
			m_horiScrollbar->setRange(0, 0);

			if (m_horiScrollbar->isShown())
			{
				m_limits.y() += m_horiScrollbar->size().y();
				m_horiScrollbar->show(False);

				// update the vertical scroll bar
				if ((m_vertScrollbar != nullptr) && m_vertScrollbar->isShown())
				{
                    m_vertScrollbar->setSize(
								m_vertScrollbar->size().x(),
								m_limits.y());

					m_vertScrollbar->setRange(0, m_virtualSize.y() - m_limits.y());
				}
			}
		}
	}

	// second pass, update the horizontal scrollbar
	if ((m_vertScrollbar != nullptr) && (m_horiScrollbar != nullptr) && verticalToggle)
	{
        m_horiScrollbar->setSize(
					m_limits.x(),
					m_horiScrollbar->size().y());

		m_horiScrollbar->setRange(0, m_virtualSize.x() - m_limits.x());
	}
}

Vector2i ListBox::getScrollPos() const
{
	Vector2i scrollPos;

	if (m_horiScrollbar != nullptr && m_horiScrollbar->isShown())
		scrollPos.x() = m_horiScrollbar->getSliderPos();

	if (m_vertScrollbar != nullptr && m_vertScrollbar->isShown())
		scrollPos.y() = m_vertScrollbar->getSliderPos();

	return scrollPos;
}

Vector2i ListBox::getOrigin() const
{
	if ((m_listBoxStyle & BORDER_STYLE) != 0)
	{
		const Theme *theme = getUsedTheme();
		const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::WINDOW_NORMAL);

		return drawMode->getTopLeftMargin();
	}
	else
		return Vector2i(0, 0);
}

Vector2i ListBox::getDefaultSize()
{
	Vector2i size = getUsedTheme()->getDefaultWidgetSize(Theme::WINDOW_NORMAL);

	// plus scrollbar...
	if ((m_listBoxStyle & VERTICALY_SCROLLABLE) != 0)
		size.x() += getUsedTheme()->getDefaultWidgetSize(Theme::SCROLL_BAR_BORDER).x();

	if ((m_listBoxStyle & HORIZONTALY_SCROLLABLE) != 0)
		size.y() += getUsedTheme()->getDefaultWidgetSize(Theme::SCROLL_BAR_BORDER).y();

	return size;
}

Bool ListBox::isTargeted(Int32 x, Int32 y, Widget *&pWidget)
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
		widget = this;

		// delta by the client origin for children widgets
        x -= org.x();
        y -= org.y();

		// vertical scrollbar
		if (m_vertScrollbar != nullptr && m_vertScrollbar->isShown())
		{
			if (m_vertScrollbar->isTargeted(
                        x - m_vertScrollbar->pos().x(),
                        y - m_vertScrollbar->pos().y(),
						pWidget))
			{
				return True;
			}
		}

		// horizontal scrollbar
		if (m_horiScrollbar != nullptr && m_horiScrollbar->isShown())
		{
			if (m_horiScrollbar->isTargeted(
                        x - m_horiScrollbar->pos().x(),
                        y - m_horiScrollbar->pos().y(),
						pWidget))
			{
				return True;
			}
		}

		// recursive targeted
		for (IT_ListBoxEltList it = m_sonList.begin(); it != m_sonList.end(); ++it)
		{
			widget = (*it)->getWidget();

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

Bool ListBox::mouseLeftPressed(Int32 x, Int32 y)
{
	if ((m_listBoxStyle & SELECTION) == 0)
		return True;

	// others children widgets
	Widget *widget = this;
	Widget *pWidget = nullptr;

	Vector2i scrollPos = getScrollPos();

	// for each elements
	for (IT_ListBoxEltList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		widget = (*it)->getWidget();

		if (widget->isTargeted(
                x - widget->pos().x() + scrollPos.x(),
                y - widget->pos().y() + scrollPos.y(),
				pWidget))
		{
			if (((m_listBoxStyle & MULTI_SELECTION) != 0)/* TODO && CTRL_KEY_DOWN*/)
			{
				(*it)->setSelected(!(*it)->isSelected());

				// signal
				onSelectElement(*it);
			}
			else
			{
				if (m_lastSelection != nullptr)
					m_lastSelection->setSelected(False);

				m_lastSelection = *it;
				m_lastSelection->setSelected(True);

				// signal
				onSelectElement(*it);
			}

			return True;
		}
	}

	return True;
}

Bool ListBox::mouseLeftReleased(Int32 x, Int32 y)
{
	return True;
}

Bool ListBox::mouseMove(Int32 x, Int32 y)
{
	return False;
}

void ListBox::mouseMoveIn()
{

}

void ListBox::mouseMoveOut()
{

}

Bool ListBox::mouseWheel(Int32 x, Int32 y, Int32 z)
{
	if (m_vertScrollbar)
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
	}

	return False;
}

Bool ListBox::keyboardToggled(Keyboard *keyboard, KeyEvent event)
{
	// TODO up/down arrow, pageup/pagedown, selection

	return False;
}

void ListBox::focused()
{
	m_capacities.enable(STATE_FOCUSED);

	// TODO
}

void ListBox::lostFocus()
{
	m_capacities.disable(STATE_FOCUSED);

	// TODO
}

void ListBox::sizeChanged()
{
	Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::WINDOW_NORMAL);

	m_limits = m_size;

	if ((m_listBoxStyle & BORDER_STYLE) != 0)
	{
		m_limits.x() -= drawMode->getLeftMargin() + drawMode->getRightMargin();
		m_limits.y() -= drawMode->getTopMargin() + drawMode->getBottomMargin();
	}

	if ((m_listBoxStyle & VERTICALY_SCROLLABLE) != 0)
	{
		Vector2i pos(
					m_limits.x() - getUsedTheme()->getDefaultWidgetSize(Theme::SCROLL_BAR_BORDER).x(),
					0);

		Vector2i size(-1, m_limits.y());

		m_vertScrollbar->setPos(pos);
        m_vertScrollbar->setSize(size);

		m_vertScrollbar->show(False);
	}

	if ((m_listBoxStyle & HORIZONTALY_SCROLLABLE) != 0)
	{
		Vector2i pos(
					0,
					m_limits.y() - getUsedTheme()->getDefaultWidgetSize(Theme::SCROLL_BAR_BORDER).y());

		Vector2i size(m_limits.x(), -1);

		m_horiScrollbar->setPos(pos);
        m_horiScrollbar->setSize(size);

		m_horiScrollbar->show(False);
	}

	updateScrollbars();

	setDirty();
}

void ListBox::draw()
{
	if (!isShown())
		return;

	Vector2i org = getOrigin();

	Widget *widget;

	Vector2i scrollPos = getScrollPos();

	processCache();

	// translate in local coordinates
	getScene()->getContext()->modelView().translate(
				Vector3((Float)m_pos.x(), (Float)m_pos.y(), 0.f));

	// translate in client area coordinates
	getScene()->getContext()->modelView().translate(
                Vector3((Float)org.x(), (Float)org.y(), 0.f));

	// draw the scrollbars in client area space
	if (m_vertScrollbar != nullptr)
	{
		getScene()->getContext()->modelView().push();
		m_vertScrollbar->draw();
		getScene()->getContext()->modelView().pop();
	}

	if (m_horiScrollbar != nullptr)
	{
		getScene()->getContext()->modelView().push();
		m_horiScrollbar->draw();
		getScene()->getContext()->modelView().pop();
	}

	Vector3 translation = getScene()->getContext()->modelView().get().getTranslation();
	getScene()->getContext()->enableScissorTest();
	getScene()->getContext()->setScissor(
				(Int32)translation.x(),
                getGui()->getHeight()-(Int32)translation.y()-m_limits.y(),
				m_limits.x(),
				m_limits.y());

	// scroll space
	getScene()->getContext()->modelView().translate(Vector3(
		(Float)-scrollPos.x(),
		(Float)-scrollPos.y(),
		0.f));

	// draw recursively all sons
	for (IT_ListBoxEltList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		widget = (*it)->getWidget();

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

	getScene()->getContext()->disableScissorTest();
}

void ListBox::updateCache()
{
	if ((m_listBoxStyle & BORDER_STYLE) != 0)
	{
		Theme *theme = getUsedTheme();

		// Draw borders
        theme->drawWidget(Theme::WINDOW_NORMAL, 0, 0, m_size.x(), m_size.y());
	}
}

ListBoxElement* ListBox::addElement(Widget *widget, Int32 index)
{
	if (widget->getParent() != this)
	{
		O3D_ERROR(E_InvalidParameter("The given widget is not a child of this widget"));
	}

	ListBoxElement *elt = new ListBoxElement(this, widget);

	widget->defineLayoutItem();

	// append
	if ((index == -1) || (index >= (Int32)m_sonList.size()) || m_sonList.empty())
	{
		// element index
		elt->setIndex(m_sonList.size());

		// compute the widget pos
		Int32 posY = 0;
		if (!m_sonList.empty())
		{
			ListBoxElement *prevElt = m_sonList.back();
            posY = prevElt->getWidget()->pos().y() + prevElt->getWidget()->size().y();
		}

		widget->setPos(0, posY);
		m_sonList.push_back(elt);
	}
	// prepend
	else if (index == 0)
	{
		// element index
		elt->setIndex(0);

		// set the widget pos
		widget->setPos(0, 0);

		// Update position of the following elements
		Int32 n = 1;
		for (IT_ListBoxEltList it = m_sonList.begin(); it != m_sonList.end(); ++it, ++n)
		{
			ListBoxElement *nextElt = *it;
            Vector2i pos = nextElt->getWidget()->pos();

			nextElt->getWidget()->setPos(pos.x(), pos.y() + widget->size().y());
			nextElt->setIndex(n);

			// signal
			onElementPosChanged(nextElt);
		}

		m_sonList.push_front(elt);
	}
	// insert before index
	else
	{
		// element index
		elt->setIndex(index);

		// compute the widget pos
        Int32 n = 0, height = 0;
		IT_ListBoxEltList it = m_sonList.begin();
		for (; it != m_sonList.end(); ++it, ++n)
		{
			if (n == index)
			{
				ListBoxElement *prevElt = *it;
                height = prevElt->getWidget()->pos().y() + prevElt->getWidget()->size().y();
				break;
			}
		}

		widget->setPos(0, height);
		m_sonList.insert(it, elt);

		// Update position of the following elements
		++n;
		for (; it != m_sonList.end(); ++it, ++n)
		{
			ListBoxElement *nextElt = *it;
            Vector2i pos = nextElt->getWidget()->pos();

			nextElt->getWidget()->setPos(pos.x(), pos.y() + widget->size().y());
			nextElt->setIndex(n);

			// signal
			onElementPosChanged(nextElt);
		}
	}

	m_virtualSize.y() += widget->size().y();
	m_virtualSize.x() = o3d::max(m_virtualSize.x(), widget->size().x());

	updateScrollbars();

	setDirty();

	// signal
	onElementAdded(elt);

	return elt;
}

ListBoxElement *ListBox::findElement(const String &name)
{
	for (IT_ListBoxEltList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		if ((*it)->getWidget()->getName() == name)
			return *it;
	}

	return nullptr;
}

const ListBoxElement *ListBox::findElement(const String &name) const
{
	for (CIT_ListBoxEltList cit = m_sonList.begin(); cit != m_sonList.end(); ++cit)
	{
		if ((*cit)->getWidget()->getName() == name)
			return *cit;
	}

	return nullptr;
}

ListBoxElement *ListBox::getElement(Int32 index)
{
	Int32 n = 0;
	for (IT_ListBoxEltList it = m_sonList.begin(); it != m_sonList.end(); ++it, ++n)
	{
		if (n == index)
			return *it;
	}

	return nullptr;
}

const ListBoxElement *ListBox::getElement(Int32 index) const
{
	Int32 n = 0;
	for (CIT_ListBoxEltList cit = m_sonList.begin(); cit != m_sonList.end(); ++cit, ++n)
	{
		if (n == index)
			return *cit;
	}

	return nullptr;
}

Widget* ListBox::findWidget(const String &name)
{
	if (getName() == name)
		return this;

	// recursive search
	for (IT_ListBoxEltList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
        if ((*it)->getWidget()->getName() == name)
            return (*it)->getWidget();

		// the child widget is a container ?
		if ((*it)->getWidget()->getLayout())
		{
			Widget *pWidget = (*it)->getWidget()->getLayout()->findWidget(name);
			if (pWidget)
				return pWidget;
		}
	}

	return NULL;
}

const Widget* ListBox::findWidget(const String &name) const
{
	if (getName() == name)
		return this;

	// recursive search
	for (CIT_ListBoxEltList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
        if ((*it)->getWidget()->getName() == name)
            return (*it)->getWidget();

		// the child widget is a container ?
		if ((*it)->getWidget()->getLayout())
		{
			const Widget *pWidget = (*it)->getWidget()->getLayout()->findWidget(name);
			if (pWidget)
				return pWidget;
		}
	}

	return NULL;
}

Bool ListBox::isWidgetExist(Widget *pWidget) const
{
	if (this == pWidget)
		return True;

	// recursive search
	for (CIT_ListBoxEltList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
        if ((*it)->getWidget() == pWidget)
            return True;

		// the child widget is a container ?
		if ((*it)->getWidget()->getLayout())
		{
			if ((*it)->getWidget()->getLayout()->isWidgetExist(pWidget))
				return True;
		}
	}

	return False;
}

Bool ListBox::deleteWidget(Widget *pWidget)
{
	// recursive search
	for (IT_ListBoxEltList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		if ((*it)->getWidget() == pWidget)
		{
            getGui()->getWidgetManager()->checkPrevTargetedWidget(pWidget);

			ListBoxElement *elt = *it;

			// compute the position and index of the following elements
			Int32 height = elt->getWidget()->size().y();

			IT_ListBoxEltList rmit = it++;
			m_sonList.erase(rmit);

			Int32 n = elt->getIndex();
			for (; it != m_sonList.end(); ++it, ++n)
			{
				ListBoxElement *elt = *it;

				elt->setIndex(n);
				elt->getWidget()->setPos(
                            elt->getWidget()->pos().x(),
                            elt->getWidget()->pos().y() - height);

				// signal
				onElementPosChanged(elt);
			}

			m_virtualSize.y() -= height;

			updateScrollbars();

			if (m_lastSelection == elt)
				m_lastSelection = nullptr;

			// signal before delete
			onRemoveElement(elt);

			deletePtr(elt);

			setDirty();
			return True;
		}

		// recurse on children
		if ((*it)->getWidget()->getLayout())
		{
			if ((*it)->getWidget()->getLayout()->deleteWidget(pWidget))
			{
				setDirty();
				return True;
			}
		}
	}

	return False;
}

Bool ListBox::deleteElement(Int32 index)
{
	Int32 n = 0;
	for (IT_ListBoxEltList it = m_sonList.begin(); it != m_sonList.end(); ++it, ++n)
	{
		if (n == index)
		{
			ListBoxElement *elt = *it;

			// compute the position and index of the following elements
			Int32 height = elt->getWidget()->size().y();

			IT_ListBoxEltList rmit = it++;
			m_sonList.erase(rmit);

			for (; it != m_sonList.end(); ++it, ++n)
			{
				ListBoxElement *elt = *it;

				elt->setIndex(n);
				elt->getWidget()->setPos(
                            elt->getWidget()->pos().x(),
                            elt->getWidget()->pos().y() - height);

				// signal
				onElementPosChanged(elt);
			}

			m_virtualSize.y() -= height;

			updateScrollbars();

			if (m_lastSelection == elt)
				m_lastSelection = nullptr;

			// signal before delete
			onRemoveElement(elt);

			deletePtr(elt);

			return True;
		}
	}

	return False;
}

void ListBox::deleteAllElements()
{
	for (IT_ListBoxEltList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
        getGui()->getWidgetManager()->checkPrevTargetedWidget((*it)->getWidget());
		deletePtr(*it);
	}

	m_virtualSize.set(0, 0);
	m_lastSelection = nullptr;

	updateScrollbars();

	m_sonList.clear();
	setDirty();
}

void ListBox::setVerticalPageStep(Int32 step)
{
	if (m_vertScrollbar != nullptr)
		m_vertScrollbar->setPageStep(step);
}

Int32 ListBox::getVerticalPageStep() const
{
	if (m_vertScrollbar != nullptr)
		m_vertScrollbar->getPageStep();

	return 0;
}

void ListBox::setVerticalSingleStep(Int32 step)
{
	if (m_vertScrollbar != nullptr)
		m_vertScrollbar->setSingleStep(step);
}

Int32 ListBox::getVerticalSingleStep() const
{
	if (m_vertScrollbar != nullptr)
		m_vertScrollbar->getSingleStep();

	return 0;
}

void ListBox::setHorizontalPageStep(Int32 step)
{
	if (m_horiScrollbar != nullptr)
		m_horiScrollbar->setPageStep(step);
}

Int32 ListBox::getHorizontalPageStep() const
{
	if (m_horiScrollbar != nullptr)
		m_horiScrollbar->getPageStep();

	return 0;
}

void ListBox::setHorizontalSingleStep(Int32 step)
{
	if (m_horiScrollbar != nullptr)
		m_horiScrollbar->setSingleStep(step);
}

Int32 ListBox::getHorizontalSingleStep() const
{
	if (m_horiScrollbar != nullptr)
		m_horiScrollbar->getSingleStep();

	return 0;
}

void ListBox::setVerticalPos(Int32 y)
{
	if (m_vertScrollbar != nullptr)
	{
		if (y < 0)
			m_vertScrollbar->setSliderPos(m_vertScrollbar->getMaximum());
		else
			m_vertScrollbar->setSliderPos(y);
	}
}

void ListBox::setHorizontalPos(Int32 x)
{
	if (m_horiScrollbar != nullptr)
	{
		if (x < 0)
			m_horiScrollbar->setSliderPos(m_horiScrollbar->getMaximum());
		else
			m_horiScrollbar->setSliderPos(x);
	}
}

ListBoxElement::ListBoxElement(ListBox *listBox, Widget *widget) :
	m_index(-1),
	m_selected(False),
	m_listBox(listBox),
	m_widget(widget),
	m_id(-1),
	m_metadata(widget)
{
}

ListBoxElement::~ListBoxElement()
{
	deletePtr(m_widget);
}

void ListBoxElement::setMetadata(BaseObject *metadata)
{
	m_metadata = metadata;
}

BaseObject* ListBoxElement::getMetadata()
{
	return m_metadata.get();
}

const BaseObject* ListBoxElement::getMetadata() const
{
	return m_metadata.get();
}

