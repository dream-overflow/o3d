/**
 * @file texture2dfeedback.h
 * @brief Pixel buffer object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-01-05
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEXTURE2DFEEDBACK_H
#define _O3D_TEXTURE2DFEEDBACK_H

#include "../pixelbuffer.h"
#include "../scene/sceneentity.h"
#include "o3d/image/imagetype.h"
#include "o3d/core/box.h"
#include "o3d/core/smartobject.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class Context;
class Texture2D;

/**
 * @brief 2D texture feedback (read-back) with double buffred PBO.
 * Feedback allow to get back in CPU memory or into a texture the content of a draw buffer.
 */
class O3D_API Texture2DFeedback
{
public:

	//! Constructor.
    Texture2DFeedback(Context *context);

	//! Destructor. Release.
	~Texture2DFeedback();

	//! Get the gl context (read only).
	inline const Context* getContext() const { return m_context; }
	//! Get the gl context.
	inline Context* getContext() { return m_context; }

	//! Create the PBO.
	//! @param readBuffer Read buffer constant or color attachment when FBO. Mostly FRONT.
	//! @param pf Pixel format of the read buffer.
	//! @param box The area to read.
	void create(UInt32 readBuffer, PixelFormat pf);

	//! Release PBO and detach texture.
	void release();

	//! Returns TRUE if the object if created successfully.
	inline Bool isValid() const { return m_state; }

	//! Set the box of the updated area.
	void setBox(const Box2i &box);

	//! Get the box of the updated area.
	inline const Box2i& getBox() const { return m_box; }

	//! Get the size of the PBO/(sub)texture in byte. Size is valid after setBox.
	inline UInt32 getSize() const { return m_size; }

	//! Get the pixel format. Valid once created.
	inline PixelFormat getPixelFormat() const { return m_pixelFormat; }

	//! Get the GL pixel format. Valid once created.
    inline TextureFormat getTextureFormat() const { return m_format; }

    //! Get the data type of the pixel format. Valid once created.
    inline DataType getDataType() const { return m_type; }

	//! Get the read buffer.
	inline UInt32 getReadBuffer() const { return m_readBuffer; }

	//! Get the PBO 0 GL identifier.
	inline UInt32 getPBO0Id() const { return m_buffersId[0]; }
	//! Get the PBO 1 GL identifier.
	inline UInt32 getPBO1Id() const { return m_buffersId[1]; }

	//! According to the box update the PBO.
	void update();

	//! Map a part of the content and returns a read only pointer on this content.
	//! @param data Valid pointer with data to update to the PBO.
	//! @param size Size of the data to update in bytes.
	//! @param offset Offset of the data to update in bytes.
	const UInt8* lock(UInt32 size, UInt32 offset);

	//! According to the box update the PBO, map the content and returns
	//! a read only pointer on this content.
	//! You must call unlock after you have finish to process the data.
	//! @param size Size of the data to map in bytes.
	const UInt8* lock(UInt32 size);

	//! Unmap and unbind the PBO.
	void unlock();

	//! Set the content of the PBO to a 2D texture.
	//! @param texture A valid texture with the same pixel format and width/height as box.
	//! @param level Mipmap level.
	void copyToTexture(Texture2D *texture, UInt32 level);

	//! Get the mapped data (same as returns by lock).
	const UInt8* getMapped() const { return m_mapped; }

private:

	BaseObject *m_parent;
	Context *m_context;

	Bool m_state;
	UInt32 m_buffersId[2];

    UInt32 m_readBuffer;  //!< Read buffer.

    Box2i m_box;    //!< Portion of the texture to update.
    UInt32 m_size;  //!< Size of the PBO in bytes.

	UInt32 m_next;

	PixelFormat m_pixelFormat;
	TextureFormat m_format;
	DataType m_type;

	UInt8 *m_mapped;
};

} // namespace o3d

#endif // _O3D_TEXTURE2DFEEDBACK_H
