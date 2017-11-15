/**
 * @file wraplayout.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/wraplayout.h"

#include "o3d/gui/widgets/window.h"
#include "o3d/gui/guitype.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/engine/context.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(WrapLayout, GUI_LAYOUT_WRAP, Layout)

/*---------------------------------------------------------------------------------------
  default constructor
---------------------------------------------------------------------------------------*/
WrapLayout::WrapLayout(BaseObject *parent) :
	Layout(parent),
	m_orient(VERTICAL_LAYOUT),
	m_scrollbarSingleStep(-1),
	m_scrollbarPageStep(-1),
    m_maxEltSize(0)
{
	m_capacities.enable(CAPS_SCROLLABLE);
}

WrapLayout::WrapLayout(Widget *widget, Orient orient) :
	Layout(widget),
	m_orient(orient),
	m_scrollbarSingleStep(-1),
	m_scrollbarPageStep(-1),
    m_maxEltSize(0)
{
	m_capacities.enable(CAPS_SCROLLABLE);
}

WrapLayout::WrapLayout(Window *window, Orient orient) :
	Layout(window),
	m_orient(orient),
	m_scrollbarSingleStep(-1),
	m_scrollbarPageStep(-1),
    m_maxEltSize(0)
{
	m_capacities.enable(CAPS_SCROLLABLE);
}

// construct as child layout
WrapLayout::WrapLayout(Layout *layout, Orient orient) :
	Layout(layout),
	m_orient(orient),
	m_scrollbarSingleStep(-1),
	m_scrollbarPageStep(-1),
    m_maxEltSize(0)
{
	m_capacities.enable(CAPS_SCROLLABLE);
}

WrapLayout::~WrapLayout()
{
}

void WrapLayout::deleteAllWidgets()
{
    for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
        getGui()->getWidgetManager()->checkPrevTargetedWidget(*it);
		deletePtr(*it);
	}

    m_sonList.clear();

	// and the scrollbar is null
	m_scrollbar = nullptr;

	setDirty();
}

Vector2i WrapLayout::getScrollPos() const
{
    if ((m_scrollbar.isValid()) && m_scrollbar->isShown())
	{
		if (m_orient == VERTICAL_LAYOUT)
			return Vector2i(m_scrollbar->getSliderPos(), 0);
		else// if (m_orient == HORIZONTAL_LAYOUT)
			return Vector2i(0, m_scrollbar->getSliderPos());
	}
	else
		return Vector2i(0, 0);
}

/*---------------------------------------------------------------------------------------
  (Re)Compute the minimal size of the layout
---------------------------------------------------------------------------------------*/
Vector2i WrapLayout::calcMin()
{
    if (m_sonList.size() == 0)
		return Vector2i();

	Widget *pWidget;
	LayoutItem *pItem;

	// Total minimum size (width or height) of sizer
	Int32 maxMinWidth = 0;
	Int32 maxMinHeight = 0;

	m_minWidth = 0;
	m_minHeight = 0;

	// precalc item minsizes and max width or height
    for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		pWidget = (*it);
		pItem = pWidget->getLayoutItem();

		// ignore scrollbar
        if (pWidget == m_scrollbar.get())
			continue;

		if (pWidget->isShown())
		{
			Vector2i size(pItem->calcMin());

			if (size.x() > maxMinWidth)
				maxMinWidth = size.x();

			if (size.y() > maxMinHeight)
				maxMinHeight = size.y();
		}
	}

	// add the size of a scrollbar
	if (m_orient == VERTICAL_LAYOUT)
		maxMinHeight += getUsedTheme()->getDefaultWidgetSize(Theme::SCROLL_BAR_BORDER).y();
	else
		maxMinWidth += getUsedTheme()->getDefaultWidgetSize(Theme::SCROLL_BAR_BORDER).x();

	m_minWidth = maxMinWidth;
	m_minHeight = maxMinHeight;

	return Vector2i(m_minWidth, m_minHeight);
}

