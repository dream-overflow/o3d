/**
 * @file theme.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_THEME_H
#define _O3D_THEME_H

#include "widgetdrawmode.h"
#include "o3d/core/smartpointer.h"
#include "o3d/core/memorydbg.h"
#include "abcfont.h"
#include "iconset.h"
#include "cursormanager.h"
#include "o3d/engine/scene/sceneentity.h"

namespace o3d {

/**
 * @brief Theme manager for widgets.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-06-21
 * Theme manager for winmanager and widgets.
 * Theme file path are relative to current working directory, and internal textures
 * file of the theme are relative to the them file path.
 *
 * Explanation of the XML theme file format
 * -----------------------------------------
 *
 * The enum WidgetDraw above, give many information about the compositing of all
 * supported widgets.
 *
 * The xml thme file is defined as following :
 *
 * <?xml version="1.0" standalone="no" ?>
 * <!-- This file contain theme definition for WindowsLook.tga -->
 * <TextureTheme>
 *    <Global texture="texture.tga" iconSet="iconsFolder" cursorSet="cursorsFolder" />
 *    <!-- texture: texture containing the theme, iconSet: standard icon folder, -->
 *    <!-- cursorSet: cursor folder -->
 *    <DefaultFont type="TrueType" name="arial.ttf" height="11" />
 *    <!-- type: 'TrueType' or 'TextureType', name: ttf or xml file, -->
 *    <!-- height: integer value -->
 * <Data>
 * <!-- Window -->
 *    <Widget name="Window" type="Compass" defaultWidth="24" defaultHeight="14">
 *    <!-- name: widget type name, type: 'Compass' or 'Simple', defaultWidth: width, -->
 *    <!-- defaultHeight: height -->
 *         <Element name="NorthWest" x="2" y="2" width="4" height="4" />
 *         <Element name="North" x="9" y="2" width="2" height="4" />
 *         <Element name="NorthEast" x="14" y="2" width="4" height="4" />
 *         <Element name="West" x="2" y="9" width="4" height="2" />
 *         <Element name="Background" x="7" y="7" width="6" height="6" />
 *         <Element name="East" x="14" y="9" width="4" height="2" />
 *         <Element name="SouthWest" x="2" y="14" width="4" height="4" />
 *         <Element name="South" x="9" y="14" width="2" height="4" />
 *         <Element name="SouthEast" x="14" y="14" width="4" height="4" />
 *         <!-- name: must define the 8 direction plus the background, -->
 *         <!-- x and y defines the position, width and height the size -->
 *     </Widget>
 *     <!-- Another example with the definition of the pushed version of the -->
 *     <!-- window, defined as a simple widget -->
 *     <Widget name="CloseButton_True" type="Simple">
 *         <Element x="2" y="50" width="13" height="13" />
 *     </Widget>
 * </Data>
 *
 * For an example of a such file, take a look at ./examples/media/gui/WindowLook.xml
 */
