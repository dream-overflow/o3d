/**
 * @file materialsetting.h
 * @brief Material setting mostly managed by a material profile.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-01-05
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MATERIALSETTING_H
#define _O3D_MATERIALSETTING_H

#include "o3d/image/color.h"
#include "o3d/core/vector2.h"
#include "o3d/core/smartobject.h"
#include "../texture/texture.h"

namespace o3d {

class Texture2D;
class CubeMapTexture;
class Scene;

/**
 * @brief Material setting mostly managed by a material profile. It contains all parameters
 * to apply a material (technique).
 */
class O3D_API MaterialSetting : NonCopyable<>
{
public:

	static const Int32 MATERIAL_SETTING_VERSION = 1;

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

	//! Map type.
	enum MapType
	{
		AMBIENT_MAP = 0,             //!< Ambient color map (2d map).
		DIFFUSE_MAP,                 //!< Diffuse color map (2d map).
		SPECULAR_MAP,                //!< Specular color map (2d map).
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

	//! Default constructor.
	//! @param parent Parent class, mostly an MaterialProfile.
	//! @note Parent become the user for shared object, like texture map.
	MaterialSetting(BaseObject *parent);

	//! Destructor.
	~MaterialSetting();

	//! Copy operator.
	MaterialSetting& operator= (const MaterialSetting &dup);

	//-----------------------------------------------------------------------------------
	// Global parameters
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

	//! Set emission color.
	inline void setSelfIllumination(const Color &color) { setColor(EMISSION, color); }
	//! Get emission color.
	inline const Color& getSelfIllumination() const { return getColor(EMISSION); }

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

	//! Set a map type texture.
	inline void setMapType(MapType type, Texture *map) { m_mapTypes[type] = map; }
	//! Get a map type texture.
	inline Texture* getMapType(MapType type) const { return m_mapTypes[type].get(); }

	//! Set the ambient map texture.
	inline void setAmbientMap(Texture2D *map) { setMapType(AMBIENT_MAP, map); }
	//! Get the ambient map texture.
	inline Texture2D* getAmbientMap() const { return reinterpret_cast<Texture2D*>(getMapType(AMBIENT_MAP)); }

	//! Set the diffuse map texture.
	inline void setDiffuseMap(Texture2D *map) { setMapType(DIFFUSE_MAP, map); }
	//! Get the diffuse map texture.
	inline Texture2D* getDiffuseMap() const { return reinterpret_cast<Texture2D*>(getMapType(DIFFUSE_MAP)); }

	//! Set the specular map texture.
	inline void setSpecularMap(Texture2D *map) { setMapType(SPECULAR_MAP, map); }
	//! Get the specular map texture.
	inline Texture2D* getSpecularMap() const { return reinterpret_cast<Texture2D*>(getMapType(SPECULAR_MAP)); }

	//! Set the emission (luminous) map texture.
	inline void setEmissionMap(Texture2D *map) { setMapType(EMISSION_MAP, map); }
	//! Get the emission (luminous) map texture.
	inline Texture2D* getEmissionMap() const { return reinterpret_cast<Texture2D*>(getMapType(EMISSION_MAP)); }

	//! Set the luminous (emission) map texture.
	inline void setLuminousMap(Texture2D *map) { setMapType(EMISSION_MAP, map); }
	//! Get the luminous (emission) map texture.
	inline Texture2D* getLuminousMap() const { return reinterpret_cast<Texture2D*>(getMapType(EMISSION_MAP)); }

	//! Set the normal (bump) map texture.
	inline void setNormalMap(Texture2D *map) { setMapType(NORMAL_MAP, map); }
	//! Get the normal (bump) map texture.
	inline Texture2D* getNormalMap() const { return reinterpret_cast<Texture2D*>(getMapType(NORMAL_MAP)); }

	//! Set the bump (normal) map texture.
	inline void setBumpMap(Texture2D *map) { setMapType(BUMP_MAP, map); }
	//! Get the bump (normal) map texture.
	inline Texture2D* getBumpMap() const { return reinterpret_cast<Texture2D*>(getMapType(BUMP_MAP)); }

	//! Set the height map texture.
	inline void setHeightMap(Texture2D *map) { setMapType(HEIGHT_MAP, map); }
	//! Get the height map texture.
	inline Texture2D* getHeightMap() const { return reinterpret_cast<Texture2D*>(getMapType(HEIGHT_MAP)); }

	//! Set the opacity map texture.
	inline void setOpacityMap(Texture2D *map) { setMapType(OPACITY_MAP, map); }
	//! Get the opacity map texture.
	inline Texture2D* getOpacityMap() const { return reinterpret_cast<Texture2D*>(getMapType(OPACITY_MAP)); }

	//! Set the reflection (reflectivity) map texture.
	inline void setReflectionMap(Texture2D *map) { setMapType(REFLECTION_MAP, map); }
	//! Get the reflection (reflectivity)map texture.
	inline Texture2D* getReflectionMap() const { return reinterpret_cast<Texture2D*>(getMapType(REFLECTION_MAP)); }

	//! Set the cubical environment map texture.
	inline void setCubicalEvnMapMap(Texture2D *map) { setMapType(CUBICAL_ENV_MAP, map); }
	//! Get the cubical environment map texture.
	inline CubeMapTexture* getCubicalEnvMap() const { return reinterpret_cast<CubeMapTexture*>(getMapType(CUBICAL_ENV_MAP)); }

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	//! Write content to file.
	Bool writeToFile(OutStream &os) const;
	//! Read content from file.
    Bool readFromFile(Scene *scene, InStream &is);

private:

	Bool m_activity;   //!< TRUE mean the material must be applied.

	Color m_colors[NUM_COLOR_MODELS];            //!< Color models.

	Float m_floatParams[NUM_FLOAT_PARAMS];      //!< Float parameters.
	Vector2f m_vec2fParams[NUM_VECTOR2F_PARAM];  //!< Vector 2f parameters.

	SmartObject<Texture> m_mapTypes[NUM_MAP_TYPES];  //!< Map types.
};

} // namespace o3d

#endif // _O3D_MATERIALSETTING_H
