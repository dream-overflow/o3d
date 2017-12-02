/**
 * @file guitype.h
 * @brief Enumerations for GUI.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2012-12-15
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_GUI_TYPE_H
#define _O3D_GUI_TYPE_H

namespace o3d {

enum GuiObjectType
{
    GUI_GUI = 0x05000000,           //!< GUI
	GUI_CONSOLE,                    //!< Consol with command mecanim
	GUI_MESSAGE_BOX,                //!< Predefined window for common modal message/alert box

    GUI_FONT_MANAGER = 0x05010000,  //!< a list of fonts to load
	GUI_FONT_ABC,                   //!< base class for font
	GUI_FONT_TEXTURE,               //!< tag for a texture based font
	GUI_FONT_TRUE_TYPE,             //!< tag for a true type font based font (TTF)
	GUI_FONT_MATERIAL,              //!< a default material for font rendering

    GUI_THEME_MANAGER = 0x05020000, //!< manager to have multiple themes
	GUI_THEME,                      //!< theme container for widgets

    GUI_WIDGET = 0x05030000,        //!< base widget object
	GUI_WIDGET_MANAGER,             //!< widget controller and manager
    GUI_WIDGET_ROOT_WINDOW,         //!< root window
	GUI_WIDGET_PROPERTIES,          //!< base class for widget
	GUI_WIDGET_STATICTEXT,          //!< a static text widget
	GUI_WIDGET_EDITBOX,             //!< an edit box for text input widget
	GUI_WIDGET_BUTTON,              //!< a button (standard & base button class) widget
	GUI_WIDGET_TOOLBUTTON,          //!< a tool button (icon & toolbar button) widget
	GUI_WIDGET_CHECKBOX,            //!< a check box (from button) widget
	GUI_WIDGET_LISTBOX,             //!< a list box of objects items widget
	GUI_WIDGET_PROGRESSBAR,         //!< a progress bar
	GUI_WIDGET_RADIOBUTTON,         //!< a radio button (from button) widget
	GUI_WIDGET_TABBED,              //!< a tabbed widget
	GUI_WIDGET_WINDOW,              //!< a window widget
	GUI_WIDGET_FRAME,               //!< a frame widget
	GUI_WIDGET_COMBOBOX,            //!< a combo box (unrolling list of objects)
	GUI_WIDGET_STATICBITMAP,        //!< a static bitmap widget
	GUI_WIDGET_SCROLLBAR,           //!< a scrollbar control widget
	GUI_WIDGET_MESSAGEBOX,          //!< dialog based message box (helper class)
	GUI_WIDGET_ICON,                //!< icon widget
	GUI_WIDGET_STATICIMAGE,         //!< static image widget
	GUI_WIDGET_TOOLBAR,             //!< toolbar widget
	GUI_WIDGET_PANE,                //!< labelizable and borderable pane widget
	GUI_WIDGET_TABBED_PANE,         //!< labelizable and borderable pane widget, with a tab component
	GUI_WIDGET_GLASS_PANE,          //!< invisible glass pane
	GUI_WIDGET_MENU,                //!< menu widget
	GUI_WIDGET_MENU_BAR,            //!< menu bar managing menu widgets or any others widgets
	GUI_WIDGET_MENU_LABEL,          //!< menu item with a simple label
    GUI_WIDGET_TOOL_TIP,            //!< a tooltip frame
	GUI_WIDGET_DRAW_MATERIAL,

    GUI_CURSOR = 0x05040000,        //!< tag for a cursor object
	GUI_CURSOR_MANAGER,
	GUI_CARET,                      //!< tag for a caret object
	GUI_ICON,                       //!< tag for an icon object
	GUI_PICTURE,                    //!< tag for picture object

    GUI_LAYOUT = 0x05050000,        //!< tag for an undefined layout widget
    GUI_LAYOUT_FREE,                //!< tag for a free layout widget
	GUI_LAYOUT_BOX,                 //!< tag for a box layout widget
	GUI_LAYOUT_GRID,                //!< tag for a grid box layout widget
	GUI_LAYOUT_FLEXGRID,            //!< tag for a flex grid layout widget
	GUI_LAYOUT_GRIDBAG,             //!< tag for a grid bag layout widget
	GUI_LAYOUT_WRAP,                //!< tag for a wrap layout widget
	GUI_LAYOUT_SPACER               //!< tag for a spacer layout widget
};

} // namespace o3d

#endif // _O3D_GUI_TYPE_H
