/**
 * @file texture.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"

#include "o3d/engine/glextdefines.h"
#include "o3d/engine/glextensionmanager.h"

#include "o3d/engine/texture/texture.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/renderer.h"
#include "o3d/engine/context.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(Texture, ENGINE_TEXTURE, SceneResource)

// Default constructor
Texture::Texture(BaseObject *parent, const TextureOps &operations) :
	SceneResource(parent),
	m_textureId(0),
	m_textureType(TEXTURE_2D),
	m_width(0),
	m_height(0),
	m_depth(0),
	m_pixelFormat(PF_RGBA_U8),
	m_minLevel(0),
	m_maxLevel(0),
	m_filtering(LINEAR_FILTERING),
    m_wrap(REPEAT),
	m_anisotropy(1.0f),
	m_operations(operations),
	m_updateFlags(UPDATE_ALL)
{
}

// Destructor
Texture::~Texture()
{
	destroy();
}

// clear all memory
void Texture::destroy()
{
	unload();
}

// delete the OpenGL texture memory
void Texture::unload()
{
	if (m_textureId != 0)
	{
		switch (m_textureType)
		{
			case TEXTURE_1D:
				O3D_GFREE(MemoryManager::GPU_TEXTURE_1D, m_textureId);
				break;

			case TEXTURE_2D:
				O3D_GFREE(MemoryManager::GPU_TEXTURE_2D, m_textureId);
				break;

			case TEXTURE_3D:
				O3D_GFREE(MemoryManager::GPU_TEXTURE_3D, m_textureId);
				break;

			case TEXTURE_CUBE_MAP:
				O3D_GFREE(MemoryManager::GPU_TEXTURE_CUBEMAP, m_textureId);
				break;

			case TEXTURE_1D_ARRAY:
				O3D_GFREE(MemoryManager::GPU_TEXTURE_1D_ARRAY, m_textureId);
				break;

			case TEXTURE_2D_ARRAY:
				O3D_GFREE(MemoryManager::GPU_TEXTURE_2D_ARRAY, m_textureId);
				break;

			default:
				break;
		}

		getScene()->getContext()->deleteTexture(m_textureId);
		m_textureId = 0;
	}

	m_updateFlags = UPDATE_ALL;
}

Bool Texture::addMipsLevels(UInt32, UInt32)
{
    return False;
}

// Define the texture parameters.
Bool Texture::generateId(
	PixelFormat pixelFormat,
	UInt32 width,
	UInt32 height,
	UInt32 depth,
	UInt32 minLevel,
	UInt32 maxLevel)
{
	Bool created = False;

	if (m_textureId == O3D_UNDEFINED)
	{
		glGenTextures(1, (GLuint*)&m_textureId);
		created = True;
	}
	else
	{
		// recreate the texture only if the max level is lesser than the new max level,
		// or if the min level is greater then the new min level.
		if ((maxLevel < m_maxLevel) && (minLevel > m_minLevel))
		{
			unload();

			glGenTextures(1, (GLuint*)&m_textureId);
			created = True;
		}
	}

	m_pixelFormat = pixelFormat;
	m_width = width;
	m_height = height;
	m_depth = depth;
	m_minLevel = minLevel;
	m_maxLevel = maxLevel;

	return created;
}

Bool Texture::generateId(const Image &pic, Bool mipmaps)
{
	UInt32 maxLevel = 0;

	// get the number of the lower mipmap level
	if (pic.getNumMipMapLvl())
		maxLevel = pic.getNumMipMapLvl();
	else if (mipmaps)
		maxLevel = (UInt32)o3d::log2(o3d::max(pic.getWidth(), pic.getHeight()));

	return generateId(
			pic.getPixelFormat(),
			pic.getWidth(),
			pic.getHeight(),
			pic.getNumDepthLayer(),
			0,
			maxLevel);
}

// bind the texture in OpenGL context
Bool Texture::bind()
{
    return False;
}

// unbind the texture in OpenGL context
void Texture::unbind()
{
}

// Get the OpenGL internal format.
TextureIntFormat Texture::getGLInternalFormat() const
{
	return GLTexture::getGLInternalFormat(getScene()->getRenderer(), m_pixelFormat);
}

// set the texture parameter, such as enable extra coord for cubemap...
void Texture::set()
{
}

// unset the texture parameter
void Texture::unSet()
{
}

// Serialization
Bool Texture::writeToFile(OutStream &os)
{
    BaseObject::writeToFile(os);

    if (m_resourceName.isEmpty())
        O3D_ERROR(E_InvalidParameter("Texture resource name must be valid"));

    os   	<< m_pixelFormat
			<< m_filtering
            << m_wrap
			<< m_anisotropy
			<< m_resourceName;
//			<< m_operations;

	return True;
}

Bool Texture::readFromFile(InStream &is)
{
    BaseObject::readFromFile(is);

    is    	>> m_pixelFormat
			>> m_filtering
            >> m_wrap
			>> m_anisotropy
			>> m_resourceName;
	//		>> m_operations;

	m_updateFlags = UPDATE_ALL;

	return True;
}
