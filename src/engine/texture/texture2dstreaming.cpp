/**
 * @file texture2dstreaming.cpp
 * @brief Implementation of TextureStreaming.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-01-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"

#include "o3d/engine/glextdefines.h"
#include "o3d/engine/glextensionmanager.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/texture/texture2dstreaming.h"
#include "o3d/engine/context.h"

using namespace o3d;

Texture2DStreaming::Texture2DStreaming(BaseObject *parent) :
	m_parent(parent),
	m_context(NULL),
	m_state(False),
	m_level(0),
	m_size(0),
	m_currPBO(False),
	m_format(TF_RGBA),
	m_type(DATA_UNSIGNED_BYTE)
{
	if (!m_parent)
		O3D_ERROR(E_NullPointer("Parent must be a valid pointer"));

	if (!o3d::typeOf<Scene>(m_parent->getTopLevelParent()))
		O3D_ERROR(E_InvalidParameter("Top level parent must be the scene"));

	m_context = reinterpret_cast<Scene*>(m_parent->getTopLevelParent())->getContext();

	m_buffersId[0] = 0;
	m_buffersId[1] = 0;
}

// Get the scene parent.
Scene* Texture2DStreaming::getScene()
{
	return reinterpret_cast<Scene*>(m_parent->getTopLevelParent());
}

// Get the scene parent (read only).
const Scene* Texture2DStreaming::getScene() const
{
	return reinterpret_cast<Scene*>(m_parent->getTopLevelParent());
}

void Texture2DStreaming::create(const Box2i &box)
{
	if (m_state)
		release();

	if (!m_texture.isValid())
		O3D_ERROR(E_InvalidPrecondition("Texture must be valid"));

	if ((box.width() == -1) || (box.height() == -1))
	{
		m_box.set(0, 0, m_texture->getWidth(), m_texture->getHeight());
	}
	else
	{
		// clamp to the texture limits
		m_box.x() = box.x() < (Int32)m_texture->getWidth() ? box.x() : 0;
		m_box.y() = box.y() < (Int32)m_texture->getHeight() ? box.y() : 0;

		m_box.width() = box.x() + box.width() <= (Int32)m_texture->getWidth() ? box.x() + box.width() : (Int32)m_texture->getWidth();
		m_box.height() = box.y() + box.height() <=(Int32) m_texture->getHeight() ? box.y() + box.height() : (Int32)m_texture->getHeight();
	}

	m_size = (m_box.width() * m_box.height() * GLTexture::getPixelSize(
                 /* getScene()->getRenderer(),*/ m_texture->getPixelFormat())) >> 3;

	glGenBuffers(2,(GLuint*)m_buffersId);
	m_context->bindPixelUnpackBuffer(m_buffersId[0]);

	O3D_GALLOC(MemoryManager::GPU_PBO, m_buffersId[0], m_size);

	glBufferData(GL_PIXEL_UNPACK_BUFFER, m_size, NULL, PixelBuffer::STREAM_DRAW);

	m_context->bindPixelUnpackBuffer(m_buffersId[1]);

	O3D_GALLOC(MemoryManager::GPU_PBO, m_buffersId[1], m_size);

	glBufferData(GL_PIXEL_UNPACK_BUFFER, m_size, NULL, PixelBuffer::STREAM_DRAW);

	m_context->bindPixelUnpackBuffer(0);

	m_currPBO = False;
	m_state = True;
}

void Texture2DStreaming::release()
{
	if (m_buffersId[0] != 0)
	{
		O3D_GFREE(MemoryManager::GPU_PBO, m_buffersId[0]);

		m_context->deletePixelBuffer(m_buffersId[0]);
		m_buffersId[0] = 0;
	}

	if (m_buffersId[1] != 0)
	{
		O3D_GFREE(MemoryManager::GPU_PBO, m_buffersId[1]);

		m_context->deletePixelBuffer(m_buffersId[1]);
		m_buffersId[1] = 0;
	}

	m_size = 0;
	m_texture = NULL;
	m_state = False;
}

