/**
 * @file matrix4.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include "o3d/core/quaternion.h"
#include "o3d/core/matrix4.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

#include <math.h>

using namespace o3d;

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

Matrix4* Matrix4::ms_identity = nullptr;
Matrix4* Matrix4::ms_null = nullptr;

void Matrix4::init()
{
    if (!ms_identity)
	{
        ms_identity = new Matrix4();
        ms_null = new Matrix4();
        ms_null->zero();
	}
}

void Matrix4::quit()
{
    deletePtr(ms_identity);
    deletePtr(ms_null);
}

// Auto Transpose the matrix.
void Matrix4::transpose()
{
#ifdef O3D_SSE2
	_MM_TRANSPOSE4_PS(__M128[0],__M128[1],__M128[2],__M128[3]);
#else
	Float tmp;

	tmp = M12; M12 = M21; M21 = tmp;
	tmp = M13; M13 = M31; M31 = tmp;
	tmp = M14; M14 = M41; M41 = tmp;
	tmp = M23; M23 = M32; M32 = tmp;
	tmp = M24; M24 = M42; M42 = tmp;
	tmp = M34; M34 = M43; M43 = tmp;
#endif
}

// Transpose the matrix and return the result.
Matrix4 Matrix4::transposeTo()const
{
	Matrix4 __M(*this);
	__M.transpose();
	return __M;
}

// Invert the Matrix4 and return the result.
Matrix4 Matrix4::invertStd() const
{
	Matrix4 result;

	result.M11 = M23 * M34 * M42 - M24 * M33 * M42 + M24 * M32 * M43 - M22 * M34 * M43
			- M23 * M32 * M44 + M22 * M33 * M44;
	result.M12 = M14 * M33 * M42 - M13 * M34 * M42 - M14 * M32 * M43 + M12 * M34 * M43
			+ M13 * M32 * M44 - M12 * M33 * M44;
	result.M13 = M13 * M24 * M42 - M14 * M23 * M42 + M14 * M22 * M43 - M12 * M24 * M43
			- M13 * M22 * M44 + M12 * M23 * M44;
	result.M14 = M14 * M23 * M32 - M13 * M24 * M32 - M14 * M22 * M33 + M12 * M24 * M33
			+ M13 * M22 * M34 - M12 * M23 * M34;
	result.M21 = M24 * M33 * M41 - M23 * M34 * M41 - M24 * M31 * M43 + M21 * M34 * M43
			+ M23 * M31 * M44 - M21 * M33 * M44;
	result.M22 = M13 * M34 * M41 - M14 * M33 * M41 + M14 * M31 * M43 - M11 * M34 * M43
			- M13 * M31 * M44 + M11 * M33 * M44;
	result.M23 = M14 * M23 * M41 - M13 * M24 * M41 - M14 * M21 * M43 + M11 * M24 * M43
			+ M13 * M21 * M44 - M11 * M23 * M44;
	result.M24 = M13 * M24 * M31 - M14 * M23 * M31 + M14 * M21 * M33 - M11 * M24 * M33
			- M13 * M21 * M34 + M11 * M23 * M34;
	result.M31 = M22 * M34 * M41 - M24 * M32 * M41 + M24 * M31 * M42 - M21 * M34 * M42
			- M22 * M31 * M44 + M21 * M32 * M44;
	result.M32 = M14 * M32 * M41 - M12 * M34 * M41 - M14 * M31 * M42 + M11 * M34 * M42
			+ M12 * M31 * M44 - M11 * M32 * M44;
	result.M33 = M12 * M24 * M41 - M14 * M22 * M41 + M14 * M21 * M42 - M11 * M24 * M42
			- M12 * M21 * M44 + M11 * M22 * M44;
	result.M34 = M14 * M22 * M31 - M12 * M24 * M31 - M14 * M21 * M32 + M11 * M24 * M32
			+ M12 * M21 * M34 - M11 * M22 * M34;
	result.M41 = M23 * M32 * M41 - M22 * M33 * M41 - M23 * M31 * M42 + M21 * M33 * M42
			+ M22 * M31 * M43 - M21 * M32 * M43;
	result.M42 = M12 * M33 * M41 - M13 * M32 * M41 + M13 * M31 * M42 - M11 * M33 * M42
			- M12 * M31 * M43 + M11 * M32 * M43;
	result.M43 = M13 * M22 * M41 - M12 * M23 * M41 - M13 * M21 * M42 + M11 * M23 * M42
			+ M12 * M21 * M43 - M11 * M22 * M43;
	result.M44 = M12 * M23 * M31 - M13 * M22 * M31 + M13 * M21 * M32 - M11 * M23 * M32
			- M12 * M21 * M33 + M11 * M22 * M33;

	Float det = M14 * M23 * M32 * M41 - M13 * M24 * M32 * M41 - M14 * M22 * M33 * M41
			+ M12 * M24 * M33 * M41 + M13 * M22 * M34 * M41 - M12 * M23 * M34 * M41 - M14
			* M23 * M31 * M42 + M13 * M24 * M31 * M42 + M14 * M21 * M33 * M42 - M11 * M24
			* M33 * M42 - M13 * M21 * M34 * M42 + M11 * M23 * M34 * M42 + M14 * M22 * M31
			* M43 - M12 * M24 * M31 * M43 - M14 * M21 * M32 * M43 + M11 * M24 * M32 * M43
			+ M12 * M21 * M34 * M43 - M11 * M22 * M34 * M43 - M13 * M22 * M31 * M44 + M12
			* M23 * M31 * M44 + M13 * M21 * M32 * M44 - M11 * M23 * M32 * M44 - M12 * M21
			* M33 * M44 + M11 * M22 * M33 * M44;

	result.scale(1.f / det);
	return result;
}

//---------------------------------------------------------------------------------------
// Invert an orthonormal Matrix4 and return the result. -M^(-1)*t
//    _           _    _  _
//   |             |  |    |
//   | 00 10 20 30 |  | 30 |
// - | 01 11 21 31 | *| 31 |
//   | 02 12 22 32 |  | 32 |
//   |_           _|  |_  _|
//----------------------------------------------------------------------------------------
Matrix4 Matrix4::invert()const
{
	Float t0 = M11*M14 + M21*M24 + M31*M34;
	Float t1 = M12*M14 + M22*M24 + M32*M34;
	Float t2 = M13*M14 + M23*M24 + M33*M34;

	return Matrix4(M11,M21,M31,-t0,
				      M12,M22,M32,-t1,
				      M13,M23,M33,-t2,
				      M41,M42,M43,M44);
}

// Auto invert an orthonormal Matrix4.
Matrix4& Matrix4::operator!()
{
	Float t0 = M11*M14 + M21*M24 + M31*M34;
	Float t1 = M12*M14 + M22*M24 + M32*M34;
	Float t2 = M13*M14 + M23*M24 + M33*M34;

	Float tmp;

	tmp = M12; M12 = M21; M21 = tmp;
	tmp = M13; M13 = M31; M31 = tmp;
	tmp = M23; M23 = M32; M32 = tmp;

	M14 = -t0;
	M24 = -t1;
	M34 = -t2;

	return (*this);
}

// Rotate from X axis of alpha radian.
void Matrix4::rotateX(const Float alpha)
{
	Float sin = sinf(alpha);
	Float cos = cosf(alpha);

	Vector3 tmp = getY() * cos + getZ() * sin;
			   setZ(-getY() * sin + getZ() * cos);
	setY(tmp);
}

// Rotate from Y axis of alpha radian.
void Matrix4::rotateY(const Float alpha)
{
	Float sin = sinf(alpha);
	Float cos = cosf(alpha);

	Vector3 tmp = getX() * cos + getZ() * sin;
			   setZ(-getX() * sin + getZ() * cos);
	setX(tmp);
}

// Rotate from Z axis of alpha radian.
void Matrix4::rotateZ(const Float alpha)
{
	Float sin = sinf(alpha);
	Float cos = cosf(alpha);

	Vector3 tmp = getX() * cos + getY() * sin;
	setY(-getX() * sin + getY() * cos );
	setX(tmp);
}

// Absolute rotate from X axis of alpha radian.
void Matrix4::rotateAbsX(Float alpha)
{
	Float sin = sinf(alpha);
	Float cos = cosf(alpha);

	register UInt8 r;
	Float tmp;

	for (UInt8 i = 0; i <= 2; ++i)
	{
		r = i << 2;

		tmp = M[r+Y] * cos - M[r+Z] * sin;
		M[r+Z] = M[r+Y] * sin + M[r+Z] * cos;
		M[r+Y] = tmp;
	}
}

// Absolute rotate from Y axis of alpha radian.
void Matrix4::rotateAbsY(Float alpha)
{
	Float sin = sinf(alpha);
	Float cos = cosf(alpha);

	register UInt8 r;
	Float tmp;

	for (UInt8 i = 0; i <= 2; ++i)
	{
		r = i << 2;

		   tmp =  M[r+X] * cos + M[r+Z] * sin;
		M[r+Z] = -M[r+X] * sin + M[r+Z] * cos;
		M[r+X] = tmp;
	}
}

// Absolute rotate from Z axis of alpha radian.
void Matrix4::rotateAbsZ(Float alpha)
{
	Float sin = sinf(alpha);
	Float cos = cosf(alpha);

	register UInt8 r;
	Float tmp;

    for (UInt8 i = 0; i <= 2; ++i) {
		r = i << 2;

		   tmp = M[r+X] * cos - M[r+Y] * sin;
		M[r+Y] = M[r+X] * sin + M[r+Y] * cos;
		M[r+X] = tmp;
	}
}

// Unscale the matrix.
void Matrix4::unscale()
{
    Float len = 1.f / Math::sqrt(M11*M11 + M21*M21 + M31*M31);
	M11 *= len;
	M21 *= len;
	M31 *= len;

    len = 1.f / Math::sqrt(M12*M12 + M22*M22 + M32*M32);
	M12 *= len;
	M22 *= len;
	M32 *= len;

    len = 1.f / Math::sqrt(M13*M13 + M23*M23 + M33*M33);
	M13 *= len;
	M23 *= len;
	M33 *= len;
}

// Recompute the orthonormal matrix basis.
void Matrix4::reComputeBasisGivenX(Vector3 v)
{
	// nouveau vecteur X
	v.normalize();
	setX(v);

	// recalcule une base
	Vector3 axeZ = v^getY();
	Float  len = axeZ.length();

	if (len < o3d::Limits<Float>::epsilon())
	{
		// l'ancien X est colineaire au nouveau Z, utilisons Y pour calculer la nouvelle base
		Vector3 axeY = getZ()^v;
		axeY.normalize();
		setY(axeY);
		setZ(v^axeY);
	}
	else
	{
		// nouveau vecteur Y
		axeZ.normalize();
		setZ(axeZ);
		setY(axeZ^v);
	}
}

// Recompute the orthonormal matrix basis.
void Matrix4::reComputeBasisGivenY(Vector3 v)
{
	// nouveau vecteur X
	v.normalize();
	setY(v);

	// recalcule une base
	Vector3 axeX = v^getZ();
	Float  len = axeX.length();

	if (len < o3d::Limits<Float>::epsilon())
	{
		// l'ancien X est colin�aire au nouveau Z, utilisons Z pour calculer la nouvelle base
		Vector3 axeZ = getX()^v;
		axeZ.normalize();
		setZ(axeZ);
		setX(v^axeZ);
	}
	else
	{
		// nouveau vecteur Y
		axeX.normalize();
		setX(axeX);
		setY(axeX^v);
	}
}

// Recompute the orthonormal matrix basis.
void Matrix4::reComputeBasisGivenZ(Vector3 v)
{
	// nouveau vecteur X
    v.normalize();
	setZ(v);

	// recalcule une base
	Vector3 axeX = getY()^v;
    Float len = axeX.length();

    if (len < o3d::Limits<Float>::epsilon()) {
		// l'ancien X est colineaire au nouveau Z, utilisons Y pour calculer la nouvelle base
		Vector3 axeY = v^getX();
		axeY.normalize();
		setY(axeY);
		setX(axeY^v);
    } else {
		// nouveau vecteur Y
		axeX.normalize();
		setX(axeX);
		setY(v^axeX);
    }
}

// define the matrix as a rotation matrix by an axis and angle (like glRotate)
void Matrix4::setRotation(const Vector3 &axis, Float angle)
{
	Vector3 laxis = axis;
	laxis.normalize();

	Float c = cosf(angle);
	Float s = sinf(angle);
	Float t = 1.f - c;

	Float x = laxis.x();
	Float y = laxis.y();
	Float z = laxis.z();

	Float xx = x*x;
	Float xy = x*y;
	Float xz = x*z;
	Float yy = y*y;
	Float yz = y*z;
	Float zz = z*z;

	M11 = t*xx + c; M12 = t*xy - z*s; M13 = t*xz + y*s; M14 = 0.f;
	M21 = t*xy + z*s; M22 = t*yy + c; M23 = t*yz - x*s; M24 = 0.f;
	M31 = t*xz - y*s; M32 = t*yz + x*s; M33 = t*zz + c; M34 = 0.f;
	M41 = 0.f; M42 = 0.f; M43 = 0.f; M44 = 1.f;
}

void Matrix4::getRotation(Float &x, Float &y, Float &z)const
{
	Quaternion q; q.fromMatrix4(*this);

	//x = atan2((2*(q[W]*q[X] + q[Y]*q[Z])) , (1-2*(q[X]*q[X] + q[Y]*q[Y])));
	////x = atan((2*(q[W]*q[X] + q[Y]*q[Z])) / (1-2*(q[X]*q[X] + q[Y]*q[Y])));
	//y = asin(2*(q[W]*q[Y] - q[Z]*q[X]));
	//z = atan2((2*(q[W]*q[Z] + q[X]*q[Y])) , (1-2*(q[Y]*q[Y] + q[Z]*q[Z])));
	////z = atan((2*(q[W]*q[Z] + q[X]*q[Y])) / (1-2*(q[Y]*q[Y] + q[Z]*q[Z])));

	// the best approach with quaternion and singularity
	Double sqw = q[W]*q[W];
	Double sqx = q[X]*q[X];
	Double sqy = q[Y]*q[Y];
	Double sqz = q[Z]*q[Z];
	Double unit = sqx + sqy + sqz + sqw; // if normalized is one, otherwise is correction factor
	Double test = q[X]*q[Y] + q[Z]*q[W];

	// singularity at north pole
	if (test > 0.499*unit)
	{
		y/*heading*/ = 2 * atan2(q[X],q[W]);
		z/*attitude*/ = o3d::HALF_PI;
		x/*bank*/ = 0;
		return;
	}
	// singularity at south pole
	if (test < -0.499*unit)
	{
		y/*heading*/ = -2 * atan2(q[X],q[W]);
		z/*attitude*/ = -o3d::HALF_PI;
		x/*bank*/ = 0;
		return;
	}

	y/*heading*/  = (Float)atan2((Double)2*q[Y]*q[W] - 2*q[X]*q[Z] , sqx - sqy - sqz + sqw);
	z/*attitude*/ = (Float)asin((Double)2*test/unit);
	x/*bank*/     = (Float)atan2((Double)2*q[X]*q[W] - 2*q[Y]*q[Z] , -sqx + sqy - sqz + sqw);

	//y/*heading*/  = (Float)atan2((Double)2 * (q[X]*q[Y] + q[Z]*q[W]) , sqx - sqy - sqz + sqw);
	//z/*attitude*/ = (Float)asin((Double)-2 * (q[X]*q[Z] - q[Y]*q[W]) / (sqx + sqy + sqz + sqw));
	//x/*bank*/     = (Float)atan2((Double)2 * (q[Y]*q[Z] + q[X]*q[W]) , -sqx - sqy + sqz + sqw);

	// without quaternion... bad idea
	/*Double angle_y = -asin(M13);
	Double C = cos(y);

	if (fabs(C) > 0.005) // Gimbal lock ?
	{
		x = (Float)atan2(-M23 / C, M33 / C);
		z = (Float)atan2(-M12 / C, M11 / C);
	}
	else // Gimbal lock
	{
		x = 0.f; // Angle X is 0
		z = (Float)atan2(M21, M22);
	}

	 y = (Float)angle_y;*/
}

