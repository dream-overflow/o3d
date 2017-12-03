/**
 * @file context.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/context.h"

#include "o3d/engine/renderer.h"
#include "o3d/engine/glextdefines.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/occlusionquery.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/core/objects.h"
#include "o3d/core/debug.h"

using namespace o3d;

// Default constructor
Context::Context(Renderer *renderer) :
	m_renderer(renderer),
	m_useSimpleDrawMode(False),
	m_recomputeModelViewProjection(True),
	m_recomputeNormalMatrix(True)
{
	O3D_ASSERT(m_renderer);

	// force to default values
	forceDefaultDrawingMode();
	forceDefaultPointSize();
	forceDefaultCullingMode();
	forceDefaultDepthTest();
	forceDefaultBackgroundColor();
	forceDefaultDepthClear();
	forceDefaultDepthRange();
	forceDefaultDepthFunc();
	forceDefaultAntiAliasing();
	forceDefaultDepthWrite();
	forceDefaultDoubleSide();
	forceDefaultScissorTest();
	forceDefaultStencilClear();
	forceDefaultStencilTest();
	forceDefaultStencilTestFunc();

    m_blending.forceDefaultFunc();
    m_blending.forceDefaultEquation();

    m_currentOccQuery = nullptr;

	m_currentTexUnit = 0;
	glActiveTexture(GL_TEXTURE0);

    memset(m_currentTexId, 0, MAX_COMBINED_TEX_UNITS * sizeof(UInt32));
    memset(m_currentSamplerId, 0, MAX_COMBINED_TEX_UNITS * sizeof(UInt32));

	m_currentFBOId = 0;
	m_currentPackPBOId = 0;
	m_currentUnpackPBOId = 0;

	m_currentShaderId = 0;

	m_overrideDrawing = DRAWING_FILLED;
	m_overrideDrawingMode = False;
	m_overrideDoubleSided = False;

	m_defaultVAOId = 0;

	m_currentVAOId = 0;
	m_currentVaoState = &m_defaultVaoState;

    // Create a default VAO and bind it
    glGenVertexArrays(1, (GLuint*)&m_defaultVAOId);
    bindVertexArray(0, nullptr);

    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, (GLint*)&m_textureMaxLayers);

    m_maxTessEvalTextureImageUnits = 0;
    m_maxTessControlTextureImageUnits = 0;
    m_maxViewports = 0;

    if (m_renderer->getVersion() >= Renderer::OGL_400) {
        glGetIntegerv(GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS, (GLint*)&m_maxTessEvalTextureImageUnits);
		glGetIntegerv(GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS, (GLint*)&m_maxTessControlTextureImageUnits);
		glGetIntegerv(GL_MAX_VIEWPORTS, (GLint*)&m_maxViewports);
	}

	// anisotropy
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, (GLfloat*)&m_maxAnisotropy);

	glDrawBuffer(GL_BACK);

	glDisable(GL_DITHER);

	// texture
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&m_textureMaxSize);
	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, (GLint*)&m_texture3dMaxSize);
    glGetIntegerv(GL_MAX_SAMPLES, (GLint*)&m_textureMaxSamples);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (GLint*)&m_maxTextureImageUnits);
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, (GLint*)&m_maxVertexTextureImageUnits);
    glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, (GLint*)&m_maxGeometryTextureImageUnits);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, (GLint*)&m_maxCombinedTextureImageUnits);

	glGetIntegerv(GL_MAX_DRAW_BUFFERS, (GLint*)&m_maxDrawBuffers);
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, (GLint*)&m_maxVertexAttribs);

	// GLSL version
    const GLubyte *version = glGetString(GL_SHADING_LANGUAGE_VERSION);
    if ((version != nullptr) && (version[0] >= '1') && (version[1] == '.'))	{
		m_glslVersion = (version[0] - '0') * 100 +
						(version[2] - '0') * 10 +
						(version[3] - '0') * 1;
    } else {
        // assume 1.10 otherwise
		m_glslVersion = 110;
    }

	O3D_ASSERT(!m_renderer->isError());

	// matrix observer
	m_modelview.addObserver(this);
	m_projection.addObserver(this);

	// Print a beautiful log message
	String message;
	message << "OpenGL capacities:\n"
            << "    |- Texture max size = " << m_textureMaxSize << "\n"
            << "    |- Texture 3D max size = " << m_texture3dMaxSize << "\n"
            << "    |- Texture max samples = " << m_textureMaxSamples << "\n"
            << "    |- Max texture layers = " << m_textureMaxLayers << "\n"
            << "    |- Max fragment texture image units = " << m_maxTextureImageUnits << "\n"
            << "    |- Max vertex texture image units = " << m_maxVertexTextureImageUnits << "\n"
            << "    |- Max geometry texture image units = " << m_maxGeometryTextureImageUnits << "\n"
            << "    |- Max tessellation eval texture image units = " << m_maxTessEvalTextureImageUnits << "\n"
            << "    |- Max tessellation control texture image units = " << m_maxTessControlTextureImageUnits << "\n"
            << "    |- Max combined texture image units = " << m_maxCombinedTextureImageUnits << "\n"
            << "    |- Max vertex attribs = " << m_maxVertexAttribs << "\n"
            << "    |- Max draw buffers = " << m_maxDrawBuffers << "\n"
            << "    |- Max anisotropy lvl = " << m_maxAnisotropy << "\n"
            << "    |- Max view-ports = " << m_maxViewports << "\n"
            << "    |- GLSL version = " << (version?(const Char*)version:"1.10") << "\n";

    O3D_MESSAGE(message);
}

// Destructor
Context::~Context()
{
	// default VAO
    if (m_defaultVAOId != O3D_UNDEFINED) {
		glDeleteVertexArrays(1, &m_defaultVAOId);
    }

	// matrix observer
	m_modelview.removeObserver(this);
	m_projection.removeObserver(this);
}

// Define the current active texture unit.
void Context::setActiveTextureUnit(UInt32 unit)
{
	O3D_ASSERT(unit < UInt32(m_maxTextureImageUnits));
    if (unit != m_currentTexUnit) {
		glActiveTexture(unit + GL_TEXTURE0);
		m_currentTexUnit = unit;
	}
}

Bool Context::resetTexture(UInt32 id)
{
    Bool res = True;

    for (UInt32 i = 0; i < MAX_COMBINED_TEX_UNITS; ++i) {
        if (m_currentTexId[i] == id) {
            m_currentTexId[i] = 0;
            res = False;
        }
    }

    return res;
}

void Context::bindTexture(TextureType type, UInt32 id, Bool force)
{
    if ((id != m_currentTexId[m_currentTexUnit]) || force) {
		glBindTexture((GLenum)type, id);
		m_currentTexId[m_currentTexUnit] = id;
	}
}

void Context::deleteTexture(UInt32 id)
{
    if (id != 0) {
        //O3D_ASSERT(resetTexture(id));
		glDeleteTextures(1, (GLuint*)&id);
    }
}

Bool Context::resetSampler(UInt32 id)
{
    Bool res = True;

    for (UInt32 i = 0; i < MAX_COMBINED_TEX_UNITS; ++i) {
        if (m_currentSamplerId[i] == id) {
            m_currentSamplerId[i] = 0;
            res = False;
        }
    }

    return res;
}

void Context::bindSampler(UInt32 id, UInt32 unit, Bool force)
{
    O3D_ASSERT(unit < (UInt32)m_maxCombinedTextureImageUnits);
    if ((id != m_currentSamplerId[unit]) || force) {
        glBindSampler(unit, id);
        m_currentSamplerId[unit] = id;
    }
}

void Context::deleteSampler(UInt32 id)
{
    if (id != 0) {
        //O3D_ASSERT(resetSampler(id));
        glDeleteSamplers(1, (GLuint*)&id);
    }
}

// Enable a vertex attribute array.
void Context::enableVertexAttribArray(UInt32 array)
{
	O3D_ASSERT(array < UInt32(m_maxVertexAttribs));
    if (!m_currentVaoState->attributeArray[array]) {
		glEnableVertexAttribArray(array);
		m_currentVaoState->attributeArray.setBit(array, True);
	}
}

// Disable a vertex attribute array.
void Context::disableVertexAttribArray(UInt32 array)
{
	O3D_ASSERT(array < UInt32(m_maxVertexAttribs));
    if (m_currentVaoState->attributeArray[array]) {
		glDisableVertexAttribArray(array);
		m_currentVaoState->attributeArray.setBit(array, False);
	}
}

// Force the state of a vertex attribute array.
void Context::forceVertexAttribArray(UInt32 array, Bool state)
{
	O3D_ASSERT(array < UInt32(m_maxVertexAttribs));
    if (state) {
		glEnableVertexAttribArray(array);
		m_currentVaoState->attributeArray.setBit(array, True);
    } else {
		glDisableVertexAttribArray(array);
		m_currentVaoState->attributeArray.setBit(array, False);
	}
}

// Attribute vertex array. Enable the vertex attribute array if necessary.
void Context::vertexAttribArray(
		UInt32 array,
		UInt32 eltSize,
		UInt32 stride,
		UInt32 offset)
{
	O3D_ASSERT(array < UInt32(m_maxVertexAttribs));
    if (!m_currentVaoState->attributeArray[array]) {
		glEnableVertexAttribArray(array);
		m_currentVaoState->attributeArray.setBit(array, True);
	}

	glVertexAttribPointer(
			array,
			eltSize,
			GL_FLOAT,
			GL_FALSE,
			stride,
			(const GLvoid*) ((GLubyte*) 0 + offset));
}

// Vertex buffer binding
void Context::bindVertexBuffer(UInt32 id)
{
    if ((id != m_currentVaoState->vbo) || (id == 0)) {
		glBindBuffer(GL_ARRAY_BUFFER, id);
		m_currentVaoState->vbo = id;
	}
}

// Delete a vertex buffer object.
void Context::deleteVertexBuffer(UInt32 id)
{
    if (id != 0) {
        if (id == m_currentVaoState->vbo) {
			m_currentVaoState->vbo = 0;
        }

		glDeleteBuffers(1, (GLuint*)&id);
	}
}

// Delete a vertex array object.
void Context::deleteVertexArray(UInt32 vao)
{
    if (vao != 0) {
        if (vao == m_currentVAOId) {
			m_currentVAOId = 0;
			m_currentVaoState = &m_defaultVaoState;

			glBindVertexArray(m_defaultVAOId/*0*/);
		}

		glDeleteVertexArrays(1, &vao);
	}
}

