/**
 * @file screenviewport.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/screenviewport.h"
#include "o3d/engine/viewportmanager.h"

#include "o3d/core/classfactory.h"
#include "o3d/core/architecture.h"
#include "o3d/core/objects.h"
#include "o3d/engine/gl.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/picking.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(ScreenViewPort, ENGINE_VIEWPORT_SCREEN, ViewPort)

ScreenViewPort::ScreenViewPort(BaseObject *parent, Camera* camera, SceneDrawer *drawer) :
		ViewPort(parent, camera, drawer)
{
}

void ScreenViewPort::display(UInt32 w, UInt32 h)
{
	TimeMesure mesure(m_duration);

	if (m_isActive)	{
		UInt32 x,y;

        if (m_percent) {
			x = (UInt32)(m_xpos * w);
			y = (UInt32)(m_ypos * h);
			m_nWidth = (UInt32)(m_width * w);
			m_nHeight = (UInt32)(m_height * h);
        } else {
			x = (UInt32)m_xpos;
			y = (UInt32)m_ypos;
			m_nWidth = (UInt32)m_width;
			m_nHeight = (UInt32)m_height;
		}

		Float newCoef = (Float)m_nWidth / (Float)m_nHeight;

        if (m_camera->getRatio() != newCoef) {
			m_camera->setRatio(newCoef);
			m_camera->reCompute();
		}

        m_camera->getScene()->setActiveCamera(m_camera.get());
        m_camera->getScene()->getContext()->modelView().set(m_camera.get()->getModelviewMatrix());

		// redraw mode
        if (m_camera->getScene()->getViewPortManager()->isDrawPickingMode()) {
			Box2i viewport(x, y, m_nWidth, m_nHeight);

			Vector2f pos = m_camera->getScene()->getPicking()->getWindowPos();//getPickingPos();
			Vector2f size = m_camera->getScene()->getPicking()->getWindowSize();

			// if the picking is not processed into this area then return immediately
			if (((UInt32)pos[X] < x) || ((UInt32)pos[Y] < y) ||
				((UInt32)pos[X] > (x+m_nWidth-1)) || ((UInt32)pos[Y] > (y+m_nHeight-1)))
				return;

			// create size pixel picking region near cursor location
			Matrix4 regionMatrix = ProjectionMatrix::pickMatrix(pos, size, viewport);
			m_camera->getScene()->getContext()->projection().set(regionMatrix);
			m_camera->multProjectionMatrix();

			// call the redrawing method
			drawPicking();
        } else {
            // normal draw mode
			m_camera->getScene()->getContext()->setViewPort(x,y,m_nWidth,m_nHeight);
			m_camera->setProjectionMatrix();

			// call the drawing method
			draw();
		}
	}
}
