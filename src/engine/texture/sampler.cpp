/**
 * @file sampler.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/texture/sampler.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/glextdefines.h"
#include "o3d/engine/glextensionmanager.h"

using namespace o3d;

Sampler::Sampler(Context *context) :
    m_context(context),
    m_samplerId(0),
    m_filtering{NEAREST_MIPMAP_LINEAR, LINEAR},
    m_wrap{REPEAT, REPEAT, REPEAT},
    m_maxAnisotropy(1.0f),
    m_lodRange{-1000.0f, 1000.0f},
    m_lodBias(0.0f),
    m_compMode(COMP_NONE),
    m_compFunc(COMP_LEQUAL)
{
    O3D_ASSERT(m_context != nullptr);

    // generate sampler
    glGenSamplers(1, &m_samplerId);

    O3D_ASSERT(m_samplerId != 0);
}

Sampler::~Sampler()
{
    release();
}

void Sampler::release()
{
    if (m_samplerId != 0)
        m_context->deleteSampler(m_samplerId);
}

void Sampler::bindSampler(UInt32 unit)
{
    m_context->bindSampler(m_samplerId, unit);
}

void Sampler::unbindSampler(UInt32 unit)
{
    m_context->bindSampler(0, unit);
}

void Sampler::setMignifier(Sampler::Filtering filtering)
{
    m_filtering[0] = filtering;
    glSamplerParameteri(m_samplerId, GL_TEXTURE_MIN_FILTER, m_filtering[0]);
}

void Sampler::setMagnifier(Sampler::Filtering filtering)
{
    m_filtering[1] = filtering;
    glSamplerParameteri(m_samplerId, GL_TEXTURE_MAG_FILTER, m_filtering[1]);
}

void Sampler::setMaxAnisotropy(Float degree)
{
    m_maxAnisotropy = degree;
    glSamplerParameterf(m_samplerId, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_maxAnisotropy);
}

void Sampler::setNearest()
{
    m_filtering[0] = m_filtering[1] = NEAREST;
    m_maxAnisotropy = 1.0f;

    glSamplerParameteri(m_samplerId, GL_TEXTURE_MIN_FILTER, m_filtering[0]);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_MAG_FILTER, m_filtering[1]);
    glSamplerParameterf(m_samplerId, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_maxAnisotropy);
}

void Sampler::setLinear()
{
    m_filtering[0] = m_filtering[1] = LINEAR;
    m_maxAnisotropy = 1.0f;

    glSamplerParameteri(m_samplerId, GL_TEXTURE_MIN_FILTER, m_filtering[0]);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_MAG_FILTER, m_filtering[1]);
    glSamplerParameterf(m_samplerId, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_maxAnisotropy);
}

void Sampler::setBilinear(Float anisotropy)
{
    m_filtering[0] = LINEAR;
    m_filtering[1] = LINEAR_MIPMAP_NEAREST;
    m_maxAnisotropy = anisotropy;

    glSamplerParameteri(m_samplerId, GL_TEXTURE_MIN_FILTER, m_filtering[0]);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_MAG_FILTER, m_filtering[1]);
    glSamplerParameterf(m_samplerId, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_maxAnisotropy);
}

void Sampler::setTrilinear(Float anisotropy)
{
    m_filtering[0] = LINEAR;
    m_filtering[1] = LINEAR_MIPMAP_LINEAR;
    m_maxAnisotropy = anisotropy;

    glSamplerParameteri(m_samplerId, GL_TEXTURE_MIN_FILTER, m_filtering[0]);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_MAG_FILTER, m_filtering[1]);
    glSamplerParameterf(m_samplerId, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_maxAnisotropy);
}

void Sampler::setWrapS(Sampler::Wrap wrap)
{
    m_wrap[0] = wrap;
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_S, m_wrap[0]);
}

void Sampler::setWrapT(Sampler::Wrap wrap)
{
    m_wrap[1] = wrap;
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_S, m_wrap[1]);
}

void Sampler::setWrapR(Sampler::Wrap wrap)
{
    m_wrap[2] = wrap;
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_R, m_wrap[2]);
}

void Sampler::setWrap(Sampler::Wrap wrap)
{
    m_wrap[0] = m_wrap[1] = m_wrap[2] = wrap;

    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_S, m_wrap[0]);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_T, m_wrap[1]);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_R, m_wrap[2]);
}

void Sampler::setLodRange(Float min, Float max)
{
    m_lodRange[0] = min;
    m_lodRange[1] = max;

    glSamplerParameterf(m_samplerId, GL_TEXTURE_MIN_LOD, m_lodRange[0]);
    glSamplerParameterf(m_samplerId, GL_TEXTURE_MAX_LOD, m_lodRange[1]);
}

void Sampler::setLodRange(Float range[])
{
    m_lodRange[0] = range[0];
    m_lodRange[1] = range[1];

    glSamplerParameterf(m_samplerId, GL_TEXTURE_MIN_LOD, m_lodRange[0]);
    glSamplerParameterf(m_samplerId, GL_TEXTURE_MAX_LOD, m_lodRange[1]);
}

void Sampler::setLodMin(Float min)
{
    m_lodRange[0] = min;
    glSamplerParameterf(m_samplerId, GL_TEXTURE_MIN_LOD, m_lodRange[0]);
}

void Sampler::setLodMax(Float max)
{
    m_lodRange[1] = max;
    glSamplerParameterf(m_samplerId, GL_TEXTURE_MAX_LOD, m_lodRange[1]);
}

void Sampler::setLodBias(Float bias)
{
    m_lodBias = bias;
    glSamplerParameterf(m_samplerId, GL_TEXTURE_LOD_BIAS, m_lodBias);
}

void Sampler::setCompMode(Sampler::CompMode mode)
{
    m_compMode = mode;
    glSamplerParameterf(m_samplerId, GL_TEXTURE_COMPARE_MODE, m_compMode);
}

void Sampler::setCompFunc(Comparison comp)
{
    m_compFunc = comp;
    glSamplerParameterf(m_samplerId, GL_TEXTURE_COMPARE_FUNC, m_compFunc);
}

