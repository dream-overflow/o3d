/**
 * @file light.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_LIGHT_H
#define _O3D_LIGHT_H

#include "../scene/sceneobject.h"
#include "o3d/image/color.h"
#include "o3d/core/smartpointer.h"
#include "o3d/core/memorydbg.h"
#include "o3d/core/vector3.h"
#include "o3d/engine/object/primitive.h"

namespace o3d {

class BCone;

/**
 * @brief The Light class
 * @date 2002-09-16
 * @author Frederic SCHERMA, Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * A scene light object which can be a 3 kind of type :
 *  - point light : position, attenuation
 *  - spot light : position, direction, attenuation, cutoff
 *  - directional light : direction
 *  - light map : light map (TODO LightMap : public Light)
 * A light that project shadow must enableShadowCast().
 */
class O3D_API Light : public SceneObject
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(Light)

	//! Type of the light.
	enum LightType
	{
		POINT_LIGHT = 0,     //!< Positional point light (all directions).
		SPOT_LIGHT,          //!< Positional and directional spot light with cutoff.
        DIRECTIONAL_LIGHT,   //!< Directional infinite light.
        LIGHT_MAP            //!< Light map.
	};

    /**
     * @brief Light The light is initialized as PointLight.
     * With {0.7,0.7,0.7,1.0} as ambient, diffuse and specular colors,
     * with {1.0,0.0,0.0} as attenuation,
     * 90.0 degree of cutoff,
     * and intensity exponent of 0.0.
     * @param parent Parent object.
     * @param lightType Light type.
     */
	Light(BaseObject *parent, LightType lightType = POINT_LIGHT);

	//! Copy constructor.
	Light(const Light &dup);

	//! Destructor.
	virtual ~Light();

	//! Define the light type (initial set is PointLight).
	inline void setLightType(LightType lightType) { m_lightType = lightType; }

	//! Get the type of the light.
	inline LightType getLightType() const { return m_lightType; }

	//! Get the drawing type according to the light type.
	virtual UInt32 getDrawType() const;

	//-----------------------------------------------------------------------------------
	// Color model
	//-----------------------------------------------------------------------------------

	//! Set the ambient color.
	inline void setAmbient(const Color& col) { m_ambient = col; }
	//! Set the diffuse color.
	inline void setDiffuse(const Color& col) { m_diffuse = col; }
	//! Set the specular color.
	inline void setSpecular(const Color& col) { m_specular = col; }

	//! Set the ambient color from 4 float.
	inline void setAmbient(Float r, Float g, Float b, Float a = 1.f)
	{
		m_ambient.set(r, g, b, a);
	}

	//! Set the diffuse color from 4 float.
	inline void setDiffuse(Float r, Float g, Float b, Float a = 1.f)
	{
		m_diffuse.set(r, g, b, a);
	}

	//! Set the specular color from 4 float.
	inline void setSpecular(Float r, Float g, Float b, Float a = 1.f)
	{
		m_specular.set(r,g,b,a);
	}

	//! Set a component of the ambient color.
	inline void setAmbient(UInt32 c, Float val) { m_ambient.set(c, val); }

	//! Set a component of the diffuse color.
	inline void setDiffuse(UInt32 c, Float val) { m_diffuse.set(c, val); }

	//! Set a component of the specular color.
	inline void setSpecular(UInt32 c,Float val) { m_specular.set(c, val); }

	//! Get the ambient color.
	inline const Color& getAmbient() const { return m_ambient; }
	//! Get the diffuse color.
	inline const Color& getDiffuse() const { return m_diffuse; }
	//! Get the specular color.
	inline const Color& getSpecular() const { return m_specular; }

	//-----------------------------------------------------------------------------------
	// Attenuation parameters
	//-----------------------------------------------------------------------------------

	//! Set the cutoff attenuation exponent.
	void setExponent(Float exponent) { m_exponent = exponent; }
	//! Get the cutoff attenuation exponent.
	inline Float getExponent() const { return m_exponent; }

	//! Set the constant light attenuation.
	void setConstantAttenuation(Float constant);
	//! Set the linear light attenuation.
	void setLinearAttenuation(Float linear);
	//! Set the quadratic light attenuation.
	void setQuadraticAttenuation(Float quadratic);

	//! Get the constant light attenuation.
	inline Float getConstantAttenuation() const { return m_attenuation[0]; }
	//! Get the linear light attenuation.
	inline Float getLinearAttenuation() const { return m_attenuation[1]; }
	//! Get the quadratic light attenuation.
	inline Float getQuadraticAttenuation() const { return m_attenuation[2]; }

	//! Set the light attenuation from 3 float.
	void setAttenuation(Float constant, Float linear, Float quadratic);

	//! Set the light attenuation from a 3 float vector.
	void setAttenuation(const Vector3 att);

	//! Get the light attenuation into a 3 float vector.
	inline const Vector3& getAttenuation() const { return m_attenuation; }

	//! Set the attenuation threshold
	void setAttenuationThreshold(Float _value);

	//! Get the attenuation threshold
	inline Float getAttenuationThreshold() const { return m_attenuationThreshold; }

    //-----------------------------------------------------------------------------------
	// CutOff parameter
	//-----------------------------------------------------------------------------------

	//! Set the light cutoff in degree [0..90]. 180 mean omnidirectional like a point light.
	void setCutOff(Float cutoff);

	//! Get the light cutoff in degree [0..90]. 180 mean omnidirectional like a point light.
	inline Float getCutOff() const { return m_cutOff; }

    //-----------------------------------------------------------------------------------
	// Drawable
	//-----------------------------------------------------------------------------------

	//! Draw according to the light type
	virtual void draw(const DrawInfo &drawInfo);

	//! Check only with its parent node position.
	virtual Geometry::Clipping checkBounding(const AABBox &bbox) const;

	//! Check only with its parent node position.
	virtual Geometry::Clipping checkBounding(const Plane &plane) const;

	//! Check only with its parent node position.
	virtual Geometry::Clipping checkBounding(const BSphere &sphere) const;

    //! Check how the light clip with the frustum.
	virtual Geometry::Clipping checkFrustum(const Frustum &frustum) const;

    //-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	//! Nothing to update.
	virtual void update();

	//! Get the light position into the eye space using the current active camera.
	//! w=1 for point and spot light, otherwise 0 for infinite directional source.
	Vector4 getPosition() const;

	//! Get the light direction into the eye space using the current active camera.
	Vector3 getDirection() const;

	//! Get the light position into world space.
	//! w=1 for point and spot light, otherwise 0 for infinite directional source.
	Vector4 getWorldPosition() const;

	//! Get the light direction into world space.
	Vector3 getWorldDirection() const;

    /**
     * Get the length of the ligth influence (radius for point light, length of the cone
     * for spot light, max float for directionnal ligth, and manualy defined for light
     * map).
     */
    inline Float getLength() const { return getThresholdDistance(); }

    /**
     * Get the light radius (radius of the cone or of the sphere).
     * For point light this value is the same as for getLength().
     */
    Float getRadius() const;


	// Serialization
	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

protected:

	LightType m_lightType;    //!< Type of the light.

    Float m_exponent;         //!< Intensity exponent.
    Vector3 m_attenuation;    //!< Quadratic attenuation (constant, linear, quadratic).
	Float m_attenuationThreshold;	//!< Threshold used to determine the region of influence

    Float m_cutOff;         //!< Spot light cutoff in degree [0..90] or 180 (omni). Correspond to cone inner angle.

    Color m_ambient;        //!< Ambient color.
    Color m_diffuse;        //!< Diffuse color.
    Color m_specular;       //!< Specular color.

    BCone *m_pConeBounding;	//!< Used for spot light

	mutable Float m_thresholdDistance;	//!< Avoid to perform again and again the same costly computation

	void drawPointLight(const DrawInfo &drawInfo);
	void drawSpotLight(const DrawInfo &drawInfo);
	void drawDirectLight(const DrawInfo &drawInfo);

	//! Returns x so that attenuation(x) == _attenuationThreshold,   0.0f for infinity
	Float getThresholdDistance() const;
};

} // namespace o3d

#endif // _O3D_LIGHT_H
