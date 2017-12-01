/**
 * @file al.h
 * @brief OpenAL base header.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-07-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_AL_H
#define _O3D_AL_H

#include "objective3dconfig.h"

#ifdef _MSC_VER
	#include <al.h>
#elif defined(__APPLE__)
	#include <OpenAL/al.h>
#elif defined(__GNUC__)
	#include <AL/al.h>
#else
    #error "<< Unsupported compiler ! >>"
#endif

#ifndef AL_BYTE_OFFSET // OpenAL < 1.1
	typedef ALubyte ALchar;
#endif

#endif // _O3D_AL_H
