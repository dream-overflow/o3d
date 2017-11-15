/**
 * @file toolbar.h
 * @brief A widget Tool bar
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2009-11-12
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TOOLBAR_H
#define _O3D_TOOLBAR_H

#include "widget.h"
#include "toolbutton.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class ToolBar
//-------------------------------------------------------------------------------------
//! A widget representing a toolbar
//---------------------------------------------------------------------------------------
class O3D_API ToolBar : public Widget
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(ToolBar)

	//! Toolbar orientation.
	enum Orientation
	{
		HORIZONTAL_ALIGN,
		VERTICAL_ALIGN
	};

	//! Create from a parent widget.
	//! @param parent Parent widget.
	//! @param buttonSize Size of the button that will be added.
	//! @param numButtons Number of button slot.
	//! @param space between buttons.
	ToolBar(
			Widget *parent,
			const Vector2i &buttonSize,
			UInt32 numButtons,
			Int32 space,
			Orientation orientation = HORIZONTAL_ALIGN);

	//! Virtual destructor.
	virtual ~ToolBar();


	//-----------------------------------------------------------------------------------
	// Properties
	//-----------------------------------------------------------------------------------

	//! Get the toolbar orientation.
	Orientation getOrientation() const { return m_orientation; }

	//! Change the toolbar orientation.
	void setOrientation(Orientation orientation);

	//! Set the tool button for a specified index.
	void setButton(UInt32 index, ToolButton *button);

	//! Get the tool button for a specified index.
	ToolButton* getButton(UInt32 index);

	//! Set the number of buttons slots.
	void setNumButtons(UInt32 numButtons);

	//! Get the number of buttons slots.
	inline UInt32 getNumButtons() const { return (UInt32)m_buttons.size(); }


	//-----------------------------------------------------------------------------------
	// Widget
	//-----------------------------------------------------------------------------------

	virtual Vector2i getDefaultSize();
	virtual Vector2i getOrigin() const;

	//! Is widget targeted ?
	virtual Bool isTargeted(Int32 x, Int32 y, Widget *&widget);

    //! Get the widget corresponding to next tab index or nullptr if none.
    virtual Widget* findNextTabIndex(Widget *widget, Int32 direction);


	// Draw
	virtual void draw();
	virtual void updateCache();

protected:

	//! Default constructor.
	ToolBar(BaseObject *parent);

	typedef std::vector<ToolButton*> T_ToolButtonVector;
	typedef T_ToolButtonVector::iterator IT_ToolButtonVector;
	typedef T_ToolButtonVector::const_iterator CIT_ToolButtonVector;

	Orientation m_orientation;      //!< Toolbar orientation.
	Vector2i m_toolButtonSize;      //!< Size of the tool button to add.

	T_ToolButtonVector m_buttons;   //!< Managed tool buttons.
	Int32 m_space;              //!< The space between the buttons.
};

} // namespace o3d

#endif // _O3D_TOOLBAR_H

