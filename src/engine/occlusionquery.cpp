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

OcclusionQuery::OcclusionQuery(Context *context) :
    m_id(O3D_UNDEFINED),
    m_visibleCount(0),
    m_occlusionType(NOT_AVAILABLE),
    m_context(context)
{
    O3D_ASSERT(m_context != nullptr);

    // Generate a new object
    glGenQueries(1, (GLuint*)&m_id);
}

OcclusionQuery::~OcclusionQuery()
{
    if (m_id != O3D_UNDEFINED) {
        glDeleteQueries(1, (GLuint*)&m_id);
    }
}

OcclusionQuery::Result OcclusionQuery::getOcclusionType()
{
	// we check if we already have a result
    if (m_occlusionType == NOT_AVAILABLE) {
		UInt32 tempResult;

        // @todo
        if (m_context->getRenderer()->isGLES()) {
            return NOT_OCCLUDED;
        }

		// Check if the result is ready
        glGetQueryObjectuiv(m_id, GL_QUERY_RESULT_AVAILABLE, (GLuint*)&tempResult);

        if (tempResult == 1) {
            // @todo how to for GLES
            if (!m_context->getRenderer()->isGLES()) {
                // The result is available gets the number of visible fragments
                glGetQueryObjectuiv(m_id, GL_QUERY_RESULT, (GLuint*)&m_visibleCount);
            } else {
                m_visibleCount = 1;
            }

            if (m_visibleCount > 0) {
				m_occlusionType = NOT_OCCLUDED;
            } else {
				m_occlusionType = OCCLUDED;
			}
		}
	}
	return m_occlusionType;
}

void OcclusionQuery::begin()
{
    // Check if we aren't already in a begin/end occlusion test
    if (!m_context->getCurrentOcclusionQuery()) {
        if (m_context->getRenderer()->isGLES()) {
            //glBeginQuery(GL_ANY_SAMPLES_PASSED, m_id);
        } else {
            glBeginQuery(GL_SAMPLES_PASSED, m_id);
        }
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
    if (m_context->getRenderer()->isGLES()) {
        //glEndQuery(GL_ANY_SAMPLES_PASSED);
    } else {
        glEndQuery(GL_SAMPLES_PASSED);
    }
    m_context->setCurrentOcclusionQuery(nullptr);
}

void OcclusionQuery::forceResults()
{
    if (m_occlusionType == NOT_AVAILABLE) {
        if (m_context->getRenderer()->isGLES()) {
            // true or false
            glGetQueryObjectuiv(m_id, GL_QUERY_RESULT_AVAILABLE, (GLuint*)&m_visibleCount);
        } else {
            // The result is available gets the number of visible fragments
            glGetQueryObjectuiv(m_id, GL_QUERY_RESULT, (GLuint*)&m_visibleCount);
        }

        if (m_visibleCount > 0) {
			m_occlusionType = NOT_OCCLUDED;
        } else {
			m_occlusionType = OCCLUDED;
		}
	}
}
