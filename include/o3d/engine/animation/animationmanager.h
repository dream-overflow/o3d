/**
 * @file animationmanager.h
 * @brief Animation manager.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-02-23
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ANIMATIONMANAGER_H
#define _O3D_ANIMATIONMANAGER_H

#include "../scene/sceneentity.h"
#include "o3d/core/mutex.h"
#include "o3d/core/garbagemanager.h"
#include "o3d/core/idmanager.h"
#include "o3d/core/stringlist.h"
#include "o3d/core/memorydbg.h"
#include "animation.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class AnimationPlayer
//-------------------------------------------------------------------------------------
//! Animation manager
//---------------------------------------------------------------------------------------
class O3D_API AnimationManager : public SceneResourceManager
{
public:

	O3D_DECLARE_CLASS(AnimationManager)

	//! Default constructor.
	AnimationManager(BaseObject *parent, const String &path = "");

	//! Virtual destructor.
	virtual ~AnimationManager();

	virtual Bool deleteChild(BaseObject *child);

 	//---------------------------------------------------------------------------------------
	// Factory
	//---------------------------------------------------------------------------------------

	//! Insert an existing animation in the manager.
	//! @note Once the animation is inserted into the manager, it become its parent.
	void addAnimation(Animation *animation);

	//! Remove an existing animation from the manager.
	//! The animation is not pushed in the garbage manager, it is simply removed of this manager.
	void removeAnimation(Animation *animation);

	//! Delete an existing animation from the manager. It is stored temporarily in the
	//! garbage manager, before to be really deleted if the mesh data is not reused
	//! before a defined time.
	void deleteAnimation(Animation *animation);

	//! Purge immediately the garbage manager of its content.
	void purgeGarbage();

    //! Creates/returns an animation data from a file (.o3dan).
    //! @param filename The resource containing the animation.
	//! @return A pointer on the asked animation.
	//! @note If a similar animation exists it is returned and shared.
	Animation* addAnimation(const String &filename);

	//! Is an animation exists.
	//! @param filename Filename to search for.
	Bool isAnimation(const String &filename);

	//-----------------------------------------------------------------------------------
	// Processing
    //-----------------------------------------------------------------------------------

	//! Update the garbage collector.
	void update();

	//-----------------------------------------------------------------------------------
	// IO
    //-----------------------------------------------------------------------------------

	//! Export complete animations with one file per animation.
	//! @return Number exported animations file.
	Int32 exportAnimation();

protected:

	typedef std::map<String, Animation*> T_FindMap;
	typedef T_FindMap::iterator IT_FindMap;
	typedef T_FindMap::const_iterator CIT_FindMap;

	T_FindMap m_findMap;

	//! Manage removed animation objects.
	GarbageManager<String, Animation*> m_garbageManager;

	IDManager m_IDManager;

    Animation* findAnimation(UInt32 type, const String &resourceName);

	//Bool m_isAsynchronous; maybe later
};

} // namespace o3d

#endif // _O3D_ANIMATIONMANAGER_H