// Vertex array object binding.
void Context::bindVertexArray(UInt32 id, VertexArrayState *vaoState)
{
    if (id == 0) {
		glBindVertexArray(m_defaultVAOId/*0*/);

		m_currentVaoState = &m_defaultVaoState;
		m_currentVAOId = 0/*m_defaultVAOId*/;
		
		return;
    } else if (id != m_currentVAOId) {
		glBindVertexArray(id);

		m_currentVaoState = vaoState;
		m_currentVAOId = id;

		O3D_ASSERT(m_currentVaoState);
	}
}

// Index buffer object binding.
void Context::bindIndexBuffer(UInt32 id)
{
    if ((id != m_currentVaoState->ibo) || (id == 0)) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		m_currentVaoState->ibo = id;
	}
}

// Delete an index buffer object.
void Context::deleteIndexBuffer(UInt32 id)
{
    if (id != 0) {
		if (id == m_currentVaoState->ibo)
			m_currentVaoState->ibo = 0;

		glDeleteBuffers(1, (GLuint*)&id);
	}
}

// Shader object binding.
void Context::bindShader(UInt32 id)
{
	if ((id != m_currentShaderId) || (id == 0))	{
		glUseProgram(id);
		m_currentShaderId = id;
	}
}

// Frame buffer object binding.
void Context::bindFrameBuffer(UInt32 id)
{
    if ((id != m_currentFBOId) || (id == 0)) {
		glBindFramebuffer(GL_FRAMEBUFFER, id);
		m_currentFBOId = id;
	}
}

