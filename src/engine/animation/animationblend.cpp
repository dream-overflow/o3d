/**
 * @file animationblend.cpp
 * @brief Implementation of AnimationBlend.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-04-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/animation/animationblend.h"

#include "o3d/engine/animation/animationmanager.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/drawinfo.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS2(AnimationBlend, ENGINE_ANIMATION_BLEND, Animation, Animatable)

/*---------------------------------------------------------------------------------------
  constructor
---------------------------------------------------------------------------------------*/
AnimationBlend::AnimationBlend(BaseObject *parent) :
	Animation(parent)
{
	m_duration = 0;
}

/*---------------------------------------------------------------------------------------
  desctructor
---------------------------------------------------------------------------------------*/
AnimationBlend::~AnimationBlend()
{
	releaseAllAnimations();
}

/*---------------------------------------------------------------------------------------
  add a target animation and return its index
---------------------------------------------------------------------------------------*/
UInt32 AnimationBlend::addAnimation(
	Animation* animation,
	Float weight,
	Animation::BlendMode blendMode,
	Float offset,
	Float scale)
{
    O3D_ASSERT(animation != nullptr);
	if (!animation)
		return 0;

	AnimationBlendElt elt;
	elt.animation = animation;
	elt.blendMode = blendMode;
	elt.weight = weight;
	elt.offset = offset;
	elt.scale = scale;

	m_animations.push_back(elt);

	m_duration = o3d::max(m_duration, animation->getDuration());
	return (UInt32)(m_animations.size() - 1);
}

/*---------------------------------------------------------------------------------------
  release all blended animations
---------------------------------------------------------------------------------------*/
void AnimationBlend::releaseAllAnimations()
{
	m_animations.clear();
	m_duration = 0.f;
}

/*---------------------------------------------------------------------------------------
  animate
---------------------------------------------------------------------------------------*/
void AnimationBlend::animate(
	AnimationTrack::TrackType type,
	const void* value,
	UInt32 sizeOfValue,
	AnimationTrack::Target target,
	UInt32 subTarget,
	Animation::BlendMode blendMode,
	Float weight)
{
	switch (type)
	{
		case AnimationTrack::TRACK_TYPE_FLOAT:
			if (sizeOfValue != sizeof(Float))
			{
				// the value size is invalid
				O3D_ERROR(E_InvalidParameter("SizeOfValue"));
				break;
			}

			switch (target)
			{
				case AnimationTrack::TARGET_BLEND_OFFSET:
					if (blendMode == Animation::BLEND_REPLACE)
						m_animations[subTarget].offset = *((Float*)value) * weight;
					else if (blendMode == Animation::BLEND_ADD)
						m_animations[subTarget].offset += *((Float*)value) * weight;
					break;

				case AnimationTrack::TARGET_BLEND_SCALE:
					if (blendMode == Animation::BLEND_REPLACE)
						m_animations[subTarget].scale = *((Float*)value) * weight;
					else if (blendMode == Animation::BLEND_ADD)
						m_animations[subTarget].scale += *((Float*)value) * weight;
					break;

				case AnimationTrack::TARGET_BLEND_WEIGHT:
					if (blendMode == Animation::BLEND_REPLACE)
						m_animations[subTarget].weight = *((Float*)value) * weight;
					else if (blendMode == Animation::BLEND_ADD)
						m_animations[subTarget].weight += *((Float*)value) * weight;
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}
}

/*---------------------------------------------------------------------------------------
  update
---------------------------------------------------------------------------------------*/
void AnimationBlend::update(
	class Animatable* father,
	Float t,
	Animation::BlendMode blendMode,
	Float weight)
{
	// reset animation before make a blending
	father->resetAnim();

	IT_AnimationBlendEltVector it;
	for (it = m_animations.begin() ; it != m_animations.end() ; ++it)
	{
		AnimationBlendElt& elt = (*it);
		Animation::BlendMode subAnimBlendMode = elt.blendMode;

		Float Time;

		if (elt.scale == 0.f)
		{
			Time = t * m_duration / elt.animation->getDuration();
		}
		else
		{
			Time = t / elt.scale;
		}

		Time += elt.offset;

		if ((blendMode == Animation::BLEND_ADD) && (subAnimBlendMode == Animation::BLEND_REPLACE))
		{
			subAnimBlendMode = Animation::BLEND_ADD;
		}

		elt.animation->update(father,Time,subAnimBlendMode,elt.weight * weight);
	}
}

/*---------------------------------------------------------------------------------------
  return the father node of the track sub-tree
---------------------------------------------------------------------------------------*/
AnimationNode* AnimationBlend::getFatherNode()
{
    return nullptr;
}

/*---------------------------------------------------------------------------------------
  set the father node of the track sub-tree
---------------------------------------------------------------------------------------*/
void AnimationBlend::setFatherNode(AnimationNode* /*father*/)
{
	// Nothing
}

/*---------------------------------------------------------------------------------------
  draw the trajectories of all sub-tree
---------------------------------------------------------------------------------------*/
void AnimationBlend::drawTrajectory(Node* curNode, const DrawInfo &drawInfo)
{
	IT_AnimationBlendEltVector it;

    for (it = m_animations.begin() ; it != m_animations.end() ; ++it) {
		AnimationBlendElt& elt = (*it);
        elt.animation->drawTrajectory(curNode, drawInfo);
	}
}

/*---------------------------------------------------------------------------------------
  serialisation
---------------------------------------------------------------------------------------*/
Bool AnimationBlend::writeToFile(
        OutStream &os,
		Node *hierarchyRoot,
		Node *selectedNode)
{
    if (!Animation::writeToFile(os, hierarchyRoot, selectedNode)) {
		return False;
    }

    os << (UInt32)m_animations.size();

    for (IT_AnimationBlendEltVector it = m_animations.begin(); it != m_animations.end(); ++it) {
		AnimationBlendElt elt = (*it);

        os   << elt.animation->getResourceName()
			 << elt.weight
			 << (Int32)elt.blendMode
			 << elt.offset
			 << elt.scale;
	}

	return True;
}

Bool AnimationBlend::writeToFile(OutStream &os)
{
    if (!Animation::writeToFile(os))
		return False;

    os << (UInt32)m_animations.size();

	for (IT_AnimationBlendEltVector it = m_animations.begin(); it != m_animations.end(); ++it)
	{
		AnimationBlendElt elt = (*it);

        os   << elt.animation->getResourceName()
			 << elt.weight
			 << (Int32)elt.blendMode
			 << elt.offset
			 << elt.scale;
	}

	return True;
}

Bool AnimationBlend::readFromFile(InStream &is)
{
    if (!Animation::readFromFile(is))
		return False;

	UInt32 size;
    is >> size;

	Int32 blendMode;

	for (UInt32 i = 0; i < size; ++i)
	{
		AnimationBlendElt elt;

        is   >> elt.fileName
			 >> elt.weight
			 >> blendMode
			 >> elt.offset
			 >> elt.scale;

		elt.blendMode = Animation::BlendMode(blendMode);

		// get the target animation
		elt.animation = getScene()->getAnimationManager()->addAnimation(elt.fileName);

		// and insert the element
		m_animations.push_back(elt);
	}

	return True;
}
