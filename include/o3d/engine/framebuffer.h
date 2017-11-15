/**
 * @file framebuffer.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FRAMEBUFFER_H
#define _O3D_FRAMEBUFFER_H

#include "o3d/core/smartcounter.h"
#include "texture/texture.h"
#include "o3d/core/vector2.h"
#include "o3d/core/box.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class Texture2D;
class Texture2DMS;

/**
 * @brief Frame buffer object for MRT and to texture rendering.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-11-14
 */
class O3D_API FrameBuffer : NonCopyable<>
{
public:

	//! Depth and stencil render buffer format.
	enum Format
	{
        DEPTH_U16 = 0,
        DEPTH_U24,
        DEPTH_U32,
		DEPTH_F32,
        STENCIL_U8,
        DEPTH_STENCIL_U24_U8,
        DEPTH_STENCIL_F32_U24_U8
	};

	//! Texture attachment format.
	enum Attachment
	{
		COLOR_ATTACHMENT0 = 0,
		COLOR_ATTACHMENT1,
		COLOR_ATTACHMENT2,
		COLOR_ATTACHMENT3,
		COLOR_ATTACHMENT4,
		COLOR_ATTACHMENT5,
		COLOR_ATTACHMENT6,
		COLOR_ATTACHMENT7,
		COLOR_ATTACHMENT8,
		COLOR_ATTACHMENT9,
		COLOR_ATTACHMENT10,
		COLOR_ATTACHMENT11,
		COLOR_ATTACHMENT12,
		COLOR_ATTACHMENT13,
		COLOR_ATTACHMENT14,
		COLOR_ATTACHMENT15,
		DEPTH_ATTACHMENT,
        STENCIL_ATTACHMENT,
		DEPTH_STENCIL_ATTACHMENT
	};

	//! Blit filter mode.
	enum Filter
	{
		FILTER_NEAREST,
		FILTER_LINEAR
	};

	//! Blit mask transfer. Use | bitwise OR operator.
	enum BlitMask
	{
		MASK_COLOR = 1,
		MASK_DEPTH = 2,
        MASK_STENCIL = 4
	};

	static const Int32 NUM_ATTACHMENTS = DEPTH_STENCIL_ATTACHMENT + 1;

	//! Default constructor.
    FrameBuffer(Context *context);

    //! Destructor.
    ~FrameBuffer() { release(); }

    //! Get the gl context (read only).
    inline const Context* getContext() const { return m_context; }
    //! Get the gl context.
    inline Context* getContext() { return m_context; }


	//-----------------------------------------------------------------------------------
	// Parameters
	//-----------------------------------------------------------------------------------

	//! Return the width/height of the FBO.
	inline const Vector2i& getDimension() const { return m_dimension; }
	//! Return if the buffer exists.
    inline Bool isValid() const { return (m_bufferId != O3D_UNDEFINED); }

	//! Is color attachment.
	//! Return the pixel format if color attachment.
	inline PixelFormat getColorFormat() const { return m_colorFormat; }


	//-----------------------------------------------------------------------------------
	// Operations
	//-----------------------------------------------------------------------------------

	//! Bind this buffer.
	void bindBuffer();
	//! Unbind this buffer.
	void unbindBuffer();
    //! Check according to the context if the frame buffer is currently bound.
    Bool isBound() const;

    //! Get the buffer id.
    inline UInt32 getBufferId() const { return m_bufferId; }

	//! Create the FBO.
	//! @param width Width of the frame buffer.
	//! @param width Height of the frame buffer.
	//! @param colorFormat One of the pixel color format.
	//! @param dontUnbind If TRUE keep the frame buffer as current, otherwise bind 0.
	void create(
		UInt32 width,
		UInt32 height,
		PixelFormat colorFormat,
		Bool dontUnbind = False);

	//! Create the FBO.
	//! @param width Width of the frame buffer.
	//! @param width Height of the frame buffer.
	//! @param dontUnbind If TRUE keep the frame buffer as current, otherwise bind 0.
	//! @note Same as previous Create method but don't attach color attachment.
	void create(UInt32 width, UInt32 height, Bool dontUnbind = False);

	//! Destroy the frame buffer and render buffers.
	//! @note If the frame buffer is not currently bound, it is bound and the previous
	//! frame buffer is restored.
	void release();

	//! Resize the FBO. Attached texture must be resized before otherwise this call
	//! throw an exception.
	//! Attached color and depth renderer are resized.
	void resize(UInt32 w, UInt32 h);