void Context::bindPixelPackBuffer(UInt32 id)
{
    if ((id != m_currentPackPBOId) || (id == 0)) {
		glBindBuffer(GL_PIXEL_PACK_BUFFER, id);
		m_currentPackPBOId = id;
	}
}

void Context::bindPixelUnpackBuffer(UInt32 id)
{
    if ((id != m_currentUnpackPBOId) || (id == 0)) {
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, id);
		m_currentUnpackPBOId = id;
	}
}

void Context::deletePixelBuffer(UInt32 id)
{
    if (id != 0) {
        if (id == m_currentPackPBOId) {
			m_currentPackPBOId = 0;
        }

        if (id == m_currentUnpackPBOId) {
			m_currentUnpackPBOId = 0;
        }

		glDeleteBuffers(1, (GLuint*)&id);
	}
}

// create a new occlusion query and set it to current
OcclusionQuery* Context::createOcclusionQuery()
{
	OcclusionQuery* occQuery = new OcclusionQuery;
	return occQuery;
}

// simple draw mode
void Context::simpleDrawMode()
{
    if (m_useSimpleDrawMode) {
		O3D_ERROR(E_InvalidOperation("The context is already in simple draw mode"));
    }

	bindTexture(TEXTURE_2D, 0);

	setDefaultDepthTest();
	setDefaultDepthWrite();
	setDefaultDoubleSide();
	setDefaultDepthRange();
	setDefaultCullingMode();

    m_blending.setDefaultFunc();

	m_useSimpleDrawMode = True;
}

