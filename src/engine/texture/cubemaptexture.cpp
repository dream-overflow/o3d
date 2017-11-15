/**
 * @file cubemaptexture.cpp
 * @brief Implementation of CubeMapTexture.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-04-26
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/texture/cubemaptexture.h"

#include "o3d/core/filemanager.h"

#include "o3d/engine/glextdefines.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/texture/gltexture.h"
#include "o3d/engine/context.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/renderer.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(CubeMapTexture, ENGINE_TEXTURE_CUBEMAP, Texture)

String CubeMapTexture::makeResourceName(
		const String &xp, const String &xn,
		const String &yp, const String &yn,
		const String &zp, const String &zn)
{
	return String("<cubemap(") <<
			xp << "|" << xn << "|" <<
			yp << "|" << yn << "|" <<
			zp << "|" << zn <<  ")>";
}

// Construct using a cube-map picture filename.
CubeMapTexture::CubeMapTexture(
	BaseObject *parent,
	const TextureOps &operations) :
		Texture(parent, operations)
{
	m_textureType = TEXTURE_CUBE_MAP;
}

// Construct using a generated and valid cube-map picture.
CubeMapTexture::CubeMapTexture(
	BaseObject *parent,
	const Image &picture,
	const TextureOps &operations) :
		Texture(parent, operations)
{
	m_textureType = TEXTURE_CUBE_MAP;

	if (picture.isEmpty())
		O3D_ERROR(E_InvalidParameter("Picture must be valid"));

	if (!picture.isCubeMap())
		O3D_ERROR(E_InvalidParameter("Picture must be a cube-map"));
}

// Construct using 6 generated and valid pictures.
CubeMapTexture::CubeMapTexture(
	BaseObject *parent,
	const Image &xp, const Image &xn,
	const Image &yp, const Image &yn,
	const Image &zp, const Image &zn,
	const TextureOps &operations) :
		Texture(parent, operations)
{
	m_textureType = TEXTURE_CUBE_MAP;

	m_pictures[0] = xp;
	m_pictures[1] = xn;
	m_pictures[2] = yp;
	m_pictures[3] = yn;
	m_pictures[4] = zp;
	m_pictures[5] = zn;

	UInt32 width = m_pictures[0].getWidth();
	UInt32 height = m_pictures[0].getHeight();
	PixelFormat pixelFormat = m_pictures[0].getPixelFormat();

	for (UInt32 i = 0; i < 6; ++i)
	{
		if (m_pictures[i].isEmpty())
			O3D_ERROR(E_InvalidParameter("Picture must be valid"));

		if ((m_pictures[i].getWidth() != width) || (m_pictures[i].getHeight() != height) ||
			(m_pictures[i].getPixelFormat() != pixelFormat))
			O3D_ERROR(E_InvalidParameter("Pictures must have the same sizes and formats"));
	}
}

CubeMapTexture::~CubeMapTexture()
{
}

void CubeMapTexture::checkPictures(Bool six)
{
	if (six)
	{
		for (UInt32 i = 1; i < 6; ++i)
		{
			if ((m_pictures[i].getPixelFormat() != m_pictures[i - 1].getPixelFormat()) ||
				(m_pictures[i].getWidth() != m_pictures[i - 1].getWidth()) ||
				(m_pictures[i].getHeight() != m_pictures[i - 1].getHeight()))
			{
				O3D_ERROR(E_InvalidFormat("Pictures must have the same size and format"));
			}
		}
	}
	else
	{
		if (!m_pictures[0].isCubeMap())
			O3D_ERROR(E_InvalidFormat("Picture must be a cube-map"));
	}
}

// Create the texture using the previously loaded picture or the filename
Bool CubeMapTexture::create(Bool mipmaps, Bool unloadImg)
{
	// if the picture data is no longer in memory we try to load it before
	if (m_pictures[0].isEmpty())
	{
        if (getResourceName().startsWith("<"))
			return False;

        // load picture
        if (!loadPicture())
			return False;
	}

	Bool create = generateId(m_pictures[0], mipmaps);
	UInt32 size = 0;

	getScene()->getContext()->bindTexture(TEXTURE_CUBE_MAP, m_textureId, True);

	m_updateFlags = UPDATE_ALL;

	setFilteringMode();
	setWrapMode();

	if (m_pictures[0].isCubeMap())
		size = GLTexture::loadTextureCubeMap(getScene()->getContext(), m_pictures[0], mipmaps);
	else
		size = GLTexture::loadTextureCubeMap(getScene()->getContext(), m_pictures, mipmaps);

	// unbind the texture
	getScene()->getContext()->bindTexture(TEXTURE_CUBE_MAP, 0);

	// debug info
	if (create)
		O3D_GALLOC(MemoryManager::GPU_TEXTURE_CUBEMAP, m_textureId, size);
	else
		O3D_GREALLOC(MemoryManager::GPU_TEXTURE_CUBEMAP, m_textureId, size);

	// are we keep the picture ?
	if (unloadImg)
		unloadImage();

	return True;
}

// Load all pictures.
Bool CubeMapTexture::loadPicture()
{
	// XP
    if (!m_pictures[LEFT_SIDE].load(getResourceName()))
		return False;

	checkPictures(True);

	return True;
}

// Apply texture filtering mode.
void CubeMapTexture::setFilteringMode()
{
	if ((m_updateFlags & UPDATE_FILTERING) != UPDATE_FILTERING)
			return;

	m_updateFlags ^= UPDATE_FILTERING;

	if ((m_updateFlags & UPDATE_ANISOTROPY) == UPDATE_ANISOTROPY)
		m_updateFlags ^= UPDATE_ANISOTROPY;

	switch (m_filtering)
	{
		case Texture::NO_FILTERING:
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
			break;

		case Texture::LINEAR_FILTERING:
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
			break;

		case Texture::BILINEAR_FILTERING:
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
			break;

		case Texture::BILINEAR_ANISOTROPIC:
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_anisotropy);
			break;

		case Texture::TRILINEAR_FILTERING:
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
			break;

		case Texture::TRILINEAR_ANISOTROPIC:
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_anisotropy);
			break;

		default: // linear
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
			break;
	}
}

// Apply texture warp mode.
void CubeMapTexture::setWrapMode()
{
	if ((m_updateFlags & UPDATE_WRAPMODE) != UPDATE_WRAPMODE)
		return;

	m_updateFlags ^= UPDATE_WRAPMODE;

	switch (m_wrap)
	{
		case Texture::REPEAT:
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
			break;

		case Texture::CLAMP:
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			break;

		default: // repeat
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
			break;
	}
}

// Apply texture anisotropy level.
void CubeMapTexture::setAnisotropyLevel()
{
	if ((m_updateFlags & UPDATE_ANISOTROPY) != UPDATE_ANISOTROPY)
			return;

	m_updateFlags ^= UPDATE_ANISOTROPY;

	switch (m_filtering)
	{
		case Texture::NO_FILTERING:
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
			break;

		case Texture::LINEAR_FILTERING:
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
			break;

		case Texture::BILINEAR_FILTERING:
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
			break;

		case Texture::BILINEAR_ANISOTROPIC:
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_anisotropy);
			break;

		case Texture::TRILINEAR_FILTERING:
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
			break;

		case Texture::TRILINEAR_ANISOTROPIC:
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_anisotropy);
			break;

		default: // linear
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
			break;
	}
}

// clear all memory
void CubeMapTexture::destroy()
{
	unloadImage();
	Texture::destroy();
}

// Unload the associated picture only
void CubeMapTexture::unloadImage()
{
	for (UInt32 i = 0; i < 6; ++i)
		m_pictures[i].destroy();
}

// set the texture parameter, such as enable extra coord for cubemap...
void CubeMapTexture::set()
{
}

// unset the texture parameter
void CubeMapTexture::unSet()
{
}

// bind the texture and parameters to OpenGL
Bool CubeMapTexture::bind()
{
	if (isValid())
	{
		getScene()->getContext()->bindTexture(TEXTURE_CUBE_MAP, m_textureId);

		setFilteringMode();
		setWrapMode();
		setAnisotropyLevel();

		return True;
	}
	return False;
}

// unbind the texture in OpenGL context
void CubeMapTexture::unbind()
{
	getScene()->getContext()->bindTexture(TEXTURE_CUBE_MAP, 0, True);
}

// serialization
Bool CubeMapTexture::writeToFile(OutStream &os)
{
    if (!Texture::writeToFile(os))
		return False;

	return True;
}

Bool CubeMapTexture::readFromFile(InStream &is)
{
    if (!Texture::readFromFile(is))
		return False;

	return True;
}

// Default constructor.
CubeMapTextureTask::CubeMapTextureTask(
		CubeMapTexture *texture,
		const String &filename,
		Bool mipmaps) :
			m_texture(texture),
			m_mipmaps(mipmaps)
{
	if (!texture)
		O3D_ERROR(E_InvalidParameter("The texture must be valid"));

	m_filenames[0] = FileManager::instance()->getFullFileName(filename);
}

CubeMapTextureTask::CubeMapTextureTask(
		CubeMapTexture *texture,
		const String &xp,
		const String &xn,
		const String &yp,
		const String &yn,
		const String &zp,
		const String &zn,
		Bool mipmaps)
{
	if (!texture)
		O3D_ERROR(E_InvalidParameter("The texture must be valid"));

	m_filenames[0] = FileManager::instance()->getFullFileName(xp);
	m_filenames[1] = FileManager::instance()->getFullFileName(xn);
	m_filenames[2] = FileManager::instance()->getFullFileName(yp);
	m_filenames[3] = FileManager::instance()->getFullFileName(yn);
	m_filenames[4] = FileManager::instance()->getFullFileName(zp);
	m_filenames[5] = FileManager::instance()->getFullFileName(zn);
}

Bool CubeMapTextureTask::execute()
{
	for (Int32 i = 0; i < 6; ++i)
	{
		if (m_filenames[i].isValid())
		{
			if (!m_pictures[i].load(m_filenames[i]))
				return False;
		}
	}

	return True;
}

Bool CubeMapTextureTask::finalize()
{
	for (Int32 i = 0; i < 6; ++i)
	{
		if (m_pictures[i].isValid())
			m_texture->getImage(CubeMapTexture::CubeSide(i)) = m_pictures[i];
		else
			return False;
	}

	if (!m_texture->getScene()->getRenderer()->isCurrent())
		m_texture->getScene()->getRenderer()->setCurrent();

	return m_texture->create(m_mipmaps);
}

