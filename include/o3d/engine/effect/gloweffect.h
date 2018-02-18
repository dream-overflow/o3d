/**
 * @file gloweffect.h
 * @brief A glow effect mainly used for lens flare effect.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-08-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_GLOWEFFECT_H
#define _O3D_GLOWEFFECT_H

#include "effectintensity.h"
#include "o3d/core/smartpointer.h"
#include "../material/materialprofile.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class Texture2D;

//---------------------------------------------------------------------------------------
//! @class GlowEffect
//-------------------------------------------------------------------------------------
//! There is two type of glow :
//!   - Atmospheric glow (light diffused by air)
//!   - Glow from a lens (light diffused by glass)
//!
//! The first type must be behind the landscape,
//! whereas the second type must ahead the landscape.
//!
//! A glow can be renderer from 3 ways :
//!   - Infinite glow, we define its direction
//!   - Local glow, we define a position
//!   - Local world glow, we define a position
//!
//! The first two type of glows are always setup at the same distance from the camera.
//! That mean they have a fixed size. For the third type of glow, it is rendered like
//! a standard object, its position is expressed in world space.
//!
//! The 'behind' parameter define if a glow is set infinite. If it is infinite,
//! it will be behind the entire scene, this is useful for the sun (atmospheric glow).

//! An infinite glow, in facts, have a depth of 1.
//!
//! @note A glow is not created itself,  it should be managed by O3DLensEffect that
//! manage all lens effects. And O3DLensEffect is responsible of the occlusion test.
//---------------------------------------------------------------------------------------
class O3D_API GlowEffect : public EffectIntensity
{
public:

	O3D_DECLARE_CLASS(GlowEffect)

	static const Float DEFAULT_DISTANCE;   //!< Glow default distance. 100.0f.
	static const Float DEFAULT_INTENSITY;  //!< Glow default intensity. 1.0f.

	//! Glow type for lens effect.
	enum GlowType
	{
		GLOW_UNDEFINED = 0,    //!< Undefined glow type.
		GLOW_INFINITE,         //!< Glow infinite, we use only the first direction
		GLOW_LOCAL,            //!< Glow local, we use only the position
		GLOW_LOCAL_WORLD       //!< Glow local, positioned such as a simple object in the world
	};

	//! Default constructor/
    GlowEffect(BaseObject *parent, Texture2D *texture = nullptr);

	//-----------------------------------------------------------------------------------
	// Parameters
	//-----------------------------------------------------------------------------------

	//! Set glow type.
	inline void setGlowType(GlowType glowType) { m_glowType = glowType; }
	//! Get glow type.
	inline GlowType getGlowType() const { return m_glowType; }

	//! Set local position.
	inline void setPosition(const Vector3 &refPos)
	{
		if (m_glowType != GLOW_INFINITE)
			m_glowVector = refPos;
	}
	//! Get local position.
	inline const Vector3& getPosition() const { return m_glowVector; }

	//! Set direction (infinite).
	inline void setDirection(const Vector3 &refDir)
	{
		if (m_glowType == GLOW_INFINITE)
		{
			m_glowVector = refDir;
			m_glowVector.normalize();
		}
	}
	//! Get direction (infinite).
	inline const Vector3& getDirection() const { return m_glowVector; }

	//! Set directly the glow vector.
	inline void setGlowVector(const Vector3 &refVec) { m_glowVector = refVec; }

	//! Set the half-size (X,Y).
	inline void setHalfSize(Float halfSizeX, Float halfSizeY)
	{
		m_halfSizeX = halfSizeX;
		m_halfSizeY = halfSizeY;
	}

	//! Set the glow X size.
	inline void setSizeX(Float halfWidth) { m_halfSizeX = halfWidth * 0.5f; }
	//! Set the glow Y size.
	inline void setSizeY(Float halfHeight) { m_halfSizeY = halfHeight * 0.5f; }

	//! Get the glow X size.
	inline Float getSizeX() const { return m_halfSizeX * 2.f; }
	//! Get the glow Y size.
	inline Float getSizeY() const { return m_halfSizeY * 2.f; }

	//! Set attenuation range [0..1].
	inline void setAttenuationRange(Float attenuationRange)
	{
		m_attenuationRange = o3d::clamp(attenuationRange,o3d::Limits<Float>::epsilon(),1.f);
	}
	//! Get attenuation range [0..1].
	inline Float getAttenuationRange()const { return m_attenuationRange; }

	//! Set glow color.
	inline void setColor(const Color &color) { m_color = color; }
	//! Get glow color.
	inline const Color& getColor() const { return m_color; }

	//! Set distance camera/glow (invalid for O3DGlowLocalWorld).
	inline void setCameraDistance(Float distance)
	{
		if (m_glowType != GLOW_LOCAL_WORLD)
			m_distance = distance;
	}
	//! Get distance camera/glow (invalid for O3DGlowLocalWorld).
	inline Float getCameraDistance() const { return m_distance; }

	//! Set glow intensity (alpha canal for color).
	inline void setGlowIntensity(Float glowIntensity) { m_color.alpha(glowIntensity); }
	//! Get glow intensity (alpha canal for color).
	inline Float getGlowIntensity() const { return m_color.alpha(); }

	//! set/get glow minimal intensity
	inline void setMinIntensity(Float minIntensity)
	{
		m_minIntensity = o3d::clamp(minIntensity,0.f,1.f);
	}
	inline Float getMinIntensity() const { return m_minIntensity; }

	//! Set is behind effect.
	void setBehindEffect(Bool isBehindEffect);
	//! Get is behind effect.
	inline Bool isBehindEffect() const { return m_isBehindEffect; }

	//! Compute the intensity with attenuation settings.
	void calcAttenuationRange(const Vector3 &refScreenPos);

	//! Get the texture.
	Texture2D* getTexture() const;

	//! Draw all faces.
    virtual void processAllFaces(Shadable::ProcessingPass pass) override;

	//! Get the current material profile (read only).
	inline const MaterialProfile& getMaterialProfile() const { return m_material; }
	//! Get the current material profile.
	inline MaterialProfile& getMaterialProfile() { return m_material; }

    virtual void attribute(VertexAttributeArray mode, UInt32 location) override;
    virtual void setUpModelView() override;

	//! Draw the effect.
    virtual void draw(const DrawInfo &drawInfo) override;

	// Serialization.
    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

protected:

    Vector3 m_glowVector;      //!< position or direction (world space)
    Color   m_color;           //!< RGBA color
	Float  m_halfSizeX;        //!< x half-size of the glow
	Float  m_halfSizeY;        //!< y half-size of the glow
    GlowType   m_glowType;     //!< kind of glow
	Float  m_distance;         //!< distance between the glow and the camera (unused for O3DGlowLocalWorld)
	Float  m_attenuationRange; //!< attenuation for the glow
	Float  m_minIntensity;     //!< Guaranteed minimal intensity
	Bool   m_isBehindEffect;   //!< Define if the effect is behind all the scene (and not behind the sky)

    MaterialProfile m_material;    //!< Material profile.
};

} // namespace o3d

#endif // _O3D_GLOWEFFECT_H
