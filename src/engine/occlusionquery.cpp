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
#include "o3d/engine/renderer.h"

using namespace o3d;

OcclusionQuery::OcclusionQuery(Context *context, SamplePassType type) :
    QueryObject(context),
    m_samplePassType(type),
    m_visibleCount(0),
    m_occlusionType(NOT_AVAILABLE)
{
    // Generate a new object
    glGenQueries(1, (GLuint*)&m_queryId);
}

OcclusionQuery::~OcclusionQuery()
{
    if (m_queryId != O3D_UNDEFINED) {
        glDeleteQueries(1, (GLuint*)&m_queryId);
    }
}

OcclusionQuery::Result OcclusionQuery::getOcclusionType()
{
	// we check if we already have a result
    if (m_occlusionType == NOT_AVAILABLE) {
		UInt32 tempResult;

		// Check if the result is ready
        glGetQueryObjectuiv(m_queryId, GL_QUERY_RESULT_AVAILABLE, (GLuint*)&tempResult);

        if (tempResult == 1) {
            if (m_samplePassType == ANY_SAMPLES_PASSED) {
                // true or false
                glGetQueryObjectuiv(m_queryId, GL_QUERY_RESULT, (GLuint*)&m_visibleCount);

                if (m_visibleCount == 0) {
                    m_occlusionType = OCCLUDED;
                } else {
                    m_occlusionType = NOT_OCCLUDED;
                }
            } else if (m_samplePassType == SAMPLES_PASSED) {
                // The result is available gets the number of visible fragments
                glGetQueryObjectuiv(m_queryId, GL_QUERY_RESULT, (GLuint*)&m_visibleCount);

                if (m_visibleCount > 0) {
                    m_occlusionType = NOT_OCCLUDED;
                } else {
                    m_occlusionType = OCCLUDED;
                }
            }
		}
	}
	return m_occlusionType;
}

void OcclusionQuery::begin()
{
    // Check if we aren't already in a begin/end occlusion test
    if (!m_context->getCurrentOcclusionQuery()) {
        glBeginQuery(m_samplePassType, m_queryId);
        m_context->setCurrentOcclusionQuery(this);

		// Reset parameters
		m_visibleCount = 0;
		m_occlusionType = NOT_AVAILABLE;
    } else {
		O3D_ERROR(E_InvalidParameter("An existing occlusion query is currently performing"));
	}
}

void OcclusionQuery::end()
{
    glEndQuery(m_samplePassType);
    m_context->setCurrentOcclusionQuery(nullptr);
}

void OcclusionQuery::forceResults()
{
    if (m_occlusionType == NOT_AVAILABLE) {
        if (m_samplePassType == ANY_SAMPLES_PASSED) {
            // true or false
            glGetQueryObjectuiv(m_queryId, GL_QUERY_RESULT, (GLuint*)&m_visibleCount);

            if (m_visibleCount == 0) {
                m_occlusionType = OCCLUDED;
            } else {
                m_occlusionType = NOT_OCCLUDED;
            }
        } else if (m_samplePassType == SAMPLES_PASSED) {
            // The result is available gets the number of visible fragments
            glGetQueryObjectuiv(m_queryId, GL_QUERY_RESULT, (GLuint*)&m_visibleCount);

            if (m_visibleCount > 0) {
                m_occlusionType = NOT_OCCLUDED;
            } else {
                m_occlusionType = OCCLUDED;
            }
        }
	}
}
