/**
 * @file layout.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/layout.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/gui/widgets/window.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/engine/context.h"

#include <algorithm>

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(Layout, GUI_LAYOUT, Widget)

/*---------------------------------------------------------------------------------------
  default constructor
---------------------------------------------------------------------------------------*/

Layout::Layout(BaseObject *parent) :
	Widget(parent),
    m_window(nullptr)
{
	m_capacities.enable(CAPS_LAYOUT_TYPE);
}

Layout::Layout(Widget *widget) :
	Widget(widget),
    m_window(nullptr)
{
	m_capacities.enable(CAPS_LAYOUT_TYPE);

	m_pos.set(0,0);
}

Layout::Layout(Window *window) :
	Widget(window),
    m_window(window)
{
	m_capacities.enable(CAPS_LAYOUT_TYPE);

	m_pos.set(0,0);

	// initial size
	if (window)
        m_size = window->getMinSize();
}

// construct as child layout
Layout::Layout(Layout *layout) :
	Widget(layout),
    m_window(nullptr)
{
	m_capacities.enable(CAPS_LAYOUT_TYPE);

	m_pos.set(0,0);
}

/*---------------------------------------------------------------------------------------
  destructor
---------------------------------------------------------------------------------------*/
Layout::~Layout()
{
    for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
		deletePtr(*it);
}

/*---------------------------------------------------------------------------------------
  Mouse events management
---------------------------------------------------------------------------------------*/
Bool Layout::isTargeted(Int32 x, Int32 y, Widget *&pWidget)
{
	// recursive targeted
    for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
        if ((*it)->isTargeted(x - (*it)->pos().x(),	y - (*it)->pos().y(), pWidget))
			return True;
	}

	return False;
}

void Layout::draw()
{
	if (!isShown())
		return;

	// local space
	getScene()->getContext()->modelView().translate(Vector3(
        (Float)m_pos.x(),
        (Float)m_pos.y(),
		0.f));

	// draw recursively all sons
    for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		getScene()->getContext()->modelView().push();
		(*it)->draw();
		getScene()->getContext()->modelView().pop();
	}
}

/*---------------------------------------------------------------------------------------
  Events Management
---------------------------------------------------------------------------------------*/
void Layout::sizeChanged()
{

}

/*---------------------------------------------------------------------------------------
  insert a widget in last position
---------------------------------------------------------------------------------------*/
void Layout::addWidget(Widget *widget)
{
	if (widget->getParent() != this)
		O3D_ERROR(E_InvalidParameter("The given widget is not a child of this layout"));

    m_sonList.push_back(widget);
	widget->defineLayoutItem();

    setDirty();

    // each time a new widget is added, check again for the initial widget focus
    if ((getGui()->getWidgetManager()->getFocusedWindow() == m_window) &&
        (getGui()->getWidgetManager()->getFocusedWidget() == nullptr))
    {
        if (widget->isFocusable())
            getGui()->getWidgetManager()->setFocus(widget);
    }
}

void Layout::insertWidget(Widget *widget, const Widget *before)
{
    if (widget->getParent() != this)
        O3D_ERROR(E_InvalidParameter("The given widget is not a child of this layout"));

    // recursive search
    for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
    {
        if (*it == before)
        {
            m_sonList.insert(it, widget);
            widget->defineLayoutItem();

            setDirty();

            // each time a new widget is added, check again for the initial widget focus
            if ((getGui()->getWidgetManager()->getFocusedWindow() == m_window) &&
                (getGui()->getWidgetManager()->getFocusedWidget() == nullptr))
            {
                if (widget->isFocusable())
                    getGui()->getWidgetManager()->setFocus(widget);
            }

            return;
        }
    }

    O3D_ERROR(E_InvalidParameter("The given 'before' widget is not a child of this layout"));
}

