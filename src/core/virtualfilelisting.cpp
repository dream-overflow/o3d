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
#include "o3d/core/diskdir.h"
#include "o3d/core/diskfileinfo.h"

#include <wctype.h>

#ifdef O3D_WINDOWS
	#include <direct.h>
#else
	#include <sys/stat.h>
	#include <dirent.h>
#endif

using namespace o3d;

/*---------------------------------------------------------------------------------------
  constructor/destructor
---------------------------------------------------------------------------------------*/
VirtualFileListing::VirtualFileListing()
: FileListing()
{
	m_virtualCurPos = -1;
}

VirtualFileListing::~VirtualFileListing()
{
	for (O3D_IT_FLItemVector it = m_virtualFileList.begin() ; it != m_virtualFileList.end() ; ++it)
	{
		FLItem* flitem = (*it);
		deletePtr(flitem);
	}
}

/*---------------------------------------------------------------------------------------
  initialise the virtual file search
---------------------------------------------------------------------------------------*/
void VirtualFileListing::virtualFindFirstFile()
{
	// On a fini de lister les fichiers qui sont sur le disk
	// On list maintenant tous les fichiers qui viennent des fichiers packs
	FLItem *fileItem = NULL;
	String curFilename;
	String OldWorkingDir;
	String absPath;
	Bool first = True;

	// search in virtual files of the file manager
	m_virtualCurPos = -1;

	FileManager *fileManager = FileManager::instance();
	fileManager->searchFirstVirtualFile();

	absPath = m_path;
	absPath.trimRight('*');

	// On recupere le repertoire courant
	String tmpAbsPath = m_path;
	FileTypes fileType;

	// list virtual files
	do
	{
		curFilename = fileManager->searchNextVirtualFile(&fileType);

		if (curFilename.isEmpty())
			break;

		// upcase the drivedir letter
		if (isAbsolutePath())
		{
			absPath[0] = (WChar)towupper(absPath[0]);
			WChar c = (WChar)towupper(curFilename[0]);

			if (c == absPath[0])
				curFilename[0] = c;
		}

		// On test si le repertoire correspond
		if (curFilename.sub(absPath, 0) == 0)
		{
			// if virtual path add .. and .
			if ((tmpAbsPath != absPath) && first && (m_fileList.size() == 0))
			{
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
			if (curFilename.length() > absPath.length())
			{
				if (curFilename.find('/', absPath.length() + 1) >= 0)
					continue;
			}
			else
				continue;

			// is a directory
			if (fileType == FILE_DIR)
			{
				// do we accept directory listing
				if (m_type != FILE_FILE)
				{
					Int32 pos;

					fileItem = new FLItem;
					fileItem->FileSize = 0;
					fileItem->FileType = FILE_DIR;

					// extract only the directory name
					curFilename.trimRight('/');

					pos = curFilename.reverseFind('/');
					if (pos != -1)
						curFilename = curFilename.sub(pos+1);
					//else
					//	curFilename = curFilename;

					fileItem->FileName = curFilename;
					m_virtualFileList.push_back(fileItem);

					continue;
				}
			}
			// do we accept file listing
			else if (m_type != FILE_DIR)
			{
				WChar extension[16];
				WChar *startPos;
				WChar *endPos;
				Int32 pos;

				// filter by extension
				if (!isValidExt("*") )
				{
					startPos = wcschr(curFilename.getData(), '.');
					if (startPos)
					{
						startPos++;
						endPos = curFilename.getData() + wcslen(curFilename.getData());

						wcsncpy(extension, startPos, endPos-startPos);
						extension[endPos-startPos] = '\0';

						if (!isValidExt(extension))
							continue;
					}
					else if (!isValidExt(""))
						continue;
				}

				// list file
				fileItem = new FLItem;
				fileItem->FileSize = 0;
				fileItem->FileType = FILE_FILE;

				// extract only the file name
				pos = curFilename.reverseFind('/');
				if (pos != -1)
					curFilename = curFilename.sub(pos+1);

				fileItem->FileName = curFilename;
				m_virtualFileList.push_back(fileItem);

				continue;
			}
		}
	} while (curFilename.isValid());
}

/*---------------------------------------------------------------------------------------
  get num file
---------------------------------------------------------------------------------------*/
Int32 VirtualFileListing::getNumFile() const
{
	return (Int32)(m_fileList.size() + m_virtualFileList.size());
}

/*---------------------------------------------------------------------------------------
  get the file
---------------------------------------------------------------------------------------*/
FLItem* VirtualFileListing::getFile() const
{
	if ((m_curPos >= 0) && (m_curPos < (Int32)m_fileList.size()))
		return m_fileList[m_curPos];
	else
	if ((m_virtualCurPos >= 0) && (m_virtualCurPos < (Int32)m_virtualFileList.size()))
		return m_virtualFileList[m_virtualCurPos];

	return NULL;
}

/*---------------------------------------------------------------------------------------
  get only the filename
---------------------------------------------------------------------------------------*/
String VirtualFileListing::getFileName() const
{
	String FileName;
	FLItem *pTempItem;

	pTempItem = getFile();
	if (pTempItem)
		FileName = pTempItem->FileName;

	return FileName;
}

// get the file type
FileTypes VirtualFileListing::getFileType() const
{
	FLItem *pTempItem;

	pTempItem = getFile();
	if (pTempItem)
		return pTempItem->FileType;

	return FILE_UNKNWON;
}
/*
// get the file size
UInt64 VirtualFileListing::getFileSize() const
{
	return 0;
}

// get the creation file time
const DateTime& VirtualFileListing::getCreationTime() const
{
    return DateTime::nullDate();
}

// get the last access file time
const DateTime& VirtualFileListing::getAccesTime() const
{
    return DateTime::nullDate();
}

 //get the last write file time
const DateTime& VirtualFileListing::getWriteTime() const
{
    return DateTime::nullDate();
}*/

// get the absolute directory of the file
String VirtualFileListing::getFilePath() const
{
	String pathOnly = m_path;
	FLItem *tempItem = getFile();

	pathOnly.trimRight('*');

    if ( tempItem )
	{
		if (tempItem->FileType == FILE_FILE)
		{
			DiskDir diskDir(pathOnly);
			DiskFileInfo diskFile(tempItem->FileName);
			diskDir.makeAbsolute();
			return diskDir.getFullPathName() + '/' + diskFile.getFilePath();
		}
		else if (tempItem->FileType == FILE_DIR)
		{
			DiskDir diskDir(pathOnly);
			diskDir.makeAbsolute();
			return diskDir.getFullPathName() + '/' + tempItem->FileName;
		}
	}

	return String();
}

/*---------------------------------------------------------------------------------------
  get the complete file name
---------------------------------------------------------------------------------------*/
String VirtualFileListing::getFileFullName() const
{
	String pathOnly = m_path;
	FLItem *tempItem = getFile();

	pathOnly.trimRight('*');

	if (tempItem)
	{
		if (tempItem->FileType == FILE_FILE)
		{
			DiskDir diskDir(pathOnly);
			diskDir.makeAbsolute();
			return diskDir.getFullPathName() + '/' + tempItem->FileName;
		}
		else if (tempItem->FileType == FILE_DIR)
		{
			DiskDir diskDir(pathOnly);
			diskDir.makeAbsolute();
			return diskDir.getFullPathName() + '/' + tempItem->FileName;
		}
	}

	return String();
}

/*---------------------------------------------------------------------------------------
  Initialize the file search
---------------------------------------------------------------------------------------*/
void VirtualFileListing::searchFirstFile()
{
	FileListing::searchFirstFile();
	virtualFindFirstFile();
}

/*---------------------------------------------------------------------------------------
  return the next file (if NULL its finished)
---------------------------------------------------------------------------------------*/
FLItem* VirtualFileListing::searchNextFile()
{
	if (m_curPos+1 < (Int32)m_fileList.size())
	{
		m_curPos++;
		return m_fileList[m_curPos];
	}
	else
	{
		if (m_virtualCurPos+1 < (Int32)m_virtualFileList.size())
		{
			m_virtualCurPos++;
			return m_virtualFileList[m_virtualCurPos];
		}
	}
	return NULL;
}

void VirtualFileListing::setPath(const String &path)
{
	for (UInt32 i = 0 ; i < m_virtualFileList.size() ; ++i)
		deletePtr(m_virtualFileList[i]);

	m_virtualFileList.clear();
	FileListing::setPath(path);
}
