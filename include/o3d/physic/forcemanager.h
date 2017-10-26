/**
 * @file forcemanager.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FORCEMANAGER_H
#define _O3D_FORCEMANAGER_H

#include "abcforce.h"
#include "rigidbody.h"
#include "o3d/engine/scene/scenetemplatemanager.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

/**
 * @brief Physic force Manager
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2006-05-30
 */
class O3D_API ForceManager : public SceneTemplateManager<ABCForce>
{
public:

	O3D_DECLARE_CLASS(ForceManager)

	ForceManager(BaseObject *pParent) :
		SceneTemplateManager<ABCForce>(pParent) {}

	//! process the force manager to a particule
	//void processParticule(Particule& particule)
	//{
	//	for (IT_TemplateManager(ABCForce) it = begin(); it != end() ; ++it)
	//	{
	//		ABCForce *pForce = (*it).second;
	//		if (pForce->isActive())
	//			pForce->processParticule(particule);
	//	}
	//}

	//! process the force manager to a rigid body
	void processObject(RigidBody& object)
	{
		for (IT_TemplateManager it = begin(); it != end() ; ++it)
		{
			ABCForce *pForce = (*it).second;
			if (pForce->getActivity())
				pForce->processObject(object);
		}
	}

	//! serialisation
	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);
};

} // namespace o3d

#endif // _O3D_FORCEMANAGER_H

