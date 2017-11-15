/**
 * @file stringmap.h
 * @brief String list container.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-08-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_STRINGMAP_H
#define _O3D_STRINGMAP_H

#include "string.h"
#include <unordered_map>

namespace o3d {

template <class V>
class StringMap : public std::unordered_map<String, V, std::hash<String> >
{
public:

	typedef typename std::unordered_map<String, V, std::hash<String> >::iterator IT;
	typedef typename std::unordered_map<String, V, std::hash<String> >::const_iterator CIT;
};

} // namespace o3d

#endif // _O3D_STRINGMAP_H

