/**
 * @file materialprofile.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MATERIALPROFILE_H
#define _O3D_MATERIALPROFILE_H

#include "../blending.h"
#include "../scene/sceneentity.h"
#include "materialtechnique.h"
#include "o3d/image/color.h"
#include "o3d/core/vector2.h"

#include <vector>

namespace o3d {

class LodStrategy;
class Shadable;
class DrawInfo;

/**
 * @brief Define a set of material techniques and material settings.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-01-03
 * Define a set of material techniques. The technique is chosen according some
 * parameters like distance (level of detail), current GPU load or such others.
 * It also define the material settings.
 */
class O3D_API MaterialProfile : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(MaterialProfile)

	//! Default constructor.
	MaterialProfile(BaseObject *parent);

	//! Virtual destructor.
	virtual ~MaterialProfile();

	//! Copy operator.
	MaterialProfile& operator= (const MaterialProfile &dup);

	//-----------------------------------------------------------------------------------
	// Activity
	//-----------------------------------------------------------------------------------

	//! Set if the material must be applied or not.
	inline void setActivity(Bool activity) { m_activity = activity; }
	//! Get the material activity.
	inline Bool getActivity() const { return m_activity; }
	//! Enable the material.
	inline void enable() { setActivity(True); }
	//! Disable the material.
	inline void disable() { setActivity(False); }
	//! Toggle the material activity and return the new state.
	inline Bool toggleActivity()
	{
		m_activity = !m_activity;
		return m_activity;
	}

	//-----------------------------------------------------------------------------------
	// Manage material parameters. Manage any Passes of any Techniques
	//-----------------------------------------------------------------------------------

	//! Set ambient color for any passes of any techniques.
	void setAmbient(const Color &color);

	//! Set diffuse color for any passes of any techniques.
	void setDiffuse(const Color &color);

	//! Set specular color for any passes of any techniques.
	void setSpecular(const Color &color);

	//! Set self illumination color for any passes of any techniques.
	void setSelfIllumination(const Color &color);

	//! Set the specular shininess exponent for any passes of any techniques.
	void setShine(Float exponent);

	//! Set the specular shininess exponent for any passes of any techniques.
	void setSpecularExponent(Float exponent);

	//! Set the reflectivity coefficient for any passes of any techniques.
	void setReflectivity(Float reflectivity);

	//! Set the transparency coefficient for any passes of any techniques.
	void setTransparency(Float transparency);

	//! Set the bump offset for any passes of any techniques.
	void setBumpOffset(const Vector2f &offset);

	//-----------------------------------------------------------------------------------
	// Manager material context parameters. Manage any Passes of any Techniques.
	//-----------------------------------------------------------------------------------

	//! Set the culling mode for any passes.
	void setCullingMode(CullingMode mode);

	//! Set the blending mode for any passes.
    void setBlendingFunc(Blending::FuncProfile func);

	//! Set if faces are sorted and displayed with the alpha-pipeline for any passes.
	void setSorted(Bool state);

	//! Set double side drawing mode status for any passes.
	void setDoubleSide(Bool state);
	//! Enable the double side drawing mode for any passes.
	inline void enableDoubleSide() { setDoubleSide(True); }
	//! Disable the double side drawing mode for any passes.
	inline void disableDoubleSide() { setDoubleSide(False); }

	//! Set the depth buffer test status for any passes.
	void setDepthTest(Bool state);
	//! Enable the depth buffer test for any passes.
	inline void enableDepthTest() { setDepthTest(True); }
	//! Disable the depth buffer test for any passes.
	inline void disableDepthTest() { setDepthTest(False); }

	//! Set the depth buffer write status for any passes..
	void setDepthWrite(Bool state);
	//! Enable the depth buffer write for any passes..
	inline void enableDepthWrite() { setDepthWrite(True); }
	//! Disable the depth buffer writer for any passes.
	inline void disableDepthWrite() { setDepthWrite(False); }

	//! Set the depth range to normal (false) or infinite (true) for any passes.
	void setInfiniteDepthRange(Bool state);
	//! Set to infinite depth range for any passes.
	inline void enableInfiniteDepthRange() { setInfiniteDepthRange(True); }
	//! Set to normal depth range for any passes.
	inline void disableInfiniteDepthRange() { setInfiniteDepthRange(False); }

	//! Set the alpha test status for any passes.
	void setAlphaTest(Bool state);
	//! Enable the alpha test for any passes.
	inline void enableAlphaTest() { setAlphaTest(True); }
	//! Disable the alpha test for any passes..
	inline void disableAlphaTest() { setAlphaTest(False); }

	//! Set the alpha test function mode and ref value for any passes.
	void setAlphaTestFunc(Comparison func, Float ref);

	//-----------------------------------------------------------------------------------
	// Level Of Detail (LOD)
	//-----------------------------------------------------------------------------------

	//! Set the LOD strategy.
	void setLodStrategy(LodStrategy *lodStrategy);

	//! Get the LOD strategy.
	const LodStrategy* getLodStrategy() const { return m_lodStrategy; }

	//! Set the LOD levels list with distances. LOD strategy must be previously defined.
	//! @param lodList The first level of detail start with index 1.
	void setLodLevels(const std::vector<Float> &lodList);

	//! Get the LOD levels list with distances.
	//! @return The first level of detail start with index 1.
	std::vector<Float> getLodLevels() const;

	//! Get the number of LOD levels.
	inline UInt32 getNumLodLevels() const
	{
		return static_cast<UInt32>(m_lodList.size());
	}

	//-----------------------------------------------------------------------------------
	// Manage techniques
	//-----------------------------------------------------------------------------------

	//! Define the number of technique to create.
	//! If the number of techniques is lesser than current, higher techniques to
	//! the new size are deleted.
	void setNumTechniques(UInt32 numTechniques);

	//! Get the number of techniques.
	inline UInt32 getNumTechniques() const
	{
		return static_cast<UInt32>(m_techniques.size());
	}

	//! Get the current technique for a specific index (read only).
	const MaterialTechnique& getTechnique(UInt32 index) const;
	//! Get the current technique for a specific index.
	MaterialTechnique& getTechnique(UInt32 index);

	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	//! Determine the techniques to use and compile them.
	//! @param shadable Shadable object that use this profile.
	void prepareAndCompile(Shadable &shadable);

    //! Release any prepared and compiled material (inverse of prepareAndCompile).
    void clear();

	//! Get the best material technique.
	MaterialTechnique* getBestTechnique(Float squaredDistance);

	//! Get the LOD index for a specific distance (squared distance value).
	//! Only works if the LOD strategy is defined (@see SetLodStrategy).
	UInt32 getLodIndex(Float squaredDistance) const;

	//! Process the material profile to a shadable object.
	//! @param shadable Shadable object to render.
	//! @param shadowable Shadowable object to render, optional, only if shadow is needed.
	//! @param pickable Pickable object to render, optional, only if picking is needed.
	//! @param drawInfo Draw information given to the material.
	void processMaterial(
			Shadable &shadable,
			Shadowable *shadowable,
			Pickable *pickable,
			const DrawInfo &drawInfo);

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	Bool writeToFile(OutStream &os);
	Bool readFromFile(InStream &is);

protected:

	//! Protected copy constructor.
	MaterialProfile(const MaterialProfile &dup);

	Bool m_activity;      //!< Profile activity.

	typedef std::vector<MaterialTechnique*> T_TechniqueVector;
	typedef T_TechniqueVector::iterator IT_TechniqueVector;
	typedef T_TechniqueVector::const_iterator CIT_TechniqueVector;

	T_TechniqueVector m_techniques;   //!< Array of defined techniques.

	//! List of technique to use according to the hardware and LOD.
	T_TechniqueVector m_activeTechniques;

    LodStrategy *m_lodStrategy;   //!< LOD strategy (can be null).

	//! List of distance for each LOD, contained in squared value.
	std::vector<Float> m_lodList;
};

} // namespace o3d

#endif // _O3D_MATERIALPROFILE_H

