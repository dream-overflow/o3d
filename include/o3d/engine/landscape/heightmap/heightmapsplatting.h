/**
 * @file heightmapsplatting.h
 * @brief Generic terrain object.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2010-01-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef __O3DHEIGHTMAPSPLATTING_H
#define __O3DHEIGHTMAPSPLATTING_H

#include "o3d/engine/landscape/landscape.h"
#include "o3d/engine/object/vertexelement.h"
#include "o3d/engine/shader/shader.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/object/facearray.h"
#include "o3d/engine/framebuffer.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class HeightmapSplatting
//-------------------------------------------------------------------------------------
//! Generic terrain object.
//---------------------------------------------------------------------------------------
class O3D_API HeightmapSplatting : public TerrainBase
{
private:

	enum UpdateFlag
	{
		UPDATE_SHADER = 1
	};

public:

	O3D_DECLARE_ABSTRACT_CLASS(HeightmapSplatting)

	enum OptionFlag
	{
		OPT_UNDEFINED			= 0,
		OPT_STATIC_LIGHTNING	= 1,	//!< (default) Uses light and shadowmap instead of using dynamic light
		OPT_NOISE				= 2
	};

	//! Default constructor.
	HeightmapSplatting(BaseObject *pParent, Camera *pCamera, UInt32 _flags = OPT_NOISE | OPT_STATIC_LIGHTNING);

	//! Virtual destructor.
	virtual ~HeightmapSplatting();

	//! Returns the object state.
	//! The this function returns True, the heightmap will be rendered.
	Bool isValid() const;

	//! Loads a heightmap from a file (*.o3dhmp)
    void load(InStream & _is);

	//! Enable/Disable flags
	void enable(OptionFlag _flag, Bool _state = True);
	Bool isEnabled(OptionFlag _flag) const { return (m_flags & _flag) != 0; }

	//! Defines the bitfield flag
	void setFlag(UInt32 _flag);
	//! Returns the bitfield flag
	UInt32 getFlag() const { return m_flags; }

	//! Defines the units along each axis
	//! @param _units contains the units on each axis: x,y,z with xz the horizontal plane
	void setUnits(const Vector3 & _units) { m_units = _units; }
	const Vector3 & getUnits() const { return m_units; }

	//! Defines the elevation source
	//! If no heightmap is specified, nothing is rendered. IsValid() returns False.
	void setHeightmap(const Image & _pic, Float _offset);

	//! Defines the normalmap
	//! Take care specifying the same units that were used to generate the normalmap.
	//! The normalmap is not taken into account if the lightmap is defined.
	void setNormalmap(const Image & _pic);

	//! Defines the colormap
	void setColormap(const Image & _pic);

	//! Defines the lightmap
	//! If "lightning" is disabled, this command raises an exception.
	//! If no lightmap is specified and "lightning" is enabled, the heightmap is illuminated using
	//! the lights added to this terrain object.
	//! If a lightmap is defined are lightning is enabled, all lights added to this object are
	//! silently ignored.
	//! @param _pic is a valid picture. If not, the existing lightmap is destroyed.
	void setLightmap(const Image & _pic);

	//! Defines the shadowmap
	//! If "self shadowing" is disabled, this command raises an exception.
	//! @param _pic is a valid picture. If not, the existing shadowmap is destroyed.
	//!        If no shadowmap is specified, the shadowmap is automatically computed.
	void setShadowmap(const Image & _pic);

	//! Defines the ground noise
	//! If "noise" is disabled, this command raises an exception.
	//! It is common to use a noise texture to improve ground quality.
	//! @param _pic is a reference to a valid picture
	void setNoise(const Image & _pic);

	//! Defines the noise scale
	//! @param _scale is noise repetition frequency. (Default is 1.0f, it means in the world space, the noise
	//!        texture size is the equal to the heightmap unit along the x-axis)
	void setNoiseScale(Float _scale) { m_noiseScale = _scale; }
	Float getNoiseScale() const { return m_noiseScale; }

	//---------------------------------------------------------------------------------------
	// Processing
	//---------------------------------------------------------------------------------------

	//! Add a light
	virtual void addLight(const LightInfos &);
	virtual void removeLight(Light *);
    virtual void updateLight(Light * = nullptr);

	virtual void draw();
	virtual void update();

protected:

	void drawDeferred();
	void drawDeferredStaticLighting();

private:

	typedef std::vector<LightInfos>			T_LightArray;
	typedef T_LightArray::iterator			IT_LightArray;
	typedef T_LightArray::const_iterator	CIT_LightArray;

	FaceArrayUInt32 m_indices;
	VertexElement m_vertices;
	VertexElement m_texCoords;

	VertexBufferObjf m_vbo;

	SmartObject<Texture2D> m_normalTex;
	SmartObject<Texture2D> m_colormapTex;
	SmartObject<Texture2D> m_lightmapTex;
	SmartObject<Texture2D> m_shadowmapTex;
	SmartObject<Texture2D> m_noiseTex;
	SmartObject<Texture2D> m_whiteTex1;		//!< Default texture used if an option is disabled
	SmartObject<Texture2D> m_whiteTex2;		//!< Second default texture used if an option is disabled
	SmartObject<Texture2D> m_whiteTex3;		//!< Third default texture used if an option is disabled

	UInt32 m_flags;
	Vector3 m_units;
	Float m_noiseScale;

	ShaderInstance m_shader;
	ShaderInstance m_blackShader;
	ShaderInstance m_texShader;
	ShaderInstance m_rgTexShader;

	ShaderInstance m_staticLightShader;			//!< Shader used if OPT_STATIC_LIGHTNING is enabled
	ShaderInstance m_deferredColorShader;
//	Int32 m_paramLocations[PARAM_ENUM_SIZE];

	FrameBuffer m_fbo;							//!< Frame buffer object used by deferred rendering
	SmartObject<Texture2D> m_fboDepthTex;
	SmartObject<Texture2D> m_fboColorTex;
	SmartObject<Texture2D> m_fboNormalTex;

	UInt32 m_updateFlag;

	// Lights
	T_LightArray m_lights;
};

} // namespace o3d

#endif // __O3DHEIGHTMAPSPLATTING_H

