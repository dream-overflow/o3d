/**
 * @file base.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_BASE_H
#define _O3D_BASE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//---------------------------------------------------------------------------------------
// Platform and compilation
//---------------------------------------------------------------------------------------

#ifdef O3D_MACOSX
	#include <mach/mach.h>
	#include <mach/mach_time.h>
#endif

#include "objective3dconfig.h"

// If no object export/import mode defined suppose IMPORT
#if !defined(O3D_EXPORT_DLL) && !defined(O3D_STATIC_LIB)
	#ifndef O3D_IMPORT_DLL
		#define O3D_IMPORT_DLL
	#endif
#endif

// strict in type declaration
#ifndef STRICT
	#define STRICT
#endif

//! Avoid "unused parameter" warnings
#define O3D_UNUSED(x) (void)x;

//---------------------------------------------------------------------------------------
// Debug Assertions
//---------------------------------------------------------------------------------------

#include "assert.h"

//---------------------------------------------------------------------------------------
// API define depend on OS and dynamic library exporting type
//---------------------------------------------------------------------------------------

#if (defined(O3D_UNIX) || defined(O3D_MACOSX) || defined(SWIG))
  #if __GNUC__ >= 4
    #define O3D_API __attribute__ ((visibility ("default")))
    #define O3D_API_PRIVATE __attribute__ ((visibility ("hidden")))
	#define O3D_API_TEMPLATE
  #else
    #define O3D_API
    #define O3D_API_PRIVATE
    #define O3D_API_TEMPLATE
  #endif
#elif defined(O3D_WINDOWS)
	// export DLL
	#ifdef O3D_EXPORT_DLL
		#define O3D_API __declspec(dllexport)
        #define O3D_API_PRIVATE
		#define O3D_API_TEMPLATE
	#endif
	// import DLL
	#ifdef O3D_IMPORT_DLL
		#define O3D_API __declspec(dllimport)
        #define O3D_API_PRIVATE
		#define O3D_API_TEMPLATE
	#endif
	// static (no DLL)
	#ifdef O3D_STATIC_LIB
		#define O3D_API
        #define O3D_API_PRIVATE
		#define O3D_API_TEMPLATE
	#endif
#endif

//---------------------------------------------------------------------------------------
// Memory alignment
//---------------------------------------------------------------------------------------

//! Check aligned
#define O3D_CHECK_ALIGN(PTR,ALIGN) (((PTR) & (ALIGN-1)) == 0)

// bytes alignment
#if defined(__GNUC__)
	#define O3D_ALIGN(X) __attribute__ ((aligned(X)))
#elif defined(__INTEL_COMPILER)
    #define O3D_ALIGN(X) _alloca(X)
#elif defined(_MSC_VER)
    #define O3D_ALIGN(X) __declspec(align(X))
#endif

//---------------------------------------------------------------------------------------
// Base typedef definitions and enumerations
//---------------------------------------------------------------------------------------

#include "types.h"
#include "coretype.h"

//---------------------------------------------------------------------------------------
// Constants and templates
//---------------------------------------------------------------------------------------

//! Global o3d namespace
namespace o3d
{
    static const Float PI = 3.1415926535897932384626433832795f;        //!< Pi
    static const Float HALF_PI = 1.5707963267948966192313216916398f;   //!< Pi/2
    static const Float TWO_PI = 6.283185307179586476925286766559f;     //!< 2*Pi
    static const Float IVI_PI = 0.31830988618379067153776752674503f;   //!< 1/Pi

    static const Float EXP = 2.7182818284590452353602874713527f;       //!< e
    static const Float INV_LOG2 = 3.3219280948873623478703194294894f;  //!< 1/log10(2)

    static const Float INV3 = 0.33333333333333333333333333333333f;     //!< 1/3
    static const Float INV6 = 0.16666666666666666666666666666667f;     //!< 1/6
    static const Float INV7 = 0.14285714285714285714285714285714f;     //!< 1/7
    static const Float INV9 = 0.11111111111111111111111111111111f;     //!< 1/9
    static const Float INV255 = 0.003921568627450980392156862745098f;  //!< 1/255

    static const Float SQRT2 = 1.4142135623730950488016887242097f;     //!< sqrt(2)

	//! Empty method.
	inline void emptyFoo() {}

	//! Return square.
	template <class T>
	inline T sqr(T a) { return (a*a); }

	//! Return cubic.
	template <class T>
	inline T cube(T a) { return (a*a*a); }

	//! Return |x|.
	template <class T>
	inline T abs(T a) { return (a>0 ? a : -a); }

	//! Return min value of a,b.
	template <class T>
	inline T min(T a, T b) { return (a<b ? a : b); }

	//! Return max value of a,b.
	template <class T>
	inline T max(T a, T b) { return (a>b ? a : b); }

	//! Return min value of a,b,c.
	template <class T>
	inline T minMin(T a, T b, T c) { return (a<b ? min(a,c) : min(b,c)); }

	//! Return max value of a,b,c.
	template <class T>
	inline T maxMax(T a, T b, T c) { return (a>b ? max(a,c) : max(b,c)); }

	//! Check x for range [min,max] and return a corrected value in [min,max] if necessary.
	template <class T>
	inline T clamp(T x, T min, T max) { return (x<min ? min : (x>max ? max : x)); }

	//! Check if x lies in the segment [min,max]
	template <class T>
    inline Bool liesIn(T x, T min, T max) { return !((x < min) || (x > max)); }

	//! Convert Radian to Degree (Float version).
    inline Float toDegree(Float radian) { return radian * 180.f / PI; }
	//! Convert Radian to Degree (Double version).
    inline Double toDegree(Double radian) { return radian * 180.0 / Double(PI); }

	//! Convert Degree to Radian (Float version).
    inline Float toRadian(Float degree) { return degree * PI / 180.f; }
	//! Convert Degree to Radian (Double version).
    inline Double toRadian(Double degree) { return degree * Double(PI) / 180.0; }

    //! Simplify in the range [-PI..+PI] an angle in radian.
    template <class T>
    inline T simplifyRadian(T radian) {
        if (abs(radian) > TWO_PI) {
            radian = radian - ((Int32)(radian / TWO_PI) * TWO_PI);
        }

        if (radian > o3d::PI) {
            return radian - TWO_PI;
        } else if (radian < -o3d::PI) {
            return radian + TWO_PI;
        } else {
            return radian;
        }

        return radian - (((int)(radian/PI + 0.4999999) / 2) * 2) * PI;
    }

	//! Return Float random value.
    inline Float randomFloat()
	{
        return ((Float(rand()) - Float(rand())) / Float(RAND_MAX));
	}

	//! Return T random value of range [min,max].
	template <class T>
	inline T randomRange(T min, T max)
	{
        return (min + (T)(Float(max-min)*Float(rand())/Float(RAND_MAX)));
	}

	//! Return T random value of range [min,max], faster than above macro, but less good randomize.
	template <class T>
	inline T randomRangeAlt(T min, T max)
	{
		return (rand() % (T)((max+1)-min))+min;
	}

	//! Return T random value of range [min,max].
	template <class T>
	inline T randomRangeAlt2(T min, T max)
	{
        return (min + (T)(Float(max-min)*Float(rand()%10000))/10000.f);
	}

    //! Delete a pointer if existing and set it to null.
	template <class T>
	inline void deletePtr(T *&ptr)
	{
        if (ptr) {
			delete ptr;
            ptr = nullptr;
		}
	}

    //! Delete a pointer if existing but doesn't set it to null.
	template<class T>
	inline void deletePtr(const T *ptr)
	{
        if (ptr) {
			delete ptr;
        }
	}

    //! Delete a pointer array if existing and set it to null.
	template <class T>
	inline void deleteArray(T *&ptr)
	{
        if (ptr) {
			delete [] ptr;
            ptr = nullptr;
		}
	}

    //! Delete a pointer array if existing and doesn't set it to null.
	template <class T>
	inline void deleteArray(const T *ptr)
	{
        if (ptr) {
			delete [] ptr;
        }
	}

	//! Returns the floor form of binary logarithm for a 32 bit integer.
	//! @note -1 is returned if n is 0.
    Int32 log2(UInt32 n);

	//! returns the next power of two : NextPow2(3) returns 4
    inline UInt32 nextPow2(UInt32 n)
	{
		n--;

		n |= n >> 1;
		n |= n >> 2;
		n |= n >> 4;
		n |= n >> 8;
		n |= n >> 16;

		return ++n;
	}

    inline Bool isPow2(UInt32 n) { return n == nextPow2(n); }

    //! Convert Int32 to Bool.
    inline Bool toBool(Int32 o) { return (o > 0) ? True : False; }
}

//---------------------------------------------------------------------------------------
// Defines constants and macros
//---------------------------------------------------------------------------------------

#define O3D_INFINITE 0xFFFFFFFF                     //!< infinite constant

//! check if pointer is allocated
#define O3D_CHECKPTR(p) if (!(p)) O3D_ERROR(E_NullPointer(#p))

//---------------------------------------------------------------------------------------
// Class declaration helpers
//---------------------------------------------------------------------------------------

//! Class declaration helpers for non copyable object
#define O3D_NONCOPYABLE(classname)              \
    private:                                    \
        classname(const classname&);            \
        classname& operator=(const classname&);

//! Class declaration helpers for non assignable object
#define O3D_NONASSIGNABLE(classname)            \
    private:                                    \
        classname& operator=(const classname&);

namespace o3d {

class NullClass {};

//! Non copyable class must inherit from it.
template <class _T = NullClass>
class NonCopyable : _T
{
protected:

	//! Permit to instantiate from children only
	NonCopyable() /*throw()*/ {}
	~NonCopyable() /*throw()*/ {}

