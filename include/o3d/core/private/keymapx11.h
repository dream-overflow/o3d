/**
 * @file keymapx11.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_KEYMAPX11_H
#define _O3D_KEYMAPX11_H

/* ONLY IF O3D_X11 IS SELECTED */
#ifdef O3D_X11

#define XK_MISCELLANY
#define XK_LATIN1
#define XK_XKB_KEYS

#include <X11/keysymdef.h>

namespace o3d {

/**
 * @brief Keyboard keys mapping define based on X11 key sym.
 * @date 2005-01-25
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
enum VKey
{
	KEY_UNDEFINED      = 0x00,

	KEY_BACKSPACE      = XK_BackSpace,
	KEY_TAB            = XK_Tab,

	KEY_CLEAR          = XK_Clear,
	KEY_RETURN         = XK_Return,

	KEY_PAUSE          = XK_Pause,
	KEY_CAPSLOCK       = XK_Caps_Lock,

	KEY_ESCAPE         = XK_Escape,
	KEY_SPACE          = XK_space,
	KEY_PGUP           = XK_Page_Up,
	KEY_PGDW           = XK_Page_Down,
	KEY_END            = XK_End,
	KEY_HOME           = XK_Home,
	KEY_LEFT           = XK_Left,
	KEY_UP             = XK_Up,
	KEY_RIGHT          = XK_Right,
	KEY_DOWN           = XK_Down,
	KEY_SELECT         = XK_Select,
    KEY_PRINT          = XK_Print,
	KEY_EXECUTE        = XK_Execute,
    KEY_SYSRQ          = XK_Sys_Req,
    KEY_SNAPSHOT       = XK_Sys_Req, // XK_3270_PrintScreen,
	KEY_INSERT         = XK_Insert,
	KEY_DELETE         = XK_Delete,
	KEY_HELP           = XK_Help,

	KEY_0              = XK_0,      // 0-9
	KEY_1              = XK_1,
	KEY_2              = XK_2,
	KEY_3              = XK_3,
	KEY_4              = XK_4,
	KEY_5              = XK_5,
	KEY_6              = XK_6,
	KEY_7              = XK_7,
	KEY_8              = XK_8,
	KEY_9              = XK_9,

	KEY_A              = XK_a,     // A-Z
	KEY_B              = XK_b,
	KEY_C              = XK_c,
	KEY_D              = XK_d,
	KEY_E              = XK_e,
	KEY_F              = XK_f,
	KEY_G              = XK_g,
	KEY_H              = XK_h,
	KEY_I              = XK_i,
	KEY_J              = XK_j,
	KEY_K              = XK_k,
	KEY_L              = XK_l,
	KEY_M              = XK_m,
	KEY_N              = XK_n,
	KEY_O              = XK_o,
	KEY_P              = XK_p,
	KEY_Q              = XK_q,
	KEY_R              = XK_r,
	KEY_S              = XK_s,
	KEY_T              = XK_t,
	KEY_U              = XK_u,
	KEY_V              = XK_v,
	KEY_W              = XK_w,
	KEY_X              = XK_x,
	KEY_Y              = XK_y,
	KEY_Z              = XK_z,

	KEY_LSUPER         = XK_Super_L,
	KEY_RSUPER         = XK_Super_R,
	KEY_MENU           = XK_Menu,

    KEY_SLEEP          = 0,  // undefined

	KEY_NUMPAD0        = XK_KP_0,         // numpad 0-9
	KEY_NUMPAD1        = XK_KP_1,
	KEY_NUMPAD2        = XK_KP_2,
	KEY_NUMPAD3        = XK_KP_3,
	KEY_NUMPAD4        = XK_KP_4,
	KEY_NUMPAD5        = XK_KP_5,
	KEY_NUMPAD6        = XK_KP_6,
	KEY_NUMPAD7        = XK_KP_7,
	KEY_NUMPAD8        = XK_KP_8,
	KEY_NUMPAD9        = XK_KP_9,
    KEY_NUMPAD_MULTIPLY = XK_KP_Multiply,
	KEY_NUMPAD_ADD     = XK_KP_Add,
    KEY_NUMPAD_SEPARATOR = XK_KP_Separator,
	KEY_NUMPAD_SUBTRACT= XK_KP_Subtract,
	KEY_NUMPAD_DECIMAL = XK_KP_Decimal,
	KEY_NUMPAD_DIVIDE  = XK_KP_Divide,
	KEY_NUMPAD_ENTER   = XK_KP_Enter,
    KEY_NUMPAD_EQUALS  = XK_KP_Equal,

	KEY_F1             = XK_F1,         // F1-F24
	KEY_F2             = XK_F2,
	KEY_F3             = XK_F3,
	KEY_F4             = XK_F4,
	KEY_F5             = XK_F5,
	KEY_F6             = XK_F6,
	KEY_F7             = XK_F7,
	KEY_F8             = XK_F8,
	KEY_F9             = XK_F9,
	KEY_F10            = XK_F10,
	KEY_F11            = XK_F11,
	KEY_F12            = XK_F12,
	KEY_F13            = XK_F13,
	KEY_F14            = XK_F14,
	KEY_F15            = XK_F15,
	KEY_F16            = XK_F16,
	KEY_F17            = XK_F17,
	KEY_F18            = XK_F18,
	KEY_F19            = XK_F19,
	KEY_F20            = XK_F20,
	KEY_F21            = XK_F21,
	KEY_F22            = XK_F22,
	KEY_F23            = XK_F23,
	KEY_F24            = XK_F24,

	KEY_NUMLOCK        = XK_Num_Lock,
	KEY_SCROLL         = XK_Scroll_Lock,

	KEY_LSHIFT         = XK_Shift_L,
	KEY_RSHIFT         = XK_Shift_R,
	KEY_LCONTROL       = XK_Control_L,
	KEY_RCONTROL       = XK_Control_R,
	KEY_LALT           = XK_Alt_L,
	KEY_RALT           = XK_Alt_R,
    KEY_ALTGR          = XK_ISO_Level3_Shift,

	KEY_SEMICOLON      = XK_semicolon,  // ';:' for US
	KEY_PLUS           = XK_plus,//SDLK_EQUALS     // '+'
	KEY_COMMA          = XK_comma,      // ','
	KEY_MINUS          = XK_minus,      // '-'
	KEY_PERIOD         = XK_period,     // '.'
	KEY_SLASH          = XK_slash,      // '/?' for US
	KEY_BACKQUOTE      = XK_quoteleft,  // '`~' for US

	KEY_LEFTBRACKET    = XK_bracketleft,   //  '[{' for US
	KEY_BACKSLASH      = XK_backslash,     //  '\|' for US
	KEY_RIGHTBRACKET   = XK_bracketright,  //  ']}' for US
	KEY_QUOTEDBL       = XK_quoteright,         //  ''"' for US
	KEY_EXCLAIM        = XK_exclam,       //  '!§'

	KEY_OEM_102         = XK_less//  "<>" or "\|" on RT 102-key kbd.
};

} // namespace o3d

#endif // O3D_X11

#endif // _O3D_KEYMAPX11_H
