/**
 * @file lensflaremodel.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/effect/lensflaremodel.h"
#include "o3d/engine/texture/texturemanager.h"

#include "o3d/engine/material/material.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/texture/texture.h"
#include "o3d/engine/texture/texture2d.h"

using namespace o3d;

// Default constructor.
LensFlareModel::LensFlareModel()
{
	m_infinite = True;
	m_halfSizeX = 20.0f / 2.0f;
	m_halfSizeY = 20.0f / 2.0f;
	m_maxDistance = 1000.0f;
	m_minDistance = 0.0f;
	m_minFadeRange = 10.0f;
	m_maxFadeRange = 100.0f;
	m_fadeInPersistence = 0;
	m_fadeOutPersistence = 0;
	m_simpleOcclusion = False;

    for (Int32 i = 0; i < MAX_NUM_FLARES; ++i) {
        m_flareManager[i] = nullptr;
    }

    for (Int32 i = 0; i < MAX_NUM_GLOWS; ++i) {
        m_glowManager[i] = nullptr;
    }
}

// Destructor.
LensFlareModel::~LensFlareModel()
{
    for (Int32 i = 0; i < MAX_NUM_FLARES; ++i) {
		deletePtr(m_flareManager[i]);
    }

    for (Int32 i = 0; i < MAX_NUM_GLOWS; ++i) {
		deletePtr(m_glowManager[i]);
    }
}

// Copy operator
LensFlareModel::LensFlareItem& LensFlareModel::LensFlareItem::operator=(
	const LensFlareModel::LensFlareItem &dup)
{
	texture = dup.texture;
	blending = dup.blending;
	color = dup.color;
	halfSizeX = dup.halfSizeX;
	halfSizeY = dup.halfSizeY;
	position = dup.position;
	attenuationRange = dup.attenuationRange;
	name = dup.name;

	return *this;
}

// Copy operator
LensFlareModel::LensGlowItem& LensFlareModel::LensGlowItem::operator=(
	const LensFlareModel::LensGlowItem &dup)
{
	texture = dup.texture;
	blending = dup.blending;
	color = dup.color;
	halfSizeX = dup.halfSizeX;
	halfSizeY = dup.halfSizeY;
	attenuationRange = dup.attenuationRange;
	minIntensity = dup.minIntensity;
	is3dGlow = dup.is3dGlow;
	isBehindEffect = dup.isBehindEffect;
	name = dup.name;

	return *this;
}

// Copy operator
LensFlareModel& LensFlareModel::operator=(const LensFlareModel &dup)
{
	m_infinite = dup.m_infinite;
	m_halfSizeX = dup.m_halfSizeX;
	m_halfSizeY = dup.m_halfSizeY;
	m_maxDistance = dup.m_maxDistance;
	m_minDistance = dup.m_minDistance;
	m_minFadeRange = dup.m_minFadeRange;
	m_maxFadeRange = dup.m_maxFadeRange;
	m_fadeInPersistence = dup.m_fadeInPersistence;
	m_fadeOutPersistence = dup.m_fadeOutPersistence;
	m_simpleOcclusion = dup.m_simpleOcclusion;

	// duplicates flares
    for (Int32 i = 0; i < MAX_NUM_FLARES; ++i) {
        if (dup.m_flareManager[i]) {
            if (!m_flareManager[i]) {
				m_flareManager[i] = new LensFlareItem;
            }

			*m_flareManager[i] = *dup.m_flareManager[i];
        } else {
            if (m_flareManager[i]) {
				deletePtr(m_flareManager[i]);
            } else {
                m_flareManager[i] = nullptr;
            }
		}
	}

	// duplicates glows
    for (Int32 i = 0; i < MAX_NUM_GLOWS; ++i) {
        if (dup.m_glowManager[i]) {
            if (!m_glowManager[i]) {
				m_glowManager[i] = new LensGlowItem;
            }

			*m_glowManager[i] = *dup.m_glowManager[i];
        } else {
            if (m_glowManager[i]) {
				deletePtr(m_glowManager[i]);
            } else {
                m_glowManager[i] = nullptr;
            }
		}
	}

	return *this;
}

// Add a flare
Int32 LensFlareModel::addFlare(const LensFlareItem &flare)
{
    for (Int32 i = 0; i < MAX_NUM_FLARES; ++i) {
        if (!m_flareManager[i]) {
			m_flareManager[i] = new LensFlareItem;

			// duplicate
			*m_flareManager[i] = flare;

			// set the name if necessary
            if (m_flareManager[i]->name.isEmpty()) {
				m_flareManager[i]->name = String("Flare") << i;
            }

			return i;
		}
	}
	return -1;
}

Int32 LensFlareModel::addFlare(
	Texture2D *pTexture,
	Float SizeX,
	Float SizeY,
	Float Position)
{
    for (Int32 i = 0; i < MAX_NUM_FLARES; ++i) {
        if (!m_flareManager[i]) {
			// create and add a new flare
			LensFlareItem *pNewFlare = new LensFlareItem;

			pNewFlare->texture = pTexture;
            pNewFlare->blending = Blending::SRC_A__ONE;
			pNewFlare->color.set(0.92f,0.8f,0.30f,1.0f);
			pNewFlare->halfSizeX = SizeX / 2.0f;
			pNewFlare->halfSizeY = SizeY / 2.0f;
			pNewFlare->position = Position;
			pNewFlare->attenuationRange = 1.0f;
			pNewFlare->name = String("Flare") << i;

			m_flareManager[i] = pNewFlare;
			return i;
		}
	}
	return -1;
}

/*---------------------------------------------------------------------------------------
  Add a glow or ray
---------------------------------------------------------------------------------------*/
Int32 LensFlareModel::addGlow(const LensGlowItem &Glow)
{
    for (Int32 i = 0 ; i < MAX_NUM_GLOWS ; ++i) {
        if (!m_glowManager[i]) {
			m_glowManager[i] = new LensGlowItem;

			// duplicate
			*m_glowManager[i] = Glow;

			// set the name if necessary
            if (m_glowManager[i]->name.length() == 0) {
                m_glowManager[i]->name = String("Glow") << i;
            }

			return i;
		}
	}
	return -1;
}

