/**
 * @file tabbedwidget.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/tabbedwidget.h"
#include "o3d/gui/widgets/window.h"
#include "o3d/gui/widgets/menuwidget.h"
#include "o3d/gui/widgets/pane.h"
#include "o3d/gui/widgets/glasspane.h"

#include "o3d/engine/context.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/gui/widgets/layout.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"
#include "o3d/gui/widgets/statictext.h"
#include "o3d/gui/widgetdrawcompass.h"

using namespace o3d;

class TabbedWidgetMenu : public GlassPane
{
public:

	//! Creation constructor as a top-level window
	TabbedWidgetMenu(
			WidgetManager *parent,
			const Vector2i &glassPaneSize,
			const Vector2i &pos = DEFAULT_POS,
			const Vector2i &size = DEFAULT_SIZE) :
		GlassPane(
			parent,
			Vector2i(0, 0),
			glassPaneSize,
			"TabbedWidgetGlassPane")
	{
		m_menu = new MenuWidget(
					getLayout(),
					pos,
					size);

		getLayout()->addWidget(m_menu);

		layout();
	}

	virtual ~TabbedWidgetMenu()
	{
	}

	MenuWidget* getMenu()
	{
		return m_menu;
	}

private:

	TabbedWidgetMenu(BaseObject *parent) :
		GlassPane(parent)
	{
	}

	MenuWidget *m_menu;
};

O3D_IMPLEMENT_DYNAMIC_CLASS1(TabbedWidget, GUI_WIDGET_TABBED, Widget)

TabbedWidget::TabbedWidget(BaseObject *parent) :
	Widget(parent),
	m_activeTab(nullptr),
	m_hoverTab(nullptr),
	m_curX(0)
{
}

TabbedWidget::TabbedWidget(
	Widget *parent,
	const Vector2i &pos,
	const Vector2i &size,
	const String &name) :
		Widget(parent, pos, size, name),
		m_activeTab(nullptr),
		m_hoverTab(nullptr),
		m_curX(0)
{
	init();
}

TabbedWidget::~TabbedWidget()
{
	for (IT_TabVector it = m_tabs.begin(); it != m_tabs.end(); ++it)
	{
		deletePtr((*it)->icon);
		deletePtr(*it);
	}
}

void TabbedWidget::init()
{
	m_horiAlign = LEFT_ALIGN;
	m_vertAlign = TOP_ALIGN;

	// calc the default tab size
	const Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::TAB_NORMAL);

	if (m_minSize.x() < 0)
		m_minSize.x() = drawMode->getDefaultSize().x();

	if (m_minSize.y() < 0)
		m_minSize.y() = max<Int32>(
					drawMode->getDefaultSize().y(),
					theme->getDefaultFontSize() + drawMode->getTopMargin() + drawMode->getBottomMargin());

	m_size = m_minSize;

	m_dropDownButton = new ToolButton(
				this,
				Theme::TAB_BUTTON_LIST_FALSE,
				Theme::TAB_BUTTON_LIST_TRUE,
                Theme::TAB_BUTTON_LIST_HOVER,
                Theme::WIDGET_NONE,
				ToolButton::PUSH_BUTTON);

	m_dropDownButton->setPos(
				m_size.x() - m_dropDownButton->size().x() - getBorderSize(),
				(m_size.y() - m_dropDownButton->size().y()) / 2);

    m_dropDownButton->onToolButtonReleased.connect(this, &TabbedWidget::dropDownButton);
}

Int32 TabbedWidget::addTab(const String &label,
		Icon *icon,
		Bool closable,
		TabbedWidgetData *data)
{
	// compute an unique id
	Int32 id = 0;
	for (IT_TabVector it = m_tabs.begin(); it != m_tabs.end(); ++it)
	{
		id = max(id, (*it)->id + 1);
	}

	Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::TAB_NORMAL);

	Tab *tab = new Tab;
	tab->data = data;
	tab->icon = icon;
	tab->id = id;
	tab->pos = Vector2i(m_curX, 0);
	tab->label.setText(label);
	tab->label.setPos(tab->pos.x(), 0);// + drawMode->getLeftMargin(), drawMode->getTopMargin());
	tab->label.setHorizontalAlignment(TextZone::CENTER_ALIGN);
	tab->label.setVerticalAlignment(TextZone::MIDDLE_ALIGN);
	tab->state = TAB_NORMAL;
	tab->order = m_tabs.size();

	// text color
	tab->label.setColor(theme->getWidgetDrawMode(Theme::TAB_NORMAL)->getDefaultTextColor());

	// calc the default tab size
	Vector2i defaultSize = getUsedTheme()->getDefaultWidgetSize(Theme::TAB_NORMAL);

	Vector2i textSize = tab->label.sizeOf(
		theme->getDefaultFont(),
		theme->getDefaultFontSize());

	// respect margins
	textSize += drawMode->getTopLeftMargin() + drawMode->getBottomRightMargin();

	tab->size.x() = o3d::max(textSize.x() ,defaultSize.x());
	tab->size.y() = o3d::max(textSize.y(), defaultSize.y());

	// adjust widget size
	m_size.x() = max(m_minSize.x(), m_size.x() + tab->size.x());
	m_size.y() = max(m_minSize.y(), tab->size.y());

	// advance
	m_curX += tab->size.x();

    tab->label.setSize(tab->size.x(), tab->size.y());

	// add it
	m_tabs.push_back(tab);

	// first added tab is active
	if (m_activeTab == nullptr)
	{
		m_activeTab = tab;
		m_activeTab->state = TAB_ACTIVE;
	}

	setDirty();
	return tab->id;
}

Int32 TabbedWidget::insertTab(
		Int32 pos,
		const String &label,
		Icon *icon,
		Bool closable,
		TabbedWidgetData *data)
{
	// compute an unique id
	Int32 id = -1;
	Int32 curPos = 0;
	IT_TabVector itPos;
	for (IT_TabVector it = m_tabs.begin(); it != m_tabs.end(); ++it, ++curPos)
	{
		id = max(id, (*it)->id);

		// insert position
		if (curPos == pos)
			itPos = it;
	}

	Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::TAB_NORMAL);

	Tab *tab = new Tab;
	tab->data = data;
	tab->icon = icon;
	tab->id = id;
	tab->pos = Vector2i(m_curX, 0);
	tab->label.setText(label);
	tab->label.setPos(tab->pos.x(), 0);// + drawMode->getLeftMargin(), drawMode->getTopMargin());
	tab->label.setHorizontalAlignment(TextZone::CENTER_ALIGN);
	tab->label.setVerticalAlignment(TextZone::MIDDLE_ALIGN);
	tab->state = TAB_NORMAL;
	tab->order = pos;

	// text color
	tab->label.setColor(theme->getWidgetDrawMode(Theme::TAB_NORMAL)->getDefaultTextColor());

	// calc the default tab size
	Vector2i defaultSize = getUsedTheme()->getDefaultWidgetSize(Theme::TAB_NORMAL);

	Vector2i textSize = tab->label.sizeOf(
		theme->getDefaultFont(),
		theme->getDefaultFontSize());

	// respect margins
	textSize += drawMode->getTopLeftMargin() + drawMode->getBottomRightMargin();

	tab->size.x() = o3d::max(textSize.x() ,defaultSize.x());
	tab->size.y() = o3d::max(textSize.y(), defaultSize.y());

	// adjust widget size
	m_size.x() = max(m_minSize.x(), m_size.x() + tab->size.x());
	m_size.y() = max(m_minSize.y(), tab->size.y());

	// advance
	m_curX += tab->size.x();

    tab->label.setSize(tab->size.x(), tab->size.y());

	// add it
	m_tabs.insert(itPos, tab);

	// first added tab is active
	if (m_activeTab == nullptr)
	{
		m_activeTab = tab;
		m_activeTab->state = TAB_ACTIVE;
	}

	setDirty();
	return tab->id;
}

void TabbedWidget::removeTab(Int32 tabId)
{
	IT_TabVector it = m_tabs.begin();
	while (it != m_tabs.end())
	{
		if ((*it)->id == tabId)
			break;

		++it;
	}

	if (it == m_tabs.end())
		return;

	Tab *tab = *it;

	if (m_activeTab == tab)
		m_activeTab = nullptr;

	// TODO detect the new hovered tab if the mouse is hovering...
	if (m_hoverTab == tab)
		m_hoverTab = nullptr;

	// advance and pos for following tabs
	if (tab->order < m_tabs.size()-1)
	{
		for (UInt32 i = tab->order+1; i < m_tabs.size(); ++i)
		{
			m_tabs[i]->pos.x() -= tab->size.x();
            m_tabs[i]->label.setPosX(m_tabs[i]->label.pos().x() - tab->size.x());
		}
	}

	// new size
	m_curX -= tab->size.x();
	m_size.x() -= tab->size.x();

	deletePtr(*it);

	m_tabs.erase(it);

	// select the last tab
	if (!m_tabs.empty())
	{
		m_activeTab = m_tabs.back();
		m_activeTab->state = TAB_ACTIVE;

		// signal
		onActivateTab(m_activeTab->id);
	}

	setDirty();
}

void TabbedWidget::removeTabAt(UInt32 tabPos)
{
	if (tabPos >= m_tabs.size())
		O3D_ERROR(E_IndexOutOfRange("Tab position"));

	IT_TabVector it = m_tabs.begin() + tabPos;

	if (it == m_tabs.end())
		return;

	Tab *tab = *it;

	if (m_activeTab == tab)
		m_activeTab = nullptr;

	// TODO detect the new hovered tab if the mouse is hovering...
	if (m_hoverTab == tab)
		m_hoverTab = nullptr;

	// advance and pos for following tabs
	if (tab->order < m_tabs.size()-1)
	{
		for (UInt32 i = tab->order+1; i < m_tabs.size(); ++i)
		{
			m_tabs[i]->pos.x() -= tab->size.x();
            m_tabs[i]->label.setPosX(m_tabs[i]->label.pos().x() - tab->size.x());
		}
	}

	// new size
	m_curX -= tab->size.x();
	m_size.x() -= tab->size.x();

	deletePtr(*it);

	m_tabs.erase(it);

	// select the last tab
	if (!m_tabs.empty())
	{
		m_activeTab = m_tabs.back();
		m_activeTab->state = TAB_ACTIVE;

		// signal
		onActivateTab(m_activeTab->id);
	}

	setDirty();
}

void TabbedWidget::setActiveTab(Int32 id)
{
	if (m_activeTab != nullptr)
	{
		if (m_activeTab->id == id)
			return;

		m_activeTab->state = TAB_NORMAL;
	}

	for (Tab *tab : m_tabs)
	{
		if (tab->id == id)
		{
			m_activeTab = tab;
			m_activeTab->state = TAB_ACTIVE;

			setDirty();

			return;
		}
	}
}

const TabbedWidget::Tab* TabbedWidget::getTab(Int32 index) const
{
	if (index >= 0 && (size_t)index < m_tabs.size())
		return m_tabs[index];

	return nullptr;
}

Int32 TabbedWidget::getActiveTab() const
{
	if (m_activeTab != nullptr)
		return m_activeTab->id;

	return -1;
}

TabbedWidgetData* TabbedWidget::getActiveTabData()
{
	if (m_activeTab != nullptr)
		return m_activeTab->data;

	return nullptr;
}

const TabbedWidgetData* TabbedWidget::getActiveTabData() const
{
	if (m_activeTab != nullptr)
		return m_activeTab->data;

	return nullptr;
}

void TabbedWidget::adjustBestSize()
{
	// calc the default tab size
	const Theme *theme = getUsedTheme();
	const WidgetDrawMode *drawMode = theme->getWidgetDrawMode(Theme::TAB_NORMAL);

	if (m_minSize.x() < 0)
		m_minSize.x() = drawMode->getDefaultSize().x();

	if (m_minSize.y() < 0)
		m_minSize.y() = max<Int32>(
					drawMode->getDefaultSize().y(),
					theme->getDefaultFontSize() + drawMode->getTopMargin() + drawMode->getBottomMargin());

	m_size = m_minSize;
}

void TabbedWidget::dropDownButton()
{
    Vector2i guiSize(getGui()->getWidth(), getGui()->getHeight());

	TabbedWidgetMenu *menuPane = new TabbedWidgetMenu(
                getGui()->getWidgetManager(),
				guiSize);

	for (Tab *tab : m_tabs)
	{
		MenuLabelItem *menuLabel = new MenuLabelItem(
					menuPane->getMenu(),
					tab->label.text());

		menuLabel->setId(tab->id);
		menuPane->getMenu()->addElement(menuLabel);
	}

	// set the menu position near the button
	Box2i menuBox(m_dropDownButton->getAbsPos(), menuPane->getMenu()->size());

	if (menuBox.x2() > guiSize.x())
		menuBox.x() -= menuBox.x2() - guiSize.x();

	if (menuBox.y2() > guiSize.y())
		menuBox.y() -= menuBox.y2() - guiSize.y();

    menuPane->getMenu()->setPos(menuBox.pos() - menuPane->getMenu()->getOrigin() - Vector2i(1, 1));

    getGui()->getWidgetManager()->addWindowForegound(menuPane);

	menuPane->getMenu()->setFocus();

    menuPane->getMenu()->onSelectMenuItem.connect(this, &TabbedWidget::menuSelectTab);
}

void TabbedWidget::menuSelectTab(Widget* widget)
{
	// select the tab
	setActiveTab(widget->getId());

	// and delete the window asynchronously
	GlassPane *wnd = (GlassPane*)widget->getTopLevelWidget();
	wnd->onDeleteGlassPane();
}

Vector2i TabbedWidget::getDefaultSize()
{
	Vector2i size = getUsedTheme()->getDefaultWidgetSize(Theme::TAB_NORMAL);
	return Vector2i(size.x(), size.y());
}

Bool TabbedWidget::isTargeted(Int32 x, Int32 y, Widget *&pWidget)
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

		// drop down button
		if (m_dropDownButton->isTargeted(
            x - m_dropDownButton->pos().x(),
            y - m_dropDownButton->pos().y(),
			pWidget))
		{
			return True;
		}

		return True;
	}

	return False;
}

void TabbedWidget::sizeChanged()
{
//	m_textZone.setSize(m_size.x(),m_size.y());

	// update is needed
	setDirty();
}

// Mouse events
Bool TabbedWidget::mouseLeftPressed(Int32 x,Int32 y)
{
	if ((m_hoverTab != nullptr) && (m_activeTab != m_hoverTab))
	{
        getGui()->getWidgetManager()->lockWidgetMouse();

		if (m_activeTab != nullptr)
		{
			m_activeTab->state = TAB_NORMAL;
			onReleaseTab(m_activeTab->id);
		}

		m_activeTab = m_hoverTab;

		setDirty();

		// signal
		onActivateTab(m_activeTab->id);

		return True;
	}

	return False;
}

Bool TabbedWidget::mouseLeftReleased(Int32 x,Int32 y)
{
    getGui()->getWidgetManager()->unlockWidgetMouse();

	return False;
}

Bool TabbedWidget::mouseMove(Int32 x,Int32 y)
{
    x -= m_pos.x();
    y -= m_pos.y();

	// tab hover
    if ((x < m_size.x()) && (y < m_size.y()))
	{
		for (Tab *tab : m_tabs)
		{
			Box2i box(tab->pos, tab->size);
			if (box.isInside(Vector2i(x, y)))
			{
				// different hovered tab
				if (m_hoverTab != tab)
				{
					if (m_hoverTab != nullptr)
					{
						if (m_hoverTab == m_activeTab)
							m_hoverTab->state = TAB_ACTIVE;
						else
							m_hoverTab->state = TAB_NORMAL;
					}

					m_hoverTab = tab;
					m_hoverTab->state = TAB_HOVER;

					setDirty();
				}

				return True;
			}
		}
	}

	// nothing hover, reset previous state
	if (m_hoverTab != nullptr)
	{
		if (m_hoverTab == m_activeTab)
			m_hoverTab->state = TAB_ACTIVE;
		else
			m_hoverTab->state = TAB_NORMAL;

		m_hoverTab = nullptr;

		setDirty();

		return True;
	}

	return False;
}

void TabbedWidget::mouseMoveIn()
{
	if (m_hoverTab != nullptr)
	{
		if (m_activeTab == m_hoverTab)
			m_hoverTab->state = TAB_ACTIVE;
		else
			m_hoverTab->state = TAB_NORMAL;

		m_hoverTab = nullptr;
		setDirty();
	}

	// signal
	onEnterWidget();
}

void TabbedWidget::mouseMoveOut()
{
	if (m_hoverTab != nullptr)
	{
		if (m_activeTab == m_hoverTab)
			m_hoverTab->state = TAB_ACTIVE;
		else
			m_hoverTab->state = TAB_NORMAL;

		m_hoverTab = nullptr;
		setDirty();
	}

	// signal
	onLeaveWidget();
}

void TabbedWidget::focused()
{
	// TODO focused state
	//setDirty();

	// signal
	onFocus();
}

void TabbedWidget::lostFocus()
{
	// TODO focused state
	//setDirty();

	// signal
	onLostFocus();
}

//---------------------------------------------------------------------------------------
// draw
//---------------------------------------------------------------------------------------
void TabbedWidget::draw()
{
	if (isShown())
		processCache();

	Vector2i org = getOrigin();

	// translate in local coordinates
	getScene()->getContext()->modelView().translate(
				Vector3((Float)m_pos.x(), (Float)m_pos.y(), 0.f));

	// translate in client area coordinates
	getScene()->getContext()->modelView().translate(
                Vector3((Float)org.x(), (Float)org.y(), 0.f));

	getScene()->getContext()->modelView().push();
	m_dropDownButton->draw();
	getScene()->getContext()->modelView().pop();
}

void TabbedWidget::updateCache()
{
	Theme *theme = getUsedTheme();

	for (Tab *tab : m_tabs)
	{
		// outside
        if (tab->pos.x() + tab->size.x() >= m_dropDownButton->pos().x())
			break;

		if (tab->state == TAB_ACTIVE)
			theme->drawWidget(Theme::TAB_ACTIVE, tab->pos.x(), tab->pos.y(), tab->size.x(), tab->size.y());
		else if (tab->state == TAB_DISABLED)
			theme->drawWidget(Theme::TAB_DISABLED, tab->pos.x(), tab->pos.y(), tab->size.x(), tab->size.y());
		else if (tab->state == TAB_HOVER)
            theme->drawWidget(Theme::TAB_HOVER, tab->pos.x(), tab->pos.y(), tab->size.x(), tab->size.y());
		else if (tab->state == TAB_NORMAL)
			theme->drawWidget(Theme::TAB_NORMAL, tab->pos.x(), tab->pos.y(), tab->size.x(), tab->size.y());

		tab->label.write(
					theme->getDefaultFont(),
					theme->getDefaultFontSize());
	}
}

TabbedWidget::Tab::Tab() :
	data(nullptr)
{
}

TabbedWidget::Tab::~Tab()
{
	deletePtr(data);
}

