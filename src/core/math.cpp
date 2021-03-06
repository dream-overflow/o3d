/**
 * @file math.cpp
 * @brief Implementation of Math.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-02-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/math.h"

#include "o3d/core/matrix4.h"
#include "o3d/core/processor.h"

#include <math.h>

// #define ACCURATE_SQRT 1

using namespace o3d;

// @see http://gruntthepeon.free.fr/ssemath/ if we need more optimized maths func

// Set the pointer to default in case Initialize is not called
Float (*Math::sqrt)(Float) = Math::_Std::sqrt;

void Math::init()
{
	Processor processor;

    if (processor.hasSSE()) {
        sqrt = _SSE::sqrt;
    } else if (processor.has3DNow()) {
        sqrt = _3DNow::sqrt;
    } else {
        sqrt = _Std::sqrt;
    }

#if defined(_MSC_VER) && defined(O3D_WINDOWS)
	// enable math SSE2 optimization on VC++ 32bit
    if (processor.hasSSE2()) {
		_set_SSE2_enable(1);
    }
#endif

	Matrix3::init();
	Matrix4::init();
}

// Release the initialize.
void Math::quit()
{
	Matrix3::quit();
	Matrix4::quit();
}

Float Math::_Std::sqrt(Float x)
{
#if defined(O3D_IX32) || defined(O3D_IX64)
  #ifdef _MSC_VER
    __asm
    {
        fld     x
        fsqrt
    }
  #elif defined(__GNUC__)
    /*register*/ Float ret;
    asm("fsqrt" : "=t"(ret): "0"(x));
    return ret;
  #else
    return ::sqrtf(x);
  #endif
#else
    return ::sqrtf(x);
#endif
}

// use of the optimized version only on NDEBUG because else performances
// are lesser than standard sqrtf.
Float Math::_SSE::sqrt(Float x)
{
#ifdef O3D_SSE2
  #if defined(_MSC_VER) && defined(NDEBUG)
   #ifndef ACCURATE_SQRT
    static Float half = 0.5f;
    static Float three = 3.0f;

    // rsqrtss does not deal if x is zero
    if (x <= 0.f) {
        return 0.f;
    }

    // never set to 0 otherwise optimization will take 0 as result of the function
    Float y;

    __asm {
        movss     xmm3, x     // xmm3 = (x, ?, ?, ?)
        movss     xmm4, xmm3   // xmm4 = (x, ?, ?, ?)
        movss     xmm1, half   // xmm1 = (0.5, ?, ?, ?)
        movss     xmm2, three  // xmm2 = (3, ?, ?, ?)
        rsqrtss   xmm0, xmm3   // xmm0 = (~ 1 / sqrt(x), ?, ?, ?)
        mulss     xmm3, xmm0   // xmm3 = (~ sqrt(x), ?, ?, ?)
        mulss     xmm1, xmm0   // xmm1 = (~ 0.5 / sqrt(x), ?, ?, ?)
        mulss     xmm3, xmm0   // xmm3 = (~ 1, ?, ?, ?)
        subss     xmm2, xmm3   // xmm2 = (~ 2, ?, ?, ?)
        mulss     xmm1, xmm2   // xmm1 = (~ 1 / sqrt(x), ?, ?, ?)
        mulss     xmm1, xmm4   // xmm1 = (sqrt(x), ?, ?, ?)
        movss     y, xmm1      // store result
    }
   #else
    __asm {
        movss xmm3, x
        sqrtss xmm3, xmm0
        movss xmm0, y
    }
   #endif
    return y;
  #elif defined(__GNUC__) && defined(NDEBUG)
   #ifndef ACCURATE_SQRT
    static Float half = 0.5f;
    static Float three = 3.0f;

    // rsqrtss does not deal if x is zero
    if (x <= 0.f) {
        return 0.f;
    }

    // never set to 0 otherwise optimization will take 0 as result of the function
    Float y;

    __asm__ __volatile__ ("movss %0,%%xmm3 \n\t" : : "m" (x));
    __asm__ __volatile__ ("movss %xmm3,%xmm4 \n\t");
    __asm__ __volatile__ ("movss %0,%%xmm1 \n\t" : : "m" (half));
    __asm__ __volatile__ ("movss %0,%%xmm2 \n\t" : : "m" (three));
    __asm__ __volatile__ ("rsqrtss %xmm3,%xmm0 \n\t");
    __asm__ __volatile__ ("mulss %xmm0,%xmm3 \n\t");
    __asm__ __volatile__ ("mulss %xmm0,%xmm1 \n\t");
    __asm__ __volatile__ ("mulss %xmm0,%xmm3 \n\t");
    __asm__ __volatile__ ("subss %xmm3,%xmm2 \n\t");
    __asm__ __volatile__ ("mulss %xmm2,%xmm1 \n\t");
    __asm__ __volatile__ ("mulss %xmm4,%xmm1 \n\t");
    __asm__ __volatile__ ("movss %%xmm1,%0" : : "m" (y));
   #else
    __asm__ __volatile__ ("movss %0,%%xmm3 \n\t" : : "m" (x));
    __asm__ __volatile__ ("sqrtss %xmm3,%xmm0 \n\t");
    __asm__ __volatile__ ("movss %%xmm0,%0" : : "m" (y));
   #endif
    return y;
  #else
    return ::sqrtf(x);
  #endif
#else
    return ::sqrtf(x);
#endif
}

Float Math::_3DNow::sqrt(Float x)
{
#if defined(O3D_IX32) || defined(O3D_IX64)
  #if defined(_MSC_VER)
	Float y = 0.f;

	__asm
	{
		movd      mm0, x     // mm0 = (x, ?)
		pfrsqrt   mm1, mm0   // mm1 = (~1 / sqrt(x), ?)
		movq      mm2, mm1   // mm2 = mm1
		pfmul     mm1, mm1   // mm1 = mm1 * mm1
		pfrsqit1  mm1, mm0   // first iteration of Newton-Rhapson
		pfrcpit2  mm1, mm2   // second iteration
		punpckldq mm1, mm1   // mm1 = (1 / sqrt(x), ?)
		pfmul     mm0, mm1   // mm0 = (sqrt(x), ?)
		movd      y, mm0     // store result
		femms                // clear MMX state
	}

	return y;
  #elif defined(__GNUC__)
	Float y = 0.f;

	__asm__ __volatile__ ("movd %0,%%mm0" : : "m" (x));
	__asm__ __volatile__ ("pfrsqrt %mm0,%mm1");
	__asm__ __volatile__ ("movq %mm1,%mm2");
	__asm__ __volatile__ ("pfmul %mm1,%mm1");
	__asm__ __volatile__ ("pfrsqit1 %mm0,%mm1");
	__asm__ __volatile__ ("pfrcpit2 %mm2,%mm1");
	__asm__ __volatile__ ("punpckldq %mm1,%mm1");
	__asm__ __volatile__ ("pfmul %mm1,%mm0");
	__asm__ __volatile__ ("movd %%mm0,%0" : : "m" (y));
	__asm__ __volatile__ ("femms");

	return y;
  #else
    return ::sqrt(x);
  #endif
#else
	return ::sqrt(x);
#endif
}
