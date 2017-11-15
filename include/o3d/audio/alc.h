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

#ifdef O3D_VC_COMPILER
	#include <alc.h>
#elif defined(__APPLE__)
	#include <OpenAL/alc.h>
#elif defined(__GNUC__)//__UNIX__)
	#include <AL/alc.h>
#else
	#error "<< Unknown architecture ! >>"
#endif

#endif // _O3D_ALC_H

