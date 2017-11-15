/**
 * @file animationnode.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/animation/animationnode.h"

using namespace o3d;

/*---------------------------------------------------------------------------------------
  constructor
---------------------------------------------------------------------------------------*/
AnimationNode::AnimationNode(AnimationNode* pFather) :
	m_animFather(pFather)
{
}

/*---------------------------------------------------------------------------------------
  destructor
---------------------------------------------------------------------------------------*/
AnimationNode::~AnimationNode()
{
	// delete all tracks
	for (IT_AnimationTrackList t_it = m_trackList.begin() ; t_it != m_trackList.end() ; ++t_it)
		deletePtr(*t_it);

	// delete recursively all son
	for (IT_AnimationNodeList n_it = m_sonList.begin() ; n_it != m_sonList.end() ; ++n_it)
		deletePtr(*n_it);
}

/*---------------------------------------------------------------------------------------
  find the nearest time of a track for a given time
---------------------------------------------------------------------------------------*/
void AnimationNode::findTrackTime(
		Animatable* target,
		Float startTime,
		Float endTime,
		Float &newStartTime,
		Float &newEndTime) const
{
    // search on each track on the Animatable
	CIT_AnimationTrackList TrackIt;

	for (TrackIt = m_trackList.begin(); TrackIt != m_trackList.end(); ++TrackIt)
	{
		AnimationTrack* Track = (*TrackIt);
		Track->findKeyFrameTime(target, startTime, endTime, newStartTime, newEndTime);
	}

	// Recurse on the sons
	Animatable* Son = target->getFirstSon();
	CIT_AnimationNodeList AnimationIt = m_sonList.begin();

	while (target->hasMoreSons())
	{
		if (AnimationIt == m_sonList.end())
		{
			// the animation node has not enough sons
			return;
		}

		// recurse on the child
		(*AnimationIt)->findTrackTime(Son, startTime, endTime, newStartTime, newEndTime);

		AnimationIt++;
		Son = target->getNextSon();
	}
}

// precompute tracks range for a defined range
void AnimationNode::computeRange(Float stime, Float etime)
{
    // search on each track on the Animatable
	IT_AnimationTrackList TrackIt;

	for (TrackIt = m_trackList.begin(); TrackIt != m_trackList.end(); ++TrackIt)
	{
		AnimationTrack* Track = (*TrackIt);
		Track->computeKeyRange(stime, etime);
	}

	// Recurse on the sons
	IT_AnimationNodeList AnimationIt = m_sonList.begin();

	while (AnimationIt != m_sonList.end())
	{
		// recurse on the child
		(*AnimationIt)->computeRange(stime, etime);
		AnimationIt++;
	}
}

// position the tracks for a defined range
void AnimationNode::useRange(Animatable* target, UInt32 id) const
{
    // search on each track on the Animatable
	CIT_AnimationTrackList TrackIt;

	for (TrackIt = m_trackList.begin(); TrackIt != m_trackList.end(); ++TrackIt)
	{
		AnimationTrack* Track = (*TrackIt);
		Track->useKeyRange(target, id);
	}

	// Recurse on the sons
	Animatable* Son = target->getFirstSon();
	CIT_AnimationNodeList AnimationIt = m_sonList.begin();

	while (target->hasMoreSons())
	{
		if (AnimationIt == m_sonList.end())
		{
			// the animation node has not enough sons
			return;
		}

		// recurse on the child
		(*AnimationIt)->useRange(Son, id);

		AnimationIt++;
		Son = target->getNextSon();
	}
}

/*---------------------------------------------------------------------------------------
  use the full anim range
---------------------------------------------------------------------------------------*/
void AnimationNode::defineFullRange(Animatable* target) const
{
    // search on each track on the Animatable
	CIT_AnimationTrackList TrackIt;

	for (TrackIt = m_trackList.begin(); TrackIt != m_trackList.end(); ++TrackIt)
	{
		AnimationTrack* Track = (*TrackIt);
		Track->setFullKeyRange(target);
	}

	// Recurse on the sons
	Animatable* Son = target->getFirstSon();
	CIT_AnimationNodeList AnimationIt = m_sonList.begin();

	while (target->hasMoreSons())
	{
		if (AnimationIt == m_sonList.end())
		{
			// the animation node has not enough sons
			return;
		}

		// recurse on the child
		(*AnimationIt)->defineFullRange(Son);

		AnimationIt++;
		Son = target->getNextSon();
	}
}

