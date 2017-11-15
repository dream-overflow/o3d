/**
 * @file base.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include "o3d/core/architecture.h"
#include "o3d/core/base.h"
#include "o3d/core/string.h"
#include "o3d/core/closable.h"

#include <iostream>

using namespace o3d;

Closable::~Closable()
{

}

Int32 o3d::log2(UInt32 n)
{
    Int32 pos = 0;
    if (n >= 1 << 16) {
		n >>= 16;
		pos += 16;
	}
    if (n >= 1 << 8) {
		n >>= 8;
		pos += 8;
	}
    if (n >= 1 << 4) {
		n >>= 4;
		pos += 4;
	}
    if (n >= 1 << 2) {
		n >>= 2;
		pos += 2;
	}
    if (n >= 1 << 1) {
		pos += 1;
	}
	return ((n == 0) ? (-1) : pos);

//	// or
//	Int32 r = 0;
//	while ((n >> r) != 0) {
//		++r;
//	}
//	return r-1; // returns -1 for n==0
}

//---------------------------------------------------------------------------------------
// System
//---------------------------------------------------------------------------------------

// return approximate processor frequency in MHz
Float System::getProcessorFrequency()
{
    Int64 t1,t2,tf;
    Int64 c1,c2;

	tf = System::getTimeFrequency();
	t1 = System::getTime();
	c1 = System::getCycleNumber();

#if defined(O3D_VC_COMPILER) && defined(O3D_WIN32) /* VC++ x86 */
	__asm {
		MOV EBX,O3D_PROC_FREQ_LOOP
		WaitAlittle:
		DEC EBX
		JNZ WaitAlittle
 	}
#elif defined(__amd64__) || defined(__x86_64__) /* GCC Intel based compilers */
	__asm__ volatile ("movl $0x5000000,%%ecx" : : : "%ecx");
	__asm__ volatile ("WaitAlittle:");
	__asm__ volatile ("dec %%ecx" : : : "%ecx");
	__asm__ volatile ("jnz WaitAlittle");
#else /* __APPLE__, WINDOWS x64... */
	volatile Int32 i;
	for (i=O3D_PROC_FREQ_LOOP; i--;) {}
#endif

	t2 = System::getTime();
	c2 = System::getCycleNumber();

	// return an approximate processor frequency
    return ((Float)((c2 - c1 + 1) * tf) / (Float)(t2 - t1) / System::getTimeFrequency());
}

void System::checking()
{
	// Check types size
    static_assert(sizeof(Char)==1, "Invalid Char size");
    static_assert(sizeof(Int8)==1, "Invalid Int8 size");
    static_assert(sizeof(UInt8)==1, "Invalid UInt8 size");
    static_assert(sizeof(Int16)==2, "Invalid Int16 size");
    static_assert(sizeof(UInt16)==2, "Invalid UInt16 size");
    static_assert(sizeof(Int32)==4, "Invalid Int32 size");
    static_assert(sizeof(UInt32)==4, "Invalid UInt32 size");
    static_assert(sizeof(Int64)==8, "Invalid Int64 size");
    static_assert(sizeof(UInt64)==8, "Invalid UInt64 size");
    static_assert(sizeof(Float)==4, "Invalid Float size");
    static_assert(sizeof(Double)==8, "Invalid Double size");
    static_assert(sizeof(Bool)==1, "Invalid Bool size");

#ifdef O3D_WINDOWS // WINDOWS is 2 bytes wide-char
    static_assert(sizeof(WChar)==2, "Invalid WChar size");
#else
    static_assert(sizeof(WChar)==4, "Invalid WChar size");
#endif

    static_assert(True == true && False == false, "Invalid Bool const values");
}
