/**
 * @file attenuationtex3d.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-07-31--
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/texture/attenuationtex3d.h"

#include "o3d/engine/glextdefines.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/renderer.h"
#include "o3d/engine/context.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(AttenuationTex3D, ENGINE_TEXTURE_ATTENUATION_3D, Texture)

// Make a key name.
String AttenuationTex3D::makeResourceName(UInt32 size)
{
	return String::print("<attenuation3d(%u)>", size);
}

// Default constructor
AttenuationTex3D::AttenuationTex3D(BaseObject *parent, UInt32 size) :
	Texture(parent),
	m_size(size)
{
	m_textureType = TEXTURE_3D;

	m_filtering = Texture::LINEAR_FILTERING;
	m_wrap = Texture::CLAMP;
}

AttenuationTex3D::~AttenuationTex3D()
{
}

// set the warp mode to OpenGL
void AttenuationTex3D::setWrapMode()
{
    if ((m_updateFlags & UPDATE_WRAPMODE) != UPDATE_WRAPMODE) {
		return;
    }

	m_updateFlags ^= UPDATE_WRAPMODE;

    switch (m_wrap) {
        case Texture::CLAMP:
            glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
            break;

        default:  // clamp
            glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
            break;
	}
}

// set le mode de filtering pour le cube-map
void AttenuationTex3D::setFilteringMode()
{
    if ((m_updateFlags & UPDATE_FILTERING) != UPDATE_FILTERING) {
        return;
    }

	m_updateFlags ^= UPDATE_FILTERING;

    if ((m_updateFlags & UPDATE_ANISOTROPY) == UPDATE_ANISOTROPY) {
		m_updateFlags ^= UPDATE_ANISOTROPY;
    }

    switch (m_filtering) {
        case Texture::LINEAR_FILTERING:
            glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameterf(GL_TEXTURE_3D,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
            break;

        default:	// linear
            glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameterf(GL_TEXTURE_3D,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
            break;
	}
}

// Apply texture anisotropy level.
void AttenuationTex3D::setAnisotropyLevel()
{
    if ((m_updateFlags & UPDATE_ANISOTROPY) != UPDATE_ANISOTROPY) {
        return;
    }

	m_updateFlags ^= UPDATE_ANISOTROPY;

    switch (m_filtering) {
		case Texture::LINEAR_FILTERING:
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
			break;

		default: // linear
			glTexParameterf(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
			break;
	}
}

// Create the cube map
Bool AttenuationTex3D::create()
{
	Bool create = generateId(PF_RED_U8, m_size, m_size, m_size, 0, 0);

	getScene()->getContext()->bindTexture(TEXTURE_3D, m_textureId, True);

	m_updateFlags = UPDATE_ALL;

	setFilteringMode();
	setWrapMode();
	setAnisotropyLevel();

    if (m_filtering != 1) {
		O3D_ERROR(E_InvalidParameter("Unsupported filtering mode"));
    }

    if (m_wrap != Texture::CLAMP) {
		O3D_ERROR(E_InvalidParameter("Unsupported warp mode"));
    }

	UInt8* data1d = new UInt8[m_size];
	UInt8* data3d = new UInt8[m_size*m_size*m_size];

	UInt32 i,j,k;
	Float halfSize = (Float)m_size * 0.5f - 0.5f;
	Float invSize  = 1.f / halfSize;

	// Attenuation 3D Texture Generation
    for (i = 0 ; i < m_size ; ++i) {
		// Get distance from center to this point
		Float dist = (Float)i;
		dist -= halfSize;
		dist *= invSize;

		// Square and Clamp to [0..1]
		dist = dist * dist;
		if (dist > 1.0f)
			dist = 1.0f;

		data1d[i] = UInt8(dist * 255);
	}

	// Make sure the color is 255 at the edges
	data1d[0] = 255;
	data1d[m_size - 1] = 255;

	int currentByte = 0;
	UInt8 dataI, dataJ, dataK;

    for (i = 0 ; i < m_size ; i++) {
		dataI = data1d[i];

        for (j = 0 ; j < m_size ; j++) {
			dataJ = data1d[j];

            for (k = 0; k < m_size; k++) {
				dataK = data1d[k];

				UInt32 newData = dataI + dataJ + dataK;
				if (newData > 255) newData = 255;

				// Invert data as there is no need to do any more summing
				// Thus the invert need not wait until the register combiners
				newData = 255 - newData;

				data3d[currentByte] = (UInt8)newData;
				currentByte++;
			}
		}
	}

	deleteArray(data1d);

    if (((m_size*m_size*m_size) % 4) == 0) {
		glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    } else {
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    }

	glTexImage3D(
		GL_TEXTURE_3D,
		0,
		GL_R8,
		m_size,
		m_size,
		m_size,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		data3d);

	// unbind the texture
	getScene()->getContext()->bindTexture(TEXTURE_3D,0);

	Int32 size = m_size*m_size*m_size;

    if (create) {
		O3D_GALLOC(MemoryManager::GPU_TEXTURE_3D, m_textureId, size);
    } else {
		O3D_GREALLOC(MemoryManager::GPU_TEXTURE_3D, m_textureId, size);
    }

	deleteArray(data3d);

	return True;
}

// Delete the texture
void AttenuationTex3D::destroy()
{
	Texture::destroy();
}

// set the texture parameter, such as enable extra coord for cubemap...
void AttenuationTex3D::set()
{
}

// unset the texture parameter
void AttenuationTex3D::unSet()
{
}

// bind this texture to the current texture unit
Bool AttenuationTex3D::bind()
{
    if (isValid()) {
		getScene()->getContext()->bindTexture(TEXTURE_3D, m_textureId);

		setFilteringMode();
		setWrapMode();
		setAnisotropyLevel();

		return True;
	}
	return False;
}

// unbind the texture to the current texture unit
void AttenuationTex3D::unbind()
{
	getScene()->getContext()->bindTexture(TEXTURE_3D, 0, True);
}

// Serialization
Bool AttenuationTex3D::writeToFile(OutStream &os)
{
    if (!Texture::writeToFile(os)) {
		return False;
    }

    os << m_size;

	return True;
}

Bool AttenuationTex3D::readFromFile(InStream &is)
{
    if (!Texture::readFromFile(is)) {
		return False;
    }

    is >> m_size;

	return True;
}