private:

	//! Cannot duplicate by copy constructor
	NonCopyable(const NonCopyable &);
	//! Cannot duplicate by assign operator
	NonCopyable& operator= (const NonCopyable &);
};

//! Non assignable class must inherit from it.
template <class _T = NullClass>
class NonAssignable : _T
{
protected:

	//! Permit to instantiate from children only
	NonAssignable() /*throw()*/ {}
	~NonAssignable() /*throw()*/ {}

private:

	//! Cannot duplicate by assign operator
	NonAssignable& operator= (const NonAssignable &);
};

//! Return a static instance according to a specified object.
//! @note The object must have a default constructor.
template <class T>
const T& instance()
{
	static const T instance = T();
	return instance;
}

class String;

/**
 * @brief Base system methods
 * @date 2001-12-09
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class O3D_API System
{
	friend class Application;

public:

	//-----------------------------------------------------------------------------------
	// Endianess
	//-----------------------------------------------------------------------------------

	//! Endianess
	enum ByteOrder
	{
		ORDER_LITTLE_ENDIAN = 0,
		ORDER_BIG_ENDIAN = 1
	};

	//! Get the system native byte-order.
	static ByteOrder getNativeByteOrder();

    //! Invert the byte-order.
    static inline void swapBytes2(void* value);

    //! Invert the byte-order.
    static inline void swapBytes4(void* value);

    //! Invert the byte-order.
    static inline void swapBytes8(void* value);

	//-----------------------------------------------------------------------------------
	// Global message output
	//-----------------------------------------------------------------------------------

	//! Icon style for print message(box).
    enum MessageLevel
	{
        MSG_DEBUG = 0,
        MSG_INFO,
		MSG_WARNING,
        MSG_ERROR,
        MSG_CRITICAL
	};

    //! Print a message on the standard output.
	//! @param content Content of the message.
	//! @param title Title of the message.
    //! @param level Level of the message. Default is MSG_INFO.
	static void print(
			const String &content,
			const String &title,
            MessageLevel level=MSG_INFO);

	//-----------------------------------------------------------------------------------
	// Global Time methods
	//-----------------------------------------------------------------------------------

    //! get time with precision defined by getTimeFrequency().
    static Int64 getTime();

    //! get time precision in ms for getTime().
    static Int64 getTimeFrequency();

    //! get time with a specified precision.
    static Int64 getTime(TimeUnit unit);

	//! get time width a precision of 1ms.
    static Int32 getMsTime();

    //! wait a delay (in ms)
    static void waitMs(Int32 ms);

    //! wait a delay (in us)
    //static void waitUs(Int64 us);

    //! wait a delay (in ns)
    //static void waitNs(Int64 ns);

    //! wait a delay (in time unit)
    //static void wait(Int64 ms, TimeUnit);

    //-----------------------------------------------------------------------------------
    // Plateform details
    //-----------------------------------------------------------------------------------

    enum Plateform
    {
        PLATEFORM_ANDROID,
        PLATEFORM_LINUX,
        PLATEFORM_WINDOWS,
        PLATEFORM_MACOSX,
        PLATEFORM_PS4,
        PLATEFORM_SWITCH,
        PLATEFORM_XBOXONE
    };

    //! Get the plateform type.
    static Plateform getPlatform();

    enum Profile
    {
        PROFILE_DESKTOP,
        PROFILE_MOBILE,
        PROFILE_MOBILE_WIDE,
        PROFILE_CONSOLE,
        PROFILE_CONSOLE_HIGH
    };

    //! Get the plateform profile.
    static Profile getProfile();

	//-----------------------------------------------------------------------------------
	// CPU methods and informations
	//-----------------------------------------------------------------------------------

	//! return approximate processor frequency in MHz
    static Float getProcessorFrequency();

private:

	//! Performing some compile time checking
	static void checking();

	//! Initialize time.
	static void initTime();
};

//---------------------------------------------------------------------------------------
// inv multi-bytes type (int,short,float,enum...)
//---------------------------------------------------------------------------------------

inline void System::swapBytes2(void* value)
{
#if defined(_MSC_VER) && (defined(O3D_IX32) || defined(O3D_IX64)) // VC++ x86
    __asm {
        mov ebx, value
        mov al, [ebx+1]
        mov ah, [ebx  ]
        mov [ebx], ax
    }
#else
#ifdef O3D_BIG_ENDIAN
    /*register*/ UInt8 t0,t1;

    t0 = ((UInt8*)value)[0];
    t1 = ((UInt8*)value)[1];

    ((UInt8*)value)[0] = t1;
    ((UInt8*)value)[1] = t0;
