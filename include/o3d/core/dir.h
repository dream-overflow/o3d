/**
 * @file dir.h
 * @brief management.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2007-06-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DIR_H
#define _O3D_DIR_H

#include "memorydbg.h"
#include "stringlist.h"
#include "virtualfilelisting.h"

namespace o3d {

typedef std::list<FLItem> T_FLItem_List;
typedef T_FLItem_List::iterator IT_FLItem_List;
typedef T_FLItem_List::const_iterator CIT_FLItem_List;

//---------------------------------------------------------------------------------------
//! @class Dir
//-------------------------------------------------------------------------------------
//! Abstract class for manager directory. @see O3DDiskDir for file system directory
//! management. Path are returned without trailing slash. Any trailing slash or backslash
//! are removed the given path name.
//---------------------------------------------------------------------------------------
class O3D_API Dir
{
public:

	//! format of the directory
	enum DirType
	{
		FILE_SYSTEM_DIR,        //!< local file system (real file on file system)
		FILE_VIRTUAL_DIR,       //!< virtual file system (virtual file on O3DZip)
		FILE_DIR,               //!< FTP directory
		UNKNOWN_TYPE_DIR
	};

	//! directory return codes
	enum DirReturn
	{
		SUCCESS,               //!< operation success
		NOT_EMPTY_DIR,         //!< the directory is not empty
		INVALID_PATH,          //!< the path is invalid
		ACCESS_PROGRAM,        //!< a program is currently accessing the directory
		NOT_WRITE_ACCESS,      //!< write access is requested
		NOT_READ_ACCESS,       //!< read access is requested
		NOT_PERMIT,            //!< the creation of a directory is not possible on the device
		ALREADY_EXISTS,        //!< the directory is already existing
		INVALID_ADDRESS,       //!< invalid address pointer
		TOO_LONG,              //!< the directory address is too long
		MEMORY_ERROR,          //!< not enough memory for the kernel
		READ_ONLY,             //!< the path is read-only
		CIRCULAR_REF,          //!< the path contain a circular reference due to a link
		INSUFISANCE_SPACE,     //!< the writing device does not have enough space for create the directory
		NOT_FOUND,             //!< invalid file descriptor
		OLD_PATH_IS_NOT_DIR,   //!< when rename, old path is not a directory path
		NOT_SOME_ENV,          //!< old path and new path are not on the same file system (environment)
		CANT_CREATE_DEST_FILE, //!< can create the destination copy file
		UNKNOWN_RET
	};

	//! default constructor. Is the path is empty '.' is used.
	Dir(const String& pathname);

	//! copy constructor
	Dir(const Dir& dup) :
		m_type(dup.m_type),
		m_isValid(dup.m_isValid),
		m_fullPathname(dup.m_fullPathname),
		m_pathName(dup.m_pathName)
	{
	}

	//! duplicator
	inline Dir& operator=(const Dir &dup)
	{
		m_type = dup.m_type;
		m_isValid = dup.m_isValid;
		m_fullPathname = dup.m_fullPathname;
		m_pathName = dup.m_pathName;

		return (*this);
	}

	//! return the type of the directory
	inline DirType getType() const { return m_type; }

	//! clean the path (remove '..' and '.' when possible)
	virtual void clean() = 0;

	//! set the full path name of this
	Bool setPathName(const String &pathName);

	//! return the pathname (not the full path, only its name) without trailing slash.
    inline const String& getPathName() const { return m_pathName; }

	//! return the full pathname without trailing slash.
	inline const String& getFullPathName() const { return m_fullPathname; }

	//! concat the filename to the directory full-path (does not check its existence)
	String makeFullFileName(const String &filename);

	//! concat the filename to the directory relative path (does not check its existence)
	String makeFileName(const String &filename);

	//! concat the pathname to the full-path (does not check its existence)
	String makeFullPathName(const String &pathName);

	//! concat the pathname to the relative path (does not check its existence)
	String makePathName(const String &pathName);

    //! adapt a filename to this path. Note: the filename path and this Dir must have a common root.
	//! e.g:
	//!     'c:/tool/one/two/file.txt' as source
	//!     'c:/tool/one/one/one' as path
	//!	     result in '../../two/file.txt'
	String makeRelative(const String &filename) const;

	//! is an absolute or relative path ?
	virtual Bool isAbsolute() const = 0;

	//! is the path is readable (check the rights)
	virtual Bool isReadable() const = 0;

	//! is the path is writable (check the rights)
	virtual Bool isWritable() const = 0;

	//! is it the root directory
	virtual Bool isRoot() const = 0;

	//! remove a directory in the dir path (if the rights permits it)
	virtual DirReturn removeDir(const String &path) const = 0;

	//! create a sub directory in the dir path (if the rights permits it)
	virtual DirReturn makeDir(const String &path) const = 0;

	//! create the complete path in the Dir path
	virtual DirReturn makePath(const String &path) const = 0;

	//! check for the existence of a sub directory or a file
	virtual Dir::DirReturn check(const String &fileOrPath) const = 0;

	//! check if Dir exist
	inline Bool isExist() const
	{
		return m_isValid && (check("") == SUCCESS);
	}

	//! change directory. return true if the new directory exists and it is readable
	Bool cd(const String &path);

	//! up directory. return true if the new directory exists and it is readable
	inline Bool cdUp()
	{
		return cd("..");
	}

	//! search for a list of file. filters must be spaced by '|'.
	//! If no filters is specified all files all files and directory listed are returned.
	//! Note : the 'DIR' filter, filter for directories. You can see also FileListing and
	//! VirtualFileListing.
	virtual T_FLItem_List findFilesInfos(
			const String &filters = "",
			FileTypes Type = FILE_BOTH) const = 0;

	//! Same as FindFiles but return only strings
	virtual T_StringList findFiles(
			const String &filters = "",
			FileTypes Type = FILE_BOTH) const = 0;

	//! make absolute. make - if relative - this path absolute depend to the current working directory
	//! Note : work only for O3DDiskDir.
	virtual Bool makeAbsolute() = 0;

	//! remove the file in directory
	virtual Dir::DirReturn removeFile(const String &filename) const = 0;

	//! rename a file or a directory
	virtual Dir::DirReturn rename(const String &oldName, const String &newName) const = 0;

	//! copy the given absolute filename/relative to working directory, to this dir
	virtual Dir::DirReturn copyFile(const String &filename, UInt32 BlockSize = 32768) const = 0;

	//! check if two dir are the same
	inline Bool operator== (const Dir &cmp) const
	{
		if (!m_isValid || !cmp.m_isValid)
			return False;
		return (m_fullPathname == cmp.m_fullPathname);
	}

	//! check if two dir are different
	inline Bool operator!= (const Dir &cmp) const
	{
		if (!m_isValid || !cmp.m_isValid)
			return False;
		return (m_fullPathname != cmp.m_fullPathname);
	}

protected:

	DirType m_type;            //!< type of the directory

	Bool m_isValid;        //!< is the pathname valid

	String m_fullPathname;  //!< absolute pathname (without trailing slash)
	String m_pathName;      //!< only the name of the path (ie let full path '/foo/bar' then 'bar')
};

} // namespace o3d

#endif // _O3D_DIR_H

