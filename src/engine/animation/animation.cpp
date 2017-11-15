/**
 * @file animation.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-02-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/animation/animation.h"

#include "o3d/engine/animation/animationnode.h"
#include "o3d/engine/hierarchy/node.h"
#include "o3d/core/templatemanager.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/classfactory.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Animation, ENGINE_ANIMATION, SceneResource)

/*---------------------------------------------------------------------------------------
  constructor
---------------------------------------------------------------------------------------*/
Animation::Animation(BaseObject *parent) :
	SceneResource(parent),
	m_fatherNode(NULL),
	m_numObjects(0),
	m_duration(1.f),
	m_computed(False)
{
}

/*---------------------------------------------------------------------------------------
  destructor
---------------------------------------------------------------------------------------*/
Animation::~Animation()
{
	deletePtr(m_fatherNode);
}

/*---------------------------------------------------------------------------------------
  get an anim range by its name (return false if not found)
---------------------------------------------------------------------------------------*/
Bool Animation::getAnimRange(const String &name, AnimRange &range)
{
	for (UInt32 i = 0 ; i < m_animRange.size() ; ++i)
	{
		if (m_animRange[i].name.length() && (m_animRange[i].name == name))
		{
			range.start = m_animRange[i].start;
			range.end = m_animRange[i].end;
			range.breaking = m_animRange[i].breaking;
			range.name = m_animRange[i].name;

			return True;
		}
	}
	return False;
}

/*---------------------------------------------------------------------------------------
  update the animation subtree (0..t..1)
---------------------------------------------------------------------------------------*/
void Animation::update(
	class Animatable *anim,
	Float time,
	Animation::BlendMode blendMode,
	Float weight)
{
	if (m_fatherNode)
		m_fatherNode->update(anim, time, blendMode, weight);
}

/*---------------------------------------------------------------------------------------
  round to the nearest keyframes
---------------------------------------------------------------------------------------*/
void Animation::roundToNearestKeyFrame(
	Animatable* target,
	Float &stime,
	Float &etime)
{
	Float nstime,netime;

	if (m_fatherNode)
		m_fatherNode->findTrackTime(target, stime, etime, nstime, netime);
	else
		nstime = netime = 0.f;

	stime = nstime;
	etime = netime;
}

/*---------------------------------------------------------------------------------------
  use an anim range for pTarget
---------------------------------------------------------------------------------------*/
void Animation::useAnimRange(Animatable* target, UInt32 id) const
{
	O3D_ASSERT(id < (UInt32)m_animRange.size());
	if (m_fatherNode)
		m_fatherNode->useRange(target, id);
}

/*---------------------------------------------------------------------------------------
  use full range anim for pTarget
---------------------------------------------------------------------------------------*/
void Animation::useFullRange(Animatable* target) const
{
	if (m_fatherNode)
		m_fatherNode->defineFullRange(target);
}

/*---------------------------------------------------------------------------------------
  precompute all animation range for tracks
---------------------------------------------------------------------------------------*/
void Animation::computeAnimRange()
{
	if (!m_fatherNode)
		return;

	Float inv = 1.f / (Float)(m_frame-1);

	for (UInt32 i = 0 ; i < m_animRange.size() ; ++i)
	{
		m_fatherNode->computeRange(m_animRange[i].start * inv, m_animRange[i].end * inv);
	}

	m_computed = True;
}

/*---------------------------------------------------------------------------------------
  draw the trajectories of all subtree
---------------------------------------------------------------------------------------*/
void Animation::drawTrajectory(Node *currentNode)
{
	if (m_fatherNode)
		m_fatherNode->drawTrajectory(currentNode);
}

