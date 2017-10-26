/**
 * @file targetnode.cpp
 * @brief Implementation of TargetNode.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-07-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/hierarchy/targetnode.h"

#include "o3d/core/classfactory.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/scene/sceneobjectmanager.h"

#include "o3d/engine/animation/animationtrack.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(TargetNode, ENGINE_NODE_TARGET, Node)

//---------------------------------------------------------------------------------------
// Constructors
//---------------------------------------------------------------------------------------
TargetNode::TargetNode(BaseObject *parent) :
	Node(parent),
	m_target(this),
	m_minDist(0.f),
	m_maxDist(0.f),
	m_roll(0.f),
	m_targetId(-1)
{
	m_transformTarget = new MTransform;
	m_transformTarget->setName("default");

	// add a default transform (used to compute the target direction)
	m_transformList.push_back(m_transformTarget);
}

TargetNode::TargetNode(const TargetNode &dup) :
	Node(dup),
	m_target(this,dup.m_target),
	m_minDist(dup.m_minDist),
	m_maxDist(dup.m_maxDist),
	m_roll(0.f),
	m_targetId(-1)
{
	m_transformTarget = new MTransform;
	m_transformTarget->setName("default");
	m_transformTarget->setMatrix(dup.m_transformTarget->getMatrix());

	// add a default transform (used to compute the target direction)
	m_transformList.push_back(m_transformTarget);
}

//---------------------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------------------
TargetNode::~TargetNode()
{
}

// Duplicate
TargetNode& TargetNode::operator=(const TargetNode &dup)
{
	Node::operator =(dup);

	m_minDist = dup.m_minDist;
	m_maxDist = dup.m_maxDist;
	m_target = dup.m_target;

	return *this;
}

// Update the branche
void TargetNode::update()
{
	if (!getActivity())
		return;

	// compute target direction
	if (m_target.isValid() && m_target->hasUpdated())
	{
		// @todo TEST perhaps try a kind of lookAt with a Y+ up axis
		Vector3 v = m_target->getAbsoluteMatrix().getTranslation() - m_worldMatrix.getTranslation();
		Float len = v.length();

		if (len > m_maxDist)
		{
			len = m_maxDist - len;
			m_transformTarget->translate(Vector3(0.f,0.f,len));
			v[Z] += len;
		}
		else if (len < m_minDist)
		{
			len = m_minDist - len;
			m_transformTarget->translate(Vector3(0.f,0.f,len));
			v[Z] += len;
		}

		Matrix4 cameraToTarget(m_transformTarget->getMatrix());
		cameraToTarget.reComputeBasisGivenZ(v);

		m_transformTarget->setMatrix(cameraToTarget);
	}

	TargetNode::update();
}

//---------------------------------------------------------------------------------------
// serialisation
//---------------------------------------------------------------------------------------
Bool TargetNode::writeToFile(OutStream &os)
{
    if (!Node::writeToFile(os))
		return False;

    os   << m_minDist
		 << m_maxDist;

	if (m_target.isValid())
        os << m_target->getSerializeId();
	else
        os << Int32(-1);

	return True;
}

Bool TargetNode::readFromFile(InStream &is)
{
    if (!Node::readFromFile(is))
		return False;

    is   >> m_minDist
		 >> m_maxDist
		 >> m_targetId;

	return True;
}

void TargetNode::postImportPass()
{
	Node::postImportPass();

	if (m_targetId != -1)
		m_target = getScene()->getSceneObjectManager()->getImportedSceneObject(m_targetId);
}

