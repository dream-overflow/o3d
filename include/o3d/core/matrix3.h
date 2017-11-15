/**
 * @file matrix3.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MATRIX3_H
#define _O3D_MATRIX3_H

#include "memorydbg.h"
#include "vector3.h"

namespace o3d {

class InStream;
class OutStream;
class Matrix4;
class String;

/**
 * @brief 3x3 Matrix definition
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-07-21
 * Row major 3x3 matrix. Notice than Matrix3 is column major like OpenGL.
 */
class O3D_API Matrix3
{
	friend class Math;

public:

	//! Return an identity matrix const reference.
    static const Matrix3& getIdentity() { return *ms_identity; }

	//! Return an null (full of zero) matrix const reference.
    static const Matrix3& getNull() { return *ms_null; }

	//! Default constructor. Identity matrix.
	inline Matrix3()
	{
		M[0][0] = 1.0f; M[0][1] = 0.0f; M[0][2] = 0.0f;
		M[1][0] = 0.0f; M[1][1] = 1.0f; M[1][2] = 0.0f;
		M[2][0] = 0.0f; M[2][1] = 0.0f; M[2][2] = 1.0f;
	}

	//! Copy constructor.
	inline Matrix3(const Matrix3& _M)
	{
		memcpy(M,_M.M,sizeof(Float)*9);
	}

	//! Copy constructor from the 3x3 sub-matrix of a 4x4 matrix.
	Matrix3(const Matrix4& _M);

	//! Construct from 9 float.
	inline Matrix3(Float a11, Float a12, Float a13,
					  Float a21, Float a22, Float a23,
					  Float a31, Float a32, Float a33)
	{
		M[0][0] = a11; M[0][1] = a21; M[0][2] = a31;
		M[1][0] = a12; M[1][1] = a22; M[1][2] = a32;
		M[2][0] = a13; M[2][1] = a23; M[2][2] = a33;
	}

	//! Construct from 3 Vector3.
	inline Matrix3(
			const Vector3 &row1,
			const Vector3 &row2,
			const Vector3 &row3)
	{
		M[0][0] = row1.x(); M[0][1] = row2.x(); M[0][2] = row3.x();
		M[1][0] = row1.y(); M[1][1] = row2.y(); M[1][2] = row3.y();
		M[2][0] = row1.z(); M[2][1] = row2.z(); M[2][2] = row3.z();
	}

	//! Zero the matrix.
	inline void zero()
	{
		memset(M,0,9*sizeof(Float));
	}

	//! Identity matrix.
	inline void identity()
	{
		M[0][0] = 1.0f; M[0][1] = 0.0f; M[0][2] = 0.0f;
		M[1][0] = 0.0f; M[1][1] = 1.0f; M[1][2] = 0.0f;
		M[2][0] = 0.0f; M[2][1] = 0.0f; M[2][2] = 1.0f;
	}

	//! Transpose the matrix.
	void transpose();
	//! Return the transpose of the matrix.
	Matrix3 transposeTo()const;

	//! Copy a 3x3 matrix.
	inline Matrix3& operator= (const Matrix3& _M)
	{
		memcpy(M,_M.M,sizeof(Float)*9);
		return (*this);
	}

	//! Copy from the 3x3 sub-matrix of a 4x4 matrix.
	Matrix3& operator= (const Matrix4& _M);

	//! Add the two matrices and set the result into this.
	inline Matrix3& operator+= (const Matrix3& _M)
	{
		M[0][0] += _M.M[0][0]; M[1][0] += _M.M[1][0]; M[2][0] += _M.M[2][0];
		M[0][1] += _M.M[0][1]; M[1][1] += _M.M[1][1]; M[2][1] += _M.M[2][1];
		M[0][2] += _M.M[0][2]; M[1][2] += _M.M[1][2]; M[2][2] += _M.M[2][2];

		return (*this);
	}

	//! Subtract the two matrices and set the result into this.
	inline Matrix3& operator-= (const Matrix3& _M)
	{
		M[0][0] -= _M.M[0][0]; M[1][0] -= _M.M[1][0]; M[2][0] -= _M.M[2][0];
		M[0][1] -= _M.M[0][1]; M[1][1] -= _M.M[1][1]; M[2][1] -= _M.M[2][1];
		M[0][2] -= _M.M[0][2]; M[1][2] -= _M.M[1][2]; M[2][2] -= _M.M[2][2];

		return (*this);
	}

	//! 3x3 matrix product.
	Matrix3& operator*= (const Matrix3& _M);

