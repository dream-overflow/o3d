/**
 * @file resourcemanager.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/resourcemanager.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/virtualfilelisting.h"

#include "o3d/core/debug.h"

#include <algorithm>

using namespace o3d;

// Default constructor.
ResourceManager::ResourceManager(
		BaseObject *parent,
		const String &path,
		const String &extList) :
    BaseObject(parent),
    m_extList(extList)
{
    if (path.isValid()) {
		String myPath(path);
		myPath.replace('\\', '/');
		myPath.trimRight('/');

		// make it absolute if necessary
		m_path = FileManager::instance()->getFullFileName(myPath);

		// and browse any files in the folder and zip files (and later in packed files)
		browseFolder(m_path);
	}
}

// Virtual destructor.
ResourceManager::~ResourceManager()
{
}

// Check for the existence of a resource.
Bool ResourceManager::isResourceExists(const String &resourceName) const
{
    FastMutexLocker locker(m_mutex);

	CIT_FilesMap cit = m_filesMap.find(resourceName);
    if (cit != m_filesMap.end()) {
		return True;
    } else {
		return False;
    }
}

// Add manually a resource.
void ResourceManager::addResource(const String &resourceName)
{
    FastMutexLocker locker(m_mutex);

	IT_FilesMap it = m_filesMap.find(resourceName);
    if (it == m_filesMap.end()) {
		m_filesMap.insert(std::make_pair(resourceName, m_path + '/' + resourceName));
    } else {
		O3D_ERROR(E_InvalidParameter("Resource name already exists"));
    }
}

// Remove manually a resource.
void ResourceManager::removeResource(const String &resourceName)
{
    FastMutexLocker locker(m_mutex);

	IT_FilesMap it = m_filesMap.find(resourceName);
    if (it != m_filesMap.end()) {
		m_filesMap.erase(it);
    } else {
		O3D_ERROR(E_InvalidParameter("Invalid resource name"));
    }
}

// Browse a sub directory.
void ResourceManager::browseFolder(const String &path)
{
	VirtualFileListing fileListing;
	fileListing.setPath(path);
	fileListing.setExt(m_extList);

	fileListing.searchFirstFile();

    FLItem *flItem = nullptr;
    while ((flItem = fileListing.searchNextFile()) != nullptr) {
        if (flItem->FileType == FILE_FILE) {
			// add a resource entry
			m_filesMap.insert(std::make_pair(flItem->FileName, fileListing.getFileFullName()));
		}
	}
}

String ResourceManager::getResourceName(const String &filename)
{
    FastMutexLocker locker(m_mutex);

    String name = filename;

    // absolute but reference a file of the managed folder
    if (filename.startsWith(m_path + '/')) {
        name.remove(0, m_path.length() + 1);
    }

     // search into the registered file name
    IT_FilesMap it = m_filesMap.find(name);
    if (it != m_filesMap.end()) {
        return name;
    } else {
        // check if the file was recently added into the folder
        // TODO
        return name;
    }
}

// Get the absolute file name for a relative one's.
String ResourceManager::getFullFileName(const String &filename)
{
    FastMutexLocker locker(m_mutex);

	String result;

    if (FileManager::instance()->isRelativePath(filename)) {
		// search into the registered file name
		IT_FilesMap it = m_filesMap.find(filename);
        if (it != m_filesMap.end()) {
			return it->second;
        } else {
			// we will try with the relative path...
			return FileManager::instance()->getFullFileName(filename);
        }
    } else {
        // absolute but reference a file of the managed folder
        if (filename.startsWith(m_path + '/')) {
            String name = filename;
            name.remove(0, m_path.length() + 1);

            // search into the registered file name
            IT_FilesMap it = m_filesMap.find(name);
            if (it != m_filesMap.end()) {
                return it->second;
            } else {
                return name;
            }
        } else {
            // absolute file name stay unchanged
            return filename;
        }
    }
}
