/**
 * @file vector3.h
 * @brief 3 dimensional vector.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-12-12
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_VECTOR3_H
#define _O3D_VECTOR3_H

#include <cmath>

#include "base.h"
#include "memorydbg.h"

namespace o3d {

class InStream;
class OutStream;
class String;

/**
 * @brief 3 dimensional vector
 */
class O3D_API Vector3
{
public:

	//! Default constructor. Set all component to zero.
	inline Vector3()
	{
		V[X] = V[Y] = V[Z] = 0.0f;
	}
	//! Copy constructor
	inline Vector3(const Vector3& vec)
	{
		V[X] = vec.V[X];
		V[Y] = vec.V[Y];
		V[Z] = vec.V[Z];
	}
	//! Initialize the vector given 3 scalars
	inline Vector3(const Float _x, const Float _y, const Float _z)
	{
		V[X] = _x;
		V[Y] = _y;
		V[Z] = _z;
	}
	//! Initialize from a float array
	inline Vector3(const Float* vec)
	{
		V[X] = vec[X];
		V[Y] = vec[Y];
		V[Z] = vec[Z];
	}

	//! Define the content of the vector given 3 scalars
	inline void set(
			const Float _x = 0.0f,
			const Float _y = 0.0f,
			const Float _z = 0.0f)
	{
		V[X] = _x;
		V[Y] = _y;
		V[Z] = _z;
	}
	//! Define the content of the vector given a float array
	inline void set(const Float* vec)
	{
		V[X] = vec[X];
		V[Y] = vec[Y];
		V[Z] = vec[Z];
	}

	//! Copy the vector to the float tab vec
	inline void copyTo(Float* vec) const
	{
		vec[X] = V[X];
		vec[Y] = V[Y];
		vec[Z] = V[Z];
	}

	//! read the content of the vector (x,y,z)
	inline void read(Float &_x, Float &_y, Float &_z) const
	{
		_x = V[X];
		_y = V[Y];
		_z = V[Z];
	}

	//! Get a ref component of the vector
	inline Float& operator[] (UInt32 i) { return V[i]; }

	//! Get a const ref component of the vector
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

	//! renvoie le vecteur sous forme de pointeur
	inline Float* getData() { return V; }
	inline const Float* getData() const { return V; }

	//! renvoie la longueur du vecteur (norme)
	inline Float length() const { return (Float)sqrtf(V[X]*V[X] + V[Y]*V[Y] + V[Z]*V[Z]); }

	//! renvoie la longueur du vecteur au carré (norme²)
	inline Float squareLength() const { return (V[X]*V[X] + V[Y]*V[Y] + V[Z]*V[Z]); }

	//! renvoie la norme infinie du vecteur (plus grande valeur absolue)
	Float normInf() const
	{
		return max(max(abs(V[X]), abs(V[Y])), abs(V[Z]));
	}

	//! return the new normalized vector (this is constant)
	inline Vector3 operator!() const
	{
		Float len = (Float) sqrtf(V[X] * V[X] + V[Y] * V[Y] + V[Z] * V[Z]);

        if (len != 0) {
			len = 1 / len;
        }

		return Vector3(V[X] * len, V[Y] * len, V[Z] * len);
	}

	//! Normalize the vector this.
	//! @return Its magnitude
	inline Float normalize()
	{
		Float len = (Float)sqrtf(V[X]*V[X] + V[Y]*V[Y] + V[Z]*V[Z]);

        if (len < Limits<Float>::epsilon()) {
			V[X] = 1;
			V[Y] = 0;
			V[Z] = 0;
        } else {
			len = 1.f/len;
			V[X] *= len;
			V[Y] *= len;
			V[Z] *= len;
		}
		return len;
	}

	//! copie le vecteur à this
	inline Vector3& operator=(const Vector3& vec)
	{
		V[X] = vec.V[X];
		V[Y] = vec.V[Y];
		V[Z] = vec.V[Z];
		return (*this);
	}

	//! ajoute un vecteur à this
	inline Vector3& operator+=(const Vector3& vec)
	{
		V[X] += vec.V[X];
		V[Y] += vec.V[Y];
		V[Z] += vec.V[Z];
		return (*this);
	}

	//! soustrait un vecteur à this
	inline Vector3& operator-=(const Vector3& vec)
	{
		V[X] -= vec.V[X];
		V[Y] -= vec.V[Y];
		V[Z] -= vec.V[Z];
		return (*this);
	}

	//! produit vectoriel this^vec
	inline Vector3& operator^= (const Vector3& vec)
	{
		Float vx, vy;

		vx   = V[Y] * vec.V[Z] - vec.V[Y] * V[Z];
		vy   = V[Z] * vec.V[X] - vec.V[Z] * V[X];
		V[Z] = V[X] * vec.V[Y] - vec.V[X] * V[Y];
		V[X] = vx;
		V[Y] = vy;

		return (*this);
	}

	//! scalairiser this*=scal
	inline Vector3& operator*=(const Float scal)
	{
		V[X] *= scal;
		V[Y] *= scal;
		V[Z] *= scal;
		return (*this);
	}

	//! division scalaire this/=scal
	inline Vector3& operator/=(const Float scal)
	{
		V[X] /= scal;
		V[Y] /= scal;
		V[Z] /= scal;
		return (*this);
	}

