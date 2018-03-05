/**
 * @file quaternion.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-07-13
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/quaternion.h"

#include "o3d/core/dualquaternion.h"
#include "o3d/core/math.h"
#include "o3d/core/instream.h"

#include <math.h>

using namespace o3d;

// cast to a string
Quaternion::operator String()const
{
	String temp;
	temp << String("{ ")
		 << V[X] << String(", ")
		 << V[Y] << String(", ")
		 << V[Z] << String(", ")
		 << V[W] << String(" }");

	return temp;
}

// convertie un matrice 3x3 en quaternion
Quaternion& Quaternion::fromMatrix3(const Matrix3& mat)
{
	Float t = mat(0,0) + mat(1,1) + mat(2,2) + 1.0f; //trace de la matrice

    if (t > 0) {
        Float s = 0.5f / Math::sqrt(t);
		V[X] = s * (mat(2,1) - mat(1,2));
		V[Y] = s * (mat(0,2) - mat(2,0));
		V[Z] = s * (mat(1,0) - mat(0,1));
		V[W] = 0.25f / s;
    } else {
		int i = 0;	//l'indice de l'element le plus grand dans la diagonale

		//identifie l'element le plus grand dans la diagonale
		if (mat(0,0) < mat(1,1)) i++;
		if (mat(i,i) < mat(2,2)) i = 2;

		//calcul du quaternion
        if (i == 0) {
            Float s = 1.0f / (Math::sqrt(1.0f + mat(0,0) - mat(1,1) - mat(2,2)) * 2.f);
			V[X] = 0.25f / s;
			V[Y] = (mat(0,1) + mat(1,0)) * s;
			V[Z] = (mat(0,2) + mat(2,0)) * s;
			V[W] = (mat(1,2) + mat(2,1)) * s;
        } else if (i == 1) {
            Float s = 1.0f / (Math::sqrt(1.0f + mat(1,1) - mat(0,0) - mat(2,2)) * 2.f);
			V[Y] = 0.25f / s;
			V[X] = (mat(0,1) + mat(1,0)) * s;
			V[W] = (mat(0,2) + mat(2,0)) * s;
			V[Z] = (mat(1,2) + mat(2,1)) * s;
        } else {
            Float s = 1.0f / (Math::sqrt(1.0f + mat(2,2) - mat(1,1) - mat(0,0)) * 2.f);
			V[Z] = 0.25f / s;
			V[W] = (mat(0,1) + mat(1,0)) * s;
			V[X] = (mat(0,2) + mat(2,0)) * s;
			V[Y] = (mat(1,2) + mat(2,1)) * s;
		}
	}

	return *this;
}

//convertie un matrice 4x4 en quaternion (la meme chose que 3x3)
Quaternion& Quaternion::fromMatrix4(const Matrix4& mat)
{
	Float t = mat(0,0) + mat(1,1) + mat(2,2) + 1.0f; //trace de la matrice

    if (t > 0) {
        Float s = 0.5f / Math::sqrt(t);
		V[X] = s * (mat(2,1) - mat(1,2));
		V[Y] = s * (mat(0,2) - mat(2,0));
		V[Z] = s * (mat(1,0) - mat(0,1));
		V[W] = 0.25f / s;
    } else {
		int i = 0;	//l'indice de l'element le plus grand dans la diagonale

		//identifie l'element le plus grand dans la diagonale
		if (mat(0,0) < mat(1,1)) i++;
		if (mat(i,i) < mat(2,2)) i = 2;

		//calcul du quaternion
        if (i == 0) {
            Float s = 1.0f / (Math::sqrt(1.0f + mat(0,0) - mat(1,1) - mat(2,2)) * 2.f);
			V[X] = 0.25f / s;
			V[Y] = (mat(0,1) + mat(1,0)) * s;
			V[Z] = (mat(0,2) + mat(2,0)) * s;
			V[W] = (mat(1,2) + mat(2,1)) * s;
        } else if (i == 1) {
            Float s = 1.0f / (Math::sqrt(1.0f + mat(1,1) - mat(0,0) - mat(2,2)) * 2.f);
			V[Y] = 0.25f / s;
			V[X] = (mat(0,1) + mat(1,0)) * s;
			V[W] = (mat(0,2) + mat(2,0)) * s;
			V[Z] = (mat(1,2) + mat(2,1)) * s;
        } else {
            Float s = 1.0f / (Math::sqrt(1.0f + mat(2,2) - mat(1,1) - mat(0,0)) * 2.f);
			V[Z] = 0.25f / s;
			V[W] = (mat(0,1) + mat(1,0)) * s;
			V[X] = (mat(0,2) + mat(2,0)) * s;
			V[Y] = (mat(1,2) + mat(2,1)) * s;
		}
	}

	return *this;
}

//convertie une axe de rotation 3D et un angle en quaternion
Quaternion& Quaternion::fromAxisAngle3(const Vector3& vec,Float angle)
{
	Float sin_a = sinf(angle * 0.5f);

	V[X] = vec[X] * sin_a;
	V[Y] = vec[Y] * sin_a;
	V[Z] = vec[Z] * sin_a;
	V[W] = cosf(angle * 0.5f);
	normalize();

	return *this;
}

//convertie une axe de rotation 4D et un angle en quaternion
Quaternion& Quaternion::fromAxisAngle4(const Vector4& vec,Float angle)
{
	Float sin_a = sinf(angle * 0.5f);

	V[X] = vec[X] * sin_a;
	V[Y] = vec[Y] * sin_a;
	V[Z] = vec[Z] * sin_a;
	V[W] = cosf(angle * 0.5f);
	normalize();

	return *this;
}

//convertie des angles de rotations spheriques en quaternion
Quaternion& Quaternion::fromSpherique(Float angle,Float latitude,Float longitude)
{
	Float sin_a = sinf(angle * 0.5f);

	V[X] = sin_a * cosf(latitude) * sinf(longitude);
	V[Y] = sin_a * sinf(latitude);
	V[Z] = sin_a * sinf(latitude) * cosf(longitude);
	V[W] = cosf(angle * 0.5f);

	return *this;
}

//convertie des angles d'euler en quaternion
Quaternion& Quaternion::fromEuler(Float x,Float y,Float z)
{
	// calculate trig identities
    Float cr = cosf(x/*roll*/*0.5f);
    Float cp = cosf(y/*pitch*/*0.5f);
    Float cy = cosf(z/*yaw*/*0.5f);

    Float sr = sinf(x/*roll*/*0.5f);
    Float sp = sinf(y/*pitch*/*0.5f);
    Float sy = sinf(z/*yaw*/*0.5f);

    Float cpcy = cp * cy;
    Float spsy = sp * sy;

	V[X] = sr * cpcy - cr * spsy;
	V[Y] = cr * sp * cy + sr * cp * sy;
	V[Z] = cr * cp * sy - sr * sp * cy;
	V[W] = cr * cpcy + sr * spsy;

	return *this;
}

