/**
 * @file fileinfo.cpp
 * @brief Implementation of FileInfo.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-06-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/basefileinfo.h"

using namespace o3d;

BaseFileInfo::BaseFileInfo(const String &filename) :
    m_type(UNKNOWN_FILE_INFO),
	m_cached(False),
	m_isValid(False)
{
	m_fullFilename = filename;
	m_fullFilename.replace('\\','/');
	m_fullFilename.trimRight('/');

	Int32 pos = m_fullFilename.reverseFind('/');
    if (pos != -1) {
		m_filename = m_fullFilename.sub(pos+1);
    } else {
        // It means there is no directory
		m_filename = filename;
    }

	pos = m_filename.find('.');
    if ((pos != -1) && (pos != 0)) {
		m_fileExt = m_filename.sub(pos+1);
    }

    if (pos == 0) {
		m_cachedData.hidden = True;
    } else {
		m_cachedData.hidden = False;
    }
}

BaseFileInfo::BaseFileInfo(const String &pathName, const String &filename) :
    m_type(UNKNOWN_FILE_INFO),
	m_cached(False),
	m_isValid(False)
{
	String lPathName(pathName);
	lPathName.replace('\\','/');
	lPathName.trimRight('/');

	String lFilename(filename);
	lFilename.replace('\\','/');
	lFilename.trimRight('/');

	m_fullFilename = lPathName + '/' + lFilename;
	m_filename = lFilename;

	Int32 pos = m_filename.find('.');
    if ((pos != -1) && (pos != 0)) {
		m_fileExt = m_filename.sub(pos+1);
    }

    if (pos == 0) {
		m_cachedData.hidden = True;
    } else {
		m_cachedData.hidden = False;
    }
}

BaseFileInfo::BaseFileInfo(const BaseDir &dir, const String &filename) :
    m_type(UNKNOWN_FILE_INFO),
	m_cached(False),
	m_isValid(False)
{
	String lFilename(filename);
	lFilename.replace('\\','/');
	lFilename.trimRight('/');

	m_fullFilename = dir.getFullPathName() + '/' + lFilename;
	m_filename = lFilename;

	Int32 pos = m_filename.find('.');
    if ((pos != -1) && (pos != 0)) {
		m_fileExt = m_filename.sub(pos+1);
    }

    if (pos == 0) {
		m_cachedData.hidden = True;
    } else {
		m_cachedData.hidden = False;
    }
}

BaseFileInfo::BaseFileInfo(const BaseFileInfo &dup) :
	m_type(dup.m_type),
	m_cached(dup.m_cached),
	m_isValid(dup.m_isValid),
	m_filename(dup.m_filename),
	m_fileExt(dup.m_fileExt),
	m_fullFilename(dup.m_fullFilename),
	m_cachedData(dup.m_cachedData)
{
}

void BaseFileInfo::setFile(const String &filename)
{
	m_cached = False;
	m_isValid = False;

	m_fullFilename = filename;
	m_fullFilename.replace('\\','/');
	m_fullFilename.trimRight('/');

	Int32 pos = m_fullFilename.reverseFind('/');
    if (pos != -1) {
		m_filename = m_fullFilename.sub(pos+1);
    } else {
		m_filename = m_fullFilename;
    }

	pos = m_filename.find('.');
    if ((pos != -1) && (pos != 0)) {
		m_fileExt = m_filename.sub(pos+1);
    }

    if (pos == 0) {
		m_cachedData.hidden = True;
    } else {
		m_cachedData.hidden = False;
    }
}

void BaseFileInfo::setFile(const String &pathName, const String &filename)
{
	String lPathName(pathName);
	lPathName.replace('\\','/');
	lPathName.trimRight('/');

	String lFilename(filename);
	lFilename.replace('\\','/');
	lFilename.trimRight('/');

	m_cached = False;
	m_isValid = False;

	m_fullFilename = lPathName + '/' + lFilename;
	m_filename = lFilename;

	Int32 pos = m_filename.find('.');
    if ((pos != -1) && (pos != 0)) {
		m_fileExt = m_filename.sub(pos+1);
    }

    if (pos == 0) {
		m_cachedData.hidden = True;
    } else {
		m_cachedData.hidden = False;
    }
}

void BaseFileInfo::setFile(const BaseDir &dir, const String &filename)
{
	String lFilename(filename);
	lFilename.replace('\\','/');
	lFilename.trimRight('/');

	m_cached = False;
	m_isValid = False;

	m_fullFilename = dir.getFullPathName() + '/' + lFilename;
	m_filename = lFilename;

	Int32 pos = m_filename.find('.');
    if ((pos != -1) && (pos != 0)) {
		m_fileExt = m_filename.sub(pos+1);
    }

    if (pos == 0) {
		m_cachedData.hidden = True;
    } else {
		m_cachedData.hidden = False;
    }
}
