/**
 * @file lenseffect.h
 * @brief Complex Lens Flare effects.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-08-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_LENSEFFECT_H
#define _O3D_LENSEFFECT_H

#include "effectintensity.h"
#include "multieffect.h"
#include "gloweffect.h"
#include "lensflaremodel.h"
#include "../shader/shader.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class OcclusionQuery;

//---------------------------------------------------------------------------------------
//! @class LensEffect
//-------------------------------------------------------------------------------------
//! Lens effect. Can be of two kind:
//!   - linked to a scene graph object (on a light for example with a world position)
//!   - infinite lens effect (like the sun for example).
//! A lens effect is compound of a glow effect, a ray effect, and of lens.
//---------------------------------------------------------------------------------------
class O3D_API LensEffect : public EffectIntensity, public MultiEffect
{
public:

	O3D_DECLARE_CLASS(LensEffect)

	//! Default constructor.
	LensEffect(BaseObject *parent);
	//! Constructor. Copy the given lensflare model.
	LensEffect(BaseObject *parent, const LensFlareModel &pModel);
	//! Constructor. Copy the given lensflare model and override the 'infinite' parameter.
	LensEffect(BaseObject *parent, const LensFlareModel &pModel, Bool Infinite);

	//! Virtual dDestructor.
	virtual ~LensEffect();

	//-----------------------------------------------------------------------------------
	// Parameters
	//-----------------------------------------------------------------------------------

	//! is the source is at infinite (sun)
	inline Bool getInfinite() const { return m_infinite; }

	//! get the radius used by the frustum culling
	inline Float getEffectRadius() const { return m_effectRadius; }

	//! set/get the lens source position (local)
	inline void setPosition(const Vector3 &refPos)
	{
		if (!m_infinite)
		{
			m_lensVector = refPos;
			// Update the position of all glows
			updateGlowsPositions();
		}
	}
	inline const Vector3 &getPosition() const { return m_lensVector; }

	//! set/get the lens source direction (infinite)
	inline void setDirection(const Vector3 &refDir)
	{
		if (m_infinite)
		{
			m_lensVector = refDir;
			m_lensVector.normalize();

			// Update the position of all glows
			updateGlowsPositions();
		}
	}
	inline const Vector3 &GetDirection()const { return m_lensVector; }

	//! set lens effect half size
	inline void setHalfSize(Float HalfSizeX, Float HalfSizeY)
	{
		m_halfSizeX = HalfSizeX;
		m_halfSizeY = HalfSizeY;
	}
	inline void setHalfSizeX(Float HalfSizeX) { m_halfSizeX = HalfSizeX; }
	inline void setHalfSizeY(Float HalfSizeY) { m_halfSizeY = HalfSizeY; }

	//! set lens effect size
	inline void setSize(Float SizeX,Float SizeY)
	{
		m_halfSizeX = SizeX * 0.5f;
		m_halfSizeY = SizeY * 0.5f;
	}
	inline void SetSizeX(Float SizeX) { m_halfSizeX = SizeX * 0.5f; }
	inline void SetSizeY(Float SizeY) { m_halfSizeY = SizeY * 0.5f; }

	//! get lens effect size
	inline Float GetSizeX() const { return m_halfSizeX * 2.0f; }
	inline Float GetSizeY() const { return m_halfSizeY * 2.0f; }

	//! get lens effect half size
	inline Float getHalfSizeX() const { return m_halfSizeX; }
	inline Float getHalfSizeY() const { return m_halfSizeY; }

	//! set/get the max distance to render this effect
	inline void setMaxDistance(Float MaxDistance) { m_maxDistance = MaxDistance; }
	inline Float getMaxDistance() const { return m_maxDistance; }

	//! set/get the min distance to render this effect
	inline void setMinDistance(Float MinDistance) { m_minDistance = MinDistance; }
	inline Float getMinDistance() const { return m_minDistance; }

	//! set/get the fade range for the max distance
	inline void setMaxFadeRange(Float MaxFadeRange) { m_maxFadeRange = MaxFadeRange; }
	inline Float getMaxFadeRange() const { return m_maxFadeRange; }

	//! set/get the fade range for the min distance
	inline void setMinFadeRange(Float MinFadeRange) { m_minFadeRange = MinFadeRange; }
	inline Float getMinFadeRange() const { return m_minFadeRange; }

	//! set/get the camera->lens distance
	inline void setCameraDistance(Float Distance) { m_distance = Distance; }
	inline Float getCameraDistance() const { return m_distance; }

	//! Set the lens flare model
	void setLensFlareModel(const LensFlareModel &pModel);
	//! Set the lens flare model and override the 'infinite' parameter
	void setLensFlareModel(const LensFlareModel &pModel, Bool Infinite);

	//! Get the lens flare model (read only)
	const LensFlareModel& getLensFlareModel();

	//! set/get the fade-in persistence of this glow (time in seconds to fade-in [0..1])
	inline void setFadeInPersistence(Float Persistence) { m_fadeInPersistence = Persistence; }
	inline Float getFadeInPersistence() const { return m_fadeInPersistence; }

	//! set/get the fade-out persistence of this glow (time in seconds to fade-out [1..0])
	inline void setFadeOutPersistence(Float Persistence) { m_fadeOutPersistence = Persistence; }
	inline Float getFadeOutPersistence() const { return m_fadeOutPersistence; }

    //! set/get if we perform a simple occlusion test (no ratio just visible or occluded)
	inline void setSimpleOcclusion(Float simpleTest) { m_simpleOcclusion = simpleTest; }
	inline Float getSimpleOcclusion() const { return m_simpleOcclusion; }

	void setUpModelView();

	//! draw the effect
	virtual void draw(const DrawInfo &drawInfo);
 	//! update the effect
	virtual void update();

	//! serialization
	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

	// post import method
	virtual void postImportPass();

protected:

	struct DiffuseMapShader
	{
		ShaderInstance instance;

		Int32 u_modelViewProjectionMatrix;
		Int32 u_color;
		Int32 u_diffuseMap;
		Int32 a_vertex;
		Int32 a_texCoords;
	};

	DiffuseMapShader m_diffuseMapShader;

	struct OcclusionShader
	{
		ShaderInstance instance;

		Int32 u_modelViewProjectionMatrix;
		Int32 a_vertex;
	};

	OcclusionShader m_occlusionShader;

	//! Manager for flares textures.
	SmartObject<Texture2D> m_flareTextures[LensFlareModel::MAX_NUM_FLARES];

	ArrayBufferf m_vertices;
	ArrayBufferf m_texCoords;

	Vector3 m_lensVector;          //!< lens effect source position or direction (world space)

	OcclusionQuery *m_occlusionQuery;  //!< for know what is visible
	OcclusionQuery *m_drawQuery;  //!< for get the pixel number if all the object was visible

	Float m_distance;             //!< distance between the lens effect and the camera

	Bool m_infinite;              //!< is this lens effect is infinite

	Float m_halfSizeX;            //!< X glow size
	Float m_halfSizeY;            //!< Y glow size
	Float m_maxDistance;          //!< the max distance to render this effect
	Float m_minDistance;          //!< the min distance to render this effect
	Float m_minFadeRange;         //!< when the src is between MinDistance and MinDistance-MinFadeRange the effect is linearly faded
	Float m_maxFadeRange;         //!< when the src is between MaxDistance and MaxDistance-MaxFadeRange the effect is linearly faded
	Float m_effectRadius;         //!< the radius used by the frustum culling to clip this effect

	LensFlareModel m_lensFlareModel;  //!< Lensflare effect model

	Float m_visibilityRatio;      //!< visibility Ratio calculated using occlusions
	Float m_fadeInPersistence;    //!< define the persistence (time to fade-in [0..1])
	Float m_fadeOutPersistence;   //!< define the persistence (time to fade-out [1..0])
	Bool m_simpleOcclusion;       //!< define if we just do a simple occlusion test (no ratio just visible or occluded)
	Int64 m_lastFrameTime;        //!< used to have an elapsed time

	void createOcclusionQueries();    //!< create the occlusion test
	void checkOcclusionQueries();     //!< make the occlusion test (transform must be defined)
	void deleteOcclusionQueries();    //!< release occlusions queries used
	void renderOcclusionTestMesh();   //!< render the test mesh for occlusion
	void calculateVisibilityRatio();  //!< compute the visibility ratio of this effect using occlusion queries
	void generateGlowsObj();          //!< generate glow from flare model
	void updateGlowsPositions();      //!< update the position of all glows

	//! compute glow attenuation depending on the AttenuationRange
	void calcGlowAttenuationRange(const Vector3 &refScreenPos);

	void createShader();              //!< Create shader program
};

} // namespace o3d

#endif // _O3D_LENSEFFECT_H
