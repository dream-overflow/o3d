/**
 * @file dir.h
 * @brief management.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-06-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_DIR_H
#define _O3D_DIR_H

#include "basedir.h"
#include "memorydbg.h"

namespace o3d {

class BaseFile;
class DateTime;

/**
 * @brief Directory information and manipulator.
 * @details This is a wrapper upside BaseDir specialization, using
 * the file manager to initiate internal data.
 * Path are returned without trailing slash. Any trailing slash or backslash
 * are removed the given path name.
 */
class O3D_API Dir
{
public:

    //! default constructor. Is the path is empty '.' is used.
    Dir(const String& pathname = String());

    //! copy constructor
    Dir(const Dir& dup);

    //! copy constructor
    Dir(const BaseDir& dup);

    //! own constructor. Own the given pointer.
    Dir(BaseDir *own);

    ~Dir();

    //! duplicator
    Dir& operator=(const BaseDir &dup);

    //! duplicator
    Dir& operator=(const Dir &dup);

    //! return the type of the directory
    FileLocation getFileLocation() const;

    //! clean the path (remove '..' and '.' when possible)
    void clean();

    //! set the full path name of this
    Bool setPathName(const String &pathName);

    //! return the pathname (not the full path, only its name) without trailing slash.
    const String& getPathName() const;

    //! return the full pathname without trailing slash.
    const String& getFullPathName() const;

    //! concat the filename to the directory full-path (does not check its existence)
    String makeFullFileName(const String &filename) const;

    //! concat the filename to the directory relative path (does not check its existence)
    String makeFileName(const String &filename) const;

    //! concat the pathname to the full-path (does not check its existence)
    String makeFullPathName(const String &pathName) const;

    //! concat the pathname to the relative path (does not check its existence)
    String makePathName(const String &pathName) const;

    //! adapt a filename to this path. Note: the filename path and this Dir must have a common root.
    //! e.g:
    //!     'c:/tool/one/two/file.txt' as source
    //!     'c:/tool/one/one/one' as path
    //!	     result in '../../two/file.txt'
    String makeRelative(const String &filename) const;

    //! is an absolute or relative path ?
    Bool isAbsolute() const;

    //! is the path is readable (check the rights)
    Bool isReadable() const;

    //! is the path is writable (check the rights)
    Bool isWritable() const;

    //! is it the root directory
    Bool isRoot() const;

    //! remove a directory in the dir path (if the rights permits it)
    BaseDir::DirReturn removeDir(const String &path) const;

    //! create a sub directory in the dir path (if the rights permits it)
    BaseDir::DirReturn makeDir(const String &path) const;

    //! create the complete path in the Dir path
    BaseDir::DirReturn makePath(const String &path) const;

    //! check for the existence of a sub directory or a file
    BaseDir::DirReturn check(const String &fileOrPath) const;

    //! check if Dir exist
    Bool exists() const;

    //! check if the directory is empty
    Bool empty() const;

    //! change directory. return true if the new directory exists and it is readable
    Bool cd(const String &path);

    //! up directory. return true if the new directory exists and it is readable
    Bool cdUp();

    //! search for a list of file. filters must be spaced by '|'.
    //! If no filters is specified all files all files and directory listed are returned.
    //! Note : the 'DIR' filter, filter for directories. You can see also FileListing and
    //! VirtualFileListing.
    T_FLItem_List findFilesInfos(const String &filters = "",
            FileTypes type = FILE_BOTH) const;

    //! Same as FindFiles but return only strings
    T_StringList findFiles(
            const String &filters = "",
            FileTypes type = FILE_BOTH) const;

    //! make absolute. make - if relative - this path absolute depend to the current working directory
    Bool makeAbsolute();

    //! remove the file in directory
    BaseDir::DirReturn removeFile(const String &filename) const;

    //! rename a file or a directory
    BaseDir::DirReturn rename(const String &oldName, const String &newName) const;

    //! copy the given absolute filename/relative to working directory, to this dir
    BaseDir::DirReturn copyFile(const String &filename, UInt32 blockSize = 32768) const;

    //-----------------------------------------------------------------------------------
    // Comparison operators
    //-----------------------------------------------------------------------------------

    //! check if two dir are the same
    Bool operator== (const Dir &cmp) const;

    //! check if two dir are different
    Bool operator!= (const Dir &cmp) const;

    //! check if two dir are the same
    Bool operator== (const BaseDir &cmp) const;

    //! check if two dir are different
    Bool operator!= (const BaseDir &cmp) const;

private:

    BaseDir *m_di;
};

} // namespace o3d

#endif // _O3D_DIR_H
