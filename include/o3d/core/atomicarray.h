/**
 * @file atomicarray.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ATOMICARRAY_H
#define _O3D_ATOMICARRAY_H

#include "o3d/core/memorydbg.h"
#include "o3d/core/templatearray.h"

namespace o3d {

/**
 * @brief Atomic array container, aligned for POD like objects.
 * @date 2007-12-03
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @details Atomic array container. 16 bytes data aligned. POD objects content are adjacent to
 * each other. Useful for optimization and to avoid some copy.
 * ContentSize must be in pow of 2 only. If your content take 3 bytes, so it must
 * take 4, so set ContentSize to 2.
 * Object specification :
 * each atomic class must define this (inline) method to set their data content
 * properly. Take care of the data alignment. This method must be public.
 * inline void _InitAtomicObject(void *ptr) { members = ptr; ...init like I need }
 */
template <class T, class U, UInt32 ContentSize>
class O3D_API_TEMPLATE AtomicArray
{
public:

	//! default constructor (no elements)
	AtomicArray() :
		m_objects(0,0),
		m_content(0,0)
	{}

	//! Initialization constructor
	//! @param NbElt Number of matrix to allocate
	AtomicArray(UInt32 nbElt) :
		m_objects(nbElt,0xffffffff),
		m_content(nbElt<<ContentSize,0xffffffff)
	{
		O3D_ASSERT(nbElt != 0);

		m_objects.forceSize(nbElt);
		m_content.forceSize(nbElt<<ContentSize);

		//! Initialize each object data ptr. Like a new.
		for (UInt32 i = 0; i < nbElt; ++i)
			m_objects[i]._initAtomicObject(&m_content[i<<ContentSize]);
	}

	//! Return the number of element of type T
	inline Int32 getSize() const { return m_objects.getSize(); }

	//! Access to an object
	inline       T& operator[](UInt32 i)        { return m_objects[i]; }
	//! access to an object (const version)
	inline const T& operator[](UInt32 i) const  { return m_objects[i]; }

	//! Access to an object
	inline       T* get(UInt32 i)        { return &m_objects[i]; }
	//! access to an object (const version)
	inline const T* get(UInt32 i) const  { return &m_objects[i]; }

	//! Direct access to the array of data
	inline       U* getData()       { return (U*)&m_content[0]; }
	//! direct access to the array of data (const version)
	inline const U* getData() const { return (U*)&m_content[0]; }

	//! Direct access to the a content data
	inline       U* getContent(UInt32 i)       { return (U*)&m_content[i<<ContentSize]; }
	//! direct access to the a content data (const version)
	inline const U* getContent(UInt32 i) const { return (U*)&m_content[i<<ContentSize]; }

	//! Resize the array
	void resize(UInt32 newNbElt)
	{
		O3D_ASSERT(newNbElt != 0);

		m_objects.forceMaxSize(newNbElt);
		m_content.forceMaxSize(newNbElt<<ContentSize);

		m_objects.forceSize(newNbElt);
		m_content.forceSize(newNbElt<<ContentSize);

		//! Initialize each object data ptr. Like a new.
		for (UInt32 i = 0; i < newNbElt; ++i)
			m_objects[i]._initAtomicObject(&m_content[i<<ContentSize]);
	}

private:

	TemplateArray<T, 16>     m_objects;  //!< Contain the object.
	TemplateArray<UInt8, 16> m_content;  //!< Contain the data.
};

}

#endif // _O3D_ATOMICARRAY_H

