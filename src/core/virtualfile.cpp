/**
 * @file virtualfileinfo.cpp
 * @brief Virtual (asset) sub file system file information.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-17
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/virtualfile.h"

#include "o3d/core/filemanager.h"
#include "o3d/core/debug.h"

using namespace o3d;

VirtualFile::VirtualFile(const String &filename) :
    BaseFile(filename)
{
    m_type = FL_VIRTUAL;
    m_isValid = m_fullFilename.isValid(); // exists();
}

VirtualFile::VirtualFile(const String &pathname,const String &filename) :
    BaseFile(pathname,filename)
{
    m_type = FL_VIRTUAL;
    m_isValid = m_fullFilename.isValid(); // exists();
}

VirtualFile::VirtualFile(const BaseDir &dir,const String &filename) :
    BaseFile(dir,filename)
{
    m_type = FL_VIRTUAL;
    m_isValid = m_fullFilename.isValid(); // exists();
}

VirtualFile::VirtualFile(const BaseFile& dup) :
    BaseFile(dup)
{
    m_type = FL_VIRTUAL;
    m_isValid = m_fullFilename.isValid(); // exists();
}

VirtualFile::~VirtualFile()
{
}

BaseFile *VirtualFile::clone() const
{
    return new VirtualFile(*this);
}

void VirtualFile::setFile(const String &filename)
{
    BaseFile::setFile(filename);
    m_isValid = m_fullFilename.isValid(); // exists();
}

void VirtualFile::setFile(const String &pathname,const String &filename)
{
    BaseFile::setFile(pathname, filename);
    m_isValid = m_fullFilename.isValid(); // exists();
}

void VirtualFile::setFile(const BaseDir &dir, const String &filename)
{
    BaseFile::setFile(dir,filename);
    m_isValid = m_fullFilename.isValid(); // exists();
}

void VirtualFile::cacheDatas()
{
    // fill the CachedData structure if cache is enable
    m_isValid = False;

    m_cachedData.symLink = False;
    m_cachedData.hidden = (m_fullFilename.length() >= 1) && (m_fullFilename[0] == '.');

    // m_cachedData.ownerName
    // m_cachedData.groupName

    // m_cachedData.lastAccess.setTime(statresult.st_atime);
    // m_cachedData.created.setTime(statresult.st_ctime);
    // m_cachedData.modified.setTime(statresult.st_mtime);
    m_cachedData.groupId = 0;
    m_cachedData.ownerId = 0;
    m_cachedData.fileSize = FileManager::instance()->fileSize(m_fullFilename);

    m_isValid = True;

    if (!m_isValid) {
        return;
    }

    m_cachedData.readable = True;
    m_cachedData.writable = False;
}

// Get the file type.
FileTypes VirtualFile::getType()
{
    if (!m_isValid) {
        return FILE_UNKNWON;
    }

    if (m_cached) {
        return m_cachedData.type;
    }

    m_cachedData.type = FileManager::instance()->fileType(m_fullFilename);
    return m_cachedData.type;
}

// Get the file size in bytes.
UInt64 VirtualFile::getFileSize()
{
    if (!m_isValid) {
        return 0;
    }

    if (m_cached) {
        return m_cachedData.fileSize;
    }

    m_cachedData.fileSize = FileManager::instance()->fileSize(m_fullFilename);
    return m_cachedData.fileSize;
}

Bool VirtualFile::exists()
{
    m_isValid = FileManager::instance()->isFile(m_fullFilename);
    return m_isValid;
}

Bool VirtualFile::isAbsolute() const
{
    if (!m_isValid || !m_fullFilename.length()) {
        return False;
    }

    return (!FileManager::instance()->isRelativePath(m_fullFilename));
}

Bool VirtualFile::isReadable()
{
    if (!m_isValid) {
        return False;
    }

    return True;
}

Bool VirtualFile::isWritable()
{
    if (!m_isValid) {
        return False;
    }

    return False;
}

Bool VirtualFile::isHidden()
{
    if (!m_isValid) {
        return False;
    }

    if (m_cached) {
        return m_cachedData.hidden;
    }

    m_cachedData.hidden = (m_fullFilename.length() >= 1) && (m_fullFilename[0] == '.');

    return m_cachedData.hidden;
}

Int16 VirtualFile::getOwnerId()
{
    if (!m_isValid) {
        return False;
    }

    return 0;
}

const String& VirtualFile::getOwnerName()
{
    if (!m_isValid) {
        return String::getNull();
    }

    return String::getNull();
}

Int16 VirtualFile::getGroupId()
{
    if (!m_isValid) {
        return 0;
    }

    return 0;
}

const String& VirtualFile::getGroupName()
{
    if (!m_isValid) {
        return String::getNull();
    }

    return String::getNull();
}

Bool VirtualFile::makeAbsolute()
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

const DateTime &VirtualFile::getCreationDate()
{
    if (!m_isValid) {
        return DateTime::nullDate();
    }

    // @todo could be supported by some archives...
    return DateTime::nullDate();
}

const DateTime &VirtualFile::getLastAccessDate()
{
    if (!m_isValid) {
        return DateTime::nullDate();
    }

    // @todo could be supported by some archives...
    return DateTime::nullDate();
}

const DateTime& VirtualFile::getModifiedDate()
{
    if (!m_isValid) {
        return DateTime::nullDate();
    }

    // @todo could be supported by some archives...
    return DateTime::nullDate();
}

Bool VirtualFile::isSymbolicLink()
{
    if (!m_isValid) {
        return False;
    }

    // @todo could be supported by some archive...
    return False;
}

Bool VirtualFile::isInRoot() const
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
