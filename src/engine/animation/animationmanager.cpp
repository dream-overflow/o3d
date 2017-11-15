/**
 * @file animationmanager.cpp
 * @brief Implementation of AnimationManager.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-02-23
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/animation/animationmanager.h"

#include "o3d/core/debug.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/virtualfilelisting.h"
#include "o3d/core/classfactory.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(AnimationManager, ENGINE_ANIMATION_LIST, SceneResourceManager)

// Constructor.
AnimationManager::AnimationManager(BaseObject *parent, const String &path) :
	SceneResourceManager(parent, path, "*.o3dan")
{
	m_garbageManager.setLifeTime(10000);
}

// Destructor
AnimationManager::~AnimationManager()
{
	if (!m_findMap.empty())
	{
		String message("Animations still exists into the manager :\n");

		for (IT_FindMap it = m_findMap.begin(); it != m_findMap.end(); ++it)
		{
			message += "       |- " + it->second->getFileName() + "\n";
			deletePtr(it->second);
		}

		O3D_WARNING(message);
	}
}

// Delete child .
Bool AnimationManager::deleteChild(BaseObject *child)
{
	if (child)
	{
		if (child->getParent() != this)
			O3D_ERROR(E_InvalidParameter("The parent child differ from this"));
		else
		{
			// is it a material, and is it managed by this
			Animation *animation = o3d::dynamicCast<Animation*>(child);
			if (animation && (animation->getManager() == this))
			{
				deleteAnimation(animation);
				return True;
			}

			// otherwise simply delete the child
			deletePtr(child);
			return True;
		}
	}
	return False;
}

// update all dynamic materials
void AnimationManager::update()
{
	FastMutexLocker locker(m_mutex);

	// process deferred objects deletion
	m_garbageManager.update();
}

// Data import/export
Animation* AnimationManager::addAnimation(const String &filename)
{
	String absFilename = getFullFileName(filename);

	Animation *foundAnimation = findAnimation(0, absFilename);
	if (foundAnimation)
		return foundAnimation;

    InStream* is = FileManager::instance()->openInStream(absFilename);
    if (!is)
        return nullptr;

    O3D_MESSAGE("Import animation file \"" + filename + "\"");

    Animation *animation = reinterpret_cast<Animation*>(ClassFactory::readFromFile(*is, this));
	if (!animation)
	{
        deletePtr(is);
        return nullptr;
	}

	animation->setFileName(absFilename);
	animation->setResourceName(filename);

	addAnimation(animation);

    deletePtr(is);

	return animation;
}

// Insert an existing animation in the manager.
void AnimationManager::addAnimation(Animation *animation)
{
	if (animation->getManager() != NULL)
		O3D_ERROR(E_InvalidOperation("The given animation already have a manager"));

	// resource name must be defined
	if (animation->getResourceName().isEmpty())
		O3D_ERROR(E_InvalidPrecondition("Resource name must be defined"));

	addResource(animation->getResourceName());

	FastMutexLocker locker(m_mutex);

	if (animation->getFileName().isEmpty())
		animation->setFileName(m_path + '/' + animation->getResourceName());

	// search for the animation
    IT_FindMap it = m_findMap.find(animation->getResourceName());
	if (it != m_findMap.end())
		O3D_ERROR(E_InvalidParameter("A same animation with the same name already exists"));
	else
        m_findMap.insert(std::make_pair(animation->getResourceName(), animation));

    O3D_MESSAGE("Add animation \"" + animation->getResourceName() + "\"");

	// this is the manager and its parent
	animation->setManager(this);
	animation->setParent(this);
	animation->setId(m_IDManager.getID());
}

// Remove an existing animation from the manager.
void AnimationManager::removeAnimation(Animation *animation)
{
	if (animation->getManager() != this)
		O3D_ERROR(E_InvalidParameter("Animation manager is not this"));

	FastMutexLocker locker(m_mutex);

	// remove the animation object from the manager.
    IT_FindMap it = m_findMap.find(animation->getResourceName());
	if (it != m_findMap.end())
	{
        animation->setManager(nullptr);
		animation->setParent(getScene());

		m_IDManager.releaseID(animation->getId());
		animation->setId(-1);

		m_findMap.erase(it);

        O3D_MESSAGE("Remove (not delete) existing animation : " + animation->getResourceName());
	}
}

// Delete an existing animation from the manager.
void AnimationManager::deleteAnimation(Animation *animation)
{
	if (animation->getManager() != this)
		O3D_ERROR(E_InvalidParameter("Animation manager is not this"));

	FastMutexLocker locker(m_mutex);


	// remove the animation object from the manager.
    IT_FindMap it = m_findMap.find(animation->getResourceName());
	if (it != m_findMap.end())
	{
        m_garbageManager.add(animation->getResourceName(), animation);

        animation->setManager(nullptr);

		m_IDManager.releaseID(animation->getId());
		animation->setId(-1);

		m_findMap.erase(it);

        O3D_MESSAGE("Delete (to GC) animation: " + animation->getResourceName());
	}
}

// Is an animation exists.
Bool AnimationManager::isAnimation(const String &filename)
{
    return (findAnimation(0, filename) != nullptr);
}

// Purge immediately the garbage manager of its content.
void AnimationManager::purgeGarbage()
{
	FastMutexLocker locker(m_mutex);
	m_garbageManager.destroy();
}

Animation* AnimationManager::findAnimation(
        UInt32 type,
        const String &resourceName)
{
	FastMutexLocker locker(m_mutex);

	// search the key name into the map, next the animation given parameters into the element
    CIT_FindMap cit = m_findMap.find(resourceName);
	if (cit != m_findMap.end())
	{
		// found it ?
		return cit->second;
	}

	// maybe the asked animation was just deleted, so search it into the garbage collector
    Animation *animation = nullptr;
    m_garbageManager.remove(resourceName, animation);

	locker.unlock();

	// if found, reinsert it into the manager
	if (animation)
		addAnimation(animation);

	return animation;
}

// Export complete animation a single file
Int32 AnimationManager::exportAnimation()
{
	FastMutexLocker locker(m_mutex);
	Int32 count = 0;

	String absFileName, resourceName;

	for (IT_FindMap it = m_findMap.begin(); it != m_findMap.end(); ++it)
	{
		Animation *animation = it->second;

		// if the file name is not defined
		if (animation->getFileName().isEmpty())
		{
			absFileName = getFullFileName(animation->getResourceName());
			animation->setFileName(absFileName);
		}

		if (animation->save())
			count++;
	}

    return count;
}

