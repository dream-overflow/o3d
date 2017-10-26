/**
 * @file map2ddrawer.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/map2d/map2ddrawer.h"
#include "o3d/engine/glextdefines.h"

#include "o3d/core/architecture.h"
#include "o3d/engine/context.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/scene/sceneobjectmanager.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/object/light.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/renderer.h"
#include "o3d/engine/viewport.h"
#include "o3d/geom/frustum.h"
#include "o3d/engine/effect/specialeffectsmanager.h"
#include "o3d/engine/visibility/visibilitymanager.h"
#include "o3d/engine/landscape/landscape.h"
#include "o3d/engine/alphapipeline.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Map2dDrawer, ENGINE_MAP_2D_DRAWER, SceneDrawer)

Map2dDrawer::Map2dDrawer(BaseObject *parent) :
	SceneDrawer(parent)
{

}

void Map2dDrawer::draw()
{
	// The camera modelview should be set before draw()
	if (getScene()->getActiveCamera() == NULL)
		return;

	if (!m_map2d.isValid())
		return;

	Context &context = *getScene()->getContext();
	Camera &camera = *getScene()->getActiveCamera();

	// Computes frustum just after camera put
	getScene()->getFrustum()->computeFrustum(context.projection().get(), camera.getModelviewMatrix());

	// Determines visible objects
	getScene()->getVisibilityManager()->processVisibility();

	//
	// Draw first pass without lighting, in camera space
	//

	// draw the first pass with ambient
	DrawInfo drawInfo(DrawInfo::AMBIENT_PASS);

	context.disableStencilTest();
	context.setDefaultDepthFunc();
	context.setDefaultDepthTest();
	context.setDefaultDepthWrite();
	context.setDefaultDepthFunc();

	// map2d draw using the visibility controller
	m_map2d->draw(drawInfo);

    // camera clear
    camera.clearCameraChanged();
}

void Map2dDrawer::drawPicking()
{
	// The camera modelview should be set before draw()
	if (getScene()->getActiveCamera() == NULL)
		return;

	if (!m_map2d.isValid())
		return;

	Context &context = *getScene()->getContext();
	Camera &camera = *getScene()->getActiveCamera();

	// Computes frustum just after camera put
	getScene()->getFrustum()->computeFrustum(context.projection().get(), camera.getModelviewMatrix());

	// Determines visible objects
	getScene()->getVisibilityManager()->processVisibility();

	//
	// Draw first pass with no lighting, in camera space
	//

	// draw the first pass in picking mode
	DrawInfo drawInfo(DrawInfo::PICKING_PASS);

	context.disableStencilTest();
	context.setDefaultDepthFunc();
	context.setDefaultDepthTest();
	context.setDefaultDepthWrite();
	context.setDefaultDepthFunc();

	// map2d
	m_map2d->draw(drawInfo);

    // camera clear
    camera.clearCameraChanged();
}

