/**
 * @file texture2d.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/texture/texture2d.h"

#include "o3d/core/filemanager.h"

#include "o3d/engine/glextdefines.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/texture/gltexture.h"
#include "o3d/engine/context.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/renderer.h"
#include "o3d/engine/enginetype.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Texture2D, ENGINE_TEXTURE_2D, Texture)

// Construct using a texture filename.
Texture2D::Texture2D(BaseObject *parent, const TextureOps &operations) :
    Texture(parent, operations)
{
	m_textureType = TEXTURE_2D;
}

// Construct using a generated and valid picture.
Texture2D::Texture2D(
	BaseObject *parent,
	const Image &picture,
	const TextureOps &operations) :
		Texture(parent, operations)
{
	m_textureType = TEXTURE_2D;

	if (picture.isEmpty())
		O3D_ERROR(E_InvalidParameter("Image must be valid"));

	m_picture = picture;
}

Texture2D::~Texture2D()
{
}

// set the filtering mode to OpenGL
void Texture2D::setFilteringMode()
{
	if ((m_updateFlags & UPDATE_FILTERING) != UPDATE_FILTERING)
			return;

	m_updateFlags ^= UPDATE_FILTERING;

	if ((m_updateFlags & UPDATE_ANISOTROPY) == UPDATE_ANISOTROPY)
		m_updateFlags ^= UPDATE_ANISOTROPY;

	switch (m_filtering)
	{
	case Texture::NO_FILTERING:
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameterf(TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;

	case Texture::LINEAR_FILTERING:
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameterf(TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;

	case Texture::BILINEAR_FILTERING:
        glTexParameteri(TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		glTexParameterf(TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;

	case Texture::BILINEAR_ANISOTROPIC:
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		glTexParameterf(TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_anisotropy);
		break;

	case Texture::TRILINEAR_FILTERING:
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;

	case Texture::TRILINEAR_ANISOTROPIC:
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_anisotropy);
		break;

	default:	// linear
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameterf(TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;
	}
}

// set the warp mode to OpenGL
void Texture2D::setWrapMode()
{
    if ((m_updateFlags & UPDATE_WRAPMODE) != UPDATE_WRAPMODE)
		return;

    m_updateFlags ^= UPDATE_WRAPMODE;

    switch (m_wrap)
	{
	case Texture::REPEAT:
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		break;

	case Texture::CLAMP:
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		break;

	default:  // repeat
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		break;
	}
}

// Set the anisotropy level to OpenGL.
void Texture2D::setAnisotropyLevel()
{
	if ((m_updateFlags & UPDATE_ANISOTROPY) != UPDATE_ANISOTROPY)
			return;

	m_updateFlags ^= UPDATE_ANISOTROPY;

	switch (m_filtering)
	{
	case Texture::NO_FILTERING:
		glTexParameterf(TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;

	case Texture::LINEAR_FILTERING:
		glTexParameterf(TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;

	case Texture::BILINEAR_FILTERING:
		glTexParameterf(TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;

	case Texture::BILINEAR_ANISOTROPIC:
		glTexParameterf(TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_anisotropy);
		break;

	case Texture::TRILINEAR_FILTERING:
		glTexParameterf(TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;

	case Texture::TRILINEAR_ANISOTROPIC:
		glTexParameterf(TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_anisotropy);
		break;

	default:	// linear
		glTexParameterf(TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;
	}
}

// create an empty texture given a size and a format
Bool Texture2D::create(
	Bool mipmaps,
	UInt32 width,
	UInt32 height,
	PixelFormat textureFormat,
	Bool dontUnbind)
{
	return create(mipmaps, width, height, textureFormat, NULL, textureFormat, dontUnbind);
}

// Create an texture from a buffer
Bool Texture2D::create(
	Bool mipmaps,
	UInt32 width,
	UInt32 height,
	PixelFormat textureFormat,
	const void *buffer,
	PixelFormat bufferFormat,
	Bool dontUnbind)
{
	UInt32 maxLevel = 0;
	UInt32 dbgTexSizeInByte = 0;

	if (mipmaps)
		maxLevel = o3d::log2(o3d::max(width,height));

	GLint internalFormat = GLTexture::getGLInternalFormat(getScene()->getRenderer(), textureFormat);
	GLenum format = GLTexture::getGLFormat(getScene()->getRenderer(), bufferFormat);
	GLenum type = GLTexture::getGLType(bufferFormat);
	UInt32 internalPixelSize = GLTexture::getInternalPixelSize(getScene()->getRenderer(), textureFormat);
	UInt32 bufferPixelSize = GLTexture::getPixelSize(bufferFormat);

	Bool compressed = False;
	
	// pixel stored is not used for compressed texture
	if ((((bufferPixelSize/* * width*/ >> 3) % 4) == 0) && !compressed)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	else
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Create the texture given its parameters
	Bool create = generateId(textureFormat, width, height, 1, 0, maxLevel);

	getScene()->getContext()->bindTexture(TEXTURE_2D, m_textureId, True);

	m_updateFlags = UPDATE_ALL;

	setFilteringMode();
    setWrapMode();

	if (compressed)
	{
		// size of buffer
		UInt32 size = (width * height * bufferPixelSize) >> 3;
		glCompressedTexImage2D(TEXTURE_2D, 0, format, width, height, 0, size, buffer);
	}
	else
		glTexImage2D(TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, buffer);

    O3D_ASSERT(!getScene()->getRenderer()->isError());

	// build mipmaps levels
	if (mipmaps)
	{
		glGenerateMipmap(TEXTURE_2D);
		dbgTexSizeInByte = GLTexture::computePixMapSize(internalPixelSize, width, height);
	}
	else
		dbgTexSizeInByte = ((width*height) * internalPixelSize) >> 3;

	//dbgTexSizeInByte = (4*dbgTexSizeInByte - (dbgTexSizeInByte >> (2*(getNumMipMaps()-1))))/3;

	if (create)
		O3D_GALLOC(MemoryManager::GPU_TEXTURE_2D, m_textureId, dbgTexSizeInByte);
	else
		O3D_GREALLOC(MemoryManager::GPU_TEXTURE_2D, m_textureId, dbgTexSizeInByte);

	// unbind the texture
	if (!dontUnbind)
		getScene()->getContext()->bindTexture(TEXTURE_2D,0);

	return True;
}

