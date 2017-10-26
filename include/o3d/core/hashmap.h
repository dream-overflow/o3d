/**
 * @file hashmap.h
 * @brief Architecture adaptation for standard C++ hashmap defined by "stdext::hash_map"
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-04-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_HASHMAP_H
#define _O3D_HASHMAP_H

#ifdef _MSC_VER
	#include <hash_map>
#else
	#if (__GNUC__ >= 4) && (__GNUC_MINOR__ >= 1)
		#include <tr1/unordered_map>
		namespace stdext = ::std::tr1;
		#define hash_map unordered_map
	#else
		#error "GCC >= 4.1 should be used"
	#endif
#endif

#endif // _O3D_HASHMAP_H

