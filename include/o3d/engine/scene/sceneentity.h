/**
 * @file sceneentity.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SCENEENTITY_H
#define _O3D_SCENEENTITY_H

#include "o3d/core/smartobject.h"
#include "o3d/core/resourcemanager.h"
#include "o3d/core/memorydbg.h"

#include "../enginetype.h"

namespace o3d {

class Scene;

/**
 * @brief A base object of the scene.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-03-22
 */
class O3D_API SceneEntity : public BaseObject
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(SceneEntity)

	SceneEntity(BaseObject *parent) :
		BaseObject(parent)
	{
	}

	//! Get the scene parent.
	inline Scene* getScene() { return reinterpret_cast<Scene*>(m_topLevelParent); }

	//! Get the scene parent (read only).
	inline const Scene* getScene() const { return reinterpret_cast<Scene*>(m_topLevelParent); }

	//! The top level parent must be a scene.
	virtual Bool hasTopLevelParentTypeOf() const;
};


/**
 * @brief A base resource of the scene.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-03-22
 */
class O3D_API SceneResource : public Resource
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(SceneResource)

	SceneResource(BaseObject *parent) :
		Resource(parent)
	{
	}

	//! Get the scene parent.
	inline Scene* getScene() { return reinterpret_cast<Scene*>(m_topLevelParent); }

	//! Get the scene parent (read only).
	inline const Scene* getScene() const { return reinterpret_cast<Scene*>(m_topLevelParent); }

	//! The top level parent must be a scene.
	virtual Bool hasTopLevelParentTypeOf() const;
};


/**
 * @brief A base resource manager of the scene.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-03-22
 */
class O3D_API SceneResourceManager : public ResourceManager
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(SceneResourceManager)

	SceneResourceManager(
			BaseObject *parent,
			const String &path,
			const String &extList) :
		ResourceManager(parent, path, extList)
	{
	}

	//! Get the scene parent.
	inline Scene* getScene() { return reinterpret_cast<Scene*>(m_topLevelParent); }

	//! Get the scene parent (read only).
	inline const Scene* getScene() const { return reinterpret_cast<Scene*>(m_topLevelParent); }

	//! The top level parent must be a scene.
	virtual Bool hasTopLevelParentTypeOf() const;
};

} // namespace o3d

#endif // _O3D_SCENEENTITY_H

