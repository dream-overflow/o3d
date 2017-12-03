/**
 * @file gltexture.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/texture/gltexture.h"

#include "o3d/engine/glextdefines.h"

#include "o3d/core/architecture.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/context.h"
#include "o3d/engine/renderer.h"

using namespace o3d;

struct Formats
{
	PixelFormat pf;
	DataType data;
	TextureFormat format;
	TextureIntFormat intFormat;
    UInt32 dataBitsPerPixel;
	UInt32 intBitsPerPixel;
};

static const Formats formatsMap[PF_RGBA_DXT5+1] = {
    { PF_RED_U8,  DATA_UNSIGNED_BYTE, TF_RED, TIF_R8,   8, 8 },
    { PF_RED_F16, DATA_HALF_FLOAT,    TF_RED, TIF_R16F, 16, 16 },
    { PF_RED_F32, DATA_FLOAT,         TF_RED, TIF_R32F, 32, 32 },

    { PF_RG_U8,  DATA_UNSIGNED_BYTE, TF_RG, TIF_RG8,   2*8, 2*8 },
    { PF_RG_F16, DATA_HALF_FLOAT,    TF_RG, TIF_RG16F, 2*16, 2*16 },
    { PF_RG_F32, DATA_FLOAT,         TF_RG, TIF_RG32F, 2*32, 2*32 },

    { PF_RGB_U8,  DATA_UNSIGNED_BYTE, TF_RGB, TIF_RGB8,   3*8, 3*8 },
    { PF_RGB_F16, DATA_HALF_FLOAT,    TF_RGB, TIF_RGB16F, 3*16, 3*16 },
    { PF_RGB_F32, DATA_FLOAT,         TF_RGB, TIF_RGB32F, 3*32, 3*32 },

    { PF_RGBA_U8,  DATA_UNSIGNED_BYTE, TF_RGBA, TIF_RGBA8,   4*8, 4*8 },
    { PF_RGBA_F16, DATA_HALF_FLOAT,    TF_RGBA, TIF_RGBA16F, 4*16, 4*16 },
    { PF_RGBA_F32, DATA_FLOAT,         TF_RGBA, TIF_RGBA32F, 4*32, 4*32 },

    { PF_DEPTH,     DATA_UNSIGNED_INT,   TF_DEPTH_COMPONENT, TIF_DEPTH_COMPONENT24,  32, 24 },
    { PF_DEPTH_U16, DATA_UNSIGNED_SHORT, TF_DEPTH_COMPONENT, TIF_DEPTH_COMPONENT16,  16, 16 },
    { PF_DEPTH_U24, DATA_UNSIGNED_INT,   TF_DEPTH_COMPONENT, TIF_DEPTH_COMPONENT24,  32, 24 },
    { PF_DEPTH_U32, DATA_UNSIGNED_INT,   TF_DEPTH_COMPONENT, TIF_DEPTH_COMPONENT32,  32, 32 },
    { PF_DEPTH_F32, DATA_FLOAT,          TF_DEPTH_COMPONENT, TIF_DEPTH_COMPONENT32F, 32, 32 },
    { PF_DEPTH_U24_STENCIL_U8, DATA_UNSIGNED_INT_24_8, TF_DEPTH_STENCIL, TIF_DEPTH24_STENCIL8, 32, 32 },
    { PF_DEPTH_F32_STENCIL_U8, DATA_FLOAT32_UNSIGNED_INT_24_8, TF_DEPTH_STENCIL, TIF_DEPTH32F_STENCIL8, 32, 32 },

    { PF_RGB_DXT1,  DATA_UNSIGNED_BYTE, TF_COMPRESSED_RGB_S3TC_DXT1,  TIF_COMPRESSED_RGB_S3TC_DXT1,  4, 4 },
    { PF_RGBA_DXT3, DATA_UNSIGNED_BYTE, TF_COMPRESSED_RGBA_S3TC_DXT3, TIF_COMPRESSED_RGBA_S3TC_DXT3, 8, 8 },
    { PF_RGBA_DXT5, DATA_UNSIGNED_BYTE, TF_COMPRESSED_RGBA_S3TC_DXT5, TIF_COMPRESSED_RGBA_S3TC_DXT5, 8, 8 }

    // no stencil textures...
};

UInt32 GLTexture::computePixMapSize(
	UInt32 pixelSizeBpp,
	UInt32 width,
	UInt32 height)
{
    if (!width || !height) {
		return 0;
    }

	UInt32 sizeCount = 0;
	UInt32 cX = width, cY = height;

	// if picture width or height is higher than one
    if (width > 1 || height > 1) {
		// n' x m' mipmaps
        do {
			sizeCount += cX * cY * pixelSizeBpp;

			if (cX > 1) cX = cX >> 1;
			if (cY > 1) cY = cY >> 1;
		}
        while (!(cX == 1 && cY == 1));
	}

	// 1x1 mipmap
	sizeCount += pixelSizeBpp;

	return sizeCount >> 3;
}

// load a 1d texture to opengl
UInt32 GLTexture::loadTexture1D(
	Context *glContext,
	Image &pic,
	Bool mipmaps)
{
	UInt32 sizeCount = 0;

	// simple 1d texture
    if (pic.isVolumeTexture() || pic.isCubeMap()) {
		O3D_ERROR(E_InvalidFormat("Invalid texture 1d"));
    }

	// main texture
	pic.bindMipMapLvl(0);

	UInt32 pixelSizeBpp = getInternalPixelSize(glContext->getRenderer(), pic.getPixelFormat());

	UInt32 format = getGLFormat(glContext->getRenderer(), pic.getPixelFormat());
	Int32 internalFormat = getGLInternalFormat(glContext->getRenderer(), pic.getPixelFormat());
	DataType type = getGLType(pic.getPixelFormat());

	// not compressed picture
    if (!pic.isCompressed()) {
		// compute the fully mipmaps
        if (mipmaps && !pic.isMipMap()) {
			sizeCount += computePixMapSize(pixelSizeBpp, pic.getWidth(), pic.getHeight());
        } else {
			sizeCount += (pixelSizeBpp * pic.getWidth() * pic.getHeight()) >> 3;
        }

		// unpack alignment
        if (((getPixelSize(pic.getPixelFormat()) >> 3/* * pic.getWidth()*/) % 4) == 0) {
			glPixelStorei(GL_UNPACK_ALIGNMENT,4);
        } else {
			glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        }

		// primary level (0)
		glTexImage1D(
				GL_TEXTURE_1D,
				0,
				internalFormat,
				pic.getWidth(),
				0,
				format,
				type,
				pic.getData());

		// loads manually the next levels
        if (pic.isMipMap()) {
            for (UInt32 i = 1; i < pic.getNumMipMapLvl()+1; ++i) {
				pic.bindMipMapLvl(i);

				sizeCount += (pixelSizeBpp * pic.getWidth() * pic.getHeight()) >> 3;

				glTexImage1D(
						GL_TEXTURE_1D,
						i,
						internalFormat,
						pic.getWidth(),
						0,
						format,
						type,
						pic.getData());
			}
			// main texture
			pic.bindMipMapLvl(0);
        } else if (mipmaps && !pic.isMipMap()) {
            // or generates mipmaps
			glGenerateMipmap(GL_TEXTURE_1D);
        }
    } else {
        // compressed picture (DTX1/3/5)

        // compute the fully mipmaps
        if (mipmaps && !pic.isMipMap()) {
			sizeCount += computePixMapSize(pixelSizeBpp, pic.getWidth(), pic.getHeight());
        } else {
			sizeCount += pic.getSize();
        }

		// unpack alignment is not used for a compressed image

		// primary level (0)
		glCompressedTexImage1D(
				GL_TEXTURE_1D,
				0,
				internalFormat,
				pic.getWidth(),
				0,
				pic.getSize(),
				pic.getData());

		// loads manually the next levels
        if (pic.isMipMap()) {
            for (UInt32 i = 1; i < pic.getNumMipMapLvl()+1; ++i) {
				pic.bindMipMapLvl(i);

				sizeCount += pic.getSize();

				glCompressedTexImage1D(
						GL_TEXTURE_1D,
						i,
						internalFormat,
						pic.getWidth(),
						0,
						pic.getSize(),
						pic.getData());
			}

			// main texture
			pic.bindMipMapLvl(0);
        } else if (mipmaps && !pic.isMipMap()) {
            // or generates mipmaps
            glGenerateMipmap(GL_TEXTURE_1D);
        }
	}

	return sizeCount;
}

