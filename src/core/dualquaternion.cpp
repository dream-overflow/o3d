/**
 * @file dualquaternion.cpp
 * @brief Implementation of DualQuaternion.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-12-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/dualquaternion.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

#include <math.h>

using namespace o3d;

// invert the dual quaternion
void DualQuaternion::invert()
{
	Float sqrLen_0 = squareLength();
	//Float SqrLen_e = 2.f; // @todo * dot(Q[0], Q[4]);

	if (sqrLen_0 > 0.f)
	{
//		Float InvSqrLen_0 = 1.f / SqrLen_0;
//		Float invSqrLen_e = -SqrLen_e / (SqrLen_0*SqrLen_0);

		DualQuaternion conj = conjugateTo();
	//	Q[0] = InvSqrLen_0 * conj.Q[0];
	//	Q[4] = InvSqrLen_0 * conj.Q[4] + InvSqrLen_e * conj.Q[0];
	}
	else
	{
		zero();
	}
}

// Log
void DualQuaternion::log(DualQuaternion &_Q) const
{
	// @todo
	Float angle=0, pitch=0;
	Vector3 direction, moment;
	toScrew(angle, pitch, direction, moment);

	_Q.Q[I] = direction[X]*angle*0.5f;
	_Q.Q[J] = direction[Y]*angle*0.5f;
	_Q.Q[K] = direction[Z]*angle*0.5f;
	_Q.Q[R] = 0.0;

	_Q.Q[EI] = moment[X]*angle*0.5f + direction[X]*pitch*0.5f;
	_Q.Q[EJ] = moment[Y]*angle*0.5f + direction[Y]*pitch*0.5f;
	_Q.Q[EK] = moment[Z]*angle*0.5f + direction[Z]*pitch*0.5f;
	_Q.Q[R] = 0.0;
}

// Exp
void DualQuaternion::exp(DualQuaternion &_Q) const
{
	Vector3 n(Q[I], Q[J], Q[K]);

	Float half_angle = n.length();

	// Is Pure translation
	if (half_angle < o3d::Limits<Float>::epsilon())
	{
		_Q.Q[I] = 0.f;
		_Q.Q[J] = 0.f;
		_Q.Q[K] = 0.f;
		_Q.Q[R] = 1.f;

		_Q.Q[EI] = Q[EI];
		_Q.Q[EJ] = Q[EJ];
		_Q.Q[EK] = Q[EK];
		_Q.Q[E] = Q[E];
	}
	else
	{
		// Get normalized dir
		Vector3 dir = n * (1.f/half_angle);

		Vector3 d(Q[EI], Q[EJ], Q[EK]);
		Float half_pitch = d*dir;
		Vector3 mom = (d - dir*half_pitch) / half_angle;

		_Q.setFromScrew(half_angle*2.f, half_pitch*2.f, dir, mom);
	}
}

// set from a 4x4 matrix
void DualQuaternion::fromMatrix4(const Matrix4& M)
{
	// @todo
}

// Set from screw
void DualQuaternion::setFromScrew(
	Float Angle,
	Float Pitch,
	const Vector3 &Dir,
	const Vector3 &Moment)
{
	// @todo
	Float sin_half_angle = sinf(Angle*0.5f);
	Float cos_half_angle = cosf(Angle*0.5f);

	Q[I] = sin_half_angle*Dir[X];
	Q[J] = sin_half_angle*Dir[Y];
	Q[K] = sin_half_angle*Dir[Z];
	Q[R] = cos_half_angle;

	Q[EI] = sin_half_angle*Moment[X] + 0.5f*Pitch*cos_half_angle*Dir[X];
	Q[EJ] = sin_half_angle*Moment[Y] + 0.5f*Pitch*cos_half_angle*Dir[Y];
	Q[EK] = sin_half_angle*Moment[Z] + 0.5f*Pitch*cos_half_angle*Dir[Z];
	Q[E] = -Pitch*sin_half_angle*0.5f;
}

// Extract to screw
void DualQuaternion::toScrew(Float &Angle, Float &Pitch, Vector3 &Dir, Vector3 &Moment) const
{
	// @todo
}

// Transform a 3d vector from unit dual quaternion
void DualQuaternion::tranformFromUnit(Vector3 &V) const
{
	// @todo
}

// linear interpolation from this to 'to' at time t and return the restult
DualQuaternion DualQuaternion::screwLerp(DualQuaternion& to,Float t) const
{
	// @todo
	return DualQuaternion();
}

// linear interpolation from this to 'to' at time t and store the result in R
void DualQuaternion::screwLerp(DualQuaternion& to,Float t, DualQuaternion& R) const
{
	// @todo
}

// dual quaternion linear interpolation. Result is normalized.
void DualQuaternion::DLB(UInt32 NbElt, const Double *Weights, const DualQuaternion *Entries)
{
	// @todo
}

// dual quaternion iterative intrinsic interpolation up to given precision. Result is normalized.
void DualQuaternion::DIB(
	UInt32 NbElt,
	const Double *Weights,
	const DualQuaternion *Entries,
	Float precision)
{
	// @todo
}

/*---------------------------------------------------------------------------------------
  Serialisation
---------------------------------------------------------------------------------------*/
Bool DualQuaternion::writeToFile(OutStream &os) const
{
    os   << Q[I]
         << Q[J]
         << Q[K]
         << Q[R]
         << Q[EI]
         << Q[EJ]
         << Q[EK]
         << Q[E];

	return True;
}

Bool DualQuaternion::readFromFile(InStream &is)
{
    is   >> Q[I]
	     >> Q[J]
		 >> Q[K]
		 >> Q[R]
		 >> Q[EI]
	     >> Q[EJ]
		 >> Q[EK]
		 >> Q[E];

	return True;
}
