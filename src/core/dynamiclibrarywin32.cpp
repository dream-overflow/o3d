/**
 * @file dynamiclibrarywin32.cpp
 * @brief Implementation of DynamicLibrary.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-07-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/dynamiclibrary.h"

/* ONLY IF O3D_WIN32_SYS IS SELECTED */
#ifdef O3D_WIN32_SYS

#include <list>
#include "o3d/core/filemanager.h"
#include "o3d/core/architecture.h"
#include "o3d/core/debug.h"

using namespace o3d;

DynamicLibrary* DynamicLibrary::load(const String &name)
{
	String fullPath = FileManager::instance()->getFullFileName(name);
	for (std::list<DynamicLibrary*>::iterator it = ms_libraries.begin(); it != ms_libraries.end(); ++it)
	{
		if ((*it)->m_name == fullPath)
			throw new E_DynamicLibraryException("Already loaded library " + name);
	}

	DynamicLibrary *library = new DynamicLibrary;

	String lname = fullPath;
	lname.replace('/','\\');

	library->m_instance = (_HINSTANCE)LoadLibraryW(lname.getData());
	if (!library->m_instance)
	{
		deletePtr(library);
		throw new E_DynamicLibraryException("Cannot load library " + fullPath);
	}

	library->m_name = fullPath;
	ms_libraries.push_back(library);

	return library;
}

void DynamicLibrary::unload(DynamicLibrary *library)
{
	if (library == NULL)
		return;

	for (std::list<DynamicLibrary*>::iterator it = ms_libraries.begin(); it != ms_libraries.end(); ++it)
	{
		if ((*it) == library)
		{
			ms_libraries.erase(it);

			O3D_MESSAGE("Unload managed dynamic library " + library->m_name);

			FreeLibrary((HMODULE)library->m_instance);
			deletePtr(library);

			return;
		}
	}

	// unmanaged
	O3D_WARNING("Unload unmanaged dynamic library " + library->m_name);

	FreeLibrary((HMODULE)library->m_instance);
	deletePtr(library);
}

void* DynamicLibrary::getFunctionPtr(const CString &foo) const
{
    return (void*)GetProcAddress((HMODULE)m_instance, foo.getData());
}

#endif // O3D_WIN32_SYS

