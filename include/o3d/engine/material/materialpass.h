/**
 * @file materialpass.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MATERIALPASS_H
#define _O3D_MATERIALPASS_H

#include "o3d/image/color.h"
#include "o3d/core/vector2.h"
#include "material.h"
#include "../blending.h"
#include "../texture/texture2d.h"

namespace o3d {

class CubeMapTexture;
class Context;
class MaterialTechnique;

/**
 * @brief A pass of material technique.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-01-12
 * @see Material for GLSL program naming conventions.
 */
class O3D_API MaterialPass
{
public:

	//! Default constructor.
	MaterialPass(MaterialTechnique *technique);

	//! Destructor.
	~MaterialPass();

	//! Copy operator.
	MaterialPass& operator= (const MaterialPass &dup);

	//! Is the pass supported by the hardware.
	Bool isPassSupported() const;

	//! Build the material pass (load texture, material...).
	//! @param shadable Shadable object that use this material pass.
	void prepareAndCompile(Shadable &shadable);

	//! Clear any loaded data (inverse of PrepareAndCompile).
	void clear();

	//! Set the material for a specified processing mode .
	void setMaterial(Material::InitMode mode, Material *material);

	//! Get the material for a specified processing mode (read only).
	inline const Material* getMaterial(Material::InitMode mode) const
	{
		return m_materials[mode].get();
	}

	//! Get the material a specified processing mode .
	inline Material* getMaterial(Material::InitMode mode)
	{
		return m_materials[mode].get();
	}

	//! Get the owner material technique (read only).
	inline MaterialTechnique* getTechnique() { return m_technique; }
	//! Get the owner material technique.
	inline const MaterialTechnique* getTechnique() const { return m_technique; }

	//-----------------------------------------------------------------------------------
	// Parameters
	//-----------------------------------------------------------------------------------

	static const Int32 MATERIAL_PASS_VERSION = 1;

	//! Color model.
	enum ColorModel
	{
		AMBIENT = 0,   //!< Ambient color or map.
		DIFFUSE,       //!< Diffuse color or map.
		SPECULAR,      //!< Specular color or map.
		EMISSION,      //!< Emission color or map.
		SELF_ILLUMINATION = EMISSION  //! Self illumination/emission color or map.
	};

	//! Number of color models.
	static const Int32 NUM_COLOR_MODELS = EMISSION + 1;

	//! Float parameters.
	enum FloatParams
	{
		SHINE = 0,                  //!< Specular exponent.
		SHININESS = SHINE,          //!< Specular exponent.
		SPECULAR_POWER = SHINE,     //!< Specular exponent.
		SPECULAR_EXPONENT = SHINE,  //!< Specular exponent.
		REFLECTIVITY = 1,           //!< Reflectivity.
		TRANSPARENCY                //!< Transparency.
	};

	//! Number of float parameters.
	static const Int32 NUM_FLOAT_PARAMS = TRANSPARENCY + 1;

	//! Vector2 float parameters.
	enum Vector2fParams
	{
		BUMP_OFFSET = 0            //!< Bump offset (x,y).
	};

	//! Number of float 2d vector parameters.
	static const Int32 NUM_VECTOR2F_PARAM = BUMP_OFFSET + 1;

    /**
     * @brief Map types
     * @note About the shine map (specular exponant), it can be put as A of the specular
     *       map, and using shine parameter or a static constant as shine value factor,
     *       to get from 0..1 to 0..X where X is the factor. More maps mean more textures
     *       fetching.
     */
	enum MapType
	{
		AMBIENT_MAP = 0,             //!< Ambient color map (2d map).
		DIFFUSE_MAP,                 //!< Diffuse color map (2d map).
		SPECULAR_MAP,                //!< Specular color map (2d map).
        SHINE_MAP,                   //!< Specular exponent map (2d map, might be R16F or R32F).
		EMISSION_MAP,                //!< Emission/self illumination color map (2d map).
		SELF_ILLUMINATION_MAP = EMISSION_MAP, //!< Self illumination/emission color map (2d map).
		NORMAL_MAP = 4,              //!< Normal map/bump map (2d map).
		BUMP_MAP = NORMAL_MAP,       //!< Bump map/normal map (2d map).
		HEIGHT_MAP = 5,              //!< Height map (2d map).
		OPACITY_MAP,                 //!< Transparency map (2d map).
		REFLECTION_MAP,              //!< Reflection map (2d map).
		CUBICAL_ENV_MAP              //!< Cube map environment map (cube map).
	};