//convertie un quaternion en matrice 3x3

/*

        |       2     2                                |
        | 1 - 2Y  - 2Z    2XY - 2ZW      2XZ + 2YW     |
        |                                              |
        |                       2     2                |
    M = | 2XY + 2ZW       1 - 2X  - 2Z   2YZ - 2XW     |
        |                                              |
        |                                      2     2 |
        | 2XZ - 2YW       2YZ + 2XW      1 - 2X  - 2Y  |
        |                                              |

o� X, Y, Z, W sont les composantes du quaternion

*/
Matrix3 Quaternion::toMatrix3() const
{
	Float X2 = V[X] + V[X]; Float Y2 = V[Y] + V[Y]; Float Z2 = V[Z] + V[Z];
	Float XX = V[X] * X2;  Float XY = V[X] * Y2; Float XZ = V[X] * Z2;
	Float YY = V[Y] * Y2;  Float YZ = V[Y] * Z2; Float ZZ = V[Z] * Z2;
	Float WX = V[W] * X2;  Float WY = V[W] * Y2; Float WZ = V[W] * Z2;

	return Matrix3(1.f - (YY + ZZ),         XY - WZ,         XZ + WY,
			         XY + WZ, 1.f - (XX + ZZ),         YZ - WX,
		   	         XZ - WY,         YZ + WX, 1.f - (XX + YY));
}

