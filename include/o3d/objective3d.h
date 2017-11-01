/**
 * @file objective3d.h
 * @brief Application dynamic library exporter
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (C) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _OBJECTIVE3D_H
#define _OBJECTIVE3D_H

#include "objective3dconfig.h"

#define O3D_NO_LIB

//---------------------------------------------------------------------------------------
// Libraries
//---------------------------------------------------------------------------------------
#if !defined(O3D_NO_LIB) && defined(_MSC_VER)
// opengl modules
#pragma comment(lib,"opengl32.lib")

// openal modules
#pragma comment(lib,"OpenAL32.lib")

// graphics modules
#if defined(_DEBUG)
	#pragma comment(lib,"libjpegd.lib")
#else
	#pragma comment(lib,"libjpeg.lib")
#endif // _DEBUG

// libpng module
#pragma comment(lib,"libpng.lib")

// zlib module
#pragma comment(lib,"zlib.lib")

// tinyxml module
#if defined(_DEBUG)
	#pragma comment(lib,"tinyxmlD.lib")
#else
	#pragma comment(lib,"tinyxml.lib")
#endif // _DEBUG

// winsock2 module
#ifdef O3D_USE_SOCKET
	#pragma comment(lib,"Ws2_32.lib")
#endif

// sdl modules
#ifdef O3D_SDL
	#pragma comment(lib,"SDL.lib")
#endif

// ogg vorbis module
#if defined(_DEBUG)
	#pragma comment(lib,"liboggd.lib")
	#pragma comment(lib,"libvorbisd.lib")
	#pragma comment(lib,"libvorbisfiled.lib")
#else
	#pragma comment(lib,"libogg.lib")
	#pragma comment(lib,"libvorbis.lib")
	#pragma comment(lib,"libvorbisfile.lib")
#endif // _DEBUG

// eax modules
#ifdef O3D_EAX
	#pragma comment(lib,"eaxguid.lib")
	#pragma comment(lib,"eax.lib")
#endif

// strippers
#if defined(_DEBUG)
	#pragma comment(lib,"TriStripperD.lib")
	#pragma comment(lib,"nvTriStripD.lib")
#else
	#pragma comment(lib,"TriStripper.lib")
	#pragma comment(lib,"nvTriStrip.lib")
#endif

// freetype2 module
#pragma comment(lib,"freetype235MT.lib")

// O3D DEBUG or RELEASE modules
#ifndef O3D_NO_LIB_FLAG // used only for DLL export (in Objective3D.cpp)
	#if defined(_DEBUG)
		#pragma comment(lib,"Objective3DD.lib")
	#else
		#pragma comment(lib,"Objective3D.lib")
	#endif // _DEBUG or RELEASE
#endif // O3D_NO_LIB_FLAG

#endif // O3D_NO_LIB and not O3D_NO_LIB_FLAG

#endif // _OBJECTIVE3D_H
