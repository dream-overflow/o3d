/**
 * @file memorymanager.h
 * @brief Memory manager singleton.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2004-12-13
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MEMORYMANAGER_H
#define _O3D_MEMORYMANAGER_H

#include "base.h"

#include <map>
#include <string>
#include <fstream>
#include <list>
#include <stack>

#ifdef _MSC_VER
#pragma warning(disable:4251)
#endif

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class MemoryManager
//-------------------------------------------------------------------------------------
//! Memory manager for log, reports memories allocations and to detect memory
//! leaks. It also provide some current value like allocations total size, peaks, and
//! a fast memory allocator for small memory blocks of fixed size (16,32,64bytes)
//---------------------------------------------------------------------------------------
class O3D_API MemoryManager
{
public:

	//! Graphic targets.
	enum GraphicTarget
	{
		GPU_TEXTURE_1D = 0,
		GPU_TEXTURE_2D,
		GPU_TEXTURE_3D,
		GPU_TEXTURE_CUBEMAP,
		GPU_TEXTURE_1D_ARRAY,
		GPU_TEXTURE_2D_ARRAY,
        GPU_TEXTURE_2D_MULTISAMPLE,
        GPU_TEXTURE_2D_MULTISAMPLE_ARRAY,
		GPU_VBO,
		GPU_FBO,
		GPU_PBO,
	};

	//! Sound targets.
	enum AudioTarget
	{
		SFX_SINGLE_BUFFER = 0,
		SFX_STREAM_BUFFER
	};

	// -------------------------------------------------------------------------------
	// Central Memory Operations
	// -------------------------------------------------------------------------------

	//! add a memory allocation
	void* allocate(size_t size, const Char* file, Int32 line, Bool array);

	//! aligned memory allocation
	void* alignedAllocate(size_t size, size_t align, const Char* file, Int32 line);

	//! reallocate a block of memory
	void* reallocate(void* ptr, size_t size, const Char* file, Int32 line);

	//! reallocate an aligned block of memory
	void* alignedReallocate(void* ptr, size_t size, size_t align, const Char* file, Int32 line);

	//! free a memory allocation
	void free(void* ptr, Bool array);

	//! aligned free
	void alignedFree(void* ptr);


	// -------------------------------------------------------------------------------
	// Fast allocator Operations
	// -------------------------------------------------------------------------------

	//! define the fast allocator preallocated size (should be <= 65536)
	//! @param num16 number of block of 16 bytes (16 bytes aligned)
	//! @param num32 number of block of 32 bytes (16 bytes aligned)
	//! @param num64 number of block of 64 bytes (16 bytes aligned)
	//! @return True if successfully initialized
	void initFastAllocator(size_t num16, size_t num32, size_t num64);

	//! Release the fast memory allocator
	void releaseFastAllocator();

	//! Fast allocation of small memory block. Always 16 bytes aligned.
	void* fastAlloc(size_t size);

	//! Fast allocation of small memory block. Always 16 bytes aligned.
	//! @note Debug version
	void* fastAlloc(size_t size, const Char* file, Int32 line);

	//! Release a previously fast allocation
	void freeFastAlloc(void* ptr, size_t size);

	//! Release a previously fast allocation in memory debug mode
	void freeFastAlloc(void* ptr, size_t size, const Char* file, Int32 line);


	// -------------------------------------------------------------------------------
	// Graphic Memory Operations
	// -------------------------------------------------------------------------------

	//! register a graphic memory allocation
	void gfxAlloc(
			GraphicTarget target,
			UInt32 id,
			size_t size,
			const Char *file,
			Int32 line);

	//! unregister a graphic memory allocation
	void gfxFree(GraphicTarget target,UInt32 id);

	//! re-register a graphic memory allocation
	void gfxRealloc(
			GraphicTarget target,
			UInt32 id,
			size_t size,
			const Char *file,
			Int32 line);


	// -------------------------------------------------------------------------------
	// Audio Memory Operations
	// -------------------------------------------------------------------------------

	//! register an audio memory allocation
	void sfxAlloc(
			AudioTarget target,
			UInt32 id,
			size_t size,
			const Char *file,
			Int32 line);

	//! unregister an audio memory allocation
	void sfxFree(AudioTarget target,UInt32 id);

	//! re-register an audio memory allocation
	void sfxRealloc(
			AudioTarget target,
			UInt32 id,
			size_t size,
			const Char *file,
			Int32 line);


	// -------------------------------------------------------------------------------
	// Globals Operations
	// -------------------------------------------------------------------------------

	//! Sound memory type.
	enum MemoryType
	{
		MEM_RAM = 0,  //!< central memory (RAM)
		MEM_GFX = 1,  //!< graphic memory (GPU RAM)
		MEM_FAST = 2, //!< fast memory pool (RAM)
		MEM_SFX = 3   //!< audio memory (SPU RAM)
	};

	//! Number of memory type.
	static const UInt32 NUM_MEMORY_TYPE = MEM_SFX + 1;

	//! Memory state.
	enum MemoryState
	{
		INCREASE,      //!< memory occupation increase
		DECREASE,      //!< memory occupation decrease
		STABLE,        //!< memory occupation is stable
		NEAR_STABLE    //!< memory occupation is near of stable
	};

	//! get class instance (singleton)
	static MemoryManager* instance();

	//! backup info from current free
	void nextDelete(MemoryType type, const Char* File, Int32 Line);

	//! enable log new/delete
	void enableLog(MemoryType type,UInt32 minLogSize=0);
	//! disable log new/delete
	void disableLog(MemoryType type);
	//! is log new/delete
	Bool isLogging(MemoryType type);

	//! get the memory evolution
	MemoryState getMemoryState(MemoryType type);

	//! set the memory evolution state delay (in ms)
	void setMemoryRefreshDelay(MemoryType type,UInt32 ms);
	//! get the memory evolution state delay (in ms)
	UInt32 getMemoryRefreshDelay(MemoryType type) const;

	//! set the memory evolution state hysteresis
	void setMemoryStateHystereris(MemoryType type,UInt32 h);
	//! get the memory evolution state hysteresis
	UInt32 getMemoryStateHystereris(MemoryType type) const;

	//! get current memory occupation
	UInt32 getCurrentMemorySize(MemoryType type) const;

	//! get total memory allocation
	UInt32 getTotalMemorySize(MemoryType type) const;

	//! get maximum memory allocation peak
	UInt32 getMemoryPeakSize(MemoryType type) const;

	//! reset memory states
	void resetCounters(MemoryType type);

	//! get number of new/delete operations per second (approximative if called each more one sec)
	Bool updateOpPerSec(MemoryType type, Float &news, Float &deletes);

private:

	//! default constructor
	MemoryManager();
	//! destructor
	~MemoryManager();

	//! no copy possible
	MemoryManager(MemoryManager&);
	void operator =(MemoryManager&);

	//! write memory leak report
	void reportLeaks();

	std::ofstream m_file;        //!< output file

	//! memory block structure
    struct TBlock
	{
		size_t Size;      //!< memory size allocated.
		size_t Align;     //!< memory alignment (0 mean default alignment).
		std::string File; //!< file where the allocation is processed.
		Int32 Line;   //!< allocation line code
		Bool Array;   //!< array or single item ?
	};

	typedef std::map<void*, TBlock> TBlockMap;

	//! memory block structure key
	template <class T>
	struct TKey
	{
		T Target;
		UInt32 Id;

		TKey(T target, UInt32 id) :
			Target(target),
			Id(id)
		{}

		inline Bool operator== (const TKey& cmp)const
		{
			return ((Target == cmp.target) && (Id == cmp.Id));
		}

		inline Bool operator< (const TKey& cmp)const
		{
			if (Target == cmp.Target)
				return (Id < cmp.Id);

			return (Target < cmp.Target);
		}
	};

	typedef TKey<GraphicTarget> TGKey;
	typedef TKey<AudioTarget> TSKey;

	//! simple memory block structure
    struct TGBlock
	{
		size_t Size;      //!< memory size allocated
		std::string File; //!< file where the allocation is processed.
		Int32 Line;   //!< allocation line code
	};

    typedef TGBlock TSBlock;

	typedef std::map<TGKey, TGBlock> TGBlockMap;
	typedef std::map<TSKey, TSBlock> TSBlockMap;

	//! structure containing informations about a memory module
	struct MemoryModule
	{
		std::stack<TBlock> m_deleteStack; //!< stack whose contain the line and file of next free

		Bool m_logging;                  //!< is allocation are logged
		UInt32 m_minLogSize;             //!< minimal allocated size to process a logging

		// Central memory RAM
		UInt32 m_memorytotalbytes;       //!< total memory allocation since the starting
		UInt32 m_currentbytes;           //!< actual memory occupation size
		UInt32 m_maxpeak;                //!< the maximum peak of memory occupation

		UInt32 m_nbrNew;                 //!< number of new operations per second
		UInt32 m_nbrDelete;              //!< number of delete operations per second

		UInt32 m_hysteris;               //!< memory progression hysteresis
		UInt32 m_prevCurrentBytes;       //!< previous memory occupation size
		MemoryState m_memState;              //!< memory progression

		// default ctor
		MemoryModule() :
			m_logging(False),
			m_minLogSize(1),
			m_memorytotalbytes(0),
			m_currentbytes(0),
			m_maxpeak(0),
			m_nbrNew(0),
			m_nbrDelete(0),
			m_hysteris(1000),
			m_prevCurrentBytes(0),
			m_memState(INCREASE)
		{}
	};

	TBlockMap  m_CBlocks;    //!< central memory blocks
	TGBlockMap m_GBlocks;    //!< graphical memory blocks
	TSBlockMap m_SBlocks;    //!< sound memory blocks

	//! Fast memory pool structure
	//! @note There is no logging of allocation/free for the fast pool
	struct FastMemoryPool
	{
		//! fast memory pool block structure
		struct BlockInfo
		{
			size_t Size;       //!< memory size allocated
			std::string File;  //!< allocation file code
			Int32  Line;   //!< allocation line code
		};

		typedef std::map<void*,BlockInfo> BlockInfoMap;

		FastMemoryPool() :
			m_status(False),
			m_pool16(NULL),
			m_pool32(NULL),
			m_pool64(NULL)
		{}

		~FastMemoryPool();

		void reportLeaks(std::ofstream &file);

		Bool m_status;

		void* m_pool16;             //!< 16 bytes blocks pool aligned 16 bytes
		void* m_pool32;             //!< 32 bytes blocks pool aligned 16 bytes
		void* m_pool64;             //!< 64 bytes blocks pool aligned 16 bytes

		BlockInfoMap m_blocks;      //!< fast central memory pool blocks
	};

	FastMemoryPool m_fastMemoryPool;

	MemoryModule m_memory[NUM_MEMORY_TYPE];   //!< memory volumes

	//! Conversion from graphic target to string.
	const Char* gfxType2String(GraphicTarget target);

	//! Conversion from sound target to string.
	const Char* sfxType2String(AudioTarget target);
};

} // namespace o3d

#endif // _O3D_MEMORYMANAGER_H

