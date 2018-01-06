/**
 * @file spheregizmo.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/spheregizmo.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/primitive/primitivemanager.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(SphereGizmo, ENGINE_GIZMO_SPHERE, Gizmo)

/*---------------------------------------------------------------------------------------
  Constructor
---------------------------------------------------------------------------------------*/
SphereGizmo::SphereGizmo(
	BaseObject *pParent,
	Vector3 vX,
	Vector3 vY,
	Vector3 vZ,
	Vector3 dim) :
	Gizmo(pParent,vX,vY,vZ,dim)
{
}

// Get the drawing type
UInt32 SphereGizmo::getDrawType() const { return Scene::DRAW_SPHERE_GIZMO; }

/*---------------------------------------------------------------------------------------
  test if a point is inside the gizmo
---------------------------------------------------------------------------------------*/
Bool SphereGizmo::isInside(Vector3 pos)
{
	return False;
}

/*---------------------------------------------------------------------------------------
  test if the gizmo is inside the frustrum view
---------------------------------------------------------------------------------------*/
Bool SphereGizmo::frustrumClip()
{
	return False;
}

/*---------------------------------------------------------------------------------------
  draw the gizmo
---------------------------------------------------------------------------------------*/
void SphereGizmo::draw(const DrawInfo &drawInfo)
{
	if (!getActivity() || !getVisibility())
		return;

	if ((drawInfo.pass == DrawInfo::AMBIENT_PASS/*SymbolicPass*/) && getScene()->getDrawObject(Scene::DRAW_SPHERE_GIZMO))
	{
		setUpModelView();

		PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access();

		if (getScene()->getDrawObject(Scene::DRAW_LOCAL_AXIS))
			primitive->drawLocalAxis();

		primitive->setColor(1.f,1.f,0.f);
		primitive->draw(PrimitiveManager::WIRE_SPHERE2, m_dim);
	}
}

