/**
 * @file math.h
 * @brief Useful mathematic functions.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-02-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MATH_H
#define _O3D_MATH_H

#include "o3d/core/base.h"
#include "o3d/core/memorydbg.h"

#if defined(O3D_USE_SIMD) && defined(O3D_VC_COMPILER) && defined(O3D_WINDOWS)
#elif defined(O3D_USE_SIMD) && (defined(O3D_IX32) || defined(O3D_IX64))
#else
#include <math.h>
#endif

namespace o3d {

/**
 * @brief Common mathematics functions
 * @todo Add True randomizer
 */
class O3D_API Math
{
public:

	//! Initialize mathematics functions and defaults objects.
	static void init();
	//! Release the initialize.
	static void quit();

	//! fast Sqrt (SSE or 3DNOW or STD)
    // static Float (*sqrt)(Float);

    inline static Float sqrt(Float x)
    {
    #if defined(O3D_USE_SIMD) && defined(O3D_VC_COMPILER) && defined(O3D_WINDOWS)
        static Float half = 0.5f;
        static Float three = 3.0f;
        Float y;  // = 0.f; not set to 0 otherwise optimization will take 0 as result of the function

        __asm
        {
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

        return y;
    #elif defined(O3D_USE_SIMD) && (defined(O3D_IX32) || defined(O3D_IX64))
        static Float half = 0.5f;
        static Float three = 3.0f;
        Float y;   // = 0.f; not set to 0 otherwise optimization will take 0 as result of the function

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

        // __asm__ __volatile__ ("sqrtss %xmm3,%xmm0 \n\t");
        // __asm__ __volatile__ ("movss %%xmm0,%0" : : "m" (y));

        return y;
    #else
         return ::sqrtf(x);
    #endif
    }

    //! Fast reciproc sqrt
    inline static Float rsqrt(Float x)
	{
		/*long i;
		Float x2, y;
		const Float threehalfs = 1.5f;

		x2 = x * 0.5f;
		y  = x;
		i  = * (long*)&y;
		i  = 0x5f3759df - (i >> 1);
		y  = * (Float*)&i;
		y  = y * (threehalfs - (x2 * y * y));*/
		union fi
		{
			Int32 i;
			Float f;
		};

		fi i, y;
		Float x2;
		const Float threehalfs = 1.5f;

		x2 = x * 0.5f;
		y.f  = x;
		i.i  = 0x5f3759df - (y.i >> 1);
		y.f  = i.f * (threehalfs - (x2 * i.f * i.f));

		return y.f;
	}

    //! standard C/C++
    struct _Std
    {
		//! standard C/C++ Sqrt
        static Float sqrt(Float);
    };

    //! SSE routines
    struct _SSE
    {
		//! SSE Sqrt
        static Float sqrt(Float);
    };

    //! 3DNow routines
    struct _3DNow
    {
		//! 3DNow Sqrt
        static Float sqrt(Float);
    };
};

} // namespace o3d

#endif // _O3D_MATH_H
