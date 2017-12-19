/**
 * @file coretype.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CORETYPE_H
#define _O3D_CORETYPE_H

namespace o3d {

//! objects type
//! 0xXYYYZZZZ where :
//! - X mean the module (core, engine...)
//! - YYY mean the group into the module
//! - ZZZZ mean the value into the group
enum ObjectType
{
	O3D_UNDEFINED = 0x00000000      //!< undefined type
};

/**
 * @brief Enumerations for core module.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2012-12-15
 */
enum CoreObjectType
{
    CORE_BASE_OBJECT = 0x01000000,
    CORE_SHARABLE,
    CORE_PARENTABLE,
	CORE_EVT_HANDLER,
	CORE_RESOURCE,

    CORE_INPUT_LIST = 0x01010000,    //!< input list
	CORE_INPUT,                      //!< void input
	CORE_KEYBOARD,                   //!< the keyboard input
	CORE_MOUSE,                      //!< the mouse input
    CORE_CAMERA,                     //!< the camera input (not implemented)
	CORE_JOYPAD,                     //!< the joypad input (not implemented)
	CORE_JOYPAD_VIBRATE,             //!< the vibrate joypad input (not implemented)
	CORE_JOYSTICK,                   //!< the joystick input (not implemented)
    CORE_JOYSTICK_FORCE_FEEDBACK,    //!< the joystick force feedback input (not implemented)
    CORE_TOUCHSCREEN,                //!< the touch-screen input

    CORE_SERVICE,                    //!< abstract service
    CORE_SERVICE_MANAGER             //!< service manager
};

enum UserObjectType
{
    USER_OBJECT = 0x10000000         //!< First id for user objects types
};

enum FileLocation
{
    FL_UNKNOWN = 0,         //!< Unknown file location.
    FL_LOCAL,               //!< Local file system.
    FL_VIRTUAL,             //!< Virtual file (asset...).
};

//! File types
enum FileTypes
{
	FILE_UNKNWON = 0,		//!< unknown file type.
	FILE_DIR,				//!< directory.
	FILE_FILE,				//!< file.
	FILE_BOTH,				//!< file and directory.
	FILE_REMOVABLE,			//!< removable logical reader.
	FILE_FIXED,				//!< fixed logical reader (hard drive).
	FILE_REMOTE,			//!< logical network reader.
	FILE_CDROM,				//!< logical CDROM/DVDROM reader.
	FILE_RAMDISK			//!< logical memory slot reader (Ram Stick).
};

//! vector/matrices/quaternions components {x,y,z,w}
enum VectorCoord {
	X = 0,
	Y = 1,
	Z = 2,
	W = 3
};


//! Time units
enum TimeUnit
{
    TIME_SECOND = 1,               //!< Time unit is second
    TIME_MILLISECOND = 1000,       //!< Time unit is millisecond
    TIME_MICROSECOND = 1000000,    //!< Time unit is microsecond
    TIME_NANOSECOND = 1000000000   //!< Time unit is nanosecond
};

//! Characters encoding
enum CharacterEncoding
{
    ENCODING_UNDEFINED = 0,
    ENCODING_ANSI,
    ENCODING_UTF8
};

} // namespace o3d

#endif // _O3D_CORETYPE_H
