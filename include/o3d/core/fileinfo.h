/**
 * @file fileinfo.h
 * @brief File system version of O3DFileInfo.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-06-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FILEINFO_H
#define _O3D_FILEINFO_H

#include "memorydbg.h"
#include "basefileinfo.h"

namespace o3d {

/**
 * @brief File system version of FileInfo.
 */
class O3D_API FileInfo : public BaseFileInfo
{
protected:

	//! fill the CachedData structure if cache is enable
	virtual void cacheDatas();

public:

	//! default constructor
    FileInfo(const String &filename) :
        BaseFileInfo(filename)
	{
        m_type = BaseFileInfo::FILE_SYSTEM_FILE_INFO;
        m_isValid = exists();
	}

	//! constructor with separates pathname and filename
    FileInfo(const String &pathname,const String &filename) :
        BaseFileInfo(pathname,filename)
	{
        m_type = BaseFileInfo::FILE_SYSTEM_FILE_INFO;
        m_isValid = exists();
	}

	//! constructor with separates pathname and filename
    FileInfo(const BaseDir &dir,const String &filename) :
        BaseFileInfo(dir,filename)
	{
        m_type = BaseFileInfo::FILE_SYSTEM_FILE_INFO;
        m_isValid = exists();
	}

	//! copy constructor
    FileInfo(const BaseFileInfo& dup) :
        BaseFileInfo(dup)
	{
        m_type = BaseFileInfo::FILE_SYSTEM_FILE_INFO;
        m_isValid = exists();
	}

    virtual ~FileInfo() {}

	//! set the filename
	void setFile(const String &filename)
	{
        BaseFileInfo::setFile(filename);
        m_isValid = exists();
	}

	//! set from a separates pathname and filename
	void setFile(const String &pathname,const String &filename)
	{
        BaseFileInfo::setFile(pathname,filename);
        m_isValid = exists();
	}

	//! set from separates pathname and filename
    void setFile(const BaseDir &dir, const String &filename)
	{
        BaseFileInfo::setFile(dir,filename);
        m_isValid = exists();
	}

	//! Get the file type.
	virtual FileTypes getType();

	//! Get the file size in bytes.
	virtual UInt64 getFileSize();

	//! check if the filename exists
    virtual Bool exists();

	//! is an absolute or relative filename ?
	virtual Bool isAbsolute() const;

	//! is the file readable (check the rights)
	virtual Bool isReadable();

	//! is the file writable (check the rights)
	virtual Bool isWritable();

	//! is the file is hidden by the system
	virtual Bool isHidden();

	//! return the owner id
	virtual Int16 getOwnerId();

	//! get the file owner
	virtual const String& getOwnerName();

	//! return the owner group id
	virtual Int16 getGroupId();

	//! return the group string name
	virtual const String& getGroupName();

	//! make absolute. make - if relative - this file name absolute depend to the current working directory
    //! @note Works only for FileInfo.
	virtual Bool makeAbsolute();

	//! get the creation date/time
    virtual const DateTime& getCreationDate();

	//! get the last access date/time
    virtual const DateTime& getLastAccessDate();

	//! get the last modification date/time
    virtual const DateTime& getModifiedDate();

	//! is the file a symbolic link
	virtual Bool isSymbolicLink();

	//! is the file is in the root directory
	virtual Bool isInRoot() const;
};

} // namespace o3d

#endif // _O3D_FILEINFO_H