// load a 2d texture to opengl
UInt32 GLTexture::loadTexture2D(
	Context *glContext,
	Image& pic,
	Bool mipmaps)
{
	UInt32 SizeCount = 0;

	// simple 2d texture
	if (pic.isVolumeTexture() || pic.isCubeMap())
		O3D_ERROR(E_InvalidFormat("Invalid texture 2d"));

	// main texture
	pic.bindMipMapLvl(0);

	UInt32 pixelSizeBpp = getInternalPixelSize(glContext->getRenderer(), pic.getPixelFormat());

	UInt32 format = getGLFormat(glContext->getRenderer(), pic.getPixelFormat());
	Int32 internalFormat = getGLInternalFormat(glContext->getRenderer(), pic.getPixelFormat());
	DataType type = getGLType(pic.getPixelFormat());

	// not compressed picture
	if (!pic.isCompressed())
	{
		// compute the fully mipmaps
		if (mipmaps && !pic.isMipMap())
			SizeCount += computePixMapSize(pixelSizeBpp, pic.getWidth(), pic.getHeight());
		else
			SizeCount += (pixelSizeBpp * pic.getWidth() * pic.getHeight()) >> 3;

		// unpack alignment
		if (((getPixelSize(pic.getPixelFormat()) >> 3/* * pic.getWidth()*/) % 4) == 0)
			glPixelStorei(GL_UNPACK_ALIGNMENT,4);
		else
			glPixelStorei(GL_UNPACK_ALIGNMENT,1);

		// primary level (0)
		glTexImage2D(
				GL_TEXTURE_2D,
				0,
				internalFormat,
				pic.getWidth(),
				pic.getHeight(),
				0,
				format,
				type,
				pic.getData());

		// loads manually the next level
		if (pic.isMipMap())
		{
			for (UInt32 i = 1; i < pic.getNumMipMapLvl()+1; ++i)
			{
				pic.bindMipMapLvl(i);

				SizeCount += (pixelSizeBpp * pic.getWidth() * pic.getHeight()) >> 3;

				glTexImage2D(
						GL_TEXTURE_2D,
						i,
						internalFormat,
						pic.getWidth(),
						pic.getHeight(),
						0,
						format,
						type,
						pic.getData());
			}
			// main texture
			pic.bindMipMapLvl(0);
		}
		// or generates mipmaps
		else if (mipmaps && !pic.isMipMap())
			glGenerateMipmap(GL_TEXTURE_2D);
	}
	// compressed picture (DTX1/3/5)
	else
	{
		if (mipmaps && !pic.isMipMap())
			SizeCount += computePixMapSize(pixelSizeBpp, pic.getWidth(), pic.getHeight());
		else
			SizeCount += pic.getSize();

		// unpack alignment is not used for a compressed image

		// primary level (0)
		glCompressedTexImage2D(
				GL_TEXTURE_2D,
				0,
				internalFormat,
				pic.getWidth(),
				pic.getHeight(),
				0,
				pic.getSize(),
				pic.getData());

		// loads manually the next level
		if (pic.isMipMap())
		{
			for (UInt32 i = 1; i < pic.getNumMipMapLvl()+1; ++i)
			{
				pic.bindMipMapLvl(i);

				SizeCount += pic.getSize();

				glCompressedTexImage2D(
						GL_TEXTURE_2D,
						i,
						internalFormat,
						pic.getWidth(),
						pic.getHeight(),
						0,
						pic.getSize(),
						pic.getData());
			}
			// main texture
			pic.bindMipMapLvl(0);
		}
		// or generates mipmaps
		else if (mipmaps && !pic.isMipMap())
			glGenerateMipmap(GL_TEXTURE_2D);
	}

	return SizeCount;
}

