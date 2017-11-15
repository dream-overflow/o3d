/**
 * @file types.h
 * @brief Primitives types, somes basic types, and primitive type ranges.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-11-21
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TYPES_H
#define _O3D_TYPES_H

#include "objective3dconfig.h"

namespace o3d {

//---------------------------------------------------------------------------------------
// Base typedef definitions
//---------------------------------------------------------------------------------------

#if (defined(__UNIX__) || defined(__APPLE__))
    typedef char                    Char;   //!< an (unsigned) bits char
    typedef char                    Int8;   //!< A signed 8 bits char
    typedef unsigned char           UInt8;  //!< An unsigned 8 bits char
    typedef short                   Int16;  //!< A signed 16 bits short
    typedef unsigned short          UInt16; //!< An unsigned 16 bits short
    typedef int                     Int32;  //!< A signed 32 bits int
    typedef unsigned int            UInt32; //!< An unsigned 32 bits int
    typedef long long int           Int64;  //!< A signed 64 bits long
    typedef unsigned long long int  UInt64; //!< An unsigned 64 bits long
    typedef float                   Float;  //!< A 32 bits float
    typedef double                  Double; //!< A 64 bits double
    typedef bool                    Bool;   //!< A 8 bits boolean
    typedef wchar_t                 WChar;  //!< A 32 bits char

    const Bool True = true;
    const Bool False = false;
#elif defined (O3D_WINDOWS)
    typedef char                    Char;   //!< an (unsigned) bits char
    typedef char                    Int8;   //!< A signed 8 bits integer
    typedef unsigned char           UInt8;  //!< An unsigned 8 bits integer
    typedef short                   Int16;  //!< A signed 16 bits short
    typedef unsigned short          UInt16; //!< An unsigned 16 bits short
    typedef int                     Int32;  //!< A signed 32 bits int
    typedef unsigned int            UInt32; //!< An unsigned 32 bits int
    typedef long long int           Int64;  //!< A signed 64 bits long
    typedef unsigned long long int  UInt64; //!< An unsigned 64 bits long
    typedef float                   Float;  //!< A 32 bits float
    typedef double                  Double; //!< A 64 bits double
    typedef bool                    Bool;   //!< A 8 bits boolean
    typedef wchar_t                 WChar;  //!< A 16 bits char

    const Bool True = true;
    const Bool False = false;
#else
	#error "<< Unknown architecture ! >>"
#endif // __UNIX__


//---------------------------------------------------------------------------------------
// Some WINDOWS specifics types
//---------------------------------------------------------------------------------------

#ifdef O3D_WINDOWS

	typedef void* _DISP;        //!< Display handle (NULL in WINDOWS)
	typedef char* _SCREEN;      //!< Screen handle (valid char*)
    typedef Int32 _PF;          //!< Pixel format of a HDC.

	//!Handle window Device definition (generally used for WINDOWS API)
	//! can be converted in HNWD for WINDOWS function.
	struct _HWND__
	{
        Int32 unused;
	};
	typedef _HWND__* _HWND; //!< Window handle.

	//! handle Device Context definition (generally used for WINDOWS API)
	//! can be converted in HDC for WINDOWS function.
	struct _HDC__
	{
        Int32 unused;
	};
	typedef _HDC__* _HDC; //!< Drawable handle.

	//! openGL Handle Device Context definition (generally used for WINDOWS API)
	//! can be converted in HGLRC for WINDOWS function.
	struct _HGLRC__
	{
        Int32 unused;
	};
	typedef _HGLRC__* _HGLRC; //!< OpenGL renderer context handle.

	//! Handle instance definition (generally used for WINDOWS API)
	//! can be converted in HINSTANCE for WINDOWS function.
	struct _HINSTANCE__
	{
        Int32 unused;
	};
	typedef _HINSTANCE__* _HINSTANCE; //!< Instance handle.

    const _DISP NULL_DISP = nullptr;
    const _SCREEN NULL_SCREEN = nullptr;
    const _HWND NULL_HWND = nullptr;
    const _HDC  NULL_HDC = nullptr;
	const _PF NULL_PF = 0;
    const _HGLRC NULL_HGLRC = nullptr;
    const _HINSTANCE NULL_HINSTANCE = nullptr;

#elif defined(O3D_SDL)

	typedef void* _DISP;        //!< Display handle (NULL in SDL).
    typedef Int32 _SCREEN;      //!< Screen handle.
    typedef UInt32 _HWND;       //!< Window handle.
	typedef void* _HDC;         //!< Drawable handle (SDL_Window).
    typedef UInt32 _PF;         //!< Pixel format of a drawable.
	typedef void* _HGLRC;       //!< OpenGL renderer context handle.
	typedef void* _HINSTANCE;   //!< Module instance handle.

    const _DISP NULL_DISP = nullptr;
	const _SCREEN NULL_SCREEN = 0;
	const _HWND NULL_HWND = 0;
    const _HDC  NULL_HDC = nullptr;
	const _PF  NULL_PF = 0;
    const _HGLRC NULL_HGLRC = nullptr;
    const _HINSTANCE NULL_HINSTANCE = nullptr;

#elif defined(O3D_X11)

    typedef UInt64 _DISP;     //!< Display handle.
    typedef UInt64 _SCREEN;   //!< Screen handle.
    typedef UInt64 _HWND;     //!< Window handle.
    typedef UInt64 _HDC;      //!< Drawable handle.
    typedef UInt64 _PF;       //!< Pixel format of a drawable (GlxFbContext).
    typedef void* _HGLRC;     //!< OpenGL renderer context handle.
    typedef void* _HINSTANCE; //!< Module instance handle.

	const _DISP NULL_DISP = 0;
	const _SCREEN NULL_SCREEN = 0;
	const _HWND NULL_HWND = 0;
	const _HDC  NULL_HDC = 0;
	const _PF  NULL_PF = 0;
    const _HGLRC NULL_HGLRC = nullptr;
    const _HINSTANCE NULL_HINSTANCE = nullptr;

#else
	#error "<< Unknown architecture ! >>"
#endif // O3D_WIN32


//---------------------------------------------------------------------------------------
// Types ranges
//---------------------------------------------------------------------------------------

#define O3D_MIN_CHAR        0x80        //!< min Char (-128)
#define O3D_MAX_CHAR        0x7f        //!< max Char (+127)

#define O3D_MIN_INT8        0x80        //!< min Int8 (-128)
#define O3D_MAX_INT8        0x7f        //!< max Int8 (+127)
#define O3D_MAX_UINT8       0xff        //!< max UInt8 (255)

#define O3D_MIN_INT16       0x8000      //!< min Int16 (-32768)
#define O3D_MAX_INT16       0x7fff      //!< max Int16 (+32767)
#define O3D_MAX_UINT16      0xffff      //!< max UInt16 (65535)

#define O3D_MIN_INT32       0x80000000  //!< min Int32 (-2147483648)
#define O3D_MAX_INT32       0x7fffffff  //!< max Int32 (+2147483647)
#define O3D_MAX_UINT32      0xffffffff  //!< max UInt32 (+4294967295)

#define O3D_MIN_INT64       0x8000000000000000LL   //!< min Int64
#define O3D_MAX_INT64       0x7fffffffffffffffLL   //!< max Int64
#define O3D_MAX_UINT64      0xffffffffffffffffULL  //!< max UInt64

#define O3D_MIN_FLOAT       (-3.402823466e+38F)  //!< (-FLT_MAX) min Float
#define O3D_MAX_FLOAT       3.402823466e+38F     //!< FLT_MAX max Float

#define O3D_MIN_DOUBLE       (-1.7976931348623158e+308)  //!< (-DBL_MAX) min Double
#define O3D_MAX_DOUBLE       1.7976931348623158e+308     //!< DBL_MAX max Double

#define O3D_FLOAT_1_0       0x3f800000  //!< hex float representation of 1.0
#define O3D_FLOAT_255_0     0x437f0000  //!< hex float representation of 255.0

//! Numerics limits
template <class _T>
struct Limits
{
    typedef _T _Ty;

    //! Minimal value (greater negative value for signed and float, 0 for unsigned).
    static inline _Ty min() { return _Ty(0); }
    //! Maximal value.
    static inline _Ty max() { return _Ty(0); }
    //! Epsilon for comparison.
    static inline _Ty epsilon() { return _Ty(0); }
};

template <> struct Limits <Bool>
{
    typedef Bool _Ty;

    static inline _Ty min() { return _Ty(False); }
    static inline _Ty max() { return _Ty(True); }
    static inline _Ty epsilon() { return _Ty(False); }
};

template <> struct Limits <Char>
{
    typedef Char _Ty;

    static inline _Ty min() { return _Ty(-O3D_MAX_CHAR-1); }
    static inline _Ty max() { return _Ty(O3D_MAX_CHAR); }
    static inline _Ty epsilon() { return _Ty(0); }
};
/*
    template <> struct Limits <Int8>
    {
        typedef Int8 _Ty;

        static inline _Ty min() { return _Ty(-O3D_MAX_INT8-1); }
        static inline _Ty max() { return _Ty(O3D_MAX_INT8); }
        static inline _Ty epsilon() { return _Ty(0); }
    };
*/
template <> struct Limits <UInt8>
{
    typedef UInt8 _Ty;

    static inline _Ty min() { return _Ty(0); }
    static inline _Ty max() { return _Ty(O3D_MAX_UINT8); }
    static inline _Ty epsilon() { return _Ty(0); }
};

