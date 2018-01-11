/**
 * @file texturearray2d.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/texture/texturearray2d.h"

#include "o3d/engine/glextdefines.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/texture/gltexture.h"
#include "o3d/engine/context.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/renderer.h"
#include "o3d/engine/enginetype.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(TextureArray2D, ENGINE_TEXTURE_2D_ARRAY, Texture)

// Make a key name.
String TextureArray2D::makeResourceName(UInt32 size)
{
    return String::print("<array2d(%u)>", size);
}

// Construct using a texture filename.
TextureArray2D::TextureArray2D(
	BaseObject *parent,
	UInt32 width,
	UInt32 height,
	UInt32 layers) :
		Texture(parent)
{
	m_textureType = TEXTURE_2D_ARRAY;
}

TextureArray2D::~TextureArray2D()
{
}

// set the filtering mode to OpenGL
void TextureArray2D::setFilteringMode()
{
	if ((m_updateFlags & UPDATE_FILTERING) != UPDATE_FILTERING)
			return;

	m_updateFlags ^= UPDATE_FILTERING;

	if ((m_updateFlags & UPDATE_ANISOTROPY) == UPDATE_ANISOTROPY)
		m_updateFlags ^= UPDATE_ANISOTROPY;

	switch (m_filtering)
	{
	case Texture::NO_FILTERING:
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameterf(TEXTURE_2D_ARRAY,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;

	case Texture::LINEAR_FILTERING:
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameterf(TEXTURE_2D_ARRAY,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;

	case Texture::BILINEAR_FILTERING:
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		glTexParameterf(TEXTURE_2D_ARRAY,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;

	case Texture::BILINEAR_ANISOTROPIC:
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		glTexParameterf(TEXTURE_2D_ARRAY,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_anisotropy);
		break;

	case Texture::TRILINEAR_FILTERING:
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(TEXTURE_2D_ARRAY,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;

	case Texture::TRILINEAR_ANISOTROPIC:
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(TEXTURE_2D_ARRAY,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_anisotropy);
		break;

	default:	// linear
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameterf(TEXTURE_2D_ARRAY,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;
	}
}

// set the warp mode to OpenGL
void TextureArray2D::setWrapMode()
{
	if ((m_updateFlags & UPDATE_WRAPMODE) != UPDATE_WRAPMODE)
		return;

	m_updateFlags ^= UPDATE_WRAPMODE;

	switch (m_wrap)
	{
	case Texture::REPEAT:
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_REPEAT);
		break;

	case Texture::CLAMP:
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		break;

	default:  // repeat
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_REPEAT);
		break;
	}
}

// Set the anisotropy level to OpenGL.
void TextureArray2D::setAnisotropyLevel()
{
	if ((m_updateFlags & UPDATE_ANISOTROPY) != UPDATE_ANISOTROPY)
			return;

	m_updateFlags ^= UPDATE_ANISOTROPY;

	switch (m_filtering)
	{
	case Texture::NO_FILTERING:
		glTexParameterf(TEXTURE_2D_ARRAY,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;

	case Texture::LINEAR_FILTERING:
		glTexParameterf(TEXTURE_2D_ARRAY,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;

	case Texture::BILINEAR_FILTERING:
		glTexParameterf(TEXTURE_2D_ARRAY,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;

	case Texture::BILINEAR_ANISOTROPIC:
		glTexParameterf(TEXTURE_2D_ARRAY,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_anisotropy);
		break;

	case Texture::TRILINEAR_FILTERING:
		glTexParameterf(TEXTURE_2D_ARRAY,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;

	case Texture::TRILINEAR_ANISOTROPIC:
		glTexParameterf(TEXTURE_2D_ARRAY,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_anisotropy);
		break;

	default:	// linear
		glTexParameterf(TEXTURE_2D_ARRAY,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;
	}
}

// create an empty texture given a size and a format
Bool TextureArray2D::create(
	Bool mipmaps,
	UInt32 width,
	UInt32 height,
	UInt32 layers,
	PixelFormat textureFormat,
	Bool dontUnbind)
{
    return create(mipmaps, width, height, layers, textureFormat, nullptr, textureFormat, dontUnbind);
}

// Create an texture from a buffer
Bool TextureArray2D::create(
	Bool mipmaps,
	UInt32 width,
	UInt32 height,
	UInt32 layers,
	PixelFormat textureFormat,
	const void *buffer,
	PixelFormat bufferFormat,
	Bool dontUnbind)
{
	UInt32 maxLevel = 0;
	UInt32 dbgTexSizeInByte = 0;

	if (mipmaps)
		maxLevel = o3d::log2(o3d::max(width,height));

    GLint internalFormat = GLTexture::getGLInternalFormat(getScene()->getContext(), textureFormat);
    GLenum format = GLTexture::getGLFormat(getScene()->getContext(), bufferFormat);
	GLenum type = GLTexture::getGLType(bufferFormat);
    UInt32 internalPixelSize = GLTexture::getInternalPixelSize(getScene()->getContext(), textureFormat);
	UInt32 bufferPixelSize = GLTexture::getPixelSize(bufferFormat);

	Bool compressed = False;

	// pixel stored is not used for compressed texture
	if ((((bufferPixelSize/* * width*/ >> 3) % 4) == 0) && !compressed)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	else
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Create the texture given its parameters
	Bool create = generateId(textureFormat, width, height, layers, 0, maxLevel);

	getScene()->getContext()->bindTexture(TEXTURE_2D_ARRAY, m_textureId, True);

	m_updateFlags = UPDATE_ALL;

	setFilteringMode();
	setWrapMode();

	if (compressed)
	{
		// size of buffer
		UInt32 size = (width * height * bufferPixelSize) >> 3;
		glCompressedTexImage3D(TEXTURE_2D_ARRAY, 0, format, width, height, layers, 0, size, buffer);
	}
	else
		glTexImage3D(TEXTURE_2D_ARRAY, 0, internalFormat, width, height, layers, 0, format, type, buffer);

	O3D_ASSERT(!getScene()->getRenderer()->isError());

	// build mipmaps levels
	if (mipmaps)
	{
		glGenerateMipmap(TEXTURE_2D_ARRAY);
		dbgTexSizeInByte = GLTexture::computePixMapSize(internalPixelSize, width, height) * layers;
	}
	else
		dbgTexSizeInByte = ((width*height*layers) * internalPixelSize) >> 3;

	if (create)
		O3D_GALLOC(MemoryManager::GPU_TEXTURE_2D_ARRAY, m_textureId, dbgTexSizeInByte);
	else
		O3D_GREALLOC(MemoryManager::GPU_TEXTURE_2D_ARRAY, m_textureId, dbgTexSizeInByte);

	// unbind the texture
	if (!dontUnbind)
		getScene()->getContext()->bindTexture(TEXTURE_2D_ARRAY,0);

	return True;
}