// normal draw mode
void Context::normalDrawMode()
{
    if (!m_useSimpleDrawMode) {
		O3D_ERROR(E_InvalidOperation("The context is already in normal draw mode"));
    }

	// restore override drawing mode
    if (isOverrideDrawingMode()) {
		setDrawingMode(m_overrideDrawing);
    }

	m_useSimpleDrawMode = False;
}

// set all parameters to the current context
void Context::setAllParameters()
{
	setDrawingMode(m_drawingMode);
	setCullingMode(m_cullingMode);

	setPointSize(m_pointSize);

	forceDepthTest(m_isDepthTest);

	setBackgroundColor(m_backgroundColor);

	setDefaultDepthClear();
	setDefaultDepthRange();

	setDefaultAntiAliasing();
	setDefaultDepthWrite();
	setDefaultDoubleSide();
	setDefaultScissorTest();

    m_blending.setDefaultFunc();
    m_blending.setDefaultEquation();
}

// Change culling mode
CullingMode Context::setCullingMode(CullingMode mode)
{
    if (m_cullingMode != mode) {
		// enable only if it was previously disabled
        if (m_cullingMode == CULLING_NONE) {
			glEnable(GL_CULL_FACE);
        }

		CullingMode old = m_cullingMode;
		m_cullingMode = mode;

        if (m_cullingMode == CULLING_FRONT_FACE) {
			glCullFace(GL_FRONT);
			return old;
        } else if (m_cullingMode == CULLING_BACK_FACE) {
			glCullFace(GL_BACK);
			return old;
        } else if (m_cullingMode == CULLING_NONE) {
			glDisable(GL_CULL_FACE);
			return old;
		}
	}
	return m_cullingMode;
}

// Force change culling mode
CullingMode Context::forceCullingMode(CullingMode mode)
{
	CullingMode old = m_cullingMode;
	m_cullingMode = mode;

    if (m_cullingMode == CULLING_FRONT_FACE) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		return old;
    } else if (m_cullingMode == CULLING_BACK_FACE) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		return old;
    } else if (m_cullingMode == CULLING_NONE) {
		glDisable(GL_CULL_FACE);
		return old;
	}
	return m_cullingMode;
}

// change point size
Float Context::setPointSize(Float val)
{
    if (val == m_pointSize) {
		return m_pointSize;
    }

	Float old = m_pointSize;
	m_pointSize = val;
	
	glPointSize(m_pointSize);

	return old;
}

// force point size
Float Context::forcePointSize(Float val)
{
	Float old = m_pointSize;
	m_pointSize = val;

	glPointSize(m_pointSize);

	return old;
}

// change point size
Float Context::modifyPointSize(Float val)
{
	Float old = m_pointSize;
	m_pointSize += val;

	glPointSize(m_pointSize);

	return old;
}

// Enable depth test.
Bool Context::enableDepthTest()
{
    if (!m_isDepthTest) {
		m_isDepthTest = True;
		glEnable(GL_DEPTH_TEST);

		return False;
	}
	return True;
}