template <> struct Limits <Int16>
{
    typedef Int16 _Ty;

    static inline _Ty min() { return _Ty(-O3D_MAX_INT16-1); }
    static inline _Ty max() { return _Ty(O3D_MAX_INT16); }
    static inline _Ty epsilon() { return _Ty(0); }
};

template <> struct Limits <UInt16>
{
    typedef UInt16 _Ty;

    static inline _Ty min() { return _Ty(0); }
    static inline _Ty max() { return _Ty(O3D_MAX_UINT16); }
    static inline _Ty epsilon() { return _Ty(0); }
};

template <> struct Limits <Int32>
{
    typedef Int32 _Ty;

    static inline _Ty min() { return _Ty(-O3D_MAX_INT32-1); }
    static inline _Ty max() { return _Ty(O3D_MAX_INT32); }
    static inline _Ty epsilon() { return _Ty(0); }
};

template <> struct Limits <UInt32>
{
    typedef UInt32 _Ty;

    static inline _Ty min() { return _Ty(0); }
    static inline _Ty max() { return _Ty(O3D_MAX_UINT32); }
    static inline _Ty epsilon() { return _Ty(0); }
};

template <> struct Limits <Int64>
{
    typedef Int64 _Ty;

    static inline _Ty min() { return _Ty(-O3D_MAX_INT64-1); }
    static inline _Ty max() { return _Ty(O3D_MAX_INT64); }
    static inline _Ty epsilon() { return _Ty(0); }
};

