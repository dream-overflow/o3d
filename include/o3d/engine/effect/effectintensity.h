/**
 * @file effectintensity.h
 * @brief This interface is used by effect with a variable intensity.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2002-08-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_EFFECTINTENSITY_H
#define _O3D_EFFECTINTENSITY_H

#include "specialeffects.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class EffectIntensity
//-------------------------------------------------------------------------------------
//! This interface is used by effect with a variable intensity
//! For example, a glow effect have a variable intensity.
//! This intensity depends on how far the effect is or if it's partially visible...
//! If we use a glow in a multi effect like a lens flare effect, it's useful to use
//! the lens flare effect intensity instead of the glow intensity.
//---------------------------------------------------------------------------------------
class O3D_API EffectIntensity : public SpecialEffects
{
public:

	O3D_DECLARE_CLASS(EffectIntensity)

	//! Intensity type.
	enum IntensityType
	{
		INTENSITY_UNDEFINED,
		INTENSITY_SELF,              //!< The effect calculates his intensity him self
		INTENSITY_CONST,             //!< The effect has a constant intensity
		INTENSITY_PARENT	         //!< The effect uses his parent intensity
	};

	//! Default contructor
	//! @param pParent Can be the effects manager or another parent effect
	//! @param itype Can be O3DParentIntensity only and only if the parent is another effect
	EffectIntensity(BaseObject *parent, IntensityType itype = INTENSITY_SELF) :
		SpecialEffects(parent),
		m_intensity(1.f),
		m_intensityType(itype),
		m_parentId(-1)
	{
	}

	//! With parent effect contructor
	//! @param pParent Must be an effect
	//! @note The default intensity type is set to O3DParentIntensity
	EffectIntensity(EffectIntensity *parent) :
		SpecialEffects(parent),
		m_intensity(1.f),
		m_intensityType(INTENSITY_PARENT),
		m_parentId(-1)
	{
	}

	//! Destructor
	virtual ~EffectIntensity();

	//! return the intensity of this effect (if ParentIntensity : return m_Intensity*parentItensity)
	Float getIntensity() const;

	//! set the intensity of this effect
	inline void setIntensity(Float intensity) { m_intensity = intensity; }

	//! get the kind of intensity for this effect
	inline IntensityType getIntensityType() const { return m_intensityType; }
	//! set the kind of intensity for this effect
	//! @note O3DParentIntensity can be set only and only if the parent is another effect
	void setIntensityType(IntensityType intensityType);

	// serialization
	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

	// post import method
	virtual void postImportPass();

protected:

	Float m_intensity;         //!< intensity of this effect [0..1]
	IntensityType m_intensityType; //!< kind of intensity

	Int32 m_parentId;          //!< only for import
};

} // namespace o3d

#endif // _O3D_EFFECTINTENSITY_H