// look at (like gluLookAt)
void Matrix4::setLookAt(const Vector3 &eye, const Vector3 &center, const Vector3 &up)
{
	Vector3 f = center - eye;
	Vector3 lup = up;

	f.normalize();
	lup.normalize();

	Vector3 s = f ^ lup;
	Vector3 u = s ^ f;

	M11 = s.x(); M12 = s.y(); M13 = s.z(); M14 = -eye.x();
	M21 = u.x(); M22 = u.y(); M23 = u.z(); M24 = -eye.y();
	M31 = -f.x(); M32 = -f.y(); M33 = -f.z(); M34 = -eye.z();
	M41 = 0.f; M42 = 0.f; M43 = 0.f; M44 = 1.f;
}

// set the matrix as skew effect
void Matrix4::setSkew(
	Float angle,
	const Vector3 &a,
	const Vector3 &b)
{
	Vector3 n1,n2;
	Vector3 a1,a2;

	Float an1,an2;
	Float rx,ry;
	Float alpha;

	n2 = !n2; // normalize to

	a1 = n2 * a.dot(n2);
	a2 = a - a1;
	n1 = !a2; // normalize to

	an1 = a.dot(n1);
	an2 = a.dot(n2);

	rx = an1*cosf(o3d::toRadian(angle)) - an2*sinf(o3d::toRadian(angle));
	ry = an1*sinf(o3d::toRadian(angle)) + an2*cosf(o3d::toRadian(angle));

	if (rx <= 0.f)
	{
		O3D_ERROR(E_InvalidParameter("Angle too large"));
		return;
	}

	// A parallel to B
	if (an1 == 0.f)
	{
		alpha = 0.f;
	}
	else
	{
		alpha = ry/rx - an2/an1;
	}

	M11 = n1.x()*n2.x()*alpha + 1.f;
	M12 = n1.y()*n2.x()*alpha;
	M13 = n1.z()*n2.x()*alpha;
	M14 = 0.f;

	M21 = n1.x()*n2.y()*alpha;
	M22 = n1.y()*n2.y()*alpha + 1.f;
	M23 = n1.z()*n2.y()*alpha;
	M24 = 0.f;

	M31 = n1.x()*n2.z()*alpha;
	M32 = n1.y()*n2.z()*alpha;
	M33 = n1.z()*n2.z()*alpha + 1.f;
	M34 = 0.f;

	M41 = 0.f;
	M42 = 0.f;
	M43 = 0.f;
	M44 = 1.f;
}

