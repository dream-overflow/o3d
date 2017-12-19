/**
 * @file locladir.h
 * @brief Local File system directory management.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-06-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_LOCALDIR_H
#define _O3D_LOCALDIR_H

#include "memorydbg.h"
#include "basedir.h"

namespace o3d {

/**
 * @brief File system directory management. Path are return without trailing slash.
 * Given path are removed of theirs trailing slash, and any backslash are changed into slash.
 */
class O3D_API LocalDir : public BaseDir
{
public:

    //! Construct using the current directory.
    static LocalDir current();

	//! default constructor. Is the path is empty '.' is used.
    LocalDir(const String& pathname = String());

	//! copy constructor
    LocalDir(const LocalDir& dup);

    virtual ~LocalDir();

    virtual BaseDir* clone() const override;

	//! clean the path (remove '..' and '.' when possible)
    virtual void clean() override;

    //! check if the directory is empty
    virtual Bool empty() const override;

	//! is an absolute or relative path ?
    virtual Bool isAbsolute() const override;

	//! is the path is readable (check the rights)
    virtual Bool isReadable() const override;

	//! is the path is writable (check the rights)
    virtual Bool isWritable() const override;

	//! is it the root directory
    virtual Bool isRoot() const override;

    //! remove a directory in the Dir path (if the rights permits it)
    virtual DirReturn removeDir(const String &path) const override;

	//! create a sub directory in the O3DDir path (if the rights permits it)
	//! @note Mode is set to 0775.
    virtual DirReturn makeDir(const String &path) const override;

	//! create the complete path in the O3DDir path
	//! @note Mode is set to 0775.
    virtual DirReturn makePath(const String &path) const override;

	//! check for the existence of a sub directory or a file
    virtual BaseDir::DirReturn check(const String &fileOrPath) const override;

	//! search for a list of file. filters must be spaced by '|'.
	//! If no filters is specified all files all files and directory listed are returned.
	//! Note : the 'DIR' filter, filter for directories. You can see also FileListing and
	//! VirtualFileListing.
	virtual T_FLItem_List findFilesInfos(
			const String &filters = "",
            FileTypes Type = FILE_BOTH) const override;

	//! Same as FindFiles but return only strings
	virtual T_StringList findFiles(
			const String &filters = "",
            FileTypes Type = FILE_BOTH) const override;

	//! make absolute. make - if relative - this path absolute depend to the current working directory
    virtual Bool makeAbsolute() override;

	//! copy the given absolute filename/relative to working directory, to this dir
    virtual BaseDir::DirReturn copyFile(const String &filename, UInt32 BlockSize = 32768) const override;

	//! remove the file in directory
    virtual BaseDir::DirReturn removeFile(const String &filename) const override;

	//! rename a file or a directory
    virtual BaseDir::DirReturn rename(const String &oldName, const String &newName) const override;
};

} // namespace o3d

#endif // _O3D_LOCALDIR_H
