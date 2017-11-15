/**
 * @file memorymanager.cpp
 * @brief Implementation of MemoryManager.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2004-12-13
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/memorymanager.h"

#include "o3d/core/thread.h"
#include "o3d/core/debug.h"
#include "o3d/core/objects.h"
#include "o3d/core/memory.h"

#include <iomanip>
#include <memory.h>
#include <stdlib.h>

using namespace o3d;

#ifdef O3D_WINDOWS
	#define O3D_GET_FILENAME                               \
		size_t fileNamePos = 0, i = 0;                     \
		while (file[i] != 0) {                             \
			if ((file[i] == '\\') || (file[i] == '/'))     \
				fileNamePos = i+1;                         \
		++i;                                               \
		}
#else
	#define O3D_GET_FILENAME                    \
		size_t fileNamePos = 0, i = 0;          \
		while (file[i] != 0) {                  \
			if (file[i] == '/')                 \
				fileNamePos = i+1;              \
		++i;                                    \
		}
#endif

static FastMutex O3D_MemoryManagerMutex;

// time measurement
static TimeCounter O3D_MemoryManagerTime[MemoryManager::NUM_MEMORY_TYPE] = { 2.f, 2.f, 2.f, 2.f };

// time measurement
static TimeCounter O3D_MemoryManagerTimeCounter[MemoryManager::NUM_MEMORY_TYPE] = { 1.f, 1.f, 1.f, 1.f };

// Conversion from graphic target to string.
const Char* MemoryManager::gfxType2String(GraphicTarget gtype)
{
	switch (gtype)
	{
	case GPU_TEXTURE_1D:
		return "T1D";
	case GPU_TEXTURE_2D:
		return "T2D";
	case GPU_TEXTURE_3D:
		return "T3D";
	case GPU_TEXTURE_CUBEMAP:
		return "TCM";
	case GPU_TEXTURE_1D_ARRAY:
		return "TA1";
	case GPU_TEXTURE_2D_ARRAY:
		return "TA2";
    case GPU_TEXTURE_2D_MULTISAMPLE:
        return "TMS";
    case GPU_TEXTURE_2D_MULTISAMPLE_ARRAY:
        return "TMA";
	case MemoryManager::GPU_VBO:
		return "VBO";
	case MemoryManager::GPU_FBO:
		return "FBO";
	case MemoryManager::GPU_PBO:
		return "PBO";
	default:
		return "G??";
	}
}

// Conversion from sound target to string.
const Char* MemoryManager::sfxType2String(AudioTarget stype)
{
	switch (stype)
	{
	case SFX_SINGLE_BUFFER:   // Single sound buffer
		return "SBO";
	case SFX_STREAM_BUFFER:   // Stream sound buffer
		return "SSB";
	default:
		return "S??";
	}
}

// Constructor
MemoryManager::MemoryManager() :
		m_file("memory.log")
{
	// is file open ?
    if (!m_file)
	{
		O3D_ERROR(E_FileNotFoundOrInvalidRights("","memory.log"));
	}

    // write header file
    m_file << "-------------------------------------- Objective-3D --------------------------------------" << std::endl;
	m_file << "-                                                                                        -" << std::endl;
    m_file << "-                                   Memory leak tracker                                  -" << std::endl;
    m_file << "------------------------------------------------------------------------------------------" << std::endl << std::endl;

    m_file << "++ mean central memory allocation (malloc, new, new [])" << std::endl;
	m_file << "-- mean central memory free (free, delete, delete [])" << std::endl;
    m_file << "** mean central memory re-allocation (realloc)" << std::endl << std::endl;

	m_file << "+G mean graphical memory allocation" << std::endl;
	m_file << "-G mean graphical memory free" << std::endl;
	m_file << "*G mean graphical memory re-allocation" << std::endl << std::endl;

	m_file << "+A mean audio memory allocation" << std::endl;
	m_file << "-A mean audio memory free" << std::endl;
	m_file << "*A mean audio memory re-allocation" << std::endl << std::endl;
}

// Destructor
MemoryManager::~MemoryManager()
{
	// Memory total allocated size
	m_file << std::endl;
	m_file << "A total of " << m_memory[MEM_RAM].m_memorytotalbytes << " bytes has been allocated in central memory" << std::endl;
	m_file << "A total of " << m_memory[MEM_GFX].m_memorytotalbytes << " bytes has been allocated in graphical memory" << std::endl;
	m_file << "A total of " << m_memory[MEM_SFX].m_memorytotalbytes << " bytes has been allocated in audio memory" << std::endl;

	if (m_CBlocks.empty() && m_GBlocks.empty())
	{
		// No leak !
		m_file << std::endl;
		m_file << "------------------------------------------------------------------------------------------" << std::endl;
		m_file << "-                          No leak detected, congratulations !                           -" << std::endl;
		m_file << "------------------------------------------------------------------------------------------" << std::endl;
	}
	else
	{
		// Leaks !!
		m_file << std::endl;
		m_file << "------------------------------------------------------------------------------------------" << std::endl;
		m_file << "-                             Memory leaks detected, bad !                               -" << std::endl;
		m_file << "------------------------------------------------------------------------------------------" << std::endl;
		m_file << std::endl;

		reportLeaks();
	}
}

// get class instance (singleton)
MemoryManager* MemoryManager::instance()
{
	static MemoryManager Inst;
	O3D_MemoryManagerMutex.lock();
	return &Inst;
}

// write memory leak report
void MemoryManager::reportLeaks()
{
	size_t TotalSize = 0;

	//-------------------------------------
	// Firstly we have central memory leaks
	//-------------------------------------

	m_file << std::endl;
	m_file << "------------------------------------------------------------------------------------------" << std::endl;
	m_file << "-                                  Central Memory Leaks                                  -" << std::endl;
	m_file << "------------------------------------------------------------------------------------------" << std::endl;
	m_file << std::endl;

	for (TBlockMap::iterator i = m_CBlocks.begin(); i != m_CBlocks.end(); ++i)
	{
		// add memory size
		TotalSize += i->second.Size;

		// write block information in file
		m_file << "-> 0x" << i->first
			   << " | "   << std::setw(7) << std::setfill(' ') << (Int32)(i->second.Size) << " bytes"
			   << " | "   << i->second.File << " (" << i->second.Line << ")" << std::endl;

        // free memory, commented because memory zone can be corrupted at this time
        //free(i->first);
    }

    // draw all memory leak sum
    m_file << std::endl << std::endl << "-- "
           << (Int32)(m_CBlocks.size()) << " none free block(s), "
           << (Int32)(TotalSize) << " bytes --"
           << std::endl;

	//------------------------------------
	// Next we have graphical memory leaks
	//------------------------------------

	TotalSize = 0;

	m_file << std::endl;
	m_file << "------------------------------------------------------------------------------------------" << std::endl;
	m_file << "-                                 Graphical Memory Leaks                                 -" << std::endl;
	m_file << "------------------------------------------------------------------------------------------" << std::endl;
	m_file << std::endl;

	for (TGBlockMap::iterator i = m_GBlocks.begin(); i != m_GBlocks.end(); ++i)
	{
		// add memory size
		TotalSize += i->second.Size;

		// write block information in file
		m_file << "-> " << std::setw(2) << std::setfill(' ') << gfxType2String(i->first.Target)
			   << ":" << std::setw(6) << std::setfill(' ') << i->first.Id
			   << " | " << std::setw(7) << std::setfill(' ') << (Int32)(i->second.Size) << " bytes"
			   << " | " << i->second.File << " (" << i->second.Line << ")" << std::endl;
    }

    // draw all memory leak sum
    m_file << std::endl << std::endl << "-- "
           << (Int32)(m_GBlocks.size()) << " none free block(s), "
           << (Int32)(TotalSize) << " bytes --"
           << std::endl;

	//--------------------------------
	// Next we have sound memory leaks
	//--------------------------------

	TotalSize = 0;

	m_file << std::endl;
	m_file << "------------------------------------------------------------------------------------------" << std::endl;
	m_file << "-                                   Audio Memory Leaks                                   -" << std::endl;
	m_file << "------------------------------------------------------------------------------------------" << std::endl;
	m_file << std::endl;

	for (TSBlockMap::iterator i = m_SBlocks.begin(); i != m_SBlocks.end(); ++i)
	{
		// add memory size
		TotalSize += i->second.Size;

		// write block information in file
		m_file << "-> " << std::setw(2) << std::setfill(' ') << sfxType2String(i->first.Target)
			   << ":" << std::setw(6) << std::setfill(' ') << i->first.Id
			   << " | " << std::setw(7) << std::setfill(' ') << (Int32)(i->second.Size) << " bytes"
			   << " | " << i->second.File << " (" << i->second.Line << ")" << std::endl;
    }

    // draw all memory leak sum
    m_file << std::endl << std::endl << "-- "
           << (Int32)(m_SBlocks.size()) << " none free block(s), "
           << (Int32)(TotalSize) << " bytes --"
           << std::endl;

	//------------------------------------
	// Next we have fast pool memory leaks
	//------------------------------------

	m_fastMemoryPool.reportLeaks(m_file);

	// footer
	m_file << std::endl;
	m_file << "------------------------------------------------------------------------------------------" << std::endl;
	m_file << "-                                       Ending Banner                                    -" << std::endl;
	m_file << "------------------------------------------------------------------------------------------" << std::endl;
	m_file << std::endl;
}


//---------------------------------------------------------------------------------------
// Central Memory Operations
//---------------------------------------------------------------------------------------

// add a memory allocation
void* MemoryManager::allocate(
	size_t Size,
	const Char *file,
	Int32 line,
	Bool array)
{
	// Allocate memory
	void* ptr = malloc(Size);

	// get only file name
	O3D_GET_FILENAME

	// Ajout du bloc à la liste des blocs alloués
	TBlock NewBlock;
	NewBlock.Size  = Size;
	NewBlock.Align = 0;
	NewBlock.File  = &file[fileNamePos];
	NewBlock.Line  = line;
	NewBlock.Array = array;
	m_CBlocks[ptr] = NewBlock;

	m_memory[MEM_RAM].m_memorytotalbytes += (UInt32)Size;
	m_memory[MEM_RAM].m_currentbytes += (UInt32)Size;

	if (m_memory[MEM_RAM].m_currentbytes > m_memory[MEM_RAM].m_maxpeak)
		m_memory[MEM_RAM].m_maxpeak = m_memory[MEM_RAM].m_currentbytes;

	++m_memory[MEM_RAM].m_nbrNew;

	// serialisation
	if (m_memory[MEM_RAM].m_logging && (Size >= m_memory[MEM_RAM].m_minLogSize))
	{
		m_file << "++ Allocation    | 0x" << ptr
			   << " | " << std::setw(7) << std::setfill(' ') << (Int32)(NewBlock.Size) << " bytes"
			   << " | " << NewBlock.File << " (" << NewBlock.Line << ")" << std::endl;
	}

	O3D_MemoryManagerMutex.unlock();
	return ptr;
}

//! aligned memory allocation
void* MemoryManager::alignedAllocate(
	size_t size,
	size_t align,
	const Char* file,
	Int32 line)
{
	O3D_ASSERT(align != 0);

	// Allocation de la memoire
#ifdef O3D_WINDOWS
	void* ptr = _mm_malloc(size,align);
#else
	void* ptr = o3d::alignedMalloc(size,align);
#endif

	// get only file name
	O3D_GET_FILENAME

	// Ajout du bloc à la liste des blocs alloués
	TBlock NewBlock;
	NewBlock.Size  = size;
	NewBlock.Align = align;
	NewBlock.File  = &file[fileNamePos];
	NewBlock.Line  = line;
	NewBlock.Array = False;
	m_CBlocks[ptr] = NewBlock;

	// extra bytes used by the alignement are not count
	m_memory[MEM_RAM].m_memorytotalbytes += (UInt32)size;
	m_memory[MEM_RAM].m_currentbytes += (UInt32)size;

	if (m_memory[MEM_RAM].m_currentbytes > m_memory[MEM_RAM].m_maxpeak)
		m_memory[MEM_RAM].m_maxpeak = m_memory[MEM_RAM].m_currentbytes;

	++m_memory[MEM_RAM].m_nbrNew;

	// serialisation
	if (m_memory[MEM_RAM].m_logging && (size >= m_memory[MEM_RAM].m_minLogSize))
	{
		m_file << "++ MMAllocation  | 0x" << ptr
			   << " | " << std::setw(7) << std::setfill(' ') << (Int32)(NewBlock.Size) << " bytes"
			   << " | " << NewBlock.File << " (" << NewBlock.Line << ")" << std::endl;
	}

	O3D_MemoryManagerMutex.unlock();
	return ptr;
}

// reallocate a block of memory
void* MemoryManager::reallocate(
	void* ptr,
	size_t size,
	const Char* file,
	Int32 line)
{
	// get the old memory block in the map
	TBlockMap::iterator It = m_CBlocks.find(ptr);

	// align check
	if (It != m_CBlocks.end())
	{
		if (It->second.Align != 0)
		{
			O3D_MemoryManagerMutex.unlock();
			O3D_ERROR(E_InvalidOperation("Aligned pointer require aligned realloc"));
			return NULL;
		}
	}

	// ReAllocation de la mémoire
	void* newptr = realloc(ptr,size);

	// get only file name
	O3D_GET_FILENAME

	TBlock NewBlock;
	NewBlock.Size  = size;
	NewBlock.Align = 0;
	NewBlock.File  = &file[fileNamePos];
	NewBlock.Line  = line;
	NewBlock.Array = False;

	m_memory[MEM_RAM].m_memorytotalbytes += (UInt32)size;
	m_memory[MEM_RAM].m_currentbytes += (UInt32)size;
	++m_memory[MEM_RAM].m_nbrNew;

	// unallocated block ? error
	if (It == m_CBlocks.end())
	{
		O3D_MemoryManagerMutex.unlock();
		m_file << "!! Reall.BADBLOCK| 0x" << ptr << std::endl;
		return newptr;
	}

	// Same base ptr
	if (ptr == newptr)
	{
		It->second.Size = NewBlock.Size;
		It->second.File = NewBlock.File;
		It->second.Line = NewBlock.Line;
	}

	m_memory[MEM_RAM].m_currentbytes -= (UInt32)It->second.Size;
	++m_memory[MEM_RAM].m_nbrDelete;

	if (m_memory[MEM_RAM].m_currentbytes > m_memory[MEM_RAM].m_maxpeak)
		m_memory[MEM_RAM].m_maxpeak = m_memory[MEM_RAM].m_currentbytes;

	// serialisation
	if (m_memory[MEM_RAM].m_logging &&
			(size >= m_memory[MEM_RAM].m_minLogSize) &&
			(m_memory[MEM_RAM].m_deleteStack.size() != 0))
	{
		m_file << "** Reallocate    | 0x" << ptr
			<< " > 0x" << std::setw(7) << std::setfill(' ') << newptr << std::setw(7)
			<< std::setfill(' ') << (Int32)(NewBlock.Size) << " bytes"
			<< " | " << NewBlock.File << " (" << NewBlock.Line << ")" << std::endl;
	}

	if (ptr != newptr)
	{
		m_CBlocks.erase(It);

		// Ajout du bloc à la liste des blocs alloués
		m_CBlocks[newptr] = NewBlock;
	}

	if (m_memory[MEM_RAM].m_deleteStack.size()!=0)
		m_memory[MEM_RAM].m_deleteStack.pop();

	O3D_MemoryManagerMutex.unlock();
	return newptr;
}

//! reallocate an aligned block of memory
void* MemoryManager::alignedReallocate(
	void* ptr,
	size_t size,
	size_t align,
	const Char* file,
	Int32 line)
{
	O3D_ASSERT(align != 0);

	// get the old memory block in the map
	TBlockMap::iterator It = m_CBlocks.find(ptr);

	// align check
	if (It != m_CBlocks.end())
	{
		if (It->second.Align == 0)
		{
			O3D_MemoryManagerMutex.unlock();
			O3D_ERROR(E_InvalidOperation("Non aligned pointer require non aligned realloc"));
			return NULL;
		}

		if (It->second.Align != align)
		{
			O3D_MemoryManagerMutex.unlock();
			O3D_ERROR(E_InvalidOperation("Same alignment is required"));
			return NULL;
		}

		if (It->second.Array != False)
		{
			O3D_MemoryManagerMutex.unlock();
			O3D_ERROR(E_InvalidOperation("Aligned realloc works only with non array allocations"));
			return NULL;
		}
	}

	// reallocate
#ifdef O3D_WINDOWS
	void* Newptr = _aligned_realloc(ptr, size, align);
#else
	void* Newptr = o3d::alignedRealloc(ptr, size, align);
#endif

	// get only file name
	O3D_GET_FILENAME

	TBlock NewBlock;
	NewBlock.Size  = size;
	NewBlock.Align = align;
	NewBlock.File  = &file[fileNamePos];
	NewBlock.Line  = line;
	NewBlock.Array = False;

	// extra bytes used by the alignement are not count
	m_memory[MEM_RAM].m_memorytotalbytes += (UInt32)size;
	m_memory[MEM_RAM].m_currentbytes += (UInt32)size;
	++m_memory[MEM_RAM].m_nbrNew;

	// unallocated block ? error
	if (It == m_CBlocks.end())
	{
		O3D_MemoryManagerMutex.unlock();
		m_file << "!! MMRea.BADBLOCK| 0x" << ptr << std::endl;
		return Newptr;
	}

	// Same base ptr
	if (ptr == Newptr)
	{
		It->second.Size = NewBlock.Size;
		It->second.File = NewBlock.File;
		It->second.Line = NewBlock.Line;
	}

	m_memory[MEM_RAM].m_currentbytes -= (UInt32)It->second.Size;
	++m_memory[MEM_RAM].m_nbrDelete;

	if (m_memory[MEM_RAM].m_currentbytes > m_memory[MEM_RAM].m_maxpeak)
		m_memory[MEM_RAM].m_maxpeak = m_memory[MEM_RAM].m_currentbytes;

	// serialisation
	if (m_memory[MEM_RAM].m_logging &&
		(size >= m_memory[MEM_RAM].m_minLogSize) &&
		(m_memory[MEM_RAM].m_deleteStack.size() != 0))
	{
		m_file << "** MMReallocate  | 0x" << ptr
			<< " > 0x" << std::setw(7) << std::setfill(' ') << Newptr << std::setw(7)
			<< std::setfill(' ') << (Int32)(NewBlock.Size) << " bytes"
			<< " | " << NewBlock.File << " (" << NewBlock.Line << ")" << std::endl;
	}

	if (ptr != Newptr)
	{
		m_CBlocks.erase(It);

		// Ajout du bloc à la liste des blocs alloués
		m_CBlocks[Newptr] = NewBlock;
	}

	if (m_memory[MEM_RAM].m_deleteStack.size()!=0)
		m_memory[MEM_RAM].m_deleteStack.pop();

	O3D_MemoryManagerMutex.unlock();
	return Newptr;
}

// free a memory allocation
void MemoryManager::free(void* ptr,Bool array)
{
	if(m_CBlocks.empty())
	{
		O3D_MemoryManagerMutex.unlock();
		return;
	}

	// get memory block in the map
	TBlockMap::iterator it = m_CBlocks.find(ptr);

	// unallocated block ? error
	if (it == m_CBlocks.end())
	{
		// En fait ca arrive souvent, du fait que le delete surcharge est pris en compte même
		// la ou on n'inclue pas MemoryDbg.h, mais pas la macro pour le new. Dans ce cas
		// on detruit le bloc et on quitte immediatement
		::free(ptr);
		O3D_MemoryManagerMutex.unlock();
		return;
	}

	// bad item type
	if (it->second.Array != array)
	{
		O3D_MemoryManagerMutex.unlock();
		O3D_ERROR(E_InvalidOperation("Not Array delete in"));
		return;
	}

	// bad align type
	if (it->second.Align != 0)
	{
		O3D_MemoryManagerMutex.unlock();
		O3D_ERROR(E_InvalidOperation("Aligned pointer require aligned free"));
		return;
	}

	// serialisation
	if (m_memory[MEM_RAM].m_logging &&
			((UInt32)(it->second.Size) >= m_memory[MEM_RAM].m_minLogSize) &&
			m_memory[MEM_RAM].m_deleteStack.size())
	{
		m_file << "-- Free          | 0x" << ptr
			<< " | " << std::setw(7) << std::setfill(' ') << (Int32)(it->second.Size) << " bytes"
			<< " | " << m_memory[MEM_RAM].m_deleteStack.top().File
			<< " (" << m_memory[MEM_RAM].m_deleteStack.top().Line << ")" << std::endl;
	}

	m_memory[MEM_RAM].m_currentbytes -= (UInt32)it->second.Size;
	++m_memory[MEM_RAM].m_nbrDelete;

	m_CBlocks.erase(it);
	if (m_memory[MEM_RAM].m_deleteStack.size()!=0)
		m_memory[MEM_RAM].m_deleteStack.pop();

	// free memory block
	::free(ptr);
	O3D_MemoryManagerMutex.unlock();
}

// aligned free
void MemoryManager::alignedFree(void* ptr)
{
	if(m_CBlocks.empty())
	{
		O3D_MemoryManagerMutex.unlock();
		return;
	}

	// get memory block in the map
	TBlockMap::iterator It = m_CBlocks.find(ptr);

	// unallocated block ? error
	if (It == m_CBlocks.end())
	{
		// En fait ca arrive souvent, du fait que le delete surcharge est pris en compte même
		// la ou on n'inclue pas O3DMemoryDbg.h, mais pas la macro pour le new. Dans ce cas
		// on detruit le bloc et on quitte immediatement
#ifdef O3D_WINDOWS
		_mm_free(ptr);
#else
		::free(ptr);
#endif
		O3D_MemoryManagerMutex.unlock();
		return;
	}

	// bad item type
	if (It->second.Array != False)
	{
		O3D_MemoryManagerMutex.unlock();
		O3D_ERROR(E_InvalidOperation("Aligned free works only with non array allocations"));
		return;
	}

	// bad align type
	if (It->second.Align == 0)
	{
		O3D_MemoryManagerMutex.unlock();
		O3D_ERROR(E_InvalidOperation("Non aligned pointer require non aligned free"));
		return;
	}

	// serialisation
	if (m_memory[MEM_RAM].m_logging &&
			((UInt32)(It->second.Size) >= m_memory[MEM_RAM].m_minLogSize) &&
			m_memory[MEM_RAM].m_deleteStack.size())
	{
		m_file << "-- MMFree        | 0x" << ptr
			<< " | " << std::setw(7) << std::setfill(' ') << (Int32)(It->second.Size) << " bytes"
			<< " | " << m_memory[MEM_RAM].m_deleteStack.top().File
			<< " (" << m_memory[MEM_RAM].m_deleteStack.top().Line << ")" << std::endl;
	}

	m_memory[MEM_RAM].m_currentbytes -= (UInt32)It->second.Size;
	++m_memory[MEM_RAM].m_nbrDelete;

	m_CBlocks.erase(It);
	if (m_memory[MEM_RAM].m_deleteStack.size()!=0)
		m_memory[MEM_RAM].m_deleteStack.pop();

	// free memory block
#ifdef O3D_WINDOWS
	_mm_free(ptr);
#else
	o3d::alignedFree(ptr);
#endif
	O3D_MemoryManagerMutex.unlock();
}


//---------------------------------------------------------------------------------------
// Fast allocator Operations
//---------------------------------------------------------------------------------------

#include "o3d/core/blockpooler.h"

// Define the fast allocator preallocated size
void MemoryManager::initFastAllocator(size_t num16, size_t num32, size_t num64)
{
	if (!num16 || !num32 || !num64)
	{
		O3D_MemoryManagerMutex.unlock();
		O3D_ERROR(E_InvalidParameter("Non null block number required"));
	}

	BlockPooler<16,16,4> *pool16 = new BlockPooler<16,16,4>(num16);
	BlockPooler<32,16,5> *pool32 = new BlockPooler<32,16,5>(num32);
	BlockPooler<64,16,6> *pool64 = new BlockPooler<64,16,6>(num64);

	m_fastMemoryPool.m_status = True;

	m_fastMemoryPool.m_pool16 = (void*)pool16;
	m_fastMemoryPool.m_pool32 = (void*)pool32;
	m_fastMemoryPool.m_pool64 = (void*)pool64;

	O3D_MemoryManagerMutex.unlock();
}

// Release the fast memory allocator
void MemoryManager::releaseFastAllocator()
{
	m_fastMemoryPool.m_status = False;

	if (m_fastMemoryPool.m_pool16)
	{
		delete ((BlockPooler<16,16,4>*)m_fastMemoryPool.m_pool16);
		m_fastMemoryPool.m_pool16 = NULL;
	}
	if (m_fastMemoryPool.m_pool32)
	{
		delete ((BlockPooler<32,16,5>*)m_fastMemoryPool.m_pool32);
		m_fastMemoryPool.m_pool32 = NULL;
	}
	if (m_fastMemoryPool.m_pool64)
	{
		delete ((BlockPooler<64,16,6>*)m_fastMemoryPool.m_pool64);
		m_fastMemoryPool.m_pool64 = NULL;
	}

	O3D_MemoryManagerMutex.unlock();
}

// Fast allocation of small memory block. Always 16 bytes aligned.
void* MemoryManager::fastAlloc(size_t size)
{
	O3D_ASSERT(m_fastMemoryPool.m_status);
	O3D_ASSERT(size <= 64);

	void* newptr = NULL;

	if (size <= 16)
		newptr = ((BlockPooler<16,16,4>*)m_fastMemoryPool.m_pool16)->allocate();
	else if (size <= 32)
		newptr = ((BlockPooler<32,16,5>*)m_fastMemoryPool.m_pool32)->allocate();
	else
		newptr = ((BlockPooler<64,16,6>*)m_fastMemoryPool.m_pool64)->allocate();

	O3D_MemoryManagerMutex.unlock();
	return newptr;
}

// Fast allocation of small memory block. Always 16 bytes aligned.
void* MemoryManager::fastAlloc(size_t size, const Char* file, Int32 line)
{
	O3D_ASSERT(m_fastMemoryPool.m_status);
	O3D_ASSERT(size <= 64);

	// get only file name
	O3D_GET_FILENAME

	void* newptr = NULL;

	if (size <= 16)
		newptr = ((BlockPooler<16,16,4>*)m_fastMemoryPool.m_pool16)->allocate();
	else if (size <= 32)
		newptr = ((BlockPooler<32,16,5>*)m_fastMemoryPool.m_pool32)->allocate();
	else
		newptr = ((BlockPooler<64,16,6>*)m_fastMemoryPool.m_pool64)->allocate();

	// Add a new block info
	FastMemoryPool::BlockInfo NewBlock;
	NewBlock.Size  = size;
	NewBlock.File  = &file[fileNamePos];
	NewBlock.Line  = line;
	m_fastMemoryPool.m_blocks[newptr] = NewBlock;

	m_memory[MEM_FAST].m_memorytotalbytes += (UInt32)size;
	m_memory[MEM_FAST].m_currentbytes += (UInt32)size;

	if (m_memory[MEM_FAST].m_currentbytes > m_memory[MEM_FAST].m_maxpeak)
		m_memory[MEM_FAST].m_maxpeak = m_memory[MEM_FAST].m_currentbytes;

	++m_memory[MEM_FAST].m_nbrNew;

	O3D_MemoryManagerMutex.unlock();
	return newptr;
}

// Release a previously fast allocation
void MemoryManager::freeFastAlloc(void* ptr, size_t size)
{
	O3D_ASSERT(m_fastMemoryPool.m_status);
	O3D_ASSERT(size <= 64);

	if (size <= 16)
		((BlockPooler<16,16,4>*)m_fastMemoryPool.m_pool16)->free(ptr);
	else if (size <= 32)
		((BlockPooler<32,16,5>*)m_fastMemoryPool.m_pool32)->free(ptr);
	else
		((BlockPooler<64,16,6>*)m_fastMemoryPool.m_pool64)->free(ptr);

	O3D_MemoryManagerMutex.unlock();
}

// Release a previously fast allocation in memory debug mode
void MemoryManager::freeFastAlloc(
	void* ptr,
	size_t size,
	const Char* file,
	Int32 line)
{
	O3D_ASSERT(m_fastMemoryPool.m_status);
	O3D_ASSERT(size <= 64);

	// get memory block in the map
	FastMemoryPool::BlockInfoMap::iterator It = m_fastMemoryPool.m_blocks.find(ptr);
	O3D_ASSERT(It != m_fastMemoryPool.m_blocks.end());

	m_memory[MEM_FAST].m_currentbytes -= (UInt32)It->second.Size;
	++m_memory[MEM_FAST].m_nbrDelete;

	m_fastMemoryPool.m_blocks.erase(It);

	if (size <= 16)
		((BlockPooler<16,16,4>*)m_fastMemoryPool.m_pool16)->free(ptr);
	else if (size <= 32)
		((BlockPooler<32,16,5>*)m_fastMemoryPool.m_pool32)->free(ptr);
	else
		((BlockPooler<64,16,6>*)m_fastMemoryPool.m_pool64)->free(ptr);

	O3D_MemoryManagerMutex.unlock();
}

MemoryManager::FastMemoryPool::~FastMemoryPool()
{
	if (m_pool16)
		delete ((BlockPooler<16,16,4>*)m_pool16);
	if (m_pool32)
		delete ((BlockPooler<32,16,5>*)m_pool32);
	if (m_pool64)
		delete ((BlockPooler<64,16,6>*)m_pool64);
}

void MemoryManager::FastMemoryPool::reportLeaks(std::ofstream &file)
{
	size_t TotalSize = 0;

	file << std::endl;
	file << "------------------------------------------------------------------------------------------" << std::endl;
	file << "-                                 Fast-pool Memory Leaks                                 -" << std::endl;
	file << "------------------------------------------------------------------------------------------" << std::endl;
	file << std::endl;

	for (BlockInfoMap::iterator i = m_blocks.begin(); i != m_blocks.end(); ++i)
	{
		// add memory size
		TotalSize += i->second.Size;

		// write block information in file
		file << "-> @0x" << i->first
			<< " | "   << std::setw(7) << std::setfill(' ') << (Int32)(i->second.Size) << " bytes"
			<< " | "   << i->second.File << " (" << i->second.Line << ")" << std::endl;
	}

	// draw all memory leak sum
	file << std::endl << std::endl << "-- "
		<< (Int32)(m_blocks.size()) << " none free block(s), "
		<< (Int32)(TotalSize)       << " bytes --"
		<< std::endl;
}


//---------------------------------------------------------------------------------------
// Graphic Memory Operations
//---------------------------------------------------------------------------------------

// register a graphic memory allocation
void MemoryManager::gfxAlloc(
	GraphicTarget target,
	UInt32 id,
	size_t size,
	const Char* file,
	Int32 fine)
{
	// get only file name
	O3D_GET_FILENAME

	// Ajout du bloc à la liste des blocs alloués
	TSBlock NewBlock;
	NewBlock.Size = size;
	NewBlock.File = &file[fileNamePos];
	NewBlock.Line = fine;
	m_GBlocks[TGKey(target,id)] = NewBlock;

	m_memory[MEM_GFX].m_memorytotalbytes += (UInt32)size;
	m_memory[MEM_GFX].m_currentbytes += (UInt32)size;

	if (m_memory[MEM_GFX].m_currentbytes > m_memory[MEM_GFX].m_maxpeak)
		m_memory[MEM_GFX].m_maxpeak = m_memory[MEM_GFX].m_currentbytes;

	++m_memory[MEM_GFX].m_nbrNew;

	// Serialization
	if (m_memory[MEM_GFX].m_logging && (size >= m_memory[MEM_GFX].m_minLogSize))
	{
		m_file << "+G GAllocation   | " << std::setw(2) << std::setfill(' ') << gfxType2String(target)
			   << ":" << std::setw(6) << std::setfill(' ') << id
			   << " | " << std::setw(7) << std::setfill(' ') << (Int32)(NewBlock.Size) << " bytes"
			   << " | " << NewBlock.File << " (" << NewBlock.Line << ")" << std::endl;
	}

	O3D_MemoryManagerMutex.unlock();
}

// unregister a graphic memory allocation
void MemoryManager::gfxFree(GraphicTarget target,UInt32 id)
{
	if(m_GBlocks.empty())
	{
		O3D_MemoryManagerMutex.unlock();
		m_file << "!! GFree MAPEMPTY| " << std::setw(2) << std::setfill(' ') << gfxType2String(target)
			<< ":" << std::setw(6) << std::setfill(' ') << id << std::endl;
		return;
	}

	// get memory block in the map
	TGBlockMap::iterator It = m_GBlocks.find(TGKey(target,id));

	// unallocated block ? error
	if (It == m_GBlocks.end())
	{
		O3D_MemoryManagerMutex.unlock();
		m_file << "!! GFree BADBLOCK| " << std::setw(2) << std::setfill(' ') << gfxType2String(target)
			<< ":" << std::setw(6) << std::setfill(' ') << id << std::endl;
		return;
	}

	// Serialization
	if (m_memory[MEM_GFX].m_logging &&
			((UInt32)(It->second.Size) >= m_memory[MEM_GFX].m_minLogSize) &&
			m_memory[MEM_GFX].m_deleteStack.size())
	{
		m_file << "-G GFree         | " << std::setw(2) << std::setfill(' ') << gfxType2String(target)
			<< ":" << std::setw(6) << std::setfill(' ') << id
			<< " | " << std::setw(7) << std::setfill(' ') << (Int32)(It->second.Size) << " bytes"
			<< " | " << m_memory[MEM_GFX].m_deleteStack.top().File
			<< " (" << m_memory[MEM_GFX].m_deleteStack.top().Line << ")" << std::endl;
	}

	m_memory[MEM_GFX].m_currentbytes -= (UInt32)It->second.Size;
	++m_memory[MEM_GFX].m_nbrDelete;

	m_GBlocks.erase(It);
	if (m_memory[MEM_GFX].m_deleteStack.size()!=0)
		m_memory[MEM_GFX].m_deleteStack.pop();

	O3D_MemoryManagerMutex.unlock();
}

// re-register a graphic memory allocation
void MemoryManager::gfxRealloc(
	GraphicTarget target,
	UInt32 id,
	size_t size,
	const Char *file,
	Int32 line)
{
	// get only file name
	O3D_GET_FILENAME

	m_memory[MEM_GFX].m_memorytotalbytes += (UInt32)size;
	m_memory[MEM_GFX].m_currentbytes += (UInt32)size;
	++m_memory[MEM_GFX].m_nbrNew;

	// get the old memory block in the map
	TGBlockMap::iterator It = m_GBlocks.find(TGKey(target,id));

	// unallocated block ? error
	if (It == m_GBlocks.end())
	{
		O3D_MemoryManagerMutex.unlock();
		m_file << "!! GReal.BADBLOCK| " << std::setw(2) << std::setfill(' ') << gfxType2String(target)
			<< ":" << std::setw(6) << std::setfill(' ') << id << std::endl;
		return;
	}

	m_memory[MEM_GFX].m_currentbytes -= (UInt32)It->second.Size;
	++m_memory[MEM_GFX].m_nbrDelete;

	if (m_memory[MEM_GFX].m_currentbytes > m_memory[MEM_GFX].m_maxpeak)
		m_memory[MEM_GFX].m_maxpeak = m_memory[MEM_GFX].m_currentbytes;

	// Change les infos du bloque existant
	It->second.Size = size;
	It->second.File = &file[fileNamePos];
	It->second.Line = line;

	// Serialization
	if (m_memory[MEM_GFX].m_logging &&
			(size >= m_memory[MEM_GFX].m_minLogSize) &&
			(m_memory[MEM_GFX].m_deleteStack.size() != 0))
	{
		m_file << "*G GReallocate   | " << std::setw(2) << std::setfill(' ') << gfxType2String(target)
			<< ":" << std::setw(6) << std::setfill(' ') << id
			<< " | " << std::setw(7) << std::setfill(' ') << (Int32)(It->second.Size) << " bytes"
			<< " | " << It->second.File << " (" << It->second.Line << ")" << std::endl;
	}

	if (m_memory[MEM_GFX].m_deleteStack.size()!=0)
		m_memory[MEM_GFX].m_deleteStack.pop();

	O3D_MemoryManagerMutex.unlock();
}

//---------------------------------------------------------------------------------------
// Audio Memory Operations
//---------------------------------------------------------------------------------------

// register a sound memory allocation
void MemoryManager::sfxAlloc(
	AudioTarget target,
	UInt32 id,
	size_t size,
	const Char* file,
	Int32 line)
{
	// get only file name
	O3D_GET_FILENAME

	// Ajout du bloc à la liste des blocs alloués
	TGBlock NewBlock;
	NewBlock.Size = size;
	NewBlock.File = &file[fileNamePos];
	NewBlock.Line = line;
	m_SBlocks[TSKey(target,id)] = NewBlock;

	m_memory[MEM_SFX].m_memorytotalbytes += (UInt32)size;
	m_memory[MEM_SFX].m_currentbytes += (UInt32)size;

	if (m_memory[MEM_SFX].m_currentbytes > m_memory[MEM_SFX].m_maxpeak)
		m_memory[MEM_SFX].m_maxpeak = m_memory[MEM_SFX].m_currentbytes;

	++m_memory[MEM_SFX].m_nbrNew;

	// Serialization
	if (m_memory[MEM_SFX].m_logging && (size >= m_memory[MEM_SFX].m_minLogSize))
	{
		m_file << "+A AAllocation   | " << std::setw(2) << std::setfill(' ') << sfxType2String(target)
			   << ":" << std::setw(6) << std::setfill(' ') << id
			   << " | " << std::setw(7) << std::setfill(' ') << (Int32)(NewBlock.Size) << " bytes"
			   << " | " << NewBlock.File << " (" << NewBlock.Line << ")" << std::endl;
	}

	O3D_MemoryManagerMutex.unlock();
}

// unregister a sound memory allocation
void MemoryManager::sfxFree(AudioTarget target,UInt32 id)
{
	if(m_SBlocks.empty())
	{
		O3D_MemoryManagerMutex.unlock();
		m_file << "!! AFree MAPEMPTY| " << std::setw(2) << std::setfill(' ') << sfxType2String(target)
			<< ":" << std::setw(6) << std::setfill(' ') << id << std::endl;
		return;
	}

	// get memory block in the map
	TSBlockMap::iterator It = m_SBlocks.find(TSKey(target,id));

	// unallocated block ? error
	if (It == m_SBlocks.end())
	{
		O3D_MemoryManagerMutex.unlock();
		m_file << "!! AFree BADBLOCK| " << std::setw(2) << std::setfill(' ') << sfxType2String(target)
			<< ":" << std::setw(6) << std::setfill(' ') << id << std::endl;
		return;
	}

	// Serialization
	if (m_memory[MEM_SFX].m_logging &&
			((UInt32)(It->second.Size) >= m_memory[MEM_SFX].m_minLogSize) &&
			m_memory[MEM_SFX].m_deleteStack.size())
	{
		m_file << "-A AFree         | " << std::setw(2) << std::setfill(' ') << sfxType2String(target)
			<< ":" << std::setw(6) << std::setfill(' ') << id
			<< " | " << std::setw(7) << std::setfill(' ') << (Int32)(It->second.Size) << " bytes"
			<< " | " << m_memory[MEM_SFX].m_deleteStack.top().File
			<< " (" << m_memory[MEM_SFX].m_deleteStack.top().Line << ")" << std::endl;
	}

	m_memory[MEM_SFX].m_currentbytes -= (UInt32)It->second.Size;
	++m_memory[MEM_SFX].m_nbrDelete;

	m_SBlocks.erase(It);
	if (m_memory[MEM_SFX].m_deleteStack.size()!=0)
		m_memory[MEM_SFX].m_deleteStack.pop();

	O3D_MemoryManagerMutex.unlock();
}

// re-register a sound memory allocation
void MemoryManager::sfxRealloc(
	AudioTarget target,
	UInt32 id,
	size_t size,
	const Char* file,
	Int32 line)
{
	// get only file name
	O3D_GET_FILENAME

	m_memory[MEM_SFX].m_memorytotalbytes += (UInt32)size;
	m_memory[MEM_SFX].m_currentbytes += (UInt32)size;
	++m_memory[MEM_SFX].m_nbrNew;

	// get the old memory block in the map
	TSBlockMap::iterator It = m_SBlocks.find(TSKey(target,id));

	// unallocated block ? error
	if (It == m_SBlocks.end())
	{
		O3D_MemoryManagerMutex.unlock();
		m_file << "!! AReal.BADBLOCK| " << std::setw(2) << std::setfill(' ') << sfxType2String(target)
			<< ":" << std::setw(6) << std::setfill(' ') << id << std::endl;
		return;
	}

	m_memory[MEM_SFX].m_currentbytes -= (UInt32)It->second.Size;
	++m_memory[MEM_SFX].m_nbrDelete;

	if (m_memory[MEM_SFX].m_currentbytes > m_memory[MEM_SFX].m_maxpeak)
		m_memory[MEM_SFX].m_maxpeak = m_memory[MEM_SFX].m_currentbytes;

	// Change les infos du bloque existant
	It->second.Size = size;
	It->second.File = &file[fileNamePos];
	It->second.Line = line;

	// Serialization
	if (m_memory[MEM_SFX].m_logging &&
			(size >= m_memory[MEM_SFX].m_minLogSize) &&
			(m_memory[MEM_SFX].m_deleteStack.size() != 0))
	{
		m_file << "*& AReallocate   | " << std::setw(2) << std::setfill(' ') << sfxType2String(target)
			<< ":" << std::setw(6) << std::setfill(' ') << id
			<< " | " << std::setw(7) << std::setfill(' ') << (Int32)(It->second.Size) << " bytes"
			<< " | " << It->second.File << " (" << It->second.Line << ")" << std::endl;
	}

	if (m_memory[MEM_SFX].m_deleteStack.size()!=0)
		m_memory[MEM_SFX].m_deleteStack.pop();

	O3D_MemoryManagerMutex.unlock();
}

//---------------------------------------------------------------------------------------
// Global methods
//---------------------------------------------------------------------------------------

// backup info from current free
void MemoryManager::nextDelete(MemoryType type,const Char* file,Int32 line)
{
	TBlock Delete;

	// get only file name
	O3D_GET_FILENAME

	Delete.File = &file[fileNamePos];
	Delete.Line = line;

    m_memory[type].m_deleteStack.push(Delete);
	O3D_MemoryManagerMutex.unlock();
}

// enable/disable log new/delete
void MemoryManager::enableLog(MemoryType type,UInt32 minLogSize)
{
	m_memory[type].m_logging = True;
	m_memory[type].m_minLogSize = minLogSize;
	O3D_MemoryManagerMutex.unlock();
}
void MemoryManager::disableLog(MemoryType type)
{
	m_memory[type].m_logging = False;
	O3D_MemoryManagerMutex.unlock();
}

Bool MemoryManager::isLogging(MemoryType type)
{
	Bool ret = m_memory[type].m_logging;
	O3D_MemoryManagerMutex.unlock();
	return ret;
}

// get the memory evolution
MemoryManager::MemoryState MemoryManager::getMemoryState(MemoryType type)
{
	O3D_MemoryManagerTime[type].update();

	// do we need to recompute the memory state ?
	if (O3D_MemoryManagerTime[type].check())
	{
		if (m_memory[type].m_currentbytes == m_memory[type].m_prevCurrentBytes)
			m_memory[type].m_memState = STABLE;

		else if (m_memory[type].m_hysteris != 0)
		{
			UInt32 _min = m_memory[type].m_prevCurrentBytes - m_memory[type].m_hysteris;
			UInt32 _max = m_memory[type].m_prevCurrentBytes + m_memory[type].m_hysteris;

			if ((m_memory[type].m_currentbytes >= _min) && (m_memory[type].m_currentbytes <= _max))
				m_memory[type].m_memState = NEAR_STABLE;

			else if (m_memory[type].m_currentbytes > m_memory[type].m_prevCurrentBytes)
				m_memory[type].m_memState = INCREASE;

			else if (m_memory[type].m_currentbytes < m_memory[type].m_prevCurrentBytes)
				m_memory[type].m_memState = DECREASE;
		}

		else if (m_memory[type].m_currentbytes > m_memory[type].m_prevCurrentBytes)
			m_memory[type].m_memState = INCREASE;
		else if (m_memory[type].m_currentbytes < m_memory[type].m_prevCurrentBytes)
			m_memory[type].m_memState = DECREASE;

		m_memory[type].m_prevCurrentBytes = m_memory[type].m_currentbytes;
	}

	MemoryState ret = m_memory[type].m_memState;
	O3D_MemoryManagerMutex.unlock();
	return ret;
}

// set/get the memory evolution state delay (in ms)
void MemoryManager::setMemoryRefreshDelay(MemoryType type,UInt32 ms)
{
	O3D_MemoryManagerTime[type].setCheckTime(ms*0.001f);
	O3D_MemoryManagerMutex.unlock();
}

UInt32 MemoryManager::getMemoryRefreshDelay(MemoryType type)const
{
	UInt32 ret = (UInt32)(O3D_MemoryManagerTime[type].getCheckTime()*1000.f);
	O3D_MemoryManagerMutex.unlock();
	return ret;
}

// set/get the memory evolution state hysteresis
void MemoryManager::setMemoryStateHystereris(MemoryType type,UInt32 h)
{
	m_memory[type].m_hysteris = h;
	O3D_MemoryManagerMutex.unlock();
}

UInt32 MemoryManager::getMemoryStateHystereris(MemoryType type)const
{
	UInt32 ret = m_memory[type].m_hysteris;
	O3D_MemoryManagerMutex.unlock();
	return ret;
}

// get current memory occupation
UInt32 MemoryManager::getCurrentMemorySize(MemoryType type)const
{
	UInt32 ret = m_memory[type].m_currentbytes;
	O3D_MemoryManagerMutex.unlock();
	return ret;
}

// get total memory allocation
UInt32 MemoryManager::getTotalMemorySize(MemoryType type)const
{
	UInt32 ret = m_memory[type].m_memorytotalbytes;
	O3D_MemoryManagerMutex.unlock();
	return ret;
}

// get maximum memory allocation peak
UInt32 MemoryManager::getMemoryPeakSize(MemoryType type)const
{
	UInt32 ret = m_memory[type].m_maxpeak;
	O3D_MemoryManagerMutex.unlock();
	return ret;
}

// reset memory states
void MemoryManager::resetCounters(MemoryType type)
{
	m_memory[type].m_memState = INCREASE;
	m_memory[type].m_currentbytes = 0;
	m_memory[type].m_memorytotalbytes = 0;
	m_memory[type].m_prevCurrentBytes = 0;
	m_memory[type].m_maxpeak = 0;

	m_memory[type].m_nbrNew = 0;
	m_memory[type].m_nbrDelete = 0;

	O3D_MemoryManagerMutex.unlock();
}

// get number of new/delete operations per second
Bool MemoryManager::updateOpPerSec(MemoryType type,Float &news, Float &deletes)
{
	O3D_MemoryManagerTimeCounter[type].update();

	if (O3D_MemoryManagerTimeCounter[type].check())
	{
		news = (Float)m_memory[type].m_nbrNew / O3D_MemoryManagerTimeCounter[type].elapsed();
		deletes = (Float)m_memory[type].m_nbrDelete / O3D_MemoryManagerTimeCounter[type].elapsed();

		m_memory[type].m_nbrNew = m_memory[type].m_nbrDelete = 0;

		O3D_MemoryManagerMutex.unlock();
		return True;
	}

	O3D_MemoryManagerMutex.unlock();
	return False;
}

