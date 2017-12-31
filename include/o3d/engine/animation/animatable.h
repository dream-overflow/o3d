/**
 * @file animatable.h
 * @brief An animatable is an object whose have the possibility to be animated
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-04-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ANIMATABLE_H
#define _O3D_ANIMATABLE_H

#include "keyframe.h"
#include "animation.h"
#include "o3d/core/hashmap.h"
#include "o3d/core/classinfo.h"
//#include <map>

namespace o3d {

class Scene;
class AnimationTrack;

/**
 * @brief An animatable is an object whose have the possibility to be animated
 */
class O3D_API Animatable
{
public:

	O3D_DECLARE_INTERFACE(Animatable)

	//! struct contained in the map for each animatable
	struct AnimatableTrack
	{
		Float Time;           //!< Previous time.
		CIT_KeyFrameList Current; //!< Start iterator for next frame.
		CIT_KeyFrameList First;   //!< Start iterator of the subanim.
		CIT_KeyFrameList Last;    //!< End iterator of the subanim.
	};

	typedef stdext::hash_map<const AnimationTrack*, AnimatableTrack> T_AnimationKeyFrameItMap;
	typedef T_AnimationKeyFrameItMap::iterator IT_AnimationKeyFrameItMap;
	typedef T_AnimationKeyFrameItMap::const_iterator CIT_AnimationKeyFrameItMap;

	//! get animation keyframe it map ref
	virtual AnimatableTrack* getAnimationStatus(const AnimationTrack *track) = 0;

	//! whose manager include this animatable (animatable inherit from its type)
	enum AnimatableManager
	{
		UNDEFINED,
		SCENE_OBJECT,
		SPECIAL_EFFECT,
		MATERIAL
	};

	//! Return the previous animation matrix for draw animation trajectory
	virtual const Matrix4& getPrevAnimationMatrix() const = 0;

	//! animate the object
	virtual void animate(
			AnimationTrack::TrackType type,
			const void* value,
			UInt32 sizeOfValue,
			AnimationTrack::Target target,
			UInt32 subTarget,
			enum Animation::BlendMode blendMode,
			Float weight) = 0;

	//! reset the animations value, in way to compute a new frame
	virtual void resetAnim() = 0;

	//! get the first son of the node
	virtual Animatable* getFirstSon() = 0;

	//! get the next son of the node
	virtual Animatable* getNextSon() = 0;

	//! is there another sons ?
	virtual Bool hasMoreSons() = 0;

	//! Get the object ID for player serialization.
	//! Get too the base type of this animatable (like BaseObject,SpecialEffect,Material,Node)
	//! Useful for serialization for know to what manager we search for the AnimatableId
	virtual Int32 getAnimatableId(AnimatableManager &type) = 0;

	// Serialization
    static void writeToFile(Scene *scene, Animatable *animatable, OutStream &os);
    static Animatable* readFromFile(Scene *scene, InStream &is);
};

} // namespace o3d

#endif // _O3D_ANIMATABLE_H
