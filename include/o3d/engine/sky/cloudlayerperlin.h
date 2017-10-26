/**
 * @file cloudlayerperlin.h
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2009-11-16
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CLOUDLAYERPERLIN_H
#define _O3D_CLOUDLAYERPERLIN_H

#include "o3d/core/smartobject.h"

#include "o3d/engine/object/facearray.h"
#include "o3d/engine/sky/cloudlayerbase.h"
#include "o3d/engine/object/dome.h"
#include "o3d/engine/shader/shader.h"
#include "o3d/engine/texture/texture2d.h"

#include "o3d/image/perlinnoise2d.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class CloudLayerPerlin
//-------------------------------------------------------------------------------------
//! Implementation of O3DCloudLayerBase
//!
//! It consists in a simple way to draw clouds in a virtual world. Perlin noise is used:
//! http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
//!
//! The version presented here is available in two similar versions, except that the second
//! one handles night/day cycles. Occlusion of clouds is indeed computed based on the
//! planet radius and the sun direction provided by the user.
//!
//! Note that except the variable <time>, all parameters are constant. It means that if
//! the sun position changes, you must update the cloud layer manually. It could by fix later.
//---------------------------------------------------------------------------------------
class O3D_API CloudLayerPerlin : public CloudLayerBase
{
private:

	enum UpdateField
	{
		UPDATE_PERLIN_NORMAL = 1,
		UPDATE_NOISE = 2,
		UPDATE_SHADER = 4
	};

	enum ParamLocation
	{
		A_VEC4_VERTEX = 0,
		A_VEC2_TEXCOORDS = 1,

		U_MAT4_MODELVIEW = 10,
		U_TEX_CLOUD = 11,
		U_TEX_NOISE = 12,
		U_TEX_NORMAL = 13,
		U_VEC4_NOISE_PARAMS = 14,
		U_VEC4_INTENSITY_PARAMS = 15,
		U_VEC4_COLOR_PARAMS = 16,
		U_VEC4_LIGHT_PARAMS = 17,
		U_VEC3_LIGHT_DIRECTION = 18,
		U_VEC3_CLOUD_COLOR = 19,
		U_VEC2_VELOCITY = 20,
		U_FLOAT_INVSCALE = 21,
		U_FLOAT_TIME = 22,
		U_FLOAT_PLANET_RADIUS = 23,
		U_FLOAT_LAYER_ALTITUDE = 24,

		PARAM_ENUM_SIZE = 25
	};

public:

	enum CloudFlag
	{
		CLOUD_NOISE = 1,
		CLOUD_SHADING = 2,
		CLOUD_SHADOWING = 4,
		CLOUD_OCCLUSION = 8
	};

	//! Default constructor
	CloudLayerPerlin(BaseObject * _pParent, UInt32 _flags = CLOUD_NOISE | CLOUD_SHADING | CLOUD_SHADOWING);
	//! The destructor
	virtual ~CloudLayerPerlin();

	//! Called when the sky must be drawn
	virtual void draw(const DrawInfo &drawInfo);

	//! On time change
	virtual void setTime(Double);

	//! Transfer function
	virtual Bool project(const Vector3 & _dir, SmartArrayFloat & _target);

	//! Returns the object's state
	//! If it is not a valid object, it won't be rendered
	Bool isValid() const;

	//! Changes the scale
	void setDome(const Dome & _dome);
	const Dome & getDome() const { return m_dome; }

	//! Defines the Perlin noise used to generate clouds
	void setPerlin(const PerlinNoise2d & _noise);
	const PerlinNoise2d & getPerlin() const { return m_perlin; }

	void setNoise(const PerlinNoise2d & _noise);
	const PerlinNoise2d & getNoise() const { return m_noise; }

	void setAverageSize(Int32 _size);
	Int32 getAverageSize() const { return m_averageSize; }

	void setScale(Float _scale);
	Float getScale() const { return m_scale; }

	void setCoveringRate(Float _covering);
	Float getCoveringRate() const { return m_coveringRate; }

	void setContrast(Float _contrast);
	Float getContrast() const { return m_contrast; }

	//! Enable shading
	void enable(CloudFlag _flag, Bool _enable = True);
	Bool isEnabled(CloudFlag _flag) const { return (m_flags & _flag) != 0; }

	//! Set all flags
	void setFlags(UInt32 _flags);
	UInt32 getFlags() const { return m_flags; }

	//! Cloud color
	void setCloudColor(const Vector3 & _color) { m_cloudColor = _color; }
	const Vector3 & getCloudColor() const { return m_cloudColor; }

	//! Light direction to light source
	void setLightDirection(const Vector3 & _dir) { m_lightDir = _dir; }
	const Vector3 & getLightDirection() const { return m_lightDir; }

	//! Noise parameters
	void setNoiseParameters(const Vector4 & _params) { m_noiseParameters = _params; }
	const Vector4 & getNoiseParameters() const { return m_noiseParameters; }

	//! Intensity parameters
	void setIntensityParameters(const Vector4 & _params) { m_intensityParameters = _params; }
	const Vector4 & getIntensityParameters() const { return m_intensityParameters; }

	//! Color parameters
	void setColorParameters(const Vector4 & _params) { m_colorParameters = _params; }
	const Vector4 & getColorParameters() const { return m_colorParameters; }

	//! Light parameters
	void setLightParameters(const Vector4 & _params) { m_lightParameters = _params; }
	const Vector4 & getLightParameters() const { return m_lightParameters; }

	//! Set the cloud velocity
	void setVelocity(const Vector2f & _speed) { m_cloudLayerSpeed = _speed; }
	const Vector2f & getVelocity() const { return m_cloudLayerSpeed; }


protected:

	Bool updateGeometry();

private:

	Dome m_dome;
	PerlinNoise2d m_perlin;
	PerlinNoise2d m_noise;

	UInt32 m_flags;

	Float m_scale;			//!< Size in percentage. (default 1.0: the size of the dome (diameter) is identical to the texture)
	Float m_time;

	Float m_coveringRate;
	Float m_contrast;
	Int32 m_averageSize;

	Vector3 m_lightDir;
	Vector3 m_cloudColor;

	Vector4 m_noiseParameters;
	Vector4 m_intensityParameters;
	Vector4 m_colorParameters;
	Vector4 m_lightParameters;

	Vector2f m_cloudLayerSpeed;

	Int32 m_paramLocations[PARAM_ENUM_SIZE];

	UInt32 m_upToDate;

	ShaderInstance m_shader;
	SmartObject<Texture2D> m_perlinTexture;
	SmartObject<Texture2D> m_normalTexture;
	SmartObject<Texture2D> m_noiseTexture;

	FaceArrayUInt32 m_indices;
	VertexBufferObjf m_vertices;
	VertexBufferObjf m_texCoords;
};

} // namespace o3d

#endif // _O3D_CLOUDLAYERPERLIN_H

