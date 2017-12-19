/**
 * @file virtualdir.cpp
 * @brief Virtual directory management.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-17
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/virtualdir.h"

#include "o3d/core/filemanager.h"
#include "o3d/core/stringtokenizer.h"
#include "o3d/core/virtualfilelisting.h"

using namespace o3d;

//! default constructor. Is the path is empty '.' is used.
VirtualDir::VirtualDir(const String& pathname) :
    BaseDir(pathname)
{
    m_type = FL_VIRTUAL;
}

//! copy constructor
VirtualDir::VirtualDir(const VirtualDir& dup) :
    BaseDir(dup)
{
    m_type = FL_VIRTUAL;
}

VirtualDir::~VirtualDir()
{
}

BaseDir *VirtualDir::clone() const
{
    return new VirtualDir(*this);
}

void VirtualDir::clean()
{
    FileManager::adaptPath(m_fullPathname);
}

Bool VirtualDir::empty() const
{
    return findFiles().size() == 0;
}

Bool VirtualDir::isAbsolute() const
{
    if (!m_isValid || m_fullPathname.isEmpty()) {
        return False;
    }

    return (!FileManager::instance()->isRelativePath(m_fullPathname));
}

Bool VirtualDir::isReadable() const
{
    if (!m_isValid) {
        return False;
    }

    // always readable
    return True;
}

Bool VirtualDir::isWritable() const
{
    if (!m_isValid) {
        return False;
    }

    // never writable
    return False;
}

Bool VirtualDir::isRoot() const
{
#ifdef O3D_WINDOWS
    return (isAbsolute() && (m_fullPathname.length() < 4));
#else
    return ((m_fullPathname.length() == 1) && (m_fullPathname[0] == '/'));
#endif
}

BaseDir::DirReturn VirtualDir::removeDir(const String &path) const
{
    O3D_ERROR(E_InvalidResult(path + " non permit"));
    return NOT_PERMIT;
}

BaseDir::DirReturn VirtualDir::makeDir(const String &path) const
{
    O3D_ERROR(E_InvalidResult(path + " non permit"));
    return NOT_PERMIT;
}

BaseDir::DirReturn VirtualDir::makePath(const String &path) const
{
    O3D_ERROR(E_InvalidResult(path + " non permit"));
    return NOT_PERMIT;
}

BaseDir::DirReturn VirtualDir::check(const String &fileOrPath) const
{
    String lFileOrPath(fileOrPath);
    lFileOrPath.replace('\\','/');

    if (!m_isValid || (lFileOrPath.sub("/",0) == 0)) {
        O3D_ERROR(E_InvalidResult(fileOrPath + " is an invalid path or filename"));
        return INVALID_PATH;
    }

    String toCheck(m_fullPathname + '/' + lFileOrPath);
    FileManager::adaptPath(toCheck);

    if (FileManager::instance()->isFile(toCheck) || FileManager::instance()->isPath(toCheck)) {
        return SUCCESS;
    }

    return NOT_FOUND;
}

T_FLItem_List VirtualDir::findFilesInfos(const String &filters, FileTypes Type) const
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

T_StringList VirtualDir::findFiles(const String &filters, FileTypes Type) const
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

Bool VirtualDir::makeAbsolute()
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

BaseDir::DirReturn VirtualDir::copyFile(const String &filename, UInt32 blockSize) const
{
    O3D_ERROR(E_InvalidResult(filename + " non permit"));
    return NOT_PERMIT;
}

BaseDir::DirReturn VirtualDir::removeFile(const String &filename) const
{
    O3D_ERROR(E_InvalidResult(filename + " non permit"));
    return NOT_PERMIT;
}

BaseDir::DirReturn VirtualDir::rename(const String &oldName, const String &newName) const
{
    O3D_ERROR(E_InvalidResult(oldName + " non permit"));
    return NOT_PERMIT;
}