	//! Scalar product.
	inline Matrix3& operator*= (const Float scal)
	{
		M[0][0] *= scal; M[1][0] *= scal; M[2][0] *= scal;
		M[0][1] *= scal; M[1][1] *= scal; M[2][1] *= scal;
		M[0][2] *= scal; M[1][2] *= scal; M[2][2] *= scal;

		return (*this);
	}

	//! Return the opposite matrix.
	inline Matrix3 operator- ()const
	{
		Matrix3 _M;

		_M.M[0][0] = -M[0][0]; _M.M[1][0] = -M[1][0]; _M.M[2][0] = -M[2][0];
		_M.M[0][1] = -M[0][1]; _M.M[1][1] = -M[1][1]; _M.M[2][1] = -M[2][1];
		_M.M[0][2] = -M[0][2]; _M.M[1][2] = -M[1][2]; _M.M[2][2] = -M[2][2];

		return _M;
	}

	//! Return the matrix product of two 3x3 matrices.
	Matrix3 operator* (const Matrix3& _M)const;
	//! Return the matrix product of this 3x3 and the upper-left 3x3 sub-matrix of a 4x4 matrix.
	Matrix4 operator* (const Matrix4& _M)const;

	//! Return a the matrix product of a Vector3 by the matrix.
	inline Vector3 operator* (const Vector3& vec)const
	{
		Vector3 v;

		v[X] = M[0][0]*vec[X] + M[1][0]*vec[Y] + M[2][0]*vec[Z];
		v[Y] = M[0][1]*vec[X] + M[1][1]*vec[Y] + M[2][1]*vec[Z];
		v[Z] = M[0][2]*vec[X] + M[1][2]*vec[Y] + M[2][2]*vec[Z];

		return v;
	}

	//! Return the scalar product.
	inline Matrix3 operator* (const Float scale) const
	{
		Matrix3 _M;

		_M.M[0][0] = M[0][0] * scale; _M.M[1][0] = M[1][0] * scale; _M.M[2][0] = M[2][0] * scale;
		_M.M[0][1] = M[0][1] * scale; _M.M[1][1] = M[1][1] * scale; _M.M[2][1] = M[2][1] * scale;
		_M.M[0][2] = M[0][2] * scale; _M.M[1][2] = M[1][2] * scale; _M.M[2][2] = M[2][2] * scale;

		return _M;
	}

	//! Add two matrices and return the result as a new matrix.
	inline Matrix3 operator+ (const Matrix3& _M)const
	{
		Matrix3 __M;

		__M.M[0][0] = M[0][0] + _M.M[0][0]; __M.M[1][0] = M[1][0] + _M.M[1][0]; __M.M[2][0] = M[2][0] + _M.M[2][0];
		__M.M[0][1] = M[0][1] + _M.M[0][1]; __M.M[1][1] = M[1][1] + _M.M[1][1]; __M.M[2][1] = M[2][1] + _M.M[2][1];
		__M.M[0][2] = M[0][2] + _M.M[0][2]; __M.M[1][2] = M[1][2] + _M.M[1][2]; __M.M[2][2] = M[2][2] + _M.M[2][2];

		return  __M;
	}

	//! Subtract two matrices and return the result as a new matrix.
	inline Matrix3 operator- (const Matrix3& _M) const
	{
		Matrix3 __M;

		__M.M[0][0] = M[0][0] - _M.M[0][0]; __M.M[1][0] = M[1][0] - _M.M[1][0]; __M.M[2][0] = M[2][0] - _M.M[2][0];
		__M.M[0][1] = M[0][1] - _M.M[0][1]; __M.M[1][1] = M[1][1] - _M.M[1][1]; __M.M[2][1] = M[2][1] - _M.M[2][1];
		__M.M[0][2] = M[0][2] - _M.M[0][2]; __M.M[1][2] = M[1][2] - _M.M[1][2]; __M.M[2][2] = M[2][2] - _M.M[2][2];

		return  __M;
	}

	//! Return the column i of the matrix (read only).
	inline const Float* operator[] (UInt32 i) const
	{
		O3D_ASSERT(i < 3);
		return &M[i][0];
	}
	//! Return the column i of the matrix.
	inline Float* operator[] (UInt32 i)
	{
		O3D_ASSERT(i < 3);
		return &M[i][0];
	}

	//! Set the matrix with 9 float.
	inline void setData(
			Float a11, Float a12, Float a13,
			Float a21, Float a22, Float a23,
			Float a31, Float a32, Float a33)
	{
		M[0][0] = a11; M[0][1] = a21; M[0][2] = a31;
		M[1][0] = a12; M[1][1] = a22; M[1][2] = a32;
		M[2][0] = a13; M[2][1] = a23; M[2][2] = a33;
	}

