/**
 * @file shadowvolumeforward.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"

#include "o3d/engine/glextdefines.h"
#include "o3d/engine/shadow/shadowvolumeforward.h"

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

O3D_IMPLEMENT_ABSTRACT_CLASS(ShadowRenderer, ENGINE_SHADOW_RENDERER)

O3D_IMPLEMENT_DYNAMIC_CLASS2(ShadowVolumeForward, ENGINE_SHADOW_VOLUME_FORWARD, SceneDrawer, ShadowRenderer)

ShadowVolumeForward::ShadowVolumeForward(BaseObject *parent) :
	SceneDrawer(parent)
{
}

void ShadowVolumeForward::processLight(Light *light)
{
    if ((light != nullptr) && light->getActivity())
	{
        Context &context = *getScene()->getContext();
        Camera &camera = *getScene()->getActiveCamera();

        DrawInfo drawInfo(DrawInfo::AMBIENT_PASS);
        drawInfo.setFromCamera(&camera);

		if (0)//light->getShadowCast())
		{
			//
			// Process the shadow casters according to the light
			//

			getScene()->getRenderer()->clearStencilBuffer();

			context.enableStencilTest();
			context.setStencilTestFunc(COMP_ALWAYS, 0, 0xffffffff);
			context.disableDepthWrite();
			context.disableColorWrite();
            context.blending().setFunc(Blending::DISABLED);
			context.enableDepthTest();
			context.setDepthFunc(COMP_LESS);
			context.disableDoubleSide();

			// infinite view frustum
			Matrix4 infiniteProj, oldProjection;
			infiniteProj.buildPerspective(
						camera.getRatio(),
						camera.getFov(),
						camera.getZnear());

			oldProjection = context.projection().get();
			//context.projection().set(infiniteProj);

			drawInfo.setFromLight(light);
			drawInfo.pass = DrawInfo::SHADOW_PASS;
			drawInfo.light.shadowType = Shadowable::SHADOW_VOLUME;

			// world objects
			getScene()->getVisibilityManager()->draw(drawInfo);

			// restore the z far finite camera projection matrix
			context.projection().set(oldProjection);

			context.setStencilTestFunc(COMP_EQUAL, 0, 0xffffffff);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // TODO STENCIL object in context
			context.enableColorWrite();
			context.enableDepthWrite();
			context.setDefaultCullingMode();
			context.setDepthFunc(COMP_EQUAL);
		}

		//
		// Draw the next pass with lighting
		//

		// reset to the active camera modelview
		context.modelView().set(camera.getModelviewMatrix());

		// only replace fragments
		context.setDepthFunc(COMP_EQUAL);

		drawInfo.setFromLight(light);

		// world objects
		getScene()->getVisibilityManager()->draw(drawInfo);
	}
}

void ShadowVolumeForward::draw()
{
	// The camera modelview should be set before draw()
    if (getScene()->getActiveCamera() == nullptr) {
		return;
    }

	Context &context = *getScene()->getContext();
	Camera &camera = *getScene()->getActiveCamera();

	// Computes frustum just after camera put
	getScene()->getFrustum()->computeFrustum(context.projection().get(), camera.getModelviewMatrix());

	// Determines visible objects
	getScene()->getVisibilityManager()->processVisibility();

	//
	// Draw first pass with no lighting, in camera space
	//

    // use MSAA if MS renderer
    if (getScene()->getRenderer()->getSamples() > 1) {
        context.setAntiAliasing(Context::AA_MULTI_SAMPLE);
    }

	// draw the first pass with ambient
	DrawInfo drawInfo(DrawInfo::AMBIENT_PASS);
    drawInfo.setFromCamera(&camera);

	context.disableStencilTest();
	context.setDefaultDepthFunc();

	// landscape
	getScene()->getLandscape()->draw();

	// world objects
	getScene()->getVisibilityManager()->draw(drawInfo);

	// special effects
	getScene()->getSpecialEffectsManager()->draw(drawInfo);

	context.disableDepthWrite();
		getScene()->getAlphaPipeline()->sort();
		getScene()->getAlphaPipeline()->draw(drawInfo);
	context.setDefaultDepthWrite();

    //
    // Process the effectives lights
    //

//	 TODO a manager to get visible objects for any active light
//    for each light
//      for each visible mesh
//        if light volume intersects mesh
//          render

    const String lights[] = { "light1", "light2", "light3", "light4" };

    for (UInt32 i = 0; i < 4; ++i)
	{
		Light *light = dynamicCast<Light*>(getScene()->getSceneObjectManager()->searchName(lights[i]));
		processLight(light);
	}

	context.disableStencilTest();
	context.setDefaultStencilTestFunc();
	context.setDefaultDepthTest();
	context.setDefaultDepthWrite();
	context.setDefaultDepthFunc();

	//
	// Draw the next pass with post effects
	//

	drawInfo.pass = DrawInfo::AMBIENT_PASS;
	drawInfo.light.type = 0;

	// post effects
	//getScene()->getSpecialEffectsManager()->drawPost(drawInfo);

    context.setAntiAliasing(Context::AA_NONE);

    // camera clear
    camera.clearCameraChanged();
}

void ShadowVolumeForward::drawPicking()
{
	// The camera modelview should be set before draw()
    if (getScene()->getActiveCamera() == nullptr)
		return;

    Camera &camera = *getScene()->getActiveCamera();

	DrawInfo drawInfo(DrawInfo::PICKING_PASS);
    drawInfo.setFromCamera(&camera);

	// world objects
	getScene()->getVisibilityManager()->draw(drawInfo);

	// TODO
	//getScene()->getContext()->disableDepthWrite();
	//getScene()->getAlphaPipeline()->sort();
	//getScene()->getAlphaPipeline()->draw(drawInfo);
	//getScene()->getContext()->setDefaultDepthWrite();

    // camera clear
    camera.clearCameraChanged();
}

void ShadowVolumeForward::setPolicyLevel(UInt32 level)
{
	// TODO
}

UInt32 ShadowVolumeForward::getPolicyLevel() const
{
	return 0;
}

