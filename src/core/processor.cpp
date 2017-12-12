/**
 * @file processor.cpp
 * @brief Implementation of Processor.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-12-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/processor.h"

#include "o3d/core/architecture.h"
#include "o3d/core/debug.h"

#if defined(_MSC_VER) && defined(O3D_WIN64)
#include <intrin.h>
#endif

using namespace o3d;

static Int32 _getNumCPU();
static Int32 _getCPUCacheLineSize(const String &cpuType);
static Int32 _getSystemRAMSize();

// Default constructor
Processor::Processor() :
	m_has_mmx(False),
	m_has_mmx_ext(False),
	m_has_3dnow(False),
	m_has_3dnow_ext(False),
	m_has_sse(False),
	m_has_sse2(False),
	m_has_sse3(False),
	m_is_htt(False),
	m_stepping(0),
	m_model(0),
	m_family(0),
	m_type(0),
	m_ext_model(0),
	m_ext_family(0),
	m_cpu_name("unknown"),
	m_cpu_vendor("unknown"),
	m_cpu_speed(0)
{
	m_cpu_speed = System::getProcessorFrequency();

	// CPU was in quiet mode, now it will be awake this time
    if (m_cpu_speed < 0.f || m_cpu_speed > 20000.f) {
		m_cpu_speed = System::getProcessorFrequency();
    }

	doCPUID();

    m_num_cpu = _getNumCPU();
    m_cache_line_size = _getCPUCacheLineSize(m_cpu_vendor);
    m_system_ram_size = _getSystemRAMSize();
}

String Processor::getVendorName() const
{
    if (m_cpu_vendor == "AuthenticAMD") {
        return String("AMD");
    } else if (m_cpu_vendor == "GenuineIntel") {
        return String("Intel");
    } else if (m_cpu_vendor == "CyrixInstead") {
        return String("Cyrix");
    } else if (m_cpu_vendor == "CentaurHauls") {
        return String("Centaur");
    } else if (m_cpu_vendor == "RiseRiseRise") {
        return String("Rise");
    } else if (m_cpu_vendor == "GenuineTMx86") {
        return String("Transmeta");
    } else if (m_cpu_vendor == "UMC UMC UMC ") {
        return String("UMC");
    } else {
        return m_cpu_vendor;
    }
}

void Processor::reportLog()
{
	String tmp;

	tmp << "CPU Information : " << m_cpu_name << "\n"
        << "    |- Vendor: " << getVendorName() << "\n"
        << "    |- Speed: "  << m_cpu_speed << "Mhz\n"
        << "    |- Family: " << m_family << "(" << m_ext_family << ")\n"
        << "    |- Model: " << m_model << "(" << m_ext_model << ")\n"
        << "    |- Stepping: " << m_stepping << "\n"
        << "    |- MMX: " << (m_has_mmx?"YES":"NO") << (m_has_mmx_ext?"(+)\n":"\n")
        << "    |- 3DNOW: " << (m_has_3dnow?"YES":"NO") << (m_has_3dnow_ext?"(+)\n":"\n")
        << "    |- SSE: " << (m_has_sse?"SSE ":"NO") << (m_has_sse2?"SSE2 ":"") <<
                             (m_has_sse3?"SSE3 ":"") << (m_has_ssse3?"SSSE3 ":"") <<
                             (m_has_sse4_1?"SSE4_1 ":"") << (m_has_sse4_2?"SSE4_2 \n":"\n")
        << "    |- HTT: " << (m_is_htt?"YES\n":"NO\n")
        << "    |- NumCPU: " << m_num_cpu << "\n"
        << "    |- CacheLineSize: " << m_cache_line_size << "\n"
        << "    |- SystemRAMSize: " << m_system_ram_size << "\n";

	O3D_MESSAGE(tmp);
}

#define SET_TRICHAR                                             \
	cpu_vendor_id_string[0]  = Char((rebx & 0x000000ff));       \
	cpu_vendor_id_string[1]  = Char((rebx & 0x0000ff00) >> 8);  \
	cpu_vendor_id_string[2]  = Char((rebx & 0x00ff0000) >> 16); \
	cpu_vendor_id_string[3]  = Char((rebx & 0xff000000) >> 24); \
	cpu_vendor_id_string[4]  = Char((redx & 0x000000ff));       \
	cpu_vendor_id_string[5]  = Char((redx & 0x0000ff00) >> 8);  \
	cpu_vendor_id_string[6]  = Char((redx & 0x00ff0000) >> 16); \
	cpu_vendor_id_string[7]  = Char((redx & 0xff000000) >> 24); \
	cpu_vendor_id_string[8]  = Char((recx & 0x000000ff));       \
	cpu_vendor_id_string[9]  = Char((recx & 0x0000ff00) >> 8);  \
	cpu_vendor_id_string[10] = Char((recx & 0x00ff0000) >> 16); \
	cpu_vendor_id_string[11] = Char((recx & 0xff000000) >> 24);

#define SET_QUADCHAR(start)                                      \
	cpu_name_string[start+0]  = Char((reax & 0x000000ff));       \
	cpu_name_string[start+1]  = Char((reax & 0x0000ff00) >> 8);  \
	cpu_name_string[start+2]  = Char((reax & 0x00ff0000) >> 16); \
	cpu_name_string[start+3]  = Char((reax & 0xff000000) >> 24); \
	cpu_name_string[start+4]  = Char((rebx & 0x000000ff));       \
	cpu_name_string[start+5]  = Char((rebx & 0x0000ff00) >> 8);  \
	cpu_name_string[start+6]  = Char((rebx & 0x00ff0000) >> 16); \
	cpu_name_string[start+7]  = Char((rebx & 0xff000000) >> 24); \
	cpu_name_string[start+8]  = Char((recx & 0x000000ff));       \
	cpu_name_string[start+9]  = Char((recx & 0x0000ff00) >> 8);  \
	cpu_name_string[start+10] = Char((recx & 0x00ff0000) >> 16); \
	cpu_name_string[start+11] = Char((recx & 0xff000000) >> 24); \
	cpu_name_string[start+12] = Char((redx & 0x000000ff));       \
	cpu_name_string[start+13] = Char((redx & 0x0000ff00) >> 8);  \
	cpu_name_string[start+14] = Char((redx & 0x00ff0000) >> 16); \
	cpu_name_string[start+15] = Char((redx & 0xff000000) >> 24);

#ifdef _MSC_VER
    #if defined(O3D_IX32) || defined(O3D_IX64)
        void cpuid(UInt32 func, UInt32* a, UInt32* b, UInt32* c, UInt32* d)
        {
            Int32 info[4];
            __cpuid(info, (Int32)func);

            *a = info[0];
            *b = info[1];
            *c = info[2];
            *d = info[3];
        }
    #elif defined(O3D_ARM) || defined(O3D_ARM64)
        void cpuid(UInt32 func, UInt32* a, UInt32* b, UInt32* c, UInt32* d)
        {
            // non Intel CPU
        }
    #else
        void cpuid(UInt32 func, UInt32* a, UInt32* b, UInt32* c, UInt32* d)
        {
            // nont Intel CPU
        }
    #endif
#elif defined(__GNUC__)
  // GCC compiler
  #if defined(O3D_IX64)
    // On x86-64, gcc save %rbx when compiling with -fPIC
    void cpuid(UInt32 func, UInt32* a, UInt32* b, UInt32* c, UInt32* d)
    {
          __asm__  (
        "cpuid              \n"
            :"=a" (*a), "=b" (*b), "=c" (*c), "=d" (*d) : "a" (func));
    }
  #elif defined(O3D_IX32)
    void cpuid(UInt32 func, UInt32* a, UInt32* b, UInt32* c, UInt32* d)
    {
          __asm__  (
        "pushl %%ebx        \n"
        "cpuid              \n"
        "movl %%ebx, %%esi  \n"
        "popl %%ebx        \n"
            :"=a" (*a), "=S" (*b), "=c" (*c), "=d" (*d) : "a" (func));
    }
  #elif defined(O3D_ARM64)
        void cpuid(UInt32 func, UInt32* a, UInt32* b, UInt32* c, UInt32* d)
        {
            // non Intel CPU
        }
  #elif defined(O3D_ARM32)
        void cpuid(UInt32 func, UInt32* a, UInt32* b, UInt32* c, UInt32* d)
        {
            // non Intel CPU
        }
  #endif
#endif

#if defined(O3D_IX32) || defined(O3D_IX64)
void Processor::doCPUID()
{
  	Char cpu_name_string[49] = {0};      // max 48 chars + terminating 0
	Char cpu_vendor_id_string[13] = {0}; // max 12 chars + terminating 0
	UInt32 cpu_feat_eax = 0;
	UInt32 cpu_feat_edx = 0;
	UInt32 cpu_feat_ecx = 0;
	UInt32 cpu_feat_ext_edx = 0;

	UInt32 reax, rebx, recx, redx;

  #if defined(_MSC_VER) && defined(O3D_IX32)
	__asm
	{
		mov     eax, 0x00000000              // first CPUID function, always supported (on reasonable cpu)
		cpuid                                // get info
		mov     [rebx], ebx                  // copy vendor id string
		mov     [recx], ecx
		mov     [redx], edx
		test    eax, eax
		jz      no_features                  // if eax is 0, no info will be available
	}

	SET_TRICHAR

	__asm
	{
		mov     eax, 0x00000001              // get extended info about cpu
		cpuid
		mov     [cpu_feat_eax], eax          // store data for later processing
		mov     [cpu_feat_edx], edx
		mov     [cpu_feat_ecx], ecx

		mov     eax, 0x80000000              // first extended function
		cpuid

		// now test which extended functions are supported
		cmp     eax, 0x80000001              // is eax < 0x80000001
		jb      no_features                  // yes -> jump to no_features label
		cmp     eax, 0x80000004              // is eax < 0x80000004
		jb      ext_feats_only               // yes -> jump to ext_feats_only label

		// now get name of the cpu
		mov     eax, 0x80000002
		cpuid
		mov     [reax], eax
		mov     [rebx], ebx
		mov     [recx], ecx
		mov     [redx], edx
	}
	SET_QUADCHAR(0)

	__asm
	{
		mov     eax, 0x80000003
		cpuid
		mov     [reax], eax
		mov     [rebx], ebx
		mov     [recx], ecx
		mov     [redx], edx
	}
	SET_QUADCHAR(16)

	__asm
	{
		mov     eax, 0x80000004
		cpuid
		mov     [reax], eax
		mov     [rebx], ebx
		mov     [recx], ecx
		mov     [redx], edx
	}
	SET_QUADCHAR(32)

	__asm
	{
		ext_feats_only:
		// get extended features
		mov     eax, 0x80000001
		cpuid
		mov     [cpu_feat_ext_edx], edx

		no_features:
	}
  #elif defined(__GNUC__) || (defined(_MSC_VER) && defined(O3D_IX64))
    // GCC compiler and Windows x64
	cpuid(0x0, &reax, &rebx, &recx, &redx);
    if (reax == 0) {
		goto no_features;
    }

	SET_TRICHAR

	cpuid(0x1, &cpu_feat_eax, &rebx, &cpu_feat_ecx, &cpu_feat_edx);

	cpuid(0x80000000, &reax, &rebx, &recx, &redx);
    if (reax < 0x80000001) {
		goto no_features;
    }
    if (reax < 0x80000004) {
		goto ext_feats_only;
    }

	cpuid(0x80000002, &reax, &rebx, &recx, &redx);
	SET_QUADCHAR(0)

	cpuid(0x80000003, &reax, &rebx, &recx, &redx);
	SET_QUADCHAR(16)

	cpuid(0x80000004, &reax, &rebx, &recx, &redx);
	SET_QUADCHAR(32)

	ext_feats_only:

	cpuid(0x80000001, &reax, &rebx, &recx, &cpu_feat_ext_edx);

	no_features:

/* old code without use of cpuid helper
	#ifndef __x86_64__
		__asm__ __volatile__ ("pushl %eax");
		__asm__ __volatile__ ("pushl %ebx");
		__asm__ __volatile__ ("pushl %ecx");
		__asm__ __volatile__ ("pushl %edx");
	#else
		__asm__ __volatile__ ("pushq %rax");
		__asm__ __volatile__ ("pushq %rbx");
		__asm__ __volatile__ ("pushq %rcx");
		__asm__ __volatile__ ("pushq %rdx");
	#endif
	__asm__ __volatile__ ("mov $0x00000000,%%eax \n\t" \
			"cpuid \n\t" \
			"movl %%ebx,%0 \n\t" \
			"movl %%ecx,%1 \n\t" \
			"movl %%edx,%2 \n\t" : : "m" (rebx), "m" (recx), "m" (redx));
	__asm__ __volatile__ ("test %eax, %eax");
	__asm__ __volatile__ ("jz no_features");

	SET_TRICHAR

	__asm__ __volatile__ ("mov $0x00000001,%eax");
	__asm__ __volatile__ ("cpuid");
	__asm__ __volatile__ ("movl %%eax,%0" : : "m" (cpu_feat_eax));
	__asm__ __volatile__ ("movl %%edx,%0" : : "m" (cpu_feat_edx));
	__asm__ __volatile__ ("movl %%ecx,%0" : : "m" (cpu_feat_ecx));
	__asm__ __volatile__ ("mov $0x80000000,%eax");
	__asm__ __volatile__ ("cpuid");
	__asm__ __volatile__ ("cmp $0x80000001,%eax");
	__asm__ __volatile__ ("jb no_features");
	__asm__ __volatile__ ("cmp $0x80000004,%eax");
	__asm__ __volatile__ ("jb ext_feats_only");

	__asm__ __volatile__ ("mov $0x80000002,%%eax \n\t" \
			"cpuid \n\t" \
			"movl %%eax,%0 \n\t" \
			"movl %%ebx,%1 \n\t" \
			"movl %%ecx,%2 \n\t" \
			"movl %%edx,%3 \n\t" : : "m" (reax), "m" (rebx), "m" (recx), "m" (redx));
	SET_QUADCHAR(0)

	__asm__ __volatile__ ("mov $0x80000003,%%eax \n\t" \
			"cpuid \n\t" \
			"movl %%eax,%0 \n\t" \
			"movl %%ebx,%1 \n\t" \
			"movl %%ecx,%2 \n\t" \
			"movl %%edx,%3 \n\t" : : "m" (reax), "m" (rebx), "m" (recx), "m" (redx));
	SET_QUADCHAR(16)

	__asm__ __volatile__ ("mov $0x80000004,%%eax \n\t" \
			"cpuid \n\t" \
			"movl %%eax,%0 \n\t" \
			"movl %%ebx,%1 \n\t" \
			"movl %%ecx,%2 \n\t" \
			"movl %%edx,%3 \n\t" : : "m" (reax), "m" (rebx), "m" (recx), "m" (redx));
	SET_QUADCHAR(32)

	__asm__ __volatile__ ("ext_feats_only:");
	__asm__ __volatile__ ("mov $0x80000001,%eax");
	__asm__ __volatile__ ("cpuid");
	__asm__ __volatile__ ("movl %%edx,%0" : : "m" (cpu_feat_ext_edx));
	__asm__ __volatile__ ("no_features:");
	#ifndef __x86_64__
		__asm__ __volatile__ ("popl %edx");
		__asm__ __volatile__ ("popl %ecx");
		__asm__ __volatile__ ("popl %ebx");
		__asm__ __volatile__ ("popl %eax");
	#else
		__asm__ __volatile__ ("popq %rdx");
		__asm__ __volatile__ ("popq %rcx");
		__asm__ __volatile__ ("popq %rbx");
		__asm__ __volatile__ ("popq %rax");
	#endif*/
  #endif

	// now process data we got from cpu
	m_cpu_name = String(cpu_name_string);
	m_cpu_vendor = String(cpu_vendor_id_string);

	m_stepping = cpu_feat_eax & 0xF;
	m_model = (cpu_feat_eax >> 4) & 0xF;
	m_family = (cpu_feat_eax >> 8) & 0xF;
	m_type = (cpu_feat_eax >> 12) & 0x3;
	m_ext_model = (cpu_feat_eax >> 16) & 0xF;
	m_ext_family = (cpu_feat_eax >> 20) & 0xFF;

	m_has_mmx = (cpu_feat_edx >> 23) & 0x1;
	m_has_sse = (cpu_feat_edx >> 25) & 0x1;
	m_has_sse2 = (cpu_feat_edx >> 26) & 0x1;
	m_is_htt = (cpu_feat_edx >> 28) & 0x1;

	m_has_sse3 = cpu_feat_ecx & 0x1;
	m_has_ssse3 = (cpu_feat_ecx >> 9) & 0x1;
	m_has_sse4_1 = (cpu_feat_ecx >> 19) & 0x1;
	m_has_sse4_2 = (cpu_feat_ecx >> 20) & 0x1;

	m_has_mmx_ext = (cpu_feat_ext_edx >> 22) & 0x1;
	m_has_3dnow = (cpu_feat_ext_edx >> 31) & 0x1;
	m_has_3dnow_ext = (cpu_feat_ext_edx >> 30) & 0x1;
}
#elif defined(O3D_ARM32) || defined(O3D_ARM64)
void Processor::doCPUID()
{
    // @todo
}
#else
  #error "<< Unknown architecture ! >>"
