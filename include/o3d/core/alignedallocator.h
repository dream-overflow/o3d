/**
 * @file alignedallocator.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ALIGNEDALLOCATOR_H
#define _O3D_ALIGNEDALLOCATOR_H

#include "memorydbg.h"

namespace o3d {

/**
 * @brief Aligned allocator STL compatible.
 * @date 2008-07-14
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
template <typename T, unsigned Alignment = 0>
class O3D_API AlignedAllocator
{
	//! Compile time check alignment is available
	typedef int isValidAlignment[Alignment <= 128 && ((Alignment & (Alignment-1)) == 0) ? +1 : -1];

public:

	//-----------------------------------------------------------------------------------
	// Typedefs for STL compatibility
	//-----------------------------------------------------------------------------------

	typedef T value_type;
	typedef value_type *pointer;
	typedef const value_type *const_pointer;
	typedef value_type &reference;
	typedef const value_type &const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	template <typename U>
	struct rebind
	{
		typedef AlignedAllocator<U, Alignment> other;
	};

public:

	//! Default constructor
	AlignedAllocator() {}

	// Copy constructor from another aligned allocator
	template <typename U, unsigned A>
	AlignedAllocator(const AlignedAllocator<U, A> &) {}


	//-----------------------------------------------------------------------------------
	// Functions for STL compatible
	//-----------------------------------------------------------------------------------

	static pointer address(reference r) { return &r; }
	static const_pointer address(const_reference s) { return &s; }
	static size_type max_size() { return (std::numeric_limits<size_type>::max)(); }
	static void construct(const pointer ptr, const value_type &t) { new (ptr) T(t); }
	static void destroy(const pointer ptr)
	{
		ptr->~T();
		(void) ptr; // avoid unused variable warning
	}

	bool operator==(const AlignedAllocator &) const { return true; }
	bool operator!=(const AlignedAllocator &) const { return false; }

	static pointer allocate(const size_type n)
	{
		// use default platform dependent alignment if 'Alignment' equal to zero.
		const pointer ret = static_cast<pointer>(Alignment ?
			O3D_ALIGNED_MALLOC(sizeof(T) * n, Alignment) :
			O3D_MALLOC(sizeof(T) * n));

		return ret;
	}

	inline static pointer allocate(const size_type n, const void * const)
	{
		return allocate(n);
	}

	inline static void deallocate(const pointer ptr, const size_type)
	{
		Alignment ? O3D_ALIGNED_FREE(ptr) : O3D_FREE(ptr);
	}
};

} // namespace o3d

#endif // _O3D_ALIGNEDALLOCATOR_H

