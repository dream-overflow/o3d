/**
 * @file camera.h
 * @brief Camera object that offer a projection matrix support.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-02-13
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CAMERA_H
#define _O3D_CAMERA_H

#include "../scene/sceneobject.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

/**
 * @brief Camera object that offer a projection matrix support. Can be ortho or perspective.
 */
class O3D_API Camera : public SceneObject
{
public:

	O3D_DECLARE_CLASS(Camera)

	//! Default contructor. fov = 60
	Camera(BaseObject *parent);

	//! Copy contructor
	Camera(const Camera &dup);

	//! Assign operator
	Camera& operator= (const Camera &dup);

	//! Get the drawing type
	virtual UInt32 getDrawType() const;


	//-----------------------------------------------------------------------------------
	// Projection
	//-----------------------------------------------------------------------------------

	//! Set screen size ratio (width/height) for perspective and FOV ortho camera
	inline void setRatio(Float coef) { m_coef = coef; }
	//! Get screen size ratio (width/height) for perspective and FOV ortho camera
	inline Float getRatio()const { return m_coef; }

	//! Set field of view for FOV ortho and perspective camera
	//! @param fov The field of view in degree. The half angle of the view cone, ie 45 degree is common
	inline void setFov(Float fov) { m_fov = fov; }
	//! Get field of view angle.
	inline Float getFov()const { return m_fov; }

	//! Set Z near clipping distance
	inline void setZnear(Float znear) { m_znear = znear; }
	//! Set Z far clipping distance
	inline void setZfar(Float zfar) { m_zfar = zfar; }
	//! Get Z near clipping distance
	inline Float getZnear()const { return m_znear; }
	//! Get Z far clipping distance
	inline Float getZfar()const { return m_zfar; }

	//! Set ortho 2d clipping planes
	inline void setOrtho(Float left,Float right,Float bottom,Float top)
	{
		m_left = left;
		m_right = right;
		m_top = top;
		m_bottom = bottom;
		m_ortho = True;
	}

	//! Get the left clipping plane for ortho 2d camera
	inline Float getLeft()const { return m_left; }
	//! Get the right clipping plane for ortho 2d camera
	inline Float getRight()const { return m_right; }
	//! Get the top clipping plane for ortho 2d camera
	inline Float getTop()const { return m_top; }
	//! Get the bottom clipping plane for ortho 2d camera
	inline Float getBottom()const { return m_bottom; }

	//! Set the left clipping plane for ortho 2d camera
	inline void setLeft(Float left) { m_left = left; }
	//! Set the right clipping plane for ortho 2d camera
	inline void setRight(Float right) { m_right = right; }
	//! Set the top clipping plane for ortho 2d camera
	inline void setTop(Float top) { m_top = top; }
	//! Set the bottom clipping plane for ortho 2d camera
	inline void setBottom(Float bottom) { m_bottom = bottom; }

	//! is it an orthonormal camera
	inline Bool isOrtho()const { return m_ortho; }
	//! is it an orthonormal camera computed with a FOV value
	inline Bool isOrthoByFov()const { return m_orthoByFov; }
	//! is it a perspective camera
	inline Bool isPerspective()const { return !m_ortho; }

	//! compute projection matrix: perspective mode (equ to gluPerspective) param[fov,znear,zfar,coef]
	void computePerspective();
	//! compute projection matrix: orthonormal mode (equ to glOrtho) param[left,rigth,bottom,top]
	void computeOrtho();
	//! compute projection matrix: orthonormal mode with FOV (equ to glOrtho) param[fov,znear,zfar,coef]
	void computeOrthoByFov();

	//! recompute the projection matrix with current values
	void reCompute();

	//! Get the projection matrix.
	const Matrix4& getProjectionMatrix() const { return m_projectionMatrix; }


	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	//! Set projection matrix to OpenGL
	void setProjectionMatrix();

	//! Mult current projection matrix by this camera projection matrix
	void multProjectionMatrix();

	//! Return the invert of the absolute camera matrix (modelview).
	inline const Matrix4& getModelviewMatrix() const { return m_modelviewMatrix; }

	//! Update transforms
	virtual void update();

	virtual void draw(const DrawInfo &drawInfo);

	virtual void setUpModelView();

    //! Returns true if the camera changed at a previous update.
    Bool isCameraChanged() const { return m_capacities.getBit(STATE_CAMERA_CHANGED); }
    //! Clear the camera changed flag. Must be done by the drawer.
    void clearCameraChanged() { m_capacities.disable(STATE_CAMERA_CHANGED); }

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

protected:

	static const UInt32 STATE_PROJ_UPDATED = CAPS_SCENE_OBJECT_NEXT;
    static const UInt32 STATE_CAMERA_CHANGED = STATE_PROJ_UPDATED+1;

	Float m_left,m_right;   //!< Orthonormal 2d projection coordinates
	Float m_bottom,m_top;   //!< Orthonormal 2d projection coordinates

	Float m_coef;           //!< reshape width/height
	Float m_fov;            //!< half field of view in degree (60 by default)

	Float m_znear,m_zfar;   //!< Depth clipping plane

	Bool m_ortho;           //!< Orthonormal projection matrix
	Bool m_orthoByFov;      //!< Orthonormal projection matrix using fov

	Matrix4 m_projectionMatrix;   //!< 3D=>2D Projection matrix.
	Matrix4 m_modelviewMatrix;    //!< Invert of the absolute matrix (modelview).

	//! Update convenience matrices. Need to be called at each modification of projection
	//! or absolute matrix (on update pass).
	void updateMatrix();
};

} // namespace o3d

#endif // _O3D_CAMERA_H
