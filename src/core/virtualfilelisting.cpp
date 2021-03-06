/**
 * @file virtualfilelisting.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/virtualfilelisting.h"

#include "o3d/core/filemanager.h"
#include "o3d/core/localdir.h"
#include "o3d/core/localfile.h"

#include <wctype.h>

#ifdef O3D_WINDOWS
	#include <direct.h>
#else
	#include <sys/stat.h>
	#include <dirent.h>
#endif

using namespace o3d;

VirtualFileListing::VirtualFileListing() :
    FileListing()
{
	m_virtualCurPos = -1;
}

VirtualFileListing::~VirtualFileListing()
{
    for (O3D_IT_FLItemVector it = m_virtualFileList.begin() ; it != m_virtualFileList.end() ; ++it) {
		FLItem* flitem = (*it);
		deletePtr(flitem);
	}
}
#include "o3d/core/debug.h"
void VirtualFileListing::virtualFindFirstFile()
{
    // On a fini de lister les fichiers qui sont sur le systeme de fichier
    // On list maintenant tous les fichiers qui viennent des assets
    FLItem *fileItem = nullptr;
	String curFilename;
	String OldWorkingDir;
	String absPath;
	Bool first = True;

	// search in virtual files of the file manager
	m_virtualCurPos = -1;

	absPath = m_path;
	absPath.trimRight('*');

    FileManager *fileManager = FileManager::instance();
    fileManager->searchFirstVirtualFile(absPath);

	// On recupere le repertoire courant
	String tmpAbsPath = m_path;
	FileTypes fileType;

	// list virtual files
    do {
        curFilename = fileManager->searchNextVirtualFile(absPath, &fileType);

        if (curFilename.isEmpty()) {
			break;
        }

		// upcase the drivedir letter
        if (isAbsolutePath()) {
			absPath[0] = (WChar)towupper(absPath[0]);
			WChar c = (WChar)towupper(curFilename[0]);

			if (c == absPath[0])
				curFilename[0] = c;
		}

		// On test si le repertoire correspond
        if (curFilename.sub(absPath, 0) == 0) {
			// if virtual path add .. and .
            if ((tmpAbsPath != absPath) && first && (m_fileList.size() == 0)) {
				fileItem = new FLItem;
				fileItem->FileSize = 0;
				fileItem->FileType = FILE_DIR;
				fileItem->FileName = ".";
				m_virtualFileList.push_back(fileItem);

				fileItem = new FLItem;
				fileItem->FileSize = 0;
				fileItem->FileType = FILE_DIR;
				fileItem->FileName = "..";
				m_virtualFileList.push_back(fileItem);

				first = False;
			}

			// Check for a slash after the base path, it one we are to far from the
			// search directory
            if (curFilename.length() > absPath.length()) {
                if (curFilename.find('/', absPath.length() + 1) >= 0) {
					continue;
                }
            } else {
				continue;
            }

			// is a directory
            if (fileType == FILE_DIR) {
				// do we accept directory listing
                if (m_type != FILE_FILE) {
					Int32 pos;

					fileItem = new FLItem;
					fileItem->FileSize = 0;
					fileItem->FileType = FILE_DIR;

					// extract only the directory name
					curFilename.trimRight('/');

					pos = curFilename.reverseFind('/');
                    if (pos != -1) {
						curFilename = curFilename.sub(pos+1);
                    } //else {
					//	curFilename = curFilename;
                    //}

					fileItem->FileName = curFilename;
					m_virtualFileList.push_back(fileItem);

					continue;
				}
            } else if (m_type != FILE_DIR) {
                // do we accept file listing
				WChar extension[16];
				WChar *startPos;
				WChar *endPos;
				Int32 pos;

				// filter by extension
                if (!isValidExt("*")) {
					startPos = wcschr(curFilename.getData(), '.');
                    if (startPos) {
						startPos++;
						endPos = curFilename.getData() + wcslen(curFilename.getData());

						wcsncpy(extension, startPos, endPos-startPos);
						extension[endPos-startPos] = '\0';

                        if (!isValidExt(extension)) {
							continue;
                        }
                    } else if (!isValidExt("")) {
						continue;
                    }
				}

				// list file
				fileItem = new FLItem;
				fileItem->FileSize = 0;
				fileItem->FileType = FILE_FILE;

				// extract only the file name
				pos = curFilename.reverseFind('/');
                if (pos != -1) {
					curFilename = curFilename.sub(pos+1);
                }

				fileItem->FileName = curFilename;
				m_virtualFileList.push_back(fileItem);

				continue;
			}
		}
	} while (curFilename.isValid());
}

Int32 VirtualFileListing::getNumFile() const
{
	return (Int32)(m_fileList.size() + m_virtualFileList.size());
}

FLItem* VirtualFileListing::getFile() const
{
    if ((m_curPos >= 0) && (m_curPos < (Int32)m_fileList.size())) {
		return m_fileList[m_curPos];
    } else if ((m_virtualCurPos >= 0) && (m_virtualCurPos < (Int32)m_virtualFileList.size())) {
		return m_virtualFileList[m_virtualCurPos];
    }

    return nullptr;
}

String VirtualFileListing::getFileName() const
{
	String FileName;
	FLItem *pTempItem;

	pTempItem = getFile();
    if (pTempItem) {
		FileName = pTempItem->FileName;
    }

	return FileName;
}

// get the file type
FileTypes VirtualFileListing::getFileType() const
{
	FLItem *pTempItem;

	pTempItem = getFile();
    if (pTempItem) {
		return pTempItem->FileType;
    }

	return FILE_UNKNWON;
}

//UInt64 VirtualFileListing::getFileSize() const
//{
//	return 0;
//}

//const DateTime& VirtualFileListing::getCreationTime() const
//{
//    return DateTime::nullDate();
//}

//const DateTime& VirtualFileListing::getAccesTime() const
//{
//    return DateTime::nullDate();
//}

//const DateTime& VirtualFileListing::getWriteTime() const
//{
//    return DateTime::nullDate();
//}

// get the absolute directory of the file
String VirtualFileListing::getFilePath() const
{
	String pathOnly = m_path;
	FLItem *tempItem = getFile();

	pathOnly.trimRight('*');

    if (tempItem) {
        if (tempItem->FileType == FILE_FILE) {
            LocalDir localDir(pathOnly);
            LocalFile localFile(tempItem->FileName);
            localDir.makeAbsolute();
            return localDir.getFullPathName() + '/' + localFile.getFilePath();
        } else if (tempItem->FileType == FILE_DIR) {
            LocalDir localDir(pathOnly);
            localDir.makeAbsolute();
            return localDir.getFullPathName() + '/' + tempItem->FileName;
		}
	}

	return String();
}

String VirtualFileListing::getFileFullName() const
{
	String pathOnly = m_path;
	FLItem *tempItem = getFile();

	pathOnly.trimRight('*');

    if (tempItem) {
        if (tempItem->FileType == FILE_FILE) {
            LocalDir localDir(pathOnly);
            localDir.makeAbsolute();
            return localDir.getFullPathName() + '/' + tempItem->FileName;
        } else if (tempItem->FileType == FILE_DIR) {
            LocalDir localDir(pathOnly);
            localDir.makeAbsolute();
            return localDir.getFullPathName() + '/' + tempItem->FileName;
		}
	}

	return String();
}

void VirtualFileListing::searchFirstFile()
{
	FileListing::searchFirstFile();
	virtualFindFirstFile();
}

FLItem* VirtualFileListing::searchNextFile()
{
    if (m_curPos+1 < (Int32)m_fileList.size()) {
		m_curPos++;
		return m_fileList[m_curPos];
    } else {
        if (m_virtualCurPos+1 < (Int32)m_virtualFileList.size()) {
			m_virtualCurPos++;
			return m_virtualFileList[m_virtualCurPos];
		}
	}
    return nullptr;
}

void VirtualFileListing::setPath(const String &path)
{
    for (UInt32 i = 0 ; i < m_virtualFileList.size() ; ++i) {
		deletePtr(m_virtualFileList[i]);
    }

	m_virtualFileList.clear();
	FileListing::setPath(path);
}
