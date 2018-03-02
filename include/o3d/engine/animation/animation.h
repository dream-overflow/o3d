/**
 * @file animation.h
 * @brief An animation object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-02-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ANIMATION_H
#define _O3D_ANIMATION_H

#include "o3d/core/baseobject.h"
#include "o3d/core/memorydbg.h"
#include "../scene/sceneentity.h"
#include "animationtrack.h"

namespace o3d {

class Node;
class DrawInfo;
class AnimationNode;
class AnimationManager;

//---------------------------------------------------------------------------------------
//! @class Animation
//-------------------------------------------------------------------------------------
//! An animation is based on the father node of a subtree. It is composed of
//! a hierarchy of animation tracks, which describes the hierarchy under the father node
//---------------------------------------------------------------------------------------
class O3D_API Animation : public SceneResource
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(Animation)

	//! Animation blending mode.
	enum BlendMode
	{
		BLEND_REPLACE,        //!< Replace the animation.
		BLEND_ADD             //!< Add the effect of animations.
	};

	struct AnimRange          //!< animation range
	{
		UInt32 start;     //!< start frame
		UInt32 end;       //!< end frame
		String name;          //!< optional name
		UInt32 breaking;  //!< breaking animation frame (must be in range [Start+1..End-1]) 0 mean no break
	};

	typedef std::vector<AnimRange> T_AnimRangeVector;
	typedef T_AnimRangeVector::iterator IT_AnimRangeVector;
	typedef T_AnimRangeVector::const_iterator CIT_AnimRangeVector;

	//! Constructor.
	Animation(BaseObject *parent);

	//! Virtual destructor.
	virtual ~Animation();

	//! update the animation subtree (0..t..1)
	void update(
			class Animatable *animation,
			Float time,
			BlendMode blendMode,
			Float weight);

	//! set/get father node of the subtree
	inline void setFatherNode(AnimationNode *father) { m_fatherNode = father; }
	inline AnimationNode *getFatherNode() { return m_fatherNode; }
	inline const AnimationNode *getFatherNode() const { return m_fatherNode; }

	//! set/get the filename that contain this animation
	inline void setFileName(const String &filename) { m_filename = filename; }
	inline const String& getFileName() const { return m_filename; }

	//! add an animation range and return its Id
	inline Int32 addAnimRange(
			const String &name,
			UInt32 start,
			UInt32 end,
			UInt32 _break = 0)
	{
		AnimRange range;
		range.start = start;
		range.end = end;
		range.breaking = _break;
		range.name = name;
		m_animRange.push_back(range);

		return (Int32)m_animRange.size();
	}

	//! Get animation range by its id (read only).
	inline const AnimRange& getAnimRange(UInt32 id) const
	{
		O3D_ASSERT(id < (UInt32)m_animRange.size());
		return m_animRange[id];
	}

	//! Get animation range by its id.
	inline AnimRange& getAnimRange(UInt32 id)
	{
		O3D_ASSERT(id < (UInt32)m_animRange.size());
		return m_animRange[id];
	}

	//! Get range id for a name.
	inline Int32 getAnimRangeId(const String &name) const
	{
        for (Int32 i = 0; i < (Int32)m_animRange.size(); ++i) {
            if (m_animRange[i].name == name) {
				return i;
            }
		}
		return -1;
	}

	//! return the number of animation range
	inline UInt32 getNumAnimRange() const { return (UInt32)m_animRange.size(); }

	//! get an anim range by its name (return false if not found)
	Bool getAnimRange(const String &name, AnimRange &range);

	//! round to the nearest keyframes
	void roundToNearestKeyFrame(Animatable *target, Float &stime, Float &etime);

	//! use an anim range for pTarget
	void useAnimRange(Animatable *target, UInt32 id) const;

	//! use full range anim for pTarget
	void useFullRange(Animatable *target) const;

	//! precompute all animation range for tracks (need to be call a single time after all range defined)
	//! Note that is automatically called when you add an animation to a player, so take care to define
	//! all animation range before to add it to a player.
	void computeAnimRange();

	//! is animations ranges are precomputed
	Bool isAnimRangeComputed() const { return m_computed ; }

	//! set/get num object
	inline void setNumObject(UInt32 n) { m_numObjects = n; }
	inline UInt32 getNumObject() const { return m_numObjects;}

	//! set/get the duration of the animation
	inline void setDuration(Float duration) { m_duration = duration; }
	inline Float getDuration() const { return m_duration; }

	//! set/get the number of frames of the animation
	inline void setNumFrames(UInt32 frame) { m_frame = frame; }
	inline UInt32 getNumFrames() const { return m_frame; }

	//! draw the trajectories of all subtree
    void drawTrajectory(Node *currentNode, const DrawInfo &drawInfo);

	// serialization
    virtual Bool writeToFile(OutStream &os) override;
	//! export from a given node of the hierarchy (pSelectedNode) with his relative hierarchy node root
	virtual Bool writeToFile(OutStream &os, Node *hierarchyRoot, Node *selectedNode);
    virtual Bool readFromFile(InStream &is) override;

	//! Save the animation file (*.o3dan) into the specified path and using the defined filename.
	Bool save();

protected:

	AnimationNode *m_fatherNode;      //!< Father node of the subtree of track.
	UInt32 m_numObjects;          //!< Number of object used in the animations.

	String m_filename;                //!< Absolute filename.
	Float m_duration;             //!< The duration of the animation in seconds.

	T_AnimRangeVector m_animRange;    //!< A list of animation ranges.

	UInt32 m_frame;               //!< Number of frames of the animation.

	UInt32 m_start;               //!< Used start frame.
	UInt32 m_end;                 //!< Used end frame.

	Bool m_computed;              //!< Is animations range are computed for tracks.

	AnimationNode* computeStartNode(
		AnimationNode *currentAnimationNode,
		class Node *currentNode,
		class Node *selectedNode);
};

} // namespace o3d

#endif // _O3D_ANIMATION_H
