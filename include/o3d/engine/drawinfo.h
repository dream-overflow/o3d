/**
 * @file drawinfo.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DRAWINFO_H
#define _O3D_DRAWINFO_H

#include "o3d/image/color.h"
#include "o3d/core/vector3.h"
#include "o3d/core/vector4.h"
#include "o3d/core/matrix4.h"

namespace o3d {

class Light;
class Camera;
class ShaderInstance;

/**
 * @brief Draw information context.
 * @date 2009-10-19
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * Draw information is a context containing current global variable for the current
 * draw pass. It is used by the Drawable::draw method.
 */
class O3D_API DrawInfo
{
public:

	//! Different kind of rendering mode.
	enum PassMode
	{
		SYMBOLIC_PASS,     //!< Symbolic object pass.
		PICKING_PASS,      //!< Picking pass (color or texture).
		AMBIENT_PASS,      //!< First rendering pass, without lighting, sort alpha.
		SHADOW_PASS,       //!< Rendering shadows volumes or map pass.
		LIGHTING_PASS,     //!< Lighting rendering pass with or without stencil.
		DEFERRED_PASS      //!< Deferred diffuse pass rendering pass.
	};

	//! Default constructor.
	DrawInfo() :
		passNum(0),
		pass(AMBIENT_PASS),
        shaderInstance(nullptr)
	{
	}

	//! Initialization constructor and reset pass number to zero.
	DrawInfo(PassMode pass) :
		passNum(0),
		pass(pass),
        shaderInstance(nullptr)
	{
	}

    //! Current light information for forward lighting TODO should we have a list of
    //! lights for forward ligting with multiple light per pass or many single pass on
    //! the same pipeline
	struct LightObject
	{
        UInt32 type;         //!< @see Light::LightTypes.
        UInt32 shadowType;   //!< @see Shadowable::ShadowTechnique.

		Color ambient;       //!< Ambient color.
		Color diffuse;       //!< Diffuse color.
		Color specular;      //!< Specular color.

		Vector4 position;    //!< Position in eye space. w=0 for infinite light source, otherwise 1.
		Vector3 direction;   //!< Direction.

        Float range;	     //!< Attenuation range.
        Vector3 attenuation; //!< Quadratic attenuation Vector3(constant, linear, quadratic).

        Float cutOff;        //!< Cutoff in radian.
        Float cosCutOff;     //!< Cosine of the cutoff.
        Float exponent;      //!< Attenuation exponent of the cutoff.

		Vector4 worldPos;    //!< Position in world space. w=0 for infinite light source, otherwise 1.
        Vector3 worldDir;    //!< Direction in world space.

        Matrix4 matrixShadowMapProjection;  //!< Light POV matrix.

		LightObject() :
			type(0),
			shadowType(0),
			range(0),
			attenuation(1, 0, 0),
			cutOff(0),
			cosCutOff(0),
			exponent(0)
		{
		}
	};

	//! Define the parameters from a valid light.
	void setFromLight(Light *pLight);

    //! Define the parameters from a valid camera.
    void setFromCamera(Camera *pCamera);

	UInt32 passNum;  //!< Current pass number. Usefull for multi pass object rendering.

	PassMode pass;       //!< Rendering pass.
	LightObject light;   //!< Light information.

    Vector3 eyePos;  //!< Eye position in world coordinates.

	ShaderInstance *shaderInstance; //!< Shader instance use for example in shadow map.
};

} // namespace o3d

#endif // _O3D_DRAWINFO_H

