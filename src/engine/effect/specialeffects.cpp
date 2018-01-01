/**
 * @file specialeffects.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-05-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/effect/specialeffects.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(SpecialEffects, ENGINE_EFFECT, SceneObject)

// Default contructor.
SpecialEffects::SpecialEffects(BaseObject *parent) :
	SceneObject(parent),
    m_manager(nullptr)
{
}

// animate the object
void SpecialEffects::animate(
	AnimationTrack::TrackType type,
	const void* value,
	UInt32 sizeOfValue,
	AnimationTrack::Target target,
	UInt32 subTarget,
	Animation::BlendMode blendMode,
	Float weight)
{
}

Int32 SpecialEffects::getAnimatableId(Animatable::AnimatableManager &type)
{
	type = Animatable::SPECIAL_EFFECT;
	return getSerializeId();
}

Bool SpecialEffects::writeToFile(OutStream &os)
{
    return SceneObject::writeToFile(os);
}

Bool SpecialEffects::readFromFile(InStream &is)
{
    return SceneObject::readFromFile(is);
}

void SpecialEffects::postImportPass()
{
}
