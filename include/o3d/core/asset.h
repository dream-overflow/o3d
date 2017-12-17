/**
 * @file asset.h
 * @brief Asset virtual controller.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-17
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_ASSET_H
#define _O3D_ASSET_H

#include "memorydbg.h"

#include "string.h"
#include "hashmap.h"

#include <vector>
#include <list>

namespace o3d {

/**
 * @brief Asset interface. Usefull to add a specific asset handling like zip support,
 * Android asset, or any other protocol.
 */
class O3D_API Asset
{
public:

    virtual ~Asset() = 0;

    /**
     * @brief Release all datas and close the asset.
     */
    virtual void destroy() = 0;

    /**
     * @brief Return a file index in the list (-1 if not found).
     * @param fileName the path of the filename is relative
     * @return index of the file, or -1 if not found.
     */
    virtual Int32 findFile(const String& fileName) = 0;

    /**
     * @brief Is a path defined into this asset.
     * @param path the path is relative
     * @return True if exists.
     */
    virtual Bool isPath(const String& path) const = 0;

    /**
     * @brief Open for a reading stream.
     * @return An opened input stream.
     * @throws E_FileNotFoundOrInvalidRights if cannot open the file name.
     */
    virtual InStream* openInStream(const String& filename) = 0;

    /**
     * @brief Open for a reading stream.
     * @return An opened input stream.
     * @throws E_FileNotFoundOrInvalidRights if cannot open the file at index.
     */
    virtual InStream* openInStream(Int32 index) = 0;

    /**
     * @return Returns the location relative to the file system, or global
     */
    virtual String location() const = 0;

    /**
     * @return Returns the name (filename...).
     */
    virtual String name() const = 0;

    /**
     * @return Returns the full path name location/name.
     */
    virtual String fullPathName() const = 0;

    /**
     * @return Returns the protocol (file://, http://, ftp://, media://...)
     */
    virtual String protocol() const = 0;

    /**
    * @brief Get the number of files in this asset (if possible).
    */
    virtual Int32 getNumFiles() const = 0;

    /**
     * @brief get the file name at a specified index, prefixed by the local path.
     * @param index
     * @return
     */
    virtual String getFileName(Int32 index) const = 0;

    /**
     * @brief get the file type at a specified index.
     * @param index
     * @return
     */
    virtual FileTypes getFileType(Int32 index) const = 0;

    /**
     * @brief get file size for a file into this asset.
     * @param fileName
     * @return
     */
    virtual UInt64 getFileSize(Int32 index) const = 0;
};

typedef std::list<Asset*> T_AssetList;
typedef T_AssetList::iterator IT_AssetList;
typedef T_AssetList::const_iterator CIT_AssetList;

} // namespace o3d

#endif // _O3D_ASSET_H
