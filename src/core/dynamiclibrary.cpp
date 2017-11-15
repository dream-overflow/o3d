/**
 * @file dynamiclibrary.cpp
 * @brief Implementation of DynamicLibrary.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-07-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/dynamiclibrary.h"
#include "o3d/core/filemanager.h"

using namespace o3d;

std::list<DynamicLibrary*> DynamicLibrary::ms_libraries;

Bool DynamicLibrary::isLoaded(const String &name)
{
	String fullPath = FileManager::instance()->getFullFileName(name);
	for (std::list<DynamicLibrary*>::iterator it = ms_libraries.begin(); it != ms_libraries.end(); ++it)
	{
		if ((*it)->m_name == fullPath)
			return True;
	}

	return False;
}

const DynamicLibrary::T_LibraryList& DynamicLibrary::getLoadedLibraries()
{
	return ms_libraries;
}

UInt32 DynamicLibrary::getNumLoadedLibraries()
{
	return ms_libraries.size();
}