	//! Get an entry (read only).
	//! @param i row
	//! @param j column
	inline const Float& operator() (UInt32 i, UInt32 j) const
	{
		O3D_ASSERT(i < 3 && j < 3);
		return M[j][i];
	}

	//! Get an entry.
	//! @param i row
	//! @param j column
	inline Float& operator() (UInt32 i, UInt32 j)
	{
		O3D_ASSERT(i < 3 && j < 3);
		return M[j][i];
	}

	//! Return the column X of the matrix.
	inline Vector3 getX() const { return Vector3(M[0]); }

	//! Return the column Y of the matrix.
	inline Vector3 getY() const { return Vector3(M[1]); }

	//! Return the column Z of the matrix.
	inline Vector3 getZ() const { return Vector3(M[2]); }

	//! Get the first row of the matrix
	inline Vector3 getRow1() const
	{
		return Vector3(M[0][0], M[1][0], M[2][0]);
	}

	//! Get the second row of the matrix
	inline Vector3 getRow2() const
	{
		return Vector3(M[0][1], M[1][1], M[2][1]);
	}

	//! Get the third row of the matrix
	inline Vector3 getRow3() const
	{
		return Vector3(M[0][2], M[1][2], M[2][2]);
	}

	//! Set the first row of the matrix
	inline void setRow1(const Vector3 &v)
	{
		M[0][0] = v.x();
		M[1][0] = v.y();
		M[2][0] = v.z();
	}

	//! Set the second row of the matrix
	inline void setRow2(const Vector3 &v)
	{
		M[0][1] = v.x();
		M[1][1] = v.y();
		M[2][1] = v.z();
	}

	//! Set the third row of the matrix
	inline void setRow3(const Vector3 &v)
	{
		M[0][2] = v.x();
		M[1][2] = v.y();
		M[2][2] = v.z();
	}

	//! Get the matrix data pointer.
	inline Float* getData()
	{
		return (Float*) &M[0][0];
	}

	//! Get the matrix data pointer (read only).
	inline const Float* getData() const
	{
		return (Float*) &M[0][0];
	}

	//! Set the column X of the matrix.
	inline void setX(const Vector3& vec)
	{
		M[0][0] = vec.x();
		M[0][1] = vec.y();
		M[0][2] = vec.z();
	}

	//! Set the column Y of the matrix.
	inline void setY(const Vector3& vec)
	{
		M[1][0] = vec.x();
		M[1][1] = vec.y();
		M[1][2] = vec.z();
	}

	//! Set the column Z of the matrix.
	inline void setZ(const Vector3& vec)
	{
		M[2][0] = vec.x();
		M[2][1] = vec.y();
		M[2][2] = vec.z();
	}

	//! Rotate from X axis of alpha radian.
	void rotateX(const Float alpha);
	//! Rotate from Y axis of alpha radian.
	void rotateY(const Float alpha);
	//! Rotate from Z axis of alpha radian.
	void rotateZ(const Float alpha);
	//! Rotate from an axis of alpha radian.
	inline void rotate(UInt32 axe, Float alpha)
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
	inline void rotateAbs(UInt32 axe, Float alpha)
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

	//! Scale on the three axis.
	void scale(const Vector3 &V);

    //! Scale a specific axis.
	void scale(UInt32 axe, Float scale);

	//! Uniform scale.
	void scale(Float scale);

	//! Return an uniform scale of the matrix.
	Matrix3 scaleTo(Float scale) const;

	//! Return the inverse of the matrix.
	Matrix3 invert() const;

	//! Return the 3 eigenvectors.
	void eigenVector(Vector3 &R, Vector3 &S, Vector3 &T) const;

	//! Return the 3 eigenvalues.
	void eigenValue(Float & e1, Float & e2, Float & e3) const;

	//! Compute eigens value and eigens vector for a symmetric matrix
	void computeSymmetricEigenSystem(Matrix3 &eigenVectors, Vector3 &eigenValues);

	//! Convert to a string.
	operator String() const;

	// Serialization
    Bool writeToFile(OutStream& os) const;
    Bool readFromFile(InStream& is);

protected:

	Float M[3][3];		//!< Matrix data.

    static Matrix3 *ms_identity;
    static Matrix3 *ms_null;

	static void init();
	static void quit();
};

} // namespace o3d

#endif // _O3D_MATRIX3_H