/*---------------------------------------------------------------------------------------
  update an object time is [0..1]
---------------------------------------------------------------------------------------*/
void AnimationNode::update(
	Animatable* target,
	Float time,
	Animation::BlendMode blendMode,
	Float weight)
{
    // Apply each track on the Animatable
	IT_AnimationTrackList trackIt;

	for (trackIt = m_trackList.begin(); trackIt != m_trackList.end(); ++trackIt)
	{
		UInt32 VarSize = 0;
		AnimationTrack* Track = (*trackIt);
		const void* Value = Track->compute(target, time, VarSize);

		target->animate(
			Track->getType(), Value, VarSize,
			Track->getTarget(), Track->getSubTarget(),
			blendMode, weight);
	}

	// Recurse on the sons
	Animatable *son = target->getFirstSon();
	IT_AnimationNodeList animationIt = m_sonList.begin();

	while (target->hasMoreSons())
	{
		if (animationIt == m_sonList.end())
		{
			// the animation node has not enough sons
			return;
		}

		// recurse on the child
		(*animationIt)->update(son, time, blendMode, weight);

		animationIt++;
		son = target->getNextSon();
	}

	if (animationIt != m_sonList.end())
	{
		// the animation node has too much sons
		O3D_WARNING("Hierarchy tree and animation tree don't fit");
		return;
	}
}

/*---------------------------------------------------------------------------------------
  draw the trajectory of the object
---------------------------------------------------------------------------------------*/
void AnimationNode::drawTrajectory(Node *node)
{
	// draw the current node trajectory
	for (IT_AnimationTrackList trackIt = m_trackList.begin() ; trackIt != m_trackList.end() ; ++trackIt)
	{
		if ((*trackIt)->getTarget() == AnimationTrack::TARGET_OBJECT_POS && node->hasUpdatable())
			(*trackIt)->drawTrajectory(node);
	}

	// recurse on the son
	const T_SonList &sonList = node->getSonList();
	IT_AnimationNodeList animationIt = m_sonList.begin();

	for (CIT_SonList HierarchyIt = sonList.begin() ; HierarchyIt != sonList.end() ; ++HierarchyIt)
	{
		if (animationIt == m_sonList.end())
		{
			// the animation node has not enough sons
			return;
		}

		// recurse on the child
		if ((*HierarchyIt)->isNodeObject())
			(*animationIt)->drawTrajectory((Node*)(*HierarchyIt));

		animationIt++;
	}

	if (animationIt != m_sonList.end())
	{
		// the animation node has too much sons
        /*O3D_WARNING*/O3D_LOG(Logger::INFO, "Hierarchy tree and animation tree don't fit");
		return;
	}
}

/*---------------------------------------------------------------------------------------
  sort the list of track to put the rotate track at the beginning
---------------------------------------------------------------------------------------*/
void AnimationNode::sortTrackList()
{
	for (IT_AnimationTrackList t_it = m_trackList.begin() ; t_it != m_trackList.end() ; ++t_it)
	{
		AnimationTrack *pTrack = *t_it;

		if (pTrack->isRotationTarget())
		{
			// the track is a rotation, so put it at the beginning
			m_trackList.erase(t_it);
			m_trackList.push_front(pTrack);
		}
	}
}

/*---------------------------------------------------------------------------------------
  rescale recursively time of all tracks from start to end
---------------------------------------------------------------------------------------*/
void AnimationNode::rescaleTime(Float start, Float end)
{
	for (IT_AnimationTrackList t_it = m_trackList.begin() ; t_it != m_trackList.end() ; ++t_it)
	{
		T_KeyFrameList &KeyList = (*t_it)->getKeyFrameList();

		for (IT_KeyFrameList k_it = KeyList.begin() ; k_it != KeyList.end() ; ++k_it)
		{
			(*k_it)->setTime(((*k_it)->getTime()-start)/(end-start));
		}
	}

	// recurse on each child
	for (IT_AnimationNodeList n_it = m_sonList.begin() ; n_it != m_sonList.end() ; ++n_it)
		(*n_it)->rescaleTime(start,end);
}

