/**
 * @file vector4.h
 * @brief 4 dimensional vector.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-07-21
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_VECTOR4_H
#define _O3D_VECTOR4_H

#include "math.h"
#include "memorydbg.h"

namespace o3d {

class InStream;
class OutStream;
class String;
class Vector3;

/**
 * @brief 4 dimensionals vector.
 */
class O3D_API Vector4
{
public:

	//! Default constructor. All component are set to zero.
	inline Vector4() { V[X] = V[Y] = V[Z] = V[W] = 0.0f; }

	//! Copy constructor
	inline Vector4(const Vector4& vec)
	{
		V[X] = vec.V[X];
		V[Y] = vec.V[Y];
		V[Z] = vec.V[Z];
		V[W] = vec.V[W];
	}

	//! Initialization constructor from 4 float
	explicit inline Vector4(const Float _x,const Float _y,const Float _z,const Float _w)
	{
		V[X] = _x;
		V[Y] = _y;
		V[Z] = _z;
		V[W] = _w;
	}

	//! Initialization constructor from a Float[4] array
	inline Vector4(const Float *vec)
	{
		V[X] = vec[X];
		V[Y] = vec[Y];
		V[Z] = vec[Z];
		V[W] = vec[W];
	}

	//! Initialize from a vector3 and a w component.
	explicit Vector4(const Vector3 &vec3, Float w);

	//! Define the content of the vector given 4 scalars
	inline void set(
			const Float _x = 0.0f,
			const Float _y = 0.0f,
			const Float _z = 0.0f,
			const Float _w = 0.0f)
	{
		V[X] = _x;
		V[Y] = _y;
		V[Z] = _z;
		V[W] = _w;
	}

	//! Define the content of the vector given a float array
	inline void set(const Float* vec)
	{
		V[X] = vec[X];
		V[Y] = vec[Y];
		V[Z] = vec[Z];
		V[W] = vec[W];
	}

	//! read the content of the vector (x,y,z,w)
	inline void read(Float& _x,Float& _y,Float& _z,Float& _w) const
	{
		_x = V[X];
		_y = V[Y];
		_z = V[Z];
		_w = V[W];
	}

	//! Return the ref of the component i of the vector
	inline Float& operator[] (UInt32 i) { return V[i]; }
	//! return the component i of the vector (const)
	inline const Float& operator[] (UInt32 i) const { return V[i]; }

	//! get the X component (const)
	inline const Float& x() const { return V[X]; }
	//! Get the X ref component
	inline Float& x() { return V[X]; }
	//! get the Y component (const)
	inline const Float& y() const { return V[Y]; }
	//! Get the Y ref component
	inline Float& y() { return V[Y]; }
	//! get the Z component (const)
	inline const Float& z() const { return V[Z]; }
	//! Get the Z ref component
	inline Float& z() { return V[Z]; }
	//! get the W component (const)
	inline const Float& w() const { return V[W]; }
	//! Get the W ref component
	inline Float& w() { return V[W]; }

	//! Return the vector content array
	inline Float* getData() { return V; }
	//! return the vector content array (const version)
	inline const Float* getData() const { return V; }

	//! Compute the norm2.
    inline Float length() const { return Math::sqrt(V[X]*V[X] + V[Y]*V[Y] + V[Z]*V[Z]+ V[W]*V[W]); }

	//! Return the vector square length
	inline Float squarelength() const { return (V[X]*V[X] + V[Y]*V[Y] + V[Z]*V[Z] + V[W]*V[W]); }

	//! return the norm1 (infinite, the greater absolute value)
	Float normInf() const { return max(max(V[X], V[Y]), max(V[Z], V[W])); }

	//! return the new normalized vector (this is constant)
	inline Vector4 operator!() const
	{
        Float len = Math::sqrt(V[X]*V[X] + V[Y]*V[Y] + V[Z]*V[Z] + V[W]*V[W]);

		if (len!=0) len = 1/len;
		return Vector4(V[X]*len,V[Y]*len,V[Z]*len,V[W]*len);
	}

	//! Normalize using norm2.
	inline void normalize()
	{
        Float len = Math::sqrt(V[X]*V[X] + V[Y]*V[Y] + V[Z]*V[Z] + V[W]*V[W]);

		if (len != 0.0f) len = 1.f/len;

		V[X] *= len;
		V[Y] *= len;
		V[Z] *= len;
		V[W] *= len;
	}

	//! Duplicate from another Vector4.
	inline Vector4& operator= (const Vector4 &vec)
	{
		V[X] = vec.V[X];
		V[Y] = vec.V[Y];
		V[Z] = vec.V[Z];
		V[W] = vec.V[W];
		return (*this);
	}

	//! Add another Vector4.
	inline Vector4& operator+= (const Vector4 &vec)
	{
		V[X] += vec.V[X];
		V[Y] += vec.V[Y];
		V[Z] += vec.V[Z];
		V[W] += vec.V[W];
		return (*this);
	}

