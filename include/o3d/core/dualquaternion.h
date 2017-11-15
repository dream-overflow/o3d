/**
 * @file dualquaternion.h
 * @brief The Dual of a Quaternion can model the movement of an object in 3d
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-12-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DUALQUATERNION_H
#define _O3D_DUALQUATERNION_H

#include "debug.h"
#include "templatearray.h"
#include "math.h"
#include "quaternion.h"
#include "memorydbg.h"

// SSE2 Optimisation in O3D_USE_SIMD mode
#ifdef O3D_USE_SIMD
    #include <xmmintrin.h>
#endif // O3D_USE_SIMD

namespace o3d {

class InStream;
class OutStream;

//---------------------------------------------------------------------------------------
//! @class DualQuaternion
//-------------------------------------------------------------------------------------
//! The Dual of a Quaternion can model the movement of an object in 3d,
//! which can rotate and translate without changing shape, that is distances and
//! angles between points on the object are preserved.
//---------------------------------------------------------------------------------------
class O3D_API DualQuaternion
{
private:

	#ifdef O3D_USE_SIMD
		union {
			__m128 *__M128;
			Float *Q;
		};
	#else
		Float *Q;          //!< Q Contain 8 float, 4 first for Q and 4 next for epsilonQ
	#endif

	//! The eights components of the dual quaternion
	enum Component
	{
		I = 0,
		J = 1,
		K = 2,
		R = 3,
		EI = 4,
		EJ = 5,
		EK = 6,
		E = 7
	};

	//! Initialization constructor from an external float*
	//! @note _Q must by 16 bytes aligned
	inline DualQuaternion(Float *_Q) { Q = _Q; }

	//! Get translation from unit dual quaternion
	inline Vector3 getTranslationFromUnit() const
	{
		O3D_ASSERT(isUnit());
		// @TODO MQuaternion qeq0 = mulQuat(mDual, mReal.conjugate());
		//_V.set(2.f*qeq0.x, 2.f*qeq0.y, 2.f*qeq0.z);
		return Vector3();
	}

	//! Transform a 3d vector from unit dual quaternion
	void tranformFromUnit(Vector3 &V) const;

public:

	//! AtomicArray initialization
	inline void _InitAtomicObject(void *ptr)
	{
		Q = (Float*)ptr;
		identity();
	}

	//-----------------------------------------------------------------------------------
	// DualQuaternion INITIALIZATION
	//-----------------------------------------------------------------------------------

	//! default constructor
	inline DualQuaternion()
	{
		Q = (Float*)O3D_FAST_ALLOC(32);
		identity();
	}

	//! Simple constructor which dont identity the qu
    inline DualQuaternion(Int32 v) {}

	//! initialisation constructor
	inline DualQuaternion(
		Float i,Float j,Float k,Float r,
		Float ei,Float ej,Float ek,Float e)
	{
		Q = (Float*)O3D_FAST_ALLOC(32);
		Q[I] = i; Q[J] = j; Q[K] = k; Q[R] = r;
		Q[EI] = ei; Q[EJ] = ej; Q[EK] = ek; Q[E] = e;
	}

	//! duplicate
	inline DualQuaternion(const DualQuaternion& _Q)
	{
		Q = (Float*)O3D_FAST_ALLOC(32);
	#ifdef O3D_USE_SIMD
		__M128[0] = _mm_load_ps(&_Q.Q[0]);
		__M128[1] = _mm_load_ps(&_Q.Q[4]);
	#else
		memcpy(Q,_Q.Q,sizeof(Float)*8);
	#endif
	}

	//! construct from a rotation quaternion
	inline DualQuaternion(const Quaternion &_Q)
	{
		Q = (Float*)O3D_FAST_ALLOC(32);
	#ifdef O3D_USE_SIMD
		__M128[0] = _mm_load_ps(&_Q.V[0]);
		__M128[1] = _mm_setzero_ps();
	#else
		Q[I] = _Q[I]; Q[J] = _Q[J]; Q[K] = _Q[K]; Q[R] = _Q[R];
		Q[EI] = Q[EJ] = Q[EK] = 0.f; Q[E] = 0.f;
	#endif
	}

	//! construct from a 4x4 matrix
	inline DualQuaternion(const Matrix4 &M)
	{
		Q = (Float*)O3D_FAST_ALLOC(32);
		fromMatrix4(M);
	}

	//! construct from a 3d translation vector and a 4d rotation quaternion
	inline DualQuaternion(const Vector3 &_V, const Quaternion &_Q)
	{
		Q = (Float*)O3D_FAST_ALLOC(32);
		fromAxisAndQuaternion(_V,_Q);
	}

	//! destructor
	~DualQuaternion() { O3D_FAST_FREE(Q,32); }

	//! set all entry to zero
	inline void zero()
	{
	#ifdef O3D_USE_SIMD
		__M128[0] = _mm_setzero_ps();
		__M128[1] = _mm_setzero_ps();
	#else
		memset(Q,0,sizeof(Float)*8);
	#endif
	}

	//! Q is set to identity
	inline void identity()
	{
#ifdef O3D_USE_SIMD
		__M128[0] = _mm_set_ps(0.f,0.f,0.f,1.f);
		__M128[1] = _mm_setzero_ps();
#else
		Q[I] = Q[J] = Q[K] = 0.f; Q[R] = 1.f;
		Q[EI] = Q[EJ] = Q[EK] = 0.f; Q[E] = 0.f;
#endif
	}

	//! Is Q is set to identity
    inline Bool isIdentity() const
	{
		return ((Q[I] == 0.f) && (Q[J] == 0.f) && (Q[K] == 0.f) && (Q[R] == 1.f) &&
				(Q[EI] == 0.f) && (Q[EJ] == 0.f) && (Q[EK] == 0.f) && (Q[E] == 0.f));
	}

	//! define the dual quaternion entries
	inline void set(Float i,Float j,Float k,Float r,
					Float ei,Float ej,Float ek,Float e)
	{
		Q[I] = i; Q[J] = j; Q[K] = k; Q[R] = r;
		Q[EI] = ei; Q[EJ] = ej; Q[EK] = ek; Q[E] = e;
	}

	//! duplicate
	inline DualQuaternion& operator=(const DualQuaternion& _Q)
	{
	#ifdef O3D_USE_SIMD
		__M128[0] = _mm_load_ps(&_Q.Q[0]);
		__M128[1] = _mm_load_ps(&_Q.Q[4]);
	#else
		memcpy(Q,_Q.Q,sizeof(Float)*8);
	#endif
		return *this;
	}

	//! construct from a rotation quaternion
	inline DualQuaternion& operator=(const Quaternion& _Q)
	{
	#ifdef O3D_USE_SIMD
		__M128[0] = _mm_load_ps(&_Q.V[0]);
		__M128[1] = _mm_setzero_ps();
	#else
		Q[I] = _Q[X]; Q[J] = _Q[Y]; Q[K] = _Q[Z]; Q[R] = _Q[W];
		Q[EI] = Q[EJ] = Q[EK] = 0.f; Q[E] = 0.f;
	#endif
		return *this;
	}

	//! construct from a 4x4 matrix
	inline DualQuaternion& operator=(const Matrix4& _M)
	{
		fromMatrix4(_M);
		return *this;
	}

	//-----------------------------------------------------------------------------------
	// DualQuaternion ACCESSORS
	//-----------------------------------------------------------------------------------

	//! return the data content ptr
	inline       Float* getData()       { return Q; }
	//! return the data content ptr (const version)
	inline const Float* getData() const { return Q; }

	//! get an entry
	inline       Float& operator[](UInt32 i)      { return Q[i]; }
	//! get an entry (const version)
	inline const Float& operator[](UInt32 i)const { return Q[i]; }

	//! get the rotation of the dual quaternion
	inline Quaternion getRotation() const { return Quaternion(Q[I],Q[J],Q[K],Q[R]); }

	//! get the rotation of the dual quaternion
	inline void getRotation(Quaternion &_Q) const { _Q.set(Q[I],Q[J],Q[K],Q[R]); }

	//! get the translation of the dual quaternion
	inline Vector3 getTranslation() const
	{
		Vector3 V;
		getTranslation(V);
		return V;
	}

	//! get the translation of the dual quaternion
	inline void getTranslation(Vector3 &_V) const
	{
		if (isUnit())
		{
			_V = getTranslationFromUnit();
		}
		else
		{
			Float scale	= -2.f / squareLength();

			_V.set((Q[E]*Q[I] - Q[R]*Q[EI] + Q[EJ]*Q[K] - Q[J]*Q[EK]) * scale,
				   (Q[E]*Q[J] - Q[EI]*Q[K] + Q[I]*Q[EK] - Q[R]*Q[EJ]) * scale,
				   (Q[E]*Q[K] - Q[I]*Q[EJ] - Q[R]*Q[EK] + Q[EI]*Q[J]) * scale);
		}
	}

	//! Test for Plucker condition
    inline Bool checkPlucker() const
	{
		// Test for Plucker condition. Dot between real and dual part must be 0
		// @todo return fabs(dotQuat(mReal,mDual))<1e-5;
		return True;
	}

	//! Check if the dual quaternion is unit length
    inline Bool isUnit() const
	{
		// Real must be unit and plucker condition must hold
		// @todo return (fabs(dotQuat(mReal,mReal)-1.0)<1e-5) && checkPlucker();
		return True;
	}

	//! Check if the dual quaternion has a rotation
    inline Bool hasRotation() const
	{
		O3D_ASSERT(isUnit());
		return fabs(Q[R]) < 0.999999f;
	}

	//! Check if the dual quaternion is a pure translation
    inline Bool isPureTranslation() const { return !hasRotation(); }

	//-----------------------------------------------------------------------------------
	// DualQuaternion COMPARISONS
	//-----------------------------------------------------------------------------------

	//! is two dual quaternions are equal
    inline Bool operator== (const DualQuaternion& _Q) const
	{
		return (memcmp(_Q.Q,Q,sizeof(Float) * 8) == 0);
		// @todo in SSE2
	}

	//! is two dual quaternions are different
    inline Bool operator!= (const DualQuaternion& _Q) const
	{
		return (memcmp(_Q.Q,Q,sizeof(Float) * 8) != 0);
		// @todo in SSE2
	}

	//! is two dual quaternions are equal (near to a given tolerance)
    inline Bool isEquivalent(const DualQuaternion& _Q, Float tolerance) const
	{
		// @todo return mReal.isEquivalent(inOther.mReal, inTolerance) && mDual.isEquivalent(inOther.mDual, inTolerance);
		// @todo in SSE2
		return True;
	}

	//-----------------------------------------------------------------------------------
	// DualQuaternion DOT Products
	//-----------------------------------------------------------------------------------

	//! compute the dot product of two dual quaternion DOT = Q dot _Q
	inline Float dot(const DualQuaternion& _Q) const
	{
	#ifdef O3D_USE_SIMD
		O3D_ALIGN(16) Float DOT[4];
		__m128 mm0, mm1, mm2;
		mm0 = _mm_mul_ps(__M128[0],_Q.__M128[0]);
		mm1 = _mm_mul_ps(__M128[1],_Q.__M128[1]);
		mm2 = _mm_add_ps(mm0,mm1);
		_mm_store_ps(DOT,mm2);
		return (DOT[0] + DOT[1] + DOT[2] + DOT[3]);
	#else
		return (Q[I]*_Q.Q[I] + Q[J]*_Q.Q[J] + Q[K]*_Q.Q[K] + Q[R]*_Q.Q[R]) +
			   (Q[EI]*_Q.Q[EI] + Q[EJ]*_Q.Q[EJ] + Q[EK]*_Q.Q[EK] + Q[E]*_Q.Q[E]);
	#endif
	}

	//! compute the dot product of dual quaternion real part DOT = Q.real dot _Q.real
	inline Float dotReal(const Quaternion& _Q) const
	{
	#ifdef O3D_USE_SIMD
		O3D_ALIGN(16) Float DOT[4];
		__m128 mm0 = _mm_mul_ps(__M128[0],_Q.__M128[0]);
		_mm_store_ps(DOT,mm0);
		return (DOT[0] + DOT[1] + DOT[2] + DOT[3]);
	#else
		return (Q[I]*_Q.V[X] + Q[J]*_Q.V[Y] + Q[K]*_Q.V[Z] + Q[R]*_Q.V[W]);
	#endif
	}

	//! compute the dot product of dual quaternion dual (epsilon) part DOT = Q.dual dot _Q.dual
	inline Float dotDual(const Quaternion& _Q) const
	{
	#ifdef O3D_USE_SIMD
		O3D_ALIGN(16) Float DOT[4];
		__m128 mm0 = _mm_mul_ps(__M128[1],_Q.__M128[0]);
		_mm_store_ps(DOT,mm0);
		return (DOT[0] + DOT[1] + DOT[2] + DOT[3]);
	#else
		return (Q[EI]*_Q.V[X] + Q[EJ]*_Q.V[Y] + Q[EK]*_Q.V[Z] + Q[E]*_Q.V[W]);
	#endif
	}

	//-----------------------------------------------------------------------------------
	// DualQuaternion ADDITION
	//-----------------------------------------------------------------------------------

	//! add two dual quaternion this + _Q and return into __Q
	inline void add(const DualQuaternion& _Q, DualQuaternion& __Q) const
	{
	#ifdef O3D_USE_SIMD
		__Q.__M128[0] = _mm_add_ps(__M128[0], _Q.__M128[0]);
		__Q.__M128[1] = _mm_add_ps(__M128[1], _Q.__M128[1]);
	#else
		__Q[I] = Q[I] + _Q[I]; __Q[J] = Q[J] + _Q[J]; __Q[K] = Q[K] + _Q[K]; __Q[R] = Q[R] + _Q[R];
		__Q[EI] = Q[EI] + _Q[EI]; __Q[EJ] = Q[EJ] + _Q[EJ]; __Q[EK] = Q[EK] + _Q[EK]; __Q[E] = Q[E] + _Q[E];
	#endif
	}

	//! add two dual quaternion return = this + _Q
	inline DualQuaternion operator+ (const DualQuaternion& _Q) const
	{
		DualQuaternion __Q((Float*)O3D_FAST_ALLOC(32));
		add(_Q,__Q);
		return __Q;
	}

	//! add two dual quaternion this = this + _Q
	inline DualQuaternion& operator+= (const DualQuaternion& _Q)
	{
		add(_Q,*this);
		return *this;
	}

	//-----------------------------------------------------------------------------------
	// DualQuaternion SUBSTRACT
	//-----------------------------------------------------------------------------------

	//! sub two dual quaternion this + _Q and return into __Q
	inline void sub(const DualQuaternion& _Q, DualQuaternion& __Q) const
	{
	#ifdef O3D_USE_SIMD
		__Q.__M128[0] = _mm_sub_ps(__M128[0], _Q.__M128[0]);
		__Q.__M128[1] = _mm_sub_ps(__M128[1], _Q.__M128[1]);
	#else
		__Q[I] = Q[I] - _Q[I]; __Q[J] = Q[J] - _Q[J]; __Q[K] = Q[K] - _Q[K]; __Q[R] = Q[R] - _Q[R];
		__Q[EI] = Q[EI] - _Q[EI]; __Q[EJ] = Q[EJ] - _Q[EJ]; __Q[EK] = Q[EK] - _Q[EK]; __Q[E] = Q[E] - _Q[E];
	#endif
	}

	//! sub two dual quaternion return = this + _Q (lesser efficient than Sub)
	inline DualQuaternion operator- (const DualQuaternion& _Q) const
	{
		DualQuaternion __Q((Float*)O3D_FAST_ALLOC(32));
		sub(_Q,__Q);
		return  __Q;
	}

	//! sub two dual quaternion this = this + _Q
	inline DualQuaternion& operator-= (const DualQuaternion& _Q)
	{
		sub(_Q,*this);
		return *this;
	}

	//-----------------------------------------------------------------------------------
	// DualQuaternion PRODUCT
	//-----------------------------------------------------------------------------------

	//! product R = this * _Q (non commutative) q*p
	inline void mult(const DualQuaternion& _Q, DualQuaternion& __Q) const
	{
		// @todo
	#ifdef O3D_USE_SIMD
		O3D_ALIGN(16) Float TMP[4];
		Quaternion::mult(&_Q.Q[0],&Q[0],&__Q.Q[0]);
		Quaternion::mult(&_Q.Q[4],&Q[0],TMP);
		Quaternion::mult(&_Q.Q[0],&Q[4],&__Q.Q[4]);
		__m128 mm0 = _mm_load_ps(TMP);
		__Q.__M128[1] = _mm_add_ps(__Q.__M128[1],mm0);
	#else
		O3D_ALIGN(16) Float TMP[4];
		Quaternion::mult(&_Q.Q[0],&Q[0],&__Q.Q[0]);
		Quaternion::mult(&_Q.Q[4],&Q[0],TMP);
		Quaternion::mult(&_Q.Q[0],&Q[4],&__Q.Q[4]);
		__Q[EI] += TMP[0];
		__Q[EJ] += TMP[1];
		__Q[EK] += TMP[2];
		__Q[E]  += TMP[3];
	#endif
	}

	//! product return = this * vec (non commutative) q*p
	inline DualQuaternion operator* (const DualQuaternion& _Q) const
	{ 
		DualQuaternion __Q((Float*)O3D_FAST_ALLOC(32));
		mult(_Q,__Q);
		return __Q;
	}

	//! product this *= vec (non commutative) q*p 
	inline DualQuaternion& operator*= (const DualQuaternion& _Q)
	{ 
		mult(_Q,*this);
		return *this; 
	}

	//-----------------------------------------------------------------------------------
	// DualQuaternion Normalize
	//-----------------------------------------------------------------------------------

    //! normalize the dual quaternion
	inline void normalize() { *this = rotationNormalized().pluckerNormalized(); }

	//! normalize and return the new dual quaternion
	inline DualQuaternion normalizeTo() const { return rotationNormalized().pluckerNormalized(); }

	//! rotation normalize and return the new dual quaternion
	inline void rotationNormalized(DualQuaternion &_Q) const
	{
		Float flength = 1.f / length();
	#ifdef O3D_USE_SIMD
		__m128 __scal = _mm_set_ps1(flength);        // scale
		_Q.__M128[0] = _mm_mul_ps(__M128[0],__scal); // real
		_Q.__M128[1] = _mm_mul_ps(__M128[1],__scal); // dual
	#else
		// real
		_Q.Q[I] = Q[I]*flength;
		_Q.Q[J] = Q[J]*flength;
		_Q.Q[K] = Q[K]*flength;
		_Q.Q[R] = Q[R]*flength;

		// dual
		_Q.Q[EI] = Q[EI]*flength;
		_Q.Q[EJ] = Q[EJ]*flength;
		_Q.Q[EK] = Q[EK]*flength;
		_Q.Q[E]  = Q[E]*flength;
	#endif
	}

	//! rotation normalize and return the new dual quaternion
	inline DualQuaternion rotationNormalized() const
	{
		DualQuaternion __Q((Float*)O3D_FAST_ALLOC(32));
		rotationNormalized(__Q);
		return __Q;
	}

	//! Plucker normalize and return the new dual quaternion
	inline void pluckerNormalized(DualQuaternion &_Q) const
	{
		Float flength = 1.f / squareLength();
	#ifdef O3D_USE_SIMD
		// real (_Q.real = this->real)
		_mm_store_ps(&_Q.Q[0],__M128[0]);

		// dual
		__m128 mm0,mm1;
		Float DOT;
		O3D_ALIGN(16) Float DOT4[4];
		mm0 = _mm_mul_ps(__M128[0],__M128[1]);
		_mm_store_ps(DOT4,mm0);
		DOT = flength * (DOT4[0] + DOT4[1] + DOT4[2] + DOT4[3]);
		mm0 = _mm_set_ps1(DOT);
		mm1 = _mm_mul_ps(__M128[0],mm0);
		mm0 = _mm_sub_ps(__M128[1],mm1);
		_mm_store_ps(&_Q.Q[4],mm0);
	#else
		//real
		_Q[I] = Q[I];
		_Q[J] = Q[J];
		_Q[K] = Q[K];
		_Q[R] = Q[R];

		// dual
		Float DOT = flength * (Q[I]*Q[EI] + Q[J]*Q[EJ] + Q[K]*Q[EK] + Q[R]*Q[E]);
		_Q[EI] = Q[EI] - (Q[I] * DOT);
		_Q[EJ] = Q[EJ] - (Q[J] * DOT);
		_Q[EK] = Q[EK] - (Q[K] * DOT);
		_Q[E]  = Q[E]  - (Q[R] * DOT);
	#endif	
	}

	//! Plucker normalize and return the new dual quaternion
	inline DualQuaternion pluckerNormalized() const
	{
		DualQuaternion __Q((Float*)O3D_FAST_ALLOC(32));
		pluckerNormalized(__Q);
		return __Q;
	}

	//-----------------------------------------------------------------------------------
	// DualQuaternion Length
	//-----------------------------------------------------------------------------------

	//! return the length of the dual quaternion
	inline Float length() const
	{
	#ifdef O3D_USE_SIMD
		O3D_ALIGN(16) Float DOT[4];
		__m128 mm0 = _mm_mul_ps(__M128[0],__M128[0]);
		_mm_store_ps(DOT,mm0);
		return Math::sqrt(DOT[0] + DOT[1] + DOT[2] + DOT[3]);
	#else
		return Math::sqrt(Q[I]*Q[I] + Q[J]*Q[J] + Q[K]*Q[K] + Q[R]*Q[R]);
	#endif
	}

	//! return the square length of the dual quaternion
	inline Float squareLength() const
	{
	#ifdef O3D_USE_SIMD
		O3D_ALIGN(16) Float DOT[4];
		__m128 mm0 = _mm_mul_ps(__M128[0],__M128[0]);
		_mm_store_ps(DOT,mm0);
		return (DOT[0] + DOT[1] + DOT[2] + DOT[3]);
	#else
		return (Q[I]*Q[I] + Q[J]*Q[J] + Q[K]*Q[K] + Q[R]*Q[R]);
	#endif
	}

	//-----------------------------------------------------------------------------------
	// DualQuaternion Misc
	//-----------------------------------------------------------------------------------

	//! Log
	void log(DualQuaternion &_Q) const;

	//! Return Log
	DualQuaternion log() const
	{
		DualQuaternion __Q((Float*)O3D_FAST_ALLOC(32));
		log(__Q);
		return __Q;
	}

	//! Exp
	void exp(DualQuaternion &_Q) const;

	//! Return Exp
	DualQuaternion exp() const
	{
		DualQuaternion __Q((Float*)O3D_FAST_ALLOC(32));
		exp(__Q);
		return __Q;
	}

	//! Set from screw
	void setFromScrew(Float Angle, Float Pitch, const Vector3 &Dir, const Vector3 &Moment);

	//! Extract to screw
	void toScrew(Float &Angle, Float &Pitch, Vector3 &Dir, Vector3 &Moment) const;

	//-----------------------------------------------------------------------------------
	// DualQuaternion Conjugate
	//-----------------------------------------------------------------------------------

	//! conjugate the dual quaternion
	inline void conjugate()
	{
		Q[I] = -Q[I]; Q[J] = -Q[J]; Q[K] = -Q[K];
		Q[EI] = -Q[EI]; Q[EJ] = -Q[EJ]; Q[EK] = -Q[EK];
	}

	//! return the conjugate of the dual quaternion 
	inline DualQuaternion conjugateTo() const
	{
		DualQuaternion __Q(*this);
		__Q.conjugate();
		return __Q;
	}

	//-----------------------------------------------------------------------------------
	// DualQuaternion Invert
	//-----------------------------------------------------------------------------------

	//! invert the dual quaternion
	void invert();

	//! return the invert of the dual quaternion
	inline DualQuaternion InvertTo() const
	{
		DualQuaternion __Q(*this);
		__Q.invert();
		return __Q;
	}

	//-----------------------------------------------------------------------------------
	// Dual Quaternion SCALAR OPERATIONS
	//-----------------------------------------------------------------------------------

	//! negative any entry of the dual quaternion
	inline void negative()
	{
		Q[I] = -Q[I]; Q[J] = -Q[J]; Q[K] = -Q[K]; Q[R] = -Q[R];
		Q[EI] = -Q[EI]; Q[EJ] = -Q[EJ]; Q[EK] = -Q[EK]; Q[E] = -Q[E];
	}

	//! negative any entry of the dual quaternion and return this dual quaternion
	inline DualQuaternion operator- () const
	{
		DualQuaternion _Q(*this);
		_Q.negative();
		return _Q;
	}

	//! Scale any entry of the dual quaternion by scale
	inline void scale(const Float scale)
	{
	#ifdef O3D_USE_SIMD
		__m128 __scal = _mm_set_ps1(scale);
		__M128[0] = _mm_mul_ps(__M128[0],__scal);
		__M128[1] = _mm_mul_ps(__M128[1],__scal);
	#else
		Q[I] *= scale; Q[J] *= scale; Q[K] *= scale; Q[R] *= scale;
		Q[EI] *= scale; Q[EJ] *= scale; Q[EK] *= scale; Q[E] *= scale;
	#endif
	}

	//! Scale any entry of the dual quaternion by scale
	//! @note For a Scale -1 prefere the use of Negative
	inline DualQuaternion& operator*= (const Float scalar)
	{
		scale(scalar);
		return (*this);
	}

	//! Scale any entry of the dual quaternion by scaleand return the new one.
	//! @note For a Scale -1 prefere the use of Negative
	inline DualQuaternion operator* (const Float scalar) const
	{
		DualQuaternion _M(*this);
		_M.scale(scalar);
		return _M;
	}

	//-----------------------------------------------------------------------------------
	// DualQuaternion CONVERTION FROM x
	//-----------------------------------------------------------------------------------

	//! set from a 4x4 matrix
	void fromMatrix4(const Matrix4& M);

	//! set from a 3d axis and a rotation quaternion
	inline void fromAxisAndQuaternion(const Vector3 &_V, const Quaternion &_Q)
	{
	#ifdef O3D_USE_SIMD
		__M128[0] = _mm_load_ps(&_Q[0]);
		__M128[1] = _mm_set_ps(0.5f*_V[X],0.5f*_V[Y],0.5f*_V[Z],0.f);
	#else
		Q[I] = _Q[I]; Q[J] = _Q[J]; Q[K] = _Q[K]; Q[R] = _Q[R];
		Q[EI] = _V[X]*0.5f; Q[EJ] = _V[Y]*0.5f; Q[EK] = _V[Z]*0.5f; Q[E] = 0.f;
	#endif

		Quaternion::mult(&Q[4],&Q[0],&Q[4]);
	}

	//-----------------------------------------------------------------------------------
	// DualQuaternion CONVERTION TO x
	//-----------------------------------------------------------------------------------

	//! convert to a 4x4 matrix
	inline Matrix4 ToMatrix4() const
	{
		Matrix4 M;

		// Fill in rotation part
		M = getRotation().toMatrix4();

		// Fill in translation part
		M.setTranslation(getTranslation());

		return M;
	}

	//! convert to a 4x4 matrix
	inline void toMatrix4(Matrix4& M) const
	{
		// Fill in rotation part
		M = getRotation().toMatrix4();

		// Fill in translation part
		M.setTranslation(getTranslation());
	}

	//! Convert to matrix from unit dual quaternion
	inline void toMatrixFromUnit(Matrix4 &M) const
	{
		O3D_ASSERT(isUnit());

		// Fill in rotation part
		M = getRotation().toMatrix4();

		// Fill in translation part
		M.setTranslation(getTranslationFromUnit());
	}

	//-----------------------------------------------------------------------------------
	// DualQuaternion TRANSFORMS
	//-----------------------------------------------------------------------------------

	//! transform a 3d vector by the dual quaternion (very not optimal method)
	inline Vector3 transform(const Vector3 &V) { return ToMatrix4() * V; }

	//-----------------------------------------------------------------------------------
	// DualQuaternion INTERPOLATION
	//-----------------------------------------------------------------------------------

	//! linear interpolation from this to 'to' at time t and return the result
	inline DualQuaternion screwLerp(DualQuaternion& to, Float t) const;

	//! linear interpolation from this to 'to' at time t and store the result in R
	inline void screwLerp(DualQuaternion& to, Float t, DualQuaternion& R) const;

	//! dual quaternion linear interpolation. Result is normalized.
	static void DLB(
			UInt32 NbElt,
			const Double *Weights,
			const DualQuaternion *Entries);

	//! dual quaternion iterative intrinsic interpolation up to given precision. Result is normalized.
	static void DIB(
			UInt32 NbElt,
			const Double *Weights,
			const DualQuaternion *Entries,
			Float precision);

	//-----------------------------------------------------------------------------------
	// DualQuaternion SERIALIZATION
	//-----------------------------------------------------------------------------------

	//! convert to a String
	operator String() const;

	//! Serialization
    Bool writeToFile(OutStream& os) const;
    Bool readFromFile(InStream& is);
};

} // namespace o3d

#endif // _O3D_DUALQUATERNION_H

