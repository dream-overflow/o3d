/**
 * @file matrix4.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MATRIX4_H
#define _O3D_MATRIX4_H

#include "debug.h"
#include "templatearray.h"
#include "vector3.h"
#include "vector4.h"
#include "matrix3.h"
#include "memorydbg.h"

// SSE2 Optimization in O3D_USE_SIMD mode
#ifdef O3D_USE_SIMD
    #include <xmmintrin.h>
#endif // O3D_USE_SIMD

namespace o3d {

class String;

/**
 * @brief 4x4 matrix definition.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-02-18
 * 4x4 column major matrix. In memory first index represent the column. Accessors works
 * in row at first index. IE m23 mean row 2, column 3.
 * @note SIMD SSE2 optimizations are available when using O3D_USE_SIMD define.
 */
class O3D_API Matrix4
{
	friend class Math;

	#define M11 M[0]
	#define M12 M[4]
	#define M13 M[8]
	#define M14 M[12]

	#define M21 M[1]
	#define M22 M[5]
	#define M23 M[9]
	#define M24 M[13]

	#define M31 M[2]
	#define M32 M[6]
	#define M33 M[10]
	#define M34 M[14]

	#define M41 M[3]
	#define M42 M[7]
	#define M43 M[11]
	#define M44 M[15]

public:

	//! Return an identity matrix const reference.
    static const Matrix4& getIdentity() { return *ms_identity; }

	//! Return an null (full of zero) matrix const reference.
    static const Matrix4& getNull() { return *ms_null; }

	//! Atomic array initialization and set to identity
	inline void _initAtomicObject(void *ptr)
	{
		M = (Float*)ptr;
		identity();
	}

	//-----------------------------------------------------------------------------------
	// Matrix INITIALIZATION
	//-----------------------------------------------------------------------------------

	//! default constructor (set to identity)
	inline Matrix4()
	{
		M = (Float*)O3D_FAST_ALLOC(64);
		identity();
	}

	//! copy constructor
	inline Matrix4(const Matrix4& _M)
	{
		M = (Float*)O3D_FAST_ALLOC(64);
	#ifdef O3D_USE_SIMD
		__M128[0] = _mm_load_ps(&_M.M[0]);
		__M128[1] = _mm_load_ps(&_M.M[4]);
		__M128[2] = _mm_load_ps(&_M.M[8]);
		__M128[3] = _mm_load_ps(&_M.M[12]);
	#else
		memcpy(M,_M.M,sizeof(Float)*16);
	#endif
	}

	//! copy constructor from a float[16]
	inline Matrix4(const Float _M[16])
	{
		M = (Float*)O3D_FAST_ALLOC(64);
	#ifdef O3D_USE_SIMD
		__M128[0] = _mm_load_ps(&_M[0]);
		__M128[1] = _mm_load_ps(&_M[4]);
		__M128[2] = _mm_load_ps(&_M[8]);
		__M128[3] = _mm_load_ps(&_M[12]);
	#else
		memcpy(M,_M,sizeof(Float)*16);
	#endif
	}

	//! Initialization constructor
	inline Matrix4(
		Float a11,Float a12,Float a13,Float a14,
		Float a21,Float a22,Float a23,Float a24,
		Float a31,Float a32,Float a33,Float a34,
		Float a41,Float a42,Float a43,Float a44)
	{
		M = (Float*)O3D_FAST_ALLOC(64);
		setData(a11,a12,a13,a14,
				a21,a22,a23,a24,
				a31,a32,a33,a34,
				a41,a42,a43,a44);
	}

	//! destructor
	~Matrix4() { O3D_FAST_FREE(M,64); }

	//! set any entries of the matrix to zero
	inline void zero()
	{
	#ifdef O3D_USE_SIMD
		__M128[0] = _mm_setzero_ps();
		__M128[1] = _mm_setzero_ps();
		__M128[2] = _mm_setzero_ps();
		__M128[3] = _mm_setzero_ps();
	#else
		memset(M,0,sizeof(Float)*16);
	#endif
	}

	//! define as identity matrix
	inline void identity()
	{
#ifdef O3D_USE_SIMD
 		__M128[0] = _mm_set_ps(0.f,0.f,0.f,1.f);
		__M128[1] = _mm_set_ps(0.f,0.f,1.f,0.f);
		__M128[2] = _mm_set_ps(0.f,1.f,0.f,0.f);
		__M128[3] = _mm_set_ps(1.f,0.f,0.f,0.f);
#else
		M11 = 1.0f; M12 = 0.0f; M13 = 0.0f; M14 = 0.0f;
		M21 = 0.0f; M22 = 1.0f; M23 = 0.0f; M24 = 0.0f;
		M31 = 0.0f; M32 = 0.0f; M33 = 1.0f; M34 = 0.0f;
		M41 = 0.0f; M42 = 0.0f; M43 = 0.0f; M44 = 1.0f;
#endif
	}

