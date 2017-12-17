/**
 * @file specialeffectsmanager.cpp
 * @brief Implementation of SpecialEffectManager.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-01-31
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/effect/specialeffectsmanager.h"

#include "o3d/core/classfactory.h"
#include "o3d/core/debug.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(SpecialEffectsManager, ENGINE_EFFET_LIST, SceneEntity)

SpecialEffectsManager::SpecialEffectsManager(BaseObject *parent) :
	SceneEntity(parent),
	m_indexToEffect(NULL),
	m_indexToEffectSize(0)
{
}

// Virtual destructor.
SpecialEffectsManager::~SpecialEffectsManager()
{
	deleteArray(m_indexToEffect);

    if (!m_findMap.empty()) {
        for (IT_FindMap it = m_findMap.begin(); it != m_findMap.end(); ++it) {
            O3D_WARNING(String("Remaining special effect : ") + it->second->getName());
            deletePtr(it->second);
        }
	}
}

Bool SpecialEffectsManager::deleteChild(BaseObject *child)
{
    if (child) {
        if (child->getParent() != this) {
			O3D_ERROR(E_InvalidParameter("The parent child differ from this"));
        } else {
			// is it a material, and is it managed by this
			SpecialEffects *specialEffect = o3d::dynamicCast<SpecialEffects*>(child);
            if (specialEffect && (specialEffect->m_manager == this)) {
				deleteSpecialEffects(specialEffect);
				return True;
			}

			// otherwise simply delete the child
			deletePtr(child);
			return True;
		}
	}
	return False;
}

// Insert an existing special effect in the manager.
void SpecialEffectsManager::addSpecialEffects(SpecialEffects *specialEffect)
{
    if (specialEffect->m_manager != nullptr) {
		O3D_ERROR(E_InvalidOperation("The given special effects already have a manager"));
    } else {
        FastMutexLocker locker(m_mutex);

        String keyName = specialEffect->getName();

        // search for the special effects
        IT_FindMap it = m_findMap.find(keyName);
        if (it != m_findMap.end()) {
            O3D_ERROR(E_InvalidParameter("A same special effects with the same name already exists"));
        } else {
            m_findMap.insert(std::make_pair(keyName, specialEffect));
        }

        O3D_MESSAGE("Add special effects \"" + keyName + "\"");

        // this is the manager and its parent
        specialEffect->m_manager = this;
        specialEffect->setParent(this);
    }
}

// Remove an existing special effect from the manager.
void SpecialEffectsManager::removeSpecialEffects(SpecialEffects *specialEffect)
{
    if (specialEffect->m_manager != this) {
		O3D_ERROR(E_InvalidParameter("Special effects manager is not this"));
    } else {
        FastMutexLocker locker(m_mutex);

        String keyName = specialEffect->getName();

        // remove the special effects object from the manager.
        IT_FindMap it = m_findMap.find(keyName);
        if (it != m_findMap.end()) {
            specialEffect->m_manager = nullptr;
            specialEffect->setParent(getScene());

            m_findMap.erase(it);

            O3D_MESSAGE("Remove (not delete) existing specials effect : " + keyName);
        }
    }
}

// Delete an existing special effect from the manager.
void SpecialEffectsManager::deleteSpecialEffects(SpecialEffects *specialEffect)
{
    if (specialEffect->m_manager != this) {
		O3D_ERROR(E_InvalidParameter("Special effects manager is not this"));
    } else {
        FastMutexLocker locker(m_mutex);

        String keyName = specialEffect->getName();

        // remove the special effects object from the manager.
        IT_FindMap it = m_findMap.find(keyName);
        if (it != m_findMap.end())
        {
            deletePtr(specialEffect);

            m_findMap.erase(it);

            O3D_MESSAGE("Delete existing special effects : " + keyName);
        }
    }
}

// Is a special effect exists.
Bool SpecialEffectsManager::isSpecialEffects(const String &name)
{
	String keyName = name;
    return (findSpecialEffect(0, keyName) != nullptr);
}

// update all managed effects
void SpecialEffectsManager::update()
{
	FastMutexLocker locker(m_mutex);

	// process some effects updates
    for (IT_FindMap it = m_findMap.begin(); it != m_findMap.end(); ++it) {
        if (it->second->getActivity()) {
			(*it).second->update();
        }
	}
}

// draw all managed effects
void SpecialEffectsManager::draw(const DrawInfo &drawInfo)
{
	// doesn't work with override drawing mode
    if (getScene()->getContext()->isOverrideDrawingMode()) {
		return;
    } else {
        FastMutexLocker locker(m_mutex);

        for (IT_FindMap it = m_findMap.begin(); it != m_findMap.end(); ++it) {
            if (it->second->getActivity() && it->second->getVisibility()) {
                (*it).second->draw(drawInfo);
            }
        }
    }
}

// Resize the list of currently importing special effects.
void SpecialEffectsManager::resizeImportedSpecialEffects(UInt32 size)
{
	deleteArray(m_indexToEffect);
	m_indexToEffect = new SpecialEffects*[size];
	m_indexToEffectSize = size;
}

// Serialization
Bool SpecialEffectsManager::writeToFile(OutStream &os)
{
    BaseObject::writeToFile(os);

    os << (Int32)m_findMap.size();

	Int32 serializeId = 0;
    {
        FastMutexLocker locker(m_mutex);

        // attribute id to all effects
        for (IT_FindMap it = m_findMap.begin(); it != m_findMap.end(); ++it) {
            (*it).second->setSerializeId(serializeId++);
        }

        // export all effects
        for (IT_FindMap it = m_findMap.begin(); it != m_findMap.end(); ++it) {
            ClassFactory::writeToFile(os, *(*it).second);
        }
    }

	return True;
}

Bool SpecialEffectsManager::readFromFile(InStream &is)
{
    BaseObject::readFromFile(is);

	Int32 size;
    is >> size;

    {
        FastMutexLocker locker(m_mutex);

        resizeImportedSpecialEffects(size);

        // import all effects
        for (Int32 i = 0; i < size; ++i) {
            SpecialEffects* specialEffects = (SpecialEffects*)ClassFactory::readFromFile(is, this);

            if (!specialEffects)
                return False;

            setImportedSpecialEffects(i, specialEffects);
        }

        // post import pass
        for (Int32 i = 0; i < size; ++i) {
            getImportedSpecialEffects(i)->postImportPass();
        }
    }

	return True;
}

SpecialEffects* SpecialEffectsManager::findSpecialEffect(
		UInt32 type,
		const String &keyName)
{
	FastMutexLocker locker(m_mutex);

	// search the key name into the map, next the special effects given parameters into the element
	CIT_FindMap cit = m_findMap.find(keyName);
    if (cit != m_findMap.end()) {
		// found it ?
		return cit->second;
	}

	// not found
    return nullptr;
}
