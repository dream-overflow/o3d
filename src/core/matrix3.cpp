/**
 * @file matrix3.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/matrix3.h"

#include "o3d/core/matrix4.h"
#include "o3d/core/math.h"
#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"
#include <math.h>

using namespace o3d;

Matrix3* Matrix3::ms_identity = nullptr;
Matrix3* Matrix3::ms_null = nullptr;

void Matrix3::init()
{
	if (!ms_identity)
	{
		ms_identity = new Matrix3();
		ms_null = new Matrix3();
		ms_null->zero();
	}
}

void Matrix3::quit()
{
	deletePtr(ms_identity);
	deletePtr(ms_null);
}

// Copy constructor from 4x4 matrix. Take the upper-left 3x3 matrix.
Matrix3::Matrix3(const Matrix4& _M)
{
	M[0][0] = _M[0][0]; M[1][0] = _M[1][0]; M[2][0] = _M[2][0];
	M[0][1] = _M[0][1]; M[1][1] = _M[1][1]; M[2][1] = _M[2][1];
	M[0][2] = _M[0][2]; M[1][2] = _M[1][2]; M[2][2] = _M[2][2];
}

// Copy from 4x4 matrix. Take the upper-left 3x3 matrix.
Matrix3& Matrix3::operator=(const Matrix4& _M)
{
	M[0][0] = _M[0][0]; M[1][0] = _M[1][0]; M[2][0] = _M[2][0];
	M[0][1] = _M[0][1]; M[1][1] = _M[1][1]; M[2][1] = _M[2][1];
	M[0][2] = _M[0][2]; M[1][2] = _M[1][2]; M[2][2] = _M[2][2];

	return (*this);
}

// Transpose the matrix.
void Matrix3::transpose()
{
	Int32 i, j;
	Float _M[3][3];

    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {
			_M[j][i] = M[i][j];
        }
    }

	memcpy(M, _M, sizeof(Float) * 9);
}

// Return the transpose of the matrix.
Matrix3 Matrix3::transposeTo() const
{
	Int32 i, j;
	Matrix3 _M;

    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {
			_M.M[j][i] = M[i][j];
        }
    }

	return _M;
}

// 3x3 matrix product.
Matrix3& Matrix3::operator*=(const Matrix3 &_M)
{
	Int32 i, j;
	Float __M[3][3];

    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {
			__M[j][i] = M[0][i] * _M.M[j][0] + M[1][i] * _M.M[j][1] + M[2][i] * _M.M[j][2];
        }
    }

	memcpy(M, __M, sizeof(Float) * 9);
	return (*this);
}

// Return the matrix product of two 3x3 matrices.
Matrix3 Matrix3::operator*(const Matrix3 &_M) const
{
	Int32 i, j;
	Matrix3 __M;

    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {
			__M.M[j][i] = M[0][i] * _M.M[j][0] + M[1][i] * _M.M[j][1] + M[2][i] * _M.M[j][2];
        }
    }

	return __M;
}

// Return the matrix product of this 3x3 and the upper-left 3x3 sub-matrix of a 4x4 matrix.
Matrix4 Matrix3::operator*(const Matrix4 &_M) const
{
	Int32 i, j;
	Matrix4 __M;

    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 4; ++j) {
			__M(i, j) = M[0][i] * _M(0, j) + M[1][i] * _M(1, j) + M[2][i] * _M(2, j);
        }
    }

	return __M;
}

// Rotate from X axis of alpha radian.
void Matrix3::rotateX(const Float alpha)
{
	Float sin = sinf(alpha);
	Float cos = cosf(alpha);

	Vector3 tmp = getY() * cos + getZ() * sin;
	setZ(getZ() * cos - getY() * sin);
	setY(tmp);
}

// Rotate from Y axis of alpha radian.
void Matrix3::rotateY(const Float alpha)
{
	Float sin = sinf(alpha);
	Float cos = cosf(alpha);

	Vector3 tmp = getX() * cos + getZ() * sin;
	setZ(getZ() * cos - getX() * sin);
	setX(tmp);
}

// Rotate from Z axis of alpha radian.
void Matrix3::rotateZ(const Float alpha)
{
	Float sin = sinf(alpha);
	Float cos = cosf(alpha);

	Vector3 tmp = getX() * cos + getY() * sin;
	setY(getY() * cos - getX() * sin);
	setX(tmp);
}

// Absolute rotate from X axis of alpha radian.
void Matrix3::rotateAbsX(Float alpha)
{
	Float sin = sinf(alpha);
	Float cos = cosf(alpha);

	Float tmp;

    for (Int32 i = 0; i <= 2; ++i) {
		tmp = M[i][Y] * cos - M[i][Z] * sin;
		M[i][Z] = M[i][Z] * cos + M[i][Y] * sin;
		M[i][Y] = tmp;
	}
}

// Absolute rotate from Y axis of alpha radian.
void Matrix3::rotateAbsY(Float alpha)
{
	Float sin = sinf(alpha);
	Float cos = cosf(alpha);

	Float tmp;

    for (Int32 i = 0; i <= 2; ++i) {
		tmp = M[i][X] * cos - M[i][Z] * sin;
		M[i][Z] = M[i][Z] * cos + M[i][X] * sin;
		M[i][X] = tmp;
	}
}

// Absolute rotate from Z axis of alpha radian.
void Matrix3::rotateAbsZ(Float alpha)
{
	Float sin = sinf(alpha);
	Float cos = cosf(alpha);

	Float tmp;

    for (Int32 i = 0; i <= 2; ++i) {
		tmp = M[i][X] * cos - M[i][Y] * sin;
		M[i][Y] = M[i][Y] * cos + M[i][X] * sin;
		M[i][X] = tmp;
	}
}

// Scale on the three axis.
void Matrix3::scale(const Vector3 &V)
{
    for (Int32 i = 0; i < 3; ++i) {
        for (Int32 j = 0; j < 3; ++j) {
			M[i][j] *= V[i];
        }
    }
}

// Scale a specific axis.
void Matrix3::scale(UInt32 axe, Float scale)
{
    for (Int32 i = 0; i < 3; ++i) {
		M[axe][i] *= scale;
    }
}

// Uniform scale.
void Matrix3::scale(Float scale)
{
    for (Int32 i = 0; i < 3; ++i) {
        for (Int32 j = 0; j < 3; ++j) {
			M[i][j] *= scale;
        }
    }
}

// Return the 3 eigenvectors.
void Matrix3::eigenVector(Vector3 &R, Vector3 &S, Vector3 &T) const
{
	// Conversion en notations standards
	Float m11 = M[0][0], m12 = M[1][0], m13 = M[2][0];
	Float m21 = M[0][1], m22 = M[1][1], m23 = M[2][1];
	Float m31 = M[0][2], m32 = M[1][2], m33 = M[2][2];

	Float v[3];
	eigenValue(v[0], v[1], v[2]);

	O3D_ASSERT(o3d::abs((v[0] - v[1])) > 0.0001f);
	O3D_ASSERT(o3d::abs((v[0] - v[2])) > 0.0001f);
	O3D_ASSERT(o3d::abs((v[1] - v[2])) > 0.0001f);

	O3D_ASSERT(o3d::abs(v[0]) > 0.0001f);
	O3D_ASSERT(o3d::abs(v[1]) > 0.0001f);
	O3D_ASSERT(o3d::abs(v[2]) > 0.0001f);

	Vector3 * sol[3] = { &R, &S, &T };

	Float a11, a12(m12), a13(m13);
	Float a21(m21), a22, a23(m23);
	Float a31(m31), a32(m32), a33;

    for (UInt32 k = 0; k < 3; ++k) {
		// On calcule le vecteur propre associe a v[k]

		Float ev = v[k]; // Valeur propre courante

		a11 = m11 - ev;
		a22 = m22 - ev;
		a33 = m33 - ev;

		Float det = (a22 * a33 - a23 * a32);

        if (o3d::abs(det) > 0.0001f) { // Avec l'hypothese suivante, le det est forcement non nul
			// Supposons que la premiere composante soit 1.

			Float x2 = -((a23 * -a31) - (a33 * -a21)) / det;
			Float x3 = ((a22 * -a31) - (a32 * -a21)) / det;

			Vector3 bas = Vector3(1.0f, x2, x3);

            //Vector3 img = (*this) * bas;

            if (((*this) * bas - bas * ev).normInf() < 0.001f) {
				*sol[k] = bas;
				continue;
			}
		}

		det = (a11 * a33 - a13 * a31);

        if (o3d::abs(det) > 0.0001f) { // Avec l'hypothese suivante, le det est forcement non nul
			// Supposons que la seconde composante soit 1.

			Float x1 = -((a13 * -a32) - (a33 * -a12)) / det;
			Float x3 = ((a11 * -a32) - (a31 * -a12)) / det;

			Vector3 bas = Vector3(x1, 1.0f, x3);

            if (((*this) * bas - bas * ev).normInf() < 0.001f) {
				*sol[k] = bas;
				continue;
			}
		}

		det = (a11 * a22 - a21 * a12);

        if (o3d::abs(det) > 0.0001f) { // Avec l'hypothese suivante, le det est forcement non nul
			// Supposons que la troisieme composante soit 1.

			Float x1 = -((a12 * -a23) - (a22 * -a13)) / det;
			Float x2 = ((a11 * -a23) - (a21 * -a13)) / det;

			Vector3 bas = Vector3(x1, x2, 1.0f);

            if (((*this) * bas - bas * ev).normInf() < 0.001f) {
				*sol[k] = bas;
				continue;
			}
		}

		O3D_ASSERT(0);
	}
}

// Return the 3 eigenvalues.
void Matrix3::eigenValue(Float & e1, Float & e2, Float & e3) const
{
	// Conversion to standard notations
	Float m11 = M[0][0], m12 = M[1][0], m13 = M[2][0];
	Float m21 = M[0][1], m22 = M[1][1], m23 = M[2][1];
	Float m31 = M[0][2], m32 = M[1][2], m33 = M[2][2];

	// First, we compute the coefficient of the characteristic polynomial
	// Its degree is 3, so we can solve it directly:
	// a * Z^3 + b * Z^2 + c * Z + d = 0
	Float a = -1.0f;
	Float b = m33 + m22 + m11;
	Float c = m31 * m13 + m21 * m12 + m32 * m23 - m11 * m22 - m11 * m33 - m22 * m33;
	Float d = m11 * m22 * m33 + m31 * m12 * m23 + m21 * m32 * m13 - m13 * m31 * m22
			- m12 * m21 * m33 - m23 * m32 * m11;

	// We use the Cardan's Method: http://fr.wikipedia.org/wiki/M%C3%A9thode_de_Cardan
	// We must transform the equation in:
	// X^3 + p*X + q = 0
	Float p = -b * b / (3.0f * a * a) + c / a;
	Float q = b / (27.0f * a) * (2.0f * b * b / (a * a) - 9.0f * c / a) + d / a;

    if (o3d::abs(p) < 0.0001f) {
        if (o3d::abs(q) > 0.0001f) {
            // Means that there is a unique solution. Impossible here !
			O3D_ASSERT(0);
		}

		// In this case, there is triple solution
		e1 = e2 = e3 = -b / (3.0f * a);
		return;
	}

    if (o3d::abs(q) < 0.0001f) {
		O3D_ASSERT(p < 0.0f);

		e1 = -b / (3.0f * a);
        e2 = Math::sqrt(-p) + e1;
		e3 = -e2 + e1;
		return;
	}

	Float delta = q * q + 4.0f / 27.0f * p * p * p;

    if (delta < -0.0001f) {
        // It means that the 3 solutions are real
        Float c0 = 2.0f * Math::sqrt(-p / 3.0f);
        Float c1 = acosf(-q / 2.0f * Math::sqrt(27.0f / -(p * p * p)));

		e1 = c0 * cosf(1.0f / 3.0f * (c1 + 2.0f * o3d::PI)) - b / (3.0f * a);
		e2 = c0 * cosf(1.0f / 3.0f * (c1 + 4.0f * o3d::PI)) - b / (3.0f * a);
		e3 = c0 * cosf(1.0f / 3.0f * (c1 + 6.0f * o3d::PI)) - b / (3.0f * a);
    } else if (delta < 0.0001f) {
        // It means 2 real solutions
		e1 = 3.0f * q / p - b / (3.0f * a);
		e2 = -1.5f * q / p - b / (3.0f * a);
		e3 = 0.f;//e3;
    } else {
		O3D_ASSERT(0);
	}
}

// Compute eigens value and eigens vector for a symmetric matrix
void Matrix3::computeSymmetricEigenSystem(Matrix3 &eigenVectors, Vector3 &eigenValues)
{
	const Int32 maxSweeps = 32;
    const Float epsilon = 1e-6f;  // o3d::limits<Float>::epsilon();

	Matrix3 inp;
	Int32 rot = 0;
    /*register*/ Int32 i;
	Vector3 B;
	Vector3 Z;

	// Save the input matrix in orig, use new matrix inp
	inp = *this;
	// Set vectors to the identity matrix
	eigenVectors.identity();
	// Set B and values to the diagonal of the input matrix
    for (i = 0; i < 3; i++) {
		B[i] = eigenValues[i] = inp(i,i);
	}

	// Rotate until off diagonal elements of input matrix are zero
    for (Int32 sweep = 0; sweep < maxSweeps; ++sweep) {
		Float sum = o3d::abs(inp(0,1)) + o3d::abs(inp(0,2)) + o3d::abs(inp(1,2));
		Float thresh;
        /*register*/ Int32 p, q;

        if (o3d::abs(sum) < epsilon) { // Normal return relies on quadratic
			break;
        }

		thresh = (sweep < 4) ? sum * 0.2f / 9.0f : 0.0f;  // First three sweeps?

        for (p = 0; p < 2; p++) {
            for (q = p+1; q < 3; q++) {
                /*register*/ Float g = 100.0f * o3d::abs(inp(p,q));

				// After 4 sweeps, skip the rotation if the
				// off-diagonal element is small.
                if ((sweep > 4) && (o3d::abs(g) < epsilon)) {
					inp(p,q) = 0.0;
                } else if (o3d::abs(inp(p,q)) > thresh) {
                    /*register*/ Float h = eigenValues[q] - eigenValues[p];
					Float c, s, t;  // cosine, sine, tangent of rotation angle
					Float tau;
                    /*register*/ Int32 j;

                    if (o3d::abs(g) < epsilon) {
						t = inp(p,q) / h;
                    } else {
						Float theta = 0.5f * h / inp(p,q);
                        t = 1.0f / (o3d::abs(theta) + Math::sqrt(1.0f + theta*theta));
                        if (theta < 0.0f) {
							t = -t;
                        }
					}

                    c = 1.0f / Math::sqrt(1.0f + t*t);   // cosine of rotation angle
					s = t*c;                     // sine of rotation angle
					tau = s / (1.0f + c);

					h = t * inp(p,q);
					Z[p] -= h;
					Z[q] += h;
					eigenValues[p] -= h;
					eigenValues[q] += h;
					inp(p,q) = 0.0f;

					// case of rotations 0 <= j < p-1
                    for (j = 0; j <= p-1; j++) {
						g = inp(j,p);
						h = inp(j,q);
						inp(j,p) = g - s*(h + g*tau);
						inp(j,q) = h + s*(g - h*tau);
					}

					// case of rotations p < j < q
                    for (j = p + 1; j < q; j++) {
						g = inp(p,j);
						h = inp(j,q);
						inp(p,j) = g - s*(h - g*tau);
						inp(j,q) = h + s*(g - h*tau);
					}

					// case of rotations q < j < 3
                    for (j = q + 1; j < 3; j++) {
						g = inp(p,j);
						h = inp(q,j);
						inp(p,j) = g - s*(h + g*tau);
						inp(q,j) = h + s*(g - h*tau);
					}

					// Set the eigen vectors
                    for (j = 0; j < 3; j++) {
						g = eigenVectors(j,p);
						h = eigenVectors(j,q);
						eigenVectors(j,p) = g - s*(h + g*tau);
						eigenVectors(j,q) = h + s*(g - h*tau);
					}
					rot++;
				}
			}
		}

		// Set the eigen values
		B += Z;
		eigenValues = B;
		Z.set(0.f,0.f,0.f);
	}

	// sort from greater to lesser
    if (eigenValues[0] < eigenValues[1]) {
		std::swap(eigenValues[0], eigenValues[1]);
        for (i = 0; i < 3; i++) {
			std::swap(eigenVectors(0,i), eigenVectors(1,i));
        }
	}

    if (eigenValues[1] < eigenValues[2]) {
		std::swap(eigenValues[1], eigenValues[2]);
        for (i = 0; i < 3; i++) {
			std::swap(eigenVectors(1,i), eigenVectors(2,i));
        }
	}

    if (eigenValues[0] < eigenValues[1]) {
		std::swap(eigenValues[0], eigenValues[1]);
        for (i = 0; i < 3; i++) {
			std::swap(eigenVectors(0,i), eigenVectors(1,i));
        }
	}
