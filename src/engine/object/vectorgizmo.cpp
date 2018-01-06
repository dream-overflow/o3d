/**
 * @file vectorgizmo.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-04-15
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/vectorgizmo.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/primitive/primitivemanager.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(VectorGizmo, ENGINE_GIZMO_VECTOR, Gizmo)

// Get the drawing type
UInt32 VectorGizmo::getDrawType() const { return Scene::DRAW_VECTOR_GIZMO; }

// constructor
VectorGizmo::VectorGizmo(BaseObject *pParent, Vector3 vZ, Float dim) :
	Gizmo(pParent)
{
	m_dim[X] = 0.f,
	m_dim[Y] = 0.f;
	m_dim[Z] = dim;

	m_axis.setZ(vZ);
}

// draw the gizmo
void VectorGizmo::draw(const DrawInfo &drawInfo)
{
	if (!getActivity() || !getVisibility())
		return;

	if ((drawInfo.pass == DrawInfo::AMBIENT_PASS/*SymbolicPass*/) && getScene()->getDrawObject(Scene::DRAW_VECTOR_GIZMO))
	{
		setUpModelView();

		PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access();

		if (getScene()->getDrawObject(Scene::DRAW_LOCAL_AXIS))
			primitive->drawLocalAxis();

		primitive->setColor(1.f,1.f,0.f);

		primitive->beginDraw(P_LINES);
			primitive->addVertex(0,0,0);
			primitive->addVertex(0,0,m_dim[Z]);
		primitive->endDraw();
	}
}

