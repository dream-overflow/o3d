/**
 * @file messagebox.cpp
 * @brief Implementation of MessageBox.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2009-11-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"

#include "o3d/gui/widgets/statictext.h"
#include "o3d/gui/widgets/button.h"
#include "o3d/gui/widgets/boxlayout.h"
#include "o3d/gui/widgets/icon.h"

#include "o3d/gui/messagebox.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/i18n.h"
#include "o3d/gui/widgetmanager.h"

#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(MessageBox, GUI_MESSAGE_BOX, Window)

MessageBox* MessageBox::create(
		Widget *parent,
		const String &title,
		const String &message,
		MessageBox::ButtonsStyle buttonsStyle,
		MessageBox::IconStyle iconStyle)
{
	MessageBox *messageBox = new MessageBox(parent, title, message, buttonsStyle, iconStyle);
	messageBox->build(title, message, buttonsStyle, iconStyle);

	messageBox->getLayout()->setSizeHints();
	messageBox->layout();
	messageBox->center(Window::CENTER_BOTH);

	return messageBox;
}

MessageBox* MessageBox::create(
		WidgetManager *parent,
		const String &title,
		const String &message,
		MessageBox::ButtonsStyle buttonsStyle,
		MessageBox::IconStyle iconStyle)
{
	MessageBox *messageBox = new MessageBox(parent, title, message, buttonsStyle, iconStyle);

	messageBox->getLayout()->setSizeHints();
	messageBox->layout();
	messageBox->center(Window::CENTER_BOTH);

	return messageBox;
}

MessageBox::MessageBox(BaseObject *parent) :
	Window(parent)
{
	for (Int32 i = 0; i < 5; ++i)
		m_buttons[i] = nullptr;
}

// Construct using a parent window.
MessageBox::MessageBox(
		Widget *parent,
		const String &title,
		const String &message,
		ButtonsStyle buttonsStyle,
		IconStyle iconStyle) :
			Window(
				parent,
				title,
				DEFAULT_POS,
				DEFAULT_SIZE,
                BORDER_STYLE | TITLE_ICON | TITLE_BAR)
{
	for (Int32 i = 0; i < 5; ++i)
		m_buttons[i] = NULL;

	build(title, message, buttonsStyle, iconStyle);
}

// Construct using a parent window.
MessageBox::MessageBox(
		WidgetManager *parent,
		const String &title,
		const String &message,
		ButtonsStyle buttonsStyle,
		IconStyle iconStyle) :
			Window(
				parent,
				title,
				DEFAULT_POS,
				DEFAULT_SIZE,
                BORDER_STYLE | TITLE_ICON | TITLE_BAR)
{
	for (Int32 i = 0; i < 5; ++i)
		m_buttons[i] = NULL;

	build(title, message, buttonsStyle, iconStyle);
}

// Virtual destructor.
MessageBox::~MessageBox()
{

}

void MessageBox::build(
		const String &title,
		const String &message,
		ButtonsStyle buttonsStyle,
		IconStyle iconStyle)
{
	m_capacities.enable(CAPS_MODAL);

	BoxLayout *hLayout = new BoxLayout(getLayout(), BoxLayout::HORIZONTAL_LAYOUT);
	getLayout()->addWidget(hLayout);

	StaticText *staticMessage = new StaticText(hLayout, message);
	hLayout->addWidget(staticMessage);

	//
	// icons
	//

	Icon *icon = NULL;

	if (iconStyle == ICON_WARNING)
		icon = new Icon(hLayout, &getUsedTheme()->getIconSet(), "alert", Vector2i(32, 32));
	else if (iconStyle == ICON_ERROR)
		icon = new Icon(hLayout, &getUsedTheme()->getIconSet(), "cancel", Vector2i(32, 32));
	else if (iconStyle == ICON_INFORMATION)
		icon = new Icon(hLayout, &getUsedTheme()->getIconSet(), "info", Vector2i(32, 32));
	else if (iconStyle == ICON_QUESTION)
		icon = new Icon(hLayout, &getUsedTheme()->getIconSet(), "help", Vector2i(32, 32));

	if (icon)
		hLayout->addWidget(icon);

	//
	// buttons
	//

	hLayout = new BoxLayout(getLayout(), BoxLayout::HORIZONTAL_LAYOUT);
	getLayout()->addWidget(hLayout);

	if (buttonsStyle == OK)
	{
		m_buttons[BUTTON_OK] = new Button(hLayout, tr("msgBoxOk"), Widget::DEFAULT_POS, Vector2i(60, 24));
		hLayout->addWidget(m_buttons[BUTTON_OK]);
	}
	else if (buttonsStyle == OK_CANCEL)
	{
		m_buttons[BUTTON_OK] = new Button(hLayout, tr("msgBoxOk"), Widget::DEFAULT_POS, Vector2i(60, 24));
		hLayout->addWidget(m_buttons[BUTTON_OK]);

		m_buttons[BUTTON_CANCEL] = new Button(hLayout, tr("msgBoxCancel"), Widget::DEFAULT_POS, Vector2i(60, 24));
		hLayout->addWidget(m_buttons[BUTTON_CANCEL]);
	}
	else if (buttonsStyle == RETRY_CANCEL)
	{
		m_buttons[BUTTON_RETRY] = new Button(hLayout, tr("msgBoxRetry"), Widget::DEFAULT_POS, Vector2i(60, 24));
		hLayout->addWidget(m_buttons[BUTTON_RETRY]);

		m_buttons[BUTTON_CANCEL] = new Button(hLayout, tr("msgBoxCancel"), Widget::DEFAULT_POS, Vector2i(60, 24));
		hLayout->addWidget(m_buttons[BUTTON_CANCEL]);
	}
	else if (buttonsStyle == YES_NO)
	{
		m_buttons[BUTTON_YES] = new Button(hLayout, tr("msgBoxYes"), Widget::DEFAULT_POS, Vector2i(60, 24));
		hLayout->addWidget(m_buttons[BUTTON_YES]);

		m_buttons[BUTTON_NO] = new Button(hLayout, tr("msgBoxNo"), Widget::DEFAULT_POS, Vector2i(60, 24));
		hLayout->addWidget(m_buttons[BUTTON_NO]);
	}
	else if (buttonsStyle == YES_NO_CANCEL)
	{
		m_buttons[BUTTON_YES] = new Button(hLayout, tr("msgBoxYes"), Widget::DEFAULT_POS, Vector2i(60, 24));
		hLayout->addWidget(m_buttons[BUTTON_YES]);

		m_buttons[BUTTON_NO] = new Button(hLayout, tr("msgBoxNo"), Widget::DEFAULT_POS, Vector2i(60, 24));
		hLayout->addWidget(m_buttons[BUTTON_NO]);

		m_buttons[BUTTON_CANCEL] = new Button(hLayout, tr("msgBoxCancel"), Widget::DEFAULT_POS, Vector2i(60, 24));
		hLayout->addWidget(m_buttons[BUTTON_CANCEL]);
	}
}

