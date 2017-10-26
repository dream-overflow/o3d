/**
 * @file lensflaremodel.h
 * @brief Lens flare effect model definition.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2002-08-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_LENSFLAREMODEL_H
#define _O3D_LENSFLAREMODEL_H

#include "gloweffect.h"
#include "../blending.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class LensFlareModel
//-------------------------------------------------------------------------------------
//! The SetPosition method accepts a float value from -1 to 1 :
//!  - 1 Indicates that the flare is on the source (on the glow)
//!  - 0 Indicates that the flare is on the center of the screen
//!  - -1 Indicates that the flare is on the symmetry of the source by the center
//!
//! The AttenuationRange accepts a float value from 0 to 1.
//! This parameter define how the attenuation is computed :
//!  - 1 means that the effect is completely attenuated when the source is on the border
//!  of the screen
//!  - 0.5 means that the effect is completely attenuated at half the border of the
//!  screen to the screen center.
//---------------------------------------------------------------------------------------
class O3D_API LensFlareModel
{
public:

	//! Max number of flares.
	static const Int32 MAX_NUM_FLARES = 16;
	//! Max number of glow.
	static const Int32 MAX_NUM_GLOWS = 8;

	//! This struct describe a flare used by the lens effect
	struct LensFlareItem
	{
        Texture2D *texture;      //!< Texture to use with this flare
        Blending::FuncProfile blending;  //!< blending function profile
        Color color;             //!< Color of this flare
		Float halfSizeX;         //!< X Half Size of this flare
		Float halfSizeY;         //!< Y Half Size of this flare
		Float position;          //!< Position of this flare [-1..1]
		Float attenuationRange;  //!< Define the attenuation of this flare [0..1]

		String name;                 //!< optional name

		//! Assign operator
		LensFlareItem& operator=(const LensFlareItem &dup);
	};

	//! This struct describe a glow/ray effect used by the lens effect
	struct LensGlowItem
	{
        Texture2D *texture;      //!< Texture to use with this glow/ray
        Blending::FuncProfile blending;  //!< blending function profile
        Color color;             //!< Color of this glow/ray
		Float halfSizeX;         //!< X Half Size of this glow/ray
		Float halfSizeY;         //!< Y Half Size of this glow/ray
		Float attenuationRange;  //!< Define the attenuation of this Glow
		Float minIntensity;      //!< minimal glow intensity
		Bool is3dGlow;           //!< Glow/Ray type
		Bool isBehindEffect;     //!< Define if this glow/ray is behind the scene

		String name;                 //!< optional name

		//! Assign operator
		LensGlowItem& operator=(const LensGlowItem &dup);
	};

	//! Default contructor.
	LensFlareModel();
	//! Destructor.
	~LensFlareModel();

	//! Assign operator
	LensFlareModel& operator=(const LensFlareModel &dup);

	//! Add a flare and return its index
	Int32 addFlare(const LensFlareItem &Flare);
	//! Add a flare and return its index
	Int32 addFlare(Texture2D *pTexture,Float SizeX,Float SizeY,Float Position);

	//! Add a glow or ray and return its index
	Int32 addGlow(const LensGlowItem &Glow);
	//! Add a glow or ray and return its index
	Int32 addGlow(Texture2D *pTexture,Float SizeX,Float SizeY,Bool isBehindEffect);

    //! Get a flare/glow by it's index (read only)
	inline const LensFlareModel::LensFlareItem* getFlare(Int32 index) const
	{
		if (index < MAX_NUM_FLARES)
			return m_flareManager[index];
		else
            return nullptr;
	}
    //! Get a flare/glow by it's index (read write)
	inline LensFlareModel::LensFlareItem* getFlare(Int32 index)
	{
		if (index < MAX_NUM_FLARES)
			return m_flareManager[index];
		else
            return nullptr;
	}

	//! Get a glow by it's index (read only)
	inline const LensFlareModel::LensGlowItem* getGlow(Int32 index) const
	{
		if (index < MAX_NUM_GLOWS)
			return m_glowManager[index];
		else
            return nullptr;
	}
	//! Get a glow by it's index (read write)
	inline LensFlareModel::LensGlowItem* getGlow(Int32 index)
	{
		if (index < MAX_NUM_GLOWS)
			return m_glowManager[index];
		else
            return nullptr;
	}

	//! Remove a flare
	void removeFlare(Int32 index);
	//! Remove a glow/ray
	void removeGlow(Int32 index);

	//-----------------------------------------------------------------------------------
	// Parameters
	//-----------------------------------------------------------------------------------

	//! Set infinite lens (ex. sun)
	inline void setInfinite(Bool Infinite) { m_infinite = Infinite; }
	//! Is infinite lens (ex. sun)
	inline Bool getInfinite() const { return m_infinite; }

	//! Set lens effect half size
	inline void setHalfSize(Float HalfSizeX,Float HalfSizeY)
	{
		m_halfSizeX = HalfSizeX;
		m_halfSizeY = HalfSizeY;
	}
	//! Set lens effect half width
	inline void setHalfSizeX(Float HalfSizeX) { m_halfSizeX = HalfSizeX; }
	//! Set lens effect half height
	inline void setHalfSizeY(Float HalfSizeY) { m_halfSizeY = HalfSizeY; }

	//! Set lens effect size
	inline void setSize(Float SizeX,Float SizeY)
	{
		m_halfSizeX = SizeX * 0.5f;
		m_halfSizeY = SizeY * 0.5f;
	}
	//! Set lens effect width
	inline void setSizeX(Float SizeX) { m_halfSizeX = SizeX * 0.5f; }
	//! Set lens effect height
	inline void setSizeY(Float SizeY) { m_halfSizeY = SizeY * 0.5f; }

	//! Get lens effect width
	inline Float getSizeX() const { return m_halfSizeX * 2.0f; }
	//! Get lens effect height
	inline Float getSizeY() const { return m_halfSizeY * 2.0f; }

	//! Get lens effect half width
	inline Float getHalfSizeX() const { return m_halfSizeX; }
	//! Get lens effect half height
	inline Float getHalfSizeY() const { return m_halfSizeY; }

	//! Set max distance to render this effect
	inline void setMaxDistance(Float MaxDistance) { m_maxDistance = MaxDistance; }
	//! Set min distance to render this effect
	inline void setMinDistance(Float MinDistance) { m_minDistance = MinDistance; }
	//! Get max distance to render this effect
	inline Float getMaxDistance() const { return m_maxDistance; }
	//! Get min distance to render this effect
	inline Float getMinDistance() const { return m_minDistance; }

	//! set max fade range for the min/max distance
	inline void setMaxFadeRange(Float MaxFadeRange) { m_maxFadeRange = MaxFadeRange; }
	//! Set min fade range for the min/max distance
	inline void setMinFadeRange(Float MinFadeRange) { m_minFadeRange = MinFadeRange; }
	//! Get max fade range for the min/max distance
	inline Float getMaxFadeRange() const { return m_maxFadeRange; }
	//! Get min fade range for the min/max distance
	inline Float getMinFadeRange() const { return m_minFadeRange; }

	//! Set fade in persistence (time in seconds to fade-in [0..1])
	inline void setFadeInPersistence(Float Persistence) { m_fadeInPersistence = Persistence; }
	//! Get fade in persistence (time in seconds to fade-in [0..1])
	inline Float getFadeInPersistence() const { return m_fadeInPersistence; }

	//! Set fade out persistence (time in seconds to fade-out [0..1])
	inline void setFadeOutPersistence(Float Persistence) { m_fadeOutPersistence = Persistence; }
	//! Get fade out persistence (time in seconds to fade-out [0..1])
	inline Float getFadeOutPersistence() const { return m_fadeOutPersistence; }

	//! Set if we perform a simple occlusion test (no ratio just visible or occluded)
	inline void setSimpleOcclusion(Bool simpleTest) { m_simpleOcclusion = simpleTest; }
	//! Get if we perform a simple occlusion test (no ratio just visible or occluded)
	inline Bool getSimpleOcclusion() const { return m_simpleOcclusion; }

	//! Get the num of flares
	Int32 getNumFlares()const;
	//! Get the num of glows/rays
	Int32 getNumGlows()const;

	// serialization
	Bool writeToFile(OutStream &os) const;
    Bool readFromFile(Scene *pScene, InStream& is);

protected :

	LensFlareItem* m_flareManager[MAX_NUM_FLARES]; //!< Manager for all flares
	LensGlowItem*  m_glowManager[MAX_NUM_GLOWS];   //!< Manager for all glow/rays

	Float m_halfSizeX;       //!< Half SizeX of the source
	Float m_halfSizeY;       //!< Half SizeY of the source

	Float m_maxDistance;     //!< The max distance to render this effect
	Float m_minDistance;     //!< The min distance to render this effect

	Float m_maxFadeRange;    //!< When the src is between MaxDistance and MaxDistance-MaxFadeRange the effect is linearly faded
	Float m_minFadeRange;    //!< When the src is between MinDistance and MinDistance-MinFadeRange the effect is linearly faded

	Float m_fadeInPersistence;   //!< Define the persistence (time to fade-in [0..1])
	Float m_fadeOutPersistence;  //!< Define the persistence (time to fade-out [1..0])

	Bool m_simpleOcclusion;   //!< Define if we just do a simple occlusion test (no ratio just visible or occluded)
	Bool m_infinite;          //!< This effect is for infinite src or not
};

} // namespace o3d

#endif // _O3D_LENSFLAREMODEL_H

