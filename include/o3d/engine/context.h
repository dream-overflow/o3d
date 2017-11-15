/**
 * @file context.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CONTEXT_H
#define _O3D_CONTEXT_H

#include "o3d/image/color.h"

#include "matrix.h"
#include "blending.h"
#include "vertexarraystate.h"
#include "enginetype.h"

#include "o3d/core/memorydbg.h"

namespace o3d {

class Renderer;
class OcclusionQuery;

/**
 * @brief OpenGL wrapper and context
 * @date 2002-09-04
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class O3D_API Context : public MatrixObserver
{
public:

	//! Default constructor.
	//! @param renderer The owner.
	Context(Renderer *renderer);

	//! Destructor
	virtual ~Context();

	//! Set all default parameters to OpenGL current context.
	void setAllParameters();

	//-----------------------------------------------------------------------------------
	// Helper drawing objects
	//-----------------------------------------------------------------------------------

	//! Check if the simple draw mode is the current mode.
	inline Bool isSimpleDrawInstance() const { return m_useSimpleDrawMode; }

	//! Simple drawing mode mean rendering with all settings to theirs default values,
	//! and with texture 2d disabled.
	void simpleDrawMode();

	//! Normal drawing mode mean enable texture 2d. Always call him when finish
	//! to use of a SimpleDrawMode.
	void normalDrawMode();

	//-----------------------------------------------------------------------------------
	// TEXTURE (see Texture)
	//-----------------------------------------------------------------------------------

	//! Define the current active texture unit.
	void setActiveTextureUnit(UInt32 unit);
	//! Get the current active texture unit.
	inline UInt32 getActiveTextureUnit() const { return m_currentTexUnit; }

	//! Texture binding on the active texture unit.
	void bindTexture(TextureType type, UInt32 id, Bool force = False);

	//! Delete texture object.
    void deleteTexture(UInt32 id);

	//! Get the current bound texture for the active texture unit.
	inline UInt32 getCurrentTexture() const { return m_currentTexId[m_currentTexUnit]; }

    /**
     * @brief Define the current texture sampler for a given texture unit.
     * @param id Sampler object id.
     * @param unit Texture unit where to bind it.
     * @param force If True force to rebind even it is currentry bound to the unit.
     */
    void bindSampler(UInt32 id, UInt32 unit, Bool force = False);

    //! Delete a sampler object.
    void deleteSampler(UInt32 id);

    //! Get the current bound sampler for a texture unit.
    inline UInt32 getSampler(UInt32 unit) const
    {
        O3D_ASSERT(unit < UInt32(m_maxCombinedTextureImageUnits));
        return m_currentSamplerId[unit];
    }

	//-----------------------------------------------------------------------------------
	// Vertex attribute array (see VertexElement)
	//-----------------------------------------------------------------------------------

	//! Enable a vertex attribute array.
	void enableVertexAttribArray(UInt32 array);
	//! Disable a vertex attribute array.
	void disableVertexAttribArray(UInt32 array);
	//! Get the current status of a specified vertex attribute array.
	inline Bool getVertexAttribArray(UInt32 array) const
	{
		O3D_ASSERT(array < UInt32(m_maxVertexAttribs));
		return m_currentVaoState->attributeArray[array];
	}
	//! Force the state of a vertex attribute array.
	void forceVertexAttribArray(UInt32 array, Bool state);

	//! Attribute vertex array. Enable the vertex attribute array if necessary.
	void vertexAttribArray(
			UInt32 array,
			UInt32 eltSize,
			UInt32 stride,
			UInt32 offset);

	//-----------------------------------------------------------------------------------
	// VBO (see VertexElement and VertexBuffer)
	//-----------------------------------------------------------------------------------

	//! Vertex buffer object binding.
	void bindVertexBuffer(UInt32 id);

	//! Delete a vertex buffer object.
	//! If the current bound vertex buffer object is the same as deleted,
	//! the vertex buffer object is bind to 0.
	void deleteVertexBuffer(UInt32 id);

	//! Get the current bound vertex buffer object.
	inline UInt32 getCurrentVertexBuffer() const { return m_currentVaoState->vbo; }

	//-----------------------------------------------------------------------------------
	// VAO (see VertexArray)
	//-----------------------------------------------------------------------------------

	//! Vertex array object binding.
	//! @param id Vertex array object identifier.
	//! @param vaoState Valid pointer (or NULL if id is 0) to a VertexArrayState structure.
	//!                 It must stay valid during the bind usage.
	void bindVertexArray(UInt32 id, VertexArrayState *vaoState);

	//! Delete a vertex array object.
	//! @param vao Vertex array.
	void deleteVertexArray(UInt32 vao);

	//! Get the current bound vertex array object.
	inline UInt32 getCurrentVertexArray() const { return m_currentVAOId; }

	//-----------------------------------------------------------------------------------
	// IBO (see FaceArray and VertexBuffer)
	//-----------------------------------------------------------------------------------

	//! Index buffer object binding.
	void bindIndexBuffer(UInt32 id);
	//! Get the current bound index buffer object.
	inline UInt32 getCurrentIndexBuffer() const { return m_currentVaoState->ibo; }

	//! Delete an index buffer object.
	//! If the current bound index buffer object is the same as deleted,
	//! the vertex index object is bind to 0.
	void deleteIndexBuffer(UInt32 id);

	//-----------------------------------------------------------------------------------
	// GLSL Shader (see Shader and ShaderInstance)
	//-----------------------------------------------------------------------------------

	//! GLSL Shader program binding.
	void bindShader(UInt32 id);
	//! Get the current bound GLSL shader program.
	inline UInt32 getCurrentShader() const { return m_currentShaderId; }

	//-----------------------------------------------------------------------------------
	// FBO (see FrameBuffer)
	//-----------------------------------------------------------------------------------

	//! Frame buffer object binding (@see FrameBuffer).
	void bindFrameBuffer(UInt32 id);
	//! Get the current bound frame buffer object  (@see FrameBuffer).
	inline UInt32 getCurrentFrameBuffer() const { return m_currentFBOId; }

	//-----------------------------------------------------------------------------------
	// PBO (see PixelBuffer)
	//-----------------------------------------------------------------------------------

	//! PixelBuffer Pack object binding.
	void bindPixelPackBuffer(UInt32 id);
	//! PixelBuffer Unpack object binding.
	void bindPixelUnpackBuffer(UInt32 id);

	//! Delete a pixel buffer object.
	//! If the current bound pixel buffer object is the same as deleted,
	//! the pixel buffer object is bind to 0.
	void deletePixelBuffer(UInt32 id);

	//! Get the current bound pixel Pack buffer object.
	inline UInt32 getCurrentPixelPackBuffer() const { return m_currentPackPBOId; }
	//! Get the current bound pixel Unpack buffer object.
	inline UInt32 getCurrentPixelUnpackBuffer() const { return m_currentUnpackPBOId; }

	//-----------------------------------------------------------------------------------
	// Occlusion query
	//-----------------------------------------------------------------------------------

	//! Set current occlusion query object.
	inline Bool setCurrentOcclusionQuery(OcclusionQuery *occQuery)
	{
		if (m_currentOccQuery != occQuery)
		{
			m_currentOccQuery = occQuery;
			return True;
		}
		return False;
	}

	//! Get current occlusion query object.
	inline OcclusionQuery* getCurrentOcclusionQuery() const { return m_currentOccQuery; }

	//! Create a new occlusion query and set it to current.
	OcclusionQuery* createOcclusionQuery();

	//-----------------------------------------------------------------------------------
	// Culling
	//-----------------------------------------------------------------------------------

	//! Change the culling mode.
	CullingMode setCullingMode(CullingMode mode);

	//! Get the current culling mode.
	inline CullingMode getCullingMode() const { return m_cullingMode; }

	//! Force culling mode.
    CullingMode forceCullingMode(CullingMode mode);

	//! Set default culling mode.
	inline CullingMode setDefaultCullingMode() { return setCullingMode(CULLING_BACK_FACE); }
	//! Force default culling mode.
	inline CullingMode forceDefaultCullingMode() { return forceCullingMode(CULLING_BACK_FACE); }

	//-----------------------------------------------------------------------------------
	// Line parameters
	//-----------------------------------------------------------------------------------

	//! Change the line width.
	Float setLineSize(Float val);

	//! Get the line width.
	inline Float getLineSize() const { return m_lineSize; }

	//! Force the line width.
	Float forceLineSize(Float val);

	//! Increment by val the line width.
	Float modifyLineSize(Float val);

	//! Set default line size.
    inline Float setDefaultLineSize() { return setLineSize(1.f); }
	//! Force default line size.
    inline Float forceDefaultLineSize() { return forceLineSize(1.f); }

	//-----------------------------------------------------------------------------------
	// Point parameters
	//-----------------------------------------------------------------------------------

	//! Change the point size.
	Float setPointSize(Float val);

	//! Return the point size.
	inline Float getPointSize() const { return m_pointSize; }

	//! Force the point size.
	Float forcePointSize(Float val);

	//! Increment by val the point size.
	Float modifyPointSize(Float val);

	//! Set default point size.
    inline Float setDefaultPointSize() { return setPointSize(1.f); }
	//! Force default point size.
    inline Float forceDefaultPointSize() { return forcePointSize(1.f); }

	//-----------------------------------------------------------------------------------
	// Blending
	//-----------------------------------------------------------------------------------

    //! Get the blending controller (read only).
    const Blending& blending() const { return m_blending; }
    //! Get the blending controller.
    Blending& blending() { return m_blending; }

	//-----------------------------------------------------------------------------------
	// Depth parameters
	//-----------------------------------------------------------------------------------

	//! Enable depth test.
	Bool enableDepthTest();

	//! Disable depth test.
	Bool disableDepthTest();

	//! Toggle depth test.
	Bool toggleDepthTest();

	//! Force depth test state.
	Bool forceDepthTest(Bool mode);

	//! Set default depth test state.
	inline Bool setDefaultDepthTest() { return enableDepthTest(); }
	//! Force default depth test state.
	inline Bool forceDefaultDepthTest() { return forceDepthTest(True); }

	//! Return current depth test state.
	inline Bool getDepthTestingMode() const { return m_isDepthTest; }

	//-----------------------------------------------------------------------------------
	// Polygons parameters
    //-----------------------------------------------------------------------------------

	//! Polygons drawing modes.
	enum DrawingMode
	{
		DRAWING_POINT,                //!< draw vertex points.
		DRAWING_WIREFRAME,            //!< draw lines.
		DRAWING_FILLED                //!< fill polygons (colors...).
	};

	//! Change drawing mode.
	DrawingMode setDrawingMode(DrawingMode mode);

	//! Force drawing mode.
	DrawingMode forceDrawingMode(DrawingMode mode);

	//! Set default drawing mode.
	inline DrawingMode setDefaultDrawingMode()   { return setDrawingMode(DRAWING_FILLED); }
	//! Force default drawing mode.
	inline DrawingMode forceDefaultDrawingMode() { return forceDrawingMode(DRAWING_FILLED); }

	//! Return current drawing mode.
	inline DrawingMode getDrawingMode() const { return m_drawingMode; }

	//! Override all materials drawing mode (useful for viewers).
	DrawingMode setOverrideDrawingMode(DrawingMode mode);

	//! Restore to material drawing mode.
	void materialsDrawingMode();

	//! Is drawing mode override.
	inline Bool isOverrideDrawingMode() { return m_overrideDrawingMode; }
	//! Is double-sided mode override.
	inline Bool isOverrideDoubleSided() { return m_overrideDoubleSided; }

	//-----------------------------------------------------------------------------------
	// Anti-aliasing
	//-----------------------------------------------------------------------------------

    enum AntiAliasingMethod
    {
        AA_NONE = 0,           //!< Disabled.
        AA_HINT_FASTEST = 1,   //!< Mostly software.
        AA_HINT_NICEST = 2,    //!< Mostly software.
        AA_MULTI_SAMPLE = 3    //!< Need hardware multi-sample support.
    };

    //! Change anti-aliasing mode.
    AntiAliasingMethod setAntiAliasing(AntiAliasingMethod mode);

    //! Force anti-aliasing mode.
    AntiAliasingMethod forceAntiAliasing(AntiAliasingMethod mode);

	//! Set default anti-aliasing state.
    inline Bool setDefaultAntiAliasing()   { return setAntiAliasing(AA_NONE); }
	//! Force default anti-aliasing state.
    inline Bool forceDefaultAntiAliasing() { return forceAntiAliasing(AA_NONE); }

	//! Return current anti-aliasing state.
    inline Bool getAntiAliasing() const { return m_antiAliasing; }

	//-----------------------------------------------------------------------------------
	// Background color
    //-----------------------------------------------------------------------------------

	//! Change the background clear color.
	Color setBackgroundColor(const Color &color);

	//! Change the background clear color.
	Color setBackgroundColor(Float r, Float g, Float b, Float a);

	//! Force the background clear color.
	inline Color forceBackgroundColor(const Color &color)
	{
		return setBackgroundColor(color);
	}

	//! Force the background clear color.
	inline Color forceBackgroundColor(Float r,Float g,Float b,Float a)
	{
		return setBackgroundColor(r,g,b,a);
	}

	//! Set default background clear color.
	inline Color setDefaultBackgroundColor() { return setBackgroundColor(0.f,0.f,0.f,1.f); }
	//! Force default background clear color.
	inline Color forceDefaultBackgroundColor() { return forceBackgroundColor(0.f,0.f,0.f,1.f); }

	//! Get the background clear color
	inline Color getBackgroundColor() const { return m_backgroundColor; }

	//-----------------------------------------------------------------------------------
	// DepthBuffer clear
    //-----------------------------------------------------------------------------------

	//! Change depth buffer clear value.
	Float setDepthClear(Float val);
    //! Get the depth buffer clear value.
    inline Float getDepthClear() const { return m_depthClear; }

	//! Force depth buffer clear value.
	Float forceDepthClear(Float val);

	//! Set default depth buffer clear value.
	inline Float setDefaultDepthClear() { return setDepthClear(1.f); }
	//! Force default depth buffer clear value.
	inline Float forceDefaultDepthClear() { return forceDepthClear(1.f); }

	//-----------------------------------------------------------------------------------
	// StencilBuffer clear
    //-----------------------------------------------------------------------------------

	//! Change stencil clear value.
	Int32 setStencilClear(Int32 val);
    //! Get the stencil clear value.
    Int32 getStencilClear() const { return m_stencilClear; }

	//! Force stencil clear value.
	Int32 forceStencilClear(Int32 val);

	//! Set default stencil clear value.
	inline Int32 setDefaultStencilClear()   { return setStencilClear(0); }
	//! Force default stencil clear value.
	inline Int32 forceDefaultStencilClear() { return forceStencilClear(0); }

	//-----------------------------------------------------------------------------------
	// DepthBuffer clipping
    //-----------------------------------------------------------------------------------

	//! Change depth buffer range.
	void setDepthRange(Float _near,Float _far);

	//! Force depth buffer range.
	void forceDepthRange(Float _near,Float _far);

	//! Force depth range in infinite mode force to write 1.0 in depth buffer.
	inline void setInfiniteDepthRange() { forceDepthRange(0.999f,1.0f); }

	//! Returns the depth buffer near bounds.
	inline Float getNearDepthRange() const { return m_depthNear; }
	//! Returns the depth buffer far bounds.
	inline Float getFarDepthRange() const { return m_depthFar; }

	//! Set default depth buffer range.
	inline void setDefaultDepthRange() { setDepthRange(0.f,1.f); }
	//! Force default depth buffer range.
	inline void forceDefaultDepthRange() { forceDepthRange(0.f,1.f); }

	//-----------------------------------------------------------------------------------
	// DepthBuffer comparison function
    //-----------------------------------------------------------------------------------

	//! Change depth buffer comparison function.
	void setDepthFunc(Comparison _func);

	//! Force depth buffer comparison function.
	void forceDepthFunc(Comparison _func);

	//! Returns the depth buffer comparison function.
	inline Comparison getDepthFunc() const { return m_depthFunc; }

	//! Set default depth buffer comparison function.
	inline void setDefaultDepthFunc() { setDepthFunc(COMP_LESS); }
	//! Force default depth buffer comparison function.
	inline void forceDefaultDepthFunc() { forceDepthFunc(COMP_LESS); }

	//-----------------------------------------------------------------------------------
	// DepthBuffer writing method
	//-----------------------------------------------------------------------------------

	//! Enable depth write.
	Bool enableDepthWrite();

	//! Disable depth write.
	Bool disableDepthWrite();

	//! Toggle depth write state.
	Bool toggleDepthWrite();

	//! Force depth write state.
	Bool forceDepthWrite(Bool mode);

	//! Set default depth write state.
	inline Bool setDefaultDepthWrite() { return enableDepthWrite(); }
	//! Force default depth write state.
	inline Bool forceDefaultDepthWrite() { return forceDepthWrite(True); }

	//! Get current depth write state.
	inline Bool getDepthWriteMode()const { return m_isDepthWrite; }

	//-----------------------------------------------------------------------------------
	// DoubleSide drawing
	//-----------------------------------------------------------------------------------

	//! Enable double side rendering.
	Bool enableDoubleSide();

	//! Disable double side rendering.
	Bool disableDoubleSide();

	//! Toggle double side rendering state.
	Bool toggleDoubleSide();

	//! Force double side rendering state.
	Bool forceDoubleSide(Bool mode);

	//! Set default double side rendering state.
	inline Bool setDefaultDoubleSide() { return disableDoubleSide(); }
	//! Force default double side rendering state.
	inline Bool forceDefaultDoubleSide() { return forceDoubleSide(False); }

	//! Get current double side rendering state.
	inline Bool getDoubleSideMode()const { return m_isDoubleSided; }

	//-----------------------------------------------------------------------------------
	// Scissor test
	//-----------------------------------------------------------------------------------

	//! Enable scissor test.
	Bool enableScissorTest();

	//! Disable scissor test.
	Bool disableScissorTest();

	//! Toggle scissor test state.
	Bool toggleScissorTest();

	//! Returns current scissor test state.
	inline Bool getScissorTest() const { return m_isScissorTest; }

	//! Force scissor test state.
	Bool forceScissorTest(Bool mode);

	//! Set default scissor test state.
	inline Bool setDefaultScissorTest() { return disableScissorTest(); }
	//! Force default scissor test state.
	inline Bool forceDefaultScissorTest() { return forceScissorTest(False); }

	//! Define the scissor size (Y up X right).
	void setScissor(Int32 xpos,Int32 ypos,Int32 width,Int32 height);

	//! Define the scissor given a Box2i.
	void setScissor(const Box2i &scissor);

	//! Define the scissor size given a ptr (4 int: x,y,width,height).
	void setScissor(Int32 *ptr);

	//! Get the scissor position and size (4 int: x,y,width,height).
	inline void getScissor(Int32 *ptr)
	{
		ptr[0] = m_scissor.x();
		ptr[1] = m_scissor.y();
		ptr[2] = m_scissor.width();
		ptr[3] = m_scissor.height();
	}

	//! Get the scissor position and size.
	inline const Box2i& getScissor() const { return m_scissor; }

	//-----------------------------------------------------------------------------------
	// Stencil test
    //-----------------------------------------------------------------------------------

	//! Enable Stencil test.
	Bool enableStencilTest();

	//! Disable Stencil test.
	Bool disableStencilTest();

	//! Toggle Stencil test state.
	Bool toggleStencilTest();

	//! Force Stencil test state.
	Bool forceStencilTest(Bool mode);

	//! Set default stencil test state.
	inline Bool setDefaultStencilTest() { return disableStencilTest(); }
	//! Force default stencil test state.
	inline Bool forceDefaultStencilTest() { return forceStencilTest(False); }

	//! Get current stencil test state.
	inline Bool getStencilTest() const { return m_isStencilTest; }

	//! Get stencil test reference value.
	inline Int32 getStencilTestRefValue() const { return m_stencilTestRef; }

	//! Get Stencil test function mode.
	inline Comparison getStencilTestFunc() const { return m_stencilTestFunc; }

	//! Get Stencil test function mask.
	inline UInt32 getStencilTestMaskValue() const { return m_stencilTestMask; }

	//! Set the stencil test function.
	void setStencilTestFunc(Comparison func, Int32 ref, UInt32 mask);

	//! Set the stencil test function.
	void forceStencilTestFunc(Comparison func, Int32 ref, UInt32 mask);

	//! Set default stencil test function.
	inline void setDefaultStencilTestFunc() { setStencilTestFunc(COMP_ALWAYS, 0, 0xffffffff); }
	//! Force default stencil test function.
	inline void forceDefaultStencilTestFunc() { forceStencilTestFunc(COMP_ALWAYS, 0, 0xffffffff); }

	//-----------------------------------------------------------------------------------
	// Miscs
    //-----------------------------------------------------------------------------------

	//! Enable an OpenGL state.
	void enable(UInt32 mode);
	//! Disable an OpenGL state.
	void disable(UInt32 mode);

	//! Get the parent renderer (read only).
	const Renderer* getRenderer() const { return m_renderer; }
	//! Get the parent renderer.
	Renderer* getRenderer() { return m_renderer; }

	//! Get the GLShadingLanguage version.
	//! @return 110 for GLSL 1.10, 120 for 1.20...
	inline UInt32 getGLSLVersion() const { return m_glslVersion; }

	//! Get the maximal texture width/height in pixels.
	inline Int32 getTextureMaxSize() const { return m_textureMaxSize; }

	//! Get the maximal 3d texture width/height in pixels.
	inline Int32 getTexture3dMaxSize() const { return m_texture3dMaxSize; }

    //! Get the maximal number of samples for multi-samples textures.
    inline Int32 getTextureMaxSamples() const { return m_textureMaxSamples; }

	//! Get the maximal texture layers.
	inline Int32 getTextureMaxLayers() const { return m_textureMaxLayers; }

	//! Get the number of image samplers that the GPU supports in the fragment shader.
	inline Int32 getMaxTextureImageUnits() const { return m_maxTextureImageUnits; }

	//! Get the number of image samplers that the GPU supports in the vertex shader.
	inline Int32 getMaxVertexTextureImageUnits() const { return m_maxVertexTextureImageUnits; }

	//! Get the number of image samplers that the GPU supports in the geometry shader.
	inline Int32 getMaxGeometryTextureImageUnits() const { return m_maxGeometryTextureImageUnits; }

    //! Get the number of image samplers that the GPU supports for combined shaders stages.
    inline Int32 getMaxCombinedTextureImageUnits() const { return m_maxCombinedTextureImageUnits; }

	//! Get the number of image samplers that the GPU supports in the tessellation
	//! evaluation shader.
	inline Int32 getMaxTessellationEvalTextureImageUnits() const
	{
		return m_maxTessEvalTextureImageUnits;
	}

	//! Get the number of image samplers that the GPU supports in the tessellation
	//! control shader.
	inline Int32 getMaxTessellationControlTextureImageUnits() const
	{
		return m_maxTessControlTextureImageUnits;
	}

	//! Get the max number of draw buffers.
	inline Int32 getMaxDrawBuffers() const { return m_maxDrawBuffers; }

	//! Get the max number of vertex attributes.
	inline Int32 getMaxVertexAttribs() const { return m_maxVertexAttribs; }

	//! Get the max number of viewports.
	inline Int32 getMaxViewPorts() const { return m_maxViewports; }

	//-----------------------------------------------------------------------------------
	// Viewport
    //-----------------------------------------------------------------------------------

	//! Define the viewport.
	void setViewPort(Int32 xpos, Int32 ypos, Int32 width, Int32 height);

	//! Define the viewport given a Box2i.
	void setViewPort(const Box2i &viewPort);

	//! Define the viewport given a ptr (4 int: x,y,width,height).
	void setViewPort(Int32 *ptr);

	//! Get the viewport position and size (4 int: x,y,width,height).
	inline void getViewPort(Int32 *ptr)
	{
		ptr[0] = m_viewPort.x();
		ptr[1] = m_viewPort.y();
		ptr[2] = m_viewPort.width();
		ptr[3] = m_viewPort.height();
	}

	//! Get the viewport position and size.
	inline const Box2i& getViewPort() const { return m_viewPort; }

	//-----------------------------------------------------------------------------------
	// Color mask
	//-----------------------------------------------------------------------------------

	//! Change the color mask.
	void setColorMask(Bool r, Bool g, Bool b, Bool a);

	//! Enable color write.
	void enableColorWrite();

	//! Disable color write.
	void disableColorWrite();

	//! Set to default color mask.
	inline void setDefaultColorMask() { enableColorWrite(); }


	//-----------------------------------------------------------------------------------
	// Anisotropy
    //-----------------------------------------------------------------------------------

	//! Get the max anisotropy
	inline Float getMaxAnisotropy() const { return m_maxAnisotropy; }


	//-----------------------------------------------------------------------------------
	// Modelview and projection matrices.
	// By default the matrix mode is on modelview.
	// Operations done on projection matrix let modelview as default matrix mode.
	//-----------------------------------------------------------------------------------

	//! Get the modelview matrix.
	inline ModelViewMatrix& modelView() { return m_modelview; }

	//! Get the modelview matrix (read only).
	inline const ModelViewMatrix& modelView() const { return m_modelview; }

	//! Get the projection matrix.
	inline ProjectionMatrix& projection() { return m_projection; }

	//! Get the projection matrix (read only).
	inline const ProjectionMatrix& projection() const { return m_projection; }

	//! Get the modelview-projection matrix.
	const Matrix4& modelViewProjection();

	//! Get the normal matrix.
	const Matrix3& normalMatrix();

	//! Project a 3d point from world space to screen space using the current
	//! modelview and projection matrices and viewport.
	Vector3 projectPoint(const Vector3 &v);

	//! Unproject a 3d point (2d + depth component) from screen space to world space
	//! using the current modelview and projection matrices and viewport.
	Vector3 unprojectPoint(const Vector3 &v);