	//! Blit to another frame buffer.
	//! @param dstFrameBuffer Destination FBO.
	//! @param src Source rectangle.
	//! @param dst Destination rectangle.
	void blit(
			FrameBuffer &dstFrameBuffer,
			const Box2i &src,
			const Box2i &dst,
            Int32/*BlitMask*/ mask,
			Filter filter);

    //! Blit to the main buffers.
    //! @param src Source rectangle.
    //! @param dst Destination rectangle.
    void blit(
            const Box2i &src,
            const Box2i &dst,
            Int32/*BlitMask*/ mask,
            Filter filter);

	//-----------------------------------------------------------------------------------
	// Attachments and completeness
	//-----------------------------------------------------------------------------------

    //! Attach an existing texture to the FBO, must be of the same format as
	//! attachment destination and of the same size as the FBO.
    //! @param texture Valid texture 2D or 2D multi-sampled.
	//! @param where Color, depth or depth_stencil attachment point.
	//! @note Frame buffer must be currently bound.
    void attachTexture2D(Texture* pTexture, Attachment where);

	//! Add a render buffer.
	//! @param where Color attachment point only.
	//! @param fragData For color attachment point, define the index of the draw buffer.
	//! For non color attachment don't care about it.
	//! @note Frame buffer must be currently bound.
    void attachColorRender(Attachment where);

	//! Add a depth/stencil render buffer.
	//! @note Frame buffer must be currently bound.
	void attachDepthStencilRender(Format format);

	//! Detach an existing attachment point. If a texture object is attached, then
	//! it is released.
	//! @note Frame buffer must be currently bound.
	void detach(Attachment where);

	//! Check for frame buffer completeness.
	//! @note Frame buffer must be currently bound.
	Bool isCompleteness() const;


	//-----------------------------------------------------------------------------------
	// Cleaning helpers
	//-----------------------------------------------------------------------------------

    //! Clear any color buffers using the default clear values (texture or render bound to draw buffers).
    //! @note Frame buffer must be currently bound.
    void clearColor();

    //! Clear a particular color buffers (texture or render bound to draw buffers).
    //! @param drawBuffer Specific draw buffer attachement.
    //! @param color Specific clear color.
    //! @note Frame buffer must be currently bound for GL < 4.5.
    void clearColor(Attachment drawBuffer, Color color=Color());

    //! Clear the depth attachment (texture or render bound to draw buffers).
    //! @param depth Specific clear depth.
    //! @note Frame buffer must be currently bound for GL < 4.5.
    void clearDepth(Float depth=1.0f);

    //! Clear the stencil attachment (texture or render bound to draw buffers).
    //! @param stencil Specific clear stencil.
    //! @note Frame buffer must be currently bound for GL < 4.5.
    void clearStencil(Int32 stencil=0);

    //! Clear the depth and stencil attachments (texture or render bound to draw buffers).
    //! @param depth Specific clear depth.
    //! @param stencil Specific clear stencil.
    //! @note Frame buffer must be currently bound for GL < 4.5.
    void clearDepthAndStencil(Float depth=0.0f, Int32 stencil=0);

    //! Clear any colors and depth buffers using the default clear values (texture or render bound to draw buffers).
    //! @note Frame buffer must be currently bound.
    void clearColorAndDepth();

    //! Clear any buffers using the default clear value s(texture or render bound to draw buffers).
    //! @note Frame buffer must be currently bound.
    void clearAllBuffers();

protected:

	struct AttachmentElt
	{
        Attachment where;      //!< Attachment of the texture.
        SmartObject<Texture> texture;

		PixelFormat pixelFormat; //!< Pixel format.

		UInt32 renderId;     //!< OpenGL Render identifier.
		UInt32 dataSize;     //!< size (in byte) of the RenderBuffer.

		AttachmentElt() :
			where(COLOR_ATTACHMENT0),
			pixelFormat(PF_RGBA_U8),
			renderId(O3D_UNDEFINED),
			dataSize(0) {}
	};

    Context *m_context;         //!< Related context.

    UInt32 m_bufferId;          //!< OpenGL FBO identifier.
    PixelFormat m_colorFormat;  //!< Pixel format if color attachment.
    UInt32 m_internalFormat;    //!< OpenGL Internal format if color attachment, otherwise 0.
    Vector2i m_dimension;       //!< Width and height of the FBO.

	std::vector<AttachmentElt*> m_attachments;
};

} // namespace o3d

#endif // _O3D_FRAMEBUFFER_H

