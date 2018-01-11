/**
 * @file pixelbuffer.h
 * @brief Pixel buffer object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-08-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PIXELBUFFER_H
#define _O3D_PIXELBUFFER_H

#include "scene/sceneentity.h"
#include "bufferobject.h"

namespace o3d {

class Texture2D;

/**
 * @brief Pixel buffer object base class. The data type is FLOAT.
 * @todo Texture to PBO transfert
 */
class O3D_API PixelBuffer : public BufferObject
{
public:

	enum Type
	{
		PIXEL_PACK_BUFFER = 0x88EB,
		PIXEL_UNPACK_BUFFER = 0x88EC
	};

	//! Constructor.
    PixelBuffer(Context *context, Storage storageType = STATIC_COPY);

	//! Destructor. Release the PBO.
	~PixelBuffer();

	//! Release the PBO content and identifier.
	void release();

	//-----------------------------------------------------------------------------------
	// Parameters
	//-----------------------------------------------------------------------------------

	//! Return the number of element in our buffer.
	inline UInt32 getCount() const { return m_count; }

	//! Return how/where the buffer is stored.
	inline Storage getStorageType() const { return m_storageType; }

	//! Return TRUE if the buffer is currently locked.
	inline Bool isLocked() const { return (m_lockCount > 0); }

	//! Return the size of our stored data (count*sizeof(T)).
	inline UInt32 getDataSize() const { return (m_count * sizeof(Float)); }

	//-----------------------------------------------------------------------------------
	// Operations
	//-----------------------------------------------------------------------------------

	//! Bind the PBO in PACK (read mode) if necessary.
	void bindPackBuffer() const;

	//! Bind the PBO in PACK (read mode) if necessary.
	void bindUnpackBuffer() const;

	//! Unbind the PBO (bind the PBO 0).
	void unbindPackBuffer() const;

	//! Unbind the PBO (bind the PBO 0).
	void unbindUnpackBuffer() const;

	//! Allocate memory for this new buffer (keep old storage type).
	//! @note The current PBO is changed to this or by 0.
	inline void create(UInt32 count, Bool dontUnbind = False)
	{
        create(count, m_storageType, nullptr, dontUnbind);
	}

	//! Create or update the PBO. An update is performed if the count and storageType are
	//! the same as previously defined.
	//! @param count Number of element to store/allocate.
	//! @param storageType Storage type (stream, static, dynamic).
    //! @param data Data to store or nullptr to only allocate.
	//! @param dontUnbind Don't process to the unbound if TRUE.
	//! @note The current PBO is changed to this or by 0.
	void create(
		UInt32 count,
		Storage storageType,
        const Float* data = nullptr,
		Bool dontUnbind = False);

	//! Create or update the PBO. An update is performed if the count and storageType are
	//! the same as previously defined.
	//! @param count Number of element to store/allocate.
	//! @param storageType Storage type (stream, static, dynamic).
    //! @param data Data to store or nullptr to only allocate.
	//! @param dontUnbind Don't process to the unbound if TRUE.
	//! @note The current PBO is changed to this or by 0.
	void copyFromTexture(
		UInt32 count,
		Storage storageType,
        const Float* data = nullptr,
		Bool dontUnbind = False);

	//! Bind the PBO if necessary and copy its content to the texture.
    //! @param data Data to store or nullptr to only allocate.
	//! @param dontUnbind Don't process to the unbound if TRUE.
    void copyToTexture(Texture2D *texture, Bool dontUnbind = False);

	//! Fill the entire buffer data using a Lock/Unlock.
	//! @note Prefer the usage of Update which can be done faster.
	//! @param data Data to fill.
	//! @param count Number of element to fill.
	//! @note Bind the VBO if necessary, then the current VBO is changed.
	void fill(const Float* data, UInt32 count)
	{
        if (data) {
            Float* bufferPtr = lock(0, 0, MAP_WRITE);

			// Check if we have a buffer to write to
            if (bufferPtr) {
				memcpy(bufferPtr, data,count*sizeof(Float));
			}

			unlock();
		}
	}

	//! Copy partially or totally the content of the the PBO to a given
	//! allocated memory.
	//! @param data Destination array (must be allocated).
	//! @param offset Offset in number of elements.
	//! @param count Number of elements to copy from the PBO.
	//! @note Bind the PBO if necessary, then the current PBO is changed.
	void getData(Float *data, UInt32 offset, UInt32 count);

	//! Update partially or totally the content of the VBO given from a data array.
	//! @note This is the best method to use for update the content of the VBO.
	//! @param data Data to fill.
	//! @param offset Offset in number of elements.
	//! @param count Number of elements to fill.
	//! @note Bind the PBO if necessary, then the current PBO is changed.
	void update(const Float* data, UInt32 offset, UInt32 count);

	//! Lock this buffer to write in or read from.
	//! @param offset Offset in number of element.
	//! @param size Number of element to lock.
	//! @param flags Locking mode (read, write, rw).
	//! @return Pointer on locked area.
	//! @note Bind the PBO if necessary, then the current PBO is changed.
    Float* lock(UInt32 offset = 0, UInt32 size = 0, LockFlags flags = MAP_READ | MAP_WRITE);

	//! Unlock the buffer after a previous Lock.
	//! @note Bind the PBO if necessary, then the current PBO is changed.
	void unlock();

	//! Check if the pixel buffer object contain data.
	//! @note Bind the PBO if necessary, then the current PBO is changed.
	Bool checkData();

private:

    UInt32 m_count;         //!< Number of element in the buffer.
    Storage m_storageType;  //!< Where this buffer is stored.

    UInt32 m_lockCount;     //!< Count the number of lock time.
    UInt32 m_lockFlags;     //!< If locked, the current lock mode.

    Float *m_mapped;        //!< Current locked pointer address.**/
};

} // namespace o3d

#endif // _O3D_PIXELBUFFER_H
