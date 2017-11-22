/**
 * @file memorydbg.h
 * @brief memory state.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2004-12-13
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MEMORYDBG_H
#define _O3D_MEMORYDBG_H

// By default use posix memalign
#define O3D_USE_MEMALIGN

// no global RAM memory manager
#if defined(O3D_DISABLE_RAM_MEMORY_MANAGER) && defined(O3D_RAM_MEMORY_MANAGER)
	#undef O3D_RAM_MEMORY_MANAGER
#endif

// no 'FAST' memory manager
#if defined(O3D_DISABLE_FAST_MEMORY_MANAGER) && defined(O3D_FAST_MEMORY_MANAGER)
	#undef O3D_FAST_MEMORY_MANAGER
#endif

// no GPU memory manager
#if defined(O3D_DISABLE_GPU_MEMORY_MANAGER) && defined(O3D_GPU_MEMORY_MANAGER)
	#undef O3D_GPU_MEMORY_MANAGER
#endif

// no SPU memory manager
#if defined(O3D_DISABLE_SPU_MEMORY_MANAGER) && defined(O3D_SPU_MEMORY_MANAGER)
	#undef O3D_SPU_MEMORY_MANAGER
#endif

//! Check aligned
#define O3D_CHECK_ALIGN(PTR,ALIGN) (((PTR) & (ALIGN-1)) == 0)

// bytes alignment
#if (defined(__UNIX__) || defined(__APPLE__) || defined (__MINGW32__))
    #define O3D_ALIGN(X) __attribute__ ((aligned(X)))
#else
    #if defined(__INTEL_COMPILER)
        #define O3D_ALIGN(X) _alloca(X)
    #elif defined(_MSC_VER)
        #define O3D_ALIGN(X) __declspec(align(X))
    #endif
#endif

#include <stdlib.h>

//---------------------------------------------------------------------------------------
// Debug memory helpers
//---------------------------------------------------------------------------------------

#include "memorymanager.h"

namespace o3d {

//! malloc function overload. Take a source file and line.
inline void* debugMalloc(size_t size, const Char* file, Int32 line)
{
	return MemoryManager::instance()->allocate(size,file,line,False);
}

//! aligned malloc function overload. Take a source file and line.
inline void* debugAlignedMalloc(
	size_t size,
	size_t Align,
	const Char* file,
	Int32 line)
{
	return MemoryManager::instance()->alignedAllocate(size,Align,file,line);
}

//! realloc function overload. Take a source file and line.
inline void* debugRealloc(
	void* ptr,
	size_t size,
	const Char* file,
	Int32 line)
{
	if (ptr == NULL)
		return o3d::debugMalloc(size,file,line);

	MemoryManager::instance()->nextDelete(MemoryManager::MEM_RAM,file,line);
	return MemoryManager::instance()->reallocate(ptr,size,file,line);
}

//! realloc function overload. Take a source file and line.
inline void* debugAlignedRealloc(
	void* ptr,
	size_t size,
	size_t Align,
	const Char* file,
	Int32 line)
{
	if (ptr == NULL)
		return o3d::debugAlignedMalloc(size,Align,file,line);

	MemoryManager::instance()->nextDelete(MemoryManager::MEM_RAM,file,line);
	return MemoryManager::instance()->alignedReallocate(ptr,size,Align,file,line);
}

//! free function overload. Take a source file and line.
inline void debugFree(void* ptr, const Char* file, Int32 line)
{
    MemoryManager::instance()->nextDelete(MemoryManager::MEM_RAM,file,line);
    MemoryManager::instance()->free(ptr,False);
}

//! free function overload. Take a source file and line.
inline void debugAlignedFree(void* ptr, const Char* file, Int32 line)
{
    MemoryManager::instance()->nextDelete(MemoryManager::MEM_RAM,file,line);
    MemoryManager::instance()->alignedFree(ptr);
}

//! graphic allocation logging
inline void debugGfxAlloc(
		MemoryManager::GraphicTarget Target,
		UInt32 Id,
		UInt32 size,
		const Char* file,
		Int32 line)
{
	MemoryManager::instance()->gfxAlloc(Target, Id, size, file, line);
}

//! graphic free logging
inline void debugGfxFree(
		MemoryManager::GraphicTarget Target,
		UInt32 Id,
		const Char* file,
		Int32 line)
{
	MemoryManager::instance()->nextDelete(MemoryManager::MEM_GFX, file, line);
	MemoryManager::instance()->gfxFree(Target, Id);
}

//! graphic re-allocation logging
inline void debugGfxRealloc(
		MemoryManager::GraphicTarget Target,
		UInt32 Id,
		UInt32 size,
		const Char* file,
		Int32 line)
{
	MemoryManager::instance()->nextDelete(MemoryManager::MEM_GFX, file, line);
	MemoryManager::instance()->gfxRealloc(Target, Id, size, file, line);
}

//! audio allocation logging
inline void debugSfxAlloc(
		MemoryManager::AudioTarget Target,
		UInt32 Id,
		UInt32 size,
		const Char* file,
		Int32 line)
{
	MemoryManager::instance()->sfxAlloc(Target, Id, size, file, line);
}

//! audio free logging
inline void debugSfxFree(
		MemoryManager::AudioTarget Target,
		UInt32 Id,
		const Char* file,
		Int32 line)
{
	MemoryManager::instance()->nextDelete(MemoryManager::MEM_SFX, file, line);
	MemoryManager::instance()->sfxFree(Target, Id);
}

//! audio re-allocation logging
inline void debugSfxRealloc(
		MemoryManager::AudioTarget Target,
		UInt32 Id,
		UInt32 size,
		const Char* file,
		Int32 line)
{
	MemoryManager::instance()->nextDelete(MemoryManager::MEM_SFX, file, line);
	MemoryManager::instance()->sfxRealloc(Target, Id, size, file, line);
}

} // namespace o3d

//---------------------------------------------------------------------------------------
// new/delete operator overload
//---------------------------------------------------------------------------------------

#ifdef O3D_RAM_MEMORY_MANAGER

//! new operator overload
inline void* operator new (size_t size, const Char* file, Int32 line)
{
	return MemoryManager::instance()->allocate(size,file,line,False);
}

//! new[] operator overload
inline void* operator new[] (size_t size, const Char* file, int line)
{
	return MemoryManager::instance()->allocate(size,file,line,True);
}

//! delete operator overload
inline void operator delete (void* ptr)
{
    MemoryManager::instance()->free(ptr,False);
}

//! delete operator overload. Take a source file and line.
inline void operator delete (void* ptr, const Char* file, Int32 line)
{
    MemoryManager::instance()->nextDelete(MemoryManager::MEM_RAM,file,line);
    MemoryManager::instance()->free(ptr,False);
}

//! delete[] operator overload
inline void operator delete[] (void* ptr)
{
    MemoryManager::instance()->free(ptr,True);
}

//! delete[] operator overload. Take a source file and line.
inline void operator delete[] (void* ptr, const Char* file, Int32 line)
{
    MemoryManager::instance()->nextDelete(MemoryManager::MEM_RAM,file,line);
    MemoryManager::instance()->free(ptr,True);
}
#endif // O3D_RAM_MEMORY_MANAGER


//---------------------------------------------------------------------------------------
// Aligned allocator for POSIX platforms
//---------------------------------------------------------------------------------------

namespace o3d {

#ifndef O3D_WINDOWS // suppose POSIX

inline void* alignedMalloc(size_t size, size_t align)
{
#ifdef O3D_USE_MEMALIGN
	void* memptr = NULL;
	posix_memalign((void**)&memptr, align, size);

	return memptr;
#else
	void *p = malloc(size + align - 1 + sizeof(void *));
	void **p2 = (void **)(((long)p + sizeof(void *) + align - 1) & ~(align-1));

	p2[-1] = p;

	return p2;
#endif
}

inline void* alignedRealloc(void *ptr, size_t size, size_t align)
{
	if (!ptr)
		return alignedMalloc(size, align);

#ifdef O3D_USE_MEMALIGN
	return realloc(ptr, size);
#else
	void *p = realloc(((void**)ptr)[-1], size + align - 1 + sizeof(void *));
	void **p2 = (void **)(((long)p + sizeof(void *) + align - 1) & ~(align-1));

	p2[-1] = p;

	return p2;
#endif
}

inline void alignedFree(void *ptr)
{
#ifdef O3D_USE_MEMALIGN
	free(ptr);
#else
	free(((void**)ptr)[-1]);
#endif
}

#endif // O3D_WINDOWS

} // namespace o3d

#endif // _O3D_MEMORYDBG_H

//---------------------------------------------------------------------------------------
//! define the new memory operators with source file and line automated.
//! (out of the guarded token)
//---------------------------------------------------------------------------------------
#ifdef O3D_RAM_MEMORY_MANAGER
#ifndef new
    #define new    new(__FILE__,__LINE__)
    #define delete MemoryManager::instance()->nextDelete(MemoryManager::MEM_RAM, __FILE__, __LINE__), delete
#endif
#ifndef O3D_MALLOC
	#define O3D_MALLOC(_S)     debugMalloc((_S),__FILE__,__LINE__)
	#define O3D_NEW            O3D_MALLOC
	#define O3D_REALLOC(_P,_S) debugRealloc((_P),(_S),__FILE__,__LINE__)
	#define O3D_FREE(_P)       debugFree((_P),__FILE__,__LINE__)

	#define O3D_ALIGNED_MALLOC(_S,_A)     debugAlignedMalloc((_S),(_A),__FILE__,__LINE__)
	#define O3D_ALIGNED_REALLOC(_P,_S,_A) debugAlignedRealloc((_P),(_S),(_A),__FILE__,__LINE__)
	#define O3D_ALIGNED_FREE(_P)          debugAlignedFree((_P),__FILE__,__LINE__)
#endif // O3D_MALLOC
#else
#ifndef O3D_MALLOC
	#define O3D_MALLOC(_S)     malloc((_S))
	#define O3D_NEW            O3D_MALLOC
	#define O3D_REALLOC(_P,_S) realloc((_P),(_S))
	#define O3D_FREE(_P)       free((_P))

#ifdef O3D_WINDOWS
	#define O3D_ALIGNED_MALLOC(_S,_A)     _mm_malloc((_S),(_A))
	#define O3D_ALIGNED_REALLOC(_P,_S,_A) _aligned_realloc((_P),(_S),(_A))
	#define O3D_ALIGNED_FREE(_P)          _mm_free((_P))
#else // suppose POSIX
	#define O3D_ALIGNED_MALLOC(_S,_A)     alignedMalloc((_S),(_A))
	#define O3D_ALIGNED_REALLOC(_P,_S,_A) alignedRealloc((_P),(_S),(_A))
	#define O3D_ALIGNED_FREE(_P)          alignedFree((_P))
#endif // O3D_WINDOWS
#endif // O3D_MALLOC
#endif // O3D_RAM_MEMORY_MANAGER

#ifdef O3D_FAST_MEMORY_MANAGER
	#define O3D_FAST_ALLOC(_S)   MemoryManager::instance()->fastAlloc((_S),__FILE__,__LINE__)
	#define O3D_FAST_FREE(_P,_S) MemoryManager::instance()->freeFastAlloc((_P),(_S),__FILE__,__LINE__)
#else
	#define O3D_FAST_ALLOC(_S)   MemoryManager::instance()->fastAlloc((_S))
	#define O3D_FAST_FREE(_P,_S) MemoryManager::instance()->freeFastAlloc((_P),(_S))
#endif // O3D_FAST_MEMORY_MANAGER

//---------------------------------------------------------------------------------------
// Graphic memory operators
//---------------------------------------------------------------------------------------
#ifdef O3D_GPU_MEMORY_MANAGER
	#define O3D_GALLOC(T,I,S)   debugGfxAlloc((T),(I),(S),__FILE__,__LINE__)
	#define O3D_GFREE(T,I)      debugGfxFree((T),(I),__FILE__,__LINE__)
	#define O3D_GREALLOC(T,I,S) debugGfxRealloc((T),(I),(S),__FILE__,__LINE__)
#else
	#define O3D_GALLOC(T,I,S) o3d::emptyFoo()
	#define O3D_GFREE(T,I) o3d::emptyFoo()
	#define O3D_GREALLOC(T,I,S) o3d::emptyFoo()
#endif // O3D_GPU_MEMORY_MANAGER

//---------------------------------------------------------------------------------------
// Sound memory operators
//---------------------------------------------------------------------------------------
#ifdef O3D_SPU_MEMORY_MANAGER
	#define O3D_SALLOC(T,I,S)   debugSfxAlloc((T),(I),(S),__FILE__,__LINE__)
	#define O3D_SFREE(T,I)      debugSfxFree((T),(I),__FILE__,__LINE__)
	#define O3D_SREALLOC(T,I,S) debugSfxRealloc((T),(I),(S),__FILE__,__LINE__)
#else
	#define O3D_SALLOC(T,I,S) emptyFoo()
	#define O3D_SFREE(T,I) emptyFoo()
	#define O3D_SREALLOC(T,I,S) emptyFoo()
#endif // O3D_SND_MEMORY_MANAGER