	//! Initialize any entry of the matrix
	inline void setData(
		Float a11,Float a12,Float a13,Float a14,
		Float a21,Float a22,Float a23,Float a24,
		Float a31,Float a32,Float a33,Float a34,
		Float a41,Float a42,Float a43,Float a44)
	{
	#ifdef O3D_USE_SIMD
		__M128[0] = _mm_set_ps(a41,a31,a21,a11);
		__M128[1] = _mm_set_ps(a42,a32,a22,a12);
		__M128[2] = _mm_set_ps(a43,a33,a23,a13);
		__M128[3] = _mm_set_ps(a44,a34,a24,a14);
	#else
		M11 = a11; M12 = a12; M13 = a13; M14 = a14;
		M21 = a21; M22 = a22; M23 = a23; M24 = a24;
		M31 = a31; M32 = a32; M33 = a33; M34 = a34;
		M41 = a41; M42 = a42; M43 = a43; M44 = a44;
	#endif
	}

	//! duplicate
	inline Matrix4& operator= (const Matrix4& _M)
	{
	#ifdef O3D_USE_SIMD
		__M128[0] = _mm_load_ps(&_M.M[0]);
		__M128[1] = _mm_load_ps(&_M.M[4]);
		__M128[2] = _mm_load_ps(&_M.M[8]);
		__M128[3] = _mm_load_ps(&_M.M[12]);
	#else
		memcpy(M,_M.M,sizeof(Float)*16);
	#endif
		return *this;
	}

	//! define the translation part of the matrix
	inline Matrix4& operator= (const Vector3& v)
	{
		M14 = v[X];
		M24 = v[Y];
		M34 = v[Z];

		return *this;
	}

	//-----------------------------------------------------------------------------------
	// Matrix TRANSPOSE
	//-----------------------------------------------------------------------------------

	//! Auto Transpose the matrix.
	void transpose();

	//! Transpose the matrix and return the result.
	Matrix4 transposeTo() const;

	//-----------------------------------------------------------------------------------
	// Matrix ORTHONORMAL INVERT
	//-----------------------------------------------------------------------------------

	//! Invert the Matrix4 and return the result. This method is not specialized to
	//! orthonormals matrices like Invert() and operator!().
	Matrix4 invertStd() const;

	//! Invert an orthonormal Matrix4 and return the result.
	Matrix4 invert() const;

	//! Auto invert an orthonormal Matrix4.
	Matrix4& operator!();

	//-----------------------------------------------------------------------------------
	// Matrix COMPARISONS
	//-----------------------------------------------------------------------------------

	//! Is two matrices are equal.
	inline Bool operator== (const Matrix4 &_M) const
	{
		return (memcmp(_M.M,M,sizeof(Float) * 16) == 0);
		// TODO in SSE2
	}

	//! Is two matrices are different.
	inline Bool operator!= (const Matrix4 &_M) const
	{
		return (memcmp(_M.M,M,sizeof(Float) * 16) != 0);
		// TODO in SSE2
	}

	//-----------------------------------------------------------------------------------
	// Matrix VECTOR OPERATIONS
	//-----------------------------------------------------------------------------------

	//! Translate from +v.
	inline Matrix4& operator+= (const Vector3 &v)
	{
		M14 += v[X];
		M24 += v[Y];
		M34 += v[Z];

		return (*this);
	}

	//! Translate from -v.
	inline Matrix4& operator-= (const Vector3 &v)
	{
		M14 -= v[X];
		M24 -= v[Y];
		M34 -= v[Z];

		return (*this);
	}

	//! Matrix4 * Vector3, v = M * vec. W vector component set to 1.
	inline Vector3 operator* (const Vector3 &vec) const
	{
		Vector3 v;

		v[X] = M11*vec[X] + M12*vec[Y] + M13*vec[Z] + M14;
		v[Y] = M21*vec[X] + M22*vec[Y] + M23*vec[Z] + M24;
		v[Z] = M31*vec[X] + M32*vec[Y] + M33*vec[Z] + M34;

		return v;
	}