Int32 LensFlareModel::addGlow(
	Texture2D *pTexture,
	Float SizeX,
	Float SizeY,
	Bool isBehindEffect)
{
    for (Int32 i = 0 ; i < MAX_NUM_GLOWS ; ++i) {
        if (!m_glowManager[i]) {
			// create and add a new flare
			LensGlowItem *pNewGlow = new LensGlowItem;

			pNewGlow->texture = pTexture;
            pNewGlow->blending = Blending::SRC_A__ONE;
			pNewGlow->color.set(0.92f,0.8f,0.30f,1.0f);
			pNewGlow->halfSizeX = SizeX / 2.0f;
			pNewGlow->halfSizeY = SizeY / 2.0f;
			pNewGlow->attenuationRange = 1.0f;
			pNewGlow->minIntensity = 0.0f;
			pNewGlow->is3dGlow = False;
			pNewGlow->isBehindEffect = isBehindEffect;
            pNewGlow->name = String("Glow") << i;

			m_glowManager[i] = pNewGlow;
			return i;
		}
	}
	return -1;
}

void LensFlareModel::removeFlare(Int32 index)
{
    if (index < MAX_NUM_FLARES) {
		deletePtr(m_flareManager[index]);
    }
}

/*---------------------------------------------------------------------------------------
  remove a glow/ray
---------------------------------------------------------------------------------------*/
void LensFlareModel::removeGlow(Int32 index)
{
	if (index < MAX_NUM_GLOWS)
		deletePtr(m_glowManager[index]);
}

/*---------------------------------------------------------------------------------------
  get the num of flares
---------------------------------------------------------------------------------------*/
Int32 LensFlareModel::getNumFlares()const
{
	Int32 ret = 0;

    for (Int32 i = 0 ; i < MAX_NUM_FLARES ; ++i) {
		if (m_flareManager[i]) ++ret;
    }

	return ret;
}

/*---------------------------------------------------------------------------------------
  get the num of glows/rays
---------------------------------------------------------------------------------------*/
Int32 LensFlareModel::getNumGlows()const
{
	Int32 ret = 0;

    for (Int32 i = 0 ; i < MAX_NUM_GLOWS ; ++i) {
		if (m_glowManager[i]) ++ret;
    }

	return ret;
}

