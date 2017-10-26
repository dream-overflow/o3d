/**
 * @file gizmo.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/gizmo.h"

#include "o3d/core/debug.h"
#include "o3d/engine/context.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/hierarchy/node.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(Gizmo, ENGINE_GIZMO, SceneObject)

/*---------------------------------------------------------------------------------------
  Constructor
---------------------------------------------------------------------------------------*/
Gizmo::Gizmo(
	BaseObject *pParent,
	Vector3 vX,
	Vector3 vY,
	Vector3 vZ,
	Vector3 dim) :
		SceneObject(pParent),
		m_dim(dim)
{
	m_dim = dim;

	setAxe(vX,vY,vZ);

	m_influenceRadius = 2 * m_dim[X];
	m_metaIntensity = 0.f;

//	autoSetMetaIntensity();

	setUpdatable(True);
	setDrawable(True);
	setPickable(True);
	setAnimatable(True);
	setShadable(False);
}

/*---------------------------------------------------------------------------------------
  set the axes of the gizmo
---------------------------------------------------------------------------------------*/
void Gizmo::setAxe(Vector3 vX,Vector3 vY,Vector3 vZ)
{
	m_axis.setX(!vX);
	m_axis.setY(!vY);
	m_axis.setZ(!vZ);
}

void Gizmo::setAxis(UInt32 axis, Vector3 v)
{
	switch (axis)
	{
	case X:
		m_axis.setX(!v);
		break;
	case Y:
		m_axis.setY(!v);
		break;
	case Z:
		m_axis.setZ(!v);
		break;
	default:
		break;
	}
}

Vector3 Gizmo::getAxis(UInt32 axis)
{
	switch (axis)
	{
	case X:
		return !m_axis.getX();
	case Y:
		return !m_axis.getY();
	case Z:
		return !m_axis.getZ();
	default:
		return Vector3();
	}
}

/*---------------------------------------------------------------------------------------
  test if a point is inside the gizmo
---------------------------------------------------------------------------------------*/
Bool Gizmo::isInside(Vector3 pos)
{
	return False;
}

/*---------------------------------------------------------------------------------------
  test if the gizmo is inside the frustrum view
---------------------------------------------------------------------------------------*/
Bool Gizmo::frustrumClip()
{
	return False;
}

/*---------------------------------------------------------------------------------------
  serialisation
---------------------------------------------------------------------------------------*/
Bool Gizmo::writeToFile(OutStream &os)
{
    if (!SceneObject::writeToFile(os))
		return False;

    os   << m_axis
		 << m_dim
		 << m_metaIntensity
		 << m_influenceRadius;

    return True;
}

Bool Gizmo::readFromFile(InStream &is)
{
    if (!SceneObject::readFromFile(is))
		return False;

    is   >> m_axis
		 >> m_dim
		 >> m_metaIntensity
		 >> m_influenceRadius;

    return True;
}

