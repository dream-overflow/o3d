/**
 * @file filelisting.cpp
 * @brief Directory files listing.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-09-05
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include "o3d/core/file.h"
#include "o3d/core/filelisting.h"
#include "o3d/core/architecture.h"
#include "o3d/core/diskfileinfo.h"
#include "o3d/core/diskdir.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/objects.h"
#include "o3d/core/debug.h"

#ifdef O3D_WIN32_SYS
	#include <direct.h>
#else
	#include <sys/stat.h>
	#include <dirent.h>
#endif

using namespace o3d;

/*---------------------------------------------------------------------------------------
  constructor/destructor
---------------------------------------------------------------------------------------*/
FileListing::FileListing()
{
	m_curPos = -1;
	m_type = FILE_BOTH;

	setExt("*.*");
}

FileListing::~FileListing()
{
	for (O3D_IT_FLItemVector it = m_fileList.begin() ; it != m_fileList.end() ; ++it)
	{
		FLItem* flitem = (*it);
		deletePtr(flitem);
	}
}

/*---------------------------------------------------------------------------------------
  get num file
---------------------------------------------------------------------------------------*/
Int32 FileListing::getNumFile() const
{
	return (Int32)m_fileList.size();
}

/*---------------------------------------------------------------------------------------
  get the file
---------------------------------------------------------------------------------------*/
FLItem* FileListing::getFile() const
{
	if ((m_curPos >= 0) && (m_curPos < (Int32)m_fileList.size()))
		return m_fileList[m_curPos];

	return NULL;
}

/*---------------------------------------------------------------------------------------
  get only the filename
---------------------------------------------------------------------------------------*/
String FileListing::getFileName() const
{
	String FileName;

	if ((m_curPos >= 0) && (m_curPos < (Int32)m_fileList.size()))
		FileName = m_fileList[m_curPos]->FileName;

	return FileName;
}

/*---------------------------------------------------------------------------------------
  get the file type
---------------------------------------------------------------------------------------*/
FileTypes FileListing::getFileType() const
{
	if ((m_curPos >= 0) && (m_curPos < (Int32)m_fileList.size()))
		return m_fileList[m_curPos]->FileType;

	return FILE_UNKNWON;
}

/*---------------------------------------------------------------------------------------
  get the filesize
---------------------------------------------------------------------------------------*/
UInt64 FileListing::getFileSize() const
{
	if ((m_curPos >= 0) && (m_curPos < (Int32)m_fileList.size()))
		return m_fileList[m_curPos]->FileSize;

	return 0;
}

/*---------------------------------------------------------------------------------------
  get the creation file time
---------------------------------------------------------------------------------------*/
const Date& FileListing::getCreationTime() const
{
	if ((m_curPos >= 0) && (m_curPos < (Int32)m_fileList.size()))
		return m_fileList[m_curPos]->CreateTime;

	return Date::nullDate();
}

/*---------------------------------------------------------------------------------------
  get the last access file time
---------------------------------------------------------------------------------------*/
const Date& FileListing::getAccesTime() const
{
	if ((m_curPos >= 0) && (m_curPos < (Int32)m_fileList.size()))
		return m_fileList[m_curPos]->AccessTime;

	return Date::nullDate();
}

/*---------------------------------------------------------------------------------------
  get the last write file time
---------------------------------------------------------------------------------------*/
const Date& FileListing::getWriteTime() const
{
	if ((m_curPos >= 0) && (m_curPos < (Int32)m_fileList.size()))
		return m_fileList[m_curPos]->WriteTime;

	return Date::nullDate();
}

/*---------------------------------------------------------------------------------------
  return true if this extension is in the extlist
---------------------------------------------------------------------------------------*/
Bool FileListing::isValidExt(const String &ext) const
{
	for (CIT_StringList it = m_extFilter.begin() ; it != m_extFilter.end() ; ++it)
	{
		if (ext == (*it))
			return True;
	}
	return False;
}

