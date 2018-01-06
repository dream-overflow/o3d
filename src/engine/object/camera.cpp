/**
 * @file camera.cpp
 * @brief Implementation of Camera.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-02-13--
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/camera.h"

#include "o3d/core/debug.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/context.h"
#include "o3d/engine/object/primitivemanager.h"

#include "o3d/engine/hierarchy/node.h"

#include <math.h>

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Camera, ENGINE_CAMERA, SceneObject)

// contructor
Camera::Camera(BaseObject *parent) :
	SceneObject(parent),
	m_left(0),
	m_right(640),
	m_bottom(480),
	m_top(0),
	m_coef(1.33333f),
    m_fov(60.f),
	m_znear(0.5f),
	m_zfar(1000.0f),
	m_ortho(False),
	m_orthoByFov(False)
{
	setUpdatable(True);
	setDrawable(True);
	setPickable(True);
	setAnimatable(True);
	setShadable(False);
}

//! copy contructor
Camera::Camera(const Camera &dup) :
	SceneObject(dup),
	m_left(dup.m_left),
	m_right(dup.m_right),
	m_bottom(dup.m_bottom),
	m_top(dup.m_top),
	m_coef(dup.m_coef),
	m_fov(dup.m_fov),
	m_znear(dup.m_znear),
	m_zfar(dup.m_zfar),
	m_ortho(dup.m_ortho),
	m_orthoByFov(dup.m_orthoByFov)
{
}

// Get the drawing type
UInt32 Camera::getDrawType() const { return Scene::DRAW_CAMERA; }

// duplicate
Camera& Camera::operator= (const Camera &dup)
{
	SceneObject::operator= (dup);

	m_left = dup.m_left;
	m_right = dup.m_right;
	m_bottom = dup.m_bottom;
	m_top = dup.m_top;
	m_coef = dup.m_coef;
	m_znear = dup.m_znear;
	m_zfar = dup.m_zfar;
	m_fov = dup.m_fov;
	m_ortho = dup.m_ortho;
	m_orthoByFov = dup.m_orthoByFov;

	return *this;
}

// compute a perspective projection matrix
void Camera::computePerspective()
{
	Float fovt = tanf((o3d::toRadian(m_fov)) * 0.5f);

	m_projectionMatrix.identity();
                    // R,C,   DATA
	m_projectionMatrix(0,0) = 1 / (m_coef * fovt);
	m_projectionMatrix(1,1) = 1 / fovt;
	m_projectionMatrix(2,2) = - ((m_zfar + m_znear) / (m_zfar - m_znear));
	m_projectionMatrix(3,2) = -1;
	m_projectionMatrix(2,3) = - ((2 * m_zfar * m_znear) / (m_zfar - m_znear));
	m_projectionMatrix(3,3) = 0.0f;

	m_ortho = False;
	m_orthoByFov = False;

    m_capacities.enable(STATE_PROJ_UPDATED);
    m_capacities.enable(STATE_CAMERA_CHANGED);
}

// orthonormal camera
void Camera::computeOrtho()
{
	m_projectionMatrix.identity();
                    // R,C,   DATA
	m_projectionMatrix(0,0) = (Float)(2.0 / (m_right - m_left));
	m_projectionMatrix(1,1) = (Float)(2.0 / (m_top - m_bottom));
	m_projectionMatrix(2,2) = (Float)(-2.0 / (m_zfar - m_znear));
	m_projectionMatrix(0,3) = - (Float)(((Double)m_right + m_left) / ((Double)m_right - m_left));
	m_projectionMatrix(1,3) = - (Float)(((Double)m_top + m_bottom) / ((Double)m_top - m_bottom));
	m_projectionMatrix(2,3) = - (Float)(((Double)m_zfar + m_znear) / ((Double)m_zfar - m_znear));

	m_ortho = True;
	m_orthoByFov = False;

    m_capacities.enable(STATE_PROJ_UPDATED);
    m_capacities.enable(STATE_CAMERA_CHANGED);
}

void Camera::computeOrthoByFov()
{
	Float _left = -m_fov/2.f;
	Float _right = m_fov/2.f;
	Float _top = -m_fov/m_coef/2.f;
	Float _bottom = m_fov/m_coef/2.f;

	m_projectionMatrix.identity();
                    // R,C,   DATA
	m_projectionMatrix(0,0) = 2.0f / (_right - _left);
	m_projectionMatrix(1,1) = 2.0f / (_top - _bottom);
	m_projectionMatrix(2,2) = 2.0f / (m_zfar - m_znear);
	m_projectionMatrix(0,3) = - ((_right + _left) / (_right - _left));
	m_projectionMatrix(1,3) = - ((_top + _bottom) / (_top - _bottom));
	m_projectionMatrix(2,3) = - ((m_zfar + m_znear) / (m_zfar - m_znear));

	m_ortho = True;
	m_orthoByFov = True;

    m_capacities.enable(STATE_PROJ_UPDATED);
    m_capacities.enable(STATE_CAMERA_CHANGED);
}

void Camera::updateMatrix()
{
	if (m_node)
        m_modelviewMatrix = m_node->getAbsoluteMatrix().invert();//Std();
	else
		m_modelviewMatrix.identity();
}

// define the projection matrix
void Camera::setProjectionMatrix()
{
	getScene()->getContext()->projection().set(m_projectionMatrix);
}

// mult current projection matrix by this camera projection matrix
void Camera::multProjectionMatrix()
{
	getScene()->getContext()->projection().mult(m_projectionMatrix);
}

// recompute the projection matrix
void Camera::reCompute()
{
	if (m_ortho)
	{
		if (m_orthoByFov)
			computeOrthoByFov();
		else
			computeOrtho();
	}
	else
	{
		computePerspective();
	}
}

void Camera::update()
{
	clearUpdated();

	if (m_capacities.getBit(STATE_PROJ_UPDATED))
	{
        m_capacities.disable(STATE_PROJ_UPDATED);
		setUpdated();

        // camera change can need update at draw stage for others objets
        m_capacities.enable(STATE_CAMERA_CHANGED);
	}

	if (m_node && m_node->hasUpdated())
	{
        m_modelviewMatrix = m_node->getAbsoluteMatrix().invert();//Std();
		setUpdated();

        // camera change can need update at draw stage for others objets
        m_capacities.enable(STATE_CAMERA_CHANGED);
    }
}

// Setup the modelview matrix to OpenGL
void Camera::setUpModelView()
{
    O3D_ASSERT(getScene()->getActiveCamera() != nullptr);

	if (getScene()->getActiveCamera() == this)
		getScene()->getContext()->modelView().identity();
	else
		SceneObject::setUpModelView();
}

void Camera::draw(const DrawInfo &drawInfo)
{
	if (!m_ortho)
		return;

	if ((drawInfo.pass == DrawInfo::AMBIENT_PASS/*SymbolicPass*/) && getScene()->getDrawObject(Scene::DRAW_CAMERA))
	{
		setUpModelView();

		PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access();

		if (getScene()->getDrawObject(Scene::DRAW_LOCAL_AXIS))
			primitive->drawLocalAxis();

		primitive->setColor(0.8f,0.8f,1.f);

		Float h = m_zfar * tanf(o3d::toRadian(m_fov)*0.5f);
		Float w = h * m_coef;

		// draw the FOV

		primitive->beginDraw(P_LINES);
			primitive->addVertex(Vector3(0.f,0.f,0.f));
			primitive->addVertex(Vector3(w, h,-m_zfar));
			primitive->addVertex(Vector3(0.f,0.f,0.f));
			primitive->addVertex(Vector3( w,-h,-m_zfar));
			primitive->addVertex(Vector3(0.f,0.f,0.f));
			primitive->addVertex(Vector3(-w,-h,-m_zfar));
			primitive->addVertex(Vector3(0.f,0.f,0.f));
			primitive->addVertex(Vector3(-w, h,-m_zfar));
			primitive->addVertex(Vector3(0.f,0.f,0.f));
			primitive->addVertex(Vector3(0,0,-m_zfar));
		primitive->endDraw();

		primitive->beginDraw(P_LINE_STRIP);
			primitive->addVertex(Vector3( w, h,-m_zfar));
			primitive->addVertex(Vector3( w,-h,-m_zfar));
			primitive->addVertex(Vector3(-w,-h,-m_zfar));
			primitive->addVertex(Vector3(-w, h,-m_zfar));
			primitive->addVertex(Vector3( w, h,-m_zfar));
		primitive->endDraw();

		Float fact = m_znear / m_zfar;

		primitive->beginDraw(P_LINE_STRIP);
			primitive->addVertex(Vector3( w*fact, h*fact,-m_znear));
			primitive->addVertex(Vector3( w*fact,-h*fact,-m_znear));
			primitive->addVertex(Vector3(-w*fact,-h*fact,-m_znear));
			primitive->addVertex(Vector3(-w*fact, h*fact,-m_znear));
			primitive->addVertex(Vector3( w*fact, h*fact,-m_znear));
		primitive->endDraw();

		primitive->modelView().translate(Vector3(0,0,-m_zfar));
		primitive->draw(PrimitiveManager::SOLID_SPHERE1, Vector3(0.2f,0.2f,0.2f));
	}
}

// serialization
Bool Camera::writeToFile(OutStream &os)
{
    if (!SceneObject::writeToFile(os))
		return False;

    os   << m_ortho
		 << m_orthoByFov;

	if (m_ortho && !m_orthoByFov)
	{
        os   << m_left
			 << m_right
			 << m_bottom
			 << m_top;
	}

    os   << m_coef
		 << m_fov
		 << m_znear
		 << m_zfar;

	return True;
}

Bool Camera::readFromFile(InStream &is)
{
    if (!SceneObject::readFromFile(is))
		return False;

    is   >> m_ortho
		 >> m_orthoByFov;

	if (m_ortho && !m_orthoByFov)
	{
        is   >> m_left
			 >> m_right
			 >> m_bottom
			 >> m_top;
	}

    is   >> m_coef
		 >> m_fov
		 >> m_znear
		 >> m_zfar;

	reCompute();
	return True;
}