// load a 3d texture to opengl
UInt32 GLTexture::loadTexture3D(
	Context *glContext,
	Image& pic,
	Bool mipmaps)
{
	UInt32 sizeCount = 0;

	// Not implemented at this time
	O3D_ASSERT(0);

	return sizeCount;
}

// load a cubemap texture to opengl
void GLTexture::loadTexCubeMapSide(
	Context *glContext,
	Image& pic,
	UInt32 side,
	Bool mipmaps,
	UInt32 &sizeCount)
{
	// simple 2d texture
	if (pic.isVolumeTexture())
		O3D_ERROR(E_InvalidFormat("Invalid cubemap texture"));

	if (mipmaps)
	{
		if (!o3d::isPow2(pic.getWidth()) || !o3d::isPow2(pic.getHeight()))
			O3D_ERROR(E_InvalidParameter("Mipmaps are not available with non-power-of-two textures"));
	}

	// main texture
	pic.bindMipMapLvl(0);

	UInt32 pixelSizeBpp = getInternalPixelSize(glContext->getRenderer(), pic.getPixelFormat());

	UInt32 format = getGLFormat(glContext->getRenderer(), pic.getPixelFormat());
	Int32 internalFormat = getGLInternalFormat(glContext->getRenderer(), pic.getPixelFormat());
	DataType type = getGLType(pic.getPixelFormat());

	// not compressed picture
	if (!pic.isCompressed())
	{
		// need to create mipmap
		if (mipmaps && !pic.isMipMap())
			sizeCount += computePixMapSize(pixelSizeBpp, pic.getWidth(), pic.getHeight());
		else
			sizeCount += (pixelSizeBpp * pic.getWidth() * pic.getHeight()) >> 3;

		// unpack alignment
		if (((getPixelSize(pic.getPixelFormat()) >> 3/* * pic.getWidth()*/) % 4) == 0)
			glPixelStorei(GL_UNPACK_ALIGNMENT,4);
		else
			glPixelStorei(GL_UNPACK_ALIGNMENT,1);

		// primary level (0)
		glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X+side,
				0,
				internalFormat,
				pic.getWidth(),
				pic.getHeight(),
				0,
				format,
				type,
				pic.getData());

		// loads manually the next level
		if (pic.isMipMap())
		{
			for (UInt32 i = 1; i < pic.getNumMipMapLvl()+1; ++i)
			{
				pic.bindMipMapLvl(i);

				sizeCount += pic.getSize();

				glTexImage2D(
						GL_TEXTURE_CUBE_MAP_POSITIVE_X+side,
						i,
						internalFormat,
						pic.getWidth(),
						pic.getHeight(),
						0,
						format,
						type,
						pic.getData());
			}
			// main texture
			pic.bindMipMapLvl(0);
		}
	}
	// compressed picture (DTX1/3/5)
	else
	{
		if (mipmaps && !pic.isMipMap())
			sizeCount += computePixMapSize(pixelSizeBpp, pic.getWidth(), pic.getHeight());
		else
			sizeCount += pic.getSize();

		// unpack alignment is not used for a compressed image

		// primary level (0)
		glCompressedTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X+side,
				0,
				internalFormat,
				pic.getWidth(),
				pic.getHeight(),
				0,
				pic.getSize(),
				pic.getData());

		// loads manually the next level
		if (pic.isMipMap())
		{
			for (UInt32 i = 1; i < pic.getNumMipMapLvl()+1; ++i)
			{
				pic.bindMipMapLvl(i);

				sizeCount += pic.getSize();

				glCompressedTexImage2D(
						GL_TEXTURE_CUBE_MAP_POSITIVE_X+side,
						i,
						internalFormat,
						pic.getWidth(),
						pic.getHeight(),
						0,
						pic.getSize(),
						pic.getData());
			}
			// main texture
			pic.bindMipMapLvl(0);
		}
	}
}