void Texture2DStreaming::setTexture(Texture2D *texture, UInt32 level)
{
	if (m_state)
	{
		if (texture != NULL)
		{
			if ((m_texture->getPixelFormat() != texture->getPixelFormat()) ||
				(m_texture->getWidth() != texture->getWidth()) ||
				(m_texture->getHeight() != texture->getHeight()))
				O3D_ERROR(E_InvalidFormat("Once created, setTexture only allows texture with the same width/height/pixel format"));
		}

		m_texture = texture;
		m_level = o3d::clamp(level, m_texture->getMinLevel(), m_texture->getMaxLevel());
	}
	else
	{
		m_texture = texture;
		m_level = level;

		if (m_texture.isValid())
		{
			m_format = GLTexture::getGLFormat(getScene()->getRenderer(), m_texture->getPixelFormat());
			m_type = GLTexture::getGLType(m_texture->getPixelFormat());

			m_level = o3d::clamp(level, m_texture->getMinLevel(), m_texture->getMaxLevel());
		}
	}
}

void Texture2DStreaming::update(UInt8 *data, UInt32 size)
{
	UInt32 pboA, pboB;

	if (!m_state)
		O3D_ERROR(E_InvalidParameter("Object must be created before"));

	if (size != m_size)
		O3D_ERROR(E_InvalidParameter("Size must be equal to m_size"));

	if (m_currPBO)
	{
		pboA = m_buffersId[1];
		pboB = m_buffersId[0];
		m_currPBO = False;
	}
	else
	{
		pboA = m_buffersId[0];
		pboB = m_buffersId[1];
		m_currPBO = True;
	}

	m_texture->bind();
	m_context->bindPixelUnpackBuffer(pboA);

	// update the content of the texture with one PBO
	glTexSubImage2D(
		GL_TEXTURE_2D,
		m_level,
		m_box.x(),
		m_box.y(),
		m_box.width(),
		m_box.height(),
	    m_format,
	    m_type,
	    NULL);

	m_context->bindPixelUnpackBuffer(pboB);

	UInt8 *src = reinterpret_cast<UInt8*>(
		glMapBuffer(GL_PIXEL_UNPACK_BUFFER, PixelBuffer::WRITE_ONLY));

	// update the content of the other PBO
	memcpy(src, data, size);

	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

	// finally unbind unpack buffer
	m_context->bindPixelUnpackBuffer(0);
	m_texture->unbind();
}

void Texture2DStreaming::update(UInt8 *data, UInt32 size, UInt32 offset)
{
	UInt32 pboA, pboB;

	if (!m_state)
		O3D_ERROR(E_InvalidParameter("Object must be created before"));

	if (size+offset > m_size)
		O3D_ERROR(E_InvalidParameter("Size+offset must be less or equal to m_size"));

	if (m_currPBO)
	{
		pboA = m_buffersId[1];
		pboB = m_buffersId[0];
		m_currPBO = False;
	}
	else
	{
		pboA = m_buffersId[0];
		pboB = m_buffersId[1];
		m_currPBO = True;
	}

	m_texture->bind();
	m_context->bindPixelUnpackBuffer(pboA);

	// update the content of the texture with one PBO
	glTexSubImage2D(
		GL_TEXTURE_2D,
		m_level,
		m_box.x(),
		m_box.y(),
		m_box.width(),
		m_box.height(),
	    m_format,
	    m_type,
	    NULL);

	m_context->bindPixelUnpackBuffer(pboB);

	UInt8 *src = reinterpret_cast<UInt8*>(
		glMapBuffer(GL_PIXEL_UNPACK_BUFFER, PixelBuffer::WRITE_ONLY)) + offset;

	// update the content of the other PBO
	memcpy(src, data, size);

	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

	// finally unbind unpack buffer
	m_context->bindPixelUnpackBuffer(0);
	m_texture->unbind();
}

