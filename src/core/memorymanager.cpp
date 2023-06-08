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
#include "o3d/core/logger.h"
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
    switch (gtype) {
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
    switch (stype) {
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
    m_logger(nullptr)
{
	// is file open ?
#ifdef O3D_ANDROID
    m_logger = new AndroidLogger("<MEM>");
#else
    m_logger = new FileLogger("memory.log");
#endif

    if (!m_logger) {
        return;
    }
}

// Destructor
MemoryManager::~MemoryManager()
{
    if (!m_logger) {
        return;
    }

	// Memory total allocated size
    if (m_memory[MEM_RAM].m_memorytotalbytes > 0) {
        m_logger->log(Logger::INFO, String("A total of ") << m_memory[MEM_RAM].m_memorytotalbytes << " bytes has been allocated in central memory");
    }

    if (m_memory[MEM_GFX].m_memorytotalbytes > 0) {
        m_logger->log(Logger::INFO, String("A total of ") << m_memory[MEM_GFX].m_memorytotalbytes << " bytes has been allocated in graphical memory");
    }

    if (m_memory[MEM_SFX].m_memorytotalbytes > 0) {
        m_logger->log(Logger::INFO, String("A total of ") << m_memory[MEM_SFX].m_memorytotalbytes << " bytes has been allocated in audio memory");
    }

    if (!m_CBlocks.empty() || !m_GBlocks.empty()) {
		// Leaks !!
        m_logger->log(Logger::WARNING, "Memory leaks detected :");
		reportLeaks();
	}

    delete m_logger;
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
	//-------------------------------------
	// Firstly we have central memory leaks
	//-------------------------------------

    size_t TotalSize = 0;
    m_logger->log(Logger::INFO, "Central Memory Leaks :");

    for (TBlockMap::iterator i = m_CBlocks.begin(); i != m_CBlocks.end(); ++i) {
		// add memory size
		TotalSize += i->second.Size;

		// write block information in file
        m_logger->log(Logger::INFO, String("-> 0x{0}").arg((UInt64)i->first, 16, 16, '0')
               << String(" | {0} bytes").arg((Int32)(i->second.Size), 7, 10, ' ')
               << String(" | {0} ({1})").arg(i->second.File.c_str()).arg(i->second.Line));

        // free memory, commented because memory zone can be corrupted at this time
        //free(i->first);
    }

    // draw all memory leak sum
    m_logger->log(Logger::INFO, String("-- {0} none free block(s), {1} bytes --")
                  .arg((Int32)m_CBlocks.size())
                  .arg((Int32)TotalSize));

	//------------------------------------
	// Next we have graphical memory leaks
	//------------------------------------

	TotalSize = 0;
    m_logger->log(Logger::INFO, "Graphical Memory Leaks :");

    for (TGBlockMap::iterator i = m_GBlocks.begin(); i != m_GBlocks.end(); ++i) {
		// add memory size
		TotalSize += i->second.Size;

		// write block information in file
        m_logger->log(Logger::INFO, String("-> {0}").arg(gfxType2String(i->first.Target))
               << String(":{0}").arg(i->first.Id, 6, 10, ' ')
               << String(" | {0} bytes").arg((Int32)(i->second.Size), 7, 10, ' ')
               << String(" | {0} ({1})").arg(i->second.File.c_str()).arg(i->second.Line));
    }

    // draw all memory leak sum
    m_logger->log(Logger::INFO, String("-- {0} none free block(s), {1} bytes --")
                  .arg((Int32)m_GBlocks.size())
                  .arg((Int32)TotalSize));

	//--------------------------------
	// Next we have sound memory leaks
	//--------------------------------

	TotalSize = 0;
    m_logger->log(Logger::INFO, "Audio Memory Leaks :");

    for (TSBlockMap::iterator i = m_SBlocks.begin(); i != m_SBlocks.end(); ++i) {
		// add memory size
		TotalSize += i->second.Size;

		// write block information in file
        m_logger->log(Logger::INFO, String("-> {0}").arg(sfxType2String(i->first.Target))
               << String(":{0}").arg(i->first.Id, 6, 10, ' ')
               << String(" | {0} bytes").arg((Int32)(i->second.Size), 7, 10, ' ')
               << String(" | {0} ({1})").arg(i->second.File.c_str()).arg(i->second.Line));
    }

    // draw all memory leak sum
    m_logger->log(Logger::INFO, String("-- {0} none free block(s), {1} bytes --")
                  .arg((Int32)m_SBlocks.size())
                  .arg((Int32)TotalSize));

	//------------------------------------
	// Next we have fast pool memory leaks
	//------------------------------------

    m_fastMemoryPool.reportLeaks(m_logger);
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
    TBlock newBlock;
    newBlock.Size  = Size;
    newBlock.Align = 0;
    newBlock.File  = &file[fileNamePos];
    newBlock.Line  = line;
    newBlock.Array = array;
    m_CBlocks[ptr] = newBlock;

	m_memory[MEM_RAM].m_memorytotalbytes += (UInt32)Size;
	m_memory[MEM_RAM].m_currentbytes += (UInt32)Size;

    if (m_memory[MEM_RAM].m_currentbytes > m_memory[MEM_RAM].m_maxpeak) {
		m_memory[MEM_RAM].m_maxpeak = m_memory[MEM_RAM].m_currentbytes;
    }

	++m_memory[MEM_RAM].m_nbrNew;

	// serialisation
    if (m_memory[MEM_RAM].m_logging && (Size >= m_memory[MEM_RAM].m_minLogSize)) {

        m_logger->log(Logger::INFO, String("++ Allocation    | 0x{0}").arg((UInt64)ptr, 16, 16, '0')
            << String(" | {0} bytes").arg((Int32)newBlock.Size, 7, 10, ' ')
            << String(" | {0} ({1})").arg(newBlock.File.c_str()).arg(newBlock.Line));
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
    TBlock newBlock;
    newBlock.Size  = size;
    newBlock.Align = align;
    newBlock.File  = &file[fileNamePos];
    newBlock.Line  = line;
    newBlock.Array = False;
    m_CBlocks[ptr] = newBlock;

	// extra bytes used by the alignement are not count
	m_memory[MEM_RAM].m_memorytotalbytes += (UInt32)size;
	m_memory[MEM_RAM].m_currentbytes += (UInt32)size;

    if (m_memory[MEM_RAM].m_currentbytes > m_memory[MEM_RAM].m_maxpeak) {
		m_memory[MEM_RAM].m_maxpeak = m_memory[MEM_RAM].m_currentbytes;
    }

	++m_memory[MEM_RAM].m_nbrNew;

	// serialisation
    if (m_memory[MEM_RAM].m_logging && (size >= m_memory[MEM_RAM].m_minLogSize)) {

        m_logger->log(Logger::INFO, String("++ MMAllocation  | 0x{0}").arg((UInt64)ptr, 16, 16, '0')
            << String(" | {0} bytes").arg((Int32)newBlock.Size, 7, 10, ' ')
            << String(" | {0} ({1})").arg(newBlock.File.c_str()).arg(newBlock.Line));
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
    TBlockMap::iterator it = m_CBlocks.find(ptr);

	// align check
    if (it != m_CBlocks.end()) {
        if (it->second.Align != 0) {
			O3D_MemoryManagerMutex.unlock();
			O3D_ERROR(E_InvalidOperation("Aligned pointer require aligned realloc"));
            return nullptr;
		}
	}

	// ReAllocation de la mémoire
    void* newPtr = realloc(ptr,size);

	// get only file name
	O3D_GET_FILENAME

    TBlock newBlock;
    newBlock.Size  = size;
    newBlock.Align = 0;
    newBlock.File  = &file[fileNamePos];
    newBlock.Line  = line;
    newBlock.Array = False;

	m_memory[MEM_RAM].m_memorytotalbytes += (UInt32)size;
	m_memory[MEM_RAM].m_currentbytes += (UInt32)size;
	++m_memory[MEM_RAM].m_nbrNew;

	// unallocated block ? error
    if (it == m_CBlocks.end()) {
		O3D_MemoryManagerMutex.unlock();
        m_logger->log(Logger::INFO, String("!! Reall.BADBLOCK| 0x{0}").arg((UInt64)ptr, 16, 16, '0'));
        return newPtr;
	}

	// Same base ptr
    if (ptr == newPtr) {
        it->second.Size = newBlock.Size;
        it->second.File = newBlock.File;
        it->second.Line = newBlock.Line;
	}

    m_memory[MEM_RAM].m_currentbytes -= (UInt32)it->second.Size;
	++m_memory[MEM_RAM].m_nbrDelete;

    if (m_memory[MEM_RAM].m_currentbytes > m_memory[MEM_RAM].m_maxpeak) {
		m_memory[MEM_RAM].m_maxpeak = m_memory[MEM_RAM].m_currentbytes;
    }

	// serialisation
    if (m_memory[MEM_RAM].m_logging && (size >= m_memory[MEM_RAM].m_minLogSize) &&
            (m_memory[MEM_RAM].m_deleteStack.size() != 0)) {

        m_logger->log(Logger::INFO, String("** Reallocate    | 0x{0}").arg((UInt64)ptr, 16, 16, '0')
            << String(" > 0x{0} {1} bytes").arg((UInt64)newPtr, 16, 16, '0').arg((Int32)newBlock.Size, 7, 10, ' ')
            << String(" | {0} ({1})").arg(newBlock.File.c_str()).arg(newBlock.Line));
	}

    if (ptr != newPtr) {
        m_CBlocks.erase(it);

        // Ajout du bloc a la liste des blocs alloues
        m_CBlocks[newPtr] = newBlock;
	}

    if (m_memory[MEM_RAM].m_deleteStack.size() != 0) {
		m_memory[MEM_RAM].m_deleteStack.pop();
    }

	O3D_MemoryManagerMutex.unlock();
    return newPtr;
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
    TBlockMap::iterator it = m_CBlocks.find(ptr);

	// align check
    if (it != m_CBlocks.end()) {
        if (it->second.Align == 0) {
			O3D_MemoryManagerMutex.unlock();
			O3D_ERROR(E_InvalidOperation("Non aligned pointer require non aligned realloc"));
            return nullptr;
		}

        if (it->second.Align != align) {
			O3D_MemoryManagerMutex.unlock();
			O3D_ERROR(E_InvalidOperation("Same alignment is required"));
            return nullptr;
		}

        if (it->second.Array != False) {
			O3D_MemoryManagerMutex.unlock();
			O3D_ERROR(E_InvalidOperation("Aligned realloc works only with non array allocations"));
            return nullptr;
		}
	}

	// reallocate
#ifdef O3D_WINDOWS
    void* newPtr = _aligned_realloc(ptr, size, align);
#else
    void* newPtr = o3d::alignedRealloc(ptr, size, align);
#endif

	// get only file name
	O3D_GET_FILENAME

    TBlock newBlock;
    newBlock.Size  = size;
    newBlock.Align = align;
    newBlock.File  = &file[fileNamePos];
    newBlock.Line  = line;
    newBlock.Array = False;

	// extra bytes used by the alignement are not count
	m_memory[MEM_RAM].m_memorytotalbytes += (UInt32)size;
	m_memory[MEM_RAM].m_currentbytes += (UInt32)size;
	++m_memory[MEM_RAM].m_nbrNew;

	// unallocated block ? error
    if (it == m_CBlocks.end()) {
		O3D_MemoryManagerMutex.unlock();
        m_logger->log(Logger::INFO, String("!! MMRea.BADBLOCK| 0x{0}").arg((UInt64)ptr, 16, 16, '0'));
        return newPtr;
	}

	// Same base ptr
    if (ptr == newPtr) {
        it->second.Size = newBlock.Size;
        it->second.File = newBlock.File;
        it->second.Line = newBlock.Line;
	}

    m_memory[MEM_RAM].m_currentbytes -= (UInt32)it->second.Size;
	++m_memory[MEM_RAM].m_nbrDelete;

    if (m_memory[MEM_RAM].m_currentbytes > m_memory[MEM_RAM].m_maxpeak) {
		m_memory[MEM_RAM].m_maxpeak = m_memory[MEM_RAM].m_currentbytes;
    }

	// serialisation
    if (m_memory[MEM_RAM].m_logging && (size >= m_memory[MEM_RAM].m_minLogSize) &&
        (m_memory[MEM_RAM].m_deleteStack.size() != 0)) {

        m_logger->log(Logger::INFO, String("** MMReallocate  | 0x{0}").arg((UInt64)ptr, 16, 16, '0')
            << String(" > 0x{0} {1} bytes").arg((UInt64)newPtr, 16, 16, '0').arg((Int32)newBlock.Size, 7, 10, ' ')
            << String(" | {0} ({1})").arg(newBlock.File.c_str()).arg(newBlock.Line));
	}

    if (ptr != newPtr) {
        m_CBlocks.erase(it);

		// Ajout du bloc à la liste des blocs alloués
        m_CBlocks[newPtr] = newBlock;
	}

    if (m_memory[MEM_RAM].m_deleteStack.size() != 0) {
		m_memory[MEM_RAM].m_deleteStack.pop();
    }

	O3D_MemoryManagerMutex.unlock();
    return newPtr;
}

// free a memory allocation
void MemoryManager::free(void* ptr,Bool array)
{
    if (m_CBlocks.empty()) {
		O3D_MemoryManagerMutex.unlock();
		return;
	}

	// get memory block in the map
	TBlockMap::iterator it = m_CBlocks.find(ptr);

	// unallocated block ? error
    if (it == m_CBlocks.end()) {
		// En fait ca arrive souvent, du fait que le delete surcharge est pris en compte même
		// la ou on n'inclue pas MemoryDbg.h, mais pas la macro pour le new. Dans ce cas
		// on detruit le bloc et on quitte immediatement
		::free(ptr);
		O3D_MemoryManagerMutex.unlock();
		return;
	}

	// bad item type
    if (it->second.Array != array) {
		O3D_MemoryManagerMutex.unlock();
		O3D_ERROR(E_InvalidOperation("Not Array delete in"));
		return;
	}

	// bad align type
    if (it->second.Align != 0) {
		O3D_MemoryManagerMutex.unlock();
		O3D_ERROR(E_InvalidOperation("Aligned pointer require aligned free"));
		return;
	}

	// serialisation
    if (m_memory[MEM_RAM].m_logging && ((UInt32)(it->second.Size) >= m_memory[MEM_RAM].m_minLogSize) &&
            m_memory[MEM_RAM].m_deleteStack.size()) {

        m_logger->log(Logger::INFO, String("-- Free          | 0x{0}").arg((UInt64)ptr, 16, 16, '0')
            << String(" | {0} bytes").arg((Int32)it->second.Size, 7, 10, ' ')
            << String(" | {0} ({1})")
                      .arg(m_memory[MEM_RAM].m_deleteStack.top().File.c_str())
                      .arg(m_memory[MEM_RAM].m_deleteStack.top().Line));
	}

	m_memory[MEM_RAM].m_currentbytes -= (UInt32)it->second.Size;
	++m_memory[MEM_RAM].m_nbrDelete;

	m_CBlocks.erase(it);
    if (m_memory[MEM_RAM].m_deleteStack.size() != 0) {
		m_memory[MEM_RAM].m_deleteStack.pop();
    }

	// free memory block
	::free(ptr);
	O3D_MemoryManagerMutex.unlock();
}

// aligned free
void MemoryManager::alignedFree(void* ptr)
{
    if (m_CBlocks.empty()) {
		O3D_MemoryManagerMutex.unlock();
		return;
	}

	// get memory block in the map
    TBlockMap::iterator it = m_CBlocks.find(ptr);

	// unallocated block ? error
    if (it == m_CBlocks.end()) {
		// En fait ca arrive souvent, du fait que le delete surcharge est pris en compte même
        // la ou on n'inclue pas memorydbg.h, mais pas la macro pour le new. Dans ce cas
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
    if (it->second.Array != False) {
		O3D_MemoryManagerMutex.unlock();
		O3D_ERROR(E_InvalidOperation("Aligned free works only with non array allocations"));
		return;
	}

	// bad align type
    if (it->second.Align == 0) {
		O3D_MemoryManagerMutex.unlock();
        O3D_ERROR(E_InvalidOperation("None aligned pointer require non aligned free"));
		return;
	}

	// serialisation
    if (m_memory[MEM_RAM].m_logging && ((UInt32)(it->second.Size) >= m_memory[MEM_RAM].m_minLogSize) &&
            m_memory[MEM_RAM].m_deleteStack.size()) {

        m_logger->log(Logger::INFO, String("-- MMFree        | 0x{0}").arg((UInt64)ptr, 16, 16, '0')
            << String(" | {0} bytes").arg((Int32)it->second.Size, 7, 10, ' ')
            << String(" | {0} ({1})")
                      .arg(m_memory[MEM_RAM].m_deleteStack.top().File.c_str())
                      .arg(m_memory[MEM_RAM].m_deleteStack.top().Line));
	}

    m_memory[MEM_RAM].m_currentbytes -= (UInt32)it->second.Size;
	++m_memory[MEM_RAM].m_nbrDelete;

    m_CBlocks.erase(it);
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
    if (!num16 || !num32 || !num64) {
		O3D_MemoryManagerMutex.unlock();
        O3D_ERROR(E_InvalidParameter("None null block number required"));
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

    if (m_fastMemoryPool.m_pool16) {
		delete ((BlockPooler<16,16,4>*)m_fastMemoryPool.m_pool16);
        m_fastMemoryPool.m_pool16 = nullptr;
	}

    if (m_fastMemoryPool.m_pool32) {
		delete ((BlockPooler<32,16,5>*)m_fastMemoryPool.m_pool32);
        m_fastMemoryPool.m_pool32 = nullptr;
	}

    if (m_fastMemoryPool.m_pool64) {
		delete ((BlockPooler<64,16,6>*)m_fastMemoryPool.m_pool64);
        m_fastMemoryPool.m_pool64 = nullptr;
	}

	O3D_MemoryManagerMutex.unlock();
}

// Fast allocation of small memory block. Always 16 bytes aligned.
void* MemoryManager::fastAlloc(size_t size)
{
	O3D_ASSERT(m_fastMemoryPool.m_status);
	O3D_ASSERT(size <= 64);

    void* newptr = nullptr;

    if (size <= 16) {
		newptr = ((BlockPooler<16,16,4>*)m_fastMemoryPool.m_pool16)->allocate();
    } else if (size <= 32) {
		newptr = ((BlockPooler<32,16,5>*)m_fastMemoryPool.m_pool32)->allocate();
    } else {
		newptr = ((BlockPooler<64,16,6>*)m_fastMemoryPool.m_pool64)->allocate();
    }

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

    void* newptr = nullptr;

    if (size <= 16) {
		newptr = ((BlockPooler<16,16,4>*)m_fastMemoryPool.m_pool16)->allocate();
    } else if (size <= 32) {
		newptr = ((BlockPooler<32,16,5>*)m_fastMemoryPool.m_pool32)->allocate();
    } else {
		newptr = ((BlockPooler<64,16,6>*)m_fastMemoryPool.m_pool64)->allocate();
    }

	// Add a new block info
    FastMemoryPool::BlockInfo newBlock;
    newBlock.Size  = size;
    newBlock.File  = &file[fileNamePos];
    newBlock.Line  = line;
    m_fastMemoryPool.m_blocks[newptr] = newBlock;

	m_memory[MEM_FAST].m_memorytotalbytes += (UInt32)size;
	m_memory[MEM_FAST].m_currentbytes += (UInt32)size;

    if (m_memory[MEM_FAST].m_currentbytes > m_memory[MEM_FAST].m_maxpeak) {
		m_memory[MEM_FAST].m_maxpeak = m_memory[MEM_FAST].m_currentbytes;
    }

	++m_memory[MEM_FAST].m_nbrNew;

	O3D_MemoryManagerMutex.unlock();
	return newptr;
}

// Release a previously fast allocation
void MemoryManager::freeFastAlloc(void* ptr, size_t size)
{
	O3D_ASSERT(m_fastMemoryPool.m_status);
	O3D_ASSERT(size <= 64);

    if (size <= 16) {
		((BlockPooler<16,16,4>*)m_fastMemoryPool.m_pool16)->free(ptr);
    } else if (size <= 32) {
		((BlockPooler<32,16,5>*)m_fastMemoryPool.m_pool32)->free(ptr);
    } else {
		((BlockPooler<64,16,6>*)m_fastMemoryPool.m_pool64)->free(ptr);
    }

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
    FastMemoryPool::BlockInfoMap::iterator it = m_fastMemoryPool.m_blocks.find(ptr);
    O3D_ASSERT(it != m_fastMemoryPool.m_blocks.end());

    m_memory[MEM_FAST].m_currentbytes -= (UInt32)it->second.Size;
	++m_memory[MEM_FAST].m_nbrDelete;

    m_fastMemoryPool.m_blocks.erase(it);

    if (size <= 16) {
		((BlockPooler<16,16,4>*)m_fastMemoryPool.m_pool16)->free(ptr);
    } else if (size <= 32) {
		((BlockPooler<32,16,5>*)m_fastMemoryPool.m_pool32)->free(ptr);
    } else {
		((BlockPooler<64,16,6>*)m_fastMemoryPool.m_pool64)->free(ptr);
    }

	O3D_MemoryManagerMutex.unlock();
}

MemoryManager::FastMemoryPool::~FastMemoryPool()
{
    if (m_pool16) {
		delete ((BlockPooler<16,16,4>*)m_pool16);
    }

    if (m_pool32) {
		delete ((BlockPooler<32,16,5>*)m_pool32);
    }

    if (m_pool64) {
		delete ((BlockPooler<64,16,6>*)m_pool64);
    }
}

void MemoryManager::FastMemoryPool::reportLeaks(Logger *logger)
{
	size_t TotalSize = 0;

    logger->log(Logger::INFO, "Fast-pool Memory Leaks :");

    for (BlockInfoMap::iterator i = m_blocks.begin(); i != m_blocks.end(); ++i) {
		// add memory size
		TotalSize += i->second.Size;

		// write block information in file
        logger->log(Logger::INFO, String("-> @0x{0}").arg((UInt64)i->first, 16, 16, '0')
               << String(" | {0} bytes").arg((Int32)(i->second.Size), 7, 10, ' ')
               << String(" | {0} ({1})").arg(i->second.File.c_str()).arg(i->second.Line));
	}

	// draw all memory leak sum
    logger->log(Logger::INFO, String("-- {0} none free block(s), {1} bytes --")
                .arg((Int32)m_blocks.size())
                .arg((Int32)TotalSize));
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
    TSBlock newBlock;
    newBlock.Size = size;
    newBlock.File = &file[fileNamePos];
    newBlock.Line = fine;
    m_GBlocks[TGKey(target,id)] = newBlock;

	m_memory[MEM_GFX].m_memorytotalbytes += (UInt32)size;
	m_memory[MEM_GFX].m_currentbytes += (UInt32)size;

    if (m_memory[MEM_GFX].m_currentbytes > m_memory[MEM_GFX].m_maxpeak) {
		m_memory[MEM_GFX].m_maxpeak = m_memory[MEM_GFX].m_currentbytes;
    }

	++m_memory[MEM_GFX].m_nbrNew;

	// Serialization
    if (m_memory[MEM_GFX].m_logging && (size >= m_memory[MEM_GFX].m_minLogSize)) {

        m_logger->log(Logger::INFO, String("+G GAllocation   | {0}:{1}").arg(gfxType2String(target))
                      .arg(id, 6, 10, ' ').arg(id, 6, 10, ' ')
                      << String(" | {0} bytes").arg((Int32)newBlock.Size, 7, 10, ' ')
                      << String(" | {0} ({1})").arg(newBlock.File.c_str()).arg(newBlock.Line));
	}

	O3D_MemoryManagerMutex.unlock();
}

// unregister a graphic memory allocation
void MemoryManager::gfxFree(GraphicTarget target,UInt32 id)
{
    if(m_GBlocks.empty()) {
		O3D_MemoryManagerMutex.unlock();

        m_logger->log(Logger::INFO, String("!! GFree MAPEMPTY| {0}:{1}").arg(gfxType2String(target))
                      .arg(id, 6, 10, ' ').arg(id, 6, 10, ' '));
		return;
	}

	// get memory block in the map
    TGBlockMap::iterator it = m_GBlocks.find(TGKey(target,id));

	// unallocated block ? error
    if (it == m_GBlocks.end()) {
		O3D_MemoryManagerMutex.unlock();
        m_logger->log(Logger::INFO, String("!! GFree.BADBLOCK| {0}:{1}").arg(gfxType2String(target)).arg(id, 6, 10, ' '));
        return;
	}

	// Serialization
    if (m_memory[MEM_GFX].m_logging && ((UInt32)(it->second.Size) >= m_memory[MEM_GFX].m_minLogSize) &&
            m_memory[MEM_GFX].m_deleteStack.size()) {

        m_logger->log(Logger::INFO, String("-G GFree         | {0}:{1}").arg(gfxType2String(target))
                      .arg(id, 6, 10, ' ').arg(id, 6, 10, ' ')
            << String(" | {0} bytes").arg((Int32)it->second.Size, 7, 10, ' ')
            << String(" | {0} ({1})")
                      .arg(m_memory[MEM_GFX].m_deleteStack.top().File.c_str())
                      .arg(m_memory[MEM_GFX].m_deleteStack.top().Line));
	}

    m_memory[MEM_GFX].m_currentbytes -= (UInt32)it->second.Size;
	++m_memory[MEM_GFX].m_nbrDelete;

    m_GBlocks.erase(it);
    if (m_memory[MEM_GFX].m_deleteStack.size() != 0) {
		m_memory[MEM_GFX].m_deleteStack.pop();
    }

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
    TGBlockMap::iterator it = m_GBlocks.find(TGKey(target,id));

	// unallocated block ? error
    if (it == m_GBlocks.end()) {
		O3D_MemoryManagerMutex.unlock();
        m_logger->log(Logger::INFO, String("!! GReal.BADBLOCK| {0}:{1}").arg(gfxType2String(target)).arg(id, 6, 10, ' '));
		return;
	}

    m_memory[MEM_GFX].m_currentbytes -= (UInt32)it->second.Size;
	++m_memory[MEM_GFX].m_nbrDelete;

    if (m_memory[MEM_GFX].m_currentbytes > m_memory[MEM_GFX].m_maxpeak) {
		m_memory[MEM_GFX].m_maxpeak = m_memory[MEM_GFX].m_currentbytes;
    }

	// Change les infos du bloque existant
    it->second.Size = size;
    it->second.File = &file[fileNamePos];
    it->second.Line = line;

	// Serialization
    if (m_memory[MEM_GFX].m_logging && (size >= m_memory[MEM_GFX].m_minLogSize) &&
            (m_memory[MEM_GFX].m_deleteStack.size() != 0)) {

        m_logger->log(Logger::INFO, String("*G GReallocate   | {0}:{1}").arg(gfxType2String(target)).arg(id, 6, 10, ' ')
            << String(" | {0} bytes").arg((Int32)it->second.Size, 7, 10, ' ')
            << String(" | {0} ({1})").arg(it->second.File.c_str()).arg(it->second.Line));
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
    TGBlock newBlock;
    newBlock.Size = size;
    newBlock.File = &file[fileNamePos];
    newBlock.Line = line;
    m_SBlocks[TSKey(target,id)] = newBlock;

	m_memory[MEM_SFX].m_memorytotalbytes += (UInt32)size;
	m_memory[MEM_SFX].m_currentbytes += (UInt32)size;

    if (m_memory[MEM_SFX].m_currentbytes > m_memory[MEM_SFX].m_maxpeak) {
		m_memory[MEM_SFX].m_maxpeak = m_memory[MEM_SFX].m_currentbytes;
    }

	++m_memory[MEM_SFX].m_nbrNew;

	// Serialization
    if (m_memory[MEM_SFX].m_logging && (size >= m_memory[MEM_SFX].m_minLogSize)) {

        m_logger->log(Logger::INFO, String("+A AAllocation   || {0}:{1}").arg(sfxType2String(target))
                      .arg(id, 6, 10, ' ').arg(id, 6, 10, ' ')
                      << String(" | {0} bytes").arg((Int32)newBlock.Size, 7, 10, ' ')
                      << String(" | {0} ({1})").arg(newBlock.File.c_str()).arg(newBlock.Line));

	}

	O3D_MemoryManagerMutex.unlock();
}

// unregister a sound memory allocation
void MemoryManager::sfxFree(AudioTarget target,UInt32 id)
{
    if(m_SBlocks.empty()){
		O3D_MemoryManagerMutex.unlock();

        m_logger->log(Logger::INFO, String("!! GFree MAPEMPTY| {0}:{1}").arg(sfxType2String(target))
                      .arg(id, 6, 10, ' ').arg(id, 6, 10, ' '));
		return;
	}

	// get memory block in the map
    TSBlockMap::iterator it = m_SBlocks.find(TSKey(target,id));

	// unallocated block ? error
    if (it == m_SBlocks.end()) {
		O3D_MemoryManagerMutex.unlock();
        m_logger->log(Logger::INFO, String("!! AFree.BADBLOCK| {0}:{1}").arg(sfxType2String(target)).arg(id, 6, 10, ' '));
		return;
	}

	// Serialization
    if (m_memory[MEM_SFX].m_logging && ((UInt32)(it->second.Size) >= m_memory[MEM_SFX].m_minLogSize) &&
            m_memory[MEM_SFX].m_deleteStack.size()) {

        m_logger->log(Logger::INFO, String("-A AFree         | {0}:{1}").arg(sfxType2String(target))
                      .arg(id, 6, 10, ' ').arg(id, 6, 10, ' ')
            << String(" | {0} bytes").arg((Int32)it->second.Size, 7, 10, ' ')
            << String(" | {0} ({1})")
                      .arg(m_memory[MEM_SFX].m_deleteStack.top().File.c_str())
                      .arg(m_memory[MEM_SFX].m_deleteStack.top().Line));
	}

    m_memory[MEM_SFX].m_currentbytes -= (UInt32)it->second.Size;
	++m_memory[MEM_SFX].m_nbrDelete;

    m_SBlocks.erase(it);
    if (m_memory[MEM_SFX].m_deleteStack.size() != 0) {
		m_memory[MEM_SFX].m_deleteStack.pop();
    }

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
    TSBlockMap::iterator it = m_SBlocks.find(TSKey(target,id));

	// unallocated block ? error
    if (it == m_SBlocks.end()) {
		O3D_MemoryManagerMutex.unlock();
        m_logger->log(Logger::INFO, String("!! AReal.BADBLOCK| {0}:{1}").arg(sfxType2String(target)).arg(id, 6, 10, ' '));
        return;
	}

    m_memory[MEM_SFX].m_currentbytes -= (UInt32)it->second.Size;
	++m_memory[MEM_SFX].m_nbrDelete;

    if (m_memory[MEM_SFX].m_currentbytes > m_memory[MEM_SFX].m_maxpeak) {
		m_memory[MEM_SFX].m_maxpeak = m_memory[MEM_SFX].m_currentbytes;
    }

	// Change les infos du bloque existant
    it->second.Size = size;
    it->second.File = &file[fileNamePos];
    it->second.Line = line;

	// Serialization
    if (m_memory[MEM_SFX].m_logging && (size >= m_memory[MEM_SFX].m_minLogSize) &&
            (m_memory[MEM_SFX].m_deleteStack.size() != 0)) {

        m_logger->log(Logger::INFO, String("*& AReallocate   | {0}:{1}").arg(sfxType2String(target)).arg(id, 6, 10, ' ')
            << String(" | {0} bytes").arg((Int32)it->second.Size, 7, 10, ' ')
            << String(" | {0} ({1})").arg(it->second.File.c_str()).arg(it->second.Line));
	}

    if (m_memory[MEM_SFX].m_deleteStack.size() != 0) {
		m_memory[MEM_SFX].m_deleteStack.pop();
    }

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
    if (O3D_MemoryManagerTime[type].check()) {
        if (m_memory[type].m_currentbytes == m_memory[type].m_prevCurrentBytes) {
			m_memory[type].m_memState = STABLE;
        } else if (m_memory[type].m_hysteris != 0) {
			UInt32 _min = m_memory[type].m_prevCurrentBytes - m_memory[type].m_hysteris;
			UInt32 _max = m_memory[type].m_prevCurrentBytes + m_memory[type].m_hysteris;

            if ((m_memory[type].m_currentbytes >= _min) && (m_memory[type].m_currentbytes <= _max)) {
				m_memory[type].m_memState = NEAR_STABLE;
            } else if (m_memory[type].m_currentbytes > m_memory[type].m_prevCurrentBytes) {
				m_memory[type].m_memState = INCREASE;
            } else if (m_memory[type].m_currentbytes < m_memory[type].m_prevCurrentBytes) {
				m_memory[type].m_memState = DECREASE;
            }
        } else if (m_memory[type].m_currentbytes > m_memory[type].m_prevCurrentBytes) {
			m_memory[type].m_memState = INCREASE;
        } else if (m_memory[type].m_currentbytes < m_memory[type].m_prevCurrentBytes) {
			m_memory[type].m_memState = DECREASE;
        }

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

    if (O3D_MemoryManagerTimeCounter[type].check()) {
		news = (Float)m_memory[type].m_nbrNew / O3D_MemoryManagerTimeCounter[type].elapsed();
		deletes = (Float)m_memory[type].m_nbrDelete / O3D_MemoryManagerTimeCounter[type].elapsed();

		m_memory[type].m_nbrNew = m_memory[type].m_nbrDelete = 0;

		O3D_MemoryManagerMutex.unlock();
		return True;
	}

	O3D_MemoryManagerMutex.unlock();
	return False;
}
