/**
 * @file freelayout.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/freelayout.h"

#include "o3d/gui/widgets/window.h"
#include "o3d/gui/guitype.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(FreeLayout, GUI_LAYOUT_FREE, Layout)

FreeLayout::FreeLayout(BaseObject *parent) :
    Layout(parent)
{
}

FreeLayout::FreeLayout(Widget *widget) :
    Layout(widget)
{
}

FreeLayout::FreeLayout(Window *window) :
    Layout(window)
{
}

// construct as child layout
FreeLayout::FreeLayout(Layout *layout) :
    Layout(layout)
{
}

Vector2i FreeLayout::calcMin()
{
    if (m_sonList.size() == 0)
        return Vector2i();

    Widget *pWidget;
    LayoutItem *pItem;

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
        }
    }

    // Total minimum size (width or height) of sizer
    Vector2i maxMinSize;

    for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
    {
        pWidget = (*it);
        pItem = pWidget->getLayoutItem();

        if (pWidget->isShown() && (pWidget->getStretchFactor() != 0))
        {
            Vector2i minSize(pItem->getMinSizeIncludeBorders());
            minSize += pItem->getPos();

            if (minSize.x() > maxMinSize.x())
                maxMinSize.x() = minSize.x();

            if (minSize.y() > maxMinSize.y())
                maxMinSize.y() = minSize.y();
        }
    }

    m_minWidth = maxMinSize.x();
    m_minHeight = maxMinSize.y();
    m_fixedWidth = m_minWidth;
    m_fixedHeight = m_minHeight;

    return maxMinSize;
}

void FreeLayout::computeSizes()
{
/*    if (m_SonList.size() == 0)
        return;

    Vector2i pt(m_pos);

    Widget *pWidget;
    LayoutItem *pItem;

    // Calculate overall minimum size
    for (IT_WidgetList it = m_SonList.begin(); it != m_SonList.end(); ++it)
    {
        pWidget = (*it);
        pItem = pWidget->getLayoutItem();

        if (pWidget->isShown())
        {
            Vector2i size(pItem->getMinSizeIncludeBorders());

            if (m_orient == VERTICAL_LAYOUT)
            {
                Int32 height = size.y();

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
    }*/
}

