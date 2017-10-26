/**
 * @file keymapsdl.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_KEYMAPSDL_H
#define _O3D_KEYMAPSDL_H

/* ONLY IF O3D_SDL IS SELECTED */
#ifdef O3D_SDL

#include <SDL2/SDL_keycode.h>

namespace o3d {

/**
 * @brief Keyboard keys mapping define based on SDL virtual key codes.
 * @date 2005-01-25
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 */
enum VKey
{
	KEY_UNDEFINED      = 0x00,

	KEY_BACKSPACE      = SDLK_BACKSPACE,
	KEY_TAB            = SDLK_TAB,

	KEY_CLEAR          = SDLK_CLEAR,
	KEY_RETURN         = SDLK_RETURN,

	KEY_PAUSE          = SDLK_PAUSE,
	KEY_CAPSLOCK       = SDLK_CAPSLOCK,

	KEY_ESCAPE         = SDLK_ESCAPE,
	KEY_SPACE          = SDLK_SPACE,
	KEY_PGUP           = SDLK_PAGEUP,
	KEY_PGDW           = SDLK_PAGEDOWN,
	KEY_END            = SDLK_END,
	KEY_HOME           = SDLK_HOME,
	KEY_LEFT           = SDLK_LEFT,
	KEY_UP             = SDLK_UP,
	KEY_RIGHT          = SDLK_RIGHT,
	KEY_DOWN           = SDLK_DOWN,
	KEY_SELECT         = SDLK_SELECT,
//	KEY_PRINT          = ,
	KEY_EXECUTE        = SDLK_EXECUTE,
	KEY_SNAPSHOT       = SDLK_PRINTSCREEN,
	KEY_INSERT         = SDLK_INSERT,
	KEY_DELETE         = SDLK_DELETE,
	KEY_HELP           = SDLK_HELP,

	KEY_0              = SDLK_0,      // 0-9
	KEY_1              = SDLK_1,
	KEY_2              = SDLK_2,
	KEY_3              = SDLK_3,
	KEY_4              = SDLK_4,
	KEY_5              = SDLK_5,
	KEY_6              = SDLK_6,
	KEY_7              = SDLK_7,
	KEY_8              = SDLK_8,
	KEY_9              = SDLK_9,

	KEY_A              = SDLK_a,     // A-Z
	KEY_B              = SDLK_b,
	KEY_C              = SDLK_c,
	KEY_D              = SDLK_d,
	KEY_E              = SDLK_e,
	KEY_F              = SDLK_f,
	KEY_G              = SDLK_g,
	KEY_H              = SDLK_h,
	KEY_I              = SDLK_i,
	KEY_J              = SDLK_j,
	KEY_K              = SDLK_k,
	KEY_L              = SDLK_l,
	KEY_M              = SDLK_m,
	KEY_N              = SDLK_n,
	KEY_O              = SDLK_o,
	KEY_P              = SDLK_p,
	KEY_Q              = SDLK_q,
	KEY_R              = SDLK_r,
	KEY_S              = SDLK_s,
	KEY_T              = SDLK_t,
	KEY_U              = SDLK_u,
	KEY_V              = SDLK_v,
	KEY_W              = SDLK_w,
	KEY_X              = SDLK_x,
	KEY_Y              = SDLK_y,
	KEY_Z              = SDLK_z,

	KEY_LSUPER         = SDLK_LGUI,
	KEY_RSUPER         = SDLK_RGUI,
	KEY_MENU           = SDLK_MENU,

//	KEY_SLEEP          = ,

