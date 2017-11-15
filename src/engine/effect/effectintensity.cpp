/**
 * @file effectintensity.cpp
 * @brief Implementation of EffectIntensity.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-08-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/effect/effectintensity.h"

#include "o3d/core/debug.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/effect/specialeffectsmanager.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(EffectIntensity, ENGINE_EFFECT_INTENSITY, SpecialEffects)

// destructor
EffectIntensity::~EffectIntensity()
{
}

// return the intensity of this effect
Float EffectIntensity::getIntensity() const
{
	if ((m_intensityType == INTENSITY_SELF) || (m_intensityType == INTENSITY_CONST))
	{
		return m_intensity;
	}
	else if (m_intensityType == INTENSITY_PARENT)
	{
		if (m_parent)
		{
			return m_intensity * reinterpret_cast<EffectIntensity*>(m_parent)->getIntensity();
		}
		else
		{
			return m_intensity;
		}
	}
	else
	{
		return m_intensity;
	}
}

void EffectIntensity::setIntensityType(IntensityType intensityType)
{
	if ((intensityType == INTENSITY_PARENT) && (m_parent->getType() == ENGINE_EFFET_LIST))
	{
		O3D_ERROR(E_InvalidParameter("O3DParentIntensity can be set only for child effect"));
		return;
	}

	m_intensityType = intensityType;
}

// Serialization
Bool EffectIntensity::writeToFile(OutStream &os)
{
    SpecialEffects::writeToFile(os);

    os   << m_intensity
		 << (Int32)m_intensityType;

	if (m_intensityType == INTENSITY_PARENT)
	{
		if (!m_parent)
		{
			O3D_ERROR(E_InvalidPrecondition("Undefined parent effect"));
			return False;
		}
        os << m_parent->getSerializeId();
	}
	else
        os << (Int32)(-1);

	return True;
}

Bool EffectIntensity::readFromFile(InStream &is)
{
    SpecialEffects::readFromFile(is);

	Int32 intensityType;

    is   >> m_intensity
		 >> intensityType
		 >> m_parentId;

	m_intensityType = IntensityType(intensityType);

	return True;
}

void EffectIntensity::postImportPass()
{
	// get effect ptr from scene
	if (m_parentId != -1)
	{
		m_parent = o3d::dynamicCast<EffectIntensity*>(
				getScene()->getSpecialEffectsManager()->getImportedSpecialEffects(m_parentId));

		if (!m_parent)
			O3D_ERROR(E_InvalidFormat("The parent special effect does not inherit from EffectIntensity"));
	}
	else
		O3D_ERROR(E_InvalidParameter("Unable to find the parent special effect"));
}

