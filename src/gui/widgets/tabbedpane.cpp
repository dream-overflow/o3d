/**
 * @file tabbedpane.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/tabbedpane.h"
#include "o3d/gui/widgetdrawmode.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/gui/widgets/boxlayout.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(TabbedPane, GUI_WIDGET_TABBED_PANE, Widget)

TabbedPane::TabbedPane(BaseObject *parent) :
	Widget(parent),
	m_tabbedPaneStyle(DEFAULT_STYLE)
{
}

TabbedPane::TabbedPane(
	Widget *parent,
	const Vector2i &pos,
	const Vector2i &size,
	TabbedPaneStyle style,
	const String &name) :
		Widget(parent, pos, size, name),
		m_tabbedPaneStyle(style)
{
	init();
}

void TabbedPane::init()
{
    m_capacities.enable(CAPS_CONTAINER);

	// default position
    if (m_pos.x() < 0) m_pos.x() = 0;
    if (m_pos.y() < 0) m_pos.y() = 0;

	// default size
	Vector2i defaultSize = getDefaultSize();
    if (m_size.x() < 0) m_minSize.x() = m_size.x() = defaultSize.x();
    if (m_size.y() < 0) m_minSize.y() = m_size.y() = defaultSize.y();

	//const Theme *theme = getUsedTheme();
	//const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::FRAME_NORMAL);

	m_tabs = new TabbedWidget(this, Vector2i(1, 0), Vector2i(m_size.x()-2, -1));

	// border rect
	m_borderRect.set(
				Vector2i(0, m_tabs->size().y() - 3),
				Vector2i(m_size.x(), m_size.y() - m_tabs->size().y() + 3));

	// tabs are before the origin
	m_tabs->setPos(-getOrigin() + Vector2i(1, 0));

    m_tabs->onActivateTab.connect(this, &TabbedPane::activateTab);
    m_tabs->onReleaseTab.connect(this, &TabbedPane::releaseTab);
}

TabbedPane::~TabbedPane()
{
}

TabbedPaneElement* TabbedPane::addTab(
		Widget *widget,
		const String &label,
		Icon *icon,
		Bool closable)
{
	TabbedPaneElement *elt = new TabbedPaneElement(this, widget);
	Int32 tabId = m_tabs->addTab(label, icon, closable, elt);

	elt->setIndex(tabId);

	return elt;
}

TabbedPaneElement* TabbedPane::insertTab(
		Widget *widget,
		Int32 pos,
		const String &label,
		Icon *icon,
		Bool closable)
{
	TabbedPaneElement *elt = new TabbedPaneElement(this, widget);
	Int32 tabId = m_tabs->insertTab(pos, label, icon, closable, elt);

	elt->setIndex(tabId);

	return elt;
}

void TabbedPane::removeTab(Int32 tabId)
{
	m_tabs->removeTab(tabId);
}

void TabbedPane::removeTabAt(UInt32 tabPos)
{
	m_tabs->removeTabAt(tabPos);
}

void TabbedPane::setActiveTab(Int32 id)
{
	m_tabs->setActiveTab(id);
}

Int32 TabbedPane::getActiveTab() const
{
	return m_tabs->getActiveTab();
}

TabbedPaneElement *o3d::TabbedPane::getActiveTabElt() const
{
	return (TabbedPaneElement*)m_tabs->getActiveTabData();
}

const TabbedPaneElement* TabbedPane::getTab(Int32 index) const
{
	if (index >= 0 && index < m_tabs->getNumTabs())
		return (const TabbedPaneElement*)m_tabs->getTab(index)->data;

	return nullptr;
}

TabbedPaneElement* TabbedPane::getTab(Int32 index)
{
	if (index >= 0 && index < m_tabs->getNumTabs())
		return (TabbedPaneElement*)m_tabs->getTab(index)->data;

	return nullptr;
}

Vector2i TabbedPane::getDefaultSize()
{
	return getUsedTheme()->getDefaultWidgetSize(Theme::FRAME_NORMAL);
}

Vector2i TabbedPane::getClientSize() const
{
	// compute the client size
	const Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::FRAME_NORMAL);
	if (!drawMode)
		return m_size;

	Vector2i size = m_borderRect.size();

	if ((m_tabbedPaneStyle & BORDER_STYLE) != 0)
	{
        size -= drawMode->getTopLeftMargin() + drawMode->getBottomRightMargin();
	}

	return size;
}

Vector2i TabbedPane::getOrigin() const
{
	if ((m_tabbedPaneStyle & BORDER_STYLE) != 0)
	{
		const Theme *theme = getUsedTheme();
		const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::FRAME_NORMAL);

		Vector2i org(0, m_borderRect.y());
		org += drawMode->getTopLeftMargin();

		return org;
	}
	else
		return Vector2i(0, m_tabs->size().y());
}

Bool TabbedPane::isTargeted(Int32 x, Int32 y, Widget *&pWidget)
{
	// no negative part
	if (x < 0 || y < 0)
		return False;

	if ((x < m_size.x()) && (y < m_size.y()))
	{
		// others childs widgets
		Vector2i org = getOrigin();
		pWidget = this;

		// delta by the client origin for childs widgets
		// title bar buttons are in childs space, using
		// negatives coordinates.
		x -= org.x();
		y -= org.y();

		// tab
		if (m_tabs->isTargeted(
            x - m_tabs->pos().x(),
            y - m_tabs->pos().y(),
			pWidget))
		{
			return True;
		}

		// recursive targeted on the current tab
		TabbedPaneElement *elt = (TabbedPaneElement*)m_tabs->getActiveTabData();
		if (elt != nullptr && elt->getWidget()->isTargeted(
            x - elt->getWidget()->pos().x(),
            y - elt->getWidget()->pos().y(),
			pWidget))
		{
			return True;
		}

		return True;
	}

    return False;
}

Bool TabbedPane::mouseWheel(Int32 x, Int32 y, Int32 z)
{
    if ((z < 0) && (m_tabs->getActiveTab() > 0))
        setActiveTab(m_tabs->getActiveTab() - 1);
    else if ((z > 0) && (m_tabs->getActiveTab() < m_tabs->getNumTabs()))
        setActiveTab(m_tabs->getActiveTab() + 1);

    return True;
}

void TabbedPane::sizeChanged()
{
	//const Theme *theme = getUsedTheme();

	// border rect
	if ((m_tabbedPaneStyle & BORDER_STYLE) != 0)
		m_borderRect.setSize(m_size - Vector2i(0, m_tabs->size().y()));

	if (getLayout())
        getLayout()->setSize(getClientSize());

	// update is needed
    setDirty();
}

Widget *TabbedPane::findNextTabIndex(Widget *widget, Int32 direction)
{
    return m_tabs->findNextTabIndex(widget, direction);
}

void TabbedPane::mouseMoveIn()
{
	m_capacities.enable(STATE_HOVER);

	setDirty();

	// signal
	onEnterWidget();
}

void TabbedPane::mouseMoveOut()
{
	m_capacities.disable(STATE_HOVER);

	setDirty();

	// signal
	onLeaveWidget();
}

void TabbedPane::focused()
{
	// focused
	//setDirty();

	m_capacities.enable(STATE_FOCUSED);

	// signal
	onFocus();
}

void TabbedPane::lostFocus()
{
	// focused
	//setDirty();

	m_capacities.disable(STATE_FOCUSED);

	// signal
	onLostFocus();
}

//---------------------------------------------------------------------------------------
// draw
//---------------------------------------------------------------------------------------
void TabbedPane::draw()
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

	// draw tabs
	getScene()->getContext()->modelView().push();
	m_tabs->draw();
	getScene()->getContext()->modelView().pop();

	// finally draw its content into the client area
	if (m_tabs->getActiveTabData() != nullptr)
	{
		TabbedPaneElement *elt = (TabbedPaneElement*)m_tabs->getActiveTabData();

		getScene()->getContext()->modelView().push();
		elt->getWidget()->draw();
		getScene()->getContext()->modelView().pop();
	}
}

void TabbedPane::updateCache()
{
	Theme *theme = getUsedTheme();

	// Draw borders
	if ((m_tabbedPaneStyle & BORDER_STYLE) != 0)
	{
		theme->drawWidget(
					Theme::FRAME_NORMAL,
					m_borderRect.x(),
					m_borderRect.y(),
					m_borderRect.width(),
					m_borderRect.height());
	}
}

void TabbedPane::activateTab(Int32 tabIndex)
{
	onActivateTab((TabbedPaneElement*)m_tabs->getActiveTabData());
}

void TabbedPane::releaseTab(Int32 tabIndex)
{
	onReleaseTab((TabbedPaneElement*)m_tabs->getActiveTabData());
}

TabbedPaneElement::TabbedPaneElement(TabbedPane *tabbedPane, Widget *widget) :
	m_index(-1),
	m_tabbedPane(tabbedPane),
	m_widget(widget),
	m_metadata(nullptr)
{
}

TabbedPaneElement::~TabbedPaneElement()
{
	deletePtr(m_widget);
}

void TabbedPaneElement::setMetadata(NullClass *metadata)
{
	m_metadata = metadata;
}

NullClass* TabbedPaneElement::getMetadata()
{
	return m_metadata.get();
}

const NullClass* TabbedPaneElement::getMetadata() const
{
	return m_metadata.get();
}

