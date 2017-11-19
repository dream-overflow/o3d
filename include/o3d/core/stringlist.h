/**
 * @file stringlist.h
 * @brief String list container.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-08-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_STRINGLIST_H
#define _O3D_STRINGLIST_H

#include "string.h"
#include <list>

namespace o3d {

typedef std::list<String> T_StringList;
typedef T_StringList::iterator IT_StringList;
typedef T_StringList::const_iterator CIT_StringList;

/**
 * @brief Split the string according to a list of characters.
 * @param chars String containings one ore many characters as separators.
 * @return List of String.
 */
T_StringList split(const String &content, const String &delimiters);

} // namespace o3d

#endif // _O3D_STRINGLIST_H