/*---------------------------------------------------------------------------------------
  serialization
---------------------------------------------------------------------------------------*/
Bool AnimationNode::writeToFile(OutStream &os)
{
    os << getNumSon();
    os << getNumTrack();

	// write all tracks
	for (IT_AnimationTrackList trackIt = m_trackList.begin(); trackIt != m_trackList.end(); ++trackIt)
	{
		AnimationTrack *track = (*trackIt);

		AnimationTrack::TrackType trackType = track->getType();
		Evaluator::Type evalType = track->getEvaluatorType();
		// write track type and valuator type
        os   << trackType
		     << evalType;
		// finally export the track
        os << *track;
	}

	// recursively write all sons
	for (IT_AnimationNodeList animationIt = m_sonList.begin(); animationIt != m_sonList.end(); ++animationIt)
        os << *(*animationIt);

	return True;
}

Bool AnimationNode::readFromFile(InStream &is)
{
	Int32 numSons, numTracks;
	Int32 i;

	AnimationTrack::TrackType trackType = AnimationTrack::TRACK_TYPE_UNDEFINED;
	Evaluator::Type evaluator = Evaluator::UNDEFINED;

    is   >> numSons
		 >> numTracks;

	// read all tracks
	for (i = 0; i < numTracks; ++i)
	{
        is   >> trackType
		     >> evaluator;

		// create the track
		AnimationTrack *track;

        switch(evaluator)
		{
		case Evaluator::LINEAR:
			switch(trackType)
			{
			case AnimationTrack::TRACK_TYPE_FLOAT:
				track = new AnimationTrack_LinearFloat;
				break;
			case AnimationTrack::TRACK_TYPE_VECTOR:
				track = new AnimationTrack_LinearVector;
				break;
			case AnimationTrack::TRACK_TYPE_QUATERNION:
				track = new AnimationTrack_LinearQuaternion;
				break;
			default:
				// ERROR
                O3D_ERROR(E_InvalidFormat("(linear) track type"));
				break;
			}
			break;

		case Evaluator::BEZIER:
			switch(trackType)
			{
			case AnimationTrack::TRACK_TYPE_FLOAT:
				track = new AnimationTrack_BezierFloat;
				break;
			case AnimationTrack::TRACK_TYPE_VECTOR:
				track = new AnimationTrack_BezierVector;
				break;
			default:
				// ERROR
                O3D_ERROR(E_InvalidFormat("(bezier) track type"));
				break;
			}
			break;

		case Evaluator::TCB:
			switch(trackType)
			{
			case AnimationTrack::TRACK_TYPE_FLOAT:
				track = new AnimationTrack_TCBFloat;
				break;
			case AnimationTrack::TRACK_TYPE_VECTOR:
				track = new AnimationTrack_TCBVector;
				break;
			default:
				// ERROR
                O3D_ERROR(E_InvalidFormat("(TCB) track type"));
				break;
			}
			break;

		case Evaluator::CONSTANT:
			switch(trackType)
			{
			case AnimationTrack::TRACK_TYPE_BOOL:
				track = new AnimationTrack_ConstantBool;
				break;
			case AnimationTrack::TRACK_TYPE_FLOAT:
				track = new AnimationTrack_ConstantFloat;
				break;
			case AnimationTrack::TRACK_TYPE_VECTOR:
				track = new AnimationTrack_ConstantVector;
				break;
			case AnimationTrack::TRACK_TYPE_QUATERNION:
				track = new AnimationTrack_ConstantQuaternion;
				break;
			default:
				// ERROR
                O3D_ERROR(E_InvalidFormat("track type"));
				break;
			}
			break;

		case Evaluator::SMOOTH:
			switch(trackType)
			{
			case AnimationTrack::TRACK_TYPE_QUATERNION:
				track = new AnimationTrack_SmoothQuaternion;
				break;
			default:
				// ERROR
                O3D_ERROR(E_InvalidFormat("(smooth) track type"));
				break;
			}
			break;

		default:
            // ERROR
            O3D_ERROR(E_InvalidFormat("evaluator type"));
			break;
		}

		// add and read the track
		addTrackLast(*track);
        is >> *track;
	}

    //sortTrack();

	// read recursively all sons
	for (i = 0 ; i < numSons ; ++i)
	{
		AnimationNode *pNode = new AnimationNode(this);
		// add the new node to the list
		addSonLast(*pNode);
		// and read it recursively
        is >> *pNode;
	}

	return True;
}

