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
#include "o3d/core/serialize.h"

using namespace o3d;

Closable::~Closable()
{

}

Serialize::~Serialize()
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


//inline static void _enablePerformanceCounter()
//{
//#if defined(O3D_ARM64) || defined(O3D_ARM32)
//    asm ("MCR p15, 0, %0, C9, C14, 0\n\t" :: "r"(1));
//    asm ("MCR p15, 0, %0, C9, C14, 2\n\t" :: "r"(0x8000000f));
//#endif
//}

//inline static void _disablePerformanceCounter()
//{
//#if defined(O3D_ARM64) || defined(O3D_ARM32)
//    asm ("MCR p15, 0, %0, C9, C14, 0\n\t" :: "r"(0));
//    asm ("MCR p15, 0, %0, C9, C14, 2\n\t" :: "r"(0x0000000f));
//#endif
//}

#if defined(O3D_ARM32) || defined(O3D_ARM64)
#include <sys/time.h>
#endif

//! return processor cycles numbers
//! @note ARM : http://liuluheng.github.io/wiki/public_html/Embedded-System/Cortex-A8/Performance%20Monitor%20Control%20Register.html
//! and http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0344k/Bgbjjhaj.html
//! that is not working on Android devices, so try with a clock_gettime (to be continued...)
#ifdef _MSC_VER
#pragma warning(once: 4035)
#endif
inline static UInt64 _getCycleNumber()
{
#if defined(_MSC_VER) && defined(O3D_IX32)
    // VC++ x86
    __asm { RDTSC }
#elif defined(_MSC_VER) && defined(O3D_IX64)
    // VC++ x64
    return __rdtsc();
#elif defined(__APPLE__)
    // Apple computer
    return mach_absolute_time();
#elif defined(__GNUC__)
  // GCC Intel based compilers
  #ifdef O3D_IX64
    UInt64 x;
    __asm__ volatile ("rdtsc" : "=a"(x));
    return x;
  #elif defined(O3D_IX32)
    UInt64 x;
    UInt32 a, d;
    __asm__ volatile ("rdtsc" : "=a"(a), "=d"(d));
    return a | (d << 32);
  #elif defined(O3D_ARM64)
    // UInt64 value;
    // asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(value));
    // return value;
    struct timespec time;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time);
    return (UInt64)time.tv_sec * 1000000000LL + (UInt64)time.tv_nsec;
  #elif defined(O3D_ARM32)
    // UInt32 value;
    // asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(value));
    // return (UInt64)value;
    struct timespec time;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time);
    return (UInt64)time.tv_sec * 1000000000LL + (UInt64)time.tv_nsec;
  #else
    #error "<< Unknown architecture ! >>"
  #endif
#else
  #error "<< Unknown architecture ! >>"
#endif
}

// return approximate processor frequency in MHz
Float System::getProcessorFrequency()
{
    Int64 t1, t2, tf;
    UInt64 c1, c2;

    // _enablePerformanceCounter();

	tf = System::getTimeFrequency();
	t1 = System::getTime();
    c1 = _getCycleNumber();

#if defined(_MSC_VER) && (defined(O3D_IX32) || defined(O3D_IX64))
    // VC++ x86
	__asm {
		MOV EBX,O3D_PROC_FREQ_LOOP
		WaitAlittle:
		DEC EBX
		JNZ WaitAlittle
 	}
#elif defined(__GNUC__) && (defined(O3D_IX32) || defined(O3D_IX64))
    // GCC Intel based compilers
    __asm__ volatile ("movl $0x5000000,%%ecx" : : : "%ecx");
    __asm__ volatile ("WaitAlittle:");
    __asm__ volatile ("dec %%ecx" : : : "%ecx");
    __asm__ volatile ("jnz WaitAlittle");
#else
    // __APPLE__, ARM...
	volatile Int32 i;
    for (i = O3D_PROC_FREQ_LOOP; i--;) {}
#endif

    c2 = _getCycleNumber();
    t2 = System::getTime();

    // _disablePerformanceCounter();

	// return an approximate processor frequency
    return ((Float)((c2 - c1 + 1) * tf) / (Float)(t2 - t1) / System::getTimeFrequency()) * 1000.f;
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

System::Plateform System::getPlatform()
{
#ifdef O3D_ANDROID
    return PLATEFORM_ANDROID;
#elif defined(O3D_WINDOWS)
    return PLATEFORM_WINDOWS;
#elif defined(O3D_LINUX)
    return PLATEFORM_LINUX;
#elif defined(O3D_MACOSX)
    return PLATEFORM_MACOSX;
#endif
}

// @todo how to be more specific, windows tablet vs phone, android tablet vs phone
System::Profile System::getProfile()
{
#ifdef O3D_ANDROID
    return PROFILE_MOBILE;
#elif defined(O3D_WINDOWS) || defined(O3D_LINUX) || defined(O3D_MACOSX)
    return PROFILE_DESKTOP;
#elif defined(O3D_PS4) || defined(O3D_XBOXONE)
    return PROFILE_CONSOLE_HIGH;
#elif defined(O3D_SWITCH)
    return PROFILE_CONSOLE;
#endif
}
