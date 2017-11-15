/**
 * @file scenetemplatemanager.h
 * @brief Base class for any scene related object that must inherit from TemplateManager.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-03-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SCENETEMPLATEMANAGER_H
#define _O3D_SCENETEMPLATEMANAGER_H

#include "o3d/core/templatemanager.h"
#include "o3d/core/memorydbg.h"

#include "../enginetype.h"

namespace o3d {

class Scene;

//---------------------------------------------------------------------------------------
//! @class SceneTemplateManager
//-------------------------------------------------------------------------------------
//! Base class for any scene related object that must inherit from TemplateManager.
//---------------------------------------------------------------------------------------
template<class T>
class O3D_API_TEMPLATE SceneTemplateManager : public TemplateManager<T>
{
public:

	SceneTemplateManager(BaseObject *parent) :
		TemplateManager<T>(parent)
	{
	}

	//! Get the scene parent.
	inline Scene* getScene() { return reinterpret_cast<Scene*>(BaseObject::m_topLevelParent); }

	//! Get the scene parent (read only).
	inline const Scene* getScene() const { return reinterpret_cast<Scene*>(BaseObject::m_topLevelParent); }

	//! The top level parent must be a scene.
	virtual Bool hasTopLevelParentTypeOf() const
	{
		if (BaseObject::m_topLevelParent)
			return (BaseObject::m_topLevelParent->getType() == ENGINE_SCENE);
		else
			return False;
	}
};

} // namespace o3d

#endif // _O3D_SCENETEMPLATEMANAGER_H