#endif

#undef SET_TRICHAR
#undef SET_QUADCHAR

#ifdef O3D_WINDOWS
#include "o3d/core/architecture.h"
#elif defined(O3D_POSIX_SYS)
#define HAVE_SYSCONF 1
#elif defined(O3D_MACOSX)
#define HAVE_SYSCTLBYNAME 1
#endif

#ifdef HAVE_SYSCONF
#include <unistd.h>
#endif
#ifdef HAVE_SYSCTLBYNAME
#include <sys/types.h>
#include <sys/sysctl.h>
#endif
#if defined(__MACOSX__) && (defined(__ppc__) || defined(__ppc64__))
#include <sys/sysctl.h>
#elif defined(__OpenBSD__) && defined(__powerpc__)
#include <sys/param.h>
#include <sys/sysctl.h>
#include <machine/cpu.h>
#endif

static Int32 _getNumCPU()
{
    Int32 n = 1;  // one at least...

#if defined(HAVE_SYSCONF) && defined(_SC_NPROCESSORS_ONLN)
    n = (Int32)sysconf(_SC_NPROCESSORS_ONLN);
#endif
#ifdef HAVE_SYSCTLBYNAME
    size_t size = sizeof(n);
    sysctlbyname("hw.ncpu", &n, &size, NULL, 0);
#endif
#ifdef O3D_WINDOWS
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    n = (Int32)info.dwNumberOfProcessors;
#endif

    return n;
}