	KEY_NUMPAD0        = SDLK_KP_0,         // numpad 0-9
	KEY_NUMPAD1        = SDLK_KP_1,
	KEY_NUMPAD2        = SDLK_KP_2,
	KEY_NUMPAD3        = SDLK_KP_3,
	KEY_NUMPAD4        = SDLK_KP_4,
	KEY_NUMPAD5        = SDLK_KP_5,
	KEY_NUMPAD6        = SDLK_KP_6,
	KEY_NUMPAD7        = SDLK_KP_7,
	KEY_NUMPAD8        = SDLK_KP_8,
	KEY_NUMPAD9        = SDLK_KP_9,
	KEY_NUMPAD_MULTIPLY= SDLK_KP_MULTIPLY,
	KEY_NUMPAD_ADD     = SDLK_KP_PLUS,
	KEY_NUMPAD_SEPARATOR= SDLK_KP_COMMA,
	KEY_NUMPAD_SUBTRACT= SDLK_KP_MINUS,
	KEY_NUMPAD_DECIMAL = SDLK_KP_PERIOD,
	KEY_NUMPAD_DIVIDE  = SDLK_KP_DIVIDE,
	KEY_NUMPAD_ENTER   = SDLK_KP_ENTER,

	KEY_F1             = SDLK_F1,         // F1-F24
	KEY_F2             = SDLK_F2,
	KEY_F3             = SDLK_F3,
	KEY_F4             = SDLK_F4,
	KEY_F5             = SDLK_F5,
	KEY_F6             = SDLK_F6,
	KEY_F7             = SDLK_F7,
	KEY_F8             = SDLK_F8,
	KEY_F9             = SDLK_F9,
	KEY_F10            = SDLK_F10,
	KEY_F11            = SDLK_F11,
	KEY_F12            = SDLK_F12,
	KEY_F13            = SDLK_F13,
	KEY_F14            = SDLK_F14,
	KEY_F15            = SDLK_F15,
	KEY_F16            = SDLK_F16,
	KEY_F17            = SDLK_F17,
	KEY_F18            = SDLK_F18,
	KEY_F19            = SDLK_F19,
	KEY_F20            = SDLK_F20,
	KEY_F21            = SDLK_F21,
	KEY_F22            = SDLK_F22,
	KEY_F23            = SDLK_F23,
	KEY_F24            = SDLK_F24,

	KEY_NUMLOCK        = SDLK_NUMLOCKCLEAR,
	KEY_SCROLL         = SDLK_SCROLLLOCK,

	KEY_NUMPAD_EQUALS  = SDLK_KP_EQUALS,

	KEY_LSHIFT         = SDLK_LSHIFT,
	KEY_RSHIFT         = SDLK_RSHIFT,
	KEY_LCONTROL       = SDLK_LCTRL,
	KEY_RCONTROL       = SDLK_RCTRL,
	KEY_LALT           = SDLK_LALT,
	KEY_RALT           = SDLK_RALT,
    KEY_ALTGR          = SDLK_MODE,

	KEY_SEMICOLON      = SDLK_SEMICOLON,  // ';:' for US
	KEY_PLUS           = SDLK_PLUS,//SDLK_EQUALS     // '+'
	KEY_COMMA          = SDLK_COMMA,      // ','
	KEY_MINUS          = SDLK_MINUS,      // '-'
	KEY_PERIOD         = SDLK_PERIOD,     // '.'
	KEY_SLASH          = SDLK_SLASH,      // '/?' for US
	KEY_BACKQUOTE      = SDLK_BACKQUOTE,  // '`~' for US

	KEY_LEFTBRACKET    = SDLK_LEFTBRACKET,   //  '[{' for US
	KEY_BACKSLASH      = SDLK_BACKSLASH,     //  '\|' for US
	KEY_RIGHTBRACKET   = SDLK_RIGHTBRACKET,  //  ']}' for US
	KEY_QUOTEDBL       = SDLK_QUOTE,         //  ''"' for US
	KEY_EXCLAIM        = SDLK_EXCLAIM,       //  '!§'

	KEY_OEM_102        = SDLK_LESS  //  "<>" or "\|" on RT 102-key kbd.
};

} // namespace o3d

#endif // O3D_SDL

#endif // _O3D_KEYMAPSDL_H