	//! Number of map types.
	static const Int32 NUM_MAP_TYPES = CUBICAL_ENV_MAP + 1;

	//! Contain a sampler.
	struct Sampler
	{
		SmartObject<Texture> texture;
		Texture::FilteringMode filtering;
		Texture::WrapMode warp;
		Float anisotropy;
		String resourceName;
	};

	//! Fog mode.
	enum FogMode
	{
		FOG_NONE = 0,       //!< No fog.
		FOG_LINEAR = 1,     //!< Linear fog.
		FOG_EXP = 2,        //!< Exponential fog.
		FOG_EXP2 = 3        //!< Exponential square fog.
	};

	//! Number of fog mode.
	static const UInt32 NUM_FOG_MODES = 4;

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
	// Color model
	//-----------------------------------------------------------------------------------

	//! Set a color model.
	inline void setColor(ColorModel model, const Color &color) { m_colors[model] = color; }
	//! Get a color model.
	inline const Color& getColor(ColorModel model) const { return m_colors[model]; }

	//! Set ambient color.
	inline void setAmbient(const Color &color) { setColor(AMBIENT, color); }
	//! Get ambient color.
	inline const Color& getAmbient() const { return getColor(AMBIENT); }

	//! Set diffuse color.
	inline void setDiffuse(const Color &color) { setColor(DIFFUSE, color); }
	//! Get diffuse color.
	inline const Color& getDiffuse() const { return getColor(DIFFUSE); }

	//! Set specular color.
	inline void setSpecular(const Color &color) { setColor(SPECULAR, color); }
	//! Get specular color.
	inline const Color& getSpecular() const { return getColor(SPECULAR); }

	//! Set emission/sel illumination color.
	inline void setEmission(const Color &color) { setColor(EMISSION, color); }
	//! Get emission/sel illumination color.
	inline const Color& getEmission() const { return getColor(EMISSION); }

	//! Set self illumination/emission color.
	inline void setSelfIllumination(const Color &color) { setColor(SELF_ILLUMINATION, color); }
	//! Get self illumination/emission color.
	inline const Color& getSelfIllumination() const { return getColor(SELF_ILLUMINATION); }

	//-----------------------------------------------------------------------------------
	// Float parameters
	//-----------------------------------------------------------------------------------

	//! Set a float parameters value.
	inline void setFloatParam(FloatParams param, Float value) { m_floatParams[param] = value; }
	//! Get a float parameters value.
	inline Float getFloatParam(FloatParams param) const { return m_floatParams[param]; }

	//! Set the specular shininess exponent.
	inline void setShine(Float exponent) { setFloatParam(SHINE, exponent); }
	//! Get the specular shininess exponent.
	inline Float getShine() const { return getFloatParam(SHINE); }

	//! Set the specular shininess exponent.
	inline void setSpecularExponent(Float exponent) { setFloatParam(SHINE, exponent); }
	//! Get the specular shininess exponent.
	inline Float getSpecularExponent() const { return getFloatParam(SHINE); }

	//! Set the reflectivity coefficient.
	inline void setReflectivity(Float reflectivity) { setFloatParam(REFLECTIVITY, reflectivity); }
	//! Get the reflectivity coefficient.
	inline Float getReflectivity() const { return getFloatParam(REFLECTIVITY); }

	//! Set the transparency coefficient.
	inline void setTransparency(Float transparency) { setFloatParam(TRANSPARENCY, transparency); }
	//! Get the transparency coefficient.
	inline Float getTransparency() const { return getFloatParam(TRANSPARENCY); }

	//-----------------------------------------------------------------------------------
	// Vector 2 float parameters
	//-----------------------------------------------------------------------------------