/*---------------------------------------------------------------------------------------
  (Re)Compute the size of the layout
---------------------------------------------------------------------------------------*/
void WrapLayout::computeSizes()
{
    if (m_sonList.size() == 0)
		return;

	Vector2i pt;

	Widget *pWidget;
	LayoutItem *pItem;

	if (m_size.x() < 0 || m_size.y() < 0)
		return;

	Vector2i layoutLimits(m_size + Vector2i(1, 1));
	Int32 maxSize = 0;
	Int32 virtualSize = 0;
	Int32 lastSize = 0;

	m_maxEltSize = 0;

	if (m_orient == VERTICAL_LAYOUT)
		layoutLimits.y() -= getUsedTheme()->getDefaultWidgetSize(Theme::SCROLL_BAR_BORDER).y();
	else
		layoutLimits.x() -= getUsedTheme()->getDefaultWidgetSize(Theme::SCROLL_BAR_BORDER).x();

	// no space to pos children
	if (layoutLimits.x() <= 0 || layoutLimits.y() <= 0)
		return;

	// Calculate overall minimum size
    for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		pWidget = (*it);
		pItem = pWidget->getLayoutItem();

		// ignore scrollbar
        if (pWidget == m_scrollbar.get())
			continue;

		if (pWidget->isShown())
		{
			Vector2i size(pItem->getMinSizeIncludeBorders());

			if (m_orient == VERTICAL_LAYOUT)
			{
				Int32 height = size.y();

				if (size.x() > maxSize)
					maxSize = size.x();

				if (maxSize > m_maxEltSize)
					m_maxEltSize = maxSize;

				// new column
				if ((pt.y() + size.y()) > layoutLimits.y())
				{
					pt.x() += maxSize;
					pt.y() = 0;

					virtualSize += maxSize;
					lastSize = maxSize;
					maxSize = 0;
				}

				Vector2i childPos(pt);
				Vector2i childSize(size.x(), height);

				// TODO align according to the biggest element of the column
//				if ((pWidget->getHorizontalAlign() == HORIZONTAL_EXPAND) || pWidget->isShaped())
//					childSize.x() = m_size.x();
//				else if (pWidget->getHorizontalAlign() == RIGHT_ALIGN)
//					childPos.x() += m_size.x() - size.x();
//				else if (pWidget->getHorizontalAlign() == CENTER_ALIGN)
//					childPos.x() += (m_size.x() - size.x()) / 2;

				pItem->setDimension(childPos, childSize);

				pt.y() += height;
			}
			else
			{
				Int32 width = size.x();

				if (size.y() > maxSize)
					maxSize = size.y();

				if (maxSize > m_maxEltSize)
					m_maxEltSize = maxSize;

				// new row
				if ((pt.x() + size.x()) > layoutLimits.x())
				{
					pt.y() += maxSize;
					pt.x() = 0;

					virtualSize += maxSize;
					lastSize = maxSize;
					maxSize = 0;
				}

				Vector2i childPos(pt);
				Vector2i childSize(width, size.y());

				// TODO align according to the biggest element of the row
//				if ((pWidget->getVerticalAlign() == VERTICAL_EXPAND) || pWidget->isShaped())
//					childSize.y() = m_size.y();
//				else if (pWidget->getVerticalAlign() == BOTTOM_ALIGN)
//					childPos.y() += m_size.y() - size.y();
//				else if (pWidget->getVerticalAlign() == MIDDLE_ALIGN)
//					childPos.y() += (m_size.y() - size.y()) / 2;

				pItem->setDimension(childPos, childSize);

				pt.x() += width;
			}
		}
	}

	// add the last row/column size
	virtualSize += lastSize;

	// compute the scrollbar
	if (m_orient == VERTICAL_LAYOUT)
	{
		// last column size
		if (virtualSize >= m_size.x())
		{
			Vector2i pos(0, m_size.y() - getUsedTheme()->getDefaultWidgetSize(Theme::SCROLL_BAR_BORDER).y());

            if (m_scrollbar.isNull() || m_scrollbar->getOrientation() != ScrollBar::HORIZONTAL_ALIGN)
			{
				m_scrollbar = new LayoutScrollBar(this, ScrollBar::HORIZONTAL_ALIGN, pos, Vector2i(m_size.x(), -1));
                m_scrollbar->onSliderMoved.connect(this, &WrapLayout::sliderMoved);
			}
			else
			{
				m_scrollbar->show(True);
				m_scrollbar->setPos(pos);
                m_scrollbar->setSize(Vector2i(m_size.x(), -1));
			}

			if (m_scrollbarPageStep <= 0)
				m_scrollbar->setPageStep(m_maxEltSize);
			else
				m_scrollbar->setPageStep(m_scrollbarPageStep);

			if (m_scrollbarSingleStep <= 0)
				m_scrollbar->setSingleStep(m_maxEltSize / 10);
			else
				m_scrollbar->setSingleStep(m_scrollbarSingleStep);

			m_scrollbar->setRange(0, virtualSize - lastSize);
		}
		else
		{
            if (m_scrollbar.isValid())
				m_scrollbar->show(False);
		}
	}
	else
	{
		// last row size
		if (virtualSize >= m_size.y())
		{
			Vector2i pos(m_size.x() - getUsedTheme()->getDefaultWidgetSize(Theme::SCROLL_BAR_BORDER).x(), 0);

            if (m_scrollbar.isNull() || m_scrollbar->getOrientation() != ScrollBar::VERTICAL_ALIGN)
			{
				m_scrollbar = new LayoutScrollBar(this, ScrollBar::VERTICAL_ALIGN, pos, Vector2i(-1, m_size.y()));
                m_scrollbar->onSliderMoved.connect(this, &WrapLayout::sliderMoved);
			}
			else
			{
				m_scrollbar->show(True);
				m_scrollbar->setPos(pos);
                m_scrollbar->setSize(Vector2i(-1, m_size.y()));
			}

			if (m_scrollbarPageStep <= 0)
				m_scrollbar->setPageStep(m_maxEltSize);
			else
				m_scrollbar->setPageStep(m_scrollbarPageStep);

			if (m_scrollbarSingleStep <= 0)
				m_scrollbar->setSingleStep(m_maxEltSize / 10);
			else
				m_scrollbar->setSingleStep(m_scrollbarSingleStep);

			m_scrollbar->setRange(0, virtualSize - lastSize);
		}
		else
		{
            if (m_scrollbar.isValid())
				m_scrollbar->show(False);
		}
	}
}

