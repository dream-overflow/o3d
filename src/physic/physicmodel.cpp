/**
 * @file physicmodel.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/physic/precompiled.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/physic/physicmodel.h"
#include "o3d/physic/physictype.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(PhysicModel, PHYSIC_MODEL, SceneEntity)

PhysicModel::PhysicModel(
	BaseObject *pParent,
	Bool approximativeModel) :
		SceneEntity(pParent),
		//m_pMesh(this,pMesh),
		m_isApprox(approximativeModel)
{
}

PhysicModel::~PhysicModel()
{
}

Bool PhysicModel::writeToFile(OutStream &os)
{
    os   << PHYSIC_MODEL
		 << m_isApprox;

    if (m_isApprox) {
        os << (UInt32)m_AABBox.size();
        for (IT_AABBoxVector it = m_AABBox.begin() ; it != m_AABBox.end() ; ++it) {
            os << *it;
        }

        os << (UInt32)m_BSphere.size();
        for (IT_BSphereVector it = m_BSphere.begin() ; it != m_BSphere.end() ; ++it) {
            os << *it;
        }

        os << (UInt32)m_Plane.size();
        for (IT_PlaneVector it = m_Plane.begin() ; it != m_Plane.end() ; ++it) {
            os << *it;
        }
    } else {
/*		if (!m_pMesh) {
            O3D_ERROR(O3D_E_InvalidPrecondition("Mesh must be non null"));
			return False;
		}*/

//		os << m_pMesh->getSerializeId();
	}

	return True;
}

Bool PhysicModel::readFromFile(InStream &is)
{
	UInt32 tmp;
    is >> tmp;

    if (tmp != PHYSIC_MODEL) {
        O3D_ERROR(E_InvalidFormat("Invalid physic model token"));
    }

	Bool bOOL = False;
    is >> bOOL;

	// approximative model with bounding volume
    if (bOOL) {
		UInt32 bint;

        is >> bint;
        for (UInt32 i = 0 ; i < bint ; ++i) {
			AABBox bbox;
            is >> bbox;
			m_AABBox.push_back(bbox);
		}

        is >> bint;
        for (UInt32 i = 0 ; i < bint ; ++i) {
			BSphere bsphere;
            is >> bsphere;
			m_BSphere.push_back(bsphere);
		}

        is >> bint;
        for (UInt32 i = 0 ; i < bint ; ++i) {
			Plane plane;
            is >> plane;
			m_Plane.push_back(plane);
		}
    } else {
		Int32 mesh_id;
        is >> mesh_id;

//		if (mesh_id != -1) {
//			m_pMesh = (Mesh*)getScene()->getImportedObject(mesh_id);
//      }
	}

	return True;
}
