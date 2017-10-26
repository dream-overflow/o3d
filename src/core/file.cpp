/**
 * @file file.cpp
 * @brief Implementation of File.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2002-01-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include "o3d/core/architecture.h"
#include "o3d/core/serialize.h"
#include "o3d/core/file.h"
#include "o3d/core/debug.h"
#include "o3d/core/stringtokenizer.h"

using namespace o3d;

Serialize::~Serialize()
{

}

// when a path contain some '..' or '.' this method reform the path without '..' and '.'
void File::adaptPath(String& path)
{
	Int32 pos,pathToDelPos;

	path.replace('\\','/');
	path.trimRight('/');

	// add a trailing slash to simplify our search
	path += '/';

	// delete the '.'
    while ((pos = path.sub("./",0)) > 1)
	{
		path.remove(pos,2);
	}

	// remove the last directory
    while ((pos = path.sub("../",0)) > 1)
	{
		pathToDelPos = path.find('/',pos-2,True);
		path.remove(pathToDelPos,pos+2-pathToDelPos);
	}

  	// no trailing slash
  	path.trimRight('/');
}

// adapt an absolute path of a file to another absolute path. ex:
String File::convertPath(const String &filename, const String &pathName)
{
	String result;
	StringTokenizer lPath(filename, L"/\\");
	StringTokenizer lPathCount(pathName, L"/\\");

	while (lPath.hasMoreTokens())
	{
		String cur = lPath.nextToken();
		lPathCount.nextToken();

		if (pathName.sub(cur,0) == -1)
		{
			result += cur;

            if (lPath.hasMoreTokens())
			{
                if (lPathCount.hasMoreTokens())
					result.insert("../",0);

				result += '/';
			}
		}
	}

	// no trailing slash
	result.trimRight('/');
	return result;
}

// Get the file name and the path name from a complete (full) file name.
void File::getFileNameAndPath(
		const String &fullFileName,
		String &filename,
		String &pathname)
{
	Int32 p = fullFileName.reverseFind('/');
	if (p == -1)
	{
		pathname = "";
		filename = fullFileName;
	}
	else
	{
		pathname = fullFileName;
		pathname.truncate(p);
		filename = fullFileName.sub(p+1);
    }
}

