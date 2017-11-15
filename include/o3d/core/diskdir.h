/**
 * @file diskdir.h
 * @brief File system directory management.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-06-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DISKDIR_H
#define _O3D_DISKDIR_H

#include "memorydbg.h"
#include "dir.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class DiskDir
//-------------------------------------------------------------------------------------
//! File system directory management. Path are return without trailing slash. Given path
//! are removed of theirs trailing slash, and any backslash are changed into slash.
//---------------------------------------------------------------------------------------
class O3D_API DiskDir : public Dir
{
public:

	//! Construct an DiskDir using the current directory.
	static DiskDir current();

	//! default constructor. Is the path is empty '.' is used.
	DiskDir(const String& pathname) :
		Dir(pathname)
	{
		m_type = Dir::FILE_SYSTEM_DIR;
	}

	//! copy constructor
	DiskDir(const DiskDir& dup) :
		Dir(dup)
	{
		m_type = Dir::FILE_SYSTEM_DIR;
	}

	virtual ~DiskDir() {}

	//! clean the path (remove '..' and '.' when possible)
	virtual void clean();

	//! is an absolute or relative path ?
	virtual Bool isAbsolute() const;

	//! is the path is readable (check the rights)
	virtual Bool isReadable() const;

	//! is the path is writable (check the rights)
	virtual Bool isWritable() const;

	//! is it the root directory
	virtual Bool isRoot() const;

	//! remove a directory in the O3DDir path (if the rights permits it)
	virtual DirReturn removeDir(const String &path) const;

	//! create a sub directory in the O3DDir path (if the rights permits it)
	//! @note Mode is set to 0775.
	virtual DirReturn makeDir(const String &path) const;

	//! create the complete path in the O3DDir path
	//! @note Mode is set to 0775.
	virtual DirReturn makePath(const String &path) const;

	//! check for the existence of a sub directory or a file
	virtual Dir::DirReturn check(const String &fileOrPath) const;

	//! search for a list of file. filters must be spaced by '|'.
	//! If no filters is specified all files all files and directory listed are returned.
	//! Note : the 'DIR' filter, filter for directories. You can see also FileListing and
	//! VirtualFileListing.
	virtual T_FLItem_List findFilesInfos(
			const String &filters = "",
			FileTypes Type = FILE_BOTH) const;

	//! Same as FindFiles but return only strings
	virtual T_StringList findFiles(
			const String &filters = "",
			FileTypes Type = FILE_BOTH) const;

	//! make absolute. make - if relative - this path absolute depend to the current working directory
	virtual Bool makeAbsolute();

	//! copy the given absolute filename/relative to working directory, to this dir
	virtual Dir::DirReturn copyFile(const String &filename, UInt32 BlockSize = 32768) const;

	//! remove the file in directory
	virtual Dir::DirReturn removeFile(const String &filename) const;

	//! rename a file or a directory
	virtual Dir::DirReturn rename(const String &oldName, const String &newName) const;
};

} // namespace o3d

#endif // _O3D_DISKDIR_H

