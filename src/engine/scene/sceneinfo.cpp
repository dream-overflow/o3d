/**
 * @file sceneinfo.cpp
 * @brief Implementation of SceneInfo.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-07-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/scene/sceneinfo.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

using namespace o3d;

// Default constructor
SceneInfo::SceneInfo() :
	m_sceneName("Unamed"),
	m_author("Undefined"),
	m_copyright("Undefined"),
	m_revision(0),
	m_unit(0.1f),
	m_unitName("decimetre")
{
	m_created.setCurrent();
	m_modified.setCurrent();
}

// Serialisation
Bool SceneInfo::writeToFile(OutStream &os)
{
	// current date
	m_modified.setCurrent();

	// new revision
	m_revision += 1;

    os   << m_sceneName
	     << m_title
	     << m_author
	     << m_copyright
	     << m_comment
	     << m_subject
	     << m_revision
	     << m_created
	     << m_modified
	     << m_unit
	     << m_unitName;

	return True;
}

Bool SceneInfo::readFromFile(InStream &is)
{
    is   >> m_sceneName
	     >> m_title
	     >> m_author
	     >> m_copyright
	     >> m_comment
	     >> m_subject
	     >> m_revision
	     >> m_created
	     >> m_modified
	     >> m_unit
	     >> m_unitName;

	return True;
}

