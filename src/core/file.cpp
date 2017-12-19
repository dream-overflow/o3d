/**
 * @file file.cpp
 * @brief Implementation of file.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-01-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include "o3d/core/file.h"
#include "o3d/core/debug.h"
#include "o3d/core/basefile.h"
#include "o3d/core/filemanager.h"

using namespace o3d;

File::File(const String &fileName)
{
    m_fi = FileManager::instance()->file(fileName);
}

File::File(const String &pathName, const String &fileName)
{
    m_fi = FileManager::instance()->file(pathName + '/' + fileName);
}

File::File(const BaseDir &dir, const String &fileName)
{
    m_fi = FileManager::instance()->file(dir.getFullPathName() + '/' + fileName);
}

File::File(const File &dup) :
    m_fi(nullptr)
{
    if (dup.m_fi) {
        m_fi = dup.m_fi->clone();
    }
}

File::File(const BaseFile &dup)
{
    m_fi = dup.clone();
}

File::File(BaseFile *own)
{
    m_fi = own;
}

File::~File()
{
    deletePtr(m_fi);
}

File &File::operator=(const BaseFile &dup)
{
    deletePtr(m_fi);

    m_fi = dup.clone();
    return *this;
}

File &File::operator=(const File &dup)
{
    deletePtr(m_fi);

    if (dup.m_fi) {
        m_fi = dup.m_fi->clone();
    }

    return *this;
}

FileTypes File::getType()
{
    if (!m_fi) {
        return FILE_UNKNWON;
    }

    return m_fi->getType();
}

UInt64 File::getFileSize()
{
    if (!m_fi) {
        return 0;
    }

    return m_fi->getFileSize();
}

Bool File::exists()
{
    if (!m_fi) {
        return False;
    }

    return m_fi->exists();
}

Bool File::isAbsolute() const
{
    if (!m_fi) {
        return False;
    }

    return m_fi->isAbsolute();
}

Bool File::isReadable()
{
    if (!m_fi) {
        return False;
    }

    return m_fi->isReadable();
}

Bool File::isWritable()
{
    if (!m_fi) {
        return False;
    }

    return m_fi->isWritable();
}

Bool File::isHidden()
{
    if (!m_fi) {
        return False;
    }

    return m_fi->isHidden();
}

Int16 File::getOwnerId()
{
    if (!m_fi) {
        return 0;
    }

    return m_fi->getOwnerId();
}

const String &File::getOwnerName()
{
    if (!m_fi) {
        return String::getNull();
    }

    return m_fi->getOwnerName();
}

Int16 File::getGroupId()
{
    if (!m_fi) {
        return 0;
    }

    return m_fi->getGroupId();
}

const String &File::getGroupName()
{
    if (!m_fi) {
        return String::getNull();
    }

    return m_fi->getGroupName();
}

Bool File::makeAbsolute()
{
    if (!m_fi) {
        return False;
    }

    return m_fi->makeAbsolute();
}

const DateTime &File::getCreationDate()
{
    if (!m_fi) {
        return DateTime::nullDate();
    }

    return m_fi->getCreationDate();
}

const DateTime &File::getLastAccessDate()
{
    if (!m_fi) {
        return DateTime::nullDate();
    }

    return m_fi->getLastAccessDate();
}

const DateTime &File::getModifiedDate()
{
    if (!m_fi) {
        return DateTime::nullDate();
    }

    return m_fi->getModifiedDate();
}

Bool File::isSymbolicLink()
{
    if (!m_fi) {
        return False;
    }

    return m_fi->isSymbolicLink();
}

Bool File::isInRoot() const
{
    if (!m_fi) {
        return False;
    }

    return m_fi->isInRoot();
}

String File::makeRelative(const BaseDir &dir) const
{
    if (!m_fi) {
        return "";
    }

    return m_fi->makeRelative(dir);
}

FileLocation File::getFileLocation() const
{
    if (!m_fi) {
        return FL_UNKNOWN;
    }

    return m_fi->getFileLocation();
}

const String &File::getFileName() const
{
    if (!m_fi) {
        return String::getNull();
    }

    return m_fi->getFileName();
}

const String &File::getFullFileName() const
{
    if (!m_fi) {
        return String::getNull();
    }

    return m_fi->getFullFileName();
}

const String &File::getFileExt() const
{
    if (!m_fi) {
        return String::getNull();
    }

    return m_fi->getFileExt();
}

String File::getFilePath() const
{
    if (!m_fi) {
        return "";
    }

    return m_fi->getFilePath();
}

Bool File::operator==(const File &cmp) const
{
    if (!cmp.m_fi || !m_fi) {
        return False;
    }

    return *m_fi == *cmp.m_fi;
}

Bool File::operator!=(const File &cmp) const
{
    if (!cmp.m_fi || !m_fi) {
        return True;
    }

    return *m_fi != *cmp.m_fi;
}

Bool File::operator==(const BaseFile &cmp) const
{
    if (m_fi) {
        return False;
    }

    return *m_fi == cmp;
}

Bool File::operator!=(const BaseFile &cmp) const
{
    if (!m_fi) {
        return True;
    }

    return *m_fi != cmp;
}