class O3D_API Theme : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(Theme)

	//! List all drawable object type. A widget can have more than one state.
	//! The Normal is the default state, False mean the same thing as Normal.
	//! For example Button_False mean the default appearance of a push button.
	enum WidgetDraw
	{
        WIDGET_NONE = 0,         //!< None widget

		// Window
		TITLE_BAR_FALSE,         //!< Title bar compass in false state
		TITLE_BAR_NORMAL,        //!< Title bar compass in normal state
		WINDOW_NORMAL,           //!< Window compass
		REDUCE_BUTTON_FALSE,     //!< Reduce tool button normal state
		REDUCE_BUTTON_TRUE,      //!< Reduce tool button pushed state
        REDUCE_BUTTON_HOVER,     //!< Reduce tool button mouse hover state
		MAXIMIZE_BUTTON_FALSE,   //!< Maximize/Restore tool button normal state
		MAXIMIZE_BUTTON_TRUE,    //!< Maximize/Restore tool button pushed state
        MAXIMIZE_BUTTON_HOVER,   //!< Maximize/Restore tool button mouse hover state
		CLOSE_BUTTON_FALSE,      //!< Close tool button normal state
		CLOSE_BUTTON_TRUE,       //!< Close tool button pushed state
        CLOSE_BUTTON_HOVER,      //!< Close tool button mouse hover state

		// Frame
		FRAME_NORMAL,            //!< Frame normal state
        FRAME_HOVER,             //!< Frame mouse hover state
		FRAME_DISABLED,          //!< Frame disabled state

		// Pane, use of the frame theme + an element for the label
        PANE_LABEL,              //!< Panel label area

        // Static text
        STATIC_TEXT_FALSE,       //!< Static text false state
        STATIC_TEXT_HOVER,       //!< Static text mouse hover state
        STATIC_TEXT_DISABLED,    //!< Static text disabled state

		// Button
		BUTTON_FALSE,            //!< Push/TwoState button normal state
		BUTTON_TRUE,             //!< Push/TwoState button pushed/checked state
        BUTTON_HOVER,            //!< Push/TwoState button mouse hover state
		BUTTON_DISABLED,         //!< Push/TwoState button disabled state

		// ToolButton
		TOOL_BUTTON_FALSE,       //!< Push/TwoState tool button normal state
		TOOL_BUTTON_TRUE,        //!< Push/TwoState tool button pushed/checked state
        TOOL_BUTTON_HOVER,       //!< Push/TwoState tool button mouse hover state
		TOOL_BUTTON_DISABLED,    //!< Push/TwoState tool button disabled state

		// RadioButton
		RADIO_BUTTON_TRUE,       //!< RadioButton check point element
		RADIO_BUTTON_NORMAL,     //!< RadioButton box normal state
        RADIO_BUTTON_HOVER,      //!< RadioButton box mouse hover
		RADIO_BUTTON_DISABLED,   //!< RadioButton box disabled

		// CheckBox
		CHECK_BOX_TRUE,          //!< CheckBox check cross element
		CHECK_BOX_THIRD,         //!< CheckBox third-state cross element
		CHECK_BOX_NORMAL,        //!< CheckBox box normal state
        CHECK_BOX_HOVER,         //!< CheckBox box mouse hover
		CHECK_BOX_DISABLED,      //!< CheckBox box disabled

		// ListBox
		LISTBOX_NORMAL,          //!< ListBox normal state
        LISTBOX_HOVER,           //!< ListBox hover state
		LISTBOX_DISABLED,        //!< ListBox disabled state

		// EditBox
		EDITBOX_NORMAL,          //!< EditBox normal state
        EDITBOX_HOVER,           //!< EditBox hover state
		EDITBOX_DISABLED,        //!< EditBox disabled state

		// ToolBar
		TOOL_BAR,                //!< ToolBar

		// ScrollBar
		SCROLL_BAR_BORDER,               //!< Scroll bar border compass.

		SCROLL_BAR_BUTTON_UP_FALSE,      //!< Vertical scroll bar, up button, Normal state.
		SCROLL_BAR_BUTTON_UP_TRUE,       //!< Vertical scroll bar, up button, Pushed state.
        SCROLL_BAR_BUTTON_UP_HOVER,      //!< Vertical scroll bar, up button, Mouse hover.
		SCROLL_BAR_BUTTON_UP_DISABLED,   //!< Vertical scroll bar, up button, Disabled state.

		SCROLL_BAR_BUTTON_DOWN_FALSE,    //!< Vertical scroll bar, down button, Normal state.
		SCROLL_BAR_BUTTON_DOWN_TRUE,     //!< Vertical scroll bar, down button, Pushed state.
        SCROLL_BAR_BUTTON_DOWN_HOVER,    //!< Vertical scroll bar, down button, Mouse hover.
		SCROLL_BAR_BUTTON_DOWN_DISABLED, //!< Vertical scroll bar, down button, Disabled state.

		SCROLL_BAR_BUTTON_LEFT_FALSE,    //!< Vertical scroll bar, left button, Normal state.
		SCROLL_BAR_BUTTON_LEFT_TRUE,     //!< Vertical scroll bar, left button, Pushed state.
        SCROLL_BAR_BUTTON_LEFT_HOVER,    //!< Vertical scroll bar, left button, Mouse hover.
		SCROLL_BAR_BUTTON_LEFT_DISABLED, //!< Vertical scroll bar, left button, Disabled state.

		SCROLL_BAR_BUTTON_RIGHT_FALSE,    //!< Vertical scroll bar, right button, Normal state.
		SCROLL_BAR_BUTTON_RIGHT_TRUE,     //!< Vertical scroll bar, right button, Pushed state.
        SCROLL_BAR_BUTTON_RIGHT_HOVER,    //!< Vertical scroll bar, right button, Mouse hover.
		SCROLL_BAR_BUTTON_RIGHT_DISABLED, //!< Vertical scroll bar, right button, Disabled state.

		SCROLL_BAR_SLIDER_FALSE,          //!< Scroll bar slider, Normal state.
		SCROLL_BAR_SLIDER_TRUE,           //!< Scroll bar slider, Pushed state.
        SCROLL_BAR_SLIDER_HOVER,          //!< Scroll bar slider, Mouse hover.
		SCROLL_BAR_SLIDER_DISABLED,       //!< Scroll bar slider, Disabled state.

		SCROLL_BAR_HORIZONTAL_EMPTY_FALSE,     //!< Scroll bar horizontal empty area, Normal state.
		SCROLL_BAR_HORIZONTAL_EMPTY_TRUE,      //!< Scroll bar horizontal empty area, Pushed state.
        SCROLL_BAR_HORIZONTAL_EMPTY_HOVER,     //!< Scroll bar horizontal empty area, Mouse hover.
		SCROLL_BAR_HORIZONTAL_EMPTY_DISABLED,  //!< Scroll bar horizontal empty area, Disabled state.

		SCROLL_BAR_VERTICAL_EMPTY_FALSE,       //!< Scroll bar vertical empty area, Normal state.
		SCROLL_BAR_VERTICAL_EMPTY_TRUE,        //!< Scroll bar vertical empty area, Pushed state.
        SCROLL_BAR_VERTICAL_EMPTY_HOVER,       //!< Scroll bar vertical empty area, Mouse hover.
		SCROLL_BAR_VERTICAL_EMPTY_DISABLED,    //!< Scroll bar vertical empty area, Disabled state.

		// Tab, use of a frame and a ListBox for the drop-down part
		TAB_ACTIVE,                 //!< Active tab
		TAB_NORMAL,                 //!< Inactive tab
        TAB_HOVER,                  //!< Tab hover
		TAB_DISABLED,               //!< Tab disabled
		TAB_BUTTON_LIST_FALSE,      //!< Tab button list, Normal state
		TAB_BUTTON_LIST_TRUE,       //!< Tab button list, Pushed state
        TAB_BUTTON_LIST_HOVER,      //!< Tab button list, Mouse hover
		TAB_BUTTON_LIST_DISABLED,   //!< Tab button list, Disabled state

		// Menu
		MENU_WIDGET,                //!< Menu widget (container of menu items)
		//MENU_BAR,                   //!< Menu bar, managing menu widget or any others widgets
		MENU_LABEL_ITEM_NORMAL,     //!< Menu entry having a simple label, Normal state.
        MENU_LABEL_ITEM_HOVER,      //!< Menu entry having a simple label, Mouse hover.
		MENU_LABEL_ITEM_DISABLED,   //!< Menu entry having a simple label, Disabled state.

		// Dopdown, use of a Frame and a ListBox for the drop-down part
		DROPDOWN_NORMAL,            //!< Dropdown field + button, normal state
        DROPDOWN_HOVER,             //!< Dropdown field + button, over state
		DROPDOWN_DISABLED,          //!< Dropdown field + button, disabled state

		// ComboBox, use of an EditBox, and a Frame with a ListBox for the drop-down part
        COMBOBOX_BUTTON_NORMAL,     //!< ComboBox buton normal state
        COMBOBOX_BUTTON_HOVER,      //!< ComboBox buton over state
        COMBOBOX_BUTTON_DISABLED,   //!< ComboBox buton disabled state

        // ToolTip
        TOOL_TYPE_NORMAL,           //!< Tool tip normal state

        WIDGET_DRAW_LAST = TOOL_TYPE_NORMAL
	};

	//! Total number of widgets draw elements.
	static const Int32 NUM_WIDGET_DRAW = WIDGET_DRAW_LAST + 1;

	//! Default constructor.
	Theme(BaseObject *parent);

	//! Construct by loading an XML theme file.
	Theme(BaseObject *parent, const String &filename);

	//! Virtual destructor.
	virtual ~Theme();

	//! Load a theme file (XML format with reference to a texture file) with it's texture
	Bool load(const String &filename);

	//! get the texture of the theme
	inline Texture2D* getTexture() { return m_texture; }

	//! load a texture for the theme
	Bool loadTexture(const String &filename);

	//! set the default font
	inline void setDefaultFont(ABCFont *defaultFont) { m_defaultFont = defaultFont; }

	//! get the default font
	ABCFont* getDefaultFont();
	//! get the default font (const version)
	const ABCFont* getDefaultFont() const;

	//! set the default font size
	inline void setDefaultFontSize(UInt32 size) { m_defaultFontSize = size; }
	//! get the default font size
	inline UInt32 getDefaultFontSize() const { return m_defaultFontSize; }

    //! set the default font color
	inline void setDefaultFontColor(const Color& color) { m_defaultFontColor = color; }
    //! get the default font color
	inline const Color& getDefaultFontColor() const { return m_defaultFontColor; }

    //! set the default highlight font color
    inline void setDefaultHighlightFontColor(const Color& color) { m_defaultHighlightFontColor = color; }
    //! get the default font size
    inline const Color& getDefaultHighlightFontColor() const { return m_defaultHighlightFontColor; }

	//! get the width of a widget element
	inline Int32 getWidgetWidth(WidgetDraw widg) const
	{
		if (widg < NUM_WIDGET_DRAW)
			return m_widgetDraw[widg]->getWidgetWidth();
		return 0;
	}

	//! get the height of a widget element
	inline Int32 getWidgetHeight(WidgetDraw widg) const
	{
		if (widg < NUM_WIDGET_DRAW)
			return m_widgetDraw[widg]->getWidgetHeight();
		return 0;
	}

	//! convert a widget draw name string to its enum WidgetDraw value
	WidgetDraw string2WidgetDraw(const String &str) const;

	//! Drawing method
	void drawWidget(WidgetDraw widget, Int32 x, Int32 y, Int32 width, Int32 height);

	//! Return the widget draw mode
	const WidgetDrawMode* getWidgetDrawMode(WidgetDraw widget) const;

	//! Return the default widget size (defined by the theme)
	Vector2i getDefaultWidgetSize(WidgetDraw widget) const;

	//! Get the icon set of the theme (read only).
	inline const IconSet& getIconSet() const { return m_iconSet; }
	//! Get the icon set of the theme.
	inline IconSet& getIconSet() { return m_iconSet; }

	//! Get the cursor manager (read only).
	inline const CursorManager& getCursorManager() const { return m_cursorManager; }
	//! Get the cursor manager.
	inline CursorManager& getCursorManager() { return m_cursorManager; }

protected:

	WidgetDrawMode* m_widgetDraw[NUM_WIDGET_DRAW];

	Texture2D* m_texture;   //!< the texture that contains the theme

    SmartObject<ABCFont> m_defaultFont;   //!< Default theme font
    UInt32 m_defaultFontSize;             //!< 11 is default
    Color m_defaultFontColor;             //!< black is default
    Color m_defaultHighlightFontColor;    //!< a kind of blue is default

	IconSet m_iconSet;       //!< Theme icon set.

	//! Create the map of string widget name to integer widget type.
	void createStringToWidgetTypeMap();

	std::map<String, WidgetDraw> m_widgetNameToType;

	CursorManager m_cursorManager;

public:

    //! Emmited when there is a change into the definition of the theme
    //! @param WidgetDraw the modified component.
    //! @note This signal is not called when theme is initially loaded.
    Signal<WidgetDraw> onThemeChanged{this};
};

} // namespace o3d

#endif // _O3D_THEME_H