// Disable depth test.
Bool Context::disableDepthTest()
{
    if (m_isDepthTest) {
		m_isDepthTest = False;
		glDisable(GL_DEPTH_TEST);

		return True;
	}
	return False;
}

// Toggle depth test.
Bool Context::toggleDepthTest()
{
    if (m_isDepthTest) {
		glDisable(GL_DEPTH_TEST);
        m_isDepthTest = False;

        return False;
    } else {
		glEnable(GL_DEPTH_TEST);
        m_isDepthTest = True;

        return True;
	}
}

// Force depth test state.
Bool Context::forceDepthTest(Bool mode)
{
	Bool old = m_isDepthTest;
	m_isDepthTest = mode;

    if (mode) {
		glEnable(GL_DEPTH_TEST);
    } else {
		glDisable(GL_DEPTH_TEST);
    }

	return old;
}

// Change drawing mode.
Context::DrawingMode Context::setDrawingMode(DrawingMode mode)
{
	DrawingMode old = mode;

    if (m_drawingMode != mode) {
        switch (mode) {
			case DRAWING_FILLED:
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				break;
			case DRAWING_WIREFRAME:
				glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
				break;
			case DRAWING_POINT:
				glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
				break;
		}
		m_drawingMode = mode;
	}
	return old;
}

// Force drawing mode.
Context::DrawingMode Context::forceDrawingMode(DrawingMode mode)
{
	DrawingMode old = mode;
	m_drawingMode = mode;

    switch (mode) {
		case DRAWING_FILLED:
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			break;
		case DRAWING_WIREFRAME:
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			break;
		case DRAWING_POINT:
			glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
			break;
	}
	return old;
}

// Override all materials drawing mode (useful for viewers).
Context::DrawingMode Context::setOverrideDrawingMode(DrawingMode mode)
{
	DrawingMode old = mode;
	m_overrideDrawing = m_drawingMode = mode;

    switch (mode) {
		case DRAWING_FILLED:
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			m_overrideDrawingMode = True;
			m_overrideDoubleSided = False;
			break;
		case DRAWING_WIREFRAME:
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			m_overrideDrawingMode = True;
			m_overrideDoubleSided = True;
			break;
		case DRAWING_POINT:
			glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
			m_overrideDrawingMode = True;
			m_overrideDoubleSided = True;
			break;
	}
	return old;
}

// Restore to material drawing mode.
void Context::materialsDrawingMode()
{
	m_drawingMode = DRAWING_FILLED;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	m_overrideDrawingMode = False;
	m_overrideDoubleSided = False;
}

// change anti aliasing mode
Context::AntiAliasingMethod Context::setAntiAliasing(AntiAliasingMethod mode)
{
    AntiAliasingMethod old = m_antiAliasing;

    if (old != mode) {
        if (mode == AA_HINT_NICEST) {
            if (old == AA_MULTI_SAMPLE) {
                glDisable(GL_MULTISAMPLE);
            }

            //glEnable(GL_POINT_SMOOTH);
            //glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

			glEnable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

			glEnable(GL_POLYGON_SMOOTH);
            glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        } else if (mode == AA_HINT_FASTEST) {
            if (old == AA_MULTI_SAMPLE) {
                glDisable(GL_MULTISAMPLE);
            }

            //glEnable(GL_POINT_SMOOTH);
            //glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);

            glEnable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);

            glEnable(GL_POLYGON_SMOOTH);
            glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
        } else if (mode == AA_MULTI_SAMPLE) {
            if (old != AA_NONE) {
                //glDisable(GL_POINT_SMOOTH);
                glDisable(GL_LINE_SMOOTH);
                glDisable(GL_POLYGON_SMOOTH);
            }

            glEnable(GL_MULTISAMPLE);
        } else {
            // NO_AA
            if (old == AA_MULTI_SAMPLE) {
                glDisable(GL_MULTISAMPLE);
            } else {
                //glDisable(GL_POINT_SMOOTH);
                glDisable(GL_LINE_SMOOTH);
                glDisable(GL_POLYGON_SMOOTH);
            }
		}
        m_antiAliasing = mode;
	}

	return old;
}

