/**
 * @file quaternion.h
 * @brief Quaternion (hyper-complex number) with 3x3 and 4x4 matrix conversion
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @author  Romain LOCCI
 * @date 2003-07-13
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_QUATERNION_H
#define _O3D_QUATERNION_H

#include "debug.h"
#include "vector4.h"
#include "matrix4.h"
#include "math.h"
#include "memorydbg.h"

// SSE2 Optimisation in O3D_USE_SIMD mode
#ifdef O3D_USE_SIMD
    #include <xmmintrin.h>
#endif // O3D_USE_SIMD

namespace o3d {

class Math;
class DualQuaternion;

//---------------------------------------------------------------------------------------
//! @class Quaternion
//-------------------------------------------------------------------------------------
//! Quaternion (hyper-complex number) with 3x3 and 4x4 matrix conversion.
//!
//! Les Quaternions sont des vecteurs 4 dimensions : X, Y, Z, W.
//! Leur representation est la suivantes :
//!   q = w + xi + yj + zk ou w est la partie reelle et xi + yj + zk la partie imaginaire
//! i, j, k obeissent aux lois internes suivantes :
//!   i² = j² = k² = ijk = -1
//!  ij = -ji = k
//!  jk = -kj = i
//!  ki = -ik = j
//!
//! (x,y,z) represente l'axe de rotation, et w l'angle.
//!  
//! @note contrairement a l'ensemble C( les nombres imaginaires ), les Quaternions
//! n'ont pas une multiplication commutative donc attention au sens.
//---------------------------------------------------------------------------------------
class O3D_API Quaternion : Atomic
{
	friend class DualQuaternion;

private:

#ifdef O3D_USE_SIMD
	union {
		__m128 *__M128;
		Float *V;
	};
#else
	Float *V;          //!< V Contain 4 float
#endif

	//! initialisation constructor from an external float*
	//! @note _V must be 16 bytes aligned
	inline Quaternion(Float *_V) { V = _V; }

public:

	//! AtomicArray initialization
	inline void _InitAtomicObject(void *ptr)
	{
		V = (Float*)ptr;
		identity();
	}


	//___________________________________________________________________________________
	// Quaternion INITIALIZATION
	//___________________________________________________________________________________

	//! default constructor
	inline Quaternion()
	{
		V = (Float*)O3D_FAST_ALLOC(16);
		identity();
	}

	//! Simple constructor which dont identity the Quaternion
    inline Quaternion(Int32 v) {}

	//! initialisation constructor
	inline Quaternion(Float x,Float y,Float z,Float w)
	{
		V = (Float*)O3D_FAST_ALLOC(16);
		V[X] = x; V[Y] = y; V[Z] = z; V[W] = w;
	}

	//! duplicate
	inline Quaternion(const Quaternion& vec)
	{
		V = (Float*)O3D_FAST_ALLOC(16);
		V[X] = vec.V[X]; V[Y] = vec.V[Y]; V[Z] = vec.V[Z]; V[W] = vec.V[W];
	}

	//! convert from a 3x3 matrix
	inline Quaternion(const Matrix3& mat)
	{
		V = (Float*)O3D_FAST_ALLOC(16);
		fromMatrix3(mat);
	}

	//! convert from a 4x4 matrix
	inline Quaternion(const Matrix4& mat)
	{
		V = (Float*)O3D_FAST_ALLOC(16);
		fromMatrix4(mat);
	}

	//! destructor
	~Quaternion() { O3D_FAST_FREE(V,16); }

	//! duplicate
	inline Quaternion& operator=(const Quaternion& vec)
	{
		V[X] = vec[X]; V[Y] = vec[Y]; V[Z] = vec[Z]; V[W] = vec[W];
		return *this;
	}

	//! define any entry of the quaternino
	inline void set(Float x,Float y,Float z,Float w)
	{
		V[X] = x; V[Y] = y; V[Z] = z; V[W] = w;
	}

	//! set to identity Quaternion
	inline void identity()
	{
		V[X] = V[Y] = V[Z] = 0.f; V[W] = 1.f;
	}

	//! Is the Quaternion equal to identity.
    inline Bool isIdentity() const
	{
		return ((V[X] == 0.f) && (V[Y] == 0.f) && (V[Z] == 0.f) && (V[W] == 1.f));
	}

	//! Is the Quaternion null.
    inline Bool isNull() const
	{
		return ((V[X] == 0.f) && (V[Y] == 0.f) && (V[Z] == 0.f) && (V[W] == 0.f));
	}

	//___________________________________________________________________________________
	// Quaternion ACCESSORS
	//___________________________________________________________________________________

	//! lit le contenue du Quaternion
	inline void read(Float& x,Float& y,Float& z,Float& w) const
	{
		x = V[X]; y = V[Y]; z = V[Z]; w = V[W];
	}

	//! get the data content ptr
	inline       Float* getData()       { return V; }
	//! get the data content ptr (const version)
	inline const Float* getData() const { return V; }

	//! get an entry of the Quaternion
	inline       Float& operator[](UInt32 i)       { return V[i]; }
	//! get an entry of the Quaternion (const version)
	inline const Float& operator[](UInt32 i) const { return V[i]; }


	//___________________________________________________________________________________
	// Quaternion OPERATORS
	//___________________________________________________________________________________

	//! operateur d'addition
	inline Quaternion operator+(const Quaternion& vec) const
	{
		return Quaternion(V[X]+vec[X],V[Y]+vec[Y],V[Z]+vec[Z],V[W]+vec[W]);
	}

	//! operateur d'adition/incrementation
	inline Quaternion& operator+=(const Quaternion& vec)
	{
		V[X]+=vec[X]; V[Y]+=vec[Y]; V[Z]+=vec[Z]; V[W]+=vec[W];
		return (*this);
	}

	//! operateur de soustraction
	inline Quaternion operator-(const Quaternion& vec) const
	{
		return Quaternion(V[X]-vec[X],V[Y]-vec[Y],V[Z]-vec[Z],V[W]-vec[W]);
	}

	//! operateur de soustraction/incrementation
	inline Quaternion& operator-=(const Quaternion& vec)
	{
		V[X]-=vec[X]; V[Y]-=vec[Y]; V[Z]-=vec[Z]; V[W]-=vec[W];
		return (*this);
	}

	//! operateur de multiplication par un reel
	inline Quaternion operator*(Float a) const
	{
		return Quaternion(V[X]*a,V[Y]*a,V[Z]*a,V[W]*a);
	}

	//! operateur de multiplication/incrementation par un reel
	inline Quaternion& operator*=(Float a)
	{
		V[X]*=a; V[Y]*=a; V[Z]*=a; V[W]*=a;
		return (*this);
	}


	//___________________________________________________________________________________
	// Quaternion PRODUCT
	//___________________________________________________________________________________

	//! product R = V * U
	inline static void mult(const Float* V, const Float* U, Float* R)
	{
		Float A = (V[W] + V[X])*(U[W] + U[X]);
		Float B = (V[Z] - V[Y])*(U[Y] - U[Z]);
		Float C = (V[W] - V[X])*(U[Y] + U[Z]); 
		Float D = (V[Y] + V[Z])*(U[W] - U[X]);
		Float E = (V[X] + V[Z])*(U[X] + U[Y]);
		Float F = (V[X] - V[Z])*(U[X] - U[Y]);
		Float G = (V[W] + V[Y])*(U[W] - U[Z]);
		Float H = (V[W] - V[Y])*(U[W] + U[Z]);

		R[0] = A - (E + F + G + H) * 0.5f;
		R[1] = C + (E - F + G - H) * 0.5f;
		R[2] = D + (E - F - G + H) * 0.5f;
		R[3] = B + (-E - F + G + H) * 0.5f;
	}

	//! product __Q = Q * _Q (non commutative) q*p
	inline void mult(const Quaternion& _Q, Quaternion& __Q) const
	{
		Float A = (V[W] + V[X])*(_Q.V[W] + _Q.V[X]);
		Float B = (V[Z] - V[Y])*(_Q.V[Y] - _Q.V[Z]);
		Float C = (V[W] - V[X])*(_Q.V[Y] + _Q.V[Z]); 
		Float D = (V[Y] + V[Z])*(_Q.V[W] - _Q.V[X]);
		Float E = (V[X] + V[Z])*(_Q.V[X] + _Q.V[Y]);
		Float F = (V[X] - V[Z])*(_Q.V[X] - _Q.V[Y]);
		Float G = (V[W] + V[Y])*(_Q.V[W] - _Q.V[Z]);
		Float H = (V[W] - V[Y])*(_Q.V[W] + _Q.V[Z]);

		__Q.set(A - (E + F + G + H) * 0.5f,
			    C + (E - F + G - H) * 0.5f,
			    D + (E - F - G + H) * 0.5f,
			    B + (-E - F + G + H) * 0.5f);
	}

	//! product return = this * vec (non commutative) q*p
	inline Quaternion operator* (const Quaternion& _Q) const
	{ 
		Quaternion __Q((Float*)O3D_FAST_ALLOC(16));
		mult(_Q,__Q);
		return __Q;
		//return O3DQuaternion(V[W] * vec[X] + vec[W] * V[X] + V[Y] * vec[Z] - V[Z] * vec[Y],
		//		 V[W] * vec[Y] + vec[W] * V[Y] + V[Z] * vec[X] - V[X] * vec[Z],
		//		 V[W] * vec[Z] + vec[W] * V[Z] + V[X] * vec[Y] - V[Y] * vec[X],
		//		 V[W] * vec[W] - (V[X] * vec[X] + V[Y] * vec[Y] + V[Z] * vec[Z]));
	}

	//! product this *= vec (non commutative) q*p 
	inline Quaternion& operator*= (const Quaternion& _Q)
	{ 
		mult(_Q,*this);
		return (*this); 
	}


	//___________________________________________________________________________________
	// Quaternion DOT PRODUCT
	//___________________________________________________________________________________

	inline Float dot(const Quaternion& _Q) const
	{

		#ifdef O3D_USE_SIMD
			O3D_ALIGN(16) Float DOT[4];
			__m128 mm0 = _mm_mul_ps(__M128[0],_Q.__M128[0]);
			_mm_store_ps(DOT,mm0);
			return (DOT[0] + DOT[1] + DOT[2] + DOT[3]);
		#else
			return (V[X]*_Q.V[X] + V[Y]*_Q.V[Y] + V[Z]*_Q.V[Z] + V[W]*_Q.V[W]);
		#endif
	}


	//___________________________________________________________________________________
	// Quaternion NORMALIZE, LENGTH, CONJUGATE and INVERT
	//___________________________________________________________________________________

	//! normalize
	inline Quaternion& normalize()
	{
		Float s = Math::sqrt(V[X]*V[X] + V[Y]*V[Y] + V[Z]*V[Z] + V[W]*V[W]);
		if (s == 0.0f)
		{
			V[X] = V[Y] = V[Z] = 0.0f, V[W] = 1.0f;
		}
		else
		{
			s = 1.f / s;
			V[X] *= s; V[Y] *= s; V[Z] *= s; V[W] *= s;
		}

		return *this;
	}

	//! return the normalized Quaternion
	inline Quaternion getNormalized() const
	{
		Float s = Math::sqrt(V[X]*V[X] + V[Y]*V[Y] + V[Z]*V[Z] + V[W]*V[W]);
		if (s == 0.0f)
		{
			return Quaternion();
		}
		else
		{
			s = 1.f / s;
			return Quaternion(-V[X]*s,-V[Y]*s,-V[Z]*s,V[W]*s);
		}
	}

	//! return the length of the Quaternion
	inline Float lenght() const
	{
		return Math::sqrt(V[X]*V[X] + V[Y]*V[Y] + V[Z]*V[Z] + V[W]*V[W]);
	}

	//! return the square length of the Quaternion
	inline Float squareLenght() const
	{
		return (Float)(V[X]*V[X] + V[Y]*V[Y] + V[Z]*V[Z] + V[W]*V[W]);
	}

	//! conjugate the Quaternion and return it
	inline void conjugate() { V[X] = -V[X]; V[Y] = -V[Y]; V[Z] = -V[Z]; }

	//! return the conjugate of the Quaternion
	inline Quaternion conjugateTo() const { return Quaternion(-V[X],-V[Y],-V[Z],V[W]); }

	//! invert the Quaternion Q = (1/Q)
	inline void invert()
	{
		Float s = V[X]*V[X] + V[Y]*V[Y] + V[Z]*V[Z] + V[W]*V[W];
		conjugate();
		if (s == 0.f)
		{
			V[X] = V[Y] = V[Z] = 0.0f; V[W] = 1.0f;
		}
		else
		{
			s = 1.f/s;

			V[X] *= s;
			V[Y] *= s;
			V[Z] *= s;
			V[W] *= s;
		}
	}

	//! return the invert of the Quaternion Q' = (1/Q)
	inline Quaternion invertTo() const
	{
		Float s = V[X]*V[X] + V[Y]*V[Y] + V[Z]*V[Z] + V[W]*V[W];
		if (s == 0.f)
		{
			return Quaternion();
		}
		else
		{
			Quaternion __Q(conjugateTo());
			s = 1.f/s;
	
			__Q.V[X] *= s;
			__Q.V[Y] *= s;
			__Q.V[Z] *= s;
			__Q.V[W] *= s;

			return __Q;
		}
	}


	//___________________________________________________________________________________
	// Quaternion TRANSFORM
	//___________________________________________________________________________________

	//! Transform a vector by a Quaternion
	inline void transform(Vector3 &v) const
	{
		Quaternion T(v[X],v[Y],v[Z],0.f);
		mult(T,T); T.mult(invertTo(),T);
		v.set(T[X],T[Y],T[Z]);
	}

	//! Transform a vector by a Quaternion
	inline Vector3 transformTo(const Vector3 &v) const
	{
		Quaternion T(v[X],v[Y],v[Z],0.f);
		mult(T,T); T.mult(invertTo(),T);
		return Vector3(T[X],T[Y],T[Z]);
	}

	//___________________________________________________________________________________
	// Quaternion CONVERTION FROM x
	//___________________________________________________________________________________

	//! set from a 3x3 matrix
	Quaternion& fromMatrix3(const Matrix3& mat);
	//! set from a 4x4 matrix
	Quaternion& fromMatrix4(const Matrix4& mat);
	//! set from an 3d axis and angle
	Quaternion& fromAxisAngle3(const Vector3& vec,Float angle);
	//! set from an 4d axis and angle
	Quaternion& fromAxisAngle4(const Vector4& vec,Float angle);
	//! set from spherical rotations
	Quaternion& fromSpherique(Float angle,Float latitude,Float longitude);
	//! set from euler rotations
	Quaternion& fromEuler(Float x,Float y,Float z);
	//! set from euler rotations
	inline Quaternion& fromEuler(const Vector3 &r) { return fromEuler(r[X],r[Y],r[Z]); }


	//___________________________________________________________________________________
	// Quaternion CONVERTION TO x
	//___________________________________________________________________________________

	//! convert to a 3x3 matrix
	Matrix3 toMatrix3() const;

	//! convert to a 4x4 matrix
	Matrix4 toMatrix4() const;

	//! convert to a 4x4 matrix
	void toMatrix4(Matrix4& M) const;

	//! convert to a 4d axis rotation with rotation angle as W component
	Vector4 toAxisAngle() const;

	//! convert to an axis rotation (x,y,z) and angle
	void toAxisAngle(Float* x,Float* y,Float* z,Float* angle) const;
	//! convert to a 3d axis rotation and angle
	void toAxisAngle(Vector3* vec,Float* angle) const;
	//! convert to a 4d axis rotation and angle
	void toAxisAngle(Vector4* vec,Float* angle) const;

	//! convert to spherical rotation angles with X = angle, Y = latitude, Z = longitude
	Vector3 toSpherique() const;

	//! convert to spherical rotation angles
	void toSpherique(Float* angle,Float* latitude,Float* longitude) const;	

	//! convert to euler rotation angles
	void toEuler(Float &x, Float &y, Float &z) const;

	//! convert to euler rotation angles
	void toEuler(Vector3 &r) const;


	//___________________________________________________________________________________
	// Quaternion INTERPOLATION
	//___________________________________________________________________________________

	//! linear interpolation from this to 'to' at time t and return the result
	inline Quaternion lerp(const Quaternion& to, Float t) const
	{
		return Quaternion(V[X]*(1-t) + to[X]*t,
					         V[Y]*(1-t) + to[Y]*t,
					         V[Z]*(1-t) + to[Z]*t,
						     V[W]*(1-t) + to[W]*t).normalize();
	}
	//! linear interpolation from this to 'to' at time t and store the result in R
	inline void lerp(const Quaternion &to, Float t, Quaternion &R) const
	{
		R.set(V[X]*(1-t) + to[X]*t,
			  V[Y]*(1-t) + to[Y]*t,
			  V[Z]*(1-t) + to[Z]*t,
			  V[W]*(1-t) + to[W]*t);
		R.normalize();
	}

	//! spherical linear interpolation of this to 'to' at time t and return the result
	Quaternion slerp(const Quaternion &to, Float t) const;
	//! spherical linear interpolation of this to 'to' at time t and store the result in R
	void slerp(const Quaternion &to, Float t, Quaternion &R) const;


	//___________________________________________________________________________________
	// Quaternion SERIALIZATION
	//___________________________________________________________________________________

	//! convert to a string
	operator String()const;

	//! serialisation
    Bool writeToFile(OutStream& os) const;
    Bool readFromFile(InStream& is);
};

// serialize
inline OutStream& operator<< (OutStream &os, const Quaternion &object)
{
    object.writeToFile(os);
    return os;
}

// serialize
inline OutStream& operator<< (OutStream &os, Quaternion &object)
{
    object.writeToFile(os);
    return os;
}

// de-serialize
inline InStream& operator>> (InStream &is, Quaternion &object)
{
    object.readFromFile(is);
    return is;
}

} // namespace o3d

#endif // _O3D_QUATERNION_H

