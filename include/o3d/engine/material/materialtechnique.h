/**
 * @file materialtechnique.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MATERIALTECHNIQUE_H
#define _O3D_MATERIALTECHNIQUE_H

#include "materialpass.h"

namespace o3d {

class MaterialProfile;

/**
 * @brief Part of a MaterialProfile.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-01-12
 */
class O3D_API MaterialTechnique
{
public:

	//! Constructor.
	//! @param profile Material profile owner.
	MaterialTechnique(MaterialProfile *profile);

	//! Destructor.
	~MaterialTechnique();

	//! Copy operator.
	MaterialTechnique& operator= (const MaterialTechnique &dup);

	//! Get the material profile (it inherit from SceneEntity).
	MaterialProfile* getProfile() { return m_profile; }

	//! Get the material profile (it inherit from SceneEntity) (read only).
	const MaterialProfile* getProfile() const { return m_profile; }

	//-----------------------------------------------------------------------------------
	// Creation
	//-----------------------------------------------------------------------------------

	//! Is the technique supported by the hardware.
	Bool isTechniqueSupported() const;

	//! Build any pass and material of the technique. Throw an exception if
	//! the technique is not supported. Texture are loaded at this time (the first time).
	//! @param shadable Shadable object that use thie technique.
	void prepareAndCompile(Shadable &shadable);

    //! Clear any loaded pass data (inverse of prepareAndCompile).
	void clear();

	//-----------------------------------------------------------------------------------
	// Manage material (techniques)
	//-----------------------------------------------------------------------------------

	//! Define the number of pass.
	void setNumPass(UInt32 numPass);

	//! Get the number of pass.
	inline UInt32 getNumPass() const
	{
		return static_cast<UInt32>(m_pass.size());
	}

	//! Get the pass at a specific index (read only).
	const MaterialPass& getPass(UInt32 index) const;
	//! Get the pass at a specific index.
	MaterialPass& getPass(UInt32 index);

	//-----------------------------------------------------------------------------------
	// Manage material pass parameters. Manage any Passes.
	//-----------------------------------------------------------------------------------

	//! Set ambient color for any passes.
	void setAmbient(const Color &color);

	//! Set diffuse color for any passes.
	void setDiffuse(const Color &color);

	//! Set specular color for any passes.
	void setSpecular(const Color &color);

	//! Set self illumination color for any passes.
	void setSelfIllumination(const Color &color);

	//! Set the specular shininess exponent for any passes.
	void setShine(Float exponent);

	//! Set the specular shininess exponent for any passes.
	void setSpecularExponent(Float exponent);

	//! Set the reflectivity coefficient for any passes.
	void setReflectivity(Float reflectivity);

	//! Set the transparency coefficient for any passes.
	void setTransparency(Float transparency);

	//! Set the bump offset for any passes.
	void setBumpOffset(const Vector2f &offset);

	//-----------------------------------------------------------------------------------
	// Manager material pass context parameters. Manage any Passes.
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
	// Manage LOD index
	//-----------------------------------------------------------------------------------

	//! Set the LOD level index.
	inline void setLodIndex(UInt32 index) { m_lodIndex = index; }
	//! Get the LOD level index. Default is 0.
	inline UInt32 getLodIndex() const { return m_lodIndex; }

	//-----------------------------------------------------------------------------------
	// Process
	//-----------------------------------------------------------------------------------

	//! Process the material technique to a shadable object.
	//! @param shadable Shadable object to render.
	//! @param shadowable Shadowable object to render, optional, only if shadow is needed.
	//! @param pickable Pickable object to render, optional, only if picking is needed.
	//! @param drawInfo Draw information given to the material.
	void processMaterial(
			Shadable &shadable,
			Shadowable *shadowable,
			Pickable *pickable,
			const DrawInfo &drawInfo);

	//! Process the material technique to a shadable object only for sorted material pass.
	//! @param object Shadable object to sort his faces.
	//! @param materialPass Material pass object that contain parameters.
	//! @param drawInfo Draw information given to the material.
	void processAlphaPipeline(
			Shadable &shadable,
			const DrawInfo &drawInfo);

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	Bool writeToFile(OutStream &os) const;
	Bool readFromFile(InStream &is);

protected:

	MaterialProfile *m_profile;  //!< Parent profile.
    UInt32 m_lodIndex;           //!< LOD index, default is 0.

    Bool m_valid;                //!< TRUE mean the technique can be used.

	typedef std::vector<MaterialPass*> T_PassVector;
	typedef T_PassVector::iterator IT_MaterialVector;
	typedef T_PassVector::const_iterator CIT_PassVector;

	T_PassVector m_pass;   //!< Array of defined pass.

	//! Process the face sorting using the alpha pipeline.
	//! @param object Shadable object to sort his faces.
	//! @note It use the current modelview matrix of the GLContext has transformation matrix.
	void sortFaces(Shadable &object);
};

} // namespace o3d

#endif // _O3D_MATERIALTECHNIQUE_H