/*
	// Conversion to standard notations
	Float m11 = M[0][0], m12 = M[1][0], m13 = M[2][0];
	Float m22 = M[1][1], m23 = M[2][1];
	Float m33 = M[2][2];

	eigenVectors.identity();

	for (Int32 a = 0; a < maxSweeps; ++a)
	{
		// exit if off-diagonal entries small enough
		if ((o3d::abs(m12) < epsilon) && (o3d::abs(m13) < epsilon) && (o3d::abs(m23) < epsilon))
			break;

		// annihilate (1,2) entry
		if (m12 != 0.f)
		{
			Float u = (m22 - m11) * 0.5f / m12;
			Float u2 = u * u;
			Float u2p1 = u2 + 1.f;
            Float t = (u2p1 != u2) ? ((u < 0.f) ? -1.f : 1.f) * (Math::sqrt(u2p1) - o3d::abs(u)) : 0.5f / u;
            Float c = 1.f / Math::sqrt(t * t + 1.f);
			Float s = c * t;

			m11 -= t * m12;
			m22 += t * m12;
			m12 = 0.f;

			Float temp = c * m13 - s * m23;
			m23 = s * m13 + c * m23;
			m13 = temp;

			for (UInt32 i = 0; i < 3; ++i)
			{
				Float temp = c * eigenVectors(i,0) - s * eigenVectors(i,1);
				eigenVectors(i,1) = s * eigenVectors(i,0) + c * eigenVectors(i,1);
				eigenVectors(i,0) = temp;
			}
		}

		// annihilate (1,3) entry
		if (m13 != 0.f)
		{
			Float u = (m33 - m11) * 0.5f / m13;
			Float u2 = u * u;
			Float u2p1 = u2 + 1.f;
            Float t = (u2p1 != u2) ? ((u < 0.f) ? -1.f : 1.f) * (Math::sqrt(u2p1) - o3d::abs(u)) : 0.5f / u;
            Float c = 1.f / Math::sqrt(t * t + 1.f);
			Float s = c * t;

			m11 -= t * m13;
			m33 += t * m13;
			m13 = 0.f;

			Float temp = c * m12 - s * m23;
			m23 = s * m12 + c * m23;
			m12 = temp;

			for (Int32 i = 0; i < 3; ++i)
			{
				Float temp = c * eigenVectors(i,0) - s * eigenVectors(i,2);
				eigenVectors(i,2) = s * eigenVectors(i,0) + c * eigenVectors(i,2);
				eigenVectors(i,0) = temp;
			}
		}

		// annihilate (2,3) entry
		if (m23 != 0.f)
		{
			Float u = (m33 - m22) * 0.5f / m23;
			Float u2 = u * u;
			Float u2p1 = u2 + 1.f;
            Float t = (u2p1 != u2) ? ((u < 0.f) ? -1.f : 1.f) * (Math::sqrt(u2p1) - o3d::abs(u)) : 0.5f / u;
            Float c = 1.f / Math::sqrt(t * t + 1.f);
			Float s = c * t;

			m22 -= t * m23;
			m33 += t * m23;
			m23 = 0.f;

			Float temp = c * m12 - s * m13;
			m23 = s * m12 + c * m13;
			m12 = temp;

			for (Int32 i = 0; i < 3; ++i)
			{
				Float temp = c * eigenVectors(i,1) - s * eigenVectors(i,2);
				eigenVectors(i,2) = s * eigenVectors(i,1) + c * eigenVectors(i,2);
				eigenVectors(i,1) = temp;
			}
		}
	}

	eigenValues[0] = m11;
	eigenValues[1] = m22;
	eigenValues[2] = m33;*/
}

