/**
 * @file drawbuffers.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/drawbuffers.h"
#include "o3d/engine/context.h"
#include "o3d/engine/glextensionmanager.h"

using namespace o3d;

//! Default constructor.
DrawBuffers::DrawBuffers(Context *context) :
    m_context(context),
    m_drawBuffers(UInt32(COLOR_ATTACHMENT0), 0, 1)
{
    O3D_ASSERT(m_context != nullptr);
}

DrawBuffers::~DrawBuffers()
{

}

void DrawBuffers::apply() const
{
    if (m_drawBuffers.isValid())
        glDrawBuffers(m_drawBuffers.getSize(), m_drawBuffers.getData());
}

void DrawBuffers::reset() const
{
    glDrawBuffer(COLOR_ATTACHMENT0);
}

/*
    if (active)
    {
        glDrawBuffer(GL_BACK);
        glReadBuffer(GL_BACK);
    }
    else
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }*/

