/**
 * @file cubegizmo.cpp
 * @brief Implementation of CubeGizmo.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-04-15
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/cubegizmo.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/primitive/primitivemanager.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(CubeGizmo, ENGINE_GIZMO_CUBE, Gizmo)

// Constructor
CubeGizmo::CubeGizmo(
	BaseObject *pParent,
	Vector3 vX,
	Vector3 vY,
	Vector3 vZ,
	Vector3 dim) :
		Gizmo(pParent,vX,vY,vZ,dim)
{
}

// Get the drawing type
UInt32 CubeGizmo::getDrawType() const { return Scene::DRAW_CUBE_GIZMO; }

// test if a point is inside the gizmo
Bool CubeGizmo::isInside(Vector3 pos)
{
	// TODO
	return False;
}

// test if the gizmo is inside the frustrum view
Bool CubeGizmo::frustrumClip()
{
	// TODO
	return False;
}

// draw the gizmo
void CubeGizmo::draw(const DrawInfo &drawInfo)
{
	if (!getActivity() || !getVisibility())
		return;

	if ((drawInfo.pass == DrawInfo::AMBIENT_PASS/*SymbolicPass*/) && getScene()->getDrawObject(Scene::DRAW_CUBE_GIZMO))
	{
		setUpModelView();

		PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access();

		if (getScene()->getDrawObject(Scene::DRAW_LOCAL_AXIS))
			primitive->drawLocalAxis();

		primitive->setColor(1.f,1.f,0.f);
		primitive->draw(PrimitiveManager::WIRE_CUBE1, m_dim*2.f);
	}
}

