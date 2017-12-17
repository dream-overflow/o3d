/**
 * @file assetandroid.cpp
 * @brief
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-17
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/base.h"

#ifdef O3D_ANDROID

#include "o3d/core/private/assetandroid.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/datainstream.h"
#include "o3d/core/application.h"

#include "o3d/core/debug.h"

#include <android/asset_manager.h>
#include "android/android_native_app_glue.h"

using namespace o3d;

AssetAndroid::AssetAndroid()
{
    struct android_app* app = reinterpret_cast<struct android_app*>(Application::getApp());
    m_assetMgr = app->activity->assetManager;
}

AssetAndroid::~AssetAndroid()
{

}

void AssetAndroid::destroy()
{

}

Int32 AssetAndroid::findFile(const String &fileName) const
{
    AAssetDir* assetDir = AAssetManager_openDir(m_assetMgr, "media");



    AAssetDir_close(assetDir);
}

InStream *AssetAndroid::openInStream(const String &filename)
{
    struct android_app* app = reinterpret_cast<struct android_app*>(Application::getApp());
    AAssetManager *assetMgr = app->activity->assetManager;
    AAssetDir* assetDir = AAssetManager_openDir(assetMgr, "media");
    AAssetDir_close(assetDir);
}

InStream *AssetAndroid::openInStream(Int32 index)
{

}

const String &AssetAndroid::location() const
{

}

String AssetAndroid::name() const
{
    return "";
}

String AssetAndroid::protocol() const
{
    return "android://";
}

Int32 AssetAndroid::getNumFiles() const
{

}

String AssetAndroid::getFileName(Int32 index) const
{

}

FileTypes AssetAndroid::getFileType(Int32 index) const
{

}

#endif // O3D_ANDROID
