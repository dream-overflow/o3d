/**
 * @file skybox.h
 * @brief shader can be only a color, with 5 or 6 simple textures.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-01-05
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SKYBOX_H
#define _O3D_SKYBOX_H

#include "specialeffects.h"
#include "o3d/core/smartpointer.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

/**
 * @brief A skybox, this is the single object who is not inherit from Shadable
 * because this shader can be only a color, with 5 or 6 simple textures.
 */
class O3D_API SkyBox : public SpecialEffects
{
public:

	O3D_DECLARE_CLASS(SkyBox)

	//! Constructor.
	SkyBox(BaseObject *parent);

	//! Virtual destructor.
	virtual ~SkyBox();

    //! Create the sky box with the six sides. Can have null sides.
	//! @note It need texture with clamp mode (no repeat).
	void create(
		Float size,
		Texture2D *xp, Texture2D *xn,
		Texture2D *yp, Texture2D *yn,
		Texture2D *zp, Texture2D *zn,
		Bool halfHeight = False,
		Texture::FilteringMode filtering = Texture::LINEAR_FILTERING,
		Float anisotropy = 1.f);

	//! Create the sky box with the six sides. Can have empty sides.
	//! @note It need texture with clamp mode (no repeat).
	Int32 create(
		Float size,
		const String &xp, const String &xn,
		const String &yp, const String &yn,
		const String &zp, const String &zn,
		Bool halfHeight = False,
		Texture::FilteringMode filtering = Texture::LINEAR_FILTERING,
		Float anisotropy = 1.f);

	//! reset settings
	void destroy();

	//! draw all faces
    virtual void processAllFaces(Shadable::ProcessingPass pass) override;

    virtual void attribute(VertexAttributeArray mode, UInt32 location) override;
    virtual void setUpModelView() override;

	//! draw the effect
    virtual void draw(const DrawInfo &drawInfo) override;

	// Serialization
    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

protected:

    ArrayBufferf *m_vertices[6];
    ArrayBufferf *m_texCoords[6];

    VertexArray *m_vaos[6];

	Int32 m_currentSide;  //!< Currently rendered side.
	Float m_size;         //!< Size of the cube.

	MaterialProfile m_material;
	SmartObject<Texture2D> m_textures[6];

    //! start skybox at half height (superior part only) need the left,right,back,front texture are half too
	Bool m_halfHeight;

	//! Initialize vertex buffer object for the valid skybox sides.
	void initVBO();
};

} // namespace o3d

#endif // _O3D_SKYBOX_H