template <> struct Limits <UInt64>
{
    typedef UInt64 _Ty;

    static inline _Ty min() { return _Ty(0); }
    static inline _Ty max() { return _Ty(O3D_MAX_UINT64); }
    static inline _Ty epsilon() { return _Ty(0); }
};

template <> struct Limits <Float>
{
    typedef Float _Ty;

    static inline _Ty min() { return _Ty(-O3D_MAX_FLOAT); }
    static inline _Ty max() { return _Ty(O3D_MAX_FLOAT); }
    static inline _Ty epsilon() { return _Ty(0.001f); }
};

template <> struct Limits <Double>
{
    typedef Double _Ty;

    static inline _Ty min() { return _Ty(-O3D_MAX_DOUBLE); }
    static inline _Ty max() { return _Ty(O3D_MAX_DOUBLE); }
    static inline _Ty epsilon() { return _Ty(0.000001f); }
};

#ifdef O3D_WINDOWS
template <> struct Limits <WChar>
{
    typedef WChar _Ty;

    static inline _Ty min() { return _Ty(0); }
    static inline _Ty max() { return _Ty(O3D_MAX_UINT16); }
    static inline _Ty epsilon() { return _Ty(0); }
};
#endif

//---------------------------------------------------------------------------------------
// Types list
//---------------------------------------------------------------------------------------

