/**
 * @file squaregizmo.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-04-15
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/squaregizmo.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/primitive/primitivemanager.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(SquareGizmo, ENGINE_GIZMO_SQUARE, Gizmo)

// Get the drawing type
UInt32 SquareGizmo::getDrawType() const { return Scene::DRAW_SQUARE_GIZMO; }

/*---------------------------------------------------------------------------------------
  Constructor
---------------------------------------------------------------------------------------*/
SquareGizmo::SquareGizmo(
	BaseObject *pParent,
	Vector3 vX,
	Vector3 vY,
	Vector3 vZ,
	Vector3 dim) :
		Gizmo(pParent,vX,vY,vZ,dim)
{
}

/*---------------------------------------------------------------------------------------
  test if a point is inside the gizmo
---------------------------------------------------------------------------------------*/
Bool SquareGizmo::isInside(Vector3 pos)
{
	return False;
}

/*---------------------------------------------------------------------------------------
  test if the gizmo is inside the frustrum view
---------------------------------------------------------------------------------------*/
Bool SquareGizmo::frustrumClip()
{
	return False;
}

/*---------------------------------------------------------------------------------------
  draw the gizmo
---------------------------------------------------------------------------------------*/
void SquareGizmo::draw(const DrawInfo &drawInfo)
{
    if (!getActivity() || !getVisibility()) {
		return;
    }

    if ((drawInfo.pass == DrawInfo::AMBIENT_PASS/*SymbolicPass*/) && getScene()->getDrawObject(Scene::DRAW_SQUARE_GIZMO)) {
		setUpModelView();

        PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access(drawInfo);

        if (getScene()->getDrawObject(Scene::DRAW_LOCAL_AXIS)) {
			primitive->drawLocalAxis();
        }

		primitive->setColor(1.f,1.f,0.f);
		primitive->beginDraw(P_LINE_LOOP);
			primitive->addVertex(-m_dim[X],-m_dim[Y],0);
			primitive->addVertex(-m_dim[X], m_dim[Y],0);
			primitive->addVertex( m_dim[X], m_dim[Y],0);
			primitive->addVertex( m_dim[X],-m_dim[Y],0);
		primitive->endDraw();
	}
}