// force anti aliasing mode
Context::AntiAliasingMethod Context::forceAntiAliasing(AntiAliasingMethod mode)
{
    AntiAliasingMethod old = m_antiAliasing;
    m_antiAliasing = mode;

    if (mode == AA_HINT_NICEST) {
        glDisable(GL_MULTISAMPLE);

        //glEnable(GL_POINT_SMOOTH);
        //glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

		glEnable(GL_POLYGON_SMOOTH);
		glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
    } else if (mode == AA_HINT_FASTEST) {
        glDisable(GL_MULTISAMPLE);

        //glEnable(GL_POINT_SMOOTH);
        //glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);

        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

        glEnable(GL_POLYGON_SMOOTH);
        glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
    } else if (mode == AA_MULTI_SAMPLE) {
        glEnable(GL_MULTISAMPLE);
    } else {
        glDisable(GL_MULTISAMPLE);
        //glDisable(GL_POINT_SMOOTH);
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_POLYGON_SMOOTH);
	}

	return old;
}

// Change the background clear color.
Color Context::setBackgroundColor(const Color &color)
{
	Color old = m_backgroundColor;

	m_backgroundColor = color;
    glClearColor(color.r(), color.g(), color.b(), color.a());

	return old;
}

// Change the background clear color.
Color Context::setBackgroundColor(Float r, Float g, Float b, Float a)
{
	Color old = m_backgroundColor;

	m_backgroundColor.set(r,g,b,a);
	glClearColor(r,g,b,a);

	return old;
}

// Change depth buffer clear value.
Float Context::setDepthClear(Float val)
{
    Float old = m_depthClear;

    if (old != val) {
		m_depthClear = val;
		glClearDepth(val);
	}
	return old;
}

// Force depth buffer clear value.
Float Context::forceDepthClear(Float val)
{
    Float old = m_depthClear;

	m_depthClear = val;
	glClearDepth(val);

	return old;
}

// Change stencil clear value.
Int32 Context::setStencilClear(Int32 val)
{
    Int32 old = m_stencilClear;

    if (old != val) {
		m_stencilClear = val;
		glClearStencil(val);
	}
	return old;
}

// Force stencil clear value.
Int32 Context::forceStencilClear(Int32 val)
{
    Int32 old = m_stencilClear;

	m_stencilClear = val;
	glClearStencil(val);

	return old;
}

// Change depth buffer range.
void Context::setDepthRange(Float _near, Float _far)
{
    if ((m_depthNear != _near) || (m_depthFar != _far)) {
		m_depthNear = _near;
		m_depthFar = _far;

		glDepthRange(_near,_far);
	}
}

// Force depth buffer range.
void Context::forceDepthRange(Float _near, Float _far)
{
	m_depthNear = _near;
	m_depthFar = _far;

	glDepthRange(_near,_far);
}

// Change depth buffer comparison function.
void Context::setDepthFunc(Comparison _func)
{
    if (_func != m_depthFunc) {
		m_depthFunc = _func;
		glDepthFunc(_func);
	}
}

// Force depth buffer comparison function.
void Context::forceDepthFunc(Comparison _func)
{
	m_depthFunc = _func;
	glDepthFunc(_func);
}

// Enable depth write.
Bool Context::enableDepthWrite()
{
    if (!m_isDepthWrite) {
		glDepthMask(GL_TRUE);
		m_isDepthWrite = True;

		return False;
	}
	return True;
}

// Disable depth write.
Bool Context::disableDepthWrite()
{
    if (m_isDepthWrite) {
		glDepthMask(GL_FALSE);
		m_isDepthWrite = False;

		return True;
	}
	return False;
}

// Toggle depth write state.
Bool Context::toggleDepthWrite()
{
    if (m_isDepthWrite) {
		glDepthMask(GL_FALSE);
		return m_isDepthWrite = False;
    } else {
		glDepthMask(GL_TRUE);
		return m_isDepthWrite = True;
	}
}

// Force depth write state.
Bool Context::forceDepthWrite(Bool mode)
{
    Bool old = m_isDepthWrite;
	m_isDepthWrite = mode;

    if (m_isDepthWrite) {
		glDepthMask(GL_TRUE);
    } else {
		glDepthMask(GL_FALSE);
    }

    return old;
}