#define O3D_TYPELIST1(A) TypeList<A, NullType>
#define O3D_TYPELIST2(A,B) TypeList<A, O3D_TYPELIST1(B) >
#define O3D_TYPELIST3(A,B,C) TypeList<A, O3D_TYPELIST2(B,C) >
#define O3D_TYPELIST4(A,B,C,D) TypeList<A, O3D_TYPELIST3(B,C,D) >
#define O3D_TYPELIST5(A,B,C,D,E) TypeList<A, O3D_TYPELIST4(B,C,D,E) >
#define O3D_TYPELIST6(A,B,C,D,E,F) TypeList<A, O3D_TYPELIST5(B,C,D,E,F) >
#define O3D_TYPELIST7(A,B,C,D,E,F,G) TypeList<A, O3D_TYPELIST6(B,C,D,E,F,G) >
#define O3D_TYPELIST8(A,B,C,D,E,F,G,H) TypeList<A, O3D_TYPELIST7(B,C,D,E,F,G,H) >
#define O3D_TYPELIST9(A,B,C,D,E,F,G,H,I) TypeList<A, O3D_TYPELIST8(B,C,D,E,F,G,H,I) >
#define O3D_TYPELIST10(A,B,C,D,E,F,G,H,I,J) TypeList<A, O3D_TYPELIST9(B,C,D,E,F,G,H,I,J) >
#define O3D_TYPELIST11(A,B,C,D,E,F,G,H,I,J,K) TypeList<A, O3D_TYPELIST10(B,C,D,E,F,G,H,I,J,K) >

#define O3D_ISTYPEOF(FOO,LIST) \
	template <class _T>        \
	struct FOO {               \
	static const bool Val = TypeIndiceOf<LIST, typename ConstCast<_T>::Result >::Val != -1; }

//! Null type.
struct NullType {};

//! Atomic class.
class Atomic {};

//-----------------------------------------------------------------------------------

//! Static type condition like an if else branching
template <bool _Cond, class _TrueCond, class _FalseCond>
struct StaticCond;

template <class _TrueCond, class _FalseCond>
struct StaticCond <true, _TrueCond, _FalseCond>
{
    typedef _TrueCond Result;
};

template <class _TrueCond, class _FalseCond>
struct StaticCond <false, _TrueCond, _FalseCond>
{
    typedef _FalseCond Result;
};


//-----------------------------------------------------------------------------------

//! Template list of types
template <class _T, class _U>
struct TypeList
{
    typedef _T Head;
    typedef _U Tail;
};


//-----------------------------------------------------------------------------------

//! Signed or unsigned char
typedef StaticCond <(static_cast<Char>(-1)<0), Char, NullType >::Result _SignedChar;
typedef	StaticCond <(static_cast<Char>(-1)>=0), Char, NullType >::Result _UnsignedChar;

//! Signed or unsigned wchar_t
typedef StaticCond <(static_cast<WChar>(-1)<0), WChar, NullType >::Result _SignedWChar;
typedef	StaticCond <(static_cast<WChar>(-1)>=0), WChar, NullType >::Result _UnsignedWChar;


//-----------------------------------------------------------------------------------

//! Compute the length of the template list
template <class>
struct ListLength;

template <>
struct ListLength<NullType>
{
    enum { Val = 0 };
};

template <class _T, class _U>
struct ListLength< TypeList<_T, _U> >
{
    enum { Val = 1 + ListLength<_U>::Val };
};


//-----------------------------------------------------------------------------------

//! Get the type at the index i of a given list
template <class _TyList, unsigned int i>
struct TypeAt;

template <class _Head, class _Tail>
struct TypeAt<TypeList<_Head, _Tail>, 0>
{
    typedef _Head Type;
};

template <class _Head, class _Tail, unsigned int i>
struct TypeAt<TypeList<_Head, _Tail>, i>
{
    typedef typename TypeAt<_Tail, i-1>::Type Type;
};


//-----------------------------------------------------------------------------------

//! Get the index of a type of a given list
template <class _TyList, class _T>
struct TypeIndiceOf;

