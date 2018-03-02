/**
 * @file animationnode.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ANIMATIONNODE_H
#define _O3D_ANIMATIONNODE_H

#include "o3d/core/memorydbg.h"
#include "../hierarchy/node.h"
#include "animationtrack.h"
#include "animatable.h"

#include "o3d/core/file.h"

namespace o3d {

class AnimationNode;

typedef std::list<AnimationNode*> T_AnimationNodeList;
typedef T_AnimationNodeList::iterator IT_AnimationNodeList;
typedef T_AnimationNodeList::const_iterator CIT_AnimationNodeList;

/**
 * @brief An animation node.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-02-23
 * An animation node is used in Animation. And each node has a list of track wich
 * contains the animations settings.
 */
class O3D_API AnimationNode : public Serialize
{
	O3D_DEFINE_SERIALIZATION(AnimationNode)

public:

	//! Default constructor.
    AnimationNode(AnimationNode* father = nullptr);

	//! Virtual destructor.
	virtual ~AnimationNode();

	//! Update an object time is [0..1].
	void update(
			Animatable *target,
			Float time,
			enum Animation::BlendMode blendMode,
			Float weight);

	//! Draw the trajectory of the object.
    void drawTrajectory(Node *node, const DrawInfo &drawInfo);

	//! Add a son at back of the list.
	inline void addSon(AnimationNode &node)
	{
		m_sonList.push_back(&node);
		node.setFather(this);
	}

	//! add a son at front/back of the list
	inline void addSonFirst(AnimationNode &node)
	{
		m_sonList.push_front(&node);
		node.setFather(this);
	}

	inline void addSonLast(AnimationNode &node)
	{
		m_sonList.push_back(&node);
		node.setFather(this);
	}

	//! Get the son list of nodes (read only).
	inline const T_AnimationNodeList& getSonList() const { return m_sonList; }
	//! Get the son list of nodes.
	inline T_AnimationNodeList& getSonList() { return m_sonList; }

	//! Define the father track node.
	inline void setFather(AnimationNode* father) { m_animFather = father; }
	//! Get the father track node (read only).
	inline const AnimationNode* getFather() const { return m_animFather; }
	//! Get the father track node.
	inline AnimationNode* getFather() { return m_animFather; }

	//! Add a track at back of the list.
	inline void addTrack(AnimationTrack &Track) { m_trackList.push_back(&Track); }

	//! Add a track at front of the track list.
	inline void addTrackFirst(AnimationTrack &Track) { m_trackList.push_front(&Track); }

	//! Add a track at back of the track list.
	inline void addTrackLast(AnimationTrack &Track) { m_trackList.push_back(&Track); }

	//! Get the number of sons.
	inline Int32 getNumSon() const { return (Int32)m_sonList.size(); }

	//! Get the number of tracks.
	inline Int32 getNumTrack() const { return (Int32)m_trackList.size(); }

	//! sort the list of track to put the rotate track at the beginning
	void sortTrackList();

	//! find the nearest time of a track for a given time
	void findTrackTime(
			Animatable *target,
			Float startTime,
			Float endTime,
			Float &newStartTime,
			Float &newEndTime) const;

	//! Precompute tracks range for a defined range.
	void computeRange(Float startTime, Float endTime);

	//! Position the animatable target for a defined range of track.
	void useRange(Animatable* target, UInt32 id) const;

	//! Use the full animation range for the animatable target.
	void defineFullRange(Animatable *target) const;

	//! Get the tracks list (read only).
	inline const T_AnimationTrackList& getTrackList() const { return m_trackList; }

	//! Get the tracks list.
	inline T_AnimationTrackList& getTrackList() { return m_trackList; }

	//! Re-scale recursively time of all tracks from start to end.
	void rescaleTime(Float start, Float end);

	// serialization
    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

private:

	T_AnimationTrackList m_trackList;   //!< List of tracks.
	AnimationNode *m_animFather;        //!< Father node.
	T_AnimationNodeList m_sonList;      //!< List of son animation nodes.
};

} // namespace o3d

#endif // _O3D_ANIMATIONNODE_H