/*---------------------------------------------------------------------------------------
  return true if the relative or absolute path is a device repertory
---------------------------------------------------------------------------------------*/
Bool FileListing::isDriveDir() const
{
	// On va parcourir toute la string en descendant l'arboresence
	// On teste à chaque niveau si on a un dossier root
	// Si un dossier root est détecté deux fois de suite c'est que
	// L'on veut lister pour windows les devices

#ifdef O3D_WINDOWS
	if (isAbsolutePath())
	{
		// Chemin absolu ne peut être que du type C:/../* ou bien C:/temp/../../*
		// On va donc compter de combien d'etages on monte et de combien on descend
		String temp = m_path;
		temp.replace('\\','/');
		temp.trimRight("/*");

		Int32 Level = 0;

		Int32 pos = temp.find('/');

		while (pos != -1)
		{
			if ( (temp[pos+1] == '.') && (temp[pos+2] == '.') )
				Level--;
			else
				Level++;
			pos = temp.find('/',pos+1);
		}

		if (Level < 0)
			return True;
	}
	else
	{
		// On compte de combien on monte et on descend puis on compare
		// à l'etage du dossier courant
		WChar Buffer[MAX_PATH];
		WChar *pFile;
		Int32 Level = 0;
		String temp = m_path;

		temp.replace('\\','/');
		temp.trimRight("/*");

		Int32 pos = temp.find('/');

		while (pos != -1)
		{
			if ( (temp[pos+1] == '.') && (temp[pos+2] == '.') )
				Level--;
			else
				Level++;
			pos = temp.find('/',pos+1);
		}

		// On compte le nombre d'etages du chemin absolue
		GetFullPathNameW(m_path.getData(), MAX_PATH, Buffer, &pFile);
		*(pFile-1) = '\0';
		temp = Buffer;

		pos = temp.find('\\');
		Level++;
		while (pos != -1)
		{
			Level++;
			pos = temp.find('\\',pos+1);
		}

		if (Level < -1)
			return True;
	}
	return False;

#else
	//Sous UNIX, cette fonction est inutile...
	return True;
#endif
}

/*---------------------------------------------------------------------------------------
  return true if it's an absolute path
---------------------------------------------------------------------------------------*/
Bool FileListing::isAbsolutePath() const
{
	if ((m_path.length() > 0) && (m_path[0] == '/'))
		return True;

	if (m_path.length() < 2)
		return False;

	if ((m_path[1] == ':') && ((m_path[2] == '\\') || (m_path[2] == '/')))
		return True;

	return False;
}

// return the path of the listed directory
String FileListing::getPath() const
{
	String result(m_path);

#ifdef O3D_WINDOWS
	result.trimRight("/*");
#else
	result.trimRight('/');
#endif

	return result;
}

/*---------------------------------------------------------------------------------------
  define working path (absolute or relative)
---------------------------------------------------------------------------------------*/
void FileListing::setPath(const String &path)
{
	for (UInt32 i = 0 ; i < m_fileList.size() ; ++i)
		deletePtr(m_fileList[i]);

	m_fileList.clear();

	if (path.isEmpty())
	{
	#ifdef O3D_WINDOWS
		m_path = FileManager::instance()->getWorkingDirectory() + "/*";
	#else
		m_path = FileManager::instance()->getWorkingDirectory() + '/';
	#endif
	}
	else
	{
		m_path = path;
		m_path.replace('\\','/');
		m_path.trimRight('/');

		// make the path absolute if necessary, using the current working directory
		DiskDir diskDir(m_path);
		if (!diskDir.isAbsolute())
		{
			m_path = FileManager::instance()->getWorkingDirectory() + '/' + m_path;
            File::adaptPath(m_path);
		}

	#ifdef O3D_WINDOWS
		m_path += "/*";
	#else
		m_path += '/';
	#endif
	}
}

/*---------------------------------------------------------------------------------------
  define the extenstion filter ex: ("*.ini|*.zip|*.dat|*.pak|")
---------------------------------------------------------------------------------------*/
void FileListing::setExt(const String &extFilter)
{
	// On cherche les | et on extrait les extension que l'on range dans la liste
	UInt32 len = extFilter.length();

	if (len)
	{
		m_extFilter.clear();

		Int32 pos = 0;

		while (((pos = extFilter.find('*', pos)) != -1) && ((UInt32)(pos+2) < len))
		{
			pos += 2;

			String found_ext = extFilter.sub(pos);
			Int32 int_pos = found_ext.find('|');

			if (int_pos != -1)
				found_ext.truncate(int_pos);

			m_extFilter.push_back(found_ext);
		}
	}
}

