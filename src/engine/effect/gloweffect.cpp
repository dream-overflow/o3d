/**
 * @file gloweffect.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/effect/gloweffect.h"
#include "o3d/engine/object/primitivemanager.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/alphapipeline.h"
#include "o3d/engine/material/materialprofile.h"
#include "o3d/engine/material/colormaterial.h"
#include "o3d/engine/context.h"
#include "o3d/image/color.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(GlowEffect, ENGINE_EFFECT_GLOW, SpecialEffects)

const Float GlowEffect::DEFAULT_DISTANCE = 100.0f;
const Float GlowEffect::DEFAULT_INTENSITY = 1.0f;

// constructor
GlowEffect::GlowEffect(BaseObject *parent, Texture2D* texture) :
	EffectIntensity(parent),
	m_material(this)
{
	// default size
	m_halfSizeX = 45.f * 0.5f;
	m_halfSizeY = m_halfSizeX;

	// default color
	m_color.set(0.92f, 0.8f, 0.30f, DEFAULT_INTENSITY);

	m_attenuationRange = 1.f;
	m_minIntensity = 0.f;
	m_isBehindEffect = False;

	// default is infinite glow with direction 0,0,1
	m_glowType = GLOW_INFINITE;
	m_distance = DEFAULT_DISTANCE;
	m_glowVector.set(0.f,0.f,1.f);

    if (!getScene()) {
		return;
    }

	// material
	m_material.setNumTechniques(1);
	m_material.getTechnique(0).setNumPass(1);

    m_material.getTechnique(0).getPass(0).setMaterial(Material::AMBIENT, new ColorMaterial(this));
    m_material.getTechnique(0).getPass(0).setDiffuseMap(texture);
    m_material.getTechnique(0).getPass(0).setMapFiltering(MaterialPass::DIFFUSE_MAP, Texture::LINEAR_FILTERING);
    m_material.getTechnique(0).getPass(0).setMapWarp(MaterialPass::DIFFUSE_MAP, Texture::CLAMP);
    m_material.getTechnique(0).getPass(0).setMapAnisotropy(MaterialPass::DIFFUSE_MAP, 1.0f);

    m_material.getTechnique(0).getPass(0).setBlendingFunc(Blending::SRC_A__ONE);
	m_material.getTechnique(0).getPass(0).setSorted(True);
    m_material.getTechnique(0).getPass(0).setDiffuse(m_color);
	m_material.getTechnique(0).getPass(0).disableDepthTest();
	m_material.getTechnique(0).getPass(0).disableDepthWrite();
	m_material.getTechnique(0).getPass(0).enableDoubleSide();

	m_material.prepareAndCompile(*this);
}

// draw the effect (ex. particle, lens-flare)
void GlowEffect::draw(const DrawInfo &drawInfo)
{
    if (!getActivity() || !getVisibility()) {
		return;
    }

	// check minimal intensity
    if (m_intensity < m_minIntensity) {
		m_intensity = m_minIntensity;
    }

	// compute the glow color
	Color color(
			m_color.red(),
			m_color.green(),
			m_color.blue(),
			m_color.alpha() * getIntensity());

	MaterialTechnique *technique = m_material.getBestTechnique(0.f);
    if (technique) {
        technique->setDiffuse(color);

		// send the face to alpha pipeline with m_GlowVector as center position of the glow
		getScene()->getAlphaPipeline()->addFace(
				this,
				technique,
				0, 1, 2,
				3*getScene()->getActiveCamera()->getModelviewMatrix().transformOnZ(m_glowVector));
	}
}

// set is behind effect
void GlowEffect::setBehindEffect(Bool IsBehindEffect)
{
	// set the shader to render behind
	m_isBehindEffect = IsBehindEffect;

    if (m_isBehindEffect) {
        if (m_glowType == GLOW_LOCAL_WORLD) {
			// put depth buffer in normal mode
			m_material.disableInfiniteDepthRange();
        } else {
			// put depth buffer in infinite mode, the object will be rendered in all the scene
			m_material.enableInfiniteDepthRange();
		}
		m_material.disableDepthWrite();
		m_material.enableDepthTest();
    } else {
		// disable depth buffer, object is always rendered on top of the scene
		m_material.enableDepthWrite();
		m_material.disableDepthTest();
		m_material.disableInfiniteDepthRange();
	}
}

// Compute the intensity with attenuation settings
void GlowEffect::calcAttenuationRange(const Vector3 &refScreenPos)
{
	Int32 tmp[4];
	getScene()->getContext()->getViewPort(tmp);

	Float halfScreenWidth = (Float)tmp[2] * 0.5f;
	Float halfScreenHeight = (Float)tmp[3] * 0.5f;

	Float glowIntensityX = 1.0f - o3d::clamp(
            (Float)fabs(refScreenPos.x() - halfScreenWidth)  / (halfScreenWidth  * m_attenuationRange),0.f,1.f);

	Float glowIntensityY = 1.0f - o3d::clamp(
            (Float)fabs(refScreenPos.y() - halfScreenHeight) / (halfScreenHeight * m_attenuationRange),0.f,1.f);

	m_intensity = glowIntensityX * glowIntensityY;
}

// Get the texture
Texture2D* GlowEffect::getTexture() const
{
    return static_cast<Texture2D*>(m_material.getTechnique(0).getPass(0).getDiffuseMap());
}

// draw all faces
void GlowEffect::processAllFaces(Shadable::ProcessingPass pass)
{
    if (pass == Shadable::PROCESS_GEOMETRY) {
		// draw the glow
		getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);
	}
}

void GlowEffect::attribute(VertexAttributeArray mode, UInt32 location)
{
    if (mode == V_VERTICES_ARRAY) {
		getScene()->getPrimitiveManager()->getQuadVerticesVBO().attribute(location, 3, 0, 0);
    } else if (mode == V_TEXCOORDS_2D_1_ARRAY) {
		getScene()->getPrimitiveManager()->getQuadTexCoordsVBO().attribute(location, 2, 0, 0);
    }
}

// setup modelview matrix
void GlowEffect::setUpModelView()
{
	Vector3 vPosition;
	Camera *camera = getScene()->getActiveCamera();

    if (m_glowType == GLOW_INFINITE) {
		// we use m_GlowVector as direction
		vPosition = camera->getAbsoluteMatrix().getTranslation() + m_glowVector * m_distance;
    } else if (m_glowType == GLOW_LOCAL) {
		// we use m_GlowVector for compute a direction between the camera and the target
		Vector3 vDirection = m_glowVector - camera->getAbsoluteMatrix().getTranslation();
		vDirection.normalize();
		vPosition = camera->getAbsoluteMatrix().getTranslation() + vDirection * m_distance;
    } else if (m_glowType == GLOW_LOCAL_WORLD) {
		// we use m_GlowVecor as glow position
		vPosition = m_glowVector;
	}

    if (getScene()->getActiveCamera()) {
        getScene()->getContext()->modelView().set(camera->getModelviewMatrix());
    } else {
        getScene()->getContext()->modelView().identity();
    }

	// make the translation
	getScene()->getContext()->modelView().translate(vPosition);

	// no rotate (billboard)
	getScene()->getContext()->modelView().unRotate();

	// scale by the glow size
	getScene()->getContext()->modelView().scale(Vector3(m_halfSizeX, m_halfSizeY, 1));
}

// Serialization
Bool GlowEffect::writeToFile(OutStream &os)
{
    SpecialEffects::writeToFile(os);

    os   << m_halfSizeX
		 << m_halfSizeY
		 << m_color
		 << m_attenuationRange
		 << m_minIntensity
		 << m_isBehindEffect
		 << m_glowType
		 << m_distance
		 << m_glowVector;

	return True;
}

Bool GlowEffect::readFromFile(InStream &is)
{
    SpecialEffects::readFromFile(is);

    is   >> m_halfSizeX
		 >> m_halfSizeY
		 >> m_color
		 >> m_attenuationRange
		 >> m_minIntensity
		 >> m_isBehindEffect
		 >> m_glowType
		 >> m_distance
		 >> m_glowVector;

    return True;
}

