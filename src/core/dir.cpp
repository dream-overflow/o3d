/**
 * @file dir.cpp
 * @brief management.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-06-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/dir.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"
#include "o3d/core/file.h"
#include "o3d/core/debug.h"

using namespace o3d;

/*---------------------------------------------------------------------------------------
  default constructor
---------------------------------------------------------------------------------------*/
Dir::Dir(const String& pathname) :
	m_type(UNKNOWN_TYPE_DIR),
	m_isValid(False)
{
	if (pathname.isValid())
		setPathName(pathname);
	else
		m_fullPathname = ".";
}

String Dir::makeRelative(const String &filename) const
{
    if (!m_isValid)
        return String("");

    String lFilename(filename);
    lFilename.replace('\\','/');

    return
        File::convertPath(lFilename, m_fullPathname);
}

/*---------------------------------------------------------------------------------------
  set the full path name of this
---------------------------------------------------------------------------------------*/
Bool Dir::setPathName(const String &pathName)
{
	m_fullPathname = pathName;
	m_fullPathname.replace('\\','/');
	m_fullPathname.trimRight('/');

	Int32 pos = m_fullPathname.reverseFind('/');
	if (pos != -1)
		m_pathName = m_fullPathname.sub(pos+1);
	else
		m_pathName = m_fullPathname;

	m_isValid = True;

	/* Don't perform the existence check, because we could manage non existing dir
	if (!IsExist())
	{
		m_fullPathname = "";
		m_pathName = "";

		m_isValid = False;

		O3D_ERROR(O3D_E_InvalidParameter(pathName));
	}*/

	return m_isValid;
}

/*---------------------------------------------------------------------------------------
  change directory. return true if the new directory exists and it is readable
---------------------------------------------------------------------------------------*/
Bool Dir::cd(const String &path)
{
	if (!m_isValid)
		return False;

	// clear the given path
	String lPath(path);
	lPath.replace('\\','/');
	lPath.trimRight('/');

	if (lPath.sub("/",0) == 0)
		O3D_ERROR(E_InvalidResult(lPath + " is an invalid path"));

	// backup the current path and transform it
	String oldDir(m_fullPathname);
	if (m_fullPathname.isValid())
		m_fullPathname += '/';
	m_fullPathname += lPath;

	// adapt the path
	clean();

	// check if it exist and if it is readable
	if (!isExist())
	{
		m_fullPathname = oldDir;

		O3D_ERROR(E_InvalidParameter(lPath));
		return False;
	}

	Int32 pos = m_fullPathname.reverseFind('/');

	if (pos != -1)
		m_pathName = m_fullPathname.sub(pos+1);
	else
		m_pathName = m_fullPathname;

	return True;
}

// concat the filename to the directory full-path (do not check its existence)
String Dir::makeFullFileName(const String &filename)
{
	if (!m_isValid)
		return String("");

	String lFilename(filename);
	lFilename.replace('\\','/');
	lFilename.trimRight('/');

	if (m_fullPathname.isValid())
	{
		lFilename = m_fullPathname + '/' + lFilename;
		File::adaptPath(lFilename);
		return lFilename;
	}
	else
	{
		File::adaptPath(lFilename);
		return lFilename;
	}
}

// concat the filename to the directory relative path (do not check its existence)
String Dir::makeFileName(const String &filename)
{
	if (!m_isValid)
		return String("");

	String lFilename(filename);
	lFilename.replace('\\','/');
	lFilename.trimRight('/');

	if (m_pathName.isValid())
	{
		lFilename = m_pathName + '/' + lFilename;
		File::adaptPath(lFilename);
		return lFilename;
	}
	else
	{
		File::adaptPath(lFilename);
		return lFilename;
	}
}

// concat the pathname to the full-path (do not check its existence)
String Dir::makeFullPathName(const String &pathName)
{
	if (!m_isValid)
		return String("");

	String lPathName(pathName);
	lPathName.replace('\\','/');
	lPathName.trimRight('/');

	if (m_fullPathname.isValid())
	{
		lPathName = m_fullPathname + '/' + lPathName;
		File::adaptPath(lPathName);
		return lPathName;
	}
	else
	{
		File::adaptPath(lPathName);
		return lPathName;
	}
}

// concat the pathname to the relative path (do not check its existence)
String Dir::makePathName(const String &pathName)
{
	if (!m_isValid)
		return String("");

	String lPathName(pathName);
	lPathName.replace('\\','/');
	lPathName.trimRight('/');

	if (m_pathName.isValid())
	{
		lPathName = m_pathName + '/' + lPathName;
		File::adaptPath(lPathName);
		return lPathName;
	}
	else
	{
		File::adaptPath(lPathName);
		return lPathName;
	}
}