// load a filename in video card and preserved the picture
Bool Texture2D::create(Bool mipmaps, Bool unloadPicture)
{
	// if the picture data is no longer in memory we try to load it before
	if (m_picture.isEmpty())
	{
        if (getResourceName().startsWith("<"))
			return False;

		// load the picture
        if (!m_picture.load(getResourceName()))
			return False;
	}

	Bool create = generateId(m_picture, mipmaps);

	getScene()->getContext()->bindTexture(TEXTURE_2D, m_textureId, True);

	m_updateFlags = UPDATE_ALL;

	setFilteringMode();
    setWrapMode();

	UInt32 dbgSize = 0;

	// pixel transfer / depth scale and bias with texture operation... TODO

	// send the picture to opengl texture
	dbgSize = GLTexture::loadTexture2D(getScene()->getContext(), m_picture, mipmaps);

	// unbind the texture
	getScene()->getContext()->bindTexture(TEXTURE_2D, 0);

	// debug info
	if (create)
		O3D_GALLOC(MemoryManager::GPU_TEXTURE_2D, m_textureId, dbgSize);
	else
		O3D_GREALLOC(MemoryManager::GPU_TEXTURE_2D, m_textureId, dbgSize);

	// are we keep the picture
	if (unloadPicture)
		m_picture.destroy();

	return True;
}

// Update the texture data. The texture data must have the same size and format as
Bool Texture2D::update(const void *buffer, PixelFormat bufferFormat, Bool dontUnbind)
{
	if (m_textureId == O3D_UNDEFINED)
		return False;

	Bool mipmaps = getNumMipMaps() > 1;

	getScene()->getContext()->bindTexture(TEXTURE_2D, m_textureId, True);

	setFilteringMode();
    setWrapMode();

	GLenum format = GLTexture::getGLFormat(getScene()->getRenderer(), bufferFormat);
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
		glCompressedTexSubImage2D(TEXTURE_2D, 0, 0, 0 , m_width, m_height, format, size, buffer);
	}
	else
		glTexSubImage2D(TEXTURE_2D, 0, 0, 0 , m_width, m_height, format, type, buffer);

	// build mipmaps levels
	if (mipmaps)
		glGenerateMipmap(TEXTURE_2D);

	// unbind the texture
	if (!dontUnbind)
		getScene()->getContext()->bindTexture(TEXTURE_2D, 0);

	return True;
}

