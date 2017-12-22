/**
 * @file dir.cpp
 * @brief Implementation of dir.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-01-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"

#include "o3d/core/dir.h"
#include "o3d/core/debug.h"
#include "o3d/core/basedir.h"
#include "o3d/core/filemanager.h"

using namespace o3d;

Dir::Dir(const String &pathName)
{
    m_di = FileManager::instance()->dir(pathName);
}

Dir::Dir(const Dir &dup) :
    m_di(nullptr)
{
    if (dup.m_di) {
        m_di = dup.m_di->clone();
    }
}

Dir::Dir(BaseDir *own)
{
    m_di = own;
}

Dir::Dir(const BaseDir &dup)
{
    m_di = dup.clone();
}

Dir::~Dir()
{
    deletePtr(m_di);
}

Dir &Dir::operator=(const BaseDir &dup)
{
    deletePtr(m_di);

    m_di = dup.clone();
    return *this;
}

Dir &Dir::operator=(const Dir &dup)
{
    deletePtr(m_di);

    if (dup.m_di) {
        m_di = dup.m_di->clone();
    }

    return *this;
}

FileLocation Dir::getFileLocation() const
{
    if (!m_di) {
        return FL_UNKNOWN;
    }

    return m_di->getFileLocation();
}

void Dir::clean()
{
    if (m_di) {
        m_di->clean();
    }
}

Bool Dir::setPathName(const String &pathName)
{
    if (!m_di) {
        return False;
    }

    return m_di->setPathName(pathName);
}

const String &Dir::getPathName() const
{
    if (!m_di) {
        return String::getNull();
    }

    return m_di->getPathName();
}

const String &Dir::getFullPathName() const
{
    if (!m_di) {
        return String::getNull();
    }

    return m_di->getFullPathName();
}

String Dir::makeFullFileName(const String &filename) const
{
    if (!m_di) {
        return "";
    }

    return m_di->makeFullFileName(filename);
}

String Dir::makeFileName(const String &filename) const
{
    if (!m_di) {
        return "";
    }

    return m_di->makeFileName(filename);
}

String Dir::makeFullPathName(const String &pathName) const
{
    if (!m_di) {
        return "";
    }

    return m_di->makeFullPathName(pathName);
}

String Dir::makePathName(const String &pathName) const
{
    if (!m_di) {
        return "";
    }

    return m_di->makePathName(pathName);
}

String Dir::makeRelative(const String &filename) const
{
    if (!m_di) {
        return "";
    }

    return m_di->makeRelative(filename);
}

Bool Dir::isAbsolute() const
{
    if (!m_di) {
        return False;
    }

    return m_di->isAbsolute();
}

Bool Dir::isReadable() const
{
    if (!m_di) {
        return False;
    }

    return m_di->isReadable();
}

Bool Dir::isWritable() const
{
    if (!m_di) {
        return False;
    }

    return m_di->isWritable();
}

Bool Dir::isRoot() const
{
    if (!m_di) {
        return False;
    }

    return m_di->isRoot();
}

BaseDir::DirReturn Dir::removeDir(const String &path) const
{
    if (!m_di) {
        return BaseDir::INVALID_PATH;
    }

    return m_di->removeDir(path);
}

BaseDir::DirReturn Dir::makeDir(const String &path) const
{
    if (!m_di) {
        return BaseDir::INVALID_PATH;
    }

    return m_di->makeDir(path);
}

BaseDir::DirReturn Dir::makePath(const String &path) const
{
    if (!m_di) {
        return BaseDir::INVALID_PATH;
    }

    return m_di->makePath(path);
}

BaseDir::DirReturn Dir::check(const String &fileOrPath) const
{
    if (!m_di) {
        return BaseDir::INVALID_PATH;
    }

    return m_di->check(fileOrPath);
}

Bool Dir::exists() const
{
    if (!m_di) {
        return False;
    }

    return m_di->exists();
}

Bool Dir::empty() const
{
    if (!m_di) {
        return False;
    }

    return m_di->empty();
}

Bool Dir::cd(const String &path)
{
    if (!m_di) {
        return False;
    }

    return m_di->cd(path);
}

Bool Dir::cdUp()
{
    if (!m_di) {
        return False;
    }

    return m_di->cdUp();
}

T_FLItem_List Dir::findFilesInfos(const String &filters, FileTypes type) const
{
    if (!m_di) {
        return T_FLItem_List();
    }

    return m_di->findFilesInfos(filters, type);
}

T_StringList Dir::findFiles(const String &filters, FileTypes type) const
{
    if (!m_di) {
        return T_StringList();
    }

    return m_di->findFiles(filters, type);
}

Bool Dir::makeAbsolute()
{
    if (!m_di) {
        return False;
    }

    return m_di->makeAbsolute();
}

BaseDir::DirReturn Dir::removeFile(const String &filename) const
{
    if (!m_di) {
        return BaseDir::INVALID_PATH;
    }

    return m_di->removeFile(filename);
}

BaseDir::DirReturn Dir::rename(const String &oldName, const String &newName) const
{
    if (!m_di) {
        return BaseDir::INVALID_PATH;
    }

    return m_di->rename(oldName, newName);
}

BaseDir::DirReturn Dir::copyFile(const String &filename, UInt32 blockSize) const
{
    if (!m_di) {
        return BaseDir::INVALID_PATH;
    }

    return m_di->copyFile(filename, blockSize);
}

Bool Dir::operator==(const Dir &cmp) const
{
    if (!cmp.m_di || !m_di) {
        return False;
    }

    return *m_di == *cmp.m_di;
}

Bool Dir::operator!=(const Dir &cmp) const
{
    if (!cmp.m_di || !m_di) {
        return True;
    }

    return *m_di != *cmp.m_di;
}

Bool Dir::operator==(const BaseDir &cmp) const
{
    if (m_di) {
        return False;
    }

    return *m_di == cmp;
}

Bool Dir::operator!=(const BaseDir &cmp) const
{
    if (!m_di) {
        return True;
    }

    return *m_di != cmp;
}
