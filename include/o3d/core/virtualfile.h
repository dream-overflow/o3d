/**
 * @file virtualfile.h
 * @brief Virtual (asset) sub file system file.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-17
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_VIRTUALFILE_H
#define _O3D_VIRTUALFILE_H

#include "memorydbg.h"
#include "basefile.h"

namespace o3d {

/**
 * @brief Virtual (asset) sub file system file.
 */
class O3D_API VirtualFile : public BaseFile
{
public:

    //! default constructor
    VirtualFile(const String &filename);

    //! constructor with separates pathname and filename
    VirtualFile(const String &pathname, const String &filename);

    //! constructor with separates pathname and filename
    VirtualFile(const BaseDir &dir, const String &filename);

    //! copy constructor
    VirtualFile(const BaseFile& dup);

    virtual ~VirtualFile();

    virtual BaseFile* clone() const override;

    //! set the filename
    virtual void setFile(const String &filename) override;

    //! set from a separates pathname and filename
    virtual void setFile(const String &pathname, const String &filename) override;

    //! set from separates pathname and filename
    virtual void setFile(const BaseDir &dir, const String &filename) override;

    //! Get the file type.
    virtual FileTypes getType() override;

    //! Get the file size in bytes.
    virtual UInt64 getFileSize() override;

    //! check if the filename exists
    virtual Bool exists() override;

    //! is an absolute or relative filename ?
    virtual Bool isAbsolute() const override;

    //! always readable
    virtual Bool isReadable() override;

    //! never writable
    virtual Bool isWritable() override;

    //! is the file is hidden by the system
    virtual Bool isHidden() override;

    //! return the owner id
    virtual Int16 getOwnerId() override;

    //! get the file owner
    virtual const String& getOwnerName() override;

    //! return the owner group id
    virtual Int16 getGroupId() override;

    //! return the group string name
    virtual const String& getGroupName() override;

    //! make absolute. make - if relative - this file name absolute depend to the current working directory
    virtual Bool makeAbsolute() override;

    //! get the creation date/time
    virtual const DateTime& getCreationDate() override;

    //! get the last access date/time
    virtual const DateTime& getLastAccessDate() override;

    //! get the last modification date/time
    virtual const DateTime& getModifiedDate() override;

    //! is the file a symbolic link
    virtual Bool isSymbolicLink() override;

    //! is the file is in the root directory
    virtual Bool isInRoot() const override;

protected:

    // @todo could have a string on the name of the related asset

    //! fill the CachedData structure if cache is enable
    virtual void cacheDatas() override;
};

} // namespace o3d

#endif // _O3D_VIRTUALFILEINFO_H