void Texture2D::resize(UInt32 width, UInt32 height, Bool dontUnbind)
{
	if (m_textureId == 0)
        O3D_ERROR(E_InvalidOperation("Texture2D must exists before to resize"));

	// same size
	if ((width == m_width) && (height == m_height))
		return;

	GLenum intFormat = GLTexture::getGLInternalFormat(getScene()->getRenderer(), m_pixelFormat);
	UInt32 format = GLTexture::getGLFormat(getScene()->getRenderer(), m_pixelFormat);
	GLenum type = GLTexture::getGLType(m_pixelFormat);

	getScene()->getContext()->bindTexture(TEXTURE_2D, m_textureId, True);

	glTexImage2D
			(TEXTURE_2D,
			 m_minLevel,
			 intFormat,
			 width,
			 height,
			 0,
			 format,
			 type,
			 NULL);

	m_width = width;
	m_height = height;

	UInt32 internalPixelSize = GLTexture::getInternalPixelSize(
				getScene()->getRenderer(), m_pixelFormat);

	UInt32 dbgSize = (m_width * m_height * internalPixelSize) >> 3;
	O3D_GREALLOC(MemoryManager::GPU_TEXTURE_2D, m_textureId, dbgSize);

	if (isMipMaps())
		glGenerateMipmap(TEXTURE_2D);

	// unbind the texture
	if (!dontUnbind)
		getScene()->getContext()->bindTexture(TEXTURE_2D, 0);
}

Bool Texture2D::addMipsLevels(UInt32 minLevel, UInt32 maxLevel)
{
	if (m_textureId != O3D_UNDEFINED)
	{
		getScene()->getContext()->bindTexture(TEXTURE_2D, m_textureId);

		glGenerateMipmap(TEXTURE_2D);

		getScene()->getContext()->bindTexture(TEXTURE_2D,0);

		m_minLevel = 0;
		m_maxLevel = (UInt32)o3d::log2(o3d::max(m_width, m_height));

		return True;
	}
	else
		return False;
}

// bind the texture to the hardware
Bool Texture2D::bind()
{
	if (isValid())
	{
		getScene()->getContext()->bindTexture(TEXTURE_2D, m_textureId);

		setFilteringMode();
        setWrapMode();
		setAnisotropyLevel();

		return True;
	}
	return False;
}

// unbind the texture in OpenGL context
void Texture2D::unbind()
{
	getScene()->getContext()->bindTexture(TEXTURE_2D, 0, True);
}

// set the texture parameter, such as enable extra coord for cubemap...
void Texture2D::set()
{
}

// unset the texture parameter
void Texture2D::unSet()
{
}

void Texture2D::setImage(Image &image)
{
    m_picture = image;
}

// clear all memory
void Texture2D::destroy()
{
	m_picture.destroy();
	Texture::destroy();
}

// Unload the associated picture only
void Texture2D::unloadImage()
{
	m_picture.destroy();
}

// serialization
Bool Texture2D::writeToFile(OutStream &os)
{
    if (!Texture::writeToFile(os))
		return False;

	return True;
}

Bool Texture2D::readFromFile(InStream &is)
{
	destroy();

    if (!Texture::readFromFile(is))
		return False;

	return True;
}

// Default constructor.
Texture2DTask::Texture2DTask(
		Texture2D *texture,
		const String &filename,
		Bool mipmaps) :
			m_texture(texture),
			m_mipmaps(mipmaps)
{
	if (!texture)
		O3D_ERROR(E_InvalidParameter("The texture must be valid"));

	m_filename = FileManager::instance()->getFullFileName(filename);
}

Bool Texture2DTask::execute()
{
	O3D_ASSERT(m_filename.isValid());
	if (m_filename.isValid())
	{
		return m_picture.load(m_filename);
	}
	else
		return False;
}

Bool Texture2DTask::finalize()
{
	if (m_picture.isValid())
	{
		if (!m_texture->getScene()->getRenderer()->isCurrent())
			m_texture->getScene()->getRenderer()->setCurrent();

        m_texture->setImage(m_picture);
		return m_texture->create(m_mipmaps);
	}
	else
		return False;
}

