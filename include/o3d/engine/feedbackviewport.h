/**
 * @file feedbackviewport.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FEEDBACKVIEWPORT_H
#define _O3D_FEEDBACKVIEWPORT_H

#include "viewport.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/texture/texture2dfeedback.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

/**
 * @brief Feedback (offscreen) viewport.
 * @date 2002-08-01
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * Feedback viewport rendering using PBO. As result you can get a buffer of data and
 * into a texture 2d.
 */
class O3D_API FeedbackViewPort : public ViewPort
{
public:

	O3D_DECLARE_CLASS(FeedbackViewPort)

	//! Default constructor
	FeedbackViewPort(
			BaseObject *parent,
            Camera* camera = nullptr,
            SceneDrawer* drawer = nullptr,
            Texture2D* texture = nullptr);

	//! Destructor.
	virtual ~FeedbackViewPort();

	//! Set the output pixel texture.
	void setTexture(Texture2D *texture);

	//! Display the viewport.
	virtual void display(UInt32 w, UInt32 h);

	//! Map a read only buffer data. Must call unmap after usage.
	//! @param offset Start offset of mapped buffer
	//! @note If call many time without unmapMap it returns the previously mapped buffer.
	const UInt8* mapData(UInt32 offset = 0);

	//! Unmap buffer data.
	void unmapData();

	//! Get buffer pixel format.
	inline PixelFormat getPixelFormat() const { return m_feedback.getPixelFormat(); }

	//! Get buffer data width.
	inline UInt32 getDataWidth() const { return m_feedback.getBox().width(); }

	//! Get buffer data height.
	inline UInt32 getDataHeight() const { return m_feedback.getBox().height(); }

	//! Get buffer data size in bytes.
	inline UInt32 getDataSize() const { return m_feedback.getSize(); }

protected:

	Texture2DFeedback m_feedback;
	SmartObject<Texture2D> m_texture;
};

} // namespace o3d

#endif // _O3D_FEEDBACKVIEWPORT_H