	//! Subtract another Vector4.
	inline Vector4& operator-= (const Vector4 &vec)
	{
		V[X] -= vec.V[X];
		V[Y] -= vec.V[Y];
		V[Z] -= vec.V[Z];
		V[W] -= vec.V[W];
		return (*this);
	}
/*
	// produit vectoriel this^vec
	void operator^= (const O3DVector4& vec);
*/
	// Auto multiply each component by a scalar (this*=scale).
	inline Vector4& operator*= (const Float scale)
	{
		V[X] *= scale;
		V[Y] *= scale;
		V[Z] *= scale;
		V[W] *= scale;
		return (*this);
	}

	// Auto divide each component by a scalar (this/=scale).
	inline Vector4& operator/= (const Float scale)
	{
		V[X] /= scale;
		V[Y] /= scale;
		V[Z] /= scale;
		V[W] /= scale;
		return (*this);
	}

	//! Dot product (this*vec).
	inline Float operator* (const Vector4& v) const
	{
		return (V[X]*v.V[X] + V[Y]*v.V[Y] + V[Z]*v.V[Z] + V[W]*v.V[W]);
	}

	//! dot product (this*vec)
	inline Float dot(const Vector4& v) const
	{ return (V[X]*v.V[X] + V[Y]*v.V[Y] + V[Z]*v.V[Z] + V[W]*v.V[W]); }

	// Multiply each component by a scalar and return the result.
	inline Vector4 operator* (const Float scale)const
	{
		return Vector4(V[X]*scale,V[Y]*scale,V[Z]*scale,V[W]*scale);
	}

	// Divide each component by a scalar and return the result.
	inline Vector4 operator/ (const Float scale) const
	{
		return Vector4(V[X]/scale,V[Y]/scale,V[Z]/scale,V[W]/scale);
	}
/*
	// produit vectoriel v1 ^ v2
	O3DVector4 operator^ (const O3DVector4& v);
*/
	// Add 2 vectors (this+v) and return the result.
	inline Vector4 operator+ (const Vector4& v)const
	{
		return Vector4(V[X] + v.V[X],V[Y] + v.V[Y],V[Z] + v.V[Z],V[W] + v.V[W]);
	}

	// Subtract 2 vectors (this-v) and return the result.
	inline Vector4 operator- (const Vector4& v)const
	{
		return Vector4(V[X] - v.V[X],V[Y] - v.V[Y],V[Z] - v.V[Z],V[W] - v.V[W]);
	}

	//! Compare the length of two vector and return TRUE if this is lesser than v.
	inline Bool operator< (const Vector4& v)const
	{
		if (length() < v.length()) return True; else return False;
	}

	//! Compare the length of two vector and return TRUE if this is greater than v.
	inline Bool operator> (const Vector4& v)const
	{
		if (length() > v.length()) return True; else return False;
	}

	//! Compare the length of two vector and return TRUE if this is lesser or equal than v.
	inline Bool operator<= (const Vector4& v)const
	{
		if (length() <= v.length()) return True; else return False;
	}

	//! Compare the length of two vector and return TRUE if this is greater or equal than v.
	inline Bool operator>= (const Vector4& v)const
	{
		if (length() >= v.length()) return True; else return False;
	}

	//! Compare two vector for a difference.
	inline Bool operator!= (const Vector4& v)const
	{
        if (o3d::abs(V[X] - v[X]) > Limits<Float>::epsilon()) return True;
        if (o3d::abs(V[Y] - v[Y]) > Limits<Float>::epsilon()) return True;
        if (o3d::abs(V[Z] - v[Z]) > Limits<Float>::epsilon()) return True;
        if (o3d::abs(V[W] - v[W]) > Limits<Float>::epsilon()) return True;
		return False;
	}

	//! Compare two vector for an equality.
	inline Bool operator== (const Vector4& v)const
	{
        if (o3d::abs(V[X] - v[X]) > Limits<Float>::epsilon()) return False;
        if (o3d::abs(V[Y] - v[Y]) > Limits<Float>::epsilon()) return False;
        if (o3d::abs(V[Z] - v[Z]) > Limits<Float>::epsilon()) return False;
        if (o3d::abs(V[W] - v[W]) > Limits<Float>::epsilon()) return False;
		return True;
	}

	//! Negate the vector and return the result.
	inline Vector4 operator- ()const	{ return Vector4(-V[X], -V[Y], -V[Z], -V[W]); }

	//! Set all vector component to zero.
	inline void zero() { V[X] = V[Y] = V[Z] = V[W] = 0.f; }

	//! convert to a string (useful for debugging) => { x, y, z, w }
	operator String() const;

	// Serialization
    Bool writeToFile(OutStream& os) const;
    Bool readFromFile(InStream& is);

protected:

	Float V[4];		//!< Vector components
};

} // namespace o3d

#endif // _O3D_VECTOR4
