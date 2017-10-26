/**
 * @file drawinfo.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/drawinfo.h"
#include "o3d/engine/object/light.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/material/material.h"

using namespace o3d;

// Define the parameters from a valid light.
void DrawInfo::setFromLight(Light *pLight)
{
	O3D_ASSERT(pLight);

	if (pLight)
	{
		pass = DrawInfo::LIGHTING_PASS;

		light.type = pLight->getLightType();

		light.ambient = pLight->getAmbient();
		light.diffuse = pLight->getDiffuse();
		light.specular = pLight->getSpecular();

		light.position = pLight->getPosition();
		light.worldPos = pLight->getWorldPosition();
        light.direction = pLight->getDirection();
        light.worldDir = pLight->getWorldDirection();

		light.range = 0.f;
		light.cutOff = o3d::toRadian(pLight->getCutOff());
		light.cosCutOff = cosf(light.cutOff);
		light.exponent = pLight->getExponent();

		light.attenuation = pLight->getAttenuation();

		if (pLight->getShadowCast())
			light.shadowType = Shadowable::SHADOW_MAP_HARD; // TODO soft...
		else
			light.shadowType = Shadowable::SHADOW_NONE;
	}
	else
	{
		pass = DrawInfo::AMBIENT_PASS;
    }
}

void DrawInfo::setFromCamera(Camera *pCamera)
{
    O3D_ASSERT(pCamera);

    if (pCamera)
    {
        eyePos = pCamera->getAbsoluteMatrix().getTranslation();
    }
}

