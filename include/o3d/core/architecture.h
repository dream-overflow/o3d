/**
 * @file architecture.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ARCHITECTURE_H
#define _O3D_ARCHITECTURE_H

#include "objective3dconfig.h"

#ifdef __MINGW32__
#include <w32api.h>
#endif

#ifdef O3D_WINDOWS
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	
	#ifdef __MINGW32__
		// WINVER
		#ifdef WINVER
		#undef WINVER
		#endif
		#define WINVER WindowsVista
	
		// WINNT
		#ifdef _WIN32_WINNT
		#undef _WIN32_WINNT
		#endif
		#define _WIN32_WINNT WindowsVista
	#endif

	#include <windows.h>

	// Remove macro declared by windows
	#undef GetFullPathName		
	#undef CopyFile
	#undef RemoveDirectory
	#undef CreateDirectory
	#undef GetDiskFreeSpace
	#undef GetCurrentDirectory
	#undef SetCurrentDirectory
	#undef GetTempFileName
	#undef GetTempPath
	#undef GetSystemDirectory
	#undef GetDriveType
	#undef UpdateResource
	#undef BeginUpdateResource
	#undef FindResource
	#undef SetEnvironmentVariable
	#undef GetEnvironmentVariable
	#undef GetCommandLine
	#undef GetStartupInfo
	#undef CreateProcess
	#undef GetModuleHandle
	#undef GetModuleFileName
	#undef LoadLibrary
	#undef OpenFileMapping
	#undef CreateFileMapping
	#undef OpenSemaphore
	#undef CreateSemaphore
	#undef OpenEvent
	#undef CreateEvent
	#undef OpenMutex
	#undef CreateMutex

	#undef FILE_ENCRYPTABLE
	#undef FILE_IS_ENCRYPTED
	#undef FILE_SYSTEM_ATTR
	#undef FILE_ROOT_DIR
	#undef FILE_SYSTEM_DIR
	#undef FILE_UNKNOWN
	#undef FILE_SYSTEM_NOT_SUPPORT
	#undef FILE_USER_DISALLOWED
	#undef FILE_READ_ONLY
	#undef FILE_DIR_DISALLOWED  

	#undef min
	#undef max

    #undef ERROR
#endif

//! MAX_PATH isn't defined under UNIX
#ifndef MAX_PATH
	#ifdef PATH_MAX
		#define MAX_PATH PATH_MAX
	#else
		#ifdef O3D_WINDOWS
			#define MAX_PATH 260
		#else
			#define MAX_PATH 1024
		#endif
	#endif
#endif

#endif // _O3D_ARCHITECTURE_H
