/**
 * @file gltexture.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_GLTEXTURE_H
#define _O3D_GLTEXTURE_H

#include "o3d/image/image.h"
#include "../enginetype.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class Renderer;
class Context;

/**
 * @brief Load picture in OpenGL texture memory (1d,2d,3d,cubemap,mipmap).
 * Contains formats and internals formats conversion from O3D pixels formats.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-08-05
 */
class O3D_API GLTexture
{
public:

	//! Load a 1d texture to OpenGL.
	static UInt32 loadTexture1D(
		Context *context,
		Image &pic,
		Bool mipmaps);

	//! Load a 2d texture to OpenGL
	static UInt32 loadTexture2D(
		Context *context,
		Image &pic,
		Bool mipmaps);

	//! Load a 3d texture to OpenGL
	static UInt32 loadTexture3D(
		Context *context,
		Image &pic,
		Bool mipmaps);

	//! Load a cubemap texture to OpenGL from 6 normals pictures
	static UInt32 loadTextureCubeMap(
		Context *context,
		Image *pics,
		Bool mipmaps);

	//! Load a cubemap texture to OpenGL from 1 cubemap pictures
	static UInt32 loadTextureCubeMap(
		Context *context,
		Image &pic,
		Bool mipmaps);

	//! Compute all mipmap size for a given size and texture OpenGL format
	//! @param pixelSize In bits per pixel.
	static UInt32 computePixMapSize(
		UInt32 pixelSize,
		UInt32 width,
		UInt32 height);

	//! Load a cubemap texture to OpenGL from 1 complex picture
	static void loadTexCubeMapSide(
		Context *context,
		Image& pic,
		UInt32 side,
		Bool mipmaps,
		UInt32 &sideSize);

	//! Pixel format to OpenGL data type. The size of a buffer depends of the type.
	static DataType getGLType(PixelFormat pixelFormat);

	//! Pixel format to OpenGL data format.
	static TextureFormat getGLFormat(const Renderer *renderer, PixelFormat pixelFormat);

	//! Pixel format to OpenGL data internal format.
	static TextureIntFormat getGLInternalFormat(const Renderer *renderer, PixelFormat pixelFormat);

	//! Get the pixel size of the pixel format, in bits per pixel related to getGLType.
	//! Used for the buffer data IO.
	static UInt32 getPixelSize(PixelFormat pixelFormat);

	//! Get the internal pixel size, in bits per pixel. Usefull to compute the GPU memory occupation.
	static UInt32 getInternalPixelSize(const Renderer *renderer, PixelFormat pixelFormat);
};

} // namespace o3d

#endif // _O3D_GLTEXTURE_H