template <class _T>
struct TypeIndiceOf <NullType, _T>
{
    enum { Val = -1 };
};

template <class _Tail, class _T>
struct TypeIndiceOf <TypeList<_T, _Tail>, _T>
{
    enum { Val = 0 };
};

template <class _Head, class _Tail, class _T>
struct TypeIndiceOf <TypeList<_Head, _Tail>, _T>
{
private:
    enum
    {
        TempVal = TypeIndiceOf<_Tail, _T>::Val
    };
public:
    enum
    {
        Val = TempVal == -1 ? -1 : 1 + TempVal
    };
};


//-----------------------------------------------------------------------------------

//! Insert a type in a list of type
template <class _TyList, class _T>
struct InsertType;

template <>
struct InsertType <NullType, NullType>
{
    typedef NullType Result;
};

template <class _T>
struct InsertType <NullType, _T>
{
    typedef TypeList<_T, NullType> Result;
};

template <class _Head, class _Tail>
struct InsertType <NullType, TypeList<_Head, _Tail> >
{
    typedef TypeList<_Head, _Tail> Result;
};

template <class _Head, class _Tail, class _T>
struct InsertType <TypeList<_Head, _Tail>, _T>
{
    typedef TypeList <_Head, typename InsertType<_Tail, _T>::Result> Result;
};


//-----------------------------------------------------------------------------------

//! Remove a first occurrence of a type into a list of type
template <class _TyList, class _T>
struct RemoveType;

template <class _T>
struct RemoveType <NullType, _T>
{
    typedef NullType Result;
};

template <class _T, class _Tail>
struct RemoveType <TypeList<_T, _Tail>, _T>
{
    typedef _Tail Result;
};

template <class _Head, class _Tail, class _T>
struct RemoveType <TypeList<_Head, _Tail>, _T>
{
    typedef TypeList<_Head,	typename RemoveType<_Tail, _T>::Result>	Result;
};


//-----------------------------------------------------------------------------------

//! List of char types
typedef O3D_TYPELIST2(Char, WChar) CharTypes;


//-----------------------------------------------------------------------------------

//! List of float types
typedef O3D_TYPELIST2(Float, Double) FloatTypes;


//-----------------------------------------------------------------------------------

//! List of integer types
typedef O3D_TYPELIST11(
        Bool,
        Char, WChar,
        Int8, UInt8,
        Int16, UInt16,
        Int32, UInt32,
        Int64, UInt64) IntegerTypes;


//-----------------------------------------------------------------------------------

//! List of primitive types
typedef InsertType<
IntegerTypes, InsertType<
FloatTypes, void >::Result >::Result PrimitiveTypes;


//-----------------------------------------------------------------------------------

//! List of signed integer types
typedef O3D_TYPELIST6(
        Int8, Int16, Int32, Int64,
        _SignedChar, _SignedWChar) SignedIntegerTypes;


//-----------------------------------------------------------------------------------

//! List of signed types
typedef InsertType< SignedIntegerTypes, FloatTypes >::Result SignedTypes;


//-----------------------------------------------------------------------------------

//! List of unsigned integer types
typedef O3D_TYPELIST7(
        Bool,
        UInt8, UInt16, UInt32, UInt64,
        _UnsignedChar, _UnsignedWChar) UnsignedIntegerTypes;

//! List of unsigned types
typedef UnsignedIntegerTypes UnsignedTypes;


//-----------------------------------------------------------------------------------

//! Is a type is const
template <class _T> struct IsConst { static const bool Val = false; };
template <class _T> struct IsConst <const _T> { static const bool Val = true; };
template <class _T> struct IsConst <const _T *> { static const bool Val = true; };
template <class _T> struct IsConst <const _T &> { static const bool Val = true; };


//-----------------------------------------------------------------------------------

//! Is a type is a reference
template <class _T> struct IsReference { static const bool Val = false; };
template <class _T> struct IsReference <_T &> { static const bool Val = true; };


//-----------------------------------------------------------------------------------

