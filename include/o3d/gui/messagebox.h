/**
 * @file messagebox.h
 * @brief Messages box helpers.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2009-11-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MESSAGEBOX_H
#define _O3D_MESSAGEBOX_H

#include "widgets/window.h"

namespace o3d {

class Button;

#ifdef MessageBox
#undef MessageBox
#endif

//---------------------------------------------------------------------------------------
//! @class MessageBox
//-------------------------------------------------------------------------------------
//! Helper widget do show common messages box. It use of a modal window.
//---------------------------------------------------------------------------------------
class O3D_API MessageBox : public Window
{
public:

	O3D_DECLARE_CLASS(MessageBox)

	//! Style of buttons.
	enum ButtonsStyle
	{
		OK = 0,
		OK_CANCEL,
		RETRY_CANCEL,
		YES_NO,
		YES_NO_CANCEL
	};

	//! Style of the icon.
	enum IconStyle
	{
		ICON_WARNING = 0,
		ICON_INFORMATION,
		ICON_QUESTION,
		ICON_ERROR
	};

	//! Result button.
	enum ButtonType
	{
		BUTTON_OK = 0,
		BUTTON_CANCEL,
		BUTTON_RETRY,
		BUTTON_YES,
		BUTTON_NO
	};

	static MessageBox* create(
			Widget *parent,
			const String &title = "",
			const String &message = "",
			ButtonsStyle buttonsStyle = OK,
			IconStyle iconStyle = ICON_INFORMATION);

	static MessageBox* create(
			WidgetManager *parent,
			const String &title = "",
			const String &message = "",
			ButtonsStyle buttonsStyle = OK,
			IconStyle iconStyle = ICON_INFORMATION);

	//! Default constructor.
	MessageBox(BaseObject *parent);

	//! Construct using a parent widget.
	//! @param parent The parent object.
	//! @param title Title of the window message.
	//! @param message Content of the window message.
	//! @param buttonStyle Kind of button.
	//! @param iconStyle Style of the icon.
	MessageBox(
			Widget *parent,
			const String &title,
			const String &message,
			ButtonsStyle buttonsStyle = OK,
			IconStyle iconStyle = ICON_INFORMATION);

	//! Construct using a parent widget manager.
	//! @param parent The parent object.
	//! @param title Title of the window message.
	//! @param message Content of the window message.
	//! @param buttonStyle Kind of button.
	//! @param iconStyle Style of the icon.
	MessageBox(
			WidgetManager *parent,
			const String &title,
			const String &message,
			ButtonsStyle buttonsStyle = OK,
			IconStyle iconStyle = ICON_INFORMATION);

	//! Virtual destructor.
	virtual ~MessageBox();

	//! Get a button.
	inline Button* getButton(ButtonType button) { return m_buttons[button]; }

protected:

	Button *m_buttons[5];

	void build(
			const String &title,
			const String &message,
			ButtonsStyle buttonsStyle,
			IconStyle iconStyle);
};

} // namespace o3d

#endif // _O3D_MESSAGEBOX_H