// Enable double side rendering.
Bool Context::enableDoubleSide()
{
    if (!m_isDoubleSided) {
		glDisable(GL_CULL_FACE);
		m_isDoubleSided = True;

		return False;
	}
	return True;
}

// Disable double side rendering.
Bool Context::disableDoubleSide()
{
    if (m_isDoubleSided) {
		glEnable(GL_CULL_FACE);
		m_isDoubleSided = False;

		return True;
	}
	return False;
}

// Toggle double side rendering state.
Bool Context::toggleDoubleSide()
{
    if (m_isDoubleSided) {
		glEnable(GL_CULL_FACE);
        m_isDoubleSided = False;

        return False;
    } else {
		glDisable(GL_CULL_FACE);
        m_isDoubleSided = True;

        return True;
	}
}

// Force double side rendering state.
Bool Context::forceDoubleSide(Bool mode)
{
    Bool old = m_isDoubleSided;
	m_isDoubleSided = mode;

    if (m_isDoubleSided) {
		glDisable(GL_CULL_FACE);
    } else {
		glEnable(GL_CULL_FACE);
    }

    return old;
}

// Enable scissor test.
Bool Context::enableScissorTest()
{
    if (!m_isScissorTest) {
		glEnable(GL_SCISSOR_TEST);
		m_isScissorTest = True;

		return False;
	}
	return True;
}

// Disable scissor test.
Bool Context::disableScissorTest()
{
    if (m_isScissorTest) {
		glDisable(GL_SCISSOR_TEST);
		m_isScissorTest = False;

		return True;
	}
	return False;
}

// Toggle scissor test state.
Bool Context::toggleScissorTest()
{
    if (m_isScissorTest) {
		glDisable(GL_SCISSOR_TEST);
        m_isScissorTest = False;

        return False;
    } else {
		glEnable(GL_SCISSOR_TEST);
        m_isScissorTest = True;

        return True;
	}
}

// Force scissor test state.
Bool Context::forceScissorTest(Bool mode)
{
    Bool old = m_isScissorTest;
	m_isScissorTest = mode;

    if (m_isScissorTest) {
		glEnable(GL_SCISSOR_TEST);
    } else {
		glDisable(GL_SCISSOR_TEST);
    }

    return old;
}

// Define the scissor size (Y up X right).
void Context::setScissor(Int32 xpos,Int32 ypos,Int32 width,Int32 height)
{
	if ((width != m_scissor.width()) || (height != m_scissor.height()) ||
        (xpos != m_scissor.x()) || (ypos != m_scissor.y())) {
		glScissor(xpos,ypos,width,height);
		m_scissor.set(xpos, ypos, width, height);
	}
}

// Define the scissor given a Box2i.
void Context::setScissor(const Box2i &scissor)
{
	if ((scissor.width() != m_scissor.width()) || (scissor.height() != m_scissor.height()) ||
        (scissor.x() != m_scissor.x()) || (scissor.y() != m_scissor.y())) {
		glScissor(scissor.x(), scissor.y(), scissor.width(), scissor.height());
		m_scissor = scissor;
	}
}

// Define the scissor size given a ptr (4 int: x,y,width,height).
void Context::setScissor(Int32 *ptr)
{
	if ((ptr[2] != m_scissor.width()) || (ptr[3] != m_scissor.height()) ||
        (ptr[0] != m_scissor.x()) || (ptr[1] != m_scissor.y())) {
		glScissor(ptr[0],ptr[1],ptr[2],ptr[3]);
		m_scissor.set(ptr);
	}
}

// Enable Stencil test.
Bool Context::enableStencilTest()
{
    if (!m_isStencilTest) {
		glEnable(GL_STENCIL_TEST);
		m_isStencilTest = True;

		return False;
	}
	return True;
}

// Disable Stencil test.
Bool Context::disableStencilTest()
{
    if (m_isStencilTest) {
		glDisable(GL_STENCIL_TEST);
		m_isStencilTest = False;

		return True;
	}
	return False;
}

// Toggle Stencil test state.
Bool Context::toggleStencilTest()
{
    if (m_isStencilTest) {
		glDisable(GL_STENCIL_TEST);
        m_isStencilTest = False;

        return False;
    } else {
		glEnable(GL_STENCIL_TEST);
        m_isStencilTest = True;

        return True;
	}
}

