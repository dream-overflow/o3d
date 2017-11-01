/**
 * @file config.h
 * @brief Defines the compilation options
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-11-17
 * @copyright Copyright (C) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_CONFIG_H
#define _O3D_CONFIG_H

//! Export/import type (dynamic or static library)
//! If none is defined O3D_IMPORT_DLL is choosen.
//#define O3D_EXPORT_DLL
//#define O3D_IMPORT_DLL
//#define O3D_STATIC_LIB

//
// Windows
//
#if defined(_MSC_VER) || defined(WIN32) || defined(__MINGW32__)

	#ifdef _MSC_VER
		#define O3D_VC_COMPILER
	#endif
	#define O3D_WINDOWS
	#if defined(WIN32) && !defined(_WINDOWS)
		#define _WINDOWS
	#endif

	#ifdef WIN32
		#define O3D_WIN32
	#else
		#define O3D_WIN64
	#endif

	#ifdef O3D_X11
		#error "X11 is not supported on Windows"
	#endif

	#if !defined(O3D_WIN32) && !defined(O3D_SDL)
		#define O3D_WIN32
	#endif

	//! System API (for thread, mutex, semaphore, timers...)
	#define O3D_WIN32_SYS

	//! No memory manager allocation/free/leaks detection for global memory.
	#ifdef _DEBUG
		//#define O3D_RAM_MEMORY_MANAGER
	#endif

	//! No memory manager allocation/free/leaks detection for fast allocator.

	//! Memory manager allocation/free/leaks detection for GPU memory.
	#ifdef _DEBUG
		#define O3D_GPU_MEMORY_MANAGER
	#endif

	//! Memory manager allocation/free/leaks detection for SPU memory.
	#ifdef _DEBUG
		#define O3D_SPU_MEMORY_MANAGER
	#endif

	// Disable some warnings for VC
	#ifdef _MSC_VER
		#pragma warning(disable:4100)    // delete the unreferenced parameter to resolve the warning
		#pragma warning(disable:4251)    // need DLL interface
		#pragma warning(disable:4800)    // 'int' to 'boolean'
		#pragma warning(disable:4355)    // 'this' used in base member initializer list
		#pragma warning(disable:4127)    // conditional expression is constant
		#pragma warning(disable:4201)    // nonstandard extension used : nameless struct/union
	#endif

	//! EAX sound effect support. Only available on Windows.
	#define O3D_BUILD_EAX

//
// UNIXes
//
#elif defined(linux) || defined(__unix__)

	#ifndef __UNIX__
		#define __UNIX__
	#endif
	#define O3D_UNIX

	#ifdef linux
		#define O3D_LINUX
	#endif

	#if !defined(O3D_X11) && !defined(O3D_SDL)
		#define O3D_X11
	#endif

	#if __BYTE_ORDER == __BIG_ENDIAN
		#define O3D_BIG_ENDIAN
	#endif

	//! System API (for thread, mutex, semaphore, timers...)
	#define O3D_POSIX_SYS

	//! No memory manager allocation/free/leaks detection for global memory.

	//! No memory manager allocation/free/leaks detection for fast allocator.

	//! Memory manager allocation/free/leaks detection for GPU memory.
	#ifdef _DEBUG
		#define O3D_GPU_MEMORY_MANAGER
	#endif

	//! Memory manager allocation/free/leaks detection for SPU memory.
	#ifdef _DEBUG
		#define O3D_SPU_MEMORY_MANAGER
	#endif
//
// MacOSX
//
#elif defined(__APPLE__) && defined(__MACH__)

	#ifndef __MACOSX__
		#define __MACOSX__
	#endif

	// support big endian for Motorola processors
	#ifdef __motorola__
		#define O3D_BIG_ENDIAN
	#endif

	#define O3D_MACOSX

	#if !defined(O3D_X11) || !defined (O3D_COCOA) || !defined(O3D_SDL)
		#define O3D_SDL
	#endif

	//! System API (for thread, mutex, semaphore, timers...)
	#define O3D_POSIX_SYS

	//! No memory manager allocation/free/leaks detection for global memory.

	//! No memory manager allocation/free/leaks detection for fast allocator.

	//! Memory manager allocation/free/leaks detection for GPU memory.
	#ifdef _DEBUG
		#define O3D_GPU_MEMORY_MANAGER
	#endif

	//! Memory manager allocation/free/leaks detection for SPU memory.
	#ifdef _DEBUG
		#define O3D_SPU_MEMORY_MANAGER
	#endif

#endif

// no SDL backward compatibility
#ifdef O3D_SDL
	#define SDL_NO_COMPAT
#endif

//! Enable SSE2 optimization in release mode (default SSE2 enabled)
#ifndef O3D_SIMD_OPTIMISATION
#define O3D_SIMD_OPTIMISATION 0
#endif

//! Enable SSE2 optimization in debug mode (default SSE2 enabled)
#ifndef O3D_SIMD_OPTIMISATION_DBG
#define O3D_SIMD_OPTIMISATION_DBG 0
#endif

//! CRT window report. Incompatibility when using timers and wnd assert reports.
//! Preferred value is 0.
#define O3D_USE_CRT_WNDREPORT 0

//! Max number of computed values in an evaluator
#define O3D_MAX_EVALUATOR_PRECISION 16

//! Numbers of loops to compute processor frequency
#define O3D_PROC_FREQ_LOOP 5000000

//! Max number of supported matrix for GPU skinning/rigging array
#define O3D_MAX_SKINNING_MATRIX_ARRAY 50

// Objective-3D version
#define O3D_VERSION                 0x0110      //!< Objective-3D version
#define O3D_VERSION_MAJOR           0x01        //!< Objective-3D major version
#define O3D_VERSION_MINOR           0x10        //!< Objective-3D minor version
#define O3D_VERSION_BETA            1           //!< beta version must put 1

#define O3D_VERSION_FILE_MIN        0x0110      //!< minimum O3D files version for reading

// Checking...
#ifdef O3D_WINDOWS
	#ifdef O3D_POSIX_SYS
	#error "O3D_POSIX_SYS is unsupported on Windows platforms"
	#endif
#else
	#ifdef O3D_WIN32_SYS
	#error "O3D_WIN32_SYS is not supported on non Windows platforms"
	#endif
#endif

// EAX sound support only on Windows
#if defined(O3D_BUILD_EAX) && !defined(O3D_WINDOWS)
	#error "EAX is only supported on Windows"
#endif

// SIMD optimization
#ifdef _DEBUG
	#if O3D_SIMD_OPTIMISATION_DBG
		#define O3D_USE_SIMD
	#endif
#else
	#if O3D_SIMD_OPTIMISATION
		#define O3D_USE_SIMD
	#endif
#endif

// EAX sound support FLAG
#ifdef O3D_BUILD_EAX
	#define O3D_EAX
#endif

#endif // _O3D_CONFIG_H