	//! Matrix4 * Vector4, v = M * vec.
	inline Vector4 operator* (const Vector4 &vec) const
	{
		Vector4 v;

		v[X] = M11*vec[X] + M12*vec[Y] + M13*vec[Z] + M14*vec[W];
		v[Y] = M21*vec[X] + M22*vec[Y] + M23*vec[Z] + M24*vec[W];
		v[Z] = M31*vec[X] + M32*vec[Y] + M33*vec[Z] + M34*vec[W];
		v[W] = M41*vec[X] + M42*vec[Y] + M43*vec[Z] + M44*vec[W];

		return v;
	}

	//! Scale the translation part of the matrix by each component of the given vector.
	inline Matrix4& operator*= (const Vector3& v)
	{
		M14 *= v[X];
		M24 *= v[Y];
		M34 *= v[Z];

		return (*this);
	}

	//! Transform the given vector by the rotation (upper 3x3 matrix) and return the result.
	inline Vector3 rotate(const Vector3& vec) const
	{
		Vector3 v;

		v[X] = M11*vec[X] + M12*vec[Y] + M13*vec[Z];
		v[Y] = M21*vec[X] + M22*vec[Y] + M23*vec[Z];
		v[Z] = M31*vec[X] + M32*vec[Y] + M33*vec[Z];

		return v;
	}

	//! Transform the given vector by the Z part of the matrix only. Mainly used to
	//! compute Z average for a triangle when need to sort faces.
	//! @return Z component of the transformed vector.
	inline Float transformOnZ(Float x, Float y, Float z) const
	{
		return (M31*x + M32*y + M33*z + M34);
	}

	//! Transform the given vector by the Z part of the matrix only. Mainly used to
	//! compute Z average for a triangle when need to sort faces.
	//! @return Z component of the transformed vector.
	inline Float transformOnZ(const Vector3& v) const
	{
		return (M31*v[X] + M32*v[Y] + M33*v[Z] + M34);
	}

	//-----------------------------------------------------------------------------------
	// Matrix CROSS PRODUCT
	//-----------------------------------------------------------------------------------

