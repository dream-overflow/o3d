/**
 * @file vector2.h
 * @brief 2 dimensional vectors.
 * @author Emmanuel Ruffio
 * @date 2006-08-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_VECTOR2_H
#define _O3D_VECTOR2_H

#include <math.h>
#include "string.h"
#include "memorydbg.h"

namespace o3d {

template <class TYPE> class Vector2;

// Type definitions
typedef Vector2<Double>	Vector2d;
typedef Vector2<Float>	Vector2f;
typedef Vector2<Int32>	Vector2i;
typedef Vector2<UInt32>	Vector2ui;
typedef Vector2<Int16>	Vector2s;
typedef Vector2<UInt16>	Vector2us;
typedef Vector2<Int8>	Vector2b;
typedef Vector2<UInt8>	Vector2ub;

typedef Vector2d		Point2d;
typedef Vector2f		Point2f;
typedef Vector2i		Point2i;
typedef Vector2ui		Point2ui;
typedef Vector2s		Point2s;
typedef Vector2us		Point2us;
typedef Vector2b		Point2c;
typedef Vector2ub		Point2uc;

//---------------------------------------------------------------------------------------
//! @class Vector2
//-------------------------------------------------------------------------------------
//! Template class of a two dimensional vector
//---------------------------------------------------------------------------------------
template <class TYPE>
class O3D_API_TEMPLATE Vector2
{
public:

	//-----------------------------------------------------------------------------------
	// Constructors
	//-----------------------------------------------------------------------------------

	//! Default constructor
	Vector2() { V[X] = V[Y] = 0; }

	//! Initialization constructor
	Vector2(TYPE _x, TYPE _y) { V[X] = _x; V[Y] = _y; }

	//! Construct from another template Vector2
	template <class U>
	Vector2(const Vector2<U> & _which)
	{
		V[X] = static_cast<TYPE>(_which[X]);
		V[Y] = static_cast<TYPE>(_which[Y]);
	}

	//! construct from a ptr
	Vector2(const TYPE * _ptr) { V[X] = _ptr[X]; V[Y] = _ptr[Y]; }

	//! copy constructor
	Vector2(const Vector2& dup) { V[X] = dup.V[X]; V[Y] = dup.V[Y]; }

	//-----------------------------------------------------------------------------------
	// Methods
	//-----------------------------------------------------------------------------------

	//! define the vector
	inline void set(const TYPE & _x, const TYPE & _y)
	{
		V[X] = _x;
		V[Y] = _y;
	}

	//! read the vector content
	inline void read(TYPE &_x,TYPE &_y)const { _x = V[X]; _y = V[Y]; }

	//! normalize the vector (rang[0,1])
	inline void normalize()
	{
		TYPE value = norm2();
		if (value != 0.f) value = 1.f / value;

		V[0] *= value;
		V[1] *= value;
	}

	// return the norm1 of the vector
	inline TYPE norm1()const { return static_cast<TYPE>(abs(V[X]) + abs(V[Y])); }

	// return the norm2 of the vector
	inline TYPE norm2()const { return static_cast<TYPE>(sqrt(Double(V[X]*V[X] + V[Y]*V[Y]))); }

	//! return the infinite norm of the vector (the greatest absolute component)
	inline TYPE normInf()const { return max(abs(V[X]), abs(V[Y])); }

	//! return the length of the vector
	inline TYPE length()const { return norm2(); }

	//! return the squared length of the vector (norm²)
	inline TYPE squareLength()const { return (V[X] * V[X] + V[Y] * V[Y]); }

	//! return the ptr data of the vector
	inline TYPE* getData() { return V; }
	inline const TYPE* getData()const { return V; }

	//-----------------------------------------------------------------------------------
	// Operators
	//-----------------------------------------------------------------------------------

	//! Get a component of the vector
	inline       TYPE& operator[] (UInt32 i)      { return V[i]; }
	inline const TYPE& operator[] (UInt32 i)const { return V[i]; }

	//! get the X component (const)
	inline const TYPE& x() const { return V[X]; }
	//! Get the X ref component
	inline TYPE& x() { return V[X]; }
	//! get the Y component (const)
	inline const TYPE& y() const { return V[Y]; }
	//! Get the Y ref component
	inline TYPE& y() { return V[Y]; }

	//! set the vector from another 2d vector
	inline Vector2<TYPE>& operator= (const Vector2<TYPE> & _which)
	{
		V[X] = _which[X];
		V[Y] = _which[Y];

		return *this;
	}

	//! set the vector from another template 2d vector
	template <class U>
	inline Vector2<TYPE>& operator= (const Vector2<U> & _which)
	{
		V[X] = static_cast<TYPE>(_which[X]);
		V[Y] = static_cast<TYPE>(_which[Y]);

		return *this;
	}

	//! compare this to another given 2d vector
	inline Bool operator== (const Vector2& _which)const { return ((V[X] == _which[X]) && (V[Y] == _which[Y])); }
	inline Bool operator!= (const Vector2& _which)const { return ((V[X] != _which[X]) || (V[Y] != _which[Y])); }

	//! compare the length of this to another given 2d vector
	inline Bool operator<  (const Vector2& _which)const { return (this->length() <  _which.length()); }
	inline Bool operator>  (const Vector2& _which)const { return (this->length() >  _which.length()); }
	inline Bool operator<= (const Vector2& _which)const { return (this->length() <= _which.length()); }
	inline Bool operator>= (const Vector2& _which)const { return (this->length() >= _which.length()); }

	//! add this and another given 2d vector and return the new vector
	inline Vector2 operator+ (const Vector2& _which)const
	{
		return Vector2(V[X] + _which.V[X], V[Y] + _which.V[Y]);
	}

	//! Subtract this from another given 2d vector and return the new vector
	inline Vector2 operator- (const Vector2& _which)const
	{
		return Vector2(V[X] - _which.V[X], V[Y] - _which.V[Y]);
	}

	//! return a new vector where all component of the vector this are inverted
	inline Vector2 operator- ()const { return Vector2(-V[X],-V[Y]); }

	//! add a 2d vector to this and return this
	inline Vector2& operator+= (const Vector2& _which)
	{
		V[X] += _which.V[X];
		V[Y] += _which.V[Y];

		return *this;
	}

	//! Subtract a 2d vector from this and return this
	inline Vector2& operator-= (const Vector2& _which)
	{
		V[X] -= _which.V[X];
		V[Y] -= _which.V[Y];

		return *this;
	}

	//! return the scalar product of this to another given 2d vector
	inline TYPE operator* (const Vector2& _which)const
	{
		return (V[X] * _which.V[X] + V[Y] * _which.V[Y]);
	}

	//! return the product of this by a scalar
	inline Vector2 operator* (TYPE scale)const
	{
		return Vector2(scale * V[X], scale * V[Y]);
	}

	//! return the cross product of this to another given 2d vector
	inline Vector2<TYPE> operator^ (const Vector2& _which) const
	{
		return Vector2<TYPE>(
					V[X] * _which.V[Y] - V[Y] * _which.V[X],
					V[Y] * _which.V[X] - V[X] * _which.V[Y]);
	}

	//! return the cross product of this to another given 2d vector
	inline Vector2<TYPE>& operator^= (const Vector2& _which)
	{
		TYPE vx, vy;

		vx = V[X] * _which.V[Y] - V[Y] * _which.V[X];
		vy = V[Y] * _which.V[X] - V[X] * _which.V[Y];

		V[X] = vx;
		V[Y] = vy;

		return *this;
	}

	//! scale of this and return this
	inline Vector2& operator*= (TYPE scale)
	{
		V[X] *= scale;
		V[Y] *= scale;

		return *this;
	}

	//! divide this by a scalar and return the new vector
	inline Vector2 operator/ (TYPE scale)const
	{
		return Vector2(V[X] / scale, V[Y] / scale);
	}

	//! divide this by a scalar
	inline Vector2f& operator/= (TYPE scale)
	{
		V[X] /= scale;
		V[Y] /= scale;

		return *this;
	}

	//! set the vectors component to zero
	inline void zero() { V[X] = V[Y] = 0; }

	//! convert to a string
	inline operator String()const
	{
		String temp;
		temp << String("{ ") << V[0] << String(" , ") << V[1] << String(" }");
		return temp;
	}

	// Serialization
    inline Bool readFromFile(InStream &is)
	{
        is >> V[X]
           >> V[Y];

		return True;
	}

    inline Bool writeToFile(OutStream &os)const
	{
        os << V[X]
           << V[Y];

		return True;
	}

private:

	TYPE V[2];        //!< vector components
};

// Additional operators

// External products
template <class TYPE>
inline Vector2<TYPE> operator* (TYPE _scalar, const Vector2<TYPE> &_which)
{
	return Vector2<TYPE>(_scalar * _which[X], _scalar * _which[Y]);
}

// Specializations

template <> inline Vector2<Float>::Vector2() { V[X] = V[Y] = 0.f; }
template <> inline void Vector2<Float>::zero()  { V[X] = V[Y] = 0.f; }
template <> inline Bool Vector2<Float>::operator!= (const Vector2<Float>& vec)const;
template <> inline Bool Vector2<Float>::operator== (const Vector2<Float>& vec)const;
template <> inline Bool Vector2<Double>::operator!= (const Vector2<Double>& vec)const;
template <> inline Bool Vector2<Double>::operator== (const Vector2<Double>& vec)const;

template <> inline Float Vector2<Float>::norm2()const { return (sqrtf(V[X]*V[X] + V[Y]*V[Y])); }

struct O3D_API Vector
{
	static Vector2d  nullVector2d;
	static Vector2f  nullVector2f;
	static Vector2i  nullVector2i;
	static Vector2ui nullVector2ui;
	static Vector2s  nullVector2s;
	static Vector2us nullVector2us;
	static Vector2b  nullVector2b;
	static Vector2ub nullVector2ub;
};

} // namespace o3d

#endif // _O3D_VECTOR2_H

