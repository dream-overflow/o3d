/**
 * @file matrix.cpp
 * @brief Implementation of GLMatrix.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-01-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/matrix.h"

#include "o3d/engine/context.h"
#include "o3d/geom/bsphere.h"

using namespace o3d;

//---------------------------------------------------------------------------------------
// Matrix
//---------------------------------------------------------------------------------------

Vector3 Matrix::projectPoint(
		const Matrix4 &projection,
		const Matrix4 &modelview,
		const Box2i &viewport,
		const Vector3 &worldPos)
{
	Vector4 projPos = projection * modelview * Vector4(worldPos, 1.f);

	if (projPos.w() == 0.f)
		return Vector3();

	Float len = 1 / projPos.w();

	projPos.x() *= len;
	projPos.y() *= len;
	projPos.z() *= len;

	return Vector3(
			viewport.x() + viewport.width() * (projPos.x() + 1.f) / 2.f,
			viewport.y() + viewport.height() * (projPos.y() + 1.f) / 2.f,
			(projPos.z() + 1.f) / 2.f);
}

Vector3 Matrix::unprojectPoint(
		const Matrix4 &projection,
		const Matrix4 &modelview,
		const Box2i &viewport,
		const Vector3 &windowPos)
{
	Matrix4 invProjMdl = (projection * modelview).invertStd();

	// normalize to [-1 1]
	Vector4 objectPos = Vector4(
			((2.f * (windowPos.x() - viewport.x())) / viewport.width()) - 1.f,
			((2.f * (windowPos.y() - viewport.y())) / viewport.height()) - 1.f,
			2.f * windowPos.z() - 1.f,
			1.0f);

	// un-project
	objectPos = invProjMdl * objectPos;

	if (objectPos.w() == 0.f)
		return Vector3();

	Float scale = 1.f / objectPos.w();

	// scale by w
	return Vector3(
			objectPos.x() * scale,
			objectPos.y() * scale,
			objectPos.z() * scale);
}

// Un-project a point from window space to world space. Same as previous but
// take a depth range (znear, zfar) and return as vector4.
Vector4 Matrix::unprojectPoint(
		const Matrix4 &projection,
		const Matrix4 &modelview,
		const Box2i &viewport,
		const Vector3 &windowPos,
		const Float clipW,
		const Float zNear,
		const Float zFar)
{
	Float z = zNear + windowPos.z() * (zFar - zNear);

	Matrix4 invProjMdl = (projection * modelview).invertStd();

	// normalize to [-1 1]
	Vector4 objectPos = Vector4(
			((2.f * (windowPos.x() - viewport.x())) / viewport.width()) - 1.f,
			((2.f * (windowPos.y() - viewport.y())) / viewport.height()) - 1.f,
			2.f * z - 1.f,
			clipW);

	// un-project
	objectPos = invProjMdl * objectPos;

	if (objectPos.w() == 0.f)
		return Vector4();

	Float scale = 1.f / objectPos.w();

	// scale by w
	return Vector4(
			objectPos.x() * scale,
			objectPos.y() * scale,
			objectPos.z() * scale,
			objectPos.w());
}

// Project a bounding from world space to window space
Box2i Matrix::projectBoundingIntoRectangle(
			const Matrix4 &projection,
			const Matrix4 &modelview,
			const Box2i &viewport,
			const BSphere & sphere)
{
	// Remarque generale:
	// Une camera est definie par un repere u,v,w. La camera pointe vers (-w). De cette facon,
	// u,v sont aussi les vecteurs du repere du plan znear et zfar. (u) va vers la droite et (v) vers
	// le haut.

	// On suppose (pour le moment) que la modelview est orthogonale, cad qu'elle contient
	// des vecteurs normes. Donc pas de transformation a la con, style scale.
	// Avec cette hypothese, le changement de base de modifie pas les longueurs, donc le rayon
	// de la sphere reste identique.
	// On transforme donc uniquement le centre.
	const Vector3 lEyeComponent = modelview * sphere.getCenter();

	Vector4 lRadiusComponent = projection * Vector4(sphere.getRadius(), sphere.getRadius(), lEyeComponent.z(), 1.0f);
	Vector4 lProjComponent = projection * Vector4(lEyeComponent, 1.0f);

	Float len = 1.0f / lProjComponent.w();

	lProjComponent.x() *= len;
	lProjComponent.y() *= len;
	lProjComponent.z() *= len;
	lRadiusComponent.x() *= len;
	lRadiusComponent.y() *= len;

	const Vector2i lCenterOnScreen(
		viewport.x() + (Int32)(floor(viewport.width() * (lProjComponent.x() + 1.0f) / 2.0f + 0.5f)),
		viewport.y() + (Int32)(floor(viewport.height() * (lProjComponent.y() + 1.0f) / 2.0f + 0.5f)));

	const Vector2i lRadiusOnScreen(
		(Int32)(ceil(viewport.width() * lRadiusComponent.x() / 2.0f)),
		(Int32)(ceil(viewport.height() * lRadiusComponent.y() / 2.0f)));

	const Vector2i lTopLeft(
		o3d::clamp<Int32>(lCenterOnScreen.x() - lRadiusOnScreen.x(), viewport.x(), viewport.x() + viewport.width() - 1),
		o3d::clamp<Int32>(lCenterOnScreen.y() - lRadiusOnScreen.y(), viewport.y(), viewport.y() + viewport.height() - 1));

	const Vector2i lBotRight(
		o3d::clamp<Int32>(lCenterOnScreen.x() + lRadiusOnScreen.x() + 1, viewport.x(), viewport.x() + viewport.width() - 1),
		o3d::clamp<Int32>(lCenterOnScreen.y() + lRadiusOnScreen.y() + 1, viewport.y(), viewport.y() + viewport.height() - 1));

	return Box2i(lTopLeft.x(),
					lTopLeft.y(),
					lBotRight.x() - lTopLeft.x(),
					lBotRight.y() - lTopLeft.y());
}

// Add an observer.
void Matrix::addObserver(MatrixObserver *observer)
{
	m_observer = observer;
}

// Remove an observer.
void Matrix::removeObserver(MatrixObserver *observer)
{
	m_observer = NULL;
}

//---------------------------------------------------------------------------------------
// ModelViewMatrix
//---------------------------------------------------------------------------------------

ModelViewMatrix::ModelViewMatrix() :
	Matrix()
{
}

// Unrotate the modelview matrix
void ModelViewMatrix::unRotate()
{
	m_matrix.setRotation(Matrix3());
	notify();
}

// push the current selected matrix
void ModelViewMatrix::push()
{
	m_matrixStack.push(m_matrix);
}

// pop to the current selected matrix
void ModelViewMatrix::pop()
{
	O3D_ASSERT(!m_matrixStack.empty());

	m_matrix = m_matrixStack.top();
	m_matrixStack.pop();
	notify();
}

// Compute a region matrix according to a center position and size.
Matrix4 ProjectionMatrix::pickMatrix(
		const Vector2f &center,
		const Vector2f &size,
		const Box2i &viewport)
{
	Float sx, sy;
	Float tx, ty;

	sx = viewport.width() / size.x();
	sy = viewport.height() / size.y();

	tx = (viewport.width() + 2.f * (viewport.x() - center.x())) / size.x();
	ty = (viewport.height() + 2.f * (viewport.y() - center.y())) / size.y();

	return Matrix4(
			 sx,  0.f,  0.f,   tx,
			0.f,   sy,  0.f,   ty,
			0.f,  0.f,  1.f,  0.f,
			0.f,  0.f,  0.f,  1.f);
}

/*---------------------------------------------------------------------------------------
  Overview on OpenGL zBuffer implementation :
  Let F and N be the zFar and zNear values used by glFrustum or gluPerspective function.

  Let Zb in [-1;1] and Zr (>0) be the depth value and the real distance from the object to the camera.

  Here is the formula used by OpenGL which determines Zb given Zr:

        F + N          2 F N
  Zb = -------  -  ------------
        F - N       Zr (F - N)

  RedBook:
  "After clipping and division by w, depth coordinates range from -1 to 1, corresponding to
  the near and far clipping planes. DepthRange specifies a linear mapping of the normalized
  depth coordinates in this range to windows depth coordinates" in fact [0; 1]

  If you want to transform a depth value [0; 1] into a real distance, you have to apply
  a linear mapping between [0; 1] and [-1; 1]. After that, use the formula previously described
  to compute Zr. Briefly:

          N F
  d = -------------    with d > 0 and z in [0; 1]
       F - z (F-N)

  => d(z = 0) = N
  => d(z = 1) = F

  More details are available on the following site:
  http://www1.cs.columbia.edu/~cs4160/slides/transformations.pdf
---------------------------------------------------------------------------------------*/

