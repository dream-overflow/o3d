/**
 * @file occlusionquery.cpp
 * @brief Implementation of OcclusionQuery.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-08-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/occlusionquery.h"

#include "o3d/core/architecture.h"
#include "o3d/core/debug.h"
#include "o3d/engine/glextdefines.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/context.h"

using namespace o3d;

/*---------------------------------------------------------------------------------------
  constructor
---------------------------------------------------------------------------------------*/
OcclusionQuery::OcclusionQuery()
{
	glGenQueries(1,(GLuint*)&m_id);  // Generate a new object
	m_visibleCount = 0;
	m_occlusionType = NOT_AVAILABLE;
}

/*---------------------------------------------------------------------------------------
  destructor
---------------------------------------------------------------------------------------*/
OcclusionQuery::~OcclusionQuery()
{
	glDeleteQueries(1,(GLuint*)&m_id);
}

/*---------------------------------------------------------------------------------------
  return if primitives are occluded or visible
---------------------------------------------------------------------------------------*/
OcclusionQuery::Result OcclusionQuery::getOcclusionType()
{
	// we check if we already have a result
	if (m_occlusionType == NOT_AVAILABLE)
	{
		UInt32 tempResult;

		// Check if the result is ready
		glGetQueryObjectuiv(m_id,GL_QUERY_RESULT_AVAILABLE,(GLuint*)&tempResult);

		if (tempResult == 1)
		{
			// The result is available gets the number of visible fragments
			glGetQueryObjectuiv(m_id,GL_QUERY_RESULT,(GLuint*)&m_visibleCount);

			if (m_visibleCount > 0)
			{
				m_occlusionType = NOT_OCCLUDED;
			}
			else
			{
				m_occlusionType = OCCLUDED;
			}
		}
	}
	return m_occlusionType;
}

/*---------------------------------------------------------------------------------------
  Start/End the occlusion test
---------------------------------------------------------------------------------------*/
void OcclusionQuery::begin(Context *pContext)
{
	// Check if we aren't already in a Begin End
	// Occlusion test
	if (!pContext->getCurrentOcclusionQuery())
	{
		glBeginQuery(GL_SAMPLES_PASSED,m_id);
		pContext->setCurrentOcclusionQuery(this);

		// Reset parameters
		m_visibleCount = 0;
		m_occlusionType = NOT_AVAILABLE;
	}
	else
	{
		O3D_ERROR(E_InvalidParameter("An existing occlusion query is currently performing"));
	}
}

void OcclusionQuery::end(Context *pContext)
{
	glEndQuery(GL_SAMPLES_PASSED);
	pContext->setCurrentOcclusionQuery(NULL);
}

/*---------------------------------------------------------------------------------------
  Force the driver to get the results
---------------------------------------------------------------------------------------*/
void OcclusionQuery::forceResults()
{
	if (m_occlusionType == NOT_AVAILABLE)
	{
		// The result is available gets the number of visible fragments
		glGetQueryObjectuiv(m_id,GL_QUERY_RESULT,(GLuint*)&m_visibleCount);

		if (m_visibleCount > 0)
		{
			m_occlusionType = NOT_OCCLUDED;
		}
		else
		{
			m_occlusionType = OCCLUDED;
		}
	}
}