/*---------------------------------------------------------------------------------------
  get the absolute directory of the file
---------------------------------------------------------------------------------------*/
String FileListing::getFilePath() const
{
	// the trailing slash is contained in pathOnly
	String pathOnly = m_path;
	pathOnly.trimRight('*');

	if ((m_curPos >= 0) && (m_curPos < (Int32)m_fileList.size()))
	{
		if (m_fileList[m_curPos]->FileType == FILE_FILE)
		{
			DiskFileInfo diskFile(pathOnly + m_fileList[m_curPos]->FileName);
			diskFile.makeAbsolute();
			return diskFile.getFilePath();
		}
		else if (m_fileList[m_curPos]->FileType == FILE_DIR)
		{
			DiskDir diskDir(pathOnly + m_fileList[m_curPos]->FileName);
			diskDir.makeAbsolute();
			return diskDir.getFullPathName();
		}
	}

	return String();
}

/*---------------------------------------------------------------------------------------
  get the complete file name
---------------------------------------------------------------------------------------*/
String FileListing::getFileFullName() const
{
	// the trailing slash is contained in pathOnly
	String pathOnly = m_path;
	pathOnly.trimRight('*');

	if ((m_curPos >= 0) && (m_curPos < (Int32)m_fileList.size()))
	{
		if (m_fileList[m_curPos]->FileType == FILE_FILE)
		{
			DiskFileInfo diskFile(pathOnly + m_fileList[m_curPos]->FileName);
			diskFile.makeAbsolute();
			return diskFile.getFullFileName();
		}
		else if (m_fileList[m_curPos]->FileType == FILE_DIR)
		{
			DiskDir diskDir(pathOnly + m_fileList[m_curPos]->FileName);
			diskDir.makeAbsolute();
			return diskDir.getFullPathName();
		}
	}

	return String();
}

// initialize the file search
#ifdef O3D_WIN32_SYS