/*---------------------------------------------------------------------------------------
  according to the node that is selected in the hierarchy tree, compute the corresponding
---------------------------------------------------------------------------------------*/
AnimationNode* Animation::computeStartNode(
	AnimationNode *currentAnimationNode,
	Node *currentNode,
	Node *selectedNode)
{
	if (currentNode == selectedNode)
		return currentAnimationNode;

	// recurse sons
	const T_SonList &SonList = currentNode->getSonList();
	T_AnimationNodeList &AnimationSonList = currentAnimationNode->getSonList();
	IT_AnimationNodeList a_it = AnimationSonList.begin();

	for (CIT_SonList n_it = SonList.begin(); n_it != SonList.end() ; ++n_it)
	{
		if (a_it == AnimationSonList.end())
		{
			// animation node has not enough sons
			O3D_WARNING("Animation node has not enough sons");
			return NULL;
		}

		// recurse childs
		if ((*n_it)->isNodeObject())
		{
			AnimationNode *pRet = computeStartNode(*a_it, ((Node*)(*n_it)), selectedNode);

			if (pRet != NULL)
				return pRet;
		}

		++a_it;
	}

	if (a_it != AnimationSonList.end())
	{
		// animation node has too much sons
		O3D_WARNING("Animation node has too much sons");
		return NULL;
	}

	return NULL;
}

/*---------------------------------------------------------------------------------------
  serialisation
---------------------------------------------------------------------------------------*/
Bool Animation::writeToFile(
    OutStream &os,
	Node *hierarchyRoot,
	Node *selectedNode)
{
    BaseObject::writeToFile(os);

	// set animation name
	m_name = selectedNode->getName();

	// write animation data
    os   << m_numObjects
		 << m_frame
		 << m_duration;

	// write all animation ranges
    os << (Int32)m_animRange.size();

	for (IT_AnimRangeVector it = m_animRange.begin() ; it != m_animRange.end() ; ++it)
	{
        os   << (*it).start
			 << (*it).end
			 << (*it).breaking
			 << (*it).name;
	}

	// compute the animation node corresponding to the hierarchy selected node
	AnimationNode *pStartNode = computeStartNode(m_fatherNode, hierarchyRoot, selectedNode);

	if (pStartNode == NULL)
	{
        O3D_ERROR(E_InvalidFormat("Can't retrieve the root node of the animation"));
	}
	else
	{
        os << *pStartNode; // write the subtree animation node
	}

	return True;
}

Bool Animation::writeToFile(OutStream &os)
{
    BaseObject::writeToFile(os);

	// write animation data
    os   << m_numObjects
		 << m_frame
		 << m_duration;

	// write all animation ranges
    os << (Int32)m_animRange.size();

	for (IT_AnimRangeVector it = m_animRange.begin(); it != m_animRange.end(); ++it)
	{
        os   << (*it).start
			 << (*it).end
			 << (*it).breaking
			 << (*it).name;
	}

	// write the sub tree of animation node
    os << *m_fatherNode;

	return True;
}

Bool Animation::readFromFile(InStream &is)
{
    BaseObject::readFromFile(is);

	// read animation data
    is   >> m_numObjects
		 >> m_frame
		 >> m_duration;

	// read all animation ranges
	Int32 numRange;
    is >> numRange;

	for (Int32 i = 0; i < numRange; ++i)
	{
		AnimRange range;

        is   >> range.start
			 >> range.end
			 >> range.breaking
		     >> range.name;

		m_animRange.push_back(range);
	}

	// create the father node
    m_fatherNode = new AnimationNode(nullptr);
	// read recursively the hierarchy
    is >> *m_fatherNode;

	// finally precompute animation range tracks keys
	computeAnimRange();

	return True;
}

// Save the animation file (*.o3dan) into the specified path and using the defined filename
Bool Animation::save()
{
	// need to define a filename
	if (m_filename.isEmpty())
		O3D_ERROR(E_InvalidPrecondition("The animation file name must be previously defined"));

	// open the file
    FileOutStream *os = FileManager::instance()->openOutStream(m_filename, FileOutStream::CREATE);

	// write the animation according to its object type
    if (!ClassFactory::writeToFile(*os, *this))
	{
        deletePtr(os);
		return False;
	}

    deletePtr(os);
	return True;
}

