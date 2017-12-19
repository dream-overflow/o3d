/**
 * @file filemanager.cpp
 * @brief Global file system manager.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-09-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/zip.h"
#include "o3d/core/filelisting.h"
#include "o3d/core/thread.h"
#include "o3d/core/localfile.h"
#include "o3d/core/localdir.h"
#include "o3d/core/virtualdir.h"
#include "o3d/core/virtualfile.h"
#include "o3d/core/stringtokenizer.h"

#include "o3d/core/debug.h"

#include "o3d/core/fileinstream.h"
#include "o3d/core/fileoutstream.h"

#ifdef O3D_WINDOWS
	#include <direct.h>
#else
	#include <sys/stat.h>
	#include <dirent.h>
#endif

#ifdef O3D_ANDROID
    #include "o3d/core/private/assetandroid.h"
#endif

using namespace o3d;

Asset::~Asset()
{

}

FileManager* FileManager::m_instance = nullptr;
static FastMutex O3D_FileManagerMutex;

// Singleton instantiation
FileManager* FileManager::instance()
{
    if (!m_instance) {
		m_instance = new FileManager();
    }
	return m_instance;
}

// Singleton destruction
void FileManager::destroy()
{
	if (m_instance)	{
		delete m_instance;
        m_instance = nullptr;
	}
}

// default contructor
FileManager::FileManager() :
	m_curFilePos(0)
{
	m_instance = (FileManager*)this; // Used to avoid recursive call when the ctor call himself...

	// current directory as default
	m_defaultWorkingDir = getWorkingDirectory();
    m_packExt = "*.opk";

#ifdef O3D_ANDROID
    // add an android asset
    AssetAndroid *aa = new AssetAndroid();
    m_assets.push_back(aa);
#endif

    m_curAssetIt = m_assets.begin();
}

// destructor
FileManager::~FileManager()
{
    umountAllAssets();
}

// return the index of file pack end if not found
IT_AssetList FileManager::findAsset(const String &assetName)
{
    String lAssetName = getFullFileName(assetName);

	O3D_FileManagerMutex.lock();
    for (IT_AssetList it = m_assets.begin() ; it != m_assets.end(); ++it) {
        if ((*it)->fullPathName() == lAssetName) {
			O3D_FileManagerMutex.unlock();
			return it;
		}
	}
	O3D_FileManagerMutex.unlock();

    return m_assets.end();
}

// return true if Path is relative otherwise return false
Bool FileManager::isRelativePath(const String &path)
{
	// On Unix like it start with '/'.
	// On Windows, it start with 'C:\' or 'D:\' ... or '\\'
#ifdef O3D_WINDOWS
	if (((path.length() > 2) && (path[1] == ':') && ((path[2] == '/') || (path[2] == '\\'))) ||
        ((path.length() > 2) && (path[0] == '/') && (path[1] == '/')) ||
        ((path.length() > 2) && (path[0] == '\\') && (path[1] == '\\'))) {
		return False;
    }
#else
    if ((path.length() > 1) && ((path[0] == '/') || (path[0] == '\\'))) {
		return False;
    }
#endif
	return True;
}

void FileManager::adaptPath(String& path)
{
    Int32 pos,pathToDelPos;

    path.replace('\\','/');
    path.trimRight('/');

    // add a trailing slash to simplify our search
    path += '/';

    // delete the '.'
    while ((pos = path.sub("./",0)) > 1) {
        path.remove(pos,2);
    }

    // remove the last directory
    while ((pos = path.sub("../",0)) > 1) {
        pathToDelPos = path.find('/',pos-2,True);
        path.remove(pathToDelPos,pos+2-pathToDelPos);
    }

    // no trailing slash
    path.trimRight('/');
}

String FileManager::convertPath(const String &filename, const String &pathName)
{
    String result;
    StringTokenizer lPath(filename, L"/\\");
    StringTokenizer lPathCount(pathName, L"/\\");

    while (lPath.hasMoreTokens()) {
        String cur = lPath.nextToken();
        lPathCount.nextToken();

        if (pathName.sub(cur,0) == -1) {
            result += cur;

            if (lPath.hasMoreTokens()) {
                if (lPathCount.hasMoreTokens()) {
                    result.insert("../",0);
                }

                result += '/';
            }
        }
    }

    // no trailing slash
    result.trimRight('/');
    return result;
}

String FileManager::getFilePath(const String &fullFileName)
{
    Int32 p = fullFileName.reverseFind('/');
    if (p == -1) {
        return "";
    } else {
        String pathName = fullFileName;
        pathName.truncate(p);

        return pathName;
    }
}

void FileManager::getFileNameAndPath(
        const String &fullFileName,
        String &filename,
        String &pathname)
{
    Int32 p = fullFileName.reverseFind('/');
    if (p == -1) {
        pathname = "";
        filename = fullFileName;
    } else {
        pathname = fullFileName;
        pathname.truncate(p);
        filename = fullFileName.sub(p+1);
    }
}

String FileManager::getFileName(const String &fullFileName)
{
    Int32 p = fullFileName.reverseFind('/');
    if (p == -1) {
        return fullFileName;
    } else {
        return fullFileName.sub(p+1);
    }
}

String FileManager::getStrippedFileName(const String &fullFileName)
{
    String fileName, fileExt;
    getFileNameAndExt(fullFileName, fileName, fileExt);
    fileName.trimRight(fileExt);

    return fileName;
}

String FileManager::getFileExt(const String &fullFileName)
{
    Int32 p = fullFileName.reverseFind('.');
    if (p == -1) {
        return "";
    } else {
        return fullFileName.sub(p+1);
    }
}

void FileManager::getFileNameAndExt(
        const String &fullFileName,
        String &filename,
        String &ext)
{
    Int32 p = fullFileName.reverseFind('/');
    if (p == -1) {
        filename = fullFileName;
    } else {
        filename = fullFileName.sub(p+1);
    }

    ext = getFileExt(filename);
}

// define the current working path (absolute)
Bool FileManager::setWorkingDirectory()
{
	FastMutexLocker locker(O3D_FileManagerMutex);

#ifdef O3D_WINDOWS
	return (_wchdir(m_workingDir.getData()) == 0);
#else
	return (chdir(m_workingDir.toUtf8().getData()) == 0);
#endif
}

Bool FileManager::setWorkingDirectory(const String &workingDir)
{
	FastMutexLocker locker(O3D_FileManagerMutex);

	m_oldWorkingDir = m_workingDir;

	m_workingDir = workingDir;
	m_workingDir.replace('\\','/');
	m_workingDir.trimRight('/');

#ifdef O3D_WINDOWS
	return (_wchdir(m_workingDir.getData()) == 0);
#else
	return (chdir(m_workingDir.toUtf8().getData()) == 0);
#endif
}

// define the default path (root absolute path)
Bool FileManager::setDefaultWorkingDirectory()
{
	FastMutexLocker locker(O3D_FileManagerMutex);

	m_oldWorkingDir = m_workingDir;
	m_workingDir = m_defaultWorkingDir;

#ifdef O3D_WINDOWS
	return (_wchdir(m_defaultWorkingDir.getData()) == 0);
#else
	return (chdir(m_defaultWorkingDir.toUtf8().getData()) == 0);
#endif
}

// set the current path with a relative one
Bool FileManager::changeWorkingDirectory(const String &relativeDir)
{
	Bool result;

	O3D_FileManagerMutex.lock();

	String lRelativeDir(relativeDir);
	lRelativeDir.replace('\\','/');
	lRelativeDir.trimRight('/');

#ifdef O3D_WINDOWS
	result = (_wchdir(lRelativeDir.getData()) == 0);
#else
	result = (chdir(lRelativeDir.toUtf8().getData()) == 0);
#endif

	m_oldWorkingDir = m_workingDir;

	O3D_FileManagerMutex.unlock();

	getWorkingDirectory();
	return result;
}

// reset to the last working directory
Bool FileManager::restoreWorkingDirectory()
{
	FastMutexLocker locker(O3D_FileManagerMutex);

    if (m_oldWorkingDir.isValid()) {
		m_workingDir = m_oldWorkingDir;
		m_oldWorkingDir.destroy();

#ifdef O3D_WINDOWS
		return (_wchdir(m_workingDir.getData()) == 0);
#else
		return (chdir(m_workingDir.toUtf8().getData()) == 0);
#endif
	}
	return False;
}

// get the current working path
String FileManager::getWorkingDirectory()
{
	FastMutexLocker locker(O3D_FileManagerMutex);

#ifdef O3D_WINDOWS
	WChar WorkingDir[MAX_PATH];
	_wgetcwd(WorkingDir, MAX_PATH);
	m_workingDir = WorkingDir;
#else
	Char WorkingDir[MAX_PATH];
	getcwd(WorkingDir, MAX_PATH);
	m_workingDir.fromUtf8(WorkingDir);
#endif

	m_workingDir = WorkingDir;
	m_workingDir.replace('\\','/');

	// no trailing slash
	m_workingDir.trimRight('/');
	return m_workingDir;
}

// return a full path filename, such as openFile but it don't open it
String FileManager::getFullFileName(const String &filename) const
{
	String lFilename(filename);
	lFilename.replace('\\','/');

    if (isRelativePath(lFilename)) {
		O3D_FileManagerMutex.lock();
		lFilename = m_workingDir + '/' + lFilename;
		O3D_FileManagerMutex.unlock();
	}

    adaptPath(lFilename);
	return lFilename;
}

InStream *FileManager::openInStream(const String &filename)
{
    // On cherche si le fichier existe dans les fichiers packs
    String lfilename = getFullFileName(filename);
    InStream *lis = nullptr;

    // Try with file on assets
    {
        FastMutexLocker locker(O3D_FileManagerMutex);

        for (IT_AssetList it = m_assets.begin() ; it != m_assets.end(); ++it) {
            Int32 index;
            // search for the file
            if ((index = (*it)->findFile(lfilename)) != -1) {
                // If found, try to open it
                return (*it)->openInStream(index);
            }
        }
    }

    // Try with filesystem
    try {
        lis = new FileInStream(lfilename);
    } catch(const E_FileNotFoundOrInvalidRights &) {
        deletePtr(lis);
        throw;
    }

    return lis;
}

FileOutStream *FileManager::openOutStream(const String &filename, FileOutStream::Mode mode)
{
    // Always write on filesystem
    String lfilename = getFullFileName(filename);
    FileOutStream *los = nullptr;

    try {
        los = new FileOutStream(lfilename, mode);
    } catch(const E_BaseException &) {
        deletePtr(los);
        throw;
    }

    return los;
}

Bool FileManager::mountAsset(const String &protocol, const String &assetName)
{
    if (protocol == "zip://") {
        Zip *pNewZipFile;
        String lPackName = getFullFileName(assetName);

        O3D_FileManagerMutex.lock();
        // Is archive already mounted
        for (IT_AssetList it = m_assets.begin() ; it != m_assets.end(); ++it) {
            if ((*it)->protocol() != "zip://") {
                continue;
            }

            if ((*it)->fullPathName() == lPackName) {
                O3D_FileManagerMutex.unlock();
                return False;
            }
        }
        O3D_FileManagerMutex.unlock();

        // open the pack file
        InStream *is = openInStream(lPackName);

        try {
            String fname, fpath;
            getFileNameAndPath(lPackName, fname, fpath);
            pNewZipFile = new Zip(*is, fname, fpath);
        } catch(E_BaseException &) {
            deletePtr(is);
            throw;
        }

        // insert it
        O3D_FileManagerMutex.lock();
        m_assets.push_back(pNewZipFile);
        O3D_FileManagerMutex.unlock();

        return True;
    } else if (protocol == "android://") {
    #ifdef O3D_ANDROID
        AssetAndroid *asset;
        String lAssetName = getFullFileName(assetName);

        O3D_FileManagerMutex.lock();
        // Is archive already mounted
        for (IT_AssetList it = m_assets.begin() ; it != m_assets.end(); ++it) {
            // a single android asset
            if ((*it)->protocol() == "android://") {
                O3D_FileManagerMutex.unlock();
                return False;
            }
        }
        O3D_FileManagerMutex.unlock();

        asset = new AssetAndroid();

        // insert it
        O3D_FileManagerMutex.lock();
        m_assets.push_back(asset);
        O3D_FileManagerMutex.unlock();

        return True;
    #else
        return False;
    #endif
    } else {
        return False;
    }
}

// add all the packs files if not presents in and return the number of added packs
Int32 FileManager::mountAllArchives()
{
    // Add from working dir any packages files
	Int32 Num = 0;
	FileListing fileListing;

	fileListing.setExt(m_packExt);
	fileListing.setPath(m_workingDir);
	fileListing.setType(FILE_FILE);

	fileListing.searchFirstFile();

    while (fileListing.searchNextFile()) {
        if (mountAsset("zip://", fileListing.getFile()->FileName)) {
			Num++;
        }
	}
	return Num;
}

// remove a file pack and return true if it have already deleted
Bool FileManager::umountAsset(const String &assetName)
{
    String lAssetName = getFullFileName(assetName);

	O3D_FileManagerMutex.lock();
    for (IT_AssetList it = m_assets.begin() ; it != m_assets.end(); ++it) {
        if ((*it)->fullPathName() == lAssetName) {
            O3D_FileManagerMutex.unlock();

			deletePtr(*it);
            m_assets.erase(it);
			return True;
		}
	}
	O3D_FileManagerMutex.unlock();

    O3D_ERROR(E_InvalidParameter(lAssetName));
	return False;
}

// remove all the files packs and the number of removed packs
Int32 FileManager::umountAllAssets()
{
	FastMutexLocker locker(O3D_FileManagerMutex);

    Int32 ret = (Int32)m_assets.size();

    for (IT_AssetList it = m_assets.begin() ; it != m_assets.end(); ++it) {
		deletePtr(*it);
	}

    m_assets.clear();

	return ret;
}

// initialize the file search
void FileManager::searchFirstVirtualFile()
{
	FastMutexLocker locker(O3D_FileManagerMutex);

    m_curFilePos = 0;
    m_curAssetIt = m_assets.begin();
}

// return the next file name (empty string if finished)
String FileManager::searchNextVirtualFile(FileTypes *fileType)
{
	FastMutexLocker locker(O3D_FileManagerMutex);
	String result;

    // @todo improve because of android asset

    while (m_curAssetIt != m_assets.end()) {
        if (m_curFilePos < (*m_curAssetIt)->getNumFiles()) {
            result = (*m_curAssetIt)->getFileName(m_curFilePos);

            if (fileType) {
                *fileType = (*m_curAssetIt)->getFileType(m_curFilePos);
            }

			++m_curFilePos;

			return result;
        } else {
			// this case when a package contain no files
			m_curFilePos = 0;
            ++m_curAssetIt;
		}
	}

    return result;
}

Bool FileManager::isPath(const String &path) const
{
    String lpath = getFullFileName(path);

    // Lookup file on assets
    {
        FastMutexLocker locker(O3D_FileManagerMutex);

        for (CIT_AssetList cit = m_assets.cbegin() ; cit != m_assets.cend(); ++cit) {
            // @todo could optimize comparing the base of the path

            // search for the path
            if ((*cit)->isPath(lpath)) {
                return True;
            }
        }
    }

    // Look on filesystem
    LocalDir dir(lpath);
    return dir.exists();
}

Bool FileManager::isFile(const String &fileName) const
{
    String lfilename = getFullFileName(fileName);

    // Lookup file on assets
    {
        FastMutexLocker locker(O3D_FileManagerMutex);

        for (CIT_AssetList cit = m_assets.cbegin() ; cit != m_assets.cend(); ++cit) {
            Int32 index;
            // search for the file
            if ((index = (*cit)->findFile(lfilename)) != -1) {
                return True;
            }
        }
    }

    // Look on filesystem
    LocalFile fileInfo(lfilename);
    return fileInfo.exists();
}

UInt64 FileManager::fileSize(const String &fileName) const
{
    String lfilename = getFullFileName(fileName);

    // Lookup file on assets
    {
        FastMutexLocker locker(O3D_FileManagerMutex);

        for (CIT_AssetList cit = m_assets.cbegin() ; cit != m_assets.cend(); ++cit) {
            Int32 index;
            // search for the file
            if ((index = (*cit)->findFile(lfilename)) != -1) {
                return (*cit)->getFileSize(index);
            }
        }
    }

    // Look on filesystem
    LocalFile fileInfo(lfilename);
    return fileInfo.exists();
}

FileTypes FileManager::fileType(const String &fileName) const
{
    String lfilename = getFullFileName(fileName);

    // Lookup file on assets
    {
        FastMutexLocker locker(O3D_FileManagerMutex);

        for (CIT_AssetList cit = m_assets.cbegin() ; cit != m_assets.cend(); ++cit) {
            Int32 index;
            // search for the file
            if ((index = (*cit)->findFile(lfilename)) != -1) {
                return (*cit)->getFileType(index);
            }
        }
    }

    // Look on filesystem
    LocalFile fileInfo(lfilename);
    return fileInfo.getType();
}

BaseFile* FileManager::file(const String &fileName) const
{
    String lfilename = getFullFileName(fileName);

    // Lookup file on assets
    {
        FastMutexLocker locker(O3D_FileManagerMutex);

        // @todo optimize lookup throught asset using part of theirs path
        // if the base path exists into a map... every where in this manager

        for (CIT_AssetList cit = m_assets.cbegin() ; cit != m_assets.cend(); ++cit) {
            Int32 index;
            // search for the file
            if ((index = (*cit)->findFile(lfilename)) != -1) {
                return new VirtualFile(fileName);
            }
        }
    }

    // Look on filesystem
    LocalFile *fileInfo = new LocalFile(lfilename);
    if (fileInfo->exists()) {
        return fileInfo;
    } else {
        delete fileInfo;
    }

    return nullptr;
}

BaseDir* FileManager::dir(const String &path) const
{
    String lpath = getFullFileName(path);

    // Lookup file on assets
    {
        FastMutexLocker locker(O3D_FileManagerMutex);

        for (CIT_AssetList cit = m_assets.cbegin() ; cit != m_assets.cend(); ++cit) {
            // @todo could optimize comparing the base of the path

            // search for the path
            if ((*cit)->isPath(lpath)) {
                return new VirtualDir(path);
            }
        }
    }

    // Look on filesystem
    LocalDir *dir = new LocalDir(lpath);
    if (dir->exists()) {
        return dir;
    } else {
        delete dir;
    }

    return nullptr;
}

// get a mounted archive file
Asset *FileManager::getAsset(const String &assetName)
{
    String lAssetName = getFullFileName(assetName);

	O3D_FileManagerMutex.lock();
    for (IT_AssetList it = m_assets.begin() ; it != m_assets.end(); ++it) {
        if ((*it)->fullPathName() == lAssetName) {
            O3D_FileManagerMutex.unlock();
			return *it;
		}
	}
	O3D_FileManagerMutex.unlock();

    O3D_ERROR(E_InvalidParameter(assetName));
    return nullptr;
}

// set the speed in delay and chunk-size
void FileManager::setSpeedManager(FileSpeedManager type, UInt32 delay,UInt32 blocksize)
{
	FastMutexLocker locker(O3D_FileManagerMutex);

	O3D_ASSERT(type < NUM_FILE_SPEED_MANAGER);

    m_flowCtrl[type].m_delay = delay;
    m_flowCtrl[type].m_size = blocksize;
    m_flowCtrl[type].m_bytesec = (UInt32)(blocksize * (1000.f / (Float)delay));
}
