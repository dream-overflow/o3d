/**
 * @file heightmapsplatting.cpp
 * @brief Implementation of HeihtmapSplatting.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2010-01-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"

#include "o3d/engine/glextdefines.h"

#include "o3d/engine/landscape/heightmap/heightmapsplatting.h"
#include "o3d/engine/landscape/heightmap/terrainheightmapformat.h"
#include "o3d/engine/shader/shadermanager.h"
#include "o3d/engine/texture/texture2dfeedback.h"
#include "o3d/engine/texture/texturemanager.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/scene/sceneobject.h"
#include "o3d/engine/context.h"
#include "o3d/engine/renderer.h"
#include "o3d/engine/pixelbuffer.h"
#include "o3d/image/image.h"
#include "o3d/core/filemanager.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(HeightmapSplatting, ENGINE_HEIGHT_MAP_SPLATTING, SceneObject)

enum ShaderParamLocation
{
	A_VEC4_VERTEX = 0,
	A_VEC2_TEXCOORDS,

	U_MAT4_MODELVIEW,
	U_TEX_COLORMAP,
	U_TEX_LIGHTMAP,
	U_TEX_SHADOWMAP,
	U_TEX_NORMALMAP,
	U_TEX_NOISE,
	U_TEX_DIFFUSE,
	U_VEC4_UNITS,
	U_FLOAT_INV_NOISE_SCALE,
	U_VEC4_AMBIANT
};
	
//! Default constructor
HeightmapSplatting::HeightmapSplatting(BaseObject *pParent, Camera *pCamera, UInt32 _flags):
	TerrainBase(pParent, pCamera),
    m_indices(getScene()->getContext()),
	m_vertices(V_VERTICES_ARRAY),
	m_texCoords(V_UV_MAP_ARRAY),
    m_vbo(getScene()->getContext(), VertexBuffer::STATIC),
	m_normalTex(this),
	m_colormapTex(this),
	m_lightmapTex(this),
	m_shadowmapTex(this),
	m_noiseTex(this),
	m_whiteTex1(this),
	m_whiteTex2(this),
	m_whiteTex3(this),
	m_flags(OPT_UNDEFINED),
	m_units(),
	m_noiseScale(1.0f),
	m_shader(),
	m_blackShader(),
	m_staticLightShader(),
    m_fbo(getScene()->getContext()),
    m_fboDepthTex(this),
    m_fboColorTex(this),
    m_fboNormalTex(this),
	m_updateFlag(0),
	m_lights()
{
	setFlag(OptionFlag(_flags));
	//	memset((void*)m_paramLocations, 0, sizeof(m_paramLocations));

	Float lWhiteData[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_whiteTex1 = new Texture2D(this);
	m_whiteTex1->setFiltering(Texture::NO_FILTERING);
	m_whiteTex1->create(False, 1, 1, PF_RGBA_F32, lWhiteData, PF_RGBA_F32);

	m_whiteTex2 = new Texture2D(this);
	m_whiteTex2->setFiltering(Texture::NO_FILTERING);
	m_whiteTex2->create(False, 1, 1, PF_RGBA_F32, lWhiteData, PF_RGBA_F32);

	m_whiteTex3 = new Texture2D(this);
	m_whiteTex3->setFiltering(Texture::NO_FILTERING);
	m_whiteTex3->create(False, 1, 1, PF_RGBA_F32, lWhiteData, PF_RGBA_F32);

	m_fboColorTex = new Texture2D(this);
	m_fboDepthTex = new Texture2D(this);
	m_fboNormalTex = new Texture2D(this);

	m_fboColorTex->setFiltering(Texture::NO_FILTERING);
	m_fboDepthTex->setFiltering(Texture::NO_FILTERING);
	m_fboNormalTex->setFiltering(Texture::NO_FILTERING);

	Shader * lpShading = getScene()->getShaderManager()->addShader("terrain/heightmap");
	O3D_ASSERT(lpShading);

	lpShading->buildInstance(m_shader);
	lpShading->buildInstance(m_blackShader);
	lpShading->buildInstance(m_staticLightShader);
	lpShading->buildInstance(m_deferredColorShader);

    m_shader.assign("default", "default");
    m_blackShader.assign("black", "black");
    m_staticLightShader.assign("staticLightning", "staticLightning");
    m_deferredColorShader.assign("deferredColorPass", "deferredColorPass");

	m_shader.setAttribute(A_VEC4_VERTEX, m_shader.getAttributeLocation("a_vertex"));
	m_shader.setAttribute(A_VEC2_TEXCOORDS, m_shader.getAttributeLocation("a_texCoords"));
	m_shader.setUniform(U_MAT4_MODELVIEW, m_shader.getUniformLocation("u_modelViewProjectionMatrix"));
	m_shader.setUniform(U_TEX_COLORMAP, m_shader.getUniformLocation("u_colormap"));
	m_shader.setUniform(U_TEX_NOISE, m_shader.getUniformLocation("u_noise"));
	m_shader.setUniform(U_VEC4_UNITS, m_shader.getUniformLocation("u_units"));
	m_shader.setUniform(U_FLOAT_INV_NOISE_SCALE, m_shader.getUniformLocation("u_invNoiseScale"));

	m_blackShader.setAttribute(A_VEC4_VERTEX, m_blackShader.getAttributeLocation("a_vertex"));
	m_blackShader.setUniform(U_MAT4_MODELVIEW, m_blackShader.getUniformLocation("u_modelViewProjectionMatrix"));
	m_blackShader.setUniform(U_VEC4_UNITS, m_blackShader.getUniformLocation("u_units"));
	
	m_staticLightShader.setAttribute(A_VEC4_VERTEX,			m_staticLightShader.getAttributeLocation("a_vertex"));
	m_staticLightShader.setAttribute(A_VEC2_TEXCOORDS,		m_staticLightShader.getAttributeLocation("a_texCoords"));

	m_staticLightShader.setUniform(U_MAT4_MODELVIEW,		m_staticLightShader.getUniformLocation("u_modelViewProjectionMatrix"));
	m_staticLightShader.setUniform(U_TEX_COLORMAP,			m_staticLightShader.getUniformLocation("u_colormap"));
	m_staticLightShader.setUniform(U_TEX_LIGHTMAP,			m_staticLightShader.getUniformLocation("u_lightmap"));
	m_staticLightShader.setUniform(U_TEX_SHADOWMAP,			m_staticLightShader.getUniformLocation("u_shadowmap"));
	m_staticLightShader.setUniform(U_TEX_NOISE,				m_staticLightShader.getUniformLocation("u_noise"));
	m_staticLightShader.setUniform(U_VEC4_UNITS,			m_staticLightShader.getUniformLocation("u_units"));
	m_staticLightShader.setUniform(U_FLOAT_INV_NOISE_SCALE, m_staticLightShader.getUniformLocation("u_invNoiseScale"));

	m_deferredColorShader.setAttribute(A_VEC4_VERTEX,		m_deferredColorShader.getAttributeLocation("a_vertex"));
	m_deferredColorShader.setAttribute(A_VEC2_TEXCOORDS,	m_deferredColorShader.getAttributeLocation("a_texCoords"));
	m_deferredColorShader.setUniform(U_MAT4_MODELVIEW,		m_deferredColorShader.getUniformLocation("u_modelViewProjectionMatrix"));
	m_deferredColorShader.setUniform(U_TEX_COLORMAP,		m_deferredColorShader.getUniformLocation("u_colormap"));
	m_deferredColorShader.setUniform(U_TEX_NORMALMAP,		m_deferredColorShader.getUniformLocation("u_normalmap"));
	m_deferredColorShader.setUniform(U_TEX_NOISE,			m_deferredColorShader.getUniformLocation("u_noise"));
	m_deferredColorShader.setUniform(U_VEC4_UNITS,			m_deferredColorShader.getUniformLocation("u_units"));
	m_deferredColorShader.setUniform(U_FLOAT_INV_NOISE_SCALE, m_deferredColorShader.getUniformLocation("u_invNoiseScale"));

	lpShading = getScene()->getShaderManager()->addShader("ambient");
	O3D_ASSERT(lpShading);
	lpShading->buildInstance(m_texShader);
    m_texShader.assign("default", "default", "MESH;AMBIENT_MAP");

	m_texShader.setAttribute(A_VEC4_VERTEX,		m_texShader.getAttributeLocation("a_vertex"));
	m_texShader.setAttribute(A_VEC2_TEXCOORDS,	m_texShader.getAttributeLocation("a_texCoords1"));
	m_texShader.setUniform(U_MAT4_MODELVIEW,	m_texShader.getUniformLocation("u_modelViewProjectionMatrix"));
	m_texShader.setUniform(U_VEC4_AMBIANT,		m_texShader.getUniformLocation("u_ambient"));
	m_texShader.setUniform(U_TEX_DIFFUSE,		m_texShader.getUniformLocation("u_ambientMap"));

	lpShading = getScene()->getShaderManager()->addShader("ambient");
	O3D_ASSERT(lpShading);
	lpShading->buildInstance(m_rgTexShader);
    m_rgTexShader.assign("default", "default", "MESH;AMBIENT_MAP;RG");

	m_rgTexShader.setAttribute(A_VEC4_VERTEX,	 m_rgTexShader.getAttributeLocation("a_vertex"));
	m_rgTexShader.setAttribute(A_VEC2_TEXCOORDS, m_rgTexShader.getAttributeLocation("a_texCoords1"));
	m_rgTexShader.setUniform(U_MAT4_MODELVIEW,	 m_rgTexShader.getUniformLocation("u_modelViewProjectionMatrix"));
	m_rgTexShader.setUniform(U_VEC4_AMBIANT,	 m_rgTexShader.getUniformLocation("u_ambient"));
	m_rgTexShader.setUniform(U_TEX_DIFFUSE,		 m_rgTexShader.getUniformLocation("u_ambientMap"));
}

//! Destructor
HeightmapSplatting::~HeightmapSplatting()
{
}

Bool HeightmapSplatting::isValid() const
{
	Bool lOk = m_shader.isOperational() && m_vertices.isValid();

    if ((m_flags & OPT_STATIC_LIGHTNING) != 0) {
		lOk &= m_colormapTex.isValid();		// Colormap must be defined
		lOk &= ((m_flags & OPT_NOISE) == 0) || (m_noiseTex.isValid()); // Noise must be disabled or be defined
	}

	return lOk;
}

void HeightmapSplatting::load(InStream &_is)
{
	TerrainHeightmap::FileHeader lFileHeader;
    _is.read(lFileHeader.header, 8);
    _is.read(lFileHeader.version, 8);
    _is.read(&lFileHeader.crc32, 1);
    _is.read(lFileHeader.heightmapSize, 2);
    _is.read(lFileHeader.heightmapUnits, 3);
    _is.read(&lFileHeader.attribCount, 1);
    _is.read(&lFileHeader.materialCount, 1);

	// Loading of vertex attributes
	std::vector<TerrainHeightmap::AttribHeader> lAttribHeaders(lFileHeader.attribCount);

    for (std::vector<TerrainHeightmap::AttribHeader>::iterator it = lAttribHeaders.begin() ; it != lAttribHeaders.end() ; it++) {
        _is.read(it->name, 64);
        _is.read(&it->position, 1);
        _is.read(&it->size, 1);
        _is.read(it->format, 32);
	}
}

void HeightmapSplatting::enable(OptionFlag _flag, Bool _state)
{
	Int32 lBitCount = 0;

    for (Int32 i = 0; i < 32 ; ++i) {
		lBitCount += ((_flag >> i) & 0x00000001);
    }

    if (lBitCount > 1) {
		setFlag(_flag);
    } else {
		const Bool lOldState = isEnabled(_flag);

        switch(_flag) {
            case OPT_STATIC_LIGHTNING:
                if (_state != lOldState)
                    m_updateFlag |= UPDATE_SHADER;
                break;
            case OPT_NOISE:
                break;
            default:
                O3D_ASSERT(0);
                break;
		}

		m_flags = (_state ? m_flags | _flag : m_flags & ~_flag);
	}
}

void HeightmapSplatting::setFlag(UInt32 _flag)
{
    if ((_flag == OPT_UNDEFINED) && (_flag != m_flags)) {
		enable(OPT_STATIC_LIGHTNING, False);
		enable(OPT_NOISE, False);
    } else {
        if ((_flag & OPT_STATIC_LIGHTNING) != (m_flags & OPT_STATIC_LIGHTNING)) {
			enable(OPT_STATIC_LIGHTNING, (_flag & OPT_STATIC_LIGHTNING) != 0);
        }

        if ((_flag & OPT_NOISE) != (m_flags & OPT_NOISE)) {
			enable(OPT_NOISE, (_flag & OPT_NOISE) != 0);
        }
	}
}

void HeightmapSplatting::setHeightmap(const Image & _pic, Float _offset)
{
    if (!_pic.isValid()) {
		O3D_ERROR(E_InvalidOperation("Invalid heightmap picture"));
    }

    if ((_pic.getWidth() != _pic.getHeight())) {
        O3D_ERROR(E_InvalidOperation("Heightmap picture must be square"));
    }

    if (!o3d::isPow2(_pic.getWidth())) {
		O3D_ERROR(E_InvalidOperation("Heightmap width must be a power of two"));
    }

	const Int32 lBpp = _pic.getBpp()/8;
	const UInt8 * lpSrc = _pic.getData();

	SmartArrayFloat lVertexBuffer(4*_pic.getWidth() * _pic.getHeight());
	SmartArrayFloat lTexCoordBuffer(2*_pic.getWidth() * _pic.getHeight());

	Float * lpBufferVertex = lVertexBuffer.getData();
	Float * lpBufferTex = lTexCoordBuffer.getData();

	const Float liWidth = 1.0f / (_pic.getWidth()-1);
	const Float liHeight = 1.0f / (_pic.getHeight()-1);

    for(UInt32 i = 0 ; i < _pic.getWidth() * _pic.getHeight() ; ++i, lpBufferVertex += 4, lpBufferTex += 2, lpSrc += lBpp) {
		lpBufferVertex[X] = Float(i / _pic.getWidth());
		lpBufferVertex[Y] = (*lpSrc) + _offset;
		lpBufferVertex[Z] = Float(i % _pic.getWidth());
		lpBufferVertex[W] = 1.0f;

		lpBufferTex[X] = lpBufferVertex[Z] * liWidth;
		lpBufferTex[Y] = lpBufferVertex[X] * liHeight;
	}

	VertexBufferBuilder vboBuilder(True);

	m_vertices.setData(lVertexBuffer, 4);
	vboBuilder.addData(&m_vertices);

	m_texCoords.setData(lTexCoordBuffer, 2);
	vboBuilder.addData(&m_texCoords);

	vboBuilder.create(m_vbo);

    if (m_indices.getNumElements() != 2*3*(_pic.getWidth()-1) * (_pic.getHeight()-1)) {
        SmartArrayUInt32 faceArray(2*3*(_pic.getWidth()-1) * (_pic.getHeight()-1));

        UInt32 *lpIndicesInit = faceArray.getData();
        UInt32 *lpIndices = lpIndicesInit;

        for (UInt32 j = 0 ; j < _pic.getWidth()-1 ; ++j) {
            UInt32 lCurrRow = j * _pic.getWidth();
            UInt32 lNextRow = (j + 1) * _pic.getWidth();

            for (UInt32 i = 0 ; i < _pic.getHeight()-1 ; ++i, lpIndices += 6, lCurrRow++, lNextRow++) {
                lpIndices[0] = lCurrRow;
                lpIndices[1] = lCurrRow+1;
                lpIndices[2] = lNextRow;

                lpIndices[3] = lNextRow;
                lpIndices[4] = lCurrRow+1;
                lpIndices[5] = lNextRow+1;
            }
        }

        m_indices.setFaces(faceArray);
        m_indices.create();
	}
}

void HeightmapSplatting::setNormalmap(const Image & _pic)
{
    if (_pic.isValid()) {
        if (_pic.getBpp() < 24) {
			O3D_ERROR(E_InvalidOperation("Invalid colormap picture: bpp must be >= 24"));
        }

        if ((_pic.getWidth() != _pic.getHeight())) {
			O3D_ERROR(E_InvalidOperation("Colormap picture must be square"));
        }

        if (!o3d::isPow2(_pic.getWidth())) {
			O3D_ERROR(E_InvalidOperation("Colormap width must be a power of two"));
        }

		m_normalTex = new Texture2D(this, _pic);
		m_normalTex->setFiltering(Texture::BILINEAR_FILTERING);
		m_normalTex->create(True);
    } else if (m_normalTex.isValid()) {
		m_normalTex.releaseCheckAndDelete();
	}
}

void HeightmapSplatting::setColormap(const Image & _pic)
{
    if (_pic.isValid()) {
        if (_pic.getBpp() < 24) {
            O3D_ERROR(E_InvalidOperation("Invalid colormap picture: bpp must be >= 24"));
        }

        if ((_pic.getWidth() != _pic.getHeight())) {
			O3D_ERROR(E_InvalidOperation("Colormap picture must be square"));
        }

        if (!o3d::isPow2(_pic.getWidth())) {
			O3D_ERROR(E_InvalidOperation("Colormap width must be a power of two"));
        }

		m_colormapTex = new Texture2D(this, _pic);
		m_colormapTex->setFiltering(Texture::BILINEAR_FILTERING);
		m_colormapTex->create(True);
    } else if (m_colormapTex.isValid()) {
		m_colormapTex.releaseCheckAndDelete();
	}
}

void HeightmapSplatting::setLightmap(const Image & _pic)
{
    if (!isEnabled(OPT_STATIC_LIGHTNING)) {
		O3D_WARNING("Static lightning must be enabled");
    }

    if (_pic.isValid()) {
        if ((_pic.getWidth() != _pic.getHeight())) {
			O3D_ERROR(E_InvalidOperation("Lightmap picture must be square"));
        }

        if (!o3d::isPow2(_pic.getWidth())) {
            O3D_ERROR(E_InvalidOperation("Lightmap width must be a power of two"));
        }

		m_lightmapTex = new Texture2D(this, _pic);
		m_lightmapTex->setFiltering(Texture::BILINEAR_FILTERING);
		m_lightmapTex->create(True);
	}
	else if (m_lightmapTex.isValid())
	{
		m_lightmapTex.releaseCheckAndDelete();
	}
}

void HeightmapSplatting::setShadowmap(const Image & _pic)
{
    if (!isEnabled(OPT_STATIC_LIGHTNING)) {
		O3D_WARNING("Static lightning must be enabled");
    }

    if (_pic.isValid()) {
        if ((_pic.getWidth() != _pic.getHeight())) {
			O3D_ERROR(E_InvalidOperation("Shadowmap picture must be square"));
        }

        if (!o3d::isPow2(_pic.getWidth())) {
			O3D_ERROR(E_InvalidOperation("Shadowmap width must be a power of two"));
        }

		m_shadowmapTex = new Texture2D(this, _pic);
		m_shadowmapTex->setFiltering(Texture::BILINEAR_FILTERING);
		m_shadowmapTex->create(True);
    } else if (m_shadowmapTex.isValid()) {
		m_shadowmapTex.releaseCheckAndDelete();
	}
}

void HeightmapSplatting::setNoise(const Image & _pic)
{
    if (_pic.isValid()) {
        if ((_pic.getWidth() != _pic.getHeight())) {
			O3D_ERROR(E_InvalidOperation("Noise picture must be square"));
        }

        if (!o3d::isPow2(_pic.getWidth())) {
			O3D_ERROR(E_InvalidOperation("Noise width must be a power of two"));
        }

		m_noiseTex = new Texture2D(this, _pic);
		m_noiseTex->setFiltering(Texture::BILINEAR_FILTERING);
		m_noiseTex->create(True);
    } else {
		m_shadowmapTex.releaseCheckAndDelete();
	}
}

//---------------------------------------------------------------------------------------
// Processing
//---------------------------------------------------------------------------------------

void HeightmapSplatting::addLight(const LightInfos & _info)
{
    for (CIT_LightArray it = m_lights.begin() ; it != m_lights.end() ; it++) {
        if (it->pLight == _info.pLight) {
			O3D_ERROR(E_InvalidOperation("Attempt to add a existing light to the terrain"));
        }
	}

	m_lights.push_back(_info);
}

void HeightmapSplatting::removeLight(Light * _pLight)
{
    for (IT_LightArray it = m_lights.begin() ; it != m_lights.end() ; it++) {
        if (it->pLight == _pLight) {
			m_lights.erase(it);
			return;
		}
	}

	O3D_ERROR(E_InvalidOperation("Attempt to remove an invalid light"));
}

void HeightmapSplatting::updateLight(Light *)
{
}

void HeightmapSplatting::draw()
{
	const Box2i viewPort = getScene()->getContext()->getViewPort();
    //const Vector2i lFboSize(viewPort.width()/2, viewPort.height()/2);
    const Vector2i lFboSize(viewPort.width(), viewPort.height());

    if (((m_updateFlag & UPDATE_SHADER) != 0) || !m_fbo.isValid()) {
		O3D_ASSERT(m_fboColorTex && m_fboDepthTex);

        if ((m_fboColorTex->getWidth() != UInt32(lFboSize[X])) ||
            (m_fboColorTex->getHeight() != UInt32(lFboSize[Y]))) {
            m_fboColorTex->create(False, lFboSize[X], lFboSize[Y], PF_RGBA_U8);
        }

        if ((m_fboDepthTex->getWidth() != UInt32(lFboSize[X])) ||
            (m_fboDepthTex->getHeight() != UInt32(lFboSize[Y]))) {
			// GL_DEPTH_COMPONENT16 ensures compatibility with some old ATI cards
			//m_fboDepthTex->create(False, lVboSize[X], lVboSize[Y], PF_DEPTH_U16);
            m_fboDepthTex->create(False, lFboSize[X], lFboSize[Y], PF_DEPTH_F32);
		}

		if (!isEnabled(OPT_STATIC_LIGHTNING) &&
            ((m_fboNormalTex->getWidth() != UInt32(lFboSize[X])) ||
            (m_fboNormalTex->getHeight() != UInt32(lFboSize[Y])))) {
            m_fboNormalTex->create(False, lFboSize.x(), lFboSize.y(), PF_RGBA_U8);
		}

        if (m_fbo.getDimension() != lFboSize) {
            m_fbo.create(lFboSize.x(), lFboSize.y(), PF_RGBA_U8, True);
            m_fbo.attachTexture2D(m_fboColorTex.get(), FrameBuffer::COLOR_ATTACHMENT0);

            if (!isEnabled(OPT_STATIC_LIGHTNING)) {
                m_fbo.attachTexture2D(m_fboNormalTex.get(), FrameBuffer::COLOR_ATTACHMENT1);
            }

			m_fbo.attachTexture2D(m_fboDepthTex.get(), FrameBuffer::DEPTH_ATTACHMENT);
			m_fbo.unbindBuffer();
		}
	}

    getScene()->getContext()->setViewPort(0, 0, lFboSize.x(), lFboSize.y());

    if ((m_flags & OPT_STATIC_LIGHTNING) != 0) {
		drawDeferredStaticLighting();
    } else {
		drawDeferred();
    }

	getScene()->getContext()->setViewPort(0, 0, viewPort.width(), viewPort.height());

    // Screen rendering of generated texture
//    pbo->bindUnpackBuffer();

//    //glReadPixels(0, 0, lVboSize.x(), lVboSize.y(), GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
//    Float *data = pbo->lock(0, 0, PixelBuffer::READ_ONLY);
//    Image out;
//    out.loadBuffer(lVboSize.x(), lVboSize.y(), lVboSize.x()*lVboSize.y()*4, PF_DEPTH_F32, (UInt8*)data);
//    out.save("test.png", Image::PNG);

//    pbo->unlock();
//    pbo->unbindUnpackBuffer();

//    // @todo into object
//    Texture2DFeedback depthTex(this);
//    depthTex.create(GL_BACK, m_fboDepthTex->getPixelFormat());
//    depthTex.setBox(Box2i(0, 0, m_fboDepthTex->getWidth(), m_fboDepthTex->getHeight()));

    // depthTex.update(); or

////    getScene()->getContext()->bindPixelPackBuffer(depthTex.getPBO0Id());
////    m_fboDepthTex->bind();
////    glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

////    getScene()->getContext()->bindPixelPackBuffer(0);
//    depthTex.copyToTexture(m_fboDepthTex.get(), 0);

//    GLuint lBuffer;
//    glGenBuffers(1, &lBuffer);
//    getScene()->getContext()->bindPixelPackBuffer(lBuffer);

//    glBufferData(GL_PIXEL_PACK_BUFFER,
//                 sizeof(Float) * lFboSize.x() * lFboSize.y(),
//                 nullptr,
//                 GL_STATIC_COPY);

//    m_fboDepthTex->bind();
//    glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

//    getScene()->getContext()->bindPixelPackBuffer(0);
//    getScene()->getContext()->bindPixelUnpackBuffer(lBuffer);

//    // faster than TexImage2D
//    glTexSubImage2D(
//                GL_TEXTURE_2D,
//                0,
//                0,
//                0,
//                lFboSize.x(),
//                lFboSize.y(),
//                m_fboDepthTex.get()->getGLInternalFormat(),
//                GL_FLOAT,
//                nullptr);

//    getScene()->getRenderer()->isError();
//    getScene()->getContext()->bindPixelUnpackBuffer(0);

//    m_fboDepthTex->unbind();
//    glDeleteBuffers(1, &lBuffer);

	// Screen rendering
	Matrix4 lProjection;
	lProjection.buildOrtho(	 0.0f, 1.0f,  // left right
							 0.0f, 1.0f,  // bottom top
							-1.0f, 1.0f);

	const Float lpTexCoords[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
	const Float lpFullScreenVertices[] = {	0.0f, 0.0f, 0.0f, 1.0f,
                                            1.0f, 0.0f, 0.0f, 1.0f,
                                            0.0f, 1.0f, 0.0f, 1.0f,
                                            1.0f, 1.0f, 0.0f, 1.0f };

	const Float lpZbufferVertices[] = {	0.6f, 0.6f, 0.0f, 1.0f,
                                        1.0f, 0.6f, 0.0f, 1.0f,
                                        0.6f, 1.0f, 0.0f, 1.0f,
                                        1.0f, 1.0f, 0.0f, 1.0f };

	const Float lpColorVertices[] = {	0.6f, 0.1f, 0.0f, 1.0f,
                                        1.0f, 0.1f, 0.0f, 1.0f,
                                        0.6f, 0.5f, 0.0f, 1.0f,
                                        1.0f, 0.5f, 0.0f, 1.0f };

	const Float lpNormalVertices[] = {	0.0f, 0.1f, 0.0f, 1.0f,
                                        0.4f, 0.1f, 0.0f, 1.0f,
                                        0.0f, 0.5f, 0.0f, 1.0f,
                                        0.4f, 0.5f, 0.0f, 1.0f };

	ArrayBufferf
        lFullScreenVbo(getScene()->getContext()),
        lTexVbo(getScene()->getContext()),
        lZbufferVertexVbo(getScene()->getContext()),
        lColorVertexVbo(getScene()->getContext()),
        lNormalVertexVbo(getScene()->getContext());

	lTexVbo.create(8, VertexBuffer::STATIC, lpTexCoords);
	lFullScreenVbo.create(16, VertexBuffer::STATIC, lpFullScreenVertices);
	lZbufferVertexVbo.create(16, VertexBuffer::STATIC, lpZbufferVertices);
	lColorVertexVbo.create(16, VertexBuffer::STATIC, lpColorVertices);
	lNormalVertexVbo.create(16, VertexBuffer::STATIC, lpNormalVertices);

	Context *glContext = getScene()->getContext();

	glContext->disableDepthTest();

	// full screen part
	m_texShader.bindShader();
		lFullScreenVbo.attribute(m_texShader.getAttribute(A_VEC4_VERTEX), 4, 0, 0);
		lTexVbo.attribute(m_texShader.getAttribute(A_VEC2_TEXCOORDS), 2, 0, 0);

		m_texShader.setConstMatrix4(m_texShader.getUniform(U_MAT4_MODELVIEW), False, lProjection);
		m_texShader.setConstVector4(m_texShader.getUniform(U_VEC4_AMBIANT), Vector4(1.0f, 1.0f, 1.0f, 1.0f));

		m_texShader.setConstTexture(m_texShader.getUniform(U_TEX_DIFFUSE), m_fboColorTex.get(), 0);

		getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);
	m_texShader.unbindShader();

	// depth part
	m_rgTexShader.bindShader();
		lZbufferVertexVbo.attribute(m_rgTexShader.getAttribute(A_VEC4_VERTEX), 4, 0, 0);
		lTexVbo.attribute(m_texShader.getAttribute(A_VEC2_TEXCOORDS), 2, 0, 0);

		m_rgTexShader.setConstMatrix4(m_rgTexShader.getUniform(U_MAT4_MODELVIEW), False, lProjection);
		m_rgTexShader.setConstVector4(m_rgTexShader.getUniform(U_VEC4_AMBIANT), Vector4(1.0f, 1.0f, 1.0f, 1.0f));

		m_rgTexShader.setConstTexture(m_rgTexShader.getUniform(U_TEX_DIFFUSE), m_fboDepthTex.get(), 0);

		getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);

		glContext->disableVertexAttribArray(m_rgTexShader.getAttribute(A_VEC4_VERTEX));
		glContext->disableVertexAttribArray(m_rgTexShader.getAttribute(A_VEC2_TEXCOORDS));
	m_rgTexShader.unbindShader();

	// color and normal
	m_texShader.bindShader();
		lTexVbo.attribute(m_texShader.getAttribute(A_VEC2_TEXCOORDS), 2, 0, 0);

		m_texShader.setConstTexture(m_texShader.getUniform(U_TEX_DIFFUSE), m_fboColorTex.get(), 0);

		lColorVertexVbo.attribute(m_texShader.getAttribute(A_VEC4_VERTEX), 4, 0, 0);

		getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);

        if (!isEnabled(OPT_STATIC_LIGHTNING)) {
			m_texShader.setConstTexture(m_texShader.getUniform(U_TEX_DIFFUSE), m_fboNormalTex.get(), 0);

			lNormalVertexVbo.attribute(m_texShader.getAttribute(A_VEC4_VERTEX), 4, 0, 0);

			getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);
		}

		glContext->disableVertexAttribArray(m_texShader.getAttribute(A_VEC4_VERTEX));
		glContext->disableVertexAttribArray(m_texShader.getAttribute(A_VEC2_TEXCOORDS));
	m_texShader.unbindShader();

	getScene()->getContext()->setDefaultDepthTest();

    //m_fboDepthTex->create(False, lFboSize.x(), lFboSize.y(), PF_DEPTH_U16);
    //m_fboDepthTex->create(False, lFboSize.x(), lFboSize.y(), PF_DEPTH_F32);
}

void HeightmapSplatting::drawDeferred()
{
	Context *glContext = getScene()->getContext();

	m_fbo.bindBuffer();
	m_fbo.clearDepth();

	// Depth pass
	m_blackShader.bindShader();
		m_blackShader.setConstMatrix4(m_blackShader.getUniform(U_MAT4_MODELVIEW), False, getScene()->getContext()->modelViewProjection());
		m_blackShader.setConstVector4(m_blackShader.getUniform(U_VEC4_UNITS), Vector4(m_units[0], m_units[1], m_units[2], 1.0f));

		m_vertices.attribute(m_blackShader.getAttribute(A_VEC4_VERTEX));

		m_indices.draw(getScene());

		glContext->disableVertexAttribArray(m_blackShader.getAttribute(A_VEC4_VERTEX));
	m_blackShader.unbindShader();

    const GLenum attch[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attch);
	m_fbo.clearColor();

	glContext->setDepthFunc(COMP_EQUAL);

	// Material/Color pass
	m_deferredColorShader.bindShader();
		m_deferredColorShader.setConstMatrix4(m_deferredColorShader.getUniform(U_MAT4_MODELVIEW), False, getScene()->getContext()->modelViewProjection());
		m_deferredColorShader.setConstVector4(m_deferredColorShader.getUniform(U_VEC4_UNITS), Vector4(m_units[0], m_units[1], m_units[2], 1.0f));
		m_deferredColorShader.setConstFloat(m_deferredColorShader.getUniform(U_FLOAT_INV_NOISE_SCALE), 1.0f / m_noiseScale);

		m_deferredColorShader.setConstTexture(m_deferredColorShader.getUniform(U_TEX_COLORMAP), m_colormapTex.isValid() ? m_colormapTex.get() : getScene()->getTextureManager()->getDefaultTexture2D(), 0);
		m_deferredColorShader.setConstTexture(m_deferredColorShader.getUniform(U_TEX_NORMALMAP), m_normalTex.isValid() ? m_normalTex.get() : m_whiteTex1.get(), 1);
		m_deferredColorShader.setConstTexture(m_deferredColorShader.getUniform(U_TEX_NOISE), m_noiseTex.isValid() && (m_flags & OPT_NOISE) != 0 ? m_noiseTex.get() : m_whiteTex2.get(), 2);

		m_vertices.attribute(m_deferredColorShader.getAttribute(A_VEC4_VERTEX));
		m_texCoords.attribute(m_deferredColorShader.getAttribute(A_VEC2_TEXCOORDS));

		m_indices.draw(getScene());

		glContext->disableVertexAttribArray(m_deferredColorShader.getAttribute(A_VEC4_VERTEX));
		glContext->disableVertexAttribArray(m_deferredColorShader.getAttribute(A_VEC2_TEXCOORDS));
	m_deferredColorShader.unbindShader();

	glContext->setDefaultDepthFunc();

    glDrawBuffers(1, attch);  // only color0
	m_fbo.unbindBuffer();

	O3D_ASSERT(!getScene()->getRenderer()->isError());
}

void HeightmapSplatting::drawDeferredStaticLighting()
{
	Context *glContext = getScene()->getContext();

	m_fbo.bindBuffer();

    m_fbo.clearColor(FrameBuffer::COLOR_ATTACHMENT0, Color(0.f, 0.f, 0.f, 0.f));
    m_fbo.clearDepth(1.0f);

	O3D_ASSERT(m_fbo.isCompleteness());
	O3D_ASSERT(m_staticLightShader.isOperational());
	O3D_ASSERT(m_blackShader.isOperational());

	// Depth pass
	m_blackShader.bindShader();
		m_blackShader.setConstMatrix4(m_blackShader.getUniform(U_MAT4_MODELVIEW), False, getScene()->getContext()->modelViewProjection());
		m_blackShader.setConstVector4(m_blackShader.getUniform(U_VEC4_UNITS), Vector4(m_units[0], m_units[1], m_units[2], 1.0f));

		m_vertices.attribute(m_blackShader.getAttribute(A_VEC4_VERTEX));

		m_indices.draw(getScene());

		glContext->disableVertexAttribArray(m_blackShader.getAttribute(A_VEC4_VERTEX));
	m_blackShader.unbindShader();

	// Color + light + shadow + noise pass
	glContext->setDepthFunc(COMP_EQUAL);

	m_staticLightShader.bindShader();
		m_staticLightShader.setConstMatrix4(m_staticLightShader.getUniform(U_MAT4_MODELVIEW), False, getScene()->getContext()->modelViewProjection());
		m_staticLightShader.setConstVector4(m_staticLightShader.getUniform(U_VEC4_UNITS), Vector4(m_units[0], m_units[1], m_units[2], 1.0f));
		m_staticLightShader.setConstFloat(m_staticLightShader.getUniform(U_FLOAT_INV_NOISE_SCALE), 1.0f / m_noiseScale);
		
		m_staticLightShader.setConstTexture(m_staticLightShader.getUniform(U_TEX_COLORMAP), m_colormapTex.isValid() ? m_colormapTex.get() : getScene()->getTextureManager()->getDefaultTexture2D(), 0);
		m_staticLightShader.setConstTexture(m_staticLightShader.getUniform(U_TEX_LIGHTMAP), m_lightmapTex.isValid() ? m_lightmapTex.get() : m_whiteTex1.get(), 1);
		m_staticLightShader.setConstTexture(m_staticLightShader.getUniform(U_TEX_SHADOWMAP), m_shadowmapTex.isValid() ? m_shadowmapTex.get() : m_whiteTex2.get(), 2);
		m_staticLightShader.setConstTexture(m_staticLightShader.getUniform(U_TEX_NOISE), m_noiseTex.isValid() && (m_flags & OPT_NOISE) != 0 ? m_noiseTex.get() : m_whiteTex3.get(), 3);

		m_vertices.attribute(m_staticLightShader.getAttribute(A_VEC4_VERTEX));
		m_texCoords.attribute(m_staticLightShader.getAttribute(A_VEC2_TEXCOORDS));

		m_indices.draw(getScene());

		glContext->disableVertexAttribArray(m_staticLightShader.getAttribute(A_VEC4_VERTEX));
		glContext->disableVertexAttribArray(m_staticLightShader.getAttribute(A_VEC2_TEXCOORDS));
	m_staticLightShader.unbindShader();

	glContext->setDefaultDepthFunc();

	m_fbo.unbindBuffer();
}

void HeightmapSplatting::update()
{
}