private:

	virtual void updateMatrix(const Matrix *matrix);

    Bool resetTexture(UInt32 id);
    Bool resetSampler(UInt32 id);

    static const UInt32 MAX_COMBINED_TEX_UNITS = 96;

	Renderer *m_renderer;           //!< Owner.

	DrawingMode m_drawingMode;      //!< polygon drawing mode (default DrawingFilled)
	CullingMode m_cullingMode;      //!< culling face mode (default CullingBackFace)

    Float m_pointSize;              //!< point size (default 1)
    Float m_lineSize;               //!< line size (default 1)

    Blending m_blending;            //!< blending function and equation manager

    Bool m_isTexture2D;                //!< texture 2D
    Bool m_isDepthTest;                //!< depth buffer
    AntiAliasingMethod m_antiAliasing; //!< anti-aliasing mode

	Bool m_isDepthWrite;            //!< write on depth buffer
	Bool m_isDoubleSided;           //!< draw on the two sides

    Color m_backgroundColor;	    //!< background color (default 0,0,0,1)

	Float m_depthClear;				//!< depth buffer clear value (default 1)
	Float m_depthNear, m_depthFar;  //!< depth buffer range (default 0..1)
    Comparison m_depthFunc;		    //!< depth function comparison

    OcclusionQuery *m_currentOccQuery;    //!< current active occlusion query

    UInt32 m_currentTexUnit;                           //!< current active texture unit
    UInt32 m_currentTexId[MAX_COMBINED_TEX_UNITS];     //!< current bound texture id
    UInt32 m_currentSamplerId[MAX_COMBINED_TEX_UNITS]; //!< current bound sampler id
	
	VertexArrayState m_defaultVaoState;   //!< default VAO state
	VertexArrayState *m_currentVaoState;  //!< current bound VAO state
	
	UInt32 m_defaultVAOId;            //!< default VAO id
	UInt32 m_currentVAOId;            //!< current bound VAO id

	UInt32 m_currentFBOId;            //!< current bound FBO id
	UInt32 m_currentPackPBOId;        //!< current bound Pack PBO id
	UInt32 m_currentUnpackPBOId;      //!< current bound Unpack PBO id
	UInt32 m_currentShaderId;         //!< current bound shader id

	Bool m_useSimpleDrawMode;         //!< TRUE if simple draw mode is current

	DrawingMode m_overrideDrawing;
	Bool m_overrideDrawingMode;       //!< override the drawing mode on all materials
	Bool m_overrideDoubleSided;       //!< override the double sided mode on all materials

	Bool m_isScissorTest;             //!< scissor test

	Bool m_isStencilTest;             //!< Stencil test state.
    Comparison m_stencilTestFunc;     //!< Stencil test func.
	Int32 m_stencilTestRef;           //!< Stencil test reference value.
	UInt32 m_stencilTestMask;         //!< Stencil test AND mask.
	Int32 m_stencilOperators[3];      //!< Stencil test operators.
	Int32 m_stencilClear;             //!< Stencil clear value.

    Box2i m_scissor;                  //!< scissor
    Box2i m_viewPort;                 //!< viewport

    Float m_maxAnisotropy;            //!< max anisotropy factor

    ModelViewMatrix m_modelview;      //!< modelview matrix.
    ProjectionMatrix m_projection;    //!< projection matrix.

	Bool m_recomputeModelViewProjection; //!< TRUE mean recompute m_modelViewProjection.
    Matrix4 m_modelViewProjection;       //!< Precomputed modelviewProjection matrix.
    Bool m_recomputeNormalMatrix;        //!< TRUE mean recompute m_normalMatrix.
    Matrix3 m_normalMatrix;              //!< Precomputed modelview normal matrix.

	UInt32 m_glslVersion;             //!< GLShadingLanguage version.

	Int32 m_textureMaxSize;
	Int32 m_texture3dMaxSize;
    Int32 m_textureMaxSamples;
	Int32 m_textureMaxLayers;
	Int32 m_maxTextureImageUnits;
	Int32 m_maxVertexTextureImageUnits;
	Int32 m_maxDrawBuffers;
	Int32 m_maxVertexAttribs;
	Int32 m_maxViewports;
	Int32 m_maxGeometryTextureImageUnits;
    Int32 m_maxCombinedTextureImageUnits;
	Int32 m_maxTessEvalTextureImageUnits;
	Int32 m_maxTessControlTextureImageUnits;
};

} // namespace o3d

#endif // _O3D_CONTEXT_H