// build a perspective matrix
void Matrix4::buildPerspective(Float coef, Float fov, Float znear, Float zfar)
{
	Float fovt = tanf((o3d::toRadian(fov)) * 0.5f);

	M11 = 1.f / (coef * fovt);
	M12 = 0.f;
	M13 = 0.f;
	M14 = 0.f;

	M21 = 0.f;
	M22 = 1.f / fovt;
	M23 = 0.f;
	M24 = 0.f;

	M31 = 0.f;
	M32 = 0.f;
	M33 = - ((zfar + znear) / (zfar - znear));
	M34 = - ((2.f * zfar * znear) / (zfar - znear));

	M41 = 0.f;
	M42 = 0.f;
	M43 = -1.f;
	M44 = 0.f;
}

// Build a perspective matrix with an infinite zfar
void Matrix4::buildPerspective(Float coef, Float fov, Float znear)
{
	Float fovt = tanf((o3d::toRadian(fov)) * 0.5f);

	M11 = 1.f / (coef * fovt);
	M12 = 0.f;
	M13 = 0.f;
	M14 = 0.f;

	M21 = 0.f;
	M22 = 1.f / fovt;
	M23 = 0.f;
	M24 = 0.f;

	M31 = 0.f;
	M32 = 0.f;
	M33 = (o3d::Limits<Float>::epsilon() - 1.f);
	M34 =  znear * (o3d::Limits<Float>::epsilon() - 2.f);
//	M33 = -1.f;
//	M34 = -2.f * znear;

	M41 = 0.f;
	M42 = 0.f;
	M43 = -1.f;
	M44 = 0.f;
}