static Int32 _getCPUCacheLineSize(const String &cpuType)
{
#if defined(O3D_IX32) || defined(O3D_IX64)
    if (cpuType == "GenuineIntel") {
        UInt32 a, b, c, d;

        cpuid(0x00000001, &a, &b, &c, &d);
        return (((b >> 8) & 0xff) * 8);
    } else if (cpuType == "AuthenticAMD") {
        UInt32 a, b, c, d;

        cpuid(0x80000005, &a, &b, &c, &d);
        return (c & 0xff);
    }
#elif defined(O3D_ARM)
    return 0;  // @todo
#endif
    return 0;  // unknown
}

static Int32 _getSystemRAMSize()
{
    Int32 n = 0;

#if defined(HAVE_SYSCONF) && defined(_SC_PHYS_PAGES) && defined(_SC_PAGESIZE)
    n = (Int32)((Int64)sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE) / (1024*1024));
#endif
#ifdef HAVE_SYSCTLBYNAME
#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#ifdef HW_REALMEM
    int mib[2] = {CTL_HW, HW_REALMEM};
#else
    /* might only report up to 2 GiB */
    int mib[2] = {CTL_HW, HW_PHYSMEM};
#endif /* HW_REALMEM */
#else
    int mib[2] = {CTL_HW, HW_MEMSIZE};
#endif /* __FreeBSD__ || __FreeBSD_kernel__ */
    Uint64 memsize = 0;
    size_t len = sizeof(memsize);

    if (sysctl(mib, 2, &memsize, &len, NULL, 0) == 0) {
        n = (Int32)(memsize / (1024*1024));
    }
#endif
#ifdef O3D_WINDOWS
    MEMORYSTATUSEX stat;
    stat.dwLength = sizeof(stat);
    if (GlobalMemoryStatusEx(&stat)) {
        n = (Int32)(stat.ullTotalPhys / (1024 * 1024));
    }
#endif

    return n;
}
