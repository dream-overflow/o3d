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
#include "o3d/core/private/instreamandroid.h"
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

    findAllFiles();
}

AssetAndroid::~AssetAndroid()
{
    destroy();
}

void AssetAndroid::destroy()
{
    for (IT_AssetTokenVector it = m_fileList.begin(); it != m_fileList.end(); ++it) {
        deletePtr(*it);
    }

    m_fileList.clear();
}

Int32 AssetAndroid::findFile(const String &fileName)
{
    if (!fileName.startsWith("/")) {
        return -1;
    }

    auto it = m_fileMap.find(fileName);
    if (it != m_fileMap.end()) {
        return it->second;
    } else {
        // not cached
        String shortFileName;
        String filePath;

        FileManager::instance()->getFileNameAndPath(fileName, shortFileName, filePath);
        filePath.trimLeft('/');

        AAssetDir* assetDir = AAssetManager_openDir(m_assetMgr, filePath.toUtf8().getData());
        Int32 index = -1;

        if (assetDir) {
            const char* filename;

            // only for the first level, listing is only possible on files...
            while ((filename = AAssetDir_getNextFileName(assetDir)) != nullptr) {

                if (shortFileName == filename) {
                    AssetToken *entry = new AssetToken;
                    entry->FileName = filePath + '/' + filename;
                    entry->FileType = FILE_FILE;

                    m_fileList.push_back(entry);

                    // to file map
                    index = (Int32)(m_fileList.size() - 1);
                    m_fileMap[fileName] = index;

                    break;
                }
            }

            AAssetDir_close(assetDir);
            return index;
        } else {
            return -1;
        }
    }
}

Bool AssetAndroid::isPath(const String &path) const
{
    if (!m_assetMgr) {
        return False;
    }

    if (path.isEmpty()) {
        return True;
    }

    if (!path.startsWith("/")) {
        return False;
    }

    String lPath(path);
    lPath.trimLeft('/');
    lPath.trimRight('/');

    AAssetDir* assetDir = AAssetManager_openDir(m_assetMgr, path.toUtf8().getData());
    if (assetDir) {
        AAssetDir_close(assetDir);
        return True;
    } else {
        return False;
    }
}

InStream *AssetAndroid::openInStream(const String &fileName)
{
    if (!fileName.startsWith("/")) {
        O3D_ERROR(E_FileNotFoundOrInvalidRights("", fileName));
    }

    String lFileName(fileName);
    lFileName.trimLeft('/');

    AAsset *asset = AAssetManager_open(m_assetMgr, fileName.toUtf8().getData(), AASSET_MODE_STREAMING);
    if (asset) {
        String lFileName(fileName);
        return new InStreamAndroid(lFileName, asset);
    } else {
        O3D_ERROR(E_FileNotFoundOrInvalidRights("", fileName));
    }
}

InStream *AssetAndroid::openInStream(Int32 index)
{
    AssetToken *entry;

    if ((index < 0) || (index >= (Int32)m_fileList.size())) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }

    entry = m_fileList[index];

    if (entry) {
        AAsset *asset = AAssetManager_open(m_assetMgr, entry->FileName.toUtf8().getData(), AASSET_MODE_STREAMING);
        if (asset) {
            return new InStreamAndroid(entry->FileName, asset);
        } else {
            O3D_ERROR(E_FileNotFoundOrInvalidRights("", entry->FileName));
        }
    } else {
        O3D_ERROR(E_IndexOutOfRange(""));
    }
}

String AssetAndroid::location() const
{
    return "";
}

String AssetAndroid::name() const
{
    return "";
}

String AssetAndroid::fullPathName() const
{
    return "";
}

String AssetAndroid::protocol() const
{
    return "android://";
}

Int32 AssetAndroid::getNumFiles() const
{
    return (Int32)m_fileList.size();
}

String AssetAndroid::getFileName(Int32 index) const
{
    if ((index >= 0) && (index < (Int32)m_fileList.size())) {
        return String("/") + m_fileList[index]->FileName;
    } else {
        O3D_ERROR(E_IndexOutOfRange(""));
    }
}

FileTypes AssetAndroid::getFileType(Int32 index) const
{
    if ((index >= 0) && (index < (Int32)m_fileList.size())) {
        return m_fileList[index]->FileType;
    } else {
        O3D_ERROR(E_IndexOutOfRange(""));
    }
}

UInt64 AssetAndroid::getFileSize(Int32 index) const
{
    AssetToken *entry;

    if ((index < 0) || (index >= (Int32)m_fileList.size())) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }

    entry = m_fileList[index];

    if (entry) {
        AAsset *asset = AAssetManager_open(m_assetMgr, entry->FileName.toUtf8().getData(), AASSET_MODE_STREAMING);
        if (asset) {
            UInt64 size = AAsset_getLength(asset);
            AAsset_close(asset);

            return size;
        }
    }

    O3D_ERROR(E_FileNotFoundOrInvalidRights("", entry->FileName));
}

void AssetAndroid::findAllFiles()
{
    AAssetDir* assetDir = AAssetManager_openDir(m_assetMgr, "");
    if (!assetDir) {
        return;
    }

    const char* filename;

    // only for the first level, listing is only possible on files...
    while ((filename = AAssetDir_getNextFileName(assetDir)) != nullptr) {
        AssetToken *entry = new AssetToken;
        entry->FileName.fromUtf8(filename);
        entry->FileType = FILE_FILE;

        m_fileList.push_back(entry);

        // to file map
        m_fileMap[String("/") + entry->FileName] = (Int32)(m_fileList.size() - 1);
    }

    AAssetDir_close(assetDir);
}

void AssetAndroid::searchFirstFile(const String &path)
{
    String lPath(path);
    lPath.trimLeft('/');
    lPath.trimRight('/');
    AAssetDir* assetDir = AAssetManager_openDir(m_assetMgr, lPath.toUtf8().getData());
    if (!assetDir) {
        return;
    }

    const char* filename;

    // only for the first level, listing is only possible on files...
    while ((filename = AAssetDir_getNextFileName(assetDir)) != nullptr) {
        AssetToken *entry = new AssetToken;
        entry->FileName = (lPath.isValid() ? (lPath + '/' + filename) : filename);
        entry->FileType = FILE_FILE;

        // add only if not already listed
        if (m_fileMap.find(String("/") + entry->FileName) == m_fileMap.end()) {
            m_fileList.push_back(entry);

            // to file map
            m_fileMap[String("/") + entry->FileName] = (Int32)(m_fileList.size() - 1);
        }
    }

    AAssetDir_close(assetDir);
}

#endif // O3D_ANDROID
