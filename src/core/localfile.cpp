/**
 * @file localfile.cpp
 * @brief Local File system version of BaseFile.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-06-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/localfile.h"

#include "o3d/core/filemanager.h"
#include "o3d/core/debug.h"

#ifdef O3D_WINDOWS
	#include <direct.h>
	#include <io.h>
#else
	#include <unistd.h>
	#include <pwd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

using namespace o3d;

LocalFile::LocalFile(const String &filename) :
    BaseFile(filename)
{
    m_type = FL_LOCAL;
    m_isValid = exists();
}

LocalFile::LocalFile(const String &pathname, const String &filename) :
    BaseFile(pathname, filename)
{
    m_type = FL_LOCAL;
    m_isValid = exists();
}

LocalFile::LocalFile(const BaseDir &dir, const String &filename) :
    BaseFile(dir, filename)
{
    m_type = FL_LOCAL;
    m_isValid = exists();
}

LocalFile::LocalFile(const BaseFile& dup) :
    BaseFile(dup)
{
    m_type = FL_LOCAL;
    m_isValid = exists();
}

BaseFile *LocalFile::clone() const
{
    return new LocalFile(*this);
}

LocalFile::~LocalFile()
{
}

void LocalFile::setFile(const String &filename)
{
    BaseFile::setFile(filename);
    m_isValid = exists();
}

void LocalFile::setFile(const String &pathname,const String &filename)
{
    BaseFile::setFile(pathname,filename);
    m_isValid = exists();
}

void LocalFile::setFile(const BaseDir &dir, const String &filename)
{
    BaseFile::setFile(dir,filename);
    m_isValid = exists();
}

void LocalFile::cacheDatas()
{
    // fill the CachedData structure if cache is enable
	Int32 result;
	m_isValid = False;

#ifdef O3D_WINDOWS
	struct _stat statresult;
	memset(&statresult, 0, sizeof(struct _stat));
	result = _wstat(m_fullFilename.getData(), &statresult);

    if (result == 0) {
		DWORD fileAttr = GetFileAttributesW(m_fullFilename.getData());
		m_cachedData.hidden = fileAttr & FILE_ATTRIBUTE_HIDDEN;
	}
#else
	struct stat statresult;
	memset(&statresult, 0, sizeof(struct stat));
	result = stat(m_fullFilename.toUtf8().getData(), &statresult);

    if (result == 0) {
		m_cachedData.symLink = S_ISLNK(statresult.st_mode);
		m_cachedData.hidden = (m_fullFilename.length() >= 1) && (m_fullFilename[0] == '.');

		// user and group name
		struct passwd *pwd;

		pwd = getpwuid(statresult.st_uid);
		m_cachedData.ownerName.fromUtf8(pwd->pw_name);

		pwd = getpwuid(statresult.st_gid);
		m_cachedData.groupName.fromUtf8(pwd->pw_name);
	}
#endif

    if (result == 0) {
		m_cachedData.lastAccess.setTime(statresult.st_atime);
		m_cachedData.created.setTime(statresult.st_ctime);
		m_cachedData.modified.setTime(statresult.st_mtime);
		m_cachedData.groupId = statresult.st_gid;
		m_cachedData.ownerId = statresult.st_uid;
		m_cachedData.fileSize = (UInt64)statresult.st_size;

		m_isValid = True;
	}

    if (!m_isValid) {
		return;
    }

	m_cachedData.readable = False;
	m_cachedData.writable = False;

#ifdef O3D_WINDOWS
	if (_waccess(m_fullFilename.getData(), 6) == 0) {
		m_cachedData.readable = True;
		m_cachedData.writable = True;
		return;
	}

	if (_waccess(m_fullFilename.getData(), 2) == 0) {
		m_cachedData.writable = True;
	}

	if (_waccess(m_fullFilename.getData(), 4) == 0)
		m_cachedData.readable = True;
#else
    if (access(m_fullFilename.toUtf8().getData(), W_OK) == 0) {
		m_cachedData.writable = True;
    }

    if (access(m_fullFilename.toUtf8().getData(), R_OK) == 0) {
		m_cachedData.readable = True;
    }
#endif
}

// Get the file type.
FileTypes LocalFile::getType()
{
    if (!m_isValid) {
		return FILE_UNKNWON;
    }

    if (m_cached) {
		return m_cachedData.type;
    }

	Int32 result;

#ifdef O3D_WINDOWS
	struct _stat statresult;
	result = _wstat(m_fullFilename.getData(), &statresult);

    if (result == 0) {
        if (statresult.st_mode & _S_IFDIR) {
			m_cachedData.type = FILE_DIR;
        } else if ((statresult.st_mode & _S_IFREG)) { // || (statresult.st_mode & _S_IFLNK)) {
			m_cachedData.type = FILE_FILE;
        } else {
			m_cachedData.type = FILE_UNKNWON;
        }
	}
#else
	struct stat statresult;
	result = stat(m_fullFilename.toUtf8().getData(), &statresult);

    if (result == 0) {
        if (statresult.st_mode & S_IFDIR) {
			m_cachedData.type = FILE_DIR;
        } else if ((statresult.st_mode & S_IFREG) || (statresult.st_mode & S_IFLNK)) {
			m_cachedData.type = FILE_FILE;
        } else {
			m_cachedData.type = FILE_UNKNWON;
        }
	}
#endif

	return m_cachedData.type;
}

// Get the file size in bytes.
UInt64 LocalFile::getFileSize()
{
    if (!m_isValid) {
		return 0;
    }

    if (m_cached) {
		return m_cachedData.fileSize;
    }

	Int32 result;

#ifdef O3D_WINDOWS
	struct _stat statresult;
	result = _wstat(m_fullFilename.getData(), &statresult);
#else
	struct stat statresult;
	result = stat(m_fullFilename.toUtf8().getData(), &statresult);
#endif

    if (result == 0) {
		m_cachedData.fileSize = (UInt64)statresult.st_size;
    }

	return m_cachedData.fileSize;
}

// check if the filename exists
Bool LocalFile::exists()
{
	Int32 result;

#ifdef O3D_WINDOWS
	struct _stat statresult;
	result = _wstat(m_fullFilename.getData(), &statresult);
#else
	struct stat statresult;
	result = stat(m_fullFilename.toUtf8().getData(), &statresult);
#endif

    if (result == 0) {
		m_isValid = True;
		return True;
	}

	m_isValid = False;
	return False;
}

// is an absolute or relative filename ?
Bool LocalFile::isAbsolute() const
{
    if (!m_isValid || !m_fullFilename.length()) {
		return False;
    }

	return (!FileManager::instance()->isRelativePath(m_fullFilename));
}

Bool LocalFile::isReadable()
{
    // is the file readable (check the rights
    if (!m_isValid) {
		return False;
    }

    if (m_cached) {
		return m_cachedData.readable;
    }

	m_cachedData.readable = False;

#ifdef O3D_WINDOWS
    if (_waccess(m_fullFilename.getData(),6) == 0) {
		m_cachedData.readable = True;
    }

    if (_waccess(m_fullFilename.getData(),4) == 0) {
		m_cachedData.readable = True;
    }
#else
    if (access(m_fullFilename.toUtf8().getData(),R_OK) == 0) {
		m_cachedData.readable = True;
    }
#endif

	return m_cachedData.readable;
}

Bool LocalFile::isWritable()
{
    if (!m_isValid) {
		return False;
    }

    if (m_cached) {
		return m_cachedData.writable;
    }

	m_cachedData.writable = False;

#ifdef O3D_WINDOWS
    if (_waccess(m_fullFilename.getData(),6) == 0) {
		m_cachedData.writable = True;
    }

    if (_waccess(m_fullFilename.getData(),2) == 0) {
		m_cachedData.writable = True;
    }
#else
    if (access(m_fullFilename.toUtf8().getData(),W_OK) == 0) {
		m_cachedData.writable = True;
    }
#endif

	return m_cachedData.writable;
}

Bool LocalFile::isHidden()
{
    if (!m_isValid) {
		return False;
    }

    if (m_cached) {
		return m_cachedData.hidden;
    }

#ifdef O3D_WINDOWS
	DWORD fileAttr = GetFileAttributesW(m_fullFilename.getData());
	m_cachedData.hidden = fileAttr & FILE_ATTRIBUTE_HIDDEN;
#else
	m_cachedData.hidden = (m_fullFilename.length() >= 1) && (m_fullFilename[0] == '.');
#endif

	return m_cachedData.hidden;
}

Int16 LocalFile::getOwnerId()
{
    if (!m_isValid) {
        return 0;
    }

    if (m_cached) {
		return m_cachedData.ownerId;
    }

	Int32 result;

#ifdef O3D_WINDOWS
	struct _stat statresult;
	result = _wstat(m_fullFilename.getData(), &statresult);
#else
	struct stat statresult;
	result = stat(m_fullFilename.toUtf8().getData(), &statresult);
#endif

    if (result == 0) {
		m_cachedData.ownerId = statresult.st_uid;
		return m_cachedData.ownerId;
	}

	return 0;
}

const String& LocalFile::getOwnerName()
{
    if (!m_isValid) {
        return String::getNull();
    }

    if (m_cached) {
		return m_cachedData.ownerName;
    }

#ifdef O3D_WINDOWS
	// Nothing
#else
	Int32 result;

	struct stat statresult;
	result = stat(m_fullFilename.toUtf8().getData(), &statresult);

    if (result == 0) {
		m_cachedData.ownerId = statresult.st_uid;

		// user name
		struct passwd *pwd;

		pwd = getpwuid(statresult.st_uid);
		m_cachedData.ownerName.fromUtf8(pwd->pw_name);
	}
#endif

	return m_cachedData.ownerName;
}

Int16 LocalFile::getGroupId()
{
    if (!m_isValid) {
		return False;
    }

    if (m_cached) {
		return m_cachedData.groupId;
    }

	Int32 result;

#ifdef O3D_WINDOWS
	struct _stat statresult;
	result = _wstat(m_fullFilename.getData(), &statresult);
#else
	struct stat statresult;
	result = stat(m_fullFilename.toUtf8().getData(), &statresult);
#endif

    if (result == 0) {
		m_cachedData.groupId = statresult.st_gid;
		return m_cachedData.groupId;
	}

	return 0;
}

// return the group string name
const String& LocalFile::getGroupName()
{
    if (!m_isValid) {
        return String::getNull();
    }

    if (m_cached) {
		return m_cachedData.groupName;
    }

#ifdef O3D_WINDOWS
	// Nothing
#else
	Int32 result;

	struct stat statresult;
	result = stat(m_fullFilename.toUtf8().getData(), &statresult);

    if (result == 0) {
		m_cachedData.groupId = statresult.st_gid;

		// group name
		struct passwd *pwd;

		pwd = getpwuid(statresult.st_gid);
		m_cachedData.groupName.fromUtf8(pwd->pw_name);
	}
#endif

	return m_cachedData.groupName;
}

Bool LocalFile::makeAbsolute()
{
    if (!m_isValid) {
		return False;
    }

    if (isAbsolute()) {
		return True;
    }

	String oldFileName = m_fullFilename;
	m_fullFilename = FileManager::instance()->getWorkingDirectory() + '/' + m_fullFilename;

    if (!exists()) {
		m_fullFilename = oldFileName;

		O3D_ERROR(E_InvalidResult(m_fullFilename));
		return False;
	}

	return True;
}

const DateTime &LocalFile::getCreationDate()
{
    if (!m_isValid) {
        return DateTime::nullDate();
    }

    if (m_cached) {
		return m_cachedData.created;
    }

	Int32 result;

#ifdef O3D_WINDOWS
	struct _stat statresult;
	statresult.st_ctime = 0;
	result = _wstat(m_fullFilename.getData(), &statresult);
#else
	struct stat statresult;
	statresult.st_ctim.tv_sec = 0;
	result = stat(m_fullFilename.toUtf8().getData(), &statresult);
#endif

    if (result == 0) {
		m_cachedData.created.setTime(statresult.st_ctime);
    }

	return m_cachedData.created;
}

const DateTime &LocalFile::getLastAccessDate()
{
    if (!m_isValid) {
        return DateTime::nullDate();
    }

    if (m_cached) {
		return m_cachedData.lastAccess;
    }

	Int32 result;

#ifdef O3D_WINDOWS
	struct _stat statresult;
	statresult.st_atime = 0;
	result = _wstat(m_fullFilename.getData(), &statresult);
#else
	struct stat statresult;
	statresult.st_atim.tv_sec = 0;
	result = stat(m_fullFilename.toUtf8().getData(), &statresult);
#endif

    if (result == 0) {
		m_cachedData.lastAccess.setTime(statresult.st_atime);
    }

	return m_cachedData.lastAccess;
}

const DateTime& LocalFile::getModifiedDate()
{
    if (!m_isValid) {
        return DateTime::nullDate();
    }

    if (m_cached) {
		return m_cachedData.modified;
    }

	Int32 result;

#ifdef O3D_WINDOWS
	struct _stat statresult;
	statresult.st_mtime = 0;
	result = _wstat(m_fullFilename.getData(), &statresult);
#else
	struct stat statresult;
	statresult.st_mtim.tv_sec = 0;
	result = stat(m_fullFilename.toUtf8().getData(), &statresult);
#endif

    if (result == 0) {
		m_cachedData.modified.setTime(statresult.st_mtime);
    }

	return m_cachedData.modified;
}

Bool LocalFile::isSymbolicLink()
{
    if (!m_isValid) {
		return False;
    }

    if (m_cached) {
        return m_cachedData.symLink;
    }

#ifdef O3D_WINDOWS
	// not supported
#else
	Int32 result;

	struct stat statresult;
	result = stat(m_fullFilename.toUtf8().getData(), &statresult);

    if (result == 0) {
		m_cachedData.symLink = S_ISLNK(statresult.st_mode);
	}
#endif

	return m_cachedData.symLink;
}

Bool LocalFile::isInRoot() const
{
    if (!m_isValid) {
		return False;
    }

#ifdef O3D_WINDOWS
    if ((m_fullFilename.length() > 2) && (m_fullFilename[1] == ':') && m_fullFilename.count('/') < 2) {
		return True;
    }
#else
    if ((m_fullFilename.length() > 1) && (m_fullFilename[0] == '/') && m_fullFilename.count('/') < 2) {
		return True;
    }
#endif

	return False;
}