	//! Matrix cross product from this * _M and return the result into __M
	inline void mult(const Matrix4& _M, Matrix4& __M) const
	{
	#if defined(O3D_USE_SIMD)
		__M.__M128[0] = _mm_add_ps(_mm_add_ps(_mm_add_ps(
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[0], _M.__M128[0], _MM_SHUFFLE(0,0,0,0)), __M128[0]),
	    	_mm_mul_ps(_mm_shuffle_ps(_M.__M128[0], _M.__M128[0], _MM_SHUFFLE(1,1,1,1)), __M128[1])),
	    	_mm_mul_ps(_mm_shuffle_ps(_M.__M128[0], _M.__M128[0], _MM_SHUFFLE(2,2,2,2)), __M128[2])),
	    	_mm_mul_ps(_mm_shuffle_ps(_M.__M128[0], _M.__M128[0], _MM_SHUFFLE(3,3,3,3)), __M128[3]));

		__M.__M128[1] = _mm_add_ps(_mm_add_ps(_mm_add_ps(
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[1], _M.__M128[1], _MM_SHUFFLE(0,0,0,0)), __M128[0]),
		 	_mm_mul_ps(_mm_shuffle_ps(_M.__M128[1], _M.__M128[1], _MM_SHUFFLE(1,1,1,1)), __M128[1])),
		 	_mm_mul_ps(_mm_shuffle_ps(_M.__M128[1], _M.__M128[1], _MM_SHUFFLE(2,2,2,2)), __M128[2])),
		 	_mm_mul_ps(_mm_shuffle_ps(_M.__M128[1], _M.__M128[1], _MM_SHUFFLE(3,3,3,3)), __M128[3]));

		__M.__M128[2] = _mm_add_ps(_mm_add_ps(_mm_add_ps(
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[2], _M.__M128[2], _MM_SHUFFLE(0,0,0,0)), __M128[0]),
		 	_mm_mul_ps(_mm_shuffle_ps(_M.__M128[2], _M.__M128[2], _MM_SHUFFLE(1,1,1,1)), __M128[1])),
		 	_mm_mul_ps(_mm_shuffle_ps(_M.__M128[2], _M.__M128[2], _MM_SHUFFLE(2,2,2,2)), __M128[2])),
		 	_mm_mul_ps(_mm_shuffle_ps(_M.__M128[2], _M.__M128[2], _MM_SHUFFLE(3,3,3,3)), __M128[3]));

		__M.__M128[3] = _mm_add_ps(_mm_add_ps(_mm_add_ps(
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[3], _M.__M128[3], _MM_SHUFFLE(0,0,0,0)), __M128[0]),
		 	_mm_mul_ps(_mm_shuffle_ps(_M.__M128[3], _M.__M128[3], _MM_SHUFFLE(1,1,1,1)), __M128[1])),
		 	_mm_mul_ps(_mm_shuffle_ps(_M.__M128[3], _M.__M128[3], _MM_SHUFFLE(2,2,2,2)), __M128[2])),
		 	_mm_mul_ps(_mm_shuffle_ps(_M.__M128[3], _M.__M128[3], _MM_SHUFFLE(3,3,3,3)), __M128[3]));
	#else
		register UInt32 i,j,r; // i column, j row

		for ( i = 0 ; i < 4 ; ++i)
		{
			r = i << 2;
			for ( j = 0 ; j < 4 ; ++j)
				__M.M[j+r] = M[j+ 0] * _M.M[0+r] +
					M[j+ 4] * _M.M[1+r] +
					M[j+ 8] * _M.M[2+r] +
					M[j+12] * _M.M[3+r];
		}
	#endif
	}

	//! renvoi le produit matriciel de deux matrice M et _M
	inline Matrix4 operator* (const Matrix4& _M) const
	{
		Matrix4 __M((Float*)O3D_FAST_ALLOC(64));
		mult(_M,__M);
		return __M;
	}

	//! produit matriciel M = M * _M
	inline Matrix4& operator*= (const Matrix4& _M)
	{
	#ifdef O3D_USE_SIMD
		__m128 m0,m1,m2,m3;

		m0 = _mm_add_ps(_mm_add_ps(_mm_add_ps(
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[0], _M.__M128[0], _MM_SHUFFLE(0,0,0,0)), __M128[0]),
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[0], _M.__M128[0], _MM_SHUFFLE(1,1,1,1)), __M128[1])),
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[0], _M.__M128[0], _MM_SHUFFLE(2,2,2,2)), __M128[2])),
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[0], _M.__M128[0], _MM_SHUFFLE(3,3,3,3)), __M128[3]));

		m1 = _mm_add_ps(_mm_add_ps(_mm_add_ps(
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[1], _M.__M128[1], _MM_SHUFFLE(0,0,0,0)), __M128[0]),
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[1], _M.__M128[1], _MM_SHUFFLE(1,1,1,1)), __M128[1])),
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[1], _M.__M128[1], _MM_SHUFFLE(2,2,2,2)), __M128[2])),
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[1], _M.__M128[1], _MM_SHUFFLE(3,3,3,3)), __M128[3]));

		m2 = _mm_add_ps(_mm_add_ps(_mm_add_ps(
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[2], _M.__M128[2], _MM_SHUFFLE(0,0,0,0)), __M128[0]),
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[2], _M.__M128[2], _MM_SHUFFLE(1,1,1,1)), __M128[1])),
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[2], _M.__M128[2], _MM_SHUFFLE(2,2,2,2)), __M128[2])),
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[2], _M.__M128[2], _MM_SHUFFLE(3,3,3,3)), __M128[3]));

		m3 = _mm_add_ps(_mm_add_ps(_mm_add_ps(
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[3], _M.__M128[3], _MM_SHUFFLE(0,0,0,0)), __M128[0]),
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[3], _M.__M128[3], _MM_SHUFFLE(1,1,1,1)), __M128[1])),
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[3], _M.__M128[3], _MM_SHUFFLE(2,2,2,2)), __M128[2])),
			_mm_mul_ps(_mm_shuffle_ps(_M.__M128[3], _M.__M128[3], _MM_SHUFFLE(3,3,3,3)), __M128[3]));

		__M128[0] = m0;
		__M128[1] = m1;
		__M128[2] = m2;
		__M128[3] = m3;
	#else
		register UInt32 i,j,r; // i column, j row
		O3D_ALIGN(16) Float __M[16];

		for ( i = 0 ; i < 4 ; ++i)
		{
			r = i << 2;
			for ( j = 0 ; j < 4 ; ++j)
				__M[j+r] = M[j+ 0] * _M.M[0+r] +
					M[j+ 4] * _M.M[1+r] +
					M[j+ 8] * _M.M[2+r] +
					M[j+12] * _M.M[3+r];
		}

		memcpy(M,__M,sizeof(Float)*16);
	#endif
		return (*this);
	}

	//-----------------------------------------------------------------------------------
	// matrix SCALAR OPERATIONS (SEE SCALE TRANSFORMATION TOO)
	//-----------------------------------------------------------------------------------

	//! Uniform scale.
	inline void scale(const Float scale)
	{
	#ifdef O3D_USE_SIMD
		__m128 __scal = _mm_set_ps1(scale);
		__M128[0] = _mm_mul_ps(__M128[0],__scal);
		__M128[1] = _mm_mul_ps(__M128[1],__scal);
		__M128[2] = _mm_mul_ps(__M128[2],__scal);
		__M128[3] = _mm_mul_ps(__M128[3],__scal);
	#else
		M11 *= scale; M12 *= scale; M13 *= scale; M14 *= scale;
		M21 *= scale; M22 *= scale; M23 *= scale; M24 *= scale;
		M31 *= scale; M32 *= scale; M33 *= scale; M34 *= scale;
		M41 *= scale; M42 *= scale; M43 *= scale; M44 *= scale;
	#endif
	}

	//! Scale any entry of the matrix by scale
	//! @note For a Scale -1 prefer the use of Negative
	inline Matrix4& operator*= (const Float scalar)
	{
		scale(scalar);
		return (*this);
	}

	//! Scale any entry of the matrix by scale and return the new one.
	//! @note For a Scale -1 prefer the use of Negative
	inline Matrix4 operator* (const Float scalar) const
	{
		Matrix4 _M(*this);
		_M.scale(scalar);
		return _M;
	}

	//! negative any entry of the matrix and return this new matrix
	inline Matrix4 operator- () const
	{
		Matrix4 _M(*this);
		_M.negative();
		return _M;
	}

	//! negative any entry of the matrix
	inline void negative()
	{
		M11 = -M11; M12 = -M12; M13 = -M13; M14 = -M14;
		M21 = -M21; M22 = -M22; M23 = -M23; M24 = -M24;
		M31 = -M31; M32 = -M32; M33 = -M33; M34 = -M34;
		M41 = -M41; M42 = -M42; M43 = -M43; M44 = -M44;
	}

	//-----------------------------------------------------------------------------------
	// Matrix ADDITION
	//-----------------------------------------------------------------------------------

	//! add two matrix this + _M and return into __M
	inline void add(const Matrix4& _M, Matrix4& __M) const
	{
	#ifdef O3D_USE_SIMD
		__M.__M128[0] = _mm_add_ps(__M128[0], _M.__M128[0]);
		__M.__M128[1] = _mm_add_ps(__M128[1], _M.__M128[1]);
		__M.__M128[2] = _mm_add_ps(__M128[2], _M.__M128[2]);
		__M.__M128[3] = _mm_add_ps(__M128[3], _M.__M128[3]);
	#else
		__M.M11 = M11 + _M.M11; __M.M12 = M12 + _M.M12; __M.M13 = M13 + _M.M13; __M.M14 = M14 + _M.M14;
		__M.M21 = M21 + _M.M21; __M.M22 = M22 + _M.M22; __M.M23 = M23 + _M.M23; __M.M24 = M24 + _M.M24;
		__M.M31 = M31 + _M.M31; __M.M32 = M32 + _M.M32; __M.M33 = M33 + _M.M33; __M.M34 = M34 + _M.M34;
		__M.M41 = M41 + _M.M41; __M.M42 = M42 + _M.M42; __M.M43 = M43 + _M.M43; __M.M44 = M44 + _M.M44;
	#endif
	}

	//! add two matrix this + _M and return a new matrix (lesser efficient than Add)
	inline Matrix4 operator+ (const Matrix4& _M) const
	{
		Matrix4 __M((Float*)O3D_FAST_ALLOC(64));
		add(_M,__M);
		return  __M;
	}

	//! add the given matrix _M to this (M) , M = M + _M
	inline Matrix4& operator+= (const Matrix4& _M)
	{
		add(_M,*this);
		return (*this);
	}

	//-----------------------------------------------------------------------------------
	// Matrix SUBSTRACT
	//-----------------------------------------------------------------------------------

	//! sub two matrix this + _M and return into __M
	inline void sub(const Matrix4& _M, Matrix4& __M) const
	{
	#ifdef O3D_USE_SIMD
		__M.__M128[0] = _mm_sub_ps(__M128[0], _M.__M128[0]);
		__M.__M128[1] = _mm_sub_ps(__M128[1], _M.__M128[1]);
		__M.__M128[2] = _mm_sub_ps(__M128[2], _M.__M128[2]);
		__M.__M128[3] = _mm_sub_ps(__M128[3], _M.__M128[3]);
	#else
		__M.M11 = M11 - _M.M11; __M.M12 = M12 - _M.M12; __M.M13 = M13 - _M.M13; __M.M14 = M14 - _M.M14;
		__M.M21 = M21 - _M.M21; __M.M22 = M22 - _M.M22; __M.M23 = M23 - _M.M23; __M.M24 = M24 - _M.M24;
		__M.M31 = M31 - _M.M31; __M.M32 = M32 - _M.M32; __M.M33 = M33 - _M.M33; __M.M34 = M34 - _M.M34;
		__M.M41 = M41 - _M.M41; __M.M42 = M42 - _M.M42; __M.M43 = M43 - _M.M43; __M.M44 = M44 - _M.M44;
	#endif
	}

	//! sub two matrix this + _M and return a new matrix (lesser efficient than Sub)
	inline Matrix4 operator- (const Matrix4& _M) const
	{
		Matrix4 __M((Float*)O3D_FAST_ALLOC(64));
		sub(_M,__M);
		return  __M;
	}

	//! subtract the given matrix _M to this (M) , M = M + _M
	inline Matrix4& operator-= (const Matrix4& _M)
	{
		sub(_M,*this);
		return (*this);
	}

	//-----------------------------------------------------------------------------------
	// matrix TRANSFORMATION (see VECTOR OPERATIONS TOO)
	//-----------------------------------------------------------------------------------

	//! translate la matrice M de v
	inline void translate(const Vector3& v)
	{
//	#ifdef O3D_USE_SIMD
//		__M128[3] = _mm_add_ps(__M128[3],v.__M128);
//	#else
		M14 += v[X];
		M24 += v[Y];
		M34 += v[Z];
//	#endif
	}

	//! translate la matrice M sur un axe
	inline void translate(UInt32 axe,Float v) { M[12+axe] += v; }

	//! translate la matrice M de {x,y,z}
	inline void translate(const Float x,const Float y,const Float z)
	{
	#ifdef O3D_USE_SIMD
		__m128 v = _mm_set_ps(0,z,y,x);
		__M128[3] = _mm_add_ps(__M128[3],v);
	#else
		M14 += x;
		M24 += y;
		M34 += z;
	#endif
	}

	//! Rotate from X axis of alpha radian.
	void rotateX(const Float alpha);
	//! Rotate from Y axis of alpha radian.
	void rotateY(const Float alpha);
	//! Rotate from Z axis of alpha radian.
	void rotateZ(const Float alpha);
	//! Rotate from an axis of alpha radian.
	inline void rotate(UInt32 axe,const Float alpha)
	{
		switch (axe)
		{
		case X:
			rotateX(alpha);
			break;
		case Y:
			rotateY(alpha);
			break;
		case Z:
			rotateZ(alpha);
			break;
		default:
			break;
		}
	}

	//! Absolute rotate from X axis of alpha radian.
	void rotateAbsX(Float alpha);
	//! Absolute rotate from Y axis of alpha radian.
	void rotateAbsY(Float alpha);
	//! Absolute rotate from Z axis of alpha radian.
	void rotateAbsZ(Float alpha);
	//! Absolute rotate from an axis of alpha radian.
	inline void rotateAbs(UInt32 axe,Float alpha)
	{
		switch (axe)
		{
		case X:
			rotateAbsX(alpha);
			break;
		case Y:
			rotateAbsY(alpha);
			break;
		case Z:
			rotateAbsZ(alpha);
			break;
		default:
			break;
		}
	}

	//! Scale on the three axis. W is supposed to be one.
	inline void scale(const Vector3 &V)
	{
	#ifdef O3D_USE_SIMD
		__m128 v = _mm_set_ps(1.f,V[Z],V[Y],V[X]);
		__M128[0] = _mm_mul_ps(__M128[0],v);//v.__M128);
		__M128[1] = _mm_mul_ps(__M128[1],v);//v.__M128);
		__M128[2] = _mm_mul_ps(__M128[2],v);//v.__M128);
		__M128[3] = _mm_mul_ps(__M128[3],v);//v.__M128);
	#else
		for (Int32 i = 0 ; i < 3 ; ++i)
			for (Int32 j =  0 ; j < 4 ; ++j)
				M[(i<<2)+j] *= V[i];
	#endif
	}

    //! Scale a specific axis.
	inline void scale(UInt32 axe, Float scale)
	{
	#ifdef O3D_USE_SIMD
		__m128 v = _mm_set_ps1(scale);
		__M128[axe] = _mm_mul_ps(__M128[axe],v);
	#else
		for (Int32 i = 0 ; i < 4 ; ++i)
			M[(axe<<2)+i] *= scale;
	#endif
	}

	//! Unscale the matrix.
	void unscale();

	//! Return an uniform scale of the matrix.
	inline Matrix4 scaleTo(Float scale) const
	{
		Matrix4 _M(*this);
		_M.scale(scale);
		return _M;
	}

	//! Recompute the orthonormal matrix basis.
	void reComputeBasisGivenX(Vector3 v);
	//! Recompute the orthonormal matrix basis.
	void reComputeBasisGivenY(Vector3 v);
	//! Recompute the orthonormal matrix basis.
	void reComputeBasisGivenZ(Vector3 v);

	//-----------------------------------------------------------------------------------
	// Matrix ACCESSORS
	//-----------------------------------------------------------------------------------

	//! Return the column i of the matrix (read only).
	inline const Float* operator[] (UInt32 i) const
	{
		O3D_ASSERT(i < 4);
		return &M[i<<2];
	}
	//! Return the column i of the matrix.
	inline Float* operator[] (UInt32 i)
	{
		O3D_ASSERT(i < 4);
		return &M[i<<2];
	}

	//! Set an entry of the matrix
	//! @param i row
	//! @param j column
	inline void setData(UInt32 i, UInt32 j, Float s)
	{
		O3D_ASSERT(i < 4 && j < 4);
		M[i+(j<<2)] = s;
	}

	//! Get an entry (read only).
	//! @param i row
	//! @param j column
	inline const Float& operator() (UInt32 i, UInt32 j) const
	{
		O3D_ASSERT(i < 4 && j < 4);
		return M[i+(j<<2)];
	}

	//! Get an entry.
	//! @param i row
	//! @param j column
	inline Float& operator() (UInt32 i, UInt32 j)
	{
		O3D_ASSERT(i < 4 && j < 4);
		return M[i+(j<<2)];
	}

	//! Return the X column of the matrix.
	inline Vector3 getX() const { return &M11; }
	//! Return the Y column of the matrix.
	inline Vector3 getY() const { return &M12; }
	//! Return the Z column of the matrix.
	inline Vector3 getZ() const { return &M13; }

	//! Return the fourth column of the matrix (translation part).
	inline Vector3 getTranslation() const { return &M14; }

	//! Return the fourth column of the matrix pointer (translation part).
	inline Float* getTranslationPtr() { return &M14; }
	//! Return the fourth column of the matrix pointer (translation part) (read only).
	inline const Float* getTranslationPtr() const { return &M14; }

	//! Get the first row.
	inline Vector4 getRow1() const
	{
		return Vector4(M11, M12, M13, M14);
	}

	//! Get the second row.
	inline Vector4 getRow2() const
	{
		return Vector4(M21, M22, M23, M24);
	}

	//! Get the third row.
	inline Vector4 getRow3() const
	{
		return Vector4(M31, M32, M33, M34);
	}

	//! Get the fourth row.
	inline Vector4 getRow4() const
	{
		return Vector4(M41, M42, M43, M44);
	}

	//! return the data pointer.
	inline Float* getData() { return M; }
	//! return the data pointer (read only).
	inline const Float* getData() const { return M; }

	inline Float m11() const { return M[0]; }
	inline Float m12() const { return M[4]; }
	inline Float m13() const { return M[8]; }
	inline Float m14() const { return M[12]; }

	inline Float m21() const { return M[1]; }
	inline Float m22() const { return M[5]; }
	inline Float m23() const { return M[9]; }
	inline Float m24() const { return M[13]; }

	inline Float m31() const { return M[2]; }
	inline Float m32() const { return M[6]; }
	inline Float m33() const { return M[10]; }
	inline Float m34() const { return M[14]; }

	inline Float m41() const { return M[3]; }
	inline Float m42() const { return M[7]; }
	inline Float m43() const { return M[11]; }
	inline Float m44() const { return M[15]; }

	//! Set the column X of the matrix.
	inline void setX(const Vector3& vec)
	{
		M11 = vec[X];
		M21 = vec[Y];
		M31 = vec[Z];
	}

	//! Set the column Y of the matrix.
	inline void setY(const Vector3& vec)
	{
		M12 = vec[X];
		M22 = vec[Y];
		M32 = vec[Z];
	}

	//! Set the column Z of the matrix.
	inline void setZ(const Vector3& vec)
	{
		M13 = vec[X];
		M23 = vec[Y];
		M33 = vec[Z];
	}

	//! Set the translation column of the matrix.
	inline void setTranslation(const Vector3& vec)
	{
		M14 = vec[X];
		M24 = vec[Y];
		M34 = vec[Z];
	}

	//! Set the translation column of the matrix.
	inline void setTranslation(Float x,Float y,Float z)
	{
		M14 = x;
		M24 = y;
		M34 = z;
	}

	//! Get the scale of the three axis.
	inline Vector3 getScale() const
	{
		return Vector3(getX().length(), getY().length(), getZ().length());
	}

	//! Get the rotation matrix (3x3 matrix).
	inline Matrix3 getRotation() const { return Matrix3(*this); }

	//! Set the 3x3 rotation sub-matrix.
	inline void setRotation(const Matrix3& mat)
	{
		for (UInt32 i = 0 ; i < 3 ; ++i)
			for (UInt32 j = 0 ; j < 3 ; ++j)
				M[i+(j<<2)] = mat(i,j);
	}

	//! Define the matrix as a rotation matrix by an axis and angle (like glRotate).
	void setRotation(const Vector3 &axis, Float angle);

	//! Get the 3 rotations angles.
	void getRotation(Float &x, Float &y, Float &z) const;

	//-----------------------------------------------------------------------------------
	// Build a matrix
	//-----------------------------------------------------------------------------------

	//! set a look at matrix (like gluLookAt)
	void setLookAt(const Vector3 &eye, const Vector3 &center, const Vector3 &up);

	//! Set the matrix as skew effect
	//! @param angle Skew angle in degree
	//! @param a First vector
	//! @param b Second vector
	void setSkew(Float angle, const Vector3 &a, const Vector3 &b);

	//! Build a perspective matrix
	//! @param coef Ratio w/h
	//! @param fov Field of view expressed in degree
	//! @param znear minimal Z clipping distance
	//! @param zfar maximal Z clipping distance
	void buildPerspective(Float coef, Float fov, Float znear, Float zfar);

	//! Build a perspective matrix with infinite zfar.
	//! @param coef Ratio w/h
	//! @param fov Field of view expressed in degree
	//! @param znear minimal Z clipping distance
	void buildPerspective(Float coef, Float fov, Float znear);

	//! Build an ortho matrix taken left,top = -1 and right,bottom = 1
	//! @param znear minimal Z clipping distance
	//! @param zfar maximal Z clipping distance
	void buildOrtho(Float znear, Float zfar);

	//! Build an ortho matrix
	//! @paral left Left clipping plane
	//! @paral left Right clipping plane
	//! @paral left Bottom clipping plane
	//! @paral left Top clipping plane
	//! @param znear minimal Z clipping distance
	//! @param zfar maximal Z clipping distance
	void buildOrtho(
		Float left,
		Float right,
		Float bottom,
		Float top,
		Float znear,
		Float zfar);

	//! Convert to a string.
	operator String() const;

	//-----------------------------------------------------------------------------------
	// Matrix SERIALIZATION
	//-----------------------------------------------------------------------------------

	// Serialization
    Bool writeToFile(OutStream& os) const;
    Bool readFromFile(InStream &is);

	#undef M11
	#undef M12
	#undef M13
	#undef M14

	#undef M21
	#undef M22
	#undef M23
	#undef M24

	#undef M31
	#undef M32
	#undef M33
	#undef M34

private:

#ifdef O3D_USE_SIMD
	union {
		__m128 *__M128;
		Float *M;
	};
#else
	union {
		Float *M;
	};
#endif

	//! Initialization constructor from an external float*
	//! @note _M must be 16 bytes aligned
	inline Matrix4(Float *_M) { M = _M; }

    static Matrix4 *ms_identity;
    static Matrix4 *ms_null;

	static void init();
	static void quit();
};

// serialize
inline OutStream& operator<< (OutStream &os, const Matrix4 &object)
{
    object.writeToFile(os);
    return os;
}

// serialize
inline OutStream& operator<< (OutStream &os, Matrix4 &object)
{
    object.writeToFile(os);
    return os;
}

// de-serialize
inline InStream& operator>> (InStream &is, Matrix4 &object)
{
    object.readFromFile(is);
    return is;
}

} // namespace o3d

#endif // _O3D_MATRIX4_H
