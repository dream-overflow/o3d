/**
 * @file specialeffects.h
 * @brief Base class to any special effect.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-05-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SPECIALEFFECTS_H
#define _O3D_SPECIALEFFECTS_H

#include "o3d/core/memorydbg.h"
#include "o3d/engine/scene/sceneobject.h"

namespace o3d {

class SpecialEffectsManager;

/**
 * @brief Base class to any special effect (pre or post)
 */
class O3D_API SpecialEffects : public SceneObject
{
	friend class SpecialEffectsManager;

public:

	O3D_DECLARE_ABSTRACT_CLASS(SpecialEffects)

	//! contructor
	SpecialEffects(BaseObject *parent);

	//! Virtual destructor.
	virtual ~SpecialEffects() {}

	virtual void animate(
		AnimationTrack::TrackType type,
		const void* value,
		UInt32 sizeOfValue,
		AnimationTrack::Target target,
		UInt32 subTarget,
		Animation::BlendMode blendMode,
        Float weight) override;

    virtual Int32 getAnimatableId(Animatable::AnimatableManager &type) override;

    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

    virtual void postImportPass() override;

protected:

	SpecialEffectsManager *m_manager;
};

} // namespace o3d

#endif // _O3D_SPECIALEFFECTS_H