// convert to a 4x4 matrix
Matrix4 Quaternion::toMatrix4() const
{
	Float X2 = V[X] + V[X]; Float Y2 = V[Y] + V[Y]; Float Z2 = V[Z] + V[Z];
	Float XX = V[X] * X2;  Float XY = V[X] * Y2; Float XZ = V[X] * Z2;
	Float YY = V[Y] * Y2;  Float YZ = V[Y] * Z2; Float ZZ = V[Z] * Z2;
	Float WX = V[W] * X2;  Float WY = V[W] * Y2; Float WZ = V[W] * Z2;

	return Matrix4(1.f - (YY + ZZ),         XY - WZ,         XZ + WY, 0.f,
			          XY + WZ, 1.f - (XX + ZZ),         YZ - WX, 0.f,
			          XZ - WY,         YZ + WX, 1.f - (XX + YY), 0.f,
			              0.f,             0.f,             0.f, 1.f);
}

// convert to a 4x4 matrix
void Quaternion::toMatrix4(Matrix4& M) const
{
	Float X2 = V[X] + V[X]; Float Y2 = V[Y] + V[Y]; Float Z2 = V[Z] + V[Z];
	Float XX = V[X] * X2;  Float XY = V[X] * Y2; Float XZ = V[X] * Z2;
	Float YY = V[Y] * Y2;  Float YZ = V[Y] * Z2; Float ZZ = V[Z] * Z2;
	Float WX = V[W] * X2;  Float WY = V[W] * Y2; Float WZ = V[W] * Z2;

	return M.setData(1.f - (YY + ZZ),         XY - WZ,         XZ + WY, 0.f,
                                 XY + WZ, 1.f - (XX + ZZ),         YZ - WX, 0.f,
			         XZ - WY,         YZ + WX, 1.f - (XX + YY), 0.f,
			             0.f,             0.f,             0.f, 1.f);
}

//convertie un quaternion en axe de rotation 3D est un angle
//(retourne un vecteur 4D dont la derniere composante est l'angle)
Vector4 Quaternion::toAxisAngle() const
{
	Vector4 vec;
	Quaternion q(*this);
    Float s = Math::sqrt(1.0f - V[W]*V[W]);

	q.normalize();
	vec[W] = acosf(q[W]);

    if (fabsf(s) < 0.0005f/*o3d::Limits<Float>::Epsilon*/) {
		s = 1.0f;
    } else {
		s = 1.0f / s;
    }

	vec[X] = q[X] * s;
	vec[Y] = q[Y] * s;
	vec[Z] = q[Z] * s;

	return vec;
}

//affecte les variables envoyees
void Quaternion::toAxisAngle(Float* x,Float* y,Float* z,Float* angle) const
{
	Vector4 vec = toAxisAngle();

	*x = vec[X]; *y = vec[Y]; *z = vec[Z]; *angle = vec[W];
}

void Quaternion::toAxisAngle(Vector3* vec,Float* angle) const
{
	Vector4 v = toAxisAngle();

	vec->set(v[X],v[Y],v[Z]); *angle = v[W];
}

void Quaternion::toAxisAngle(Vector4* vec,Float* angle) const
{
	(Vector4)(*vec) = toAxisAngle();
	((Vector4)(*vec))[W] = 1.0f;
	*angle = V[W];
}

//convertie un quaternion en angle de rotation spherique
//(retourne un vecteur 3D : X = angle, Y = latitude, Z = longitude)
Vector3 Quaternion::toSpherique() const
{
	Vector4 v;
	Vector3 vec;

	v = toAxisAngle(); // v[W] is the angle

    // latitude
    vec[Y] = (Float)(-asinf(v[Y]));

    // longitude
    if ((v[X] * v[Y] * v[Z]) < 0.0005f) {
        vec[Z] = 0.0f;
    } else {
        vec[Z] = (Float)atan2f(v[X], v[Z]);
    }

    if (vec[Z] < 0.0f) {
        vec[Z] += 2 * o3d::PI;
    }

	return vec;
}

