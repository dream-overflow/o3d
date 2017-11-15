/**
 * @file diskfileinfo.h
 * @brief File system version of O3DFileInfo.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-06-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DISKFILEINFO_H
#define _O3D_DISKFILEINFO_H

#include "memorydbg.h"
#include "fileinfo.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class DiskFileInfo
//-------------------------------------------------------------------------------------
//! File system version of FileInfo.
//---------------------------------------------------------------------------------------
class O3D_API DiskFileInfo : public FileInfo
{
protected:

	//! fill the CachedData structure if cache is enable
	virtual void cacheDatas();

public:

	//! default constructor
	DiskFileInfo(const String &filename) :
		FileInfo(filename)
	{
		m_type = FileInfo::FILE_SYSTEM_FILE_INFO;
		m_isValid = isExist();
	}

	//! constructor with separates pathname and filename
	DiskFileInfo(const String &pathname,const String &filename) :
		FileInfo(pathname,filename)
	{
		m_type = FileInfo::FILE_SYSTEM_FILE_INFO;
		m_isValid = isExist();
	}

	//! constructor with separates pathname and filename
	DiskFileInfo(const Dir &dir,const String &filename) :
		FileInfo(dir,filename)
	{
		m_type = FileInfo::FILE_SYSTEM_FILE_INFO;
		m_isValid = isExist();
	}

	//! copy constructor
	DiskFileInfo(const FileInfo& dup) :
		FileInfo(dup)
	{
		m_type = FileInfo::FILE_SYSTEM_FILE_INFO;
		m_isValid = isExist();
	}

	virtual ~DiskFileInfo() {}

	//! set the filename
	void setFile(const String &filename)
	{
		FileInfo::setFile(filename);
		m_isValid = isExist();
	}

	//! set from a separates pathname and filename
	void setFile(const String &pathname,const String &filename)
	{
		FileInfo::setFile(pathname,filename);
		m_isValid = isExist();
	}

	//! set from separates pathname and filename
	void setFile(const Dir &dir, const String &filename)
	{
		FileInfo::setFile(dir,filename);
		m_isValid = isExist();
	}

	//! Get the file type.
	virtual FileTypes getType();

	//! Get the file size in bytes.
	virtual UInt64 getFileSize();

	//! check if the filename exists
	virtual Bool isExist();

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
	//! @note Works only for O3DDiskFileInfo.
	virtual Bool makeAbsolute();

	//! get the creation date/time
	virtual const Date& getCreationDate();

	//! get the last access date/time
	virtual const Date& getLastAccessDate();

	//! get the last modification date/time
	virtual const Date& getModifiedDate();

	//! is the file a symbolic link
	virtual Bool isSymbolicLink();

	//! is the file is in the root directory
	virtual Bool isInRoot() const;
};

} // namespace o3d

#endif // _O3D_DISKFILEINFO_H