/*---------------------------------------------------------------------------------------
  serialisation
---------------------------------------------------------------------------------------*/
Bool LensFlareModel::writeToFile(OutStream &os) const
{
    os   << (UInt32)ENGINE_LENSFLARE_MODEL
		 <<	m_infinite
		 <<	m_halfSizeX
		 << m_halfSizeY
		 << m_maxDistance
		 << m_minDistance
		 <<	m_minFadeRange
		 <<	m_maxFadeRange
		 << m_fadeInPersistence
		 <<	m_fadeOutPersistence
		 <<	m_simpleOcclusion;

    os << getNumFlares();
	// write all flares
    for (Int32 i = 0 ; i < MAX_NUM_FLARES ; ++i) {
        if (m_flareManager[i]) {
            if (!m_flareManager[i]->texture) {
                O3D_ERROR(E_InvalidPrecondition("Flare(s) texture(s) must be defined"));
            }

            os   << m_flareManager[i]->color
				 << m_flareManager[i]->blending
				 << m_flareManager[i]->attenuationRange
				 << m_flareManager[i]->halfSizeX
				 << m_flareManager[i]->halfSizeY
				 << m_flareManager[i]->position
				 << m_flareManager[i]->name
				 << *m_flareManager[i]->texture;
		}
    }

    os << getNumGlows();
	// write all glow
    for (Int32 i = 0 ; i < MAX_NUM_GLOWS ; ++i) {
        if (m_glowManager[i]) {
            if (!m_glowManager[i]->texture) {
                O3D_ERROR(E_InvalidPrecondition("Glow(s) texture(s) must be defined"));
            }

            os   << m_glowManager[i]->color
				 << m_glowManager[i]->blending
				 << m_glowManager[i]->halfSizeX
				 << m_glowManager[i]->halfSizeY
				 << m_glowManager[i]->is3dGlow
				 << m_glowManager[i]->isBehindEffect
				 << m_glowManager[i]->minIntensity
				 << m_glowManager[i]->attenuationRange
				 << m_glowManager[i]->name
				 << *m_glowManager[i]->texture;
		}
    }

	return True;
}

Bool LensFlareModel::readFromFile(Scene *pScene, InStream &is)
{
	UInt32 tmp;

    is >> tmp;

    if (tmp != ENGINE_LENSFLARE_MODEL) {
        O3D_ERROR(E_InvalidFormat("Invalid lensflare effect token"));
    }

    is   >>	m_infinite
		 >>	m_halfSizeX
		 >> m_halfSizeY
		 >> m_maxDistance
		 >> m_minDistance
		 >>	m_minFadeRange
		 >>	m_maxFadeRange
		 >> m_fadeInPersistence
		 >>	m_fadeOutPersistence
		 >>	m_simpleOcclusion;

	Int32 nFlares,nGlows;

    is >> nFlares;

    if (nFlares > MAX_NUM_FLARES) {
        O3D_ERROR(E_InvalidFormat("Invalid flares number"));
    }

	// read all flares
    for (Int32 i = 0 ; i < nFlares ; ++i) {
        m_flareManager[i] = new LensFlareItem;
        if (m_flareManager[i]) {
            is   >> m_flareManager[i]->color
				 >> m_flareManager[i]->blending
				 >> m_flareManager[i]->attenuationRange
				 >> m_flareManager[i]->halfSizeX
				 >> m_flareManager[i]->halfSizeY
				 >> m_flareManager[i]->position
				 >> m_flareManager[i]->name;

            m_flareManager[i]->texture = pScene->getTextureManager()->readTexture2D(is);
        } else {
            O3D_ERROR(E_InvalidAllocation("New flare is null"));
        }
	}

    is >> nGlows;

    if (nGlows > MAX_NUM_GLOWS) {
        O3D_ERROR(E_InvalidFormat("Invalid glows number"));
    }

	// read all flares
    for (Int32 i = 0 ; i < nGlows ; ++i) {
		m_glowManager[i] = new LensGlowItem;

        if (m_flareManager[i]) {
            is >> m_glowManager[i]->color
				 >> m_glowManager[i]->blending
				 >> m_glowManager[i]->halfSizeX
				 >> m_glowManager[i]->halfSizeY
				 >> m_glowManager[i]->is3dGlow
				 >> m_glowManager[i]->isBehindEffect
				 >> m_glowManager[i]->minIntensity
				 >> m_glowManager[i]->attenuationRange
				 >> m_glowManager[i]->name;

            m_glowManager[i]->texture = pScene->getTextureManager()->readTexture2D(is);
        } else {
            O3D_ERROR(E_InvalidAllocation("New glow is null"));
        }
	}

	return True;
}
