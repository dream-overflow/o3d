/**
 * @file localdir.cpp
 * @brief Local file system directory management.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-06-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/localdir.h"

#include "o3d/core/filemanager.h"
#include "o3d/core/stringtokenizer.h"
#include "o3d/core/virtualfilelisting.h"

#ifdef O3D_WINDOWS
	#include <direct.h>
	#include <io.h>
#else
	#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

using namespace o3d;

LocalDir LocalDir::current()
{
    return LocalDir(FileManager::instance()->getWorkingDirectory());
}

LocalDir::LocalDir(const String& pathname) :
    BaseDir(pathname)
{
    m_type = FL_LOCAL;
}

LocalDir::LocalDir(const LocalDir& dup) :
    BaseDir(dup)
{
    m_type = FL_LOCAL;
}

LocalDir::~LocalDir()
{
}

BaseDir *LocalDir::clone() const
{
    return new LocalDir(*this);
}

// clean the path (remove '..' and '.' when possible)
void LocalDir::clean()
{
    FileManager::adaptPath(m_fullPathname);
}

Bool LocalDir::empty() const
{
    // @todo could be optimized (but what about virtualfile listing if we optimize ?)
    return findFiles().size() == 0;
}

Bool LocalDir::isAbsolute() const
{
    if (!m_isValid || m_fullPathname.isEmpty()) {
		return False;
    }

	return (!FileManager::instance()->isRelativePath(m_fullPathname));
}

Bool LocalDir::isReadable() const
{
    if (!m_isValid) {
		return False;
    }

#ifdef O3D_WINDOWS
    if (_waccess(m_fullPathname.getData(),4) == 0) {
		return True;
    }

    if (_waccess(m_fullPathname.getData(),6) == 0) {
		return True;
    }
#else
    if (access(m_fullPathname.toUtf8().getData(),R_OK) == 0) {
		return True;
    }
#endif

	return True;
}

Bool LocalDir::isWritable() const
{
    if (!m_isValid) {
		return False;
    }

#ifdef O3D_WINDOWS
    if (_waccess(m_fullPathname.getData(),2) == 0) {
		return True;
    }

    if (_waccess(m_fullPathname.getData(),6) == 0) {
		return True;
    }
#else
    if (access(m_fullPathname.toUtf8().getData(),W_OK) == 0) {
		return True;
    }
#endif

	return True;
}

Bool LocalDir::isRoot() const
{
#ifdef O3D_WINDOWS
	return (isAbsolute() && (m_fullPathname.length() < 4));
#else
	return ((m_fullPathname.length() == 1) && (m_fullPathname[0] == '/'));
#endif
}

BaseDir::DirReturn LocalDir::removeDir(const String &path) const
{
	String lPath(path);
	lPath.replace('\\','/');
	lPath.trimRight('/');

    if (!m_isValid || (lPath.sub("..",0) != -1) || (lPath.sub("/",0) == 0)) {
		O3D_ERROR(E_InvalidResult(lPath + " is an invalid path"));
		return INVALID_PATH;
	}

	String newPath = m_fullPathname + '/' + lPath;

    Int32 result;

#ifdef O3D_WINDOWS
	result = _wrmdir(newPath.getData());
#else
	result = rmdir(newPath.toUtf8().getData());
#endif

    if (result == 0) {
		return SUCCESS;
    } else if (errno == ENOTEMPTY) {
		O3D_ERROR(E_InvalidResult(lPath + " is not empty"));
		return NOT_EMPTY_DIR;
    } else if (errno == EPERM) {
		O3D_ERROR(E_InvalidResult(lPath + " non permit"));
		return NOT_PERMIT;
    } else if (errno == EEXIST) {
		O3D_ERROR(E_InvalidResult(lPath + " already exist"));
		return ALREADY_EXISTS;
    } else if (errno == EFAULT) {
		O3D_ERROR(E_InvalidResult(lPath + " is an invalid address"));
		return INVALID_ADDRESS;
    } else if (errno == EACCES) {
		O3D_ERROR(E_InvalidResult(lPath + " non read access"));
		return NOT_READ_ACCESS;
    } else if (errno == ENAMETOOLONG) {
		O3D_ERROR(E_InvalidResult(lPath + " name is too long"));
		return TOO_LONG;
    } else if (errno == ENOENT) {
		O3D_ERROR(E_InvalidResult(lPath + " name is too long"));
		return INVALID_PATH;
    } else if (errno == ENOMEM) {
		O3D_ERROR(E_InvalidResult(lPath + " Kernel memory error"));
		return MEMORY_ERROR;
    } else if (errno == EROFS) {
		O3D_ERROR(E_InvalidResult(lPath + " is read only"));
		return READ_ONLY;
    } else if (errno == EBUSY) {
		O3D_ERROR(E_InvalidResult(lPath + " is busy by another process"));
		return ACCESS_PROGRAM;
	}
#ifndef O3D_WINDOWS
    else if (errno == ELOOP) {
		O3D_ERROR(E_InvalidResult(lPath + " have circulars references"));
		return CIRCULAR_REF;
	}
#endif
    else if (errno == ENOSPC) {
		O3D_ERROR(E_InvalidResult(lPath + " Insufficiency space"));
		return INSUFISANCE_SPACE;
	}

	O3D_ERROR(E_InvalidResult(lPath + " Unknown error"));
	return UNKNOWN_RET;
}

BaseDir::DirReturn LocalDir::makeDir(const String &path) const
{
	String lPath(path);
	lPath.replace('\\','/');
	lPath.trimRight('/');

    if (!m_isValid || (lPath.sub("..",0) != -1) || (lPath.sub("/",0) == 0)) {
		O3D_ERROR(E_InvalidResult(lPath + " is an invalid path"));
		return INVALID_PATH;
	}

	String newPath = m_fullPathname + '/' + lPath;

    Int32 result;

#ifdef O3D_WINDOWS
	result = _wmkdir(newPath.getData());
#else
	result = mkdir(newPath.toUtf8().getData(), 0775);
#endif

    if (result == 0) {
		return SUCCESS;
    } else if (errno == EPERM) {
		O3D_ERROR(E_InvalidResult(lPath + " not permit"));
		return NOT_PERMIT;
    } else if (errno == EEXIST) {
		O3D_ERROR(E_InvalidResult(lPath + " already exist"));
		return ALREADY_EXISTS;
    } else if (errno == EFAULT) {
		O3D_ERROR(E_InvalidResult(lPath + " invalid address"));
		return INVALID_ADDRESS;
    } else if (errno == EACCES)	{
		O3D_ERROR(E_InvalidResult(lPath + " non read access"));
		return NOT_READ_ACCESS;
    } else if (errno == ENAMETOOLONG) {
		O3D_ERROR(E_InvalidResult(lPath + " is too long"));
		return TOO_LONG;
    } else if (errno == ENOENT) {
		O3D_ERROR(E_InvalidResult(lPath + " is an invalid path"));
		return INVALID_PATH;
    } else if (errno == ENOMEM) {
		O3D_ERROR(E_InvalidResult(lPath + " Kernel memory error"));
		return MEMORY_ERROR;
    } else if (errno == EROFS) {
		O3D_ERROR(E_InvalidResult(lPath + " is read only"));
		return READ_ONLY;
	}
#ifndef O3D_WINDOWS
    else if (errno == ELOOP) {
		O3D_ERROR(E_InvalidResult(lPath + " have circulars references"));
		return CIRCULAR_REF;
	}
#endif
    else if (errno == ENOSPC) {
		O3D_ERROR(E_InvalidResult(lPath + " Insufficiency Space"));
		return INSUFISANCE_SPACE;
	}

	O3D_ERROR(E_InvalidResult(lPath + " Unknown error"));
	return UNKNOWN_RET;
}

BaseDir::DirReturn LocalDir::makePath(const String &path) const
{
	String lPath(path);
	lPath.replace('\\','/');
	lPath.trimRight('/');

    if (!m_isValid || (lPath.sub("..",0) != -1) || (lPath.sub("/",0) == 0)) {
		O3D_ERROR(E_InvalidResult(lPath + " is an invalid path"));
		return INVALID_PATH;
	}

	StringTokenizer tokenize(m_fullPathname + '/' + lPath, L"/");
	String currentPath(m_fullPathname);

    Int32 result = 0;

    while (tokenize.hasMoreElements()) {
		String nextElt(tokenize.nextElement());

#ifdef O3D_WINDOWS
		result = _wmkdir((currentPath + '/' + nextElt).getData());
#else
		result = mkdir((currentPath + '/' + nextElt).toUtf8().getData(), 0775);
#endif

        if ((result != 0) && (errno != EEXIST)) {
			break;
        }

		currentPath = currentPath + '/' + nextElt;
	}

    if (result == 0) {
		return SUCCESS;
    } else if (errno == EPERM) {
		O3D_ERROR(E_InvalidResult(lPath + " non permit"));
		return NOT_PERMIT;
    } else if (errno == EEXIST) {
		O3D_ERROR(E_InvalidResult(lPath + " already exist"));
		return ALREADY_EXISTS;
    } else if (errno == EFAULT) {
		O3D_ERROR(E_InvalidResult(lPath + " is an invalid address"));
		return INVALID_ADDRESS;
    } else if (errno == EACCES)	{
		O3D_ERROR(E_InvalidResult(lPath + " non read access"));
		return NOT_READ_ACCESS;
    } else if (errno == ENAMETOOLONG) {
		O3D_ERROR(E_InvalidResult(lPath + " is too long"));
		return TOO_LONG;
    } else if (errno == ENOENT) {
		O3D_ERROR(E_InvalidResult(lPath + " is an invalid path"));
		return INVALID_PATH;
    } else if (errno == ENOMEM) {
		O3D_ERROR(E_InvalidResult(lPath + " Kernel memory error"));
		return MEMORY_ERROR;
    } else if (errno == EROFS) {
		O3D_ERROR(E_InvalidResult(lPath + " is read only"));
		return READ_ONLY;
	}
#ifndef O3D_WINDOWS
    else if (errno == ELOOP) {
		O3D_ERROR(E_InvalidResult(lPath + " have circulars references"));
		return CIRCULAR_REF;
	}
#endif
    else if (errno == ENOSPC) {
		O3D_ERROR(E_InvalidResult(lPath + " Insufficiency space"));
		return INSUFISANCE_SPACE;
	}

	O3D_ERROR(E_InvalidResult(lPath + " Unknown error"));
	return UNKNOWN_RET;
}

BaseDir::DirReturn LocalDir::check(const String &fileOrPath) const
{
	String lFileOrPath(fileOrPath);
	lFileOrPath.replace('\\','/');

    if (!m_isValid || (lFileOrPath.sub("/",0) == 0)) {
		O3D_ERROR(E_InvalidResult(fileOrPath + " is an invalid path or filename"));
		return INVALID_PATH;
	}

	String toCheck(m_fullPathname + '/' + lFileOrPath);
    FileManager::adaptPath(toCheck);

    Int32 result;

#ifdef O3D_WINDOWS
	struct _stat statresult;
	result = _wstat(toCheck.getData(), &statresult);
#else
	struct stat statresult;
	result = stat(toCheck.toUtf8().getData(), &statresult);
#endif

    if (result == 0) {
		return SUCCESS;
    } else if (errno == EBADF) {
		//O3D_ERROR(O3D_E_InvalidResult(toCheck + " is not found"));
		return NOT_FOUND;
    } else if (errno == EFAULT) {
		//O3D_ERROR(O3D_E_InvalidResult(toCheck + " is an invalid address"));
		return INVALID_ADDRESS;
    } else if (errno == EACCES) {
		//O3D_ERROR(O3D_E_InvalidResult(toCheck + " non read access"));
		return NOT_READ_ACCESS;
    } else if (errno == ENAMETOOLONG) {
		//O3D_ERROR(O3D_E_InvalidResult(toCheck + " is too long"));
		return TOO_LONG;
    } else if (errno == ENOENT) {
		//O3D_ERROR(O3D_E_InvalidResult(toCheck + " is an invalid path"));
		return INVALID_PATH;
    } else if (errno == ENOMEM) {
		//O3D_ERROR(O3D_E_InvalidResult(toCheck + " Kernel memory error"));
		return MEMORY_ERROR;
    } else if (errno == ENOTDIR) {
		//O3D_ERROR(O3D_E_InvalidResult(toCheck + " is an invalid path"));
		return INVALID_PATH;
	}
#ifndef O3D_WINDOWS
    else if (errno == ELOOP) {
		//O3D_ERROR(O3D_E_InvalidResult(toCheck + " have circulars references"));
		return CIRCULAR_REF;
	}
#endif

	//O3D_ERROR(O3D_E_InvalidResult(toCheck + " Unknown error"));
	return UNKNOWN_RET;
}

T_FLItem_List LocalDir::findFilesInfos(const String &filters, FileTypes Type) const
{
    if (!m_isValid) {
		return T_FLItem_List();
    }

	VirtualFileListing fileListing;
	fileListing.setPath(m_fullPathname);
	fileListing.setExt(filters);
	fileListing.setType(Type);

	fileListing.searchFirstFile();

	T_FLItem_List result;
	FLItem *Item;
    while ((Item = fileListing.searchNextFile()) != nullptr) {
		result.push_back(*Item);
	}

	return result;
}

T_StringList LocalDir::findFiles(const String &filters, FileTypes Type) const
{
    if (!m_isValid) {
		return T_StringList();
    }

	VirtualFileListing fileListing;
	fileListing.setPath(m_fullPathname);
	fileListing.setExt(filters);
	fileListing.setType(Type);

	fileListing.searchFirstFile();

	T_StringList result;
	FLItem *Item;
    while ((Item = fileListing.searchNextFile()) != nullptr) {
		result.push_back(Item->FileName);
	}

	return result;
}

Bool LocalDir::makeAbsolute()
{
    if (!m_isValid) {
		return False;
    }

    if (isAbsolute()) {
		return True;
    }

	String oldPath = m_fullPathname;
	m_fullPathname = FileManager::instance()->getWorkingDirectory() + '/' + m_fullPathname;

    if (!exists()) {
		m_fullPathname = oldPath;

		O3D_ERROR(E_InvalidResult(m_fullPathname + " can not make absolute"));
		return False;
	}

	return True;
}

BaseDir::DirReturn LocalDir::copyFile(const String &filename, UInt32 blockSize) const
{
	String srcFilename(filename);
	srcFilename.replace('\\','/');

    if (!m_isValid) {
		O3D_ERROR(E_InvalidResult(srcFilename + " is an invalid path"));
    }

    InStream *src = FileManager::instance()->openInStream(srcFilename);

	String destFile(m_fullPathname);
    Int32 pos = srcFilename.reverseFind('/');
    if (pos != -1) {
		destFile = destFile + '/' + srcFilename.sub(pos+1);
    } else {
		destFile = destFile + '/' + srcFilename;
    }

    FileOutStream *dest = FileManager::instance()->openOutStream(destFile, FileOutStream::CREATE);

    if (blockSize == 0) {
        blockSize = 32768;
    }

    UInt32 SrcSize = src->getAvailable();
    UInt8 *pBuffer = new UInt8[blockSize];

	// copy the file
	UInt32 count = 0;
    while (count < SrcSize) {
        src->read(pBuffer, blockSize);
        dest->write(pBuffer, blockSize);

        count += blockSize;

        if (count > SrcSize) {
            blockSize -= count - SrcSize;
        }
	}

	deleteArray(pBuffer);

    deletePtr(src);
    deletePtr(dest);

	return SUCCESS;
}

BaseDir::DirReturn LocalDir::removeFile(const String &filename) const
{
	String lFilename(filename);
	lFilename.replace('\\','/');

    if (!m_isValid || (lFilename.sub("..",0) != -1) || (lFilename.sub("/",0) == 0)) {
		O3D_ERROR(E_InvalidResult(lFilename + " is an invalid path filename"));
		return INVALID_PATH;
	}

#ifdef O3D_WINDOWS
    Int32 result = _wremove((m_fullPathname + '/' + lFilename).getData());
#else
    Int32 result = remove((m_fullPathname + '/' + lFilename).toUtf8().getData());
#endif

    if (result == 0) {
		return SUCCESS;
    } else if (errno == EPERM) {
		O3D_ERROR(E_InvalidResult(lFilename + " non permit"));
		return NOT_PERMIT;
    } else if (errno == EBUSY) {
		O3D_ERROR(E_InvalidResult(lFilename + " is busy by another process"));
		return ACCESS_PROGRAM;
    } else if (errno == EINVAL) {
		O3D_ERROR(E_InvalidResult(lFilename + " is an invalid path"));
		return INVALID_PATH;
    } else if (errno == EFAULT) {
		O3D_ERROR(E_InvalidResult(lFilename + " invalid address"));
		return INVALID_ADDRESS;
    } else if (errno == EACCES) {
		O3D_ERROR(E_InvalidResult(lFilename + " non read access"));
		return NOT_READ_ACCESS;
    } else if (errno == ENAMETOOLONG) {
		O3D_ERROR(E_InvalidResult(lFilename + " is too long"));
		return TOO_LONG;
    } else if (errno == ENOENT) {
		O3D_ERROR(E_InvalidResult(lFilename + " is an invalid path"));
		return INVALID_PATH;
    } else if (errno == ENOMEM) {
		O3D_ERROR(E_InvalidResult(lFilename + " Kernel memory error"));
		return MEMORY_ERROR;
    } else if (errno == EROFS) {
		O3D_ERROR(E_InvalidResult(lFilename + " is read only"));
		return READ_ONLY;
    } else if (errno == ENOTDIR) {
		O3D_ERROR(E_InvalidResult(lFilename + " is an invalid path"));
		return INVALID_PATH;
    } else if (errno == EMLINK) {
		O3D_ERROR(E_InvalidResult(lFilename + " is an invalid path link"));
		return INVALID_PATH;
	}
#ifndef O3D_WINDOWS
    else if (errno == ELOOP) {
		O3D_ERROR(E_InvalidResult(lFilename + " have circulars references"));
		return CIRCULAR_REF;
	}
#endif

	O3D_ERROR(E_InvalidResult(lFilename + " Unknown error"));
	return UNKNOWN_RET;
}

BaseDir::DirReturn LocalDir::rename(const String &oldName, const String &newName) const
{
	String lOldName(oldName);
	lOldName.replace('\\','/');

	String lNewName(newName);
	lNewName.replace('\\','/');

	if (!m_isValid || (lOldName.sub("..",0) != -1) || (lNewName.sub("..",0) != -1) ||
            (lOldName.sub("/",0) == 0) || (lNewName.sub("/",0) == 0)) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " invalid path"));
		return INVALID_PATH;
	}

#ifdef O3D_WINDOWS
    Int32 result = ::_wrename(
		(m_fullPathname + '/' + lOldName).getData(),
		(m_fullPathname + '/' + lNewName).getData());
#else
    Int32 result = ::rename(
		(m_fullPathname + '/' + lOldName).toUtf8().getData(),
		(m_fullPathname + '/' + lNewName).toUtf8().getData());
#endif

    if (result == 0) {
		return SUCCESS;
    } else if (errno == EISDIR) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " Old path is not a directory"));
		return OLD_PATH_IS_NOT_DIR;
    } else if (errno == EXDEV) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " The paths are not in same file system (environment)"));
		return NOT_SOME_ENV;
    } else if (errno == EEXIST) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " already existing"));
		return ALREADY_EXISTS;
    } else if (errno == ENOTEMPTY) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " non empty"));
		return ALREADY_EXISTS;
    }else if (errno == EPERM) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " non permit"));
		return NOT_PERMIT;
    } else if (errno == EBUSY) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " busy by another process"));
		return ACCESS_PROGRAM;
    } else if (errno == EINVAL) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " invalid path"));
		return INVALID_PATH;
    } else if (errno == EFAULT) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " invalid address"));
		return INVALID_ADDRESS;
    } else if (errno == EACCES) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " non read access"));
		return NOT_READ_ACCESS;
    } else if (errno == ENAMETOOLONG) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " is too long"));
		return TOO_LONG;
    } else if (errno == ENOENT) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " invalid Path"));
		return INVALID_PATH;
    } else if (errno == ENOMEM) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " Kernel memory error"));
		return MEMORY_ERROR;
    } else if (errno == EROFS) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " is read only"));
		return READ_ONLY;
    } else if (errno == ENOTDIR) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " invalid path"));
		return INVALID_PATH;
    } else if (errno == EMLINK) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " invalid path link"));
		return INVALID_PATH;
	}
#ifndef O3D_WINDOWS
    else if (errno == ELOOP) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " have circulars references"));
		return CIRCULAR_REF;
	}
#endif
    else if (errno == ENOSPC) {
		O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " Insufficient space"));
		return INSUFISANCE_SPACE;
	}

	O3D_ERROR(E_InvalidResult(lOldName + "=>" + lNewName + " Unknown error"));
	return UNKNOWN_RET;
}