void FileListing::searchFirstFile()   // Version windows du file listing
{
	WIN32_FIND_DATAW wData;
	SYSTEMTIME wTime;
	HANDLE listing;
	FLItem *Item;
	WChar extension[16];
	WChar *StartPos;
	WChar *EndPos;

	m_curPos = -1;

	if (m_path.isEmpty())
		O3D_ERROR(E_InvalidPrecondition("Path must be defined"));

	// On test si le repertoire à parcourir est avant le répertoire root ou non
	if (isDriveDir())
	{
		// On construit la liste de lecteurs
		UInt32 Drives;
		Int32 i;
		Drives = GetLogicalDrives();

		// Pour chaque bit
		for (i = 0 ; i < 32 ; i++)
		{
			if (Drives&0x00000001)
			{
				// Ce lecteur existe
				Item = new FLItem;
				wsprintfW(Item->FileName.getData(), L"%c:\\", 'A'+i);

				UInt32 Type = GetDriveTypeW(Item->FileName.getData());

				switch(Type)
				{
				case DRIVE_UNKNOWN:
					Item->FileType = FILE_UNKNWON;
					break;
				case DRIVE_NO_ROOT_DIR:
					Item->FileType = FILE_UNKNWON;
					break;
				case DRIVE_REMOVABLE:
					Item->FileType = FILE_REMOVABLE;
					break;
				case DRIVE_FIXED:
					Item->FileType = FILE_FIXED;
					break;
				case DRIVE_REMOTE:
					Item->FileType = FILE_REMOTE;
					break;
				case DRIVE_CDROM:
					Item->FileType = FILE_CDROM;
					break;
				case DRIVE_RAMDISK:
					Item->FileType = FILE_RAMDISK;
					break;
				default:
					Item->FileType = FILE_UNKNWON;
				}
				m_fileList.push_back(Item);
			}
			Drives = Drives >> 1;
		}
	}
	else
	{
		// On construit la liste de fichiers
		listing = ::FindFirstFileW(m_path.getData(), &wData);

		// On ajoute le premier fichier
		if ((listing != INVALID_HANDLE_VALUE) && !((wData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) && (m_type == FILE_FILE)))
		{
			Item = new FLItem;
			Item->FileName = wData.cFileName;

			// Time info
			FileTimeToSystemTime(&wData.ftLastAccessTime,&wTime);
			Item->AccessTime.day = Day(wTime.wDay);
			Item->AccessTime.mday = UInt8(wTime.wDayOfWeek);
			Item->AccessTime.hour = UInt8(wTime.wHour);
			Item->AccessTime.millisecond = UInt16(wTime.wMilliseconds);
			Item->AccessTime.minute = UInt8(wTime.wMinute);
			Item->AccessTime.month = Month(wTime.wMonth - 1);
			Item->AccessTime.second = UInt8(wTime.wSecond);
			Item->AccessTime.year = UInt16(wTime.wYear);

			FileTimeToSystemTime(&wData.ftCreationTime,&wTime);
			Item->CreateTime.day = Day(wTime.wDay);
			Item->CreateTime.mday = UInt8(wTime.wDayOfWeek);
			Item->CreateTime.hour = UInt8(wTime.wHour);
			Item->CreateTime.millisecond = UInt16(wTime.wMilliseconds);
			Item->CreateTime.minute = UInt8(wTime.wMinute);
			Item->CreateTime.month = Month(wTime.wMonth - 1);
			Item->CreateTime.second = UInt8(wTime.wSecond);
			Item->CreateTime.year = UInt16(wTime.wYear);

			FileTimeToSystemTime(&wData.ftLastWriteTime,&wTime);
			Item->WriteTime.day = Day(wTime.wDay);
			Item->WriteTime.mday = UInt8(wTime.wDayOfWeek);
			Item->WriteTime.hour = UInt8(wTime.wHour);
			Item->WriteTime.millisecond = UInt16(wTime.wMilliseconds);
			Item->WriteTime.minute = UInt8(wTime.wMinute);
			Item->WriteTime.month = Month(wTime.wMonth - 1);
			Item->WriteTime.second = UInt8(wTime.wSecond);
			Item->WriteTime.year = UInt16(wTime.wYear);

			// Taille du fichier
			Item->FileSize = (wData.nFileSizeHigh * (UInt64(O3D_MAX_UINT32)+1)) + wData.nFileSizeLow;

			// Type de fichier
			Item->FileType = FILE_DIR;

			m_fileList.push_back(Item);
		}
		else
		{
			//return;
		}

		while(::FindNextFileW(listing, &wData))
		{
			// On test le type
			if ((wData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) && (m_type == FILE_FILE))
				continue;

			if ((!(wData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) && (m_type == FILE_DIR))
				continue;

			// On test les extension
			if (!isValidExt("*") && !(wData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
			{
				StartPos = wcschr(wData.cFileName, '.');
				if (StartPos)
				{
					StartPos++;
					EndPos = wData.cFileName + wcslen(wData.cFileName);

					wcsncpy(extension, StartPos, EndPos-StartPos);
					extension[EndPos-StartPos] = '\0';

					if (!isValidExt(extension))
						continue;
				}
				else if (!isValidExt(""))
					continue;
			}

			// Ce ficher a passé tous les testes, il est valide
			Item = new FLItem;
			Item->FileName = wData.cFileName;

			// Type de fichier
			if (wData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
				Item->FileType = FILE_DIR;
			else
				Item->FileType = FILE_FILE;

			// Time info
			FileTimeToSystemTime(&wData.ftLastAccessTime,&wTime);
			Item->AccessTime.day = Day(wTime.wDay);
			Item->AccessTime.mday = UInt8(wTime.wDayOfWeek);
			Item->AccessTime.hour = UInt8(wTime.wHour);
			Item->AccessTime.millisecond = UInt16(wTime.wMilliseconds);
			Item->AccessTime.minute = UInt8(wTime.wMinute);
			Item->AccessTime.month = Month(wTime.wMonth - 1);
			Item->AccessTime.second = UInt8(wTime.wSecond);
			Item->AccessTime.year = UInt16(wTime.wYear);

			FileTimeToSystemTime(&wData.ftCreationTime,&wTime);
			Item->CreateTime.day = Day(wTime.wDay);
			Item->CreateTime.mday = UInt8(wTime.wDayOfWeek);
			Item->CreateTime.hour = UInt8(wTime.wHour);
			Item->CreateTime.millisecond = UInt16(wTime.wMilliseconds);
			Item->CreateTime.minute = UInt8(wTime.wMinute);
			Item->CreateTime.month = Month(wTime.wMonth - 1);
			Item->CreateTime.second = UInt8(wTime.wSecond);
			Item->CreateTime.year = UInt16(wTime.wYear);

			FileTimeToSystemTime(&wData.ftLastWriteTime,&wTime);
			Item->WriteTime.day = Day(wTime.wDay);
			Item->WriteTime.mday = UInt8(wTime.wDayOfWeek);
			Item->WriteTime.hour = UInt8(wTime.wHour);
			Item->WriteTime.millisecond = UInt16(wTime.wMilliseconds);
			Item->WriteTime.minute = UInt8(wTime.wMinute);
			Item->WriteTime.month = Month(wTime.wMonth - 1);
			Item->WriteTime.second = UInt8(wTime.wSecond);
			Item->WriteTime.year = UInt16(wTime.wYear);

			// Taille du fichier
			Item->FileSize = (wData.nFileSizeHigh * (UInt64(O3D_MAX_UINT32)+1)) + wData.nFileSizeLow;

			m_fileList.push_back(Item);
		}

		/*if (m_type != FILE_FILE)
		{
			// On test si on a bien . et .. si ce n'est pas le cas on les rajoutes
			Bool Found = False;
			for (i = 0 ; i < (Int32)m_FileList.size() ; i++)
			{
				if (m_FileList[i]->FileName == ".")
				{
					Found = True;
					break;
				}
			}

			// On ajoute les deux dossier . et ..
			if (!Found)
			{
				Item = new O3DFLItem;
				Item->FileSize = 0;
				Item->FileType = O3DFileDir;
				strcpy(Item->FileName,"..");
				m_fileList.insert(0,Item);

				Item = new O3DFLItem;
				Item->FileSize = 0;
				Item->FileType = O3DFileDir;
				strcpy(Item->FileName,".");
				m_fileList.insert(0,Item);
			}
		}*/
	}
}
#else // UNIX
void FileListing::searchFirstFile()		// Version UNIX du file listing
{
	struct dirent *lecture;

	struct stat FileInfos;
	FileInfos.st_ctim.tv_sec = FileInfos.st_atim.tv_sec = FileInfos.st_mtim.tv_sec = 0;

	DIR *rep;
	FLItem *Item;

	Char extension[16];
	Char *StartPos;
	Char *EndPos;

	m_curPos = -1;

	if (m_path.isEmpty())
		O3D_ERROR(E_InvalidPrecondition("Path must be defined"));

	rep = opendir(m_path.toUtf8().getData());
	if (rep)
	{
		while ((lecture = readdir(rep)))
		{
			stat(lecture->d_name, &FileInfos);

			if (((lecture->d_type == DT_DIR) && (m_type == FILE_FILE)) ||
				(!(lecture->d_type == DT_DIR) && (m_type == FILE_DIR)))
				continue;

			if (!isValidExt("*") && !(lecture->d_type == DT_DIR))
			{
				StartPos = strrchr(lecture->d_name, '.');
				if (StartPos)
				{
					StartPos++;
					EndPos = lecture->d_name + strlen(lecture->d_name);

					strncpy(extension, StartPos, EndPos-StartPos);
					extension[EndPos-StartPos] = '\0';

					if (!isValidExt(extension))
						continue;
				}
				else if (!isValidExt(""))
					continue;
			}

			Item = new FLItem;

			// Copie des informations du fichier
			Item->FileName.fromUtf8(lecture->d_name);
			Item->FileSize = FileInfos.st_size;

			// on met le type du fichier (fichier ou repertoire)
			if (lecture->d_type == DT_DIR)
				Item->FileType = FILE_DIR;
			else
				Item->FileType = FILE_FILE;

			// Time info
			Item->AccessTime.setTime(FileInfos.st_atime);
			Item->CreateTime.setTime(FileInfos.st_ctime);
			Item->WriteTime.setTime(FileInfos.st_mtime);

			m_fileList.push_back(Item);
		}
		closedir(rep);
	}
	else
	{
//		O3D_ERROR(O3D_E_InvalidResult(O3DString("Unable to open the directory ") + O3DString(m_path)));
	}
}
#endif

/*---------------------------------------------------------------------------------------
  return the next file (if NULL its finished)
---------------------------------------------------------------------------------------*/
FLItem* FileListing::searchNextFile()
{
	if (m_curPos+1 < (Int32)m_fileList.size())
	{
		m_curPos++;
		return m_fileList[m_curPos];
	}

	return NULL;
}