#else
    UInt16 temp = *(UInt16*)value;
    *(UInt16*)value = (temp >> 8) | (temp << 8);
#endif
#endif
}

inline void System::swapBytes4(void* value)
{
#if defined(_MSC_VER) && (defined(O3D_IX32) || defined(O3D_IX64)) // VC++ x86
    __asm {
        mov ebx, value
        mov eax, [ebx]
        bswap eax
        mov [ebx], eax
    }
#else
#ifdef O3D_BIG_ENDIAN
    /*register*/ UInt8 t0,t1,t2,t3;

    t0 = ((UInt8*)value)[0];
    t1 = ((UInt8*)value)[1];
    t2 = ((UInt8*)value)[2];
    t3 = ((UInt8*)value)[3];

    ((UInt8*)value)[0] = t3;
    ((UInt8*)value)[1] = t2;
    ((UInt8*)value)[2] = t1;
    ((UInt8*)value)[3] = t0;
#else
    UInt32 temp = *(UInt32*)value;
    *(UInt32*)value = (temp >> 24) | ((temp >> 8) & 0xff00) | ((temp << 8) & 0xff0000) | (temp << 24);
#endif
#endif
}

inline void System::swapBytes8(void* value)
{
    // swap 8 bytes type
    /*register*/ UInt8 t0,t1,t2,t3,t4,t5,t6,t7;

    t0 = ((UInt8*)value)[0];
    t1 = ((UInt8*)value)[1];
    t2 = ((UInt8*)value)[2];
    t3 = ((UInt8*)value)[3];
    t4 = ((UInt8*)value)[4];
    t5 = ((UInt8*)value)[5];
    t6 = ((UInt8*)value)[6];
    t7 = ((UInt8*)value)[7];

    ((UInt8*)value)[0] = t7;
    ((UInt8*)value)[1] = t6;
    ((UInt8*)value)[2] = t5;
    ((UInt8*)value)[3] = t4;
    ((UInt8*)value)[4] = t3;
    ((UInt8*)value)[5] = t2;
    ((UInt8*)value)[6] = t1;
    ((UInt8*)value)[7] = t0;
}

} // namespace o3d

#endif // _O3D_BASE_H