	//! dot product (this*vec)
	inline Float operator*(const Vector3& v) const
	{
		return (V[X] * v.V[X] + V[Y] * v.V[Y] + V[Z] * v.V[Z]);
	}

	//! dot product (this*vec)
	inline Float dot(const Vector3& v) const
	{
		return (V[X] * v.V[X] + V[Y] * v.V[Y] + V[Z] * v.V[Z]);
	}

	//! Scalar multiplication vec*scal
	inline Vector3 operator*(const Float scal) const
	{
		return Vector3(V[X] * scal, V[Y] * scal, V[Z] * scal);
	}

	//! Correct scalar multiplication scal*vec
	inline friend Vector3 operator *(const Float scal, const Vector3 & vec)
	{
		return Vector3(vec.V[X] * scal, vec.V[Y] * scal, vec.V[Z] * scal);
	}

	//! Scalar division vec/scal
	inline Vector3 operator/(const Float scal) const
	{
		return Vector3(V[X] / scal, V[Y] / scal, V[Z] / scal);
	}

	//! correct scalar division (1/scal)*vec
	inline friend Vector3 operator /(const Float scal, const Vector3 & vec)
	{
		return (1.0f / scal) * vec;
	}

	//! Cross product v1 ^ v2
	inline Vector3 operator^ (const Vector3& v) const
	{
		Vector3 vec;

		vec.V[X] = V[Y] * v.V[Z] - v.V[Y] * V[Z];
		vec.V[Y] = V[Z] * v.V[X] - v.V[Z] * V[X];
		vec.V[Z] = V[X] * v.V[Y] - v.V[X] * V[Y];

		return vec;
	}

	//! addition vectorielle v1 + v2
	inline Vector3 operator+(const Vector3& v) const
	{
		return Vector3(V[X] + v.V[X], V[Y] + v.V[Y], V[Z] + v.V[Z]);
	}

	//! soustraction vectorielle v1 - v2
	inline Vector3 operator-(const Vector3& v) const
	{
		return Vector3(V[X] - v.V[X], V[Y] - v.V[Y], V[Z] - v.V[Z]);
	}

	//! compare la longueur de 2 vecteurs v1 < v2 et renvoi True si vrai
	inline Bool operator< (const Vector3& v) const
	{
		if (length() < v.length()) return True; else return False;
	}

	//! compare la longueur de 2 vecteurs v1 > v2 et renvoi True si vrai
	inline Bool operator> (const Vector3& v) const
	{
		if (length() > v.length()) return True; else return False;
	}

	//! compare la longueur de 2 vecteurs v1 < v2 et renvoi True si vrai
	inline Bool operator<= (const Vector3& v) const
	{
		if (length() <= v.length()) return True; else return False;
	}

	//! compare la longueur de 2 vecteurs v1 > v2 et renvoi True si vrai
	inline Bool operator>= (const Vector3& v) const
	{
		if (length() >= v.length()) return True; else return False;
	}

	//! Compare two vectors
	//! @return False if they are equals
	inline Bool operator!= (const Vector3& v) const
	{
		if (fabs(V[X] - v[X]) > Limits<Float>::epsilon()) return True;
		if (fabs(V[Y] - v[Y]) > Limits<Float>::epsilon()) return True;
		if (fabs(V[Z] - v[Z]) > Limits<Float>::epsilon()) return True;
		return False;
	}

	//! Compare two vectors
	//! @return True if they are equals
	inline Bool operator== (const Vector3& v) const
	{
		if (fabs(V[X] - v[X]) > Limits<Float>::epsilon()) return False;
		if (fabs(V[Y] - v[Y]) > Limits<Float>::epsilon()) return False;
		if (fabs(V[Z] - v[Z]) > Limits<Float>::epsilon()) return False;
		return True;
	}

	//! Negative all component of the vector
	inline Vector3 operator- () const { return Vector3(-V[X],-V[Y],-V[Z]); }

	//! Set all components to zero.
	inline void zero() { V[X] = V[Y] = V[Z] = 0.f; }

	//! Set all vector x/y/z as minimum of a/b x/y/z  (respectively).
	inline void minOf(const Vector3 &a, const Vector3 &b)
	{
		V[0] = min(a.V[0], b.V[0]);
		V[1] = min(a.V[1], b.V[1]);
		V[2] = min(a.V[2], b.V[2]);
	}

	//! Set all vector x/y/z as maximum of a/b x/y/z  (respectively).
	inline void maxOf(const Vector3 &a, const Vector3 &b)
	{
		V[0] = max(a.V[0], b.V[0]);
		V[1] = max(a.V[1], b.V[1]);
		V[2] = max(a.V[2], b.V[2]);
	}

	//! A cross-product matrix corresponding to the vector
	class Matrix3 crossProductMatrix() const;

	//! convert to a string (useful for debugging) => { x, y, z }
	operator String() const;

	// Serialization
    Bool writeToFile(OutStream& os) const;
    Bool readFromFile(InStream& is);

protected:

	Float V[3];		//!< vector data
};

typedef Vector3    Point3f;
typedef Vector3    Vector3f;

} // namespace o3d

#endif // _O3D_VECTOR3_H
