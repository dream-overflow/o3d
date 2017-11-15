/**
 * @file blockpooler.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_BLOCKPOOLER_H
#define _O3D_BLOCKPOOLER_H

#include "objective3dconfig.h"

#define O3D_B_MALLOC(_S)     ::malloc((_S))
#define O3D_B_FREE(_P)       ::free((_P))

#ifdef O3D_WINDOWS
	#define O3D_B_ALIGNED_MALLOC(_S,_A)     _mm_malloc((_S),(_A))
	#define O3D_B_ALIGNED_FREE(_P)          _mm_free((_P))
#else // suppose POSIX
	#define O3D_B_ALIGNED_MALLOC(_S,_A)     o3d::alignedMalloc((_S),(_A))
	#define O3D_B_ALIGNED_FREE(_P)          o3d::alignedFree((_P))
#endif

#ifdef new
	#undef new
	#undef delete
#endif // new

namespace o3d {

/**
 * @brief An optimized allocator for fixed size block allocation.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-11-03
 * Fast memory pooler for fixed block allocation. Take care this file is used only
 * in inline, and than the memory manager is not present here.
 *
 * All allocations are aligned to @em Align bytes.
 * Multi-threading should be managed by the user.
 *
 * When it require to alloc an array of POD object, it is a better thing to use
 * AtomicArray which allocate very quickly the array of object, and set the
 * data content without using constructor, neither destructor by default.
 */
template <size_t EltSize, size_t Align, size_t EltSizePow2>
class O3D_API_TEMPLATE BlockPooler : NonCopyable<>
{
public:

	//! Default constructor
	//! @param numBlock number of allocated element for a chunk (max is 65535)
	BlockPooler(Int32 numBlock) :
		m_numBlock(numBlock),
        m_heap(nullptr),
        m_stacks(nullptr)
	{
		O3D_ASSERT(numBlock <= 65536);

		// the first chunk
		m_stacks = new Chunk((Int32)m_numBlock);
        m_stacks->m_previous = nullptr;
        m_stacks->m_next = nullptr;

		m_heap = m_stacks;
	}

	//! Destructor
	~BlockPooler()
	{
		Chunk *current = m_stacks;
		while (current)
		{
			Chunk *next = current->m_next;
			delete current;
			current = next;
		}
	}

	//! Allocate an element at the last free chunk
	inline void* allocate()
	{
		Chunk *current = m_stacks;
		while (current)
		{
			if (current->isFreeSpace())
				return current->allocate();

			current = current->m_next;
		}

		// need one more chunk
		current = newChunk();
		return current->allocate();
	}

	//! Free an element
	inline void free(void *ptr)
	{
		// search for the chunk that contain it
		// the mostly it is contained into the heap
		Chunk *current = m_heap;
		while (current)
		{
			if (current->isOwner(ptr))
			{
				current->free(ptr);

				// delete the chunk if it is empty and if it is not the front one
				if ((current != m_stacks) && current->isEmpty())
					deleteChunk(current);

				return;
			}

			current = current->m_previous;
		}
	}

private:

	//! A chunk of a memory pool. We cannot process some realloc so we
	//! add more chunks as necessary.
	struct Chunk
	{
		Chunk *m_previous;    //!< doubly linked list previous elt
		Chunk *m_next;        //!< doubly linked list next elt

		UInt8 *m_stack;         //!< The stack itself
		UInt8 *m_stackEnd;      //!< Stack ptr + its size
		UInt16 *m_lookupTable;  //!< A linked list of next free element

		UInt32 m_curPos;   //!< current position into the lookup table (in elt position)
		UInt32 m_numElt;   //!< number of block used

		//! Initialization constructor.
		//! @param maxChunkNbElt Number of allocated element for a chunk
		//! @param extraChunkNbElt number of extra allocations (in element number),
		//! a safe area unable to allocate but useful for some optimizations
		Chunk(Int32 numBlock) :
            m_previous(nullptr),
            m_next(nullptr),
            m_stack(nullptr),
            m_stackEnd(nullptr),
            m_lookupTable(nullptr),
			m_curPos(0),
			m_numElt(0)
		{
			m_stack = (UInt8*)O3D_B_ALIGNED_MALLOC(numBlock << EltSizePow2, Align);
			m_lookupTable = (UInt16*)O3D_B_MALLOC(numBlock*sizeof(UInt16));

			m_stackEnd = m_stack + (numBlock << EltSizePow2);

			// initialize the lookup table (this is the slowest stape of the algorithm)
			for (Int32 i = 0; i < numBlock-1; ++i)
			{
				m_lookupTable[i] = (UInt16)(i+1);
			}

			m_lookupTable[numBlock-1] = o3d::Limits<UInt16>::max();
		}

		// Destructor
		~Chunk()
		{
			O3D_B_ALIGNED_FREE(m_stack);
			O3D_B_FREE(m_lookupTable);
		}

		//! Is free space
		inline Bool isFreeSpace() const { return m_curPos != o3d::Limits<UInt16>::max(); }

		//! Is totaly free
		inline Bool isEmpty() const	{ return (m_numElt == 0); }

		//! Allocate an element at the last free position
		//! @precondition It assume for available free space
		inline void* allocate()
		{
			// get a valid ptr
			void* ptr = (void*)&m_stack[m_curPos << EltSizePow2];

			// if more free space is avalaible
			if (isFreeSpace())
			{
				// set the cursor to the next valid (free) element
				m_curPos = m_lookupTable[m_curPos];
				++m_numElt;
			}
			else
			{
				m_curPos = o3d::Limits<UInt16>::max();
			}

			return ptr;
		}

		//! Free an element
		inline void free(void* ptr)
		{
			// next free position
			UInt32 nextPos = (UInt32)((UInt8*)ptr - &m_stack[0]) >> EltSizePow2;
			m_lookupTable[nextPos] = (UInt16)m_curPos;
			m_curPos = nextPos;
			--m_numElt;
		}

		//! Check for an element existence
		inline Bool isOwner(void *ptr) const
		{
			return ((ptr >= m_stack) && (ptr < m_stackEnd));
		}
	};

	//! Create a new chunk at heap and return it
	inline Chunk* newChunk()
	{
		Chunk *previous = m_heap;

		m_heap = new Chunk((Int32)m_numBlock);
		m_heap->m_previous = previous;
        m_heap->m_next = nullptr;

		previous->m_next = m_heap;

		return m_heap;
	}

	//! Delete a chunk
	inline void deleteChunk(Chunk *who)
	{
		O3D_ASSERT(who);

		if (who == m_heap)
			m_heap = who->m_previous;

		if (who->m_previous)
			who->m_previous->m_next = who->m_next;

		if (who->m_next)
			who->m_next->m_previous = who->m_previous;

		delete who;
	}

	size_t m_numBlock;   //!< The maximal number of elements that can contain of chunk

	Chunk *m_heap;       //!< heap of the link list
	Chunk *m_stacks;     //!< begin of a doubly linked list of chunks
};

} // namespace o3d

#endif // _O3D_BLOCKPOOLER_H

