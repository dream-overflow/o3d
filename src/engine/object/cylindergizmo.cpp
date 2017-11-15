/**
 * @file cylindergizmo.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-04-15
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/cylindergizmo.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/object/primitivemanager.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(CylinderGizmo, ENGINE_GIZMO_CYLINDER, Gizmo)

// Constructor
CylinderGizmo::CylinderGizmo(
	BaseObject *pParent,
	Vector3 vX,
	Vector3 vY,
	Vector3 vZ,
	Vector3 dim) :
		Gizmo(pParent,vX,vY,vZ,dim)
{
}

// Get the drawing type
UInt32 CylinderGizmo::getDrawType() const { return Scene::DRAW_CYLINDER_GYZMO; }

CylinderGizmo::~CylinderGizmo(void)
{
}

// test if a point is inside the gizmo
Bool CylinderGizmo::isInside(Vector3 pos)
{
	return False;
}

// test if the gizmo is inside the frustrum view
Bool CylinderGizmo::frustrumClip()
{
	return False;
}

// draw the gizmo
void CylinderGizmo::draw(const DrawInfo &drawInfo)
{
	if (!getActivity() || !getVisibility())
		return;

	if ((drawInfo.pass == DrawInfo::AMBIENT_PASS/*SymbolicPass*/) && getScene()->getDrawObject(Scene::DRAW_CYLINDER_GYZMO))
	{
		setUpModelView();

		PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access();

		if (getScene()->getDrawObject(Scene::DRAW_LOCAL_AXIS))
			primitive->drawLocalAxis();

		primitive->setColor(1.f,1.f,0.f);
		primitive->draw(PrimitiveManager::WIRE_CYLINDER1, Vector3(m_dim[X],m_dim[Y],2.f*m_dim[Z]));
	}
}

// compute the value to add to the cell
Float CylinderGizmo::metaCellValue(Vector3 pos)
{
	return 0.f;
}