Bool WrapLayout::isTargeted(Int32 x, Int32 y, Widget *&pWidget)
{
	Vector2i scrollPos = getScrollPos();
	Widget *widget;

	// no negative part
	if (x < 0 || y < 0)
		return False;

	// recursive targeted
    for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		widget = *it;

        if (widget != m_scrollbar.get())
		{
			if (widget->isTargeted(
                    x - widget->pos().x() + scrollPos.x(),
                    y - widget->pos().y() + scrollPos.y(),
					pWidget))

				return True;
		}
	}

    if (m_scrollbar.isValid())
	{
		if (m_scrollbar->isTargeted(
                x - m_scrollbar->pos().x(),
                y - m_scrollbar->pos().y(),
				pWidget))

			return True;
	}

    if ((x < m_size.x()) && (y < m_size.y()))
	{
		pWidget = this;
		return True;
	}

	return False;
}

void WrapLayout::setScrollBarSingleStep(Int32 step)
{
	m_scrollbarSingleStep = step;

    if (m_scrollbar.isValid())
	{
		if (m_scrollbarSingleStep <= 0)
			m_scrollbar->setSingleStep(m_maxEltSize / 10);
		else
			m_scrollbar->setSingleStep(m_scrollbarSingleStep);
	}
}

void WrapLayout::setScrollBarPageStep(Int32 step)
{
	m_scrollbarPageStep = step;

    if (m_scrollbar.isValid())
	{
		if (m_scrollbarPageStep <= 0)
			m_scrollbar->setPageStep(m_maxEltSize);
		else
			m_scrollbar->setPageStep(m_scrollbarPageStep);
	}
}

void WrapLayout::setOrientation(WrapLayout::Orient orient)
{
	m_orient = orient;
}

void WrapLayout::draw()
{
	if (!isShown())
		return;

	Widget *widget;

	Vector2i scrollPos = getScrollPos();

	// local space
	getScene()->getContext()->modelView().translate(Vector3(
		(Float)m_pos.x(),
		(Float)m_pos.y(),
		0.f));

	// draw the scrollbar in parent space
    if (m_scrollbar.isValid())
	{
		getScene()->getContext()->modelView().push();
		m_scrollbar->draw();
		getScene()->getContext()->modelView().pop();
	}

	Vector3 translation = getScene()->getContext()->modelView().get().getTranslation();
	getScene()->getContext()->enableScissorTest();
	getScene()->getContext()->setScissor(
				(Int32)translation.x(),
                getGui()->getHeight()-(Int32)translation.y()-m_size.y(),
				m_size.x(),
				m_size.y());

	// scroll space
	getScene()->getContext()->modelView().translate(Vector3(
		(Float)-scrollPos.x(),
		(Float)-scrollPos.y(),
		0.f));

	// draw recursively all sons
    for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		widget = *it;

		// Draw only if partialy or fully visible
        if ((widget->pos().x()-scrollPos.x() <= m_size.x()) &&
            (widget->pos().y()-scrollPos.y() <= m_size.y()) &&
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

Bool WrapLayout::mouseWheel(Int32 x, Int32 y, Int32 z)
{
    if (m_scrollbar.isValid())
	{
		if (z > 0)
		{
			m_scrollbar->setSliderPos(m_scrollbar->getSliderPos()+m_scrollbar->getSingleStep());
			return True;
		}
		else if (z < 0)
		{
			m_scrollbar->setSliderPos(m_scrollbar->getSliderPos()-m_scrollbar->getSingleStep());
			return True;
		}
	}

	return False;
}

void WrapLayout::sliderMoved(Int32 pos)
{

}

