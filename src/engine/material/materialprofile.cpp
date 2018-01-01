/**
 * @file materialprofile.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/texture/cubemaptexture.h"
#include "o3d/engine/drawinfo.h"
#include "o3d/engine/shader/shadable.h"
#include "o3d/engine/material/materialtechnique.h"
#include "o3d/engine/material/materialprofile.h"
#include "o3d/engine/lodstrategy.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/core/math.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(MaterialProfile, ENGINE_MATERIAL_PROFILE, SceneEntity)

// Default constructor.
MaterialProfile::MaterialProfile(BaseObject *parent) :
	SceneEntity(parent),
	m_activity(True),
    m_lodStrategy(nullptr)
{
	// first LOD level start at 0
	m_lodList.push_back(0.f);
}

// Virtual destructor.
MaterialProfile::~MaterialProfile()
{
	// delete any techniques
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		deletePtr(m_techniques[i]);
    }
}

// Copy operator.
MaterialProfile& MaterialProfile::operator= (const MaterialProfile &dup)
{
	// copy techniques
	UInt32 numTechniques = static_cast<UInt32>(dup.m_techniques.size());
	setNumTechniques(static_cast<UInt32>(numTechniques));

    for (UInt32 i = 0; i < numTechniques; ++i) {
		*m_techniques[i] = *dup.m_techniques[i];
    }

	// LOD levels
	m_lodList = dup.m_lodList;

	m_lodStrategy = dup.m_lodStrategy;

	return *this;
}

// Set ambient color for any passes of any techniques.
void MaterialProfile::setAmbient(const Color &color)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		m_techniques[i]->setAmbient(color);
    }
}

// Set diffuse color for any passes of any techniques.
void MaterialProfile::setDiffuse(const Color &color)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		m_techniques[i]->setDiffuse(color);
    }
}

// Set specular color for any passes of any techniques.
void MaterialProfile::setSpecular(const Color &color)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		m_techniques[i]->setSpecular(color);
    }
}

// Set self illumination color for any passes of any techniques.
void MaterialProfile::setSelfIllumination(const Color &color)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		m_techniques[i]->setSelfIllumination(color);
    }
}

// Set the specular shininess exponent for any passes of any techniques.
void MaterialProfile::setShine(Float exponent)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		m_techniques[i]->setShine(exponent);
    }
}

// Set the specular shininess exponent for any passes of any techniques.
void MaterialProfile::setSpecularExponent(Float exponent)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		m_techniques[i]->setSpecularExponent(exponent);
    }
}

// Set the reflectivity coefficient for any passes of any techniques.
void MaterialProfile::setReflectivity(Float reflectivity)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		m_techniques[i]->setReflectivity(reflectivity);
    }
}

// Set the transparency coefficient for any passes of any techniques.
void MaterialProfile::setTransparency(Float transparency)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		m_techniques[i]->setTransparency(transparency);
    }
}

// Set the bump offset for any passes of any techniques.
void MaterialProfile::setBumpOffset(const Vector2f &offset)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		m_techniques[i]->setBumpOffset(offset);
    }
}

// Set the culling mode for any passes.
void MaterialProfile::setCullingMode(CullingMode mode)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		m_techniques[i]->setCullingMode(mode);
    }
}

// Set the blending mode for any passes.
void MaterialProfile::setBlendingFunc(Blending::FuncProfile func)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
        m_techniques[i]->setBlendingFunc(func);
    }
}

// Set if faces are sorted and displayed with the alpha-pipeline for any passes.
void MaterialProfile::setSorted(Bool state)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		m_techniques[i]->setSorted(state);
    }
}

// Set double side drawing mode status for any passes.
void MaterialProfile::setDoubleSide(Bool state)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		m_techniques[i]->setDoubleSide(state);
    }
}

// Set the depth buffer test status for any passes.
void MaterialProfile::setDepthTest(Bool state)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		m_techniques[i]->setDepthTest(state);
    }
}

// Set the depth buffer write status for any passes.
void MaterialProfile::setDepthWrite(Bool state)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		m_techniques[i]->setDepthWrite(state);
    }
}

// Set the depth range to normal (false) or infinite (true) for any passes.
void MaterialProfile::setInfiniteDepthRange(Bool state)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		m_techniques[i]->setInfiniteDepthRange(state);
    }
}

// Set the alpha test status for any passes.
void MaterialProfile::setAlphaTest(Bool state)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		m_techniques[i]->setAlphaTest(state);
    }
}

// Set the alpha test function mode and ref value for any passes.
void MaterialProfile::setAlphaTestFunc(Comparison func, Float ref)
{
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());

    for (UInt32 i = 0; i < numTechniques; ++i) {
		m_techniques[i]->setAlphaTestFunc(func, ref);
    }
}

// Set the LOD strategy.
void MaterialProfile::setLodStrategy(LodStrategy *lodStrategy)
{
	m_lodStrategy = lodStrategy;
}

// Set the LOD levels list with distances. LOD strategy must be previously defined.
void MaterialProfile::setLodLevels(const std::vector<Float> &lodList)
{
	m_lodList.resize(lodList.size());

    if (!lodList.empty()) {
        for (size_t i = 0; i < lodList.size(); ++i) {
			m_lodList[i] = o3d::sqr(lodList[i]);
        }

		m_lodList[0] = 0.f;
	}
}

// Get the LOD levels list with distances.
std::vector<Float> MaterialProfile::getLodLevels() const
{
	std::vector<Float> result;
	result.resize(m_lodList.size());

    if (!m_lodList.empty()) {
        for (size_t i = 0; i < m_lodList.size(); ++i) {
            result[i] = Math::sqrt(m_lodList[i]);
        }

		result[0] = 0.f;
	}

	return result;
}

// Define the number of technique to create.
void MaterialProfile::setNumTechniques(UInt32 numTechniques)
{
	// delete more techniques
    if (m_techniques.size() > static_cast<size_t>(numTechniques)) {
        for (size_t i = numTechniques; i < m_techniques.size(); ++i) {
			deletePtr(m_techniques[i]);
        }
	}

	size_t oldSize = m_techniques.size();
	m_techniques.resize(numTechniques);

	// add more techniques
    if (oldSize < static_cast<size_t>(numTechniques)) {
        for (size_t i = oldSize; i < numTechniques; ++i) {
            m_techniques[i] = new MaterialTechnique(this);
        }
	}
}

// Get the current technique for a specific index (read only).
const MaterialTechnique& MaterialProfile::getTechnique(UInt32 index) const
{
    if (static_cast<size_t>(index) >= m_techniques.size()) {
		O3D_ERROR(E_IndexOutOfRange("Technique index"));
    }

	return *m_techniques[index];
}

// Get the current technique for a specific index.
MaterialTechnique& MaterialProfile::getTechnique(UInt32 index)
{
    if (static_cast<size_t>(index) >= m_techniques.size()) {
		O3D_ERROR(E_IndexOutOfRange("Technique index"));
    }

	return *m_techniques[index];
}

// Determine the techniques to use and compile them.
void MaterialProfile::prepareAndCompile(Shadable &shadable)
{
	m_activeTechniques.resize(m_lodList.size());

	// determine for each LOD level the best technique to use
	size_t numTechniques = m_techniques.size();
	size_t numLodLevels = m_lodList.size();
    for (size_t i = 0; i < numLodLevels; ++i) {
		// no technique for the moment
        m_activeTechniques[i] = nullptr;

		// search one
        for (size_t j = 0; j < numTechniques; ++j) {
			// technique found for the current LOD level and material specificities
            if ((m_techniques[j]->getLodIndex() == i) && (m_techniques[j]->isTechniqueSupported())) {
				m_activeTechniques[i] = m_techniques[j];
				break;
			}
		}
	}

	// prepare and compile all chosen techniques
	numTechniques = m_activeTechniques.size();
    for (size_t i = 0; i < numTechniques; ++i) {
		if (m_activeTechniques[i])
			m_activeTechniques[i]->prepareAndCompile(shadable);
    }
}

void MaterialProfile::clear()
{
    // prepare and compile all chosen techniques
    size_t numTechniques = m_activeTechniques.size();
    for (size_t i = 0; i < numTechniques; ++i) {
        if (m_activeTechniques[i]) {
            m_activeTechniques[i]->clear();
        }
    }
}

// Get the optimal material according to the current scene context and parameters.
MaterialTechnique* MaterialProfile::getBestTechnique(Float squaredDistance)
{
	UInt32 lodIndex = getLodIndex(squaredDistance);
    if (lodIndex < static_cast<UInt32>(m_activeTechniques.size())) {
		return m_activeTechniques[lodIndex];
    } else {
        return nullptr;
    }
}

// Get the LOD index for a specific distance (squared distance value).
UInt32 MaterialProfile::getLodIndex(Float squaredDistance) const
{
    if (m_lodStrategy) {
		return m_lodStrategy->getIndex(squaredDistance, m_lodList);
    } else {
		return 0;
    }
}

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/object/camera.h"

// Process the material profile to a shadable object.
void MaterialProfile::processMaterial(
		Shadable &shadable,
		Shadowable *shadowable,
		Pickable *pickable,
		const DrawInfo &drawInfo)
{
    if (m_activity) {
		Float squaredDistance = shadable.getDistanceFrom(
				getScene()->getActiveCamera()->getAbsoluteMatrix().getTranslation());

		MaterialTechnique *technique = getBestTechnique(squaredDistance);
        if (technique) {
			technique->processMaterial(shadable, shadowable, pickable, drawInfo);
		}
	}
}

Bool MaterialProfile::writeToFile(OutStream &os)
{
    SceneEntity::writeToFile(os);

    os << m_activity;

	// techniques
	UInt32 numTechniques = static_cast<UInt32>(m_techniques.size());
    os << numTechniques;

    for (UInt32 i = 0; i < numTechniques; ++i) {
        os << *m_techniques[i];
    }

	// LOD levels
	std::vector<Float> lodLevels = getLodLevels();

    os << static_cast<UInt32>(lodLevels.size());
    for (size_t i = 0; i < lodLevels.size(); ++i) {
        os << lodLevels[i];
    }

	return True;
}

Bool MaterialProfile::readFromFile(InStream &is)
{
    SceneEntity::readFromFile(is);

    is >> m_activity;

	// techniques
	UInt32 numTechniques;
    is >> numTechniques;

	setNumTechniques(numTechniques);

    for (UInt32 i = 0; i < numTechniques; ++i) {
        is >> *m_techniques[i];
    }

	// LOD levels
	std::vector<Float> lodLevels;
	UInt32 numLod;

    is >> numLod;
	lodLevels.resize(numLod);

    for (UInt32 i = 0; i < numLod; ++i) {
        is >> lodLevels[i];
    }

	setLodLevels(lodLevels);

	return True;
}