	//! Set a vector 2 float parameters value.
	inline void setVec2fParam(Vector2fParams param, const Vector2f &value) { m_vec2fParams[param] = value; }
	//! Get a vector 2 float parameters value.
	inline const Vector2f& getVec2fParam(Vector2fParams param) const { return m_vec2fParams[param]; }

	//! Set the bump offset.
	inline void setBumpOffset(const Vector2f &offset) { setVec2fParam(BUMP_OFFSET, offset); }
	//! Get the bump offset.
	inline const Vector2f& getBumpOffset() const { return getVec2fParam(BUMP_OFFSET); }

	//-----------------------------------------------------------------------------------
	// Map type
	//-----------------------------------------------------------------------------------

	//! Set a map texture. Need a call to PrepareAndCompile before to be effective.
    //! @map Valid or null texture to define for the specified type.
	void setMap(MapType type, Texture *map);
	//! Get a map texture.
	inline Texture* getMap(MapType type) const { return m_mapTypes[type].texture.get(); }

	//! Set a map resource name. Need a call to PrepareAndCompile before to be effective.
	//! @param name Resource name that will loaded using the texture manager.
	void setMapResourceName(MapType type, const String &name);
	//! Get a map resource name.
	inline const String& getMapResourceName(MapType type) const { return m_mapTypes[type].resourceName; }

	//! Set for a map type the filtering mode.
	inline void setMapFiltering(MapType type, Texture::FilteringMode filtering) { m_mapTypes[type].filtering = filtering; }
	//! Get for a map type the filtering mode.
	inline Texture::FilteringMode getMapFiltering(MapType type) const { return m_mapTypes[type].filtering; }

	//! Set for a map type the warp mode.
	inline void setMapWarp(MapType type, Texture::WrapMode warp) { m_mapTypes[type].warp = warp; }
	//! Get for a map type the warp mode..
	inline Texture::WrapMode getMapWarp(MapType type) const { return m_mapTypes[type].warp; }

	//! Set for a map type the anisotropy filtering level.
	inline void setMapAnisotropy(MapType type, Float anisotropy) { m_mapTypes[type].anisotropy = anisotropy; }
	//! Get for a map type the anisotropy filtering level.
	inline Float getMapAnisotropy(MapType type) const { return m_mapTypes[type].anisotropy; }

	//! Set the ambient map texture.
	inline void setAmbientMap(Texture2D *map) { setMap(AMBIENT_MAP, map); }
	//! Get the ambient map texture.
	inline Texture2D* getAmbientMap() const { return reinterpret_cast<Texture2D*>(getMap(AMBIENT_MAP)); }

	//! Set the diffuse map texture.
	inline void setDiffuseMap(Texture2D *map) { setMap(DIFFUSE_MAP, map); }
	//! Get the diffuse map texture.
	inline Texture2D* getDiffuseMap() const { return reinterpret_cast<Texture2D*>(getMap(DIFFUSE_MAP)); }

	//! Set the specular map texture.
	inline void setSpecularMap(Texture2D *map) { setMap(SPECULAR_MAP, map); }
	//! Get the specular map texture.
	inline Texture2D* getSpecularMap() const { return reinterpret_cast<Texture2D*>(getMap(SPECULAR_MAP)); }

    //! Set the specular exponent map texture.
    inline void setShineMap(Texture2D *map) { setMap(SHINE_MAP, map); }
    //! Get the specular exponent map texture.
    inline Texture2D* getShineMap() const { return reinterpret_cast<Texture2D*>(getMap(SHINE_MAP)); }

	//! Set the emission (luminous) map texture.
	inline void setEmissionMap(Texture2D *map) { setMap(EMISSION_MAP, map); }
	//! Get the emission (luminous) map texture.
	inline Texture2D* getEmissionMap() const { return reinterpret_cast<Texture2D*>(getMap(EMISSION_MAP)); }

	//! Set the luminous (emission) map texture.
	inline void setLuminousMap(Texture2D *map) { setMap(EMISSION_MAP, map); }
	//! Get the luminous (emission) map texture.
	inline Texture2D* getLuminousMap() const { return reinterpret_cast<Texture2D*>(getMap(EMISSION_MAP)); }