UInt32 GLTexture::loadTextureCubeMap(
	Context *glContext,
	Image *pics,
	Bool mipmaps)
{
	UInt32 sizeCount = 0;

	// if only one texture and not a cubemap
	if (pics[0].isValid() && pics[1].isEmpty())
	{
		if (!pics[0].isCubeMap())
			O3D_ERROR(E_InvalidFormat("Invalid cubemap texture"));

		for (UInt32 i = 0; i < 6; ++i)
		{
			pics[0].bindCubeMapSide(ImgFormat::CubeMapSide(i));
			loadTexCubeMapSide(glContext, pics[0], i, mipmaps, sizeCount);
		}

		// reset to first side
		pics[0].bindCubeMapSide(ImgFormat::LEFT_SIDE);

		// generates mipmaps if necessary
		if (mipmaps && !pics[0].isMipMap())
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
	else
	{
		if (pics[0].isEmpty() || pics[1].isEmpty() || pics[2].isEmpty() ||
		    pics[3].isEmpty() || pics[4].isEmpty() || pics[5].isEmpty())
		{
			O3D_ERROR(E_InvalidParameter("One or more picture(s) are empty"));
		}

		for (UInt32 i = 0; i < 6; ++i)
		{
			loadTexCubeMapSide(glContext, pics[i], i, mipmaps, sizeCount);
		}

		Bool mips = pics[0].isMipMap() & pics[1].isMipMap() & pics[2].isMipMap() &
			pics[3].isMipMap() & pics[4].isMipMap() & pics[5].isMipMap();

		// generates mipmaps if necessary
		if (mipmaps && !mips)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	return sizeCount;
}

// load a cubemap texture to opengl from 1 cubemap pictures
UInt32 GLTexture::loadTextureCubeMap(
	Context *glContext,
	Image &pic,
	Bool mipmaps)
{
	UInt32 sizeCount = 0;

    if (pic.isEmpty()) {
		O3D_ERROR(E_InvalidParameter("Image is empty"));
    }

	// if only one texture and not a cubemap
    if (!pic.isCubeMap()) {
		O3D_ERROR(E_InvalidFormat("Invalid cubemap texture"));
    }

    for (UInt32 i = 0 ; i < 6 ; ++i) {
		pic.bindCubeMapSide(ImgFormat::CubeMapSide(i));
		loadTexCubeMapSide(glContext, pic, i, mipmaps, sizeCount);
	}
	pic.bindCubeMapSide(ImgFormat::LEFT_SIDE);

	// generates mipmaps if necessary
    if (mipmaps && !pic.isMipMap()) {
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

	return sizeCount;
}

TextureFormat GLTexture::getGLFormat(const Renderer *renderer, PixelFormat pixelFormat)
{
    return formatsMap[pixelFormat].format;
}

TextureIntFormat GLTexture::getGLInternalFormat(const Renderer *renderer, PixelFormat pixelFormat)
{
    return formatsMap[pixelFormat].intFormat;
}

DataType GLTexture::getGLType(PixelFormat pixelFormat)
{
	return formatsMap[pixelFormat].data;
}

UInt32 GLTexture::getPixelSize(PixelFormat pixelFormat)
{
	return formatsMap[pixelFormat].dataBitsPerPixel;
}

UInt32 GLTexture::getInternalPixelSize(const Renderer *renderer, PixelFormat pixelFormat)
{
    return formatsMap[pixelFormat].intBitsPerPixel;
}
