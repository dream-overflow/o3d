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

#if defined(O3D_USE_SIMD) && defined(_MSC_VER) && defined(O3D_WINDOWS)
#elif defined(O3D_USE_SIMD) && defined(__GNUC__) && (defined(O3D_IX32) || defined(O3D_IX64))
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
    static Float (*sqrt)(Float);

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
