/**
 * @file tooltip.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/tooltip.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/gui/widgets/boxlayout.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(ToolTip, GUI_WIDGET_TOOL_TIP, Widget)

ToolTip::ToolTip(BaseObject *parent) :
    Widget(parent)
{

}

ToolTip::ToolTip(Widget *parent,
    const Vector2i &pos,
    const Vector2i &size,
    const String &name) :
        Widget(parent, pos, size, name)
{
     init();
}

void ToolTip::init()
{
    // default position
    if (m_pos.x() < 0)
        m_pos.x() = 0;
    if (m_pos.y() < 0)
        m_pos.y() = 0;

    // default size
    Vector2i defaultSize = getDefaultSize();
    if (m_size.x() < 0)
        m_minSize.x() = m_size.x() = defaultSize.x();
    if (m_size.y() < 0)
        m_minSize.y() = m_size.y() = defaultSize.y();

    // default layout
    m_pLayout = new BoxLayout(this, BoxLayout::VERTICAL_LAYOUT);
    m_pLayout->setParent(this);
    m_pLayout->defineLayoutItem();

    m_pLayout->setSize(getClientSize());
}

ToolTip::~ToolTip()
{
}

void ToolTip::setLayout(Layout *pLayout)
{
    m_pLayout = pLayout;
    if (m_pLayout.isValid())
        m_pLayout->setSize(getClientSize());
}

const Layout* ToolTip::getLayout() const
{
    return m_pLayout.get();
}

Layout* ToolTip::getLayout()
{
    return m_pLayout.get();
}

Vector2i ToolTip::getOrigin() const
{
    const Theme *theme = getUsedTheme();
    const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::TOOL_TYPE_NORMAL);

    Vector2i org = drawMode->getTopLeftMargin();
    return org;
}

Vector2i ToolTip::getClientSize() const
{
    // compute the client size
    const Theme *theme = getUsedTheme();
    const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::TOOL_TYPE_NORMAL);
    if (!drawMode)
        return m_size;

    Vector2i size = m_size;
    size -= drawMode->getTopLeftMargin() + drawMode->getBottomRightMargin();

    return size;
}

Vector2i ToolTip::getDefaultSize()
{
    const Theme *theme = getUsedTheme();
    const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::TOOL_TYPE_NORMAL);

    return drawMode->getDefaultSize();
}

void ToolTip::sizeChanged()
{
    if (getLayout())
        getLayout()->setSize(getClientSize());

    setDirty();
}

Bool ToolTip::isTargeted(Int32 x, Int32 y, Widget *&pWidget)
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

//---------------------------------------------------------------------------------------
// draw
//---------------------------------------------------------------------------------------
void ToolTip::draw()
{
    if (!isShown())
        return;

    // is need to recompute the layout
    if (getLayout() && getLayout()->isDirty())
        getLayout()->layout();

    Vector2i org = getOrigin();

    processCache();

    // translate in local coordinates
    getScene()->getContext()->modelView().translate(
                Vector3((Float)m_pos.x(), (Float)m_pos.y(), 0.f));

    // translate in client area local coordinates
    getScene()->getContext()->modelView().translate(
                Vector3((Float)org.x(), (Float)org.y(),0.f));

    // finally draw its content into the client area
    if (getLayout())
    {
        getScene()->getContext()->modelView().push();
        getLayout()->draw();
        getScene()->getContext()->modelView().pop();
    }
}

void ToolTip::updateCache()
{
    Theme *theme = getUsedTheme();

    // Draw borders
    theme->drawWidget(
                Theme::TOOL_TYPE_NORMAL,
                0,
                0,
                m_size.x(),
                m_size.y());

    setClean();
}

