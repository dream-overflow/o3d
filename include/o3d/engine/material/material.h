/**
 * @file material.h
 * @brief Abstract shader object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-08-30
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MATERIAL_H
#define _O3D_MATERIAL_H

#include "o3d/image/color.h"

#include "../scene/sceneentity.h"
#include "../drawinfo.h"

#include "../vertexarray.h"

#include "../shader/shader.h"

#include "o3d/core/memorydbg.h"

namespace o3d {

class Shadable;
class Shadowable;
class Pickable;
class MaterialPass;

/**
 * @brief A material establish the OpenGL context parameters, add bind texture, attributes
 * array, and define uniforms parameters (like ambient, diffuse, bump offset...).
 * @details A material can be manually specified by inheriting from this class, but it can also
 * use the programmable definition or by importing its definition from an o3dmt file.
 * Uniforms locations :
 *  - For uniforms parameters @deprecated @todo UBO :
 *    - vec4 u_ambient : Premultiplied object ambient * current light ambient color.
 *    - vec4 u_diffuse : Premultiplied object diffuse * current light diffuse color.
 *    - vec4 u_specular : Premultiplied object specular * current light specular color
 *    - vec4 u_matAmbient : Object material ambient color.
 *    - vec4 u_matDiffuse : Object material diffuse color.
 *    - vec4 u_matSpecular : Object material specular color.
 *    - vec4 u_matEmission : Object material emission/self illumination color.
 *    - float u_shine : Object material specular shininess [0..+oo].
 *    - float u_transparency : Object material transparency [0..1].
 *
 * Vertex attribute locations :
 *  -  layout(location = 0) in vec3 a_vertex;
 *  - @see VertexAttributeArray @todo update following values
 *  - vec3 a_normal (location 1) : Vertex normal (TBN) attribute.
 *  - vec3 a_tangent (location 2) : Vertex tangent (TBN) attribute.
 *  - vec3 a_bitangent (location 3) : Vertex bi-tangent (TBN) attribute.
 *  - vec4 a_color (location 4) : Vertex color attribute.
 *  - float a_rigging (location 5) : Vertex rigging bones ID attribute.
 *  - vec4 a_skinning (location 6) : Vertex skinning 4 bones ID attribute.
 *  - vec4 a_weighting (location 7) : Vertex weighting 4 bones weight attribute.
 *  - xxx a_texCoords1 (location 8) : Vertex texture 1d, 2d, 3d or 4d coordinates 0 attribute.
 *  - xxx a_texCoords2 (location 9) : Vertex texture 1d, 2d, 3d or 4d coordinates 0 attribute.
 *
 * Link in/out locations :
 *  - layout(location = 0) smooth {in,out} vec3 io_position;
 *  - layout(location = 1) smooth {in,out} vec3 io_normal;
 */
class O3D_API Material : public SceneResource, NonCopyable<>
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(Material)

	//! Default constructor.
	Material(BaseObject *parent);

	//! Virtual destructor.
	virtual ~Material();

	//! Shader initialization mode.
	enum InitMode
	{
		PICKING,      //!< Initialize for picking.
		AMBIENT,      //!< Initialize for ambient pass.
		LIGHTING,     //!< Initialize for lighting pass.
		DEFERRED      //!< Initialize for deferred diffuse pass.
	};

	//! Number of init modes.
	static const Int32 NUM_INIT_MODES = 4;

	//! Check if the material is supported by the hardware.
	virtual Bool isMaterialSupported() const = 0;

	//! Initialize material shader according to a specific MaterialPass setting,
	//! and shadable vertex mode.
	virtual void initialize(
			InitMode initMode,
			MaterialPass &materialPass,
			Shadable &shadable) = 0;

	//! Release initialized data.
	virtual void release() = 0;

	//! Is the material operational for use.
	inline Bool isValid() const { return m_valid; }

	//! Get the specialization mode of the material.
	inline InitMode getInitMode() const { return m_initMode; }

	//! Get the option string used to build the shader instance.
	inline const String& getOptions() const { return m_options; }

	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	//! Shadable object rendering for ambient pass.
	//! @param object Shadable object to render.
	//! @param processMode Rendering mode.
	//! @param materialPass Material pass object that contain parameters.
	virtual void processAmbient(
			Shadable &object,
			const DrawInfo &drawInfo,
			const MaterialPass &materialPass) = 0;

	//! Shadable object rendering for picking.
	//! @param object Shadable object to render.
	//! @param pickable Pickable object to render.
	//! @param processMode Rendering mode.
	//! @param materialPass Material pass object that contain parameters.
	virtual void processPicking(
			Shadable &object,
			Pickable &pickable,
			const DrawInfo &drawInfo,
			const MaterialPass &materialPass) = 0;

	//! Shadable object rendering for lighting and shadow pass.
	//! @param object Shadable object to render.
	//! @param object Shadowable object to render.
	//! @param processMode Rendering mode.
	//! @param materialPass Material pass object that contain parameters.
	virtual void processLighting(
			Shadable &object,
			Shadowable &shadowable,
			const DrawInfo &drawInfo,
			const MaterialPass &materialPass) = 0;

	//! Shadable object rendering for deferred diffuse pass.
	//! @param object Shadable object to render.
	//! @param processMode Rendering mode.
	//! @param materialPass Material pass object that contain parameters.
	virtual void processDeferred(
			Shadable &object,
			const DrawInfo &drawInfo,
			const MaterialPass &materialPass) = 0;

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

	//! Save the material definition according to its class type and file name.
	Bool save();

protected:

    Bool m_valid;               //!< True means valid for usage.

	InitMode m_initMode;        //!< Specialization mode.

	VertexArray::T_VertexAttributeList m_arrays; //!< Used attribute array.

	String m_options;       //!< Options used to build the shader instance.
	String m_shaderName;    //!< Related shader object name.
	String m_filename;      //!< The file that defines this material.
};

} // namespace o3d

#endif // _O3D_MATERIAL_H
