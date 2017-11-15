/**
 * @file pointgizmo.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-04-15
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/pointgizmo.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/object/primitivemanager.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(PointGizmo, ENGINE_GIZMO_POINT, Gizmo)

// Get the drawing type
UInt32 PointGizmo::getDrawType() const { return Scene::DRAW_POINT_GIZMO; }

/*---------------------------------------------------------------------------------------
  Constructor
---------------------------------------------------------------------------------------*/
PointGizmo::PointGizmo(BaseObject *pParent) :
	Gizmo(pParent)
{
	m_dim[X] = m_dim[Y] = m_dim[X] = 0;
}

/*---------------------------------------------------------------------------------------
  test if a point is inside the gizmo
---------------------------------------------------------------------------------------*/
Bool PointGizmo::isInside(Vector3 pos)
{
	return False;
}

/*---------------------------------------------------------------------------------------
  test if the gizmo is inside the frustrum view
---------------------------------------------------------------------------------------*/
Bool PointGizmo::frustrumClip()
{
	return False;
}

/*---------------------------------------------------------------------------------------
  draw the gizmo
---------------------------------------------------------------------------------------*/
void PointGizmo::draw(const DrawInfo &drawInfo)
{
	if (!getActivity() || !getVisibility())
		return;

	if ((drawInfo.pass == DrawInfo::AMBIENT_PASS/*SymbolicPass*/) && getScene()->getDrawObject(Scene::DRAW_POINT_GIZMO))
	{
		setUpModelView();

		PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access();

		if (getScene()->getDrawObject(Scene::DRAW_LOCAL_AXIS))
			primitive->drawLocalAxis();

		primitive->setColor(1.f,1.f,0.f);
		primitive->draw(PrimitiveManager::WIRE_CUBE1, Vector3(0.2f,0.2f,0.2f));
	}
}

