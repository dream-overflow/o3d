/**
 * @file assetandroid.h
 * @brief Android Asset controller.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-17
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_ASSETANDROID_H
#define _O3D_ASSETANDROID_H

#include "../asset.h"

#ifdef O3D_ANDROID

struct AAssetManager;

namespace o3d {

/**
 * @brief Asset interface. Usefull to add a specific asset handling like zip support,
 * Android asset, or any other protocol.
 */
class O3D_API AssetAndroid : public Asset
{
public:

    AssetAndroid();
    virtual ~AssetAndroid();

    /**
     * @brief Release all datas and close the asset.
     */
    virtual void destroy() override;

    virtual Int32 findFile(const String& fileName) override;

    virtual Bool isPath(const String& path) const override;

    virtual InStream* openInStream(const String& fileName) override;
    virtual InStream* openInStream(Int32 index) override;

    /**
     * @return Return "".
     */
    virtual String location() const override;

    /**
     * @return Returns the empty name "".
     */
    virtual String name() const override;

    /**
     * @return Returns the empty path "".
     */
    virtual String fullPathName() const override;

    /**
     * @return Returns the protocol (android://)
     */
    virtual String protocol() const override;

    /**
    * @brief Get the number of files in this asset (if possible).
    */
    virtual Int32 getNumFiles() const override;

    virtual String getFileName(Int32 index) const override;

    virtual FileTypes getFileType(Int32 index) const override;

    virtual UInt64 getFileSize(Int32 index) const override;

    virtual void searchFirstFile(const String &path) override;

protected:

    struct AssetToken
    {
        UInt64 FilePos;		    //!< File token position since data chunk start
        String FileName;		//!< File token filename relative to ZIP archive
        FileTypes FileType;     //!< File type (file or directory)
    };

    typedef std::vector<AssetToken*> T_AssetTokenVector;
    typedef T_AssetTokenVector::iterator IT_AssetTokenVector;
    typedef T_AssetTokenVector::const_iterator CIT_AssetTokenVector;

    AAssetManager *m_assetMgr;

    T_AssetTokenVector m_fileList; //!< file list

    typedef stdext::hash_map<String, Int32, std::hash<String> > T_AssetFileMap;
    typedef T_AssetFileMap::iterator IT_AssetFileMap;
    typedef T_AssetFileMap::const_iterator CIT_AssetFileMap;

    T_AssetFileMap m_fileMap;  //!< File map (relative path, index)

    void findAllFiles();
};

} // namespace o3d

#endif // O3D_ANDROID

#endif // _O3D_ASSETANDROID_H
