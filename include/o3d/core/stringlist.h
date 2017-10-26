/**
 * @file stringlist.h
 * @brief String list container.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
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

} // namespace o3d

#endif // _O3D_STRINGLIST_H