// Return the inverse of the matrix
Matrix3 Matrix3::invert() const
{
#define ACCESS(i,j) Data[j*3+i]
#define ACCESS_R(i,j) ResData[j*3+i]
#define SUBDET(i0,j0, i1,j1) ( ACCESS(j0,i0)*ACCESS(j1,i1) - ACCESS(j1,i0)*ACCESS(j0,i1) )

	Matrix3 r;

	const Float* Data = this->getData();
	Float* ResData = r.getData();

	// cofactor
	Float c00, c01, c02, c10, c11, c12, c20, c21, c22;

	c00 = SUBDET(1,1, 2,2);  //   (11 22 - 21 12)
	c01 = -SUBDET(1,0, 2,2); // - (10 22 - 21 02)
	c02 = SUBDET(1,0, 2,1);  //   (01 12 - 11 02)

	c10 = -SUBDET(0,1, 2,2); // - (10 22 - 20 12)
	c11 = SUBDET(0,0, 2,2);  //   (00 22 - 20 02)
	c12 = -SUBDET(0,0, 2,1); // - (00 12 - 10 02)

	c20 = SUBDET(0,1, 1,2);  //   (10 21 - 20 11)
	c21 = -SUBDET(0,0, 1,2); // - (00 21 - 20 01)
	c22 = SUBDET(0,0, 1,1);  //   (00 11 - 10 01)

	// compute 3x3 determinant & its reciprocal
	Float det = ACCESS(0,0) * c00 + ACCESS(1,0) * c01 + ACCESS(2,0) * c02;

	// the Data can't be inverted
    if (det == 0.0f) {
		return r;
	}

	det = 1.0f / det;

	ACCESS_R(0,0) = c00 * det;
	ACCESS_R(0,1) = c01 * det;
	ACCESS_R(0,2) = c02 * det;

	ACCESS_R(1,0) = c10 * det;
	ACCESS_R(1,1) = c11 * det;
	ACCESS_R(1,2) = c12 * det;

	ACCESS_R(2,0) = c20 * det;
	ACCESS_R(2,1) = c21 * det;
	ACCESS_R(2,2) = c22 * det;

	return r;

#undef ACCESS
#undef ACCESS_R
#undef SUBDET
}

// Convert to a string
Matrix3::operator String() const
{
	String temp;

    for (UInt32 i = 0; i < 3; ++i) {
		temp << String("\n{ ") << operator ()(i, 0) << String(", ") << operator ()(
				i,
				1) << String(", ") << operator ()(i, 2) << String(" }");
    }

	return temp;
}

// serialization
Bool Matrix3::writeToFile(OutStream& os) const
{
    for (UInt32 i = 0; i < 3; ++i) {
        for (UInt32 j = 0; j < 3; ++j) {
            os << M[j][i];
        }
    }

	return True;
}

Bool Matrix3::readFromFile(InStream &is)
{
    for (UInt32 i = 0; i < 3; ++i) {
        for (UInt32 j = 0; j < 3; ++j) {
            is >> M[j][i];
        }
    }

	return True;
}
