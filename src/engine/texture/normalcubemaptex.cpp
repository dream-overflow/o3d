/**
 * @file normalcubemaptex.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-04-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/texture/normalcubemaptex.h"

#include "o3d/engine/glextdefines.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/context.h"
#include "o3d/engine/texture/gltexture.h"

#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(NormalCubeMapTex, ENGINE_TEXTURE_NORMCUBEMAP, Texture)

// Make a key name.
String NormalCubeMapTex::makeResourceName(UInt32 size, Float offset)
{
	return String::print(
			"<normalcubemap(%u,%.4f)>",
			size,
			offset);
}

// Constructor/desctructor
NormalCubeMapTex::NormalCubeMapTex(
	BaseObject *parent,
	UInt32 size,
	Float offset) :
		Texture(parent),
		m_size(size),
		m_offset(offset)
{
	m_textureType = TEXTURE_CUBE_MAP;

	m_filtering = Texture::LINEAR_FILTERING;
	m_wrap = Texture::CLAMP;
}

NormalCubeMapTex::~NormalCubeMapTex()
{
}

// set the warp mode to OpenGL
void NormalCubeMapTex::setWrapMode()
{
	if ((m_updateFlags & UPDATE_WRAPMODE) != UPDATE_WRAPMODE)
		return;

	m_updateFlags ^= UPDATE_WRAPMODE;

	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
}

// set le mode de filtering pour le cubemap
void NormalCubeMapTex::setFilteringMode()
{
	if ((m_updateFlags & UPDATE_FILTERING) != UPDATE_FILTERING)
			return;

	m_updateFlags ^= UPDATE_FILTERING;

	if ((m_updateFlags & UPDATE_ANISOTROPY) == UPDATE_ANISOTROPY)
		m_updateFlags ^= UPDATE_ANISOTROPY;

	switch (m_filtering)
	{
	case Texture::LINEAR_FILTERING:
		glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;

	default:	// linear
		glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
		break;
	}
}

// Apply texture anisotropy level.
void NormalCubeMapTex::setAnisotropyLevel()
{
	if ((m_updateFlags & UPDATE_ANISOTROPY) != UPDATE_ANISOTROPY)
			return;

	m_updateFlags ^= UPDATE_ANISOTROPY;

	switch (m_filtering)
	{
		case Texture::LINEAR_FILTERING:
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
			break;

		default: // linear
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
			break;
	}
}

// Create the cube map
Bool NormalCubeMapTex::create()
{
	UInt32 maxLevel = 0;

//	if (m_filtering > 1 && m_filtering < 6)
//		maxLevel = o3d::log2(o3d::max(width,height));

	Bool create = generateId(PF_RGBA_U8, m_size, m_size, 1, 0, maxLevel);

	if (m_filtering != 1)
		O3D_ERROR(E_InvalidParameter("Unsupported filtering mode"));

	if (m_wrap != Texture::CLAMP)
		O3D_ERROR(E_InvalidParameter("Unsupported warp mode"));

	getScene()->getContext()->bindTexture(TEXTURE_CUBE_MAP, m_textureId, True);

	m_updateFlags = UPDATE_ALL;

	setFilteringMode();
	setWrapMode();

	UInt8* data = new UInt8[m_size*m_size*4];

	Float halfSize = (Float)m_size * 0.5f;
	Vector3 v;

	UInt32 bytePtr = 0;

	if (((m_size*m_size*4) % 4) == 0)
		glPixelStorei(GL_UNPACK_ALIGNMENT,4);
	else
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	if (m_filtering > 1 && m_filtering < 6)
	{
		if (create)
			O3D_GALLOC(MemoryManager::GPU_TEXTURE_CUBEMAP, m_textureId,
				6*GLTexture::computePixMapSize(32,m_size,m_size));
			else
			O3D_GREALLOC(MemoryManager::GPU_TEXTURE_CUBEMAP, m_textureId,
				6*GLTexture::computePixMapSize(32,m_size,m_size));
	}
	else
	{
		if (create)
			O3D_GALLOC(MemoryManager::GPU_TEXTURE_CUBEMAP, m_textureId, 6*m_size*m_size*4);
		else
			O3D_GREALLOC(MemoryManager::GPU_TEXTURE_CUBEMAP, m_textureId, 6*m_size*m_size*4);
	}

	// positive X cubeface
	for (UInt32 j = 0 ; j < m_size ; j++)
	{
		for (UInt32 i = 0 ; i < m_size ; i++)
		{
			v[X] = halfSize;
			v[Y] = -((Float)j + m_offset - halfSize);
			v[Z] = -((Float)i + m_offset - halfSize);
			v.normalize();

			data[bytePtr]   = (UInt8)((v[X] * 127.f) + 127.f);
			data[bytePtr+1] = (UInt8)((v[Y] * 127.f) + 127.f);
			data[bytePtr+2] = (UInt8)((v[Z] * 127.f) + 127.f);
			data[bytePtr+3] = 255;

			bytePtr += 4;
		}
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,0,GL_RGBA8,m_size,m_size,0,GL_RGBA,GL_UNSIGNED_BYTE,data);

	// negative X cubeface
	bytePtr = 0;
	for(UInt32 j = 0 ; j < m_size ; j++)
	{
		for(UInt32 i = 0 ; i < m_size ; i++)
		{
			v[X] = -halfSize;
			v[Y] = -((Float)j + m_offset - halfSize);
			v[Z] =  ((Float)i + m_offset - halfSize);
			v.normalize();

			data[bytePtr]   = (UInt8)((v[X] * 127.f) + 127.f);
			data[bytePtr+1] = (UInt8)((v[Y] * 127.f) + 127.f);
			data[bytePtr+2] = (UInt8)((v[Z] * 127.f) + 127.f);
			data[bytePtr+3] = 255;

			bytePtr+=4;
		}
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,0,GL_RGBA8,m_size,m_size,0,GL_RGBA,GL_UNSIGNED_BYTE,data);

	// positive Y cubeface
	bytePtr = 0;
	for(UInt32 j = 0 ; j < m_size ; j++)
	{
		for(UInt32 i = 0 ; i < m_size ; i++)
		{
			v[X] = (Float)i + m_offset - halfSize;
			v[Y] = halfSize;
			v[Z] = (Float)j + m_offset - halfSize;
			v.normalize();

			data[bytePtr]   = (UInt8)((v[X] * 127.f) + 127.f);
			data[bytePtr+1] = (UInt8)((v[Y] * 127.f) + 127.f);
			data[bytePtr+2] = (UInt8)((v[Z] * 127.f) + 127.f);
			data[bytePtr+3] = 255;

			bytePtr+=4;
		}
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,0,GL_RGBA8,m_size,m_size,0,GL_RGBA,GL_UNSIGNED_BYTE,data);

	// negative Y cubeface
	bytePtr = 0;
	for(UInt32 j = 0 ; j < m_size ; j++)
	{
		for(UInt32 i = 0 ; i < m_size ; i++)
		{
			v[X] =  ((Float)i + m_offset - halfSize);
			v[Y] = -halfSize;
			v[Z] = -((Float)j + m_offset - halfSize);
			v.normalize();

			data[bytePtr]   = (UInt8)((v[X] * 127.f) + 127.f);
			data[bytePtr+1] = (UInt8)((v[Y] * 127.f) + 127.f);
			data[bytePtr+2] = (UInt8)((v[Z] * 127.f) + 127.f);
			data[bytePtr+3] = 255;

			bytePtr+=4;
		}
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,0,GL_RGBA8,m_size,m_size,0,GL_RGBA,GL_UNSIGNED_BYTE,data);

	// positive Z cubeface
	bytePtr = 0;
	for(UInt32 j = 0 ; j < m_size ; j++)
	{
		for(UInt32 i = 0 ; i < m_size ; i++)
		{
			v[X] =  ((Float)i + m_offset - halfSize);
			v[Y] = -((Float)j + m_offset - halfSize);
			v[Z] = halfSize;
			v.normalize();

			data[bytePtr]   = (UInt8)((v[X] * 127.f) + 127.f);
			data[bytePtr+1] = (UInt8)((v[Y] * 127.f) + 127.f);
			data[bytePtr+2] = (UInt8)((v[Z] * 127.f) + 127.f);
			data[bytePtr+3] = 255;

			bytePtr+=4;
		}
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,0,GL_RGBA8,m_size,m_size,0,GL_RGBA,GL_UNSIGNED_BYTE,data);

	// negative Z cubeface
	bytePtr = 0;
	for(UInt32 j = 0 ; j < m_size ; j++)
	{
		for(UInt32 i = 0 ; i < m_size ; i++)
		{
			v[X] = -((Float)i + m_offset - halfSize);
			v[Y] = -((Float)j + m_offset - halfSize);
			v[Z] = -halfSize;
			v.normalize();

			data[bytePtr]   = (UInt8)((v[X] * 127.f) + 127.f);
			data[bytePtr+1] = (UInt8)((v[Y] * 127.f) + 127.f);
			data[bytePtr+2] = (UInt8)((v[Z] * 127.f) + 127.f);
			data[bytePtr+3] = 255;

			bytePtr+=4;
		}
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,0,GL_RGBA8,m_size,m_size,0,GL_RGBA,GL_UNSIGNED_BYTE,data);

	// generates mipmaps if necessary
	if (m_filtering > 1 && m_filtering < 6)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// unbind the texture
	getScene()->getContext()->bindTexture(TEXTURE_CUBE_MAP,0);

	deleteArray(data);

	return True;
}

// Delete the texture
void NormalCubeMapTex::destroy()
{
	Texture::destroy();
}

// set the texture parameter, such as enable extra coord for cubemap...
void NormalCubeMapTex::set()
{
}

// unset the texture parameter
void NormalCubeMapTex::unSet()
{
}

// bind this texture to the current texture unit
Bool NormalCubeMapTex::bind()
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

// unbound the texture in OpenGL context
void NormalCubeMapTex::unbind()
{
	getScene()->getContext()->bindTexture(TEXTURE_CUBE_MAP, 0, True);
}

// Serialization
Bool NormalCubeMapTex::writeToFile(OutStream &os)
{
    if (!Texture::writeToFile(os))
		return False;

    os   << m_size
	     << m_offset;

	return True;
}

Bool NormalCubeMapTex::readFromFile(InStream &is)
{
    if (!Texture::readFromFile(is))
		return False;

    is   >> m_size
	     >> m_offset;

	return True;
}

