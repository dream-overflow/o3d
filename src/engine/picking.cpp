/**
 * @file picking.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"

#include "o3d/engine/glextdefines.h"
#include "o3d/engine/picking.h"

#include "o3d/core/architecture.h"
#include "o3d/engine/context.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/scene/sceneobjectmanager.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/viewport.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Picking, ENGINE_PICKING, SceneEntity)

// Add an object to check
Picking::Picking(BaseObject *parent) :
	SceneEntity(parent),
	m_mode(DISABLE),
	m_isPicking(False),
	m_redraw(False),
    m_size(5.f, 5.f),
	m_bypass(False),
	m_pointerMode(True),
    m_hit(False),
    m_hitPos(),
    m_x(0),
    m_y(0),
    m_stacksize(0),
	m_camera(this),
    m_fbo(getScene()->getContext())
{
}

// Add an object to check
Picking::~Picking()
{
}

// post a picking pass
void Picking::postPickingEvent(Int32 x, Int32 y)
{
	m_isPicking = True;

	m_x = x;
	m_y = y;

	m_position[X] = x - (m_size[X] * 0.5f);
	m_position[Y] = y - (m_size[Y] * 0.5f);
}

// set the viewport used for compute the hit pixel position
void Picking::setCamera(Camera *camera)
{
	m_camera = camera;
}

// pre-redraw the scene if picking
void Picking::preReDraw()
{
	// is a picking requested ?
    if (!m_isPicking) {
		return;
    }

	m_redraw = True;
	m_outObjectList.clear();

	// color mode
    if (m_mode == COLOR) {
		// create it for the first time
        if (!m_fbo.isValid()) {
            m_fbo.create((UInt32)m_size.x(), (UInt32)m_size.y(), PF_RGBA_8, True);
            m_fbo.attachColorRender(FrameBuffer::COLOR_ATTACHMENT0);
			m_fbo.attachDepthStencilRender(FrameBuffer::DEPTH_U32);
			m_fbo.isCompleteness();
        } else {
			m_fbo.bindBuffer();
		}

		// define the viewport size as picking region size
		getScene()->getContext()->setViewPort(0, 0, (Int32)m_size.x(), (Int32)m_size.y());
		m_oldBackground = getScene()->getContext()->setBackgroundColor(0.f, 0.f, 0.f, 0.f);

        // no blending
        getScene()->getContext()->blending().setDefaultFunc();

        m_fbo.clearColor(FrameBuffer::COLOR_ATTACHMENT0, Color(0.f, 0.f, 0.f, 0.f));
        m_fbo.clearDepth(1.0f);
    } else if (m_mode == HIGHLIGHTING) {
        // hybrid model
		// TODO
	}
}

// post-redraw the scene if picking, hits can be checked after this call
void Picking::postReDraw()
{
	// is a picking is asked ?
    if (!m_isPicking) {
		return;
    }

	m_hit = False;
	m_redraw = False;

	// color mode
    if (m_mode == COLOR) {
		getScene()->getContext()->setBackgroundColor(m_oldBackground);

		// get the hit 3d point (on current rendered scene)
		Float pixel[4];
		Int32 depth;
		Float floatDepth;

        // @todo read buffer from FrameBuffer::setReadBuffer();
		glReadBuffer(FrameBuffer::COLOR_ATTACHMENT0 + GL_COLOR_ATTACHMENT0);
		glReadPixels((GLint)(m_size.x() * 0.5f), (GLint)(m_size.y() * 0.5), 1, 1, GL_RGBA, GL_FLOAT, pixel);

		// picking color key
        UInt32 id =  (UInt32)(pixel[0] * 255.f) +
                    ((UInt32)(pixel[1] * 255.f) << 8) +
                    ((UInt32)(pixel[2] * 255.f) << 16) +
                    ((UInt32)(pixel[3] * 255.f) << 24);

		// get the hit depth in integer for sorting, and float for compute the hit position
		glReadPixels((GLint)(m_size.x() * 0.5f), (GLint)(m_size.y() * 0.5), 1, 1, GL_DEPTH_COMPONENT, GL_INT, &depth);
		glReadPixels((GLint)(m_size.x() * 0.5f), (GLint)(m_size.y() * 0.5), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &floatDepth);

        // @todo read buffer from FrameBuffer::ResetReadBuffers();
		m_fbo.unbindBuffer();
		glReadBuffer(GL_BACK);

        // hit
        if (id > 0 && id < O3D_MAX_UINT32) {
            // find pickable id
			Pickable *object = getScene()->getSceneObjectManager()->get(id);
            if (object) {
                if (m_camera) {
					// get the hit 3d point (on current rendered scene)
					m_hitPos = Matrix::unprojectPoint(
							getScene()->getContext()->projection().get(),  // projection matrix assigned by the viewport
							m_camera->getModelviewMatrix(),
							Box2i(0, 0, (Int32)m_size.x(), (Int32)m_size.y()),
							Vector3(m_size.x() * 0.5f, m_size.y() * 0.5f, floatDepth));
				}

				// found so add it to results
				m_hit = True;
				m_outObjectList.insert(Hit(object, depth, m_hitPos));

				onHit(object, m_hitPos);
            } else {
                // hit but no pickable found
                m_hit = True;

                onUnknownHit(id, m_hitPos);
			}
		}
    } else if (m_mode == HIGHLIGHTING) {
        // hybrid model
        // @todo
	}

	// picking processed
	m_isPicking = False;
}

// Get from a 2d position a 3d coordinate
Vector3 Picking::getPointerPos(Int32 x, Int32 y)
{
    if (m_camera) {
		Float depth;
		glReadPixels(
				x,
				y,
				1,
				1,
				GL_DEPTH_COMPONENT,
				GL_FLOAT,
				&depth);

		return Matrix::unprojectPoint(
				m_camera->getProjectionMatrix(),
				m_camera->getModelviewMatrix(),
				getScene()->getContext()->getViewPort(),
				Vector3((Float)x, (Float)y, depth));
    } else {
		return Vector3();
    }
}
