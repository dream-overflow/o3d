/**
 * @file virtualdir.h
 * @brief Virtual directory management.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-17
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_VIRTUALDIR_H
#define _O3D_VIRTUALDIR_H

#include "memorydbg.h"
#include "basedir.h"

namespace o3d {

/**
 * @brief Virtual directory management. Path are return without trailing slash.
 * Given path are removed of theirs trailing slash, and any backslash are changed into slash.
 * VirtualDir refers to a path into a virtual (asset) sub file system.
 */
class O3D_API VirtualDir : public BaseDir
{
public:

    //! default constructor. Is the path is empty '.' is used.
    VirtualDir(const String& pathname = String());

    //! copy constructor
    VirtualDir(const VirtualDir& dup);

    virtual ~VirtualDir();

    //! clean the path (remove '..' and '.' when possible)
    virtual void clean() override;

    //! check if the directory is empty
    virtual Bool empty() const override;

    //! is an absolute or relative path ?
    virtual Bool isAbsolute() const override;

    //! always readable
    virtual Bool isReadable() const override;

    //! never writable
    virtual Bool isWritable() const override;

    //! is it the root directory
    virtual Bool isRoot() const override;

    //! throws E_InvalidResult
    virtual DirReturn removeDir(const String &path) const override;

    //! throws E_InvalidResult
    virtual DirReturn makeDir(const String &path) const override;

    //! throws E_InvalidResult
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

    //! throws E_InvalidResult
    virtual BaseDir::DirReturn copyFile(const String &filename, UInt32 BlockSize = 32768) const override;

    //! throws E_InvalidResult
    virtual BaseDir::DirReturn removeFile(const String &filename) const override;

    //! throws E_InvalidResult
    virtual BaseDir::DirReturn rename(const String &oldName, const String &newName) const override;
};

} // namespace o3d

#endif // _O3D_VIRTUALDIR_H