//! Is a type is a volatile
template <class _T> struct IsVolatile { static const bool Val = false; };
template <class _T> struct IsVolatile <volatile _T> { static const bool Val = true; };
template <class _T> struct IsVolatile <volatile _T *> { static const bool Val = true; };
template <class _T> struct IsVolatile <volatile _T &> { static const bool Val = true; };


//-----------------------------------------------------------------------------------

//! Is a type is a reference
template <class _T> struct RefType { static const bool Val = false; };
template <class _T> struct RefType <_T &> { static const bool Val = true; };
template <class _T> struct RefType <const _T &> { static const bool Val = true; };


//-----------------------------------------------------------------------------------

//! Remove the const of a type
template <class _T> struct ConstCast { typedef _T Result; };
template <class _T> struct ConstCast <const _T> { typedef _T Result; };


//-----------------------------------------------------------------------------------

//! Remove the volatile of a type
template <class _T> struct VolatileCast { typedef _T Result; };
template <class _T> struct VolatileCast <volatile _T> { typedef _T Result; };
template <class _T> struct VolatileCast <volatile _T &> { typedef _T & Result; };
template <class _T> struct VolatileCast <volatile _T *> { typedef _T * Result; };


//-----------------------------------------------------------------------------------

//! Remove the reference of a type
template <class _T> struct ReferenceCast { typedef _T Result; };
template <class _T> struct ReferenceCast <_T &> { typedef _T Result; };


//-----------------------------------------------------------------------------------

//! Remove the pointer of a type
template <class _T> struct PtrCast { typedef _T PointedType; };
template <class _T> struct PtrCast <_T *> { typedef _T PointedType; };


//-----------------------------------------------------------------------------------

//! Test if _T is a primitive type
O3D_ISTYPEOF(IsPrimitive, PrimitiveTypes);

//! Test if _T is an integer type
O3D_ISTYPEOF(IsInteger, IntegerTypes);

//! Test if _T is a float type
O3D_ISTYPEOF(IsFloat, FloatTypes);

//! Test if _T is a signed integer type primitive type
O3D_ISTYPEOF(IsSignedInteger, SignedIntegerTypes);

//! Test if _T is a signed type primitive
O3D_ISTYPEOF(IsSigned, SignedTypes);

//! Test if _T is an unsigned integer type primitive
O3D_ISTYPEOF(IsUnsignedInteger, UnsignedIntegerTypes);

//! Test if _T is an unsigned type primitive
O3D_ISTYPEOF(IsUnsigned, UnsignedTypes);


//-----------------------------------------------------------------------------------

//! Type traits
template <class _T>
struct TypeTraits
{
    static const bool isConst = IsConst<_T>::Val;
    static const bool isVolatile = IsVolatile<_T>::Val;
    static const bool isReference = IsReference<_T>::Val;
    static const bool isPrimitive = IsPrimitive<_T>::Val;
    static const bool isInteger = IsInteger<_T>::Val;
    static const bool isFloat = IsFloat<_T>::Val;
    static const bool isSigned = IsSigned<_T>::Val;
    static const bool isUnsigned = IsUnsigned<_T>::Val;

private:

    template <class _U>
    struct PtrTraits
    {
        static const bool Val = false;
        typedef NullType PointedType;
    };
    template <class _U>
    struct PtrTraits<_U*>
    {
        static const bool Val = true;
        typedef _U PointedType;
    };
    template <class _U>
    struct PtrMethodTraits
    {
        static const bool Val = false;
    };
    template <class _U, class _V>
    struct PtrMethodTraits<_U _V::*>
    {
        static const bool Val = true;
    };

public:

    static const bool isPtr = PtrTraits<_T>::Val;
    static const bool isMethodPtr = PtrMethodTraits<_T>::Val;

    typedef typename ConstCast<_T>::Result NonConstType;
    typedef typename ReferenceCast<_T>::Result NonRefType;

    typedef typename PtrTraits<_T>::PointedType PointedType;

    //! Best type for use as parameter.
    typedef typename StaticCond<isPtr || isMethodPtr || isPrimitive, _T,
        typename StaticCond<!isReference, const NonRefType &, NonRefType & >::Result
        >::Result T_Parameter;
};

} // namespace o3d

#endif // _O3D_TYPES_H

