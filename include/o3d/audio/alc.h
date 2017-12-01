/**
 * @file alc.h
 * @brief OpenALC base header.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005/07/02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ALC_H
#define _O3D_ALC_H

#include "objective3dconfig.h"

#ifdef _MSC_VER
	#include <alc.h>
#elif defined(__APPLE__)
	#include <OpenAL/alc.h>
#elif defined(__GNUC__)
	#include <AL/alc.h>
#else
    #error "<< Unsupported compiler ! >>"
#endif

#endif // _O3D_ALC_H

