/**
 * @file lenseffect.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/effect/lenseffect.h"

#include "o3d/engine/object/camera.h"
#include "o3d/core/debug.h"
#include "o3d/core/memorydbg.h"
#include "o3d/engine/occlusionquery.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/drawcontext.h"
#include "o3d/engine/shader/shadermanager.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/core/math.h"
#include "o3d/geom/frustum.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS2(LensEffect, ENGINE_EFFECT_LENS_FLARE, SpecialEffects,MultiEffect)

// Default constructor.
LensEffect::LensEffect(BaseObject *parent) :
	EffectIntensity(parent),
    m_vertices(getScene()->getContext()),
    m_texCoords(getScene()->getContext()),
    m_occlusionQuery(nullptr),
    m_drawQuery(nullptr),
	m_distance(100.0f),
	m_infinite(True),
	m_halfSizeX(30.0f/2.0f),
	m_halfSizeY(30.0f/2.0f),
	m_maxDistance(1000.0f),
	m_minDistance(0.0f),
	m_minFadeRange(10.0f),
	m_maxFadeRange(100.0f),
	m_visibilityRatio(1.0f),
	m_fadeInPersistence(0),
	m_fadeOutPersistence(0),
	m_simpleOcclusion(False),
	m_lastFrameTime(0)
{
    for (Int32 i = 0; i < LensFlareModel::MAX_NUM_FLARES; ++i) {
		m_flareTextures[i].setUser(this);
    }

	createShader();
}

LensEffect::LensEffect(BaseObject *parent, const LensFlareModel &model) :
	EffectIntensity(parent),
    m_vertices(getScene()->getContext()),
    m_texCoords(getScene()->getContext()),
    m_occlusionQuery(nullptr),
    m_drawQuery(nullptr),
	m_distance(100.0f),
	m_infinite(True),
	m_halfSizeX(30.0f/2.0f),
	m_halfSizeY(30.0f/2.0f),
	m_maxDistance(1000.0f),
	m_minDistance(0.0f),
	m_minFadeRange(10.0f),
	m_maxFadeRange(100.0f),
	m_visibilityRatio(1.0f),
	m_fadeInPersistence(0),
	m_fadeOutPersistence(0),
	m_simpleOcclusion(False),
	m_lastFrameTime(0)
{
    for (Int32 i = 0; i < LensFlareModel::MAX_NUM_FLARES; ++i) {
		m_flareTextures[i].setUser(this);
    }

	createShader();
	setLensFlareModel(model);
}

LensEffect::LensEffect(
	BaseObject *parent,
	const LensFlareModel &model,
	Bool infinite) :
		EffectIntensity(parent),
        m_vertices(getScene()->getContext()),
        m_texCoords(getScene()->getContext()),
        m_occlusionQuery(nullptr),
        m_drawQuery(nullptr),
		m_distance(100.0f),
		m_infinite(True),
		m_halfSizeX(30.0f/2.0f),
		m_halfSizeY(30.0f/2.0f),
		m_maxDistance(1000.0f),
		m_minDistance(0.0f),
		m_minFadeRange(10.0f),
		m_maxFadeRange(100.0f),
		m_visibilityRatio(1.0f),
		m_fadeInPersistence(0),
		m_fadeOutPersistence(0),
		m_simpleOcclusion(False),
		m_lastFrameTime(0)
{
    for (Int32 i = 0; i < LensFlareModel::MAX_NUM_FLARES; ++i) {
		m_flareTextures[i].setUser(this);
    }

	createShader();
	setLensFlareModel(model,infinite);
}

void LensEffect::createShader()
{
    if (!m_diffuseMapShader.instance.isDefined()) {
		// diffuse map shader
		Shader *shader = getScene()->getShaderManager()->addShader("lensEffect");
		shader->buildInstance(m_diffuseMapShader.instance);

        m_diffuseMapShader.instance.assign("diffuseMap", "diffuseMap", "", Shader::BUILD_COMPILE_AND_LINK);

		m_diffuseMapShader.u_modelViewProjectionMatrix = m_diffuseMapShader.instance.getUniformLocation("u_modelViewProjectionMatrix");
		m_diffuseMapShader.u_color = m_diffuseMapShader.instance.getUniformLocation("u_color");
		m_diffuseMapShader.u_diffuseMap = m_diffuseMapShader.instance.getUniformLocation("u_diffuseMap");

		m_diffuseMapShader.a_vertex = m_diffuseMapShader.instance.getAttributeLocation("a_vertex");
		m_diffuseMapShader.a_texCoords = m_diffuseMapShader.instance.getAttributeLocation("a_texCoords");
	}

    if (!m_occlusionShader.instance.isDefined()) {
		// occlusion shader
		Shader *shader = getScene()->getShaderManager()->addShader("lensEffect");
		shader->buildInstance(m_occlusionShader.instance);

        m_occlusionShader.instance.assign("occlusion", "occlusion", "", Shader::BUILD_COMPILE_AND_LINK);

		m_occlusionShader.u_modelViewProjectionMatrix = m_occlusionShader.instance.getUniformLocation("u_modelViewProjectionMatrix");
		m_occlusionShader.a_vertex = m_occlusionShader.instance.getAttributeLocation("a_vertex");
	}

    if (!m_texCoords.exists()) {
		// texture coordinates VBO
		static Float texCoords[8] = { 0,0, 1,0, 0,1, 1,1 };
		m_texCoords.create(8, VertexBuffer::STATIC, texCoords);
	}

    if (!m_vertices.exists()) {
		// vertices VBO
		static Float vertices[12] = {
				-1, -1, 0,
				 1, -1, 0,
				-1,  1, 0,
				 1,  1, 0 };
		m_vertices.create(12, VertexBuffer::STATIC, vertices);
	}
}

LensEffect::~LensEffect()
{
	// release the usage for each flare
    for (Int32 i = 0; i < LensFlareModel::MAX_NUM_FLARES; ++i) {
        m_flareTextures[i] = nullptr;
    }

	removeAll();
	deleteOcclusionQueries();
}

void LensEffect::createOcclusionQueries()
{
    // Test si on a pas deja un test d'occlusion queries en cours
    if (!m_occlusionQuery) {
        m_occlusionQuery = new OcclusionQuery(getScene()->getContext());
	}

    if (!m_simpleOcclusion) {
        if (!m_drawQuery) {
            m_drawQuery = new OcclusionQuery(getScene()->getContext());
		}
	}
}

// make the occlusion test (les transformations doivent avoir ete definies)
void LensEffect::checkOcclusionQueries()
{
	Context *glContext = getScene()->getContext();

    // Definition de modelView
	glContext->modelView().push();

	Vector3 vPosition;
	Camera *pCam = getScene()->getActiveCamera();

    if (m_infinite) {
		// On utilise vGlowVector comme une direction
		vPosition = pCam->getAbsoluteMatrix().getTranslation() + m_lensVector * m_distance;
    } else {
		// On utilise vGlowVector comme la position du glow.
        // Meme si on rend le glow a une distance constante de la camera (GLOW_LOCAL)
        // Il faut faire le test d'occlusion a la source du glow
		vPosition = m_lensVector;
	}

	glContext->modelView().translate(vPosition);
	glContext->modelView().unRotate();

	glContext->modelView().scale(Vector3(m_halfSizeX, m_halfSizeY, 1.f));

    // Le but est de ne rien rendre a l'ecran et ne pas modifier le Z-Buffer
    DrawContext drawContext(getScene()->getContext());
	drawContext.disableDepthWrite();
	drawContext.apply();

	glContext->disableColorWrite();

	// Si on a un lens effect infinie,
	// On doit placer le mesh de test d'occlusion en l'infini
	// Pour se faire, on joue sur le DepthRange
    if (m_infinite) {
		glContext->setInfiniteDepthRange();
    } else {
		glContext->setDefaultDepthRange();
    }

	// shader
	m_occlusionShader.instance.bindShader();
	m_occlusionShader.instance.setConstMatrix4(
			m_occlusionShader.u_modelViewProjectionMatrix,
			False,
			glContext->modelViewProjection());

	m_vertices.attribute(m_occlusionShader.a_vertex, 3, 0, 0);

	// Fais le test d'occlusion avec un objet totalement visible
	if (!m_simpleOcclusion)
	{
		glContext->disableDepthTest();
        m_drawQuery->begin();
			renderOcclusionTestMesh();
        m_drawQuery->end();
	}
	
	// Fais le test d'occlusion avec l'objet rendu normalement
	glContext->enableDepthTest();
    m_occlusionQuery->begin();
		renderOcclusionTestMesh();
    m_occlusionQuery->end();
	
	// shader
	glContext->disableVertexAttribArray(m_occlusionShader.a_vertex);
	m_occlusionShader.instance.unbindShader();

	glContext->modelView().pop();

	glContext->enableColorWrite();
}

void LensEffect::deleteOcclusionQueries()
{
    if (m_occlusionQuery) {
        if (getScene()->getContext()->getCurrentOcclusionQuery() == m_occlusionQuery) {
            getScene()->getContext()->setCurrentOcclusionQuery(nullptr);
        }

        // Cette occlusion n'est pas en cours d'utilisation on peut la liberer
		deletePtr(m_occlusionQuery);
	}

    if (m_drawQuery) {
        if (getScene()->getContext()->getCurrentOcclusionQuery() == m_drawQuery) {
            getScene()->getContext()->setCurrentOcclusionQuery(nullptr);
        }

        // Cette occlusion n'est pas en cours d'utilisation on peut la liberer
		deletePtr(m_drawQuery);
	}
}

void LensEffect::renderOcclusionTestMesh()
{
	getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);
}

void LensEffect::calculateVisibilityRatio()
{
    if (m_simpleOcclusion) {
		// Test si il faut effacer le test d'occlusions
        if (m_drawQuery) {
			deleteOcclusionQueries();
		}

        // Test si il faut creer de nouvelles occlusions
        if (!m_occlusionQuery) {
			// Reset le resultat
			m_visibilityRatio = 1.0f;
			createOcclusionQueries();

            // Effectu le test d'occlusions mais ne cherche pas a recuperer
            // les resultats (on laisse passer au moins une frame)
			checkOcclusionQueries();
        } else {
            // Nos occlusions existent, on va pouvoir essayer de recuperer les resultats
			// Si ils sont disponibles...

			// Test si le resultat des deux occlusions sont disponibles
            // Si le resultat n'est pas disponible, on garde l'ancien
            if (m_occlusionQuery->getOcclusionType() != OcclusionQuery::NOT_AVAILABLE) {
                // On utilise le resultat
				m_visibilityRatio = (m_occlusionQuery->getVisibleCount() > 0 ? 1.0f : 0.0f);

				// Re-Effectue le test d'occlusions
				checkOcclusionQueries();
			}
		}
    } else {
        // Test si il faut creer de nouvelles occlusions
        if ((!m_occlusionQuery) && (!m_drawQuery)) {
			// Reset le resultat
			m_visibilityRatio = 1.0f;
			createOcclusionQueries();

            // Effectu le test d'occlusions mais ne cherche pas a recuperer
            // les resultats (on laisse passer au moins une frame)
			checkOcclusionQueries();
        } else if ((m_occlusionQuery) && (m_drawQuery)) {
            // Nos occlusions existent, on va pouvoir essayer de recuperer les resultats
			// Si ils sont disponibles...

			// Test si le resultat des deux occlusions sont disponibles
            // Si le resultat n'est pas disponible, on garde l'ancien
			
			// On Windows we have to perform the two check for availability otherwise 
			// the first occlusion lock the second and we never have results...
			m_occlusionQuery->getOcclusionType();

            if ((m_drawQuery->getOcclusionType() != OcclusionQuery::NOT_AVAILABLE) && (m_occlusionQuery->getOcclusionType() != OcclusionQuery::NOT_AVAILABLE)) {
                // On utilise le resultat
				m_visibilityRatio = (Float)(m_occlusionQuery->getVisibleCount())/Float(m_drawQuery->getVisibleCount());

				// Re-Effectu le test d'occlusions
				checkOcclusionQueries();
			}
        } else {
            // Une occlusion est cree mais pas l'autre... On reset le systeme
			O3D_MESSAGE("(m_pOcclusionQuery && m_pDrawQuery)==FALSE : Reset the occlusion");
			deleteOcclusionQueries();
			m_visibilityRatio = 1.0f;
		}
	}
}

void LensEffect::generateGlowsObj()
{
	LensFlareModel::LensGlowItem *pGlowItem;
	GlowEffect *pNewGlow;
	Int32 i;

	// Delete all elements of the multi-effects
	removeAll();

	// Reset the radius of this effect
	m_effectRadius = o3d::max(m_halfSizeX,m_halfSizeY);

	// Create each glows according to Lens Effect Params
    for (i = 0; i < LensFlareModel::MAX_NUM_GLOWS; ++i) {
		pGlowItem = m_lensFlareModel.getGlow(i);
        if (pGlowItem) {
			// the user is added by the shader of the glow effect
			pNewGlow = new GlowEffect(this, pGlowItem->texture);

            if (m_infinite) {
				pNewGlow->setGlowType(GlowEffect::GLOW_INFINITE);
            } else {
                if (pGlowItem->is3dGlow) {
					pNewGlow->setGlowType(GlowEffect::GLOW_LOCAL_WORLD);
                } else {
					pNewGlow->setGlowType(GlowEffect::GLOW_LOCAL);
                }
			}

			pNewGlow->setColor(pGlowItem->color);
			pNewGlow->setHalfSize(pGlowItem->halfSizeX,pGlowItem->halfSizeY);
			pNewGlow->setAttenuationRange(pGlowItem->attenuationRange);
			pNewGlow->setMinIntensity(pGlowItem->minIntensity);
			pNewGlow->setBehindEffect(pGlowItem->isBehindEffect);
			pNewGlow->setIntensityType(EffectIntensity::INTENSITY_PARENT);

			// If the min intensity is greater than 0, this glow must affect
			// The effect radius. This is needed because without that, the glow
			// Will disappear suddenly due to frustum clipping
            if (pGlowItem->minIntensity > 0) {
				m_effectRadius = o3d::max(m_effectRadius, o3d::max(pGlowItem->halfSizeX,pGlowItem->halfSizeY));
            }

			// defining the blending parameter
            pNewGlow->getMaterialProfile().setBlendingFunc(pGlowItem->blending);

			// Add this glow to the effect list
			addEffect(pNewGlow);
		}
	}
}

void LensEffect::updateGlowsPositions()
{
	SpecialEffects *pTempEffect;

    for (IT_SpecialEffectsList it = m_effectlist.begin(); it != m_effectlist.end(); ++it) {
		pTempEffect = (*it).get();
        if (pTempEffect) {
            if (pTempEffect->getType() == ENGINE_EFFECT_GLOW) {
				((GlowEffect*)pTempEffect)->setGlowVector(m_lensVector);
			}
		}
	}
}

void LensEffect::calcGlowAttenuationRange(const Vector3 &refScreenPos)
{
	SpecialEffects *pTempEffect;

    for (IT_SpecialEffectsList it = m_effectlist.begin(); it != m_effectlist.end(); ++it) {
		pTempEffect = (*it).get();
        if (pTempEffect) {
            if (pTempEffect->getType() == ENGINE_EFFECT_GLOW) {
				((GlowEffect*)pTempEffect)->calcAttenuationRange(refScreenPos);
			}
		}
	}
}

void LensEffect::setLensFlareModel(const LensFlareModel &Model)
{
	// copy the flare model
	m_lensFlareModel = Model;

	// unuse for previous model
	LensFlareModel::LensFlareItem *flareItem;

	// relase the usage for each flare
	// at the lens-flare model destruction the texture are released again
    for (Int32 i = 0; i < LensFlareModel::MAX_NUM_FLARES; ++i) {
		flareItem = m_lensFlareModel.getFlare(i);

		// use the texture
        if (flareItem && flareItem->texture) {
			m_flareTextures[i] = flareItem->texture;
        } else {
            m_flareTextures[i] = nullptr;
        }
	}

	// Generals parameters (these parameters can be overloaded)
	m_infinite = m_lensFlareModel.getInfinite();
	m_halfSizeX = m_lensFlareModel.getHalfSizeX();
	m_halfSizeY = m_lensFlareModel.getHalfSizeY();
	m_minDistance = m_lensFlareModel.getMinDistance();
	m_maxDistance = m_lensFlareModel.getMaxDistance();
	m_minFadeRange = m_lensFlareModel.getMinFadeRange();
	m_maxFadeRange = m_lensFlareModel.getMaxFadeRange();
	m_fadeInPersistence = m_lensFlareModel.getFadeInPersistence();
	m_fadeOutPersistence = m_lensFlareModel.getFadeOutPersistence();
	m_simpleOcclusion = m_lensFlareModel.getSimpleOcclusion();

	generateGlowsObj(); // Generate glows
}

void LensEffect::setLensFlareModel(const LensFlareModel &Model, Bool Infinite)
{
	// copy the flare model
	m_lensFlareModel = Model;

	LensFlareModel::LensFlareItem *flareItem;

	// release the usage for each flare
	// at the lens-flare model destruction the texture are released again
    for (Int32 i = 0; i < LensFlareModel::MAX_NUM_FLARES; ++i) {
		flareItem = m_lensFlareModel.getFlare(i);

		// use the texture
        if (flareItem && flareItem->texture) {
			m_flareTextures[i] = flareItem->texture;
        } else {
            m_flareTextures[i] = nullptr;
        }
	}

	// Generals parameters (these parameters can be overloaded)
	m_infinite = Infinite;
	m_halfSizeX = m_lensFlareModel.getHalfSizeX();
	m_halfSizeY = m_lensFlareModel.getHalfSizeY();
	m_minDistance = m_lensFlareModel.getMinDistance();
	m_maxDistance = m_lensFlareModel.getMaxDistance();
	m_minFadeRange = m_lensFlareModel.getMinFadeRange();
	m_maxFadeRange = m_lensFlareModel.getMaxFadeRange();
	m_fadeInPersistence = m_lensFlareModel.getFadeInPersistence();
	m_fadeOutPersistence = m_lensFlareModel.getFadeOutPersistence();
	m_simpleOcclusion = m_lensFlareModel.getSimpleOcclusion();

	generateGlowsObj(); // Generate glows
}

// Get the lens flare model (read only)
const LensFlareModel& LensEffect::getLensFlareModel()
{
	// check for deleted textures

	// flares...
	LensFlareModel::LensFlareItem *flareItem;

    for (Int32 i = 0; i < LensFlareModel::MAX_NUM_FLARES; ++i) {
		flareItem = m_lensFlareModel.getFlare(i);

		// use the texture
        if ((flareItem && flareItem->texture) && !m_flareTextures[i]) {
            flareItem->texture = nullptr;
        }
	}

	// glows next...
	LensFlareModel::LensGlowItem *glowItem;
	IT_SpecialEffectsList itGlow = getEffectList().begin();

    for (Int32 i = 0; i < LensFlareModel::MAX_NUM_GLOWS; ++i) {
		glowItem = m_lensFlareModel.getGlow(i);

		// use the texture
        if ((glowItem && glowItem->texture) && !((GlowEffect*)(*itGlow).get())->getTexture()) {
            glowItem->texture = nullptr;
        }

		// next effect
        if (glowItem) {
			++itGlow;
        }
	}

	m_lensFlareModel.setInfinite(m_infinite);
	m_lensFlareModel.setHalfSizeX(m_halfSizeX);
	m_lensFlareModel.setHalfSizeY(m_halfSizeY);
	m_lensFlareModel.setMinDistance(m_minDistance);
	m_lensFlareModel.setMaxDistance(m_maxDistance);
	m_lensFlareModel.setMinFadeRange(m_minFadeRange);
	m_lensFlareModel.setMaxFadeRange(m_maxFadeRange);
	m_lensFlareModel.setFadeInPersistence(m_fadeInPersistence);
	m_lensFlareModel.setFadeOutPersistence(m_fadeOutPersistence);
	m_lensFlareModel.setSimpleOcclusion(m_simpleOcclusion);

	return m_lensFlareModel;
}

// setup the modelview matrix for OpenGL before render the object
void LensEffect::setUpModelView()
{
    if (getScene()->getActiveCamera()) {
        getScene()->getContext()->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix());
    } else {
        getScene()->getContext()->modelView().identity();
    }
}

// draw the effect
void LensEffect::draw(const DrawInfo &drawInfo)
{
    if (!getActivity() || !getVisibility()) {
		return;
    }

    // @todo Do a path to draw it when we have a deferred scene pipeline.
    // can draw it after, but need a special PASS for that
    if (drawInfo.pass != DrawInfo::AMBIENT_PASS) {
        return;
    }

	Context *glContext = getScene()->getContext();

	setUpModelView();

	Vector3 vPosition;
	Vector3 vCameraCenter;

	Float ZDistanceFromGlow;
	LensFlareModel::LensFlareItem *flareItem;

	Int32 tmp[4];
	glContext->getViewPort(tmp);

	Float halfScreenWidth = (Float)tmp[2] * 0.5f;
	Float halfScreenHeight = (Float)tmp[3] * 0.5f;

	Matrix4 mModelView;
	Float DistanceAttenuation;
	Float elapsedTime;      // In seconds
	Int32 i;

	// Compute the time elapsed between the last call
    if (m_lastFrameTime == 0) {
		elapsedTime = 0.0f;
    } else {
		elapsedTime = Float(System::getTime() - m_lastFrameTime) / System::getTimeFrequency();
    }

	m_lastFrameTime = System::getTime();

	// Get parameters
	Camera *pCam = getScene()->getActiveCamera();
	mModelView = pCam->getModelviewMatrix();

    if (m_infinite) {
		// On utilise vLensVector comme une direction
		vPosition = pCam->getAbsoluteMatrix().getTranslation() + m_lensVector * m_distance;
		ZDistanceFromGlow = -mModelView.transformOnZ(vPosition);
		DistanceAttenuation = 1.0f; // No attenuation with distance
    } else {
		// On utilise vLensVector comme la position du glow
		Vector3 vDirection = m_lensVector - pCam->getAbsoluteMatrix().getTranslation();
		vDirection.normalize();
		vPosition = pCam->getAbsoluteMatrix().getTranslation() + vDirection * m_distance;
		ZDistanceFromGlow = -mModelView.transformOnZ(m_lensVector);

		// The source is too close
        if (ZDistanceFromGlow < m_minDistance) {
			return;
        }

		// The source is too far
        if (ZDistanceFromGlow > m_maxDistance) {
			return;
        }

		// Calculate the distance attenuation
		DistanceAttenuation = o3d::clamp(((m_maxDistance - ZDistanceFromGlow) / m_maxFadeRange),0.f,1.f) *
							  o3d::clamp(((ZDistanceFromGlow - m_minDistance) / m_minFadeRange),0.f,1.f);
	}

	// Check if the src is behind the viewer (Z < 0)
    if (ZDistanceFromGlow < 0) {
		return;
    }

	// Check if we have to render this effect
    if (getScene()->getFrustum()->sphereInFrustum(vPosition,m_effectRadius) == Geometry::CLIP_OUTSIDE) {
		return;
    }
	
	// Calcul the attenuation of the src and check if the src is visible
	calculateVisibilityRatio();

	// Calcul the intensity
	Float oldIntensity = getIntensity();
	Float newIntensity = m_visibilityRatio * DistanceAttenuation;

	// Fade in / Fade out
    if (newIntensity > oldIntensity) {
		// Fade in
		// Immediate update
        if (m_fadeInPersistence < o3d::Limits<Float>::epsilon()) {
			setIntensity(newIntensity);
        } else {
			// Need Persistence seconds to go from 0 to 1 so to add 1
			oldIntensity += 1.0f / m_fadeInPersistence * elapsedTime;
            if (oldIntensity > newIntensity) {
				oldIntensity = newIntensity;
            }
			setIntensity(oldIntensity);
		}
    } else if (newIntensity < oldIntensity) {
		// Fade out
		// Immediate update
        if (m_fadeOutPersistence < o3d::Limits<Float>::epsilon()) {
			setIntensity(newIntensity);
        } else {
			// Need Persistence seconds to go from 1 to 0 so to sub 1
			oldIntensity -= 1.0f / m_fadeOutPersistence * elapsedTime;
            if (oldIntensity < newIntensity) {
				oldIntensity = newIntensity;
            }
			setIntensity(oldIntensity);
		}
	}

    if (getIntensity() < o3d::Limits<Float>::epsilon()) {
		return;
    }

	// Calcul du point 3D world space correspondant au centre de la camera
	vCameraCenter = pCam->getAbsoluteMatrix().getTranslation() + pCam->getModelviewMatrix().getRotation().transposeTo() * Vector3(0,0,-1) * m_distance;

	// Vecteur sur lequel seront les flares
	Vector3 vLensDir = vPosition - vCameraCenter;

	// Projection de la source sur l'ecran
	Vector3 srcScreenPosition = getScene()->getContext()->projectPoint(vPosition);
    srcScreenPosition.x() += (Float)tmp[0];
    srcScreenPosition.y() += (Float)tmp[1];

	// Calculate all glows attenuation
	calcGlowAttenuationRange(srcScreenPosition);

	// Render all glow/Rays
	drawAll(drawInfo);

	// Setup material for lens flare rendering
    DrawContext drawContext(getScene()->getContext());
	drawContext.disableDepthTest();
    drawContext.setBlendingFunc(Blending::SRC_A__ONE_MINUS_SRC_A);
	drawContext.apply();

	setUpModelView();

	m_diffuseMapShader.instance.bindShader();

	m_texCoords.attribute(m_diffuseMapShader.a_texCoords, 2, 0, 0);
	m_vertices.attribute(m_diffuseMapShader.a_vertex, 3, 0, 0);

	// Render all flares
    for (i = 0; i < LensFlareModel::MAX_NUM_FLARES; i++) {
		flareItem = m_lensFlareModel.getFlare(i);
        if (flareItem) {
			// Calcul du coefficient d'attenuation
			Float FlareIntensityX = 1.0f - o3d::clamp(
					(Float)fabs(srcScreenPosition[0] - halfScreenWidth)/(halfScreenWidth*(flareItem->attenuationRange == 0
							? o3d::Limits<Float>::epsilon():flareItem->attenuationRange)),
					0.f,
					1.f);

			Float FlareIntensityY = 1.0f - o3d::clamp(
					(Float)fabs(srcScreenPosition[1] - halfScreenHeight)/(halfScreenHeight*(flareItem->attenuationRange == 0
							? o3d::Limits<Float>::epsilon():flareItem->attenuationRange)),
					0.f,
					1.f);

			Float FlareIntensity = FlareIntensityX * FlareIntensityY;

            if (m_flareTextures[i] && m_flareTextures[i]->isValid()) {
                glContext->blending().setFunc(flareItem->blending);

				glContext->modelView().push();

				Vector3 position = vCameraCenter + vLensDir * flareItem->position;
				glContext->modelView().translate(position);
				glContext->modelView().unRotate(); // billboard

				glContext->modelView().scale(Vector3(flareItem->halfSizeX, flareItem->halfSizeY, 1.f));

				m_diffuseMapShader.instance.setConstMatrix4(
						m_diffuseMapShader.u_modelViewProjectionMatrix,
						False,
						glContext->modelViewProjection());

				m_diffuseMapShader.instance.setConstTexture(
						m_diffuseMapShader.u_diffuseMap,
						m_flareTextures[i].get(),
						0);

				m_diffuseMapShader.instance.setConstColor(m_diffuseMapShader.u_color, Color(
						flareItem->color[0],
						flareItem->color[1],
						flareItem->color[2],
						flareItem->color[3] * FlareIntensity * getIntensity()));

				getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);

				glContext->modelView().pop();
			}
		}
	}

	glContext->disableVertexAttribArray(m_diffuseMapShader.a_texCoords);
	glContext->disableVertexAttribArray(m_diffuseMapShader.a_vertex);

	m_diffuseMapShader.instance.unbindShader();
}

// update the effect
void LensEffect::update()
{
	// Update the positions of all glows
	updateGlowsPositions();
	updateAll();
}

// serialization
Bool LensEffect::writeToFile(OutStream &os)
{
    EffectIntensity::writeToFile(os);

    os   << m_distance
		 << m_lensVector;

    getLensFlareModel().writeToFile(os);

	return True;
}

Bool LensEffect::readFromFile(InStream &is)
{
    EffectIntensity::readFromFile(is);

    is   >> m_distance
		 >> m_lensVector;

    m_lensFlareModel.readFromFile(getScene(),is);
	setLensFlareModel(m_lensFlareModel);

	return True;
}

void LensEffect::postImportPass()
{
	EffectIntensity::postImportPass();
}
