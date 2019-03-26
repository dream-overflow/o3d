/**
 * @file keymapdummy.h
 * @brief
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2019-03-25
 * @copyright Copyright (c) 2001-2019 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_KEYMAPDUMMY_H
#define _O3D_KEYMAPDUMMY_H

/* ONLY IF O3D_DUMMY IS SELECTED */
#ifdef O3D_DUMMY

namespace o3d {

/**
 * @brief Keyboard keys mapping define based on WIN32 virtual key codes.
 * @date 2005-01-25
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
enum VKey
{
    KEY_UNDEFINED      = 0x00,

    KEY_BACKSPACE      = 0x08,
    KEY_TAB            = 0x09,

    KEY_CLEAR          = 0x0C,
    KEY_RETURN         = 0x0D,

    KEY_PAUSE          = 0x13,
    KEY_CAPSLOCK       = 0x14,

    KEY_ESCAPE         = 0x1B,
    KEY_SPACE          = 0x20,
    KEY_PGUP           = 0x21,
    KEY_PGDW           = 0x22,
    KEY_END            = 0x23,
    KEY_HOME           = 0x24,
    KEY_LEFT           = 0x25,
    KEY_UP             = 0x26,
    KEY_RIGHT          = 0x27,
    KEY_DOWN           = 0x28,
    KEY_SELECT         = 0x29,
    KEY_PRINT          = 0x2A,
    KEY_EXECUTE        = 0x2B,
    KEY_SNAPSHOT       = 0x2C,
    KEY_INSERT         = 0x2D,
    KEY_DELETE         = 0x2E,
    KEY_HELP           = 0x2F,

    KEY_0              = 0x30,      // 0-9
    KEY_1              = 0x31,
    KEY_2              = 0x32,
    KEY_3              = 0x33,
    KEY_4              = 0x34,
    KEY_5              = 0x35,
    KEY_6              = 0x36,
    KEY_7              = 0x37,
    KEY_8              = 0x38,
    KEY_9              = 0x39,

    KEY_A              = 0x41,     // A-Z
    KEY_B              = 0x42,
    KEY_C              = 0x43,
    KEY_D              = 0x44,
    KEY_E              = 0x45,
    KEY_F              = 0x46,
    KEY_G              = 0x47,
    KEY_H              = 0x48,
    KEY_I              = 0x49,
    KEY_J              = 0x4A,
    KEY_K              = 0x4B,
    KEY_L              = 0x4C,
    KEY_M              = 0x4D,
    KEY_N              = 0x4E,
    KEY_O              = 0x4F,
    KEY_P              = 0x50,
    KEY_Q              = 0x51,
    KEY_R              = 0x52,
    KEY_S              = 0x53,
    KEY_T              = 0x54,
    KEY_U              = 0x55,
    KEY_V              = 0x56,
    KEY_W              = 0x57,
    KEY_X              = 0x58,
    KEY_Y              = 0x59,
    KEY_Z              = 0x5A,

    KEY_LSUPER         = 0x5B,
    KEY_RSUPER         = 0x5C,
    KEY_MENU           = 0x5D,

    KEY_SLEEP          = 0x5F,

    KEY_NUMPAD0        = 0x60,         // numpad 0-9
    KEY_NUMPAD1        = 0x61,
    KEY_NUMPAD2        = 0x62,
    KEY_NUMPAD3        = 0x63,
    KEY_NUMPAD4        = 0x64,
    KEY_NUMPAD5        = 0x65,
    KEY_NUMPAD6        = 0x66,
    KEY_NUMPAD7        = 0x67,
    KEY_NUMPAD8        = 0x68,
    KEY_NUMPAD9        = 0x69,
    KEY_NUMPAD_MULTIPLY= 0x6A,
    KEY_NUMPAD_ADD     = 0x6B,
    KEY_NUMPAD_SEPARATOR = 0x6C,
    KEY_NUMPAD_SUBTRACT= 0x6D,
    KEY_NUMPAD_DECIMAL = 0x6E,
    KEY_NUMPAD_DIVIDE  = 0x6F,
    KEY_NUMPAD_ENTER   = 0x0E,
    KEY_NUMPAD_EQUALS  = 0x92,

    KEY_F1             = 0x70,         // F1-F24
    KEY_F2             = 0x71,
    KEY_F3             = 0x72,
    KEY_F4             = 0x73,
    KEY_F5             = 0x74,
    KEY_F6             = 0x75,
    KEY_F7             = 0x76,
    KEY_F8             = 0x77,
    KEY_F9             = 0x78,
    KEY_F10            = 0x79,
    KEY_F11            = 0x7A,
    KEY_F12            = 0x7B,
    KEY_F13            = 0x7C,
    KEY_F14            = 0x7D,
    KEY_F15            = 0x7E,
    KEY_F16            = 0x7F,
    KEY_F17            = 0x80,
    KEY_F18            = 0x81,
    KEY_F19            = 0x82,
    KEY_F20            = 0x83,
    KEY_F21            = 0x84,
    KEY_F22            = 0x85,
    KEY_F23            = 0x86,
    KEY_F24            = 0x87,

    KEY_NUMLOCK        = 0x90,
    KEY_SCROLL         = 0x91,

    KEY_LSHIFT         = 0xA0,
    KEY_RSHIFT         = 0xA1,
    KEY_LCONTROL       = 0xA2,
    KEY_RCONTROL       = 0xA3,
    KEY_LALT           = 0xA4,
    KEY_RALT           = 0xA5,
    KEY_ALTGR          = KEY_RALT, //

    KEY_SEMICOLON      = 0xBA,   // ';:' for US
    KEY_PLUS           = 0xBB,   // '+'
    KEY_COMMA          = 0xBC,   // ','
    KEY_MINUS          = 0xBD,   // '-'
    KEY_PERIOD         = 0xBE,   // '.'
    KEY_SLASH          = 0xBF,   // '/?' for US
    KEY_BACKQUOTE      = 0xC0,   // '`~' for US

    KEY_LEFTBRACKET    = 0xDB,  //  '[{' for US
    KEY_BACKSLASH      = 0xDC,  //  '\|' for US
    KEY_RIGHTBRACKET   = 0xDD,  //  ']}' for US
    KEY_QUOTEDBL       = 0xDE,  //  ''"' for US
    KEY_EXCLAIM        = 0xDF,  //  '!§' for FR

    KEY_OEM_102        = 0xE2  //  "<>" or "\|" on RT 102-key kbd.
};

} // namespace o3d

#endif // O3D_DUMMY

#endif // _O3D_KEYMAPDUMMY_H
