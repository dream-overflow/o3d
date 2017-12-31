/**
 * @file texture2dstreaming.h
 * @brief Pixel buffer object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-01-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEXTURE2DSTREAMING_H
#define _O3D_TEXTURE2DSTREAMING_H

#include "../pixelbuffer.h"
#include "../scene/sceneentity.h"
#include "o3d/core/box.h"
#include "o3d/core/smartobject.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class Context;
class Texture2D;

//---------------------------------------------------------------------------------------
//! @class Texture2DStreaming
//-------------------------------------------------------------------------------------
//! 2D texture streaming with two PBO.
//---------------------------------------------------------------------------------------
class O3D_API Texture2DStreaming
{
public:

	//! Constructor.
	Texture2DStreaming(BaseObject *parent);

	//! Get the parent object (read only).
	inline const BaseObject* getParent() const { return m_parent; }
	//! Get the parent object.
	inline BaseObject* getParent() { return m_parent; }

	//! Get the scene parent.
	Scene* getScene();
	//! Get the scene parent (read only).
	const Scene* getScene() const;

	//! Get the gl context (read only).
	inline const Context* getContext() const { return m_context; }
	//! Get the gl context.
	inline Context* getContext() { return m_context; }

	//! Create the PBO.
	//! @param box Portion of the texture to update. Default is full area.
	//! Coordinates are clamped to the texture size if necessary.
	void create(const Box2i &box = Box2i(0, 0, -1, -1));

	//! Release PBO and detach texture.
	void release();

	//! Returns TRUE if the object if created successfully.
	inline Bool isValid() const { return m_state; }

	//! Define the texture 2D to update.
	//! Coordinates of the box are reset to the entire area of the texture.
	//! When changing the texture after create, the must must have the same size and
	//! pixel format.
    //! @param texture null to detach the previous texture.
	//! @param level Mipmap level to update (default 0).
	void setTexture(Texture2D *texture, UInt32 level = 0);

	//! Get the texture.
	inline Texture2D* getTexture() const { return m_texture.get(); }

	//! Get the box. Valid once created.
	inline const Box2i& getBox() const { return m_box; }

	//! Get the mipmap level updated.
	inline UInt32 getLevel() const { return m_level; }

	//! Get the size of the PBO/(sub)texture in byte.
	inline UInt32 getSize() const { return m_size; }

	//! Get the GL pixel format. Valid once created.
	inline TextureFormat getGLFormat() const { return m_format; }

	//! Get the GL data type of the pixel format. Valid once created.
	inline DataType getGLType() const { return m_type; }

	//! Get the PBO 0 GL identifier.
	inline UInt32 getPBO0Id() const { return m_buffersId[0]; }
	//! Get the PBO 1 GL identifier.
	inline UInt32 getPBO1Id() const { return m_buffersId[1]; }

	//! Update the content of the texture in streaming.
	//! @param data Valid pointer with data to update to the PBO.
	//! @param size Size of the data to update in bytes.
	void update(UInt8 *data, UInt32 size);

	//! Update a part of the content of the texture in streaming.
	//! @param data Valid pointer with data to update to the PBO.
	//! @param size Size of the data to update in bytes.
	//! @param offset Offset of the data to update in bytes.
	void update(UInt8 *data, UInt32 size, UInt32 offset);

private:

	BaseObject *m_parent;
	Context *m_context;

	Bool m_state;
	UInt32 m_buffersId[2];

	UInt32 m_level; //!< Mipmap level of the texture to update.

	Box2i m_box;        //!< Portion of the texture to update.
	UInt32 m_size;  //!< Size of the PBO in bytes.

	SmartObject<Texture2D> m_texture;

	Bool m_currPBO;

	TextureFormat m_format;
	DataType m_type;
};

} // namespace o3d

#endif // _O3D_TEXTURE2DSTREAMING_H

