/**
 * @file primitivequery.h
 * @brief This class is an interface for a primitive query object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2018-01-10
 * @copyright Copyright (c) 2001-2018 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/primitivequery.h"

#include "o3d/core/debug.h"
#include "o3d/engine/glextdefines.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/context.h"
#include "o3d/engine/renderer.h"

using namespace o3d;

PrimitiveQuery::PrimitiveQuery(Context *context, PrimitiveType type) :
    QueryObject(context),
    m_primitiveType(type),
    m_primitiveCount(0)
{
    // Generate a new object
    glGenQueries(1, (GLuint*)&m_queryId);
}

PrimitiveQuery::~PrimitiveQuery()
{
    if (m_queryId != O3D_UNDEFINED) {
        glDeleteQueries(1, (GLuint*)&m_queryId);
    }
}

void PrimitiveQuery::begin()
{

}

void PrimitiveQuery::end()
{

}

void PrimitiveQuery::forceResults()
{

}
