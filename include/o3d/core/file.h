/**
 * @file file.h
 * @brief Base base for disk and memory file objects.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-01-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FILE_H
#define _O3D_FILE_H

#include "string.h"
#include "memorydbg.h"

namespace o3d {

class BaseFileInfo;
class BaseDir;
class DateTime;

/**
 * @brief File information and manipulator.
 * @details This is a wrapper upside BaseFileInfo specialization, using
 * the file manager to initiate internal data.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-18
 */
class O3D_API File
{
public:

    //! default constructor
    File(const String &fileName);

    //! constructor with separates pathname and filename
    File(const String &pathName, const String &fileName);

    //! constructor with separates pathname and filename
    File(const BaseDir &dir, const String &fileName);

    //! copy constructor
    File(const File &dup);

    //! copy constructor
    File(const BaseFileInfo &dup);

    ~File();

    //! Get the file type.
    FileTypes getType();

    //! Get the file size in bytes.
    UInt64 getFileSize();

    //! check if the filename exists
    Bool exists();

    //! is an absolute or relative filename ?
    Bool isAbsolute() const;

    //! is the file readable (check the rights)
    Bool isReadable();

    //! is the file writable (check the rights)
    Bool isWritable();

    //! is the file is hidden by the system
    Bool isHidden();

    //! return the owner id
    Int16 getOwnerId();

    //! get the file owner
    const String& getOwnerName();

    //! return the owner group id
    Int16 getGroupId();

    //! return the group string name
    const String& getGroupName();

    //! make absolute. make - if relative - this file name absolute depend to the current working directory
    Bool makeAbsolute();

    //! get the creation date/time
    const DateTime& getCreationDate();

    //! get the last access date/time
    const DateTime& getLastAccessDate();

    //! get the last modification date/time
    const DateTime& getModifiedDate();

    //! is the file a symbolic link
    Bool isSymbolicLink();

    //! is the file is in the root directory
    Bool isInRoot() const;

    //! adapt a filename to this path. Note: the filename path and the Dir must have a common root.
    String makeRelative(const BaseDir &dir) const;

    //-----------------------------------------------------------------------------------
    // Accessors
    //-----------------------------------------------------------------------------------

    //! return the file information type
    FileLocation getFileLocation() const;

    //! return the file name only (without the path)
    const String& getFileName() const;

    //! return the full file name (with complete path)
    const String& getFullFileName() const;

    //! return the file extension only
    const String& getFileExt() const;

    //! return the file path only, without trailing slash.
    String getFilePath() const;

    //-----------------------------------------------------------------------------------
    // Comparison operators
    //-----------------------------------------------------------------------------------

    //! check if two dir are the same
    Bool operator== (const File &cmp) const;

    //! check if two dir are different
    Bool operator!= (const File &cmp) const;

    //! check if two dir are the same
    Bool operator== (const BaseFileInfo &cmp) const;

    //! check if two dir are different
    Bool operator!= (const BaseFileInfo &cmp) const;

private:

    BaseFileInfo *m_fi;
};

} // namespace o3d

#endif // _O3D_FILE_H
