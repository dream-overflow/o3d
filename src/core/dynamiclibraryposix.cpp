/**
 * @file dynamiclibraryposix.cpp
 * @brief Implementation of DynamicLibrary.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-07-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/dynamiclibrary.h"

/* ONLY IF O3D_POSIX IS SELECTED */
#ifdef O3D_POSIX_SYS

#include <list>
#include "o3d/core/filemanager.h"
#include "o3d/core/architecture.h"
#include "o3d/core/debug.h"

#include <sys/types.h>
#include <dlfcn.h>

using namespace o3d;

DynamicLibrary* DynamicLibrary::load(const String &name)
{
	String fullPath = FileManager::instance()->getFullFileName(name);
    for (std::list<DynamicLibrary*>::iterator it = ms_libraries.begin(); it != ms_libraries.end(); ++it) {
        if ((*it)->m_name == fullPath) {
            O3D_ERROR(E_DynamicLibraryException("Already loaded library " + name));
        }
	}

	DynamicLibrary *library = new DynamicLibrary;

	library->m_instance = dlopen(fullPath.toUtf8().getData(), RTLD_LAZY);
    if (!library->m_instance) {
		deletePtr(library);
        O3D_ERROR(E_DynamicLibraryException("Cannot load library " + fullPath));
	}

	library->m_name = fullPath;
	ms_libraries.push_back(library);

	return library;
}

void DynamicLibrary::unload(DynamicLibrary *library)
{
    if (library == nullptr) {
		return;
    }

    for (std::list<DynamicLibrary*>::iterator it = ms_libraries.begin(); it != ms_libraries.end(); ++it) {
        if ((*it) == library) {
			ms_libraries.erase(it);

			O3D_MESSAGE("Unload managed dynamic library " + library->m_name);

			dlclose(library->m_instance);
			deletePtr(library);

			return;
		}
	}

	// unmanaged
	O3D_WARNING("Unload unmanaged dynamic library " + library->m_name);

	dlclose(library->m_instance);
	deletePtr(library);
}

void* DynamicLibrary::getFunctionPtr(const CString &foo) const
{
	return dlsym(m_instance, foo.getData());
}

#endif // O3D_POSIX_SYS
