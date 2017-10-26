/**
 * @file animationblend.h
 * @brief An animation blending manager.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2006-04-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ANIMATIONBLEND_H
#define _O3D_ANIMATIONBLEND_H

#include "animation.h"
#include "animatable.h"
#include <vector>

namespace o3d {

//! an animation blend element
struct AnimationBlendElt
{
	SmartObject<Animation> animation; //!< Used animation.
	Float              weight;    //!< Weight of the animation.
	Animation::BlendMode   blendMode; //!< Blending mode of the animation.
	Float              offset;    //!< Animation offset.
	Float              scale;     //!< Scale of the animation.
	String                 fileName;  //!< Filename of the animation used for importation.
};

typedef std::vector<AnimationBlendElt> T_AnimationBlendEltVector;
typedef T_AnimationBlendEltVector::iterator IT_AnimationBlendEltVector;
typedef T_AnimationBlendEltVector::const_iterator CIT_AnimationBlendEltVector;

//---------------------------------------------------------------------------------------
//! @class Animation
//-------------------------------------------------------------------------------------
//! An animation blending manager
//---------------------------------------------------------------------------------------
class O3D_API AnimationBlend : public Animation, public Animatable
{
public:

	O3D_DECLARE_CLASS(AnimationBlend)

	//! Default constructor.
	AnimationBlend(BaseObject *parent);

	//! Virtual destructor.
	virtual ~AnimationBlend();

	//! Add a target animation and return its index
	//! @note The given animation stay a son of the animation manager
	UInt32 addAnimation(
		Animation* pAnim,
		Float weight,
		Animation::BlendMode blendMode,
		Float offset = 0,
		Float scale = 0);

	//! Set the target animation for an index
	inline Bool setAnimation(UInt32 index, Animation* pAnim)
	{
		if (index >= m_animations.size())
			return False;

		m_animations[index].animation = pAnim;

		return True;
	}
	//! Get the target animation for an index
	inline Animation* getAnimation(UInt32 index) const
	{
		O3D_ASSERT(index < m_animations.size());
		return m_animations[index].animation.get();
	}

	//! Set the animation weight for index
	inline Bool setAnimationBlendWeight(UInt32 index, Float weight)
	{
		if (index >= m_animations.size())
			return False;

		m_animations[index].weight = weight;
		return True;
	}
	//! Get the animation weight for index
	inline Float getAnimationBlendWeight(UInt32 index) const
	{
		O3D_ASSERT(index < m_animations.size());
		return m_animations[index].weight;
	}

	//! Set the animation blending mode for index
	inline Bool setAnimationBlendMode(UInt32 index, Animation::BlendMode blendmode)
	{
		if (index >= m_animations.size())
			return False;

		m_animations[index].blendMode = blendmode;
		return True;
	}
	//! Get the animation blending mode for index
	inline Animation::BlendMode getAnimationBlendMode(UInt32 index) const
	{
		O3D_ASSERT(index < m_animations.size());
		return m_animations[index].blendMode;
	}

	//! Set the animation offset for index
	inline Bool setAnimationOffset(UInt32 index, Float offset)
	{
		if (index >= m_animations.size())
			return False;

		m_animations[index].offset = offset;
		return True;
	}
	//! Get the animation offset for index
	inline Float getAnimationOffset(UInt32 index) const
	{
		O3D_ASSERT(index < m_animations.size());
		return m_animations[index].offset;
	}

	//! Set the animation scale for index
	inline Bool setAnimationScale(UInt32 index, Float scale)
	{
		if (index >= m_animations.size())
			return False;

		m_animations[index].scale = scale;
		return True;
	}
	//! Get the animation scale for index
	inline Float getAnimationScale(UInt32 index)const
	{
		O3D_ASSERT(index < m_animations.size());
		return m_animations[index].scale;
	}

	//! Return the number of blended animation
	inline UInt32 getNumAnimations()const { return (UInt32)m_animations.size(); }

	//! Release all blended animations
	void releaseAllAnimations();

	// Target is here
	virtual void animate(
		AnimationTrack::TrackType type,
		const void* value,
		UInt32 sizeOfValue,
		AnimationTrack::Target target,
		UInt32 subTarget,
		Animation::BlendMode blendMode,
		Float weight);

	virtual void update(
			class Animatable* father,
			Float t,
			Animation::BlendMode blendMode,
			Float weight);

	//! Return the father node of the track sub-tree
	virtual AnimationNode* getFatherNode();

	//! Set the father node of the track sub-tree
	virtual void setFatherNode(AnimationNode* father);

	//! Draw the trajectories of all sub-tree
	virtual void drawTrajectory(Node* curNode);

	virtual Animatable* getFirstSon() { return NULL; }
	virtual Animatable* getNextSon() { return NULL; }
	virtual Bool hasMoreSons() { return False; }
	virtual void resetAnim() {}
	virtual Int32 getAnimatableId(Animatable::AnimatableManager &type) { return -1; }
	virtual AnimatableTrack* getAnimationStatus(const AnimationTrack* track) { return NULL; }
	virtual const Matrix4& getPrevAnimationMatrix() const { return Matrix4::getIdentity(); }

	// Serialization
	virtual Bool writeToFile(OutStream &os);
	//! Export from a given node of the hierarchy (pSelectedNode) with his relative hierarchy node root
	virtual Bool writeToFile(OutStream &os, Node *hierarchyRoot, Node *selectedNode);
	virtual Bool readFromFile(InStream &is);

protected:

	T_AnimationBlendEltVector m_animations; //!< blended animations
};

} // namespace o3d

#endif // _O3D_ANIMATIONBLEND_H