//(affecte les variables envoyees)
void Quaternion::toSpherique(Float* angle,Float* latitude,Float* longitude) const
{
	Vector3 vec = toSpherique();

	*angle = vec[0]; *latitude = vec[0]; *longitude = vec[0];
}

// convert to euler rotation angles
void Quaternion::toEuler(Float &x, Float &y, Float &z) const
{
    // @todo could check for singularity
	x = atan2(2*(V[W]*V[X] + V[Y]*V[Z]), 1-2*(V[X]*V[X] + V[Y]*V[Y]));
	y = asin(2*(V[W]*V[Y] - V[Z]*V[X]));
	z = atan2(2*(V[W]*V[Z]+V[X]*V[Y]), 1-2*(V[Y]*V[Y]+V[Z]*V[Z]));
}

// convert to euler rotation angles
void Quaternion::toEuler(Vector3 &r) const
{
    // @todo could check for singularity
    r[X] = atan2(2*(V[W]*V[X] + V[Y]*V[Z]), 1-2*(V[X]*V[X] + V[Y]*V[Y]));
    r[Y] = asin(2*(V[W]*V[Y] - V[Z]*V[X]));
    r[Z] = atan2(2*(V[W]*V[Z]+V[X]*V[Y]), 1-2*(V[Y]*V[Y]+V[Z]*V[Z]));

//    // roll (x-axis rotation)
//    r[X] = atan2(2*(V[W]*V[X] + V[Y]*V[Z]), 1-2*(V[X]*V[X] + V[Y]*V[Y]));  // sinr, cosr

//    // pitch (y-axis rotation)
//    Double sinp = 2*(V[W]*V[Y] - V[Z]*V[X]);
//    if (fabs(sinp) >= 1) {
//        r[Y] = copysign(o3d::HALP_PI, sinp); // use 90 degrees if out of range
//    } else {
//        r[Y] = asin(sinp);
//    }

//    // yaw (z-axis rotation)
//    r[Z] = atan2(2*(V[W]*V[Z] + V[X]*V[Y]), 1-2*(V[Y]*V[Y] + V[Z]*V[Z]);  // siny, cosy
}

// spherical linear interpolation of this to 'to' at time t and put the result int output
void Quaternion::slerp(const Quaternion &to, Float t, Quaternion &R) const
{
    Float omega, cosom, sinom, sclp, sclq;

    // @todo SSE2 optimization
	cosom = V[X]*to[X] + V[Y]*to[Y] + V[Z]*to[Z] + V[W]*to[W];

	// adjust signs (if necessary)
    if (cosom < 0.f) {
		cosom = -cosom;
		R[X] = -to[X];
		R[Y] = -to[Y];
		R[Z] = -to[Z];
		R[W] = -to[W];
    } else {
		R[X] = to[X];
		R[Y] = to[Y];
		R[Z] = to[Z];
		R[W] = to[W];
	}

	// calculate coeficients
    if ((1.0f-cosom) > o3d::Limits<Float>::epsilon()) {
		omega = acosf(cosom);
		sinom = sinf(omega);
		sclp = sinf((1.0f-t)*omega) / sinom;
		sclq = sinf(t*omega) / sinom;
    } else {
        // quaternions are very close so we can do a linear interpolation
		sclp = 1.0f - t;
		sclq = t;
	}

	// calculate final values (TODO SSE2 optimize)
	R[X] = sclp*V[X] + sclq*to[X];
	R[Y] = sclp*V[Y] + sclq*to[Y];
	R[Z] = sclp*V[Z] + sclq*to[Z];
	R[W] = sclp*V[W] + sclq*to[W];
}

// interpolation spherique de t entre this et to
Quaternion Quaternion::slerp(const Quaternion &to, Float t) const
{
	Quaternion R;
	slerp(to, t, R);
	return R;
}

//---------------------------------------------------------------------------------------
// Serialization
//---------------------------------------------------------------------------------------
Bool Quaternion::writeToFile(OutStream &os) const
{
    os  << V[0]
		<< V[1]
		<< V[2]
		<< V[3];

	return True;
}

Bool Quaternion::readFromFile(InStream &is)
{
    is  >> V[0]
		>> V[1]
		>> V[2]
		>> V[3];

	return True;
}