//! build an ortho matrix
void Matrix4::buildOrtho(Float znear, Float zfar)
{
	M11 = 1.f;
	M12 = 0.f;
	M13 = 0.f;
	M14 = 0.f;

	M21 = 0.f;
	M22 = 1.f;
	M23 = 0.f;
	M24 = 0.f;

	M31 = 0.f;
	M32 = 0.f;
	M33 = -2.f / (zfar - znear);//1.f;
	M34 = - ((zfar + znear) / (zfar - znear));//0.f;

	M41 = 0.f;//1.f / (zfar-znear);
	M42 = 0.f;//-znear/(zfar-znear);
	M43 = 0.f;
	M44 = 1.f;
}

// Build an ortho matrix
void Matrix4::buildOrtho(
	Float left,
	Float right,
	Float bottom,
	Float top,
	Float znear,
	Float zfar)
{
	M11 = 2.f / (right - left);
	M12 = 0.f;
	M13 = 0.f;
	M14 = - ((right + left) / (right - left));

	M21 = 0.f;
	M22 = 2.f / (top - bottom);
	M23 = 0.f;
	M24 = - ((top + bottom) / (top - bottom));

	M31 = 0.f;
	M32 = 0.f;
	M33 = -2.f / (zfar - znear);
	M34 = - ((zfar + znear) / (zfar - znear));

	M41 = 0.f;
	M42 = 0.f;
	M43 = 0.f;
	M44 = 1.f;
}

// convert to a string
Matrix4::operator String() const
{
	String temp;

    for (UInt32 i = 0 ; i < 4 ; ++i) {
		temp << String("\n{ ")
			 << operator ()(i,0) << String(", ")
			 << operator ()(i,1) <<	String(", ")
			 << operator ()(i,2) << String(", ")
			 << operator ()(i,3)
			 << String(" }");
    }

	return temp;
}

// serialisation
Bool Matrix4::writeToFile(OutStream& os)const
{
    for (UInt32 i = 0 ; i < 16 ; ++i) {
        os << M[i];
    }

	return True;
}

Bool Matrix4::readFromFile(InStream& is)
{
    for (UInt32 i = 0 ; i < 16 ; ++i) {
        is >> M[i];
    }

	return True;
}

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

#undef M41
#undef M42
#undef M43
#undef M44
