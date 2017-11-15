/**
 * @file animatable.cpp
 * @brief Implementation of Animatable.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-04-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/animation/animatable.h"

#include "o3d/engine/animation/animationtrack.h"

#include "o3d/engine/scene/sceneobjectmanager.h"
#include "o3d/engine/material/material.h"
#include "o3d/engine/effect/specialeffectsmanager.h"
#include "o3d/engine/hierarchy/hierarchytree.h"

#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_CLASS_COMMON(Animatable, ENGINE_ANIMATABLE, NULL)

// serialization
void Animatable::writeToFile(Scene *scene,
    Animatable *animatable,
    OutStream &os)
{
    O3D_ASSERT(animatable != nullptr);

	AnimatableManager type;
	Int32 id = animatable->getAnimatableId(type);

    os << id
       << (Int32)type;
}

Animatable* Animatable::readFromFile(Scene *scene, InStream &is)
{
	AnimatableManager type;
	Int32 id, typeInt;

    is >> id
       >> typeInt;

	type = (AnimatableManager)typeInt;

	switch(type)
	{
	case SCENE_OBJECT:
		return static_cast<Animatable*>(scene->getSceneObjectManager()->getImportedSceneObject(id));
//	case Material:
//		return static_cast<Animatable*>(scene->getMaterialManager()->getImportedMaterial(id));
	case SPECIAL_EFFECT:
		return static_cast<Animatable*>(scene->getSpecialEffectsManager()->getImportedSpecialEffects(id));
	default:
        return nullptr;
	}
}