// Update the texture data. The texture data must have the same size and format as
Bool TextureArray2D::update(const void *buffer, PixelFormat bufferFormat, Bool dontUnbind)
{
	if (m_textureId == O3D_UNDEFINED)
		return False;

	Bool mipmaps = getNumMipMaps() > 1;

	getScene()->getContext()->bindTexture(TEXTURE_2D_ARRAY, m_textureId, True);

	setFilteringMode();
	setWrapMode();

    GLenum format = GLTexture::getGLFormat(getScene()->getContext(), bufferFormat);
	GLenum type = GLTexture::getGLType(bufferFormat);
	UInt32 bufferPixelSize = GLTexture::getPixelSize(bufferFormat);

	Bool compressed = False;

	// pixel stored is not used for compressed texture
	if ((((bufferPixelSize/* * m_width*/ >> 3) % 4) == 0) && !compressed)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	else
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (compressed)
	{
		UInt32 size = (m_width * m_height * bufferPixelSize) >> 3;
		glCompressedTexSubImage3D(TEXTURE_2D_ARRAY, 0, 0, 0, 0, m_width, m_height, m_depth, format, size, buffer);
	}
	else
		glTexSubImage3D(TEXTURE_2D_ARRAY, 0, 0, 0, 0, m_width, m_height, m_depth, format, type, buffer);

	// build mipmaps levels
	if (mipmaps)
		glGenerateMipmap(TEXTURE_2D_ARRAY);

	// unbind the texture
	if (!dontUnbind)
		getScene()->getContext()->bindTexture(TEXTURE_2D_ARRAY, 0);

	return True;
}

Bool TextureArray2D::addMipsLevels(UInt32 minLevel, UInt32 maxLevel)
{
	if (m_textureId != O3D_UNDEFINED)
	{
		getScene()->getContext()->bindTexture(TEXTURE_2D_ARRAY, m_textureId);

		glGenerateMipmap(TEXTURE_2D_ARRAY);

		getScene()->getContext()->bindTexture(TEXTURE_2D_ARRAY,0);

		m_minLevel = 0;
		m_maxLevel = (UInt32)o3d::log2(o3d::max(m_width, m_height));

		return True;
	}
	else
		return False;
}

// bind the texture to the hardware
Bool TextureArray2D::bind()
{
	if (isValid())
	{
		getScene()->getContext()->bindTexture(TEXTURE_2D_ARRAY, m_textureId);

		setFilteringMode();
		setWrapMode();
		setAnisotropyLevel();

		return True;
	}
	return False;
}

// unbind the texture in OpenGL context
void TextureArray2D::unbind()
{
	getScene()->getContext()->bindTexture(TEXTURE_2D_ARRAY, 0, True);
}

// set the texture parameter, such as enable extra coord for cubemap...
void TextureArray2D::set()
{
}

// unset the texture parameter
void TextureArray2D::unSet()
{
}

// clear all memory
void TextureArray2D::destroy()
{
	Texture::destroy();
}

// serialization
Bool TextureArray2D::writeToFile(OutStream &os)
{
    if (!Texture::writeToFile(os))
		return False;

	return True;
}

Bool TextureArray2D::readFromFile(InStream &is)
{
	destroy();

    if (!Texture::readFromFile(is))
		return False;

	return True;
}

