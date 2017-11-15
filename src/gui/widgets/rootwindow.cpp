/**
 * @file rootwindow.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/rootwindow.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"
#include "o3d/gui/widgets/toolbutton.h"
#include "o3d/gui/widgetdrawcompass.h"
#include "o3d/gui/widgets/boxlayout.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(RootWindow, GUI_WIDGET_ROOT_WINDOW, Widget)

//! Default constructor.
RootWindow::RootWindow(BaseObject *parent) :
        Widget(parent)
        //m_focusWidget(nullptr)
{
}

//! Creation constructor as a top-level window
RootWindow::RootWindow(
    WidgetManager *parent,
    const Vector2i &pos,
    const Vector2i &size,
    const String &name) :
        Widget(parent, pos, size, name)
        //m_focusWidget(nullptr)
{
    m_pos = pos;
    m_minSize = m_size = size;

    init();
}

void RootWindow::init()
{
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

    // update the window
    sizeChanged();
}

RootWindow::~RootWindow()
{
}

// set the parent layout. By default an HLayout is setup.
void RootWindow::setLayout(Layout *pLayout)
{
    m_pLayout = pLayout;
}

// get the parent layout. By default an HLayout is setup. (const version)
const Layout* RootWindow::getLayout() const
{
    return m_pLayout.get();
}

// get the parent layout. By default an HLayout is setup.
Layout* RootWindow::getLayout()
{
    return m_pLayout.get();
}

// Get the current cursor type name for this widget if targeted.
String RootWindow::getCursorTypeName() const
{
    return "Default";
}

void RootWindow::show(Bool show)
{
    if (show != m_capacities.getBit(STATE_VISIBILITY))
        m_capacities.setBit(STATE_VISIBILITY, show);
}

// return the top-left absolute position of the widget (containing the local origin)
Vector2i RootWindow::getOrigin() const
{
    return Vector2i(0, 0);
}

// get the recommended widget default size
Vector2i RootWindow::getDefaultSize()
{
    return Vector2i(1, 1);
}

// get the extra size of the window depending of its defined theme
Vector2i RootWindow::getWindowBorderSize() const
{
    return Vector2i(0, 0);
}

Bool RootWindow::isTargeted(Int32 x,Int32 y,Widget *&pWidget)
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

Bool RootWindow::mouseLeftPressed(Int32 x,Int32 y)
{
    return False;
}

Bool RootWindow::mouseLeftReleased(Int32 x,Int32 y)
{
    return False;
}

Bool RootWindow::mouseMove(Int32 x,Int32 y)
{
    return False;
}

void RootWindow::positionChanged()
{
    // Nothing
}

void RootWindow::focused()
{
    if (!m_capacities.getBit(STATE_FOCUSED))
    {
        m_capacities.enable(STATE_FOCUSED);

        // signal
        onFocus();
    }
}

void RootWindow::lostFocus()
{
    if (m_capacities.getBit(STATE_FOCUSED))
    {
        m_capacities.disable(STATE_FOCUSED);

        // signal
        onLostFocus();
    }
}

Vector2i RootWindow::getClientSize() const
{
    return m_size;
}

void RootWindow::sizeChanged()
{
    setDirty();
}

Bool RootWindow::keyboardToggled(Keyboard *keyboard, KeyEvent event)
{
    return False;
}

void RootWindow::draw()
{
    if (isShown())
    {
        // is need to recompute the layout
        if (getLayout() && getLayout()->isDirty())
            getLayout()->layout();

        Vector2i org = getOrigin();

        //processCache();

        // translate in window coordinates
        getScene()->getContext()->modelView().translate(
                Vector3((Float)m_pos.x(), (Float)m_pos.y(), 0.f));

        // translate in client area window coordinates
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
}

void RootWindow::updateCache()
{
}