	//! Set the normal (bump) map texture.
	inline void setNormalMap(Texture2D *map) { setMap(NORMAL_MAP, map); }
	//! Get the normal (bump) map texture.
	inline Texture2D* getNormalMap() const { return reinterpret_cast<Texture2D*>(getMap(NORMAL_MAP)); }

	//! Set the bump (normal) map texture.
	inline void setBumpMap(Texture2D *map) { setMap(BUMP_MAP, map); }
	//! Get the bump (normal) map texture.
	inline Texture2D* getBumpMap() const { return reinterpret_cast<Texture2D*>(getMap(BUMP_MAP)); }

	//! Set the height map texture.
	inline void setHeightMap(Texture2D *map) { setMap(HEIGHT_MAP, map); }
	//! Get the height map texture.
	inline Texture2D* getHeightMap() const { return reinterpret_cast<Texture2D*>(getMap(HEIGHT_MAP)); }

	//! Set the opacity map texture.
	inline void setOpacityMap(Texture2D *map) { setMap(OPACITY_MAP, map); }
	//! Get the opacity map texture.
	inline Texture2D* getOpacityMap() const { return reinterpret_cast<Texture2D*>(getMap(OPACITY_MAP)); }

	//! Set the reflection (reflectivity) map texture.
	inline void setReflectionMap(Texture2D *map) { setMap(REFLECTION_MAP, map); }
	//! Get the reflection (reflectivity)map texture.
	inline Texture2D* getReflectionMap() const { return reinterpret_cast<Texture2D*>(getMap(REFLECTION_MAP)); }

	//! Set the cubical environment map texture.
    inline void setCubicalEnvMap(Texture2D *map) { setMap(CUBICAL_ENV_MAP, map); }
	//! Get the cubical environment map texture.
	inline CubeMapTexture* getCubicalEnvMap() const { return reinterpret_cast<CubeMapTexture*>(getMap(CUBICAL_ENV_MAP)); }

	//-----------------------------------------------------------------------------------
	// Context
	//-----------------------------------------------------------------------------------

	//! Set the culling mode.
	inline void setCullingMode(CullingMode mode) { m_cullingMode = mode; }
	//! Get the culling mode.
	inline CullingMode getCullingMode() const { return m_cullingMode; }

	//! Set the blending mode.
    inline void setBlendingFunc(Blending::FuncProfile func) { m_blendingFunc = func; }
	//! Get the blending mode.
    inline Blending::FuncProfile getBlendingFunc() const { return m_blendingFunc; }

	//! Set if faces are sorted and displayed with the alpha-pipeline.
	inline void setSorted(Bool state) { m_isSorted = state; }
	//! Get the state of the face sorting (using the alpha-pipeline).
	inline Bool getSorted() const { return needsSorting(); }
	//! Is face sorting is asked by this shader.
	inline Bool needsSorting() const { return m_isSorted; }

	//! Set double side drawing mode status.
	inline void setDoubleSide(Bool state) { m_doubleSide = state; }
	//! Get double side drawing mode status.
	inline Bool getDoubleSide() const { return m_doubleSide; }
	//! Enable the double side drawing mode.
	inline void enableDoubleSide() { m_doubleSide = True; }
	//! Disable the double side drawing mode.
	inline void disableDoubleSide() { m_doubleSide = False; }

	//! Set the depth buffer test status.
	inline void setDepthTest(Bool state) { m_depthTest = state; }
	//! Get the depth buffer test status.
	inline Bool getDepthTest()const { return m_depthTest; }
	//! Enable the depth buffer test.
	inline void enableDepthTest() { m_depthTest = True; }
	//! Disable the depth buffer test.
	inline void disableDepthTest() { m_depthTest = False; }

	//! Set the depth buffer write status.
	inline void setDepthWrite(Bool state) { m_depthWrite = state; }
	//! Get the depth buffer write status.
	inline Bool getDepthWrite() const { return m_depthWrite; }
	//! Enable the depth buffer write.
	inline void enableDepthWrite() { m_depthWrite = True; }
	//! Disable the depth buffer writer.
	inline void disableDepthWrite() { m_depthWrite = False; }