// Force Stencil test state.
Bool Context::forceStencilTest(Bool mode)
{
    Bool old = m_isStencilTest;
	m_isStencilTest = mode;

    if (m_isStencilTest) {
		glEnable(GL_STENCIL_TEST);
    } else {
		glDisable(GL_STENCIL_TEST);
    }

    return old;
}

// Set the stencil test function.
void Context::setStencilTestFunc(Comparison func, Int32 ref, UInt32 mask)
{
    if (func != m_stencilTestFunc || ref != m_stencilTestRef || mask != m_stencilTestMask) {
		glStencilFunc(func, ref, mask);

		m_stencilTestFunc = func;
		m_stencilTestRef = ref;
		m_stencilTestMask = mask;
	}
}

// Set the stencil test function.
void Context::forceStencilTestFunc(Comparison func, Int32 ref, UInt32 mask)
{
	glStencilFunc(func, ref, mask);

	m_stencilTestFunc = func;
	m_stencilTestRef = ref;
	m_stencilTestMask = mask;
}

// Enable an OpenGL state.
void Context::enable(UInt32 mode)
{
	glEnable(mode);
}

// Disable an OpenGL state.
void Context::disable(UInt32 mode)
{
	glDisable(mode);
}

// Define the viewport.
void Context::setViewPort(Int32 xpos, Int32 ypos, Int32 width, Int32 height)
{
	if ((width != m_viewPort.width()) || (height != m_viewPort.height()) ||
        (xpos != m_viewPort.x()) || (ypos != m_viewPort.y())) {
		glViewport(xpos, ypos, width, height);
		m_viewPort.set(xpos, ypos, width, height);
	}
}

// Define the viewport given a Box2i.
void Context::setViewPort(const Box2i &viewPort)
{
	if ((viewPort.width() != m_viewPort.width()) || (viewPort.height() != m_viewPort.height()) ||
        (viewPort.x() != m_viewPort.x()) || (viewPort.y() != m_viewPort.y())) {
		glViewport(viewPort.x(), viewPort.y(), viewPort.width(), viewPort.height());
		m_viewPort = viewPort;
	}
}

// Define the viewport given a ptr (4 int: x,y,width,height).
void Context::setViewPort(Int32 *ptr)
{
	if ((ptr[2] != m_viewPort.width()) || (ptr[3] != m_viewPort.height()) ||
        (ptr[0] != m_viewPort.x()) || (ptr[1] != m_viewPort.y())) {
		glViewport(ptr[0], ptr[1], ptr[2], ptr[3]);
		m_viewPort.set(ptr);
	}
}

// Change the color mask.
void Context::setColorMask(Bool r, Bool g, Bool b, Bool a)
{
	glColorMask(r,g,b,a);
}

// Enable color write.
void Context::enableColorWrite()
{
	glColorMask(1,1,1,1);
}

// Disable color write.
void Context::disableColorWrite()
{
	glColorMask(0,0,0,0);
}

// Get the modelview-projection matrix.
const Matrix4& Context::modelViewProjection()
{
    if (m_recomputeModelViewProjection) {
		projection().get().mult(modelView().get(), m_modelViewProjection);
		m_recomputeModelViewProjection = False;
	}

	return m_modelViewProjection;
}

// Get the normal matrix.
const Matrix3& Context::normalMatrix()
{
    if (m_recomputeNormalMatrix) {
        m_normalMatrix = modelView().get().getRotation().invert().transposeTo();
		m_recomputeNormalMatrix = False;
	}

	return m_normalMatrix;
}

// project a point from world space to window space.
Vector3 Context::projectPoint(const Vector3 &v)
{
	return Matrix::projectPoint(projection().get(), modelView().get(), m_viewPort, v);
}

// unproject a point from window space to world space.
Vector3 Context::unprojectPoint(const Vector3 &v)
{
	return Matrix::unprojectPoint(projection().get(), modelView().get(), m_viewPort, v);
}

void Context::updateMatrix(const Matrix *matrix)
{
	m_recomputeModelViewProjection = True;
	m_recomputeNormalMatrix = True;
}
