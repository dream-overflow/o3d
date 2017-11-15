/**
 * @file texture2dms.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/texture/texture2dms.h"

#include "o3d/core/filemanager.h"

#include "o3d/engine/glextdefines.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/texture/gltexture.h"
#include "o3d/engine/context.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/renderer.h"
#include "o3d/engine/enginetype.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Texture2DMS, ENGINE_TEXTURE_2D_MULTISAMPLE, Texture)

Texture2DMS::Texture2DMS(BaseObject *parent, const TextureOps &operations) :
    Texture(parent, operations)
{
    m_textureType = TEXTURE_2D_MULTISAMPLE;
}

Texture2DMS::~Texture2DMS()
{
}

// set the filtering mode to OpenGL
void Texture2DMS::setFilteringMode()
{
    if ((m_updateFlags & UPDATE_FILTERING) != UPDATE_FILTERING)
            return;

    m_updateFlags ^= UPDATE_FILTERING;

    if ((m_updateFlags & UPDATE_ANISOTROPY) == UPDATE_ANISOTROPY)
        m_updateFlags ^= UPDATE_ANISOTROPY;

    switch (m_filtering)
    {
    case Texture::NO_FILTERING:
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameterf(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
        break;

    case Texture::LINEAR_FILTERING:
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameterf(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
        break;

    case Texture::BILINEAR_FILTERING:
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
        glTexParameterf(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
        break;

    case Texture::BILINEAR_ANISOTROPIC:
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
        glTexParameterf(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_anisotropy);
        break;

    case Texture::TRILINEAR_FILTERING:
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
        break;

    case Texture::TRILINEAR_ANISOTROPIC:
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_anisotropy);
        break;

    default:	// linear
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameterf(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
        break;
    }
}

// set the warp mode to OpenGL
void Texture2DMS::setWrapMode()
{
    if ((m_updateFlags & UPDATE_WRAPMODE) != UPDATE_WRAPMODE)
        return;

    m_updateFlags ^= UPDATE_WRAPMODE;

    switch (m_wrap)
    {
    case Texture::REPEAT:
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_WRAP_T,GL_REPEAT);
        break;

    case Texture::CLAMP:
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        break;

    default:  // repeat
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_WRAP_T,GL_REPEAT);
        break;
    }
}

// Set the anisotropy level to OpenGL.
void Texture2DMS::setAnisotropyLevel()
{
    if ((m_updateFlags & UPDATE_ANISOTROPY) != UPDATE_ANISOTROPY)
            return;

    m_updateFlags ^= UPDATE_ANISOTROPY;

    switch (m_filtering)
    {
    case Texture::NO_FILTERING:
        glTexParameterf(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
        break;

    case Texture::LINEAR_FILTERING:
        glTexParameterf(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
        break;

    case Texture::BILINEAR_FILTERING:
        glTexParameterf(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
        break;

    case Texture::BILINEAR_ANISOTROPIC:
        glTexParameterf(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_anisotropy);
        break;

    case Texture::TRILINEAR_FILTERING:
        glTexParameterf(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
        break;

    case Texture::TRILINEAR_ANISOTROPIC:
        glTexParameterf(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_anisotropy);
        break;

    default:	// linear
        glTexParameterf(TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.f);
        break;
    }
}

// create an empty texture given a size and a format
Bool Texture2DMS::create(
    UInt32 samples,
    UInt32 width,
    UInt32 height,
    PixelFormat textureFormat,
    Bool dontUnbind)
{
    UInt32 dbgTexSizeInByte = 0;

    if ((Int32)samples > getScene()->getContext()->getTextureMaxSamples())
        O3D_ERROR(E_IndexOutOfRange("Number of samples"));

    GLint internalFormat = GLTexture::getGLInternalFormat(getScene()->getRenderer(), textureFormat);
    UInt32 internalPixelSize = GLTexture::getInternalPixelSize(getScene()->getRenderer(), textureFormat);

    // Create the texture given its parameters
    Bool create = generateId(textureFormat, width, height, 1, 0, 0);

    getScene()->getContext()->bindTexture(TEXTURE_2D_MULTISAMPLE, m_textureId, True);

    m_updateFlags = UPDATE_ALL;

   // setFilteringMode();
//    setWrapMode();

    glTexImage2DMultisample(TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, True);

    O3D_ASSERT(!getScene()->getRenderer()->isError());

    // TODO how to compute that ?
    dbgTexSizeInByte = ((width*height) * internalPixelSize) >> 3;

    if (create)
        O3D_GALLOC(MemoryManager::GPU_TEXTURE_2D_MULTISAMPLE, m_textureId, dbgTexSizeInByte);
    else
        O3D_GREALLOC(MemoryManager::GPU_TEXTURE_2D_MULTISAMPLE, m_textureId, dbgTexSizeInByte);

    // unbind the texture
    if (!dontUnbind)
        getScene()->getContext()->bindTexture(TEXTURE_2D_MULTISAMPLE, 0);

    m_numSamples = samples;

    return True;
}

void Texture2DMS::resize(UInt32 width, UInt32 height, Bool dontUnbind)
{
    if (m_textureId == 0)
        O3D_ERROR(E_InvalidOperation("Texture2DMS must exists before to resize"));

    // same size
    if ((width == m_width) && (height == m_height))
        return;

    GLenum intFormat = GLTexture::getGLInternalFormat(getScene()->getRenderer(), m_pixelFormat);

    getScene()->getContext()->bindTexture(TEXTURE_2D_MULTISAMPLE, m_textureId, True);

    glTexImage2DMultisample(TEXTURE_2D_MULTISAMPLE, m_numSamples, intFormat, width, height, True);

    m_width = width;
    m_height = height;

    UInt32 internalPixelSize = GLTexture::getInternalPixelSize(
                getScene()->getRenderer(), m_pixelFormat);

    UInt32 dbgSize = (m_width * m_height * internalPixelSize) >> 3;
    O3D_GREALLOC(MemoryManager::GPU_TEXTURE_2D_MULTISAMPLE, m_textureId, dbgSize);

    // unbind the texture
    if (!dontUnbind)
        getScene()->getContext()->bindTexture(TEXTURE_2D_MULTISAMPLE, 0);
}

// bind the texture to the hardware
Bool Texture2DMS::bind()
{
    if (isValid())
    {
        getScene()->getContext()->bindTexture(TEXTURE_2D_MULTISAMPLE, m_textureId);

      //  setFilteringMode();
      //  setWrapMode();
      //  setAnisotropyLevel();

        return True;
    }
    return False;
}

// unbind the texture in OpenGL context
void Texture2DMS::unbind()
{
    getScene()->getContext()->bindTexture(TEXTURE_2D_MULTISAMPLE, 0, True);
}

// set the texture parameter, such as enable extra coord for cubemap...
void Texture2DMS::set()
{
}

// unset the texture parameter
void Texture2DMS::unSet()
{
}

// clear all memory
void Texture2DMS::destroy()
{
    Texture::destroy();
}

Bool Texture2DMS::addMipsLevels(UInt32 minLevel, UInt32 maxLevel)
{
    return False;
}

// serialization
Bool Texture2DMS::writeToFile(OutStream &os)
{
    if (!Texture::writeToFile(os))
        return False;

    return True;
}

Bool Texture2DMS::readFromFile(InStream &is)
{
    destroy();

    if (!Texture::readFromFile(is))
        return False;

    return True;
}

