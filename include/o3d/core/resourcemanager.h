/**
 * @file resourcemanager.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_RESOURCEMANAGER_H
#define _O3D_RESOURCEMANAGER_H

#include "mutex.h"
#include "resource.h"
#include "o3d/core/stringmap.h"

namespace o3d {

/**
 * @brief Base resource manager.
 * @date 2008-11-23
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * The resource manager is able to manage a path containings somes resources that
 * are retrieved into the path by the file extensions according to its extensions list.
 * It is also possible to add virtual resources dynamicaly.
 * All resources are on the same path level, no sub-directory, and if ZIP files are found
 * they must not contains sub-directories neither, otherwise theirs sub-directories are
 * simply ignored.
 */
class O3D_API ResourceManager : public BaseObject
{
public:

	//! Constructor.
	//! @param parent Parent object.
	//! @param path Absolute or relative to current working directory path where to find resources.
	//! @param extList List of supported extension.
	ResourceManager(
			BaseObject *parent,
			const String &path,
			const String &extList);

	//! Virtual destructor.
	virtual ~ResourceManager();

	//! Get the absolute path containing resources.
	inline const String& getPath() const { return m_path; }

	//! Get the filtered extension list.
	inline const String& getExtList() const { return m_extList; }

    //! Get the resource name for a filename (absolute or relative).
    String getResourceName(const String &filename);

	//! Get the absolute file name for a relative one's or resource name.
	String getFullFileName(const String &resourceName);

	//! Check for the existence of a resource.
	Bool isResourceExists(const String &resourceName) const;

	//! Add manually a resource.
	void addResource(const String &resourceName);

	//! Remove manually a resource.
	void removeResource(const String &resourceName);

protected:

	FastMutex m_mutex;

	String m_path;      //!< Absolute path where resource are search in.
	String m_extList;   //!< Filtering file extensions

	//! Browse a sub directory.
	//! @param path Absolute path.
	virtual void browseFolder(const String &path);

    typedef StringMap<String> T_FilesMap;
    typedef T_FilesMap::IT IT_FilesMap;
    typedef T_FilesMap::CIT CIT_FilesMap;

	T_FilesMap m_filesMap;        //!< Resource name to absolute path files list.
};

} // namespace o3d

#endif // _O3D_RESOURCEMANAGER_H

