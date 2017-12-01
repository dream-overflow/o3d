/**
 * @file matrix.h
 * @brief Engine matrix wrapper and manager
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-01-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MATRIX_H
#define _O3D_MATRIX_H

#include "o3d/core/box.h"
#include "o3d/core/matrix4.h"
#include "o3d/core/quaternion.h"
#include "o3d/core/memorydbg.h"

#include <stack>

namespace o3d {

class Matrix;
class BSphere;

//---------------------------------------------------------------------------------------
//! @class MatrixObserver
//-------------------------------------------------------------------------------------
//! Matrix common observer.
//---------------------------------------------------------------------------------------
class O3D_API MatrixObserver
{
public:

	//! Called when the matrix change.
	virtual void updateMatrix(const Matrix *matrix) = 0;
};

//---------------------------------------------------------------------------------------
//! @class Matrix
//-------------------------------------------------------------------------------------
//! Matrix common class and tools.
//---------------------------------------------------------------------------------------
class O3D_API Matrix
{
public:

	//! Default constructor.
	Matrix() :
        m_observer(nullptr)
	{
	}

	//! Project a point from world space to window space.
	//! The origin is supposed to be located a the bottom left of the screen.
	//! @param projection Projection matrix.
	//! @param modelview ModelView matrix.
	//! @param viewport Viewport box.
	//! @param worldPos World space coordinate.
	//! @return Window space coordinate.
	static Vector3 projectPoint(
			const Matrix4 &projection,
			const Matrix4 &modelview,
			const Box2i &viewport,
			const Vector3 &worldPos);

	//! Unproject a point from window space to world space.
	//! @param projection Projection matrix.
	//! @param modelview ModelView matrix.
	//! @param viewport Viewport box.
	//! @param windowPos Window space coordinate.
	//! @return World space coordinate.
	static Vector3 unprojectPoint(
			const Matrix4 &projection,
			const Matrix4 &modelview,
			const Box2i &viewport,
			const Vector3 &windowPos);

	//! Unproject a point from window space to world space. Same as previous but
	//! take a depth range (znear, zfar) and return as vector4.
	//! @param projection Projection matrix.
	//! @param modelview ModelView matrix.
	//! @param viewport Viewport box.
	//! @param windowPos Window space coordinate.
	//! @param clipW W component of the point to un-project.
	//! @param zNear Z near.
	//! @param zFar Z far.
	//! @return World space coordinate.
	static Vector4 unprojectPoint(
			const Matrix4 &projection,
			const Matrix4 &modelview,
			const Box2i &viewport,
			const Vector3 &windowPos,
			const Float clipW,
			const Float zNear,
			const Float zFar);

	//! Project a bounding from world space to window space
	//! The origin is supposed to be located a the bottom left of the screen.
	//! @param projection Projection Matrix
	//! @param modelview ModelView matrix. It is the inverse of the one that  contains
	//!        the unit vectors of the eye base in its columns. In other word, the modelview
	//!        matrix transforms a world position into a position relative to the camera.
	//! @param sphere Bounding sphere
	//! @param projected Defines the rectangle. Returns a null box (width = 0) if something went wrong.
	static Box2i projectBoundingIntoRectangle(
			const Matrix4 &projection,
			const Matrix4 &modelview,
			const Box2i &viewport,
			const BSphere & sphere);

	//! Add an observer.
	void addObserver(MatrixObserver *observer);

	//! Remove an observer.
	void removeObserver(MatrixObserver *observer);

	//! Notify all observers.
	inline void notify() const
	{
		m_observer->updateMatrix(this);
	}

protected:

	MatrixObserver *m_observer;
};

//---------------------------------------------------------------------------------------
//! @class ModelViewMatrix
//-------------------------------------------------------------------------------------
//! Modelview matrix wrapper and manager. An instance is provided by the context.
//! The modelview matrix mode is always the default.
//---------------------------------------------------------------------------------------
class O3D_API ModelViewMatrix : public Matrix
{

public:

	//! Default constructor.
	ModelViewMatrix();

	//! Define the current modelview matrix as identity.
	inline void identity()
	{
		m_matrix.identity();
		notify();
	}

	//! Define the current modelview matrix.
	inline void set(const Matrix4 &m)
	{
		m_matrix = m;
		notify();
	}

	//! Get the current ModelView matrix.
	inline const Matrix4& get() const { return m_matrix; }

	//! Multiply M to the current modelview matrix.
	inline void mult(const Matrix4 &m)
	{
		m_matrix *= m;
		notify();
	}

	//! Cancel the modelview rotation (used for billboard).
	void unRotate();

	//! Push the current modelview matrix.
	void push();

	//! Pop to the current modelview matrix.
	void pop();

	//! Get number of pushed matrix
	inline UInt32 getStackSize() { return static_cast<UInt32>(m_matrixStack.size()); }

	//! Translate the current modelview matrix.
	inline void translate(const Vector3 &t)
	{
		m_temp.identity();
		m_temp.translate(t);
		m_matrix *= m_temp;

		notify();
	}

	//! Rotate on X axis the current modelview matrix.
	inline void rotateX(Float alpha)
	{
		m_temp.identity();
		m_temp.rotateX(alpha);
		m_matrix *= m_temp;

		notify();
	}

	//! Rotate on Y axis the current modelview matrix.
	inline void rotateY(Float alpha)
	{
		m_temp.identity();
		m_temp.rotateY(alpha);
		m_matrix *= m_temp;

		notify();
	}

	//! Rotate on Z axis the current modelview matrix.
	inline void rotateZ(Float alpha)
	{
		m_temp.identity();
		m_temp.rotateZ(alpha);
		m_matrix *= m_temp;

		notify();
	}

	//! Rotate the current modelview matrix.
	inline void rotate(const Quaternion &q)
	{
		m_matrix *= q.toMatrix4();
		notify();
	}

	//! Scale the current modelview matrix.
	inline void scale(const Vector3 &t)
	{
		m_temp.identity();
		m_temp.scale(t);
		m_matrix *= m_temp;

		notify();
	}

private:

	Matrix4 m_temp;                     //!< Temporary matrix.

	Matrix4 m_matrix;                   //!< Current matrix.
	std::stack<Matrix4> m_matrixStack;  //!< Extra list of pushed matrix.
};

//---------------------------------------------------------------------------------------
//! @class ProjectionMatrix
//-------------------------------------------------------------------------------------
//! Projection matrix wrapper and manager.
//! An instance is provided by the context.
//! The projection matrix operation let the modelview matrix of the context as default
//! matrix mode.
//---------------------------------------------------------------------------------------
class O3D_API ProjectionMatrix : public Matrix
{
public:

	//! Define the current Projection matrix
	inline void set(const Matrix4 &m)
	{
		m_matrix = m;
		notify();
	}

	//! Get the current Projection matrix (read only).
	inline const Matrix4& get() const { return m_matrix; }

	//! Multiply M to the current projection matrix
	inline void mult(const Matrix4 &m)
	{
		m_matrix *= m;
		notify();
	}

	//! Compute a region matrix according to a center position and size.
	//! @param center Centered position into the view-port.
	//! @param size Size of the region into the view-port.
	//! @param viewport View-port box.
	//! @return A new region matrix.
	static Matrix4 pickMatrix(
			const Vector2f &center,
			const Vector2f &size,
			const Box2i &viewport);

private:

	Matrix4 m_matrix;       //!< Current matrix.
};

} // namespace o3d

#endif // _O3D_MATRIX_H

