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

#include "asset.h"

#ifdef O3D_ANDROID

struct AAssetManager;

namespace o3d {

/**
 * @brief Asset interface. Usefull to add a specific asset handling like zip support,
 * Android asset, or any other protocol.
 */
class O3D_API AssetAndroid
{
public:

    AssetAndroid();
    virtual ~AssetAndroid();

    /**
     * @brief Release all datas and close the asset.
     */
    virtual void destroy() override;

    virtual Int32 findFile(const String& fileName) const override;

    virtual InStream* openInStream(const String& filename) override;
    virtual InStream* openInStream(Int32 index) override;

    /**
     * @return Return "android".
     */
    virtual const String& location() const override;

    /**
     * @return Returns the empty name "".
     */
    virtual String name() const override;

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

protected:

    AAssetManager *m_assetMgr;
};

} // namespace o3d

#endif // O3D_ANDROID

#endif // _O3D_ASSETANDROID_H
