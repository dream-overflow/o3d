/**
 * @file texturearray2d.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEXTUREARRAY2D_H
#define _O3D_TEXTUREARRAY2D_H

#include "texture.h"

namespace o3d {

/**
 * @brief Two dimensional texture array.
 * In others words a texture array 2d is a multi-layer set a 2d texture. Like 3d texture,
 * but without relations between the pixels of two layers.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-08-26
 */
class O3D_API TextureArray2D : public Texture
{
public:

	O3D_DECLARE_CLASS(TextureArray2D)

    //! Make a resource name.
    static String makeResourceName(UInt32 size);

	//! Construct using a size and number of layer.
	//! @param parent Owner.
	TextureArray2D(
		BaseObject *parent,
		UInt32 width = 0,
		UInt32 height = 0,
		UInt32 layers = 0);

	//! Destructor.
	virtual ~TextureArray2D();


	//-----------------------------------------------------------------------------------
	// Texture creation
	//-----------------------------------------------------------------------------------

	//! Create an empty texture given a size and a format.
	//! If the texture already exists it return false immediately.
	//! @param mipmaps If true it generate mipmaps levels.
	//! @param pixelFormat can be one of the PixelFormat enum.
	//! @param dontUnbind if true the texture is not unbound, in others words it is kept as current
	Bool create(
		Bool mipmaps,
		UInt32 width,
		UInt32 height,
		UInt32 layers,
		PixelFormat pixelFormat,
		Bool dontUnbind = False);

	//! Create a texture from a buffer.
	//! If the texture already exists it return false immediately.
	//! Use Update to replace the data.
	//! @param mipmaps If true it generate mipmaps levels.
	//! @param pixelFormat Internal texture pixel format, can be one of the PixelFormat enum.
    //! @param buffer data or null. Size of the data is defined by w*h*l*bufferFormat.bpp.
	//! @param bufferFormat Pixel format of the buffer.
	Bool create(
		Bool mipmaps,
		UInt32 width,
		UInt32 height,
		UInt32 layers,
		PixelFormat pixelFormat,
		const void *buffer,
		PixelFormat bufferFormat,
		Bool dontUnbind = False);

	//! Update the texture data. The texture data must have the same dimensions as current.
	//! It will have the same number of mipmap levels.
	//! @param bufferFormat Pixel format of the buffer. Defines the data type and format.
	//! @return False if the texture does not exists.
	Bool update(const void *buffer, PixelFormat bufferFormat, Bool dontUnbind = False);

	//! Destroy the OpenGL texture.
	virtual void destroy();

	virtual Bool addMipsLevels(UInt32 minLevel, UInt32 maxLevel);


	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	//! Set the texture parameter, such as enable extra coord for cubemap...
	virtual void set();
	//! Bind an opengl texture
	virtual Bool bind();
	//! unbind the texture
	virtual void unbind();
	//! Unset the texture parameter
	virtual void unSet();


	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

protected:

	//! Set the filtering mode to OpenGL.
	virtual void setFilteringMode();
	//! Set the warp mode to OpenGL.
	virtual void setWrapMode();
	//! Set the anisotropy level to OpenGL.
	virtual void setAnisotropyLevel();
};


} // namespace o3d

#endif // _O3D_TEXTUREARRAY2D_H

