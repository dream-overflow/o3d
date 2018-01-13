/**
 * @file forcemanager.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/physic/precompiled.h"
#include "o3d/physic/forcemanager.h"

#include "o3d/engine/scene/scene.h"

#include "o3d/core/classfactory.h"
#include "o3d/core/debug.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(ABCForce, PHYSIC_FORCE, SceneObject)
O3D_IMPLEMENT_DYNAMIC_CLASS1(ForceManager, PHYSIC_FORCE_MANAGER, SceneTemplateManager<ABCForce>)

Bool ForceManager::writeToFile(OutStream &os)
{
    os << PHYSIC_FORCE_MANAGER;
    os << getNumElt();

    for (IT_TemplateManager it = m_MyMap.begin() ; it != m_MyMap.end() ; ++it) {
        ClassFactory::writeToFile(os,*(*it).second);
	}

	return True;
}

Bool ForceManager::readFromFile(InStream &is)
{
	UInt32 tmp;
    is >> tmp;

	if (tmp != PHYSIC_FORCE_MANAGER)
        O3D_ERROR(E_InvalidFormat("Invalid force list token"));

	Int32 size;
    is >> size;

    for (Int32 i = 0; i < size ; ++i) {
        ABCForce* pForce = (ABCForce*)ClassFactory::readFromFile(is, this);

        if (!pForce) {
			return False;
        }

		addElement(pForce);
	}

	return True;
}
