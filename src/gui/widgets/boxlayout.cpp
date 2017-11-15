/**
 * @file boxlayout.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/boxlayout.h"

#include "o3d/gui/widgets/window.h"
#include "o3d/gui/guitype.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(BoxLayout, GUI_LAYOUT_BOX, Layout)

/*---------------------------------------------------------------------------------------
  default constructor
---------------------------------------------------------------------------------------*/
BoxLayout::BoxLayout(BaseObject *parent) :
	Layout(parent),
	m_orient(VERTICAL_LAYOUT),
	m_stretchable(0)
{
}

BoxLayout::BoxLayout(Widget *widget, Orient orient) :
	Layout(widget),
	m_orient(orient),
	m_stretchable(0)
{
}

BoxLayout::BoxLayout(Window *window, Orient orient) :
	Layout(window),
	m_orient(orient),
	m_stretchable(0)
{
}

// construct as child layout
BoxLayout::BoxLayout(Layout *layout, Orient orient) :
	Layout(layout),
	m_orient(orient),
	m_stretchable(0)
{
}

void BoxLayout::setOrientation(Orient orient)
{
	m_orient = orient;
}

/*---------------------------------------------------------------------------------------
  (Re)Compute the minimal size of the layout
---------------------------------------------------------------------------------------*/
Vector2i BoxLayout::calcMin()
{
    if (m_sonList.size() == 0)
		return Vector2i();

	Widget *pWidget;
	LayoutItem *pItem;

    m_stretchable = 0;
    m_minWidth = 0;
    m_minHeight = 0;
    m_fixedWidth = 0;
	m_fixedHeight = 0;

	// precalc item minsizes and count stretch factors
    for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		pWidget = (*it);
		pItem = pWidget->getLayoutItem();

		if (pWidget->isShown())
		{
			pItem->calcMin();

			m_stretchable += pWidget->getStretchFactor();
		}
	}

	// Total minimum size (width or height) of sizer
	Int32 maxMinSize = 0;

    for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		pWidget = (*it);
		pItem = pWidget->getLayoutItem();

		if (pWidget->isShown() && (pWidget->getStretchFactor() != 0))
		{
			Int32 stretch = pWidget->getStretchFactor();
			Vector2i size(pItem->getMinSizeIncludeBorders());
			Int32 minSize;

            // Integer division rounded up is (a + b - 1) / b
            // Round up needed in order to guarantee that all items will have size
			// not less than their min size
            if (m_orient == HORIZONTAL_LAYOUT)
                minSize = (size.x() * m_stretchable + stretch - 1) / stretch;
            else
                minSize = (size.y() * m_stretchable + stretch - 1) / stretch;

            if (minSize > maxMinSize)
                maxMinSize = minSize;
		}
	}

	// Calculate overall minimum size
    for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		pWidget = (*it);
		pItem = pWidget->getLayoutItem();

		if (pWidget->isShown())
		{
			Vector2i size(pItem->getMinSizeIncludeBorders());
			if (pWidget->getStretchFactor() != 0)
			{
				if (m_orient == HORIZONTAL_LAYOUT)
                    size.x() = (maxMinSize * pWidget->getStretchFactor()) / m_stretchable;
				else
                    size.y() = (maxMinSize * pWidget->getStretchFactor()) / m_stretchable;
			}
			else
			{
				if (m_orient == VERTICAL_LAYOUT)
				{
                    m_fixedHeight += size.y();
                    m_fixedWidth = o3d::max(m_fixedWidth, size.x());
				}
				else
				{
                    m_fixedWidth += size.x();
                    m_fixedHeight = o3d::max(m_fixedHeight, size.y());
				}
			}

			if (m_orient == HORIZONTAL_LAYOUT)
			{
                m_minWidth += size.x();
                m_minHeight = o3d::max(m_minHeight, size.y());
			}
			else
			{
                m_minHeight += size.y();
                m_minWidth = o3d::max(m_minWidth, size.x());
			}
		}
	}

	return Vector2i(m_minWidth,m_minHeight);
}

/*---------------------------------------------------------------------------------------
  (Re)Compute the size of the layout
---------------------------------------------------------------------------------------*/
void BoxLayout::computeSizes()
{
    if (m_sonList.size() == 0)
		return;

    Int32 delta = 0;
    if (m_stretchable)
    {
        if (m_orient == HORIZONTAL_LAYOUT)
            delta = m_size.x() - m_fixedWidth;
        else
            delta = m_size.y() - m_fixedHeight;
    }

	Vector2i pt(m_pos);

	Widget *pWidget;
	LayoutItem *pItem;

	// Calculate overall minimum size
    for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		pWidget = (*it);
		pItem = pWidget->getLayoutItem();

		if (pWidget->isShown())
		{
			Vector2i size(pItem->getMinSizeIncludeBorders());

			if (m_orient == VERTICAL_LAYOUT)
			{
                Int32 height = size.y();
				if (pWidget->getStretchFactor())
				{
                    // Because of at least one visible item has non-zero stretch factor
					// then m_stretchable is not zero
                    height = (delta * pWidget->getStretchFactor()) / m_stretchable;
				}

				Vector2i childPos(pt);
                Vector2i childSize(size.x(),height);

				if ((pWidget->getHorizontalAlign() == HORIZONTAL_EXPAND) || pWidget->isShaped())
                    childSize.x() = m_size.x();
				else if (pWidget->getHorizontalAlign() == RIGHT_ALIGN)
                    childPos.x() += m_size.x() - size.x();
				else if (pWidget->getHorizontalAlign() == CENTER_ALIGN)
                    childPos.x() += (m_size.x() - size.x()) / 2;

				// one widget can be verticaly expanded
				if (pWidget->getVerticalAlign() == VERTICAL_EXPAND)
				{
					childSize.y() = m_size.y() - pt.y();
					height = childSize.y();
				}

				pItem->setDimension(childPos - m_pos, childSize);

                pt.y() += height;
			}
			else
			{
                Int32 width = size.x();
				if (pWidget->getStretchFactor())
				{
                    // Because of at least one visible item has non-zero stretch factor
					// then m_stretchable is not zero
                    width = (delta * pWidget->getStretchFactor()) / m_stretchable;
				}

				Vector2i childPos(pt);
                Vector2i childSize(width,size.y());

				if ((pWidget->getVerticalAlign() == VERTICAL_EXPAND) || pWidget->isShaped())
                    childSize.y() = m_size.y();
				else if (pWidget->getVerticalAlign() == BOTTOM_ALIGN)
                    childPos.y() += m_size.y() - size.y();
				else if (pWidget->getVerticalAlign() == MIDDLE_ALIGN)
                    childPos.y() += (m_size.y() - size.y()) / 2;

				// one widget can be horizontaly expanded
				if (pWidget->getHorizontalAlign() == HORIZONTAL_EXPAND)
				{
					childSize.x() = m_size.x() - pt.x();
					width = childSize.x();
				}

				pItem->setDimension(childPos - m_pos, childSize);

                pt.x() += width;
			}
		}
	}
}