/*---------------------------------------------------------------------------------------
  retrieve a widget by its object name (objects must have unique name for a valid result)
---------------------------------------------------------------------------------------*/
Widget* Layout::findWidget(const String &name)
{
	if (getName() == name)
		return this;

	// recursive search
    for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
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

const Widget* Layout::findWidget(const String &name) const
{
	if (getName() == name)
		return this;

	// recursive search
    for (CIT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
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

/*---------------------------------------------------------------------------------------
  search for an existing widget by its pointer (search in recursively if it is not a direct son)
---------------------------------------------------------------------------------------*/
Bool Layout::isWidgetExist(Widget *pWidget) const
{
	if (this == pWidget)
		return True;

	// recursive search
    for (CIT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
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

/*---------------------------------------------------------------------------------------
  delete a widget (and recursively all its sons) and return true if success
---------------------------------------------------------------------------------------*/
Bool Layout::deleteWidget(Widget *pWidget)
{
	// recursive search
    for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
		if ((*it) == pWidget)
		{
            getGui()->getWidgetManager()->checkPrevTargetedWidget(pWidget);

			deletePtr(*it);
            m_sonList.erase(it);

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

void Layout::deleteAllWidgets()
{
    for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
	{
        getGui()->getWidgetManager()->checkPrevTargetedWidget(*it);
		deletePtr(*it);
	}

    m_sonList.clear();
    setDirty();
}

Widget* Layout::findNextTabIndex(Widget *widget, Int32 direction)
{
    // returns the first widget (deep finding)
    if (!widget)
    {
        if (m_sonList.size() > 0)
        {
            if (direction > 0)
            {
                for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
                {
                    if ((*it)->getTabIndex() < 0)
                    {
                        Widget *first = (*it)->findNextTabIndex(nullptr, 1);
                        if (first && first->isFocusable())
                            return first;
                    }
                }
            }
            else if (direction < 0)
            {
                for (RIT_WidgetList rit = m_sonList.rbegin(); rit != m_sonList.rend(); ++rit)
                {
                    if ((*rit)->isFocusable() && ((*rit)->getTabIndex() < 0))
                    {
                        Widget *last = (*rit)->findNextTabIndex(nullptr, -1);
                        if (last && last->isFocusable())
                            return last;
                    }
                }
            }
        }

        // no focusable widgets founds
        return nullptr;
    }

    // bubble search
    if (direction > 0)
    {
        Widget *next = nullptr;

        // first search on sibling widgets and deeply
        for (IT_WidgetList it = m_sonList.begin(); it != m_sonList.end(); ++it)
        {
            if (*it == widget)
            {
                ++it;
                while (it != m_sonList.end())
                {
                    if ((*it)->getLayout())
                        next = (*it)->getLayout()->findNextTabIndex(nullptr, 1);
                    else
                        next = (*it)->findNextTabIndex(nullptr, 1);

                    if (next && next->isFocusable())
                        return next;

                    ++it;
                }
                break;
            }
        }

        return nullptr;
    }
    else if (direction < 0)
    {
        Widget *next = nullptr;

        for (RIT_WidgetList rit = m_sonList.rbegin(); rit != m_sonList.rend(); ++rit)
        {
            if (*rit == widget)
            {
                ++rit;
                while (rit != m_sonList.rend())
                {
                    if ((*rit)->getLayout())
                        next = (*rit)->getLayout()->findNextTabIndex(nullptr, -1);
                    else
                        next = (*rit)->findNextTabIndex(nullptr, -1);

                    if (next && next->isFocusable())
                        return next;

                    ++rit;
                }
                break;
            }
        }

        return nullptr;
    }
    else
    {
        return widget;
    }
}

/*---------------------------------------------------------------------------------------
  process the layout calculation
---------------------------------------------------------------------------------------*/
void Layout::layout()
{
	// (re)calculates minimums needed for each item and other preparations for layout
	calcMin();

	// apply the layout and repositions/resizes the items
	computeSizes();

	// layout is now updated
	setClean();
}

/*---------------------------------------------------------------------------------------
  set the dimensions of the layout and perform the layout
---------------------------------------------------------------------------------------*/
void Layout::setDimension(const Vector2i &pos, const Vector2i &size)
{
	m_pos = pos;
	m_size = size;

	layout();
}

/*---------------------------------------------------------------------------------------
  get the minimal widget size
---------------------------------------------------------------------------------------*/
Vector2i Layout::getMinSize()
{
	Vector2i minSize(calcMin());

    if (minSize.x() < m_minSize.x()) {
        minSize.x() = m_minSize.x();
    }

    if (minSize.y() < m_minSize.y()) {
        minSize.y() = m_minSize.y();
    }

	return minSize;
}

/*---------------------------------------------------------------------------------------
  Fit the window to the size of the content of the layout
---------------------------------------------------------------------------------------*/
Vector2i Layout::getMinWindowSize()
{
    Vector2i minSize(getMinSize());
    Vector2i size(m_window->size());
    Vector2i clientSize(m_window->getClientSize());

	return Vector2i(
            minSize.x() + size.x() - clientSize.x(),
            minSize.y() + size.y() - clientSize.y());
}

Vector2i Layout::getMaxWindowSize()
{
    return m_window->maxSize();
}

Vector2i Layout::fit()
{
    if (!m_window || !m_window->isTopLevelWidget()) {
        O3D_WARNING("Works only on top-level windows objects");
        return Vector2i(0,0);
    }

	// take the min size by default and limit it by max size
	Vector2i size(getMinWindowSize());
	Vector2i sizeMax(getMaxWindowSize());

    if (m_window->isTopLevelWidget()) {
        sizeMax.x() = getGui()->getWidth();
        sizeMax.y() = getGui()->getHeight();
	}

    if (sizeMax.x() != -1 && size.x() > sizeMax.x()) {
        size.x() = sizeMax.x();
    }
    if (sizeMax.y() != -1 && size.y() > sizeMax.y()) {
        size.y() = sizeMax.y();
    }

    m_window->setSize(size);
	return size;
}

/*---------------------------------------------------------------------------------------
  Similar to Fit, but sizes the interior (virtual) size of a window
---------------------------------------------------------------------------------------*/
Vector2i Layout::getMinClientSize()
{
    return getMinSize();
}

Vector2i Layout::getMaxClientSize()
{
    Vector2i maxSize(m_window->maxSize());

    if (maxSize != Widget::DEFAULT_SIZE) {
        Vector2i size(m_window->size());
        Vector2i clientSize(m_window->getClientSize());

		return Vector2i(
                maxSize.x() + clientSize.x() - size.x(),
                maxSize.y() + clientSize.y() - size.y());
    } else {
		return Widget::DEFAULT_SIZE;
    }
}

Vector2i Layout::virtualFitSize()
{
	Vector2i size(getMinClientSize());
	Vector2i sizeMax(getMaxClientSize());

	// Limit the size if sizeMax != DefaultSize
    if (size.x() > sizeMax.x() && sizeMax.x() != -1) {
        size.x() = sizeMax.x();
    }
    if (size.y() > sizeMax.y() && sizeMax.y() != -1) {
        size.y() = sizeMax.y();
    }

	return size;
}

void Layout::fitInside()
{
    if (!m_window) {
		O3D_WARNING("Works only on windows objects");
		return;
	}

	Vector2i size;

    if (m_window->isTopLevelWidget()) {
		size = virtualFitSize();
    } else {
		size = getMinClientSize();
    }

    m_window->setVirtualSize(size);
}

void Layout::setSizeHints()
{
    if (!m_window || !m_window->isTopLevelWidget())	{
		O3D_WARNING("Works only on top-level windows objects");
		return;
	}

	// Preserve the window's max size hints, but set the lower bound according to
	// the sizer calculations
	Vector2i size = fit();

    m_window->setSizeHints(
            size.x(),
            size.y(),
            m_window->maxSize().x(),
            m_window->maxSize().y());
}

void Layout::setVirtualSizeHints()
{
    if (!m_window) {
		O3D_WARNING("Works only on windows objects");
		return;
	}

	// Preserve the window's max size hints, but set the lower bound according to
	// the layout calculations
	fitInside();
    Vector2i size(m_window->getVirtualSize());

    m_window->setVirtualSizeHints(
            size.x(),
            size.y(),
            m_window->maxSize().x(),
            m_window->maxSize().y());
}

/*---------------------------------------------------------------------------------------
  constructors
---------------------------------------------------------------------------------------*/
LayoutItem::LayoutItem(Widget *pWidget) :
	m_itemType(ITEM_NONE),
    m_pWidget(nullptr),
	m_ratio(0.f)
{
	setWidget(pWidget);
}

LayoutItem::LayoutItem(Layout *pLayout) :
	m_itemType(ITEM_NONE),
    m_pWidget(nullptr),
	m_ratio(0.f)
{
	setLayout(pLayout);
}

LayoutItem::LayoutItem(Spacer *pSpacer) :
	m_itemType(ITEM_NONE),
    m_pWidget(nullptr),
	m_ratio(0.f)
{
	setSpacer(pSpacer);
}

/*---------------------------------------------------------------------------------------
  define the widget
---------------------------------------------------------------------------------------*/
void LayoutItem::setWidget(Widget *pWidget)
{
    O3D_ASSERT(pWidget != nullptr);

	m_itemType = ITEM_WIDGET;
	m_pWidget = pWidget;

	// widget doesn't become smaller than its initial size
	m_minSize = pWidget->size();

    if (pWidget->isFixedMinSize()) {
        pWidget->setMinSize(m_minSize);
    }

	// aspect ratio calculated from initial size
	setRatio(m_minSize);
}

/*---------------------------------------------------------------------------------------
  define the layout
---------------------------------------------------------------------------------------*/
void LayoutItem::setLayout(Layout *pLayout)
{
    O3D_ASSERT(pLayout != nullptr);

	m_itemType = ITEM_LAYOUT;
	m_pLayout = pLayout;

	// m_minSize is set later
}

/*---------------------------------------------------------------------------------------
  define the space
---------------------------------------------------------------------------------------*/
void LayoutItem::setSpacer(Spacer *pSpacer)
{
    O3D_ASSERT(pSpacer != nullptr);

	m_itemType = ITEM_SPACER;
	m_pSpacer = pSpacer;
	m_minSize = pSpacer->size();

	setRatio(m_minSize);
}

/*---------------------------------------------------------------------------------------
  set the layout item dimensions
---------------------------------------------------------------------------------------*/
void LayoutItem::setDimension(const Vector2i &_pos, const Vector2i &_size)
{
    O3D_ASSERT(m_pWidget != nullptr);

	Vector2i pos(_pos);
	Vector2i size(_size);

	// aspect ratio
    if (m_pWidget->isShaped()) {
		// adjust aspect ratio
        Int32 rwidth = (Int32)(size.y() * m_ratio);
        if (rwidth > size.x()) {
			// fit horizontally
            Int32 rheight = (Int32)(size.x() / m_ratio);

			// add vertical space
            if (m_pWidget->getVerticalAlign() == WidgetProperties::MIDDLE_ALIGN) {
                pos.y() += (size.y() - rheight) / 2;
            } else if (m_pWidget->getVerticalAlign() == WidgetProperties::BOTTOM_ALIGN) {
                pos.y() += (size.y() - rheight);
            }

			// use reduced dimensions
            size.y() = rheight;
        } else if (rwidth < size.x()) {
			// add horizontal space
            if (m_pWidget->getHorizontalAlign() == WidgetProperties::CENTER_ALIGN) {
                pos.x() += (size.x() - rwidth) / 2;
            } else if (m_pWidget->getHorizontalAlign() == WidgetProperties::RIGHT_ALIGN) {
                pos.x() += (size.x() - rwidth);
            }

            size.x() = rwidth;
		}
	}

	// This is what getPosition() returns. Since we calculate borders afterwards,
	// getPos() will be the top-left corner of the surrounding border
	m_pos = pos;

    if (m_pWidget->getBorderElt() & WidgetProperties::BORDER_LEFT) {
        pos.x() += m_pWidget->getBorderSize();
        size.x() -= m_pWidget->getBorderSize();
	}

	if (m_pWidget->getBorderElt() & WidgetProperties::BORDER_RIGHT)	{
        size.x() -= m_pWidget->getBorderSize();
	}

    if (m_pWidget->getBorderElt() & WidgetProperties::BORDER_TOP) {
        pos.y() += m_pWidget->getBorderSize();
        size.y() -= m_pWidget->getBorderSize();
	}

    if (m_pWidget->getBorderElt() & WidgetProperties::BORDER_BOTTOM) {
        size.y() -= m_pWidget->getBorderSize();
	}

    if (size.x() < 0) {
        size.x() = 0;
    }
    if (size.y() < 0) {
        size.y() = 0;
    }

	m_rect = Box2i(pos,size);

	switch (m_itemType)	{
		case ITEM_NONE:
			O3D_WARNING("Undefined layout item type");
			break;

		case ITEM_WIDGET:
            m_pWidget->setSize(pos.x(), pos.y(), size.x(), size.y(), Widget::SIZE_ALLOW_MINUS_ONE);
			break;

		case ITEM_LAYOUT:
			m_pLayout->setDimension(pos,size);
			break;

		case ITEM_SPACER:
            m_pSpacer->setSize(size);
			break;
	}
}

Vector2i LayoutItem::getMinSizeIncludeBorders() const
{
	// TODO see for window first layout borders...
	Vector2i minSize(m_minSize);

    if (m_pWidget->getBorderElt() & WidgetProperties::BORDER_LEFT) {
        minSize.x() += m_pWidget->getBorderSize();
    }

    if (m_pWidget->getBorderElt() & WidgetProperties::BORDER_RIGHT) {
        minSize.x() += m_pWidget->getBorderSize();
    }

    if (m_pWidget->getBorderElt() & WidgetProperties::BORDER_TOP) {
        minSize.y() += m_pWidget->getBorderSize();
    }

    if (m_pWidget->getBorderElt() & WidgetProperties::BORDER_BOTTOM) {
        minSize.y() += m_pWidget->getBorderSize();
    }

	return minSize;
}

//---------------------------------------------------------------------------------------
// return the minimal size of the widget item
//---------------------------------------------------------------------------------------
Vector2i LayoutItem::calcMin()
{
    O3D_ASSERT(m_pWidget != nullptr);

	if (isLayout())	{
        m_minSize = m_pLayout->getMinSize();

		// if we have to preserve aspect ratio and this is the first-time calculation,
		// consider ration to be initial size
        if (m_pLayout->isShaped() && (m_ratio == 0.f)) {
			setRatio(m_minSize);
        }
    } else if (isWidget()) {
		// Since the size of the widget may change during runtime, we should use
		// the current minimal/best size
		m_minSize = m_pWidget->getEffectiveMinSize();
	}

	return getMinSizeIncludeBorders();
}