	//! Set the depth range to normal (false) or infinite (true).
	inline void setInfiniteDepthRange(Bool state) { m_infiniteDepthRange = state; }
	//! Get the depth range mode.
	inline Bool getInfiniteDepthRange() const { return m_infiniteDepthRange; }
	//! Set to infinite depth range.
	inline void enableInfiniteDepthRange() { m_infiniteDepthRange = True; }
	//! Set to normal depth range.
	inline void disableInfiniteDepthRange() { m_infiniteDepthRange = False; }

	//! Set the alpha test status.
	inline void setAlphaTest(Bool state) { m_alphaTest = state; }
	//! Get the alpha test status.
	inline Bool getAlphaTest() const { return m_alphaTest; }
	//! Enable the alpha test.
	inline void enableAlphaTest() { m_alphaTest = True; }
	//! Disable the alpha test.
	inline void disableAlphaTest() { m_alphaTest = False; }

	//! Set the alpha test function mode and ref value.
	inline void setAlphaTestFunc(Comparison func, Float ref)
	{
		m_alphaFunc = func;
		m_alphaRef = ref;
	}
	//! Get the alpha test function mode.
	inline Comparison getAlphaTestFunc() const { return m_alphaFunc; }
	//! Get the alpha test reference value.
	inline Float getAlphaTestFuncRef() const { return m_alphaRef; }

	//-----------------------------------------------------------------------------------
	// Process
	//-----------------------------------------------------------------------------------

	//! Process the material pass to a shadable object.
	//! @param shadable Shadable object to render.
	//! @param shadowable Shadowable object to render, optional, only if shadow is needed.
	//! @param pickable Pickable object to render, optional, only if picking is needed.
	//! @param drawInfo Draw information given to the material.
	void processMaterial(
			Shadable &shadable,
			Shadowable *shadowable,
			Pickable *pickable,
			const DrawInfo &drawInfo);

	//! Establish the filtering mode, warp mode, and anisotropy filtering to a specific map.
	void assignMapSetting(MapType type) const;

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	Bool writeToFile(OutStream &os) const;
	Bool readFromFile(InStream &is);

protected:

	//! Restricted copy constructor.
	MaterialPass(const MaterialPass &dup);

	MaterialTechnique *m_technique;      //!< Owner.
	SmartObject<Material> m_materials[Material::NUM_INIT_MODES]; //!< Materials.

	Bool m_activity;   //!< TRUE mean the pass must be applied.

	Color m_colors[NUM_COLOR_MODELS];            //!< Color models.

    Float m_floatParams[NUM_FLOAT_PARAMS];       //!< Float parameters.
    Vector2f m_vec2fParams[NUM_VECTOR2F_PARAM];  //!< Vector 2f parameters.

	Sampler m_mapTypes[NUM_MAP_TYPES];  //!< Map types.

    Context *m_glContext;         //!< Related OpenGL context.

    CullingMode m_cullingMode;	  //!< Face culling mode.

    Bool m_isSorted;			  //!< Face sorting (use alpha pipeline).
    Blending::FuncProfile m_blendingFunc;  //!< Blending function.

	Bool m_doubleSide;          //!< Draw the 2 sides.
	Bool m_depthTest;           //!< Enable depth test.
	Bool m_depthWrite;          //!< Enable depth write.
	Bool m_infiniteDepthRange;  //!< Enable infinite depth range.

	Bool m_alphaTest;        //!< Enable alpha test.
    Comparison m_alphaFunc;  //!< Alpha function if enable.
	Float m_alphaRef;        //!< Alpha ref value if enable.

	//! Load a 2D map to a specific type.
	void load2DMap(MapType type);

	//! Load a cube map to a specific type.
	void loadCubeMap(MapType type);

	//! Load valid textures maps.
	void loadMaps();
};

} // namespace o3d

#endif // _O3D_MATERIALPASS_H
