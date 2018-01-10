/**
 * @file vertexbuffer.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_VERTEXBUFFER_H
#define _O3D_VERTEXBUFFER_H

#include "bufferobject.h"
#include "glextensionmanager.h"

namespace o3d {

class Context;

/**
 * @brief Vertex buffer object base class.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-09-25
 */
class O3D_API VertexBuffer : public BufferObject
{
public:

	enum Type
	{
		ARRAY_BUFFER = 0x8892,
		ELEMENT_ARRAY_BUFFER = 0x8893
	};

	//! Default constructor.
    VertexBuffer(Context *context, Storage storageType = STATIC);

	//! Destructor.
	~VertexBuffer() {}

	//-----------------------------------------------------------------------------------
	// Parameters
	//-----------------------------------------------------------------------------------

	//! Return the number of element in our buffer.
	inline UInt32 getCount() const { return m_count; }

	//! Return how/where the buffer is stored.
	inline Storage getStorageType() const { return m_storageType; }

	//! Return TRUE if the buffer is currently locked.
	inline Bool isLocked() const { return (m_lockCount > 0); }

protected:

    UInt32 m_count;         //!< Number of element in the buffer.
    Storage m_storageType;  //!< Where this buffer is stored.

    UInt32 m_lockCount;     //!< Count the number of lock time.
    UInt32 m_lockFlags;     //!< If locked, the current map flags.

	//! Bind the VBO if necessary.
	void bindArrayBuffer() const;
	//! Unbind the VBO (bind the VBO 0).
	void unbindArrayBuffer() const;

	//! Bind the IBO if necessary.
	void bindElementArrayBuffer() const;
	//! Unbind the IBO (bind the VBO 0).
	void unbindElementArrayBuffer() const;

	//! Attribute to a specific vertex array by its index.
	void attributeVertex(
			UInt32 index,
			UInt32 eltSize,
			UInt32 stride,
			UInt32 offset);

	//! Delete VBO.
	void deleteVertexBuffer();

	//! Delete IBO.
	void deleteElementArrayBuffer();
};

/**
 * @brief Generic vertex buffer.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-09-25
 */
template <UInt32 bufferType, class T>
class O3D_API_TEMPLATE TemplateVertexBuffer : public VertexBuffer
{
public:

	//! Default constructor.
    TemplateVertexBuffer(Context *context, Storage storageType = STATIC) :
        VertexBuffer(context, storageType),
        m_mapped(nullptr)
	{
	}

	//! Destructor.
    ~TemplateVertexBuffer() { release(); }

	//! Release the VBO content and identifier.
	void release();

	//-----------------------------------------------------------------------------------
	// Parameters
	//-----------------------------------------------------------------------------------

	//! Return the size of our stored data (count*sizeof(T)).
	inline UInt32 getDataSize() const { return (m_count * sizeof(T)); }

	//! Return the OpenGL type of this buffer.
	inline UInt32 getBufferType() const { return bufferType; }

	//-----------------------------------------------------------------------------------
	// Operations
	//-----------------------------------------------------------------------------------

	//! Bind the VBO if necessary.
	void bindBuffer() const;

	//! Unbind the VBO (bind the VBO 0).
	void unbindBuffer() const;

	//! Allocate memory for this new buffer (keep old storage type).
	//! @note The current VBO is changed to this or by 0.
	inline void create(UInt32 count, Bool dontUnbind = False)
	{
        create(count, m_storageType, nullptr, dontUnbind);
	}

	//! Create or update the VBO. An update is performed if the count and storageType are
	//! the same as previously defined.
	//! @param count Number of element to store/allocate.
	//! @param storageType Storage type (stream, static, dynamic).
    //! @param data Data to store or null to only allocate.
	//! @param dontUnbind Don't process to the unbound if TRUE.
	//! @note The current VBO is changed to this or by 0.
	void create(
		UInt32 count,
		Storage storageType,
        const T* data = nullptr,
		Bool dontUnbind = False)
	{
	#ifdef O3D_GPU_MEMORY_MANAGER
		Bool realloc = False;

		if (m_bufferId != O3D_UNDEFINED)
			realloc = True;
	#endif

		// Generate the buffer if necessary
        if (m_bufferId == O3D_UNDEFINED) {
			glGenBuffers(1,(GLuint*)&m_bufferId);
        }

		bindBuffer();

		// simply perform an update
        if ((count == m_count) && (storageType == m_storageType)) {
			glBufferSubData(bufferType, 0, m_count*sizeof(T), data);
        } else {
            // or a create
			// Define global parameters
			m_count = count;
			m_storageType = storageType;

		#ifdef O3D_GPU_MEMORY_MANAGER
			if (realloc)
				O3D_GREALLOC(MemoryManager::GPU_VBO, m_bufferId, getDataSize());
			else
				O3D_GALLOC(MemoryManager::GPU_VBO, m_bufferId, getDataSize());
		#endif

			glBufferData(bufferType, getDataSize(), data, storageType);
		}

        if (!dontUnbind) {
			unbindBuffer();
        }
	}

	//! Fill the entire buffer data using a Lock/Unlock.
    //! @note Prefer the usage of update which can be done faster.
	//! @param data Data to fill.
	//! @param count Number of element to fill.
	//! @note Bind the VBO if necessary, then the current VBO is changed.
	void fill(const T* data, UInt32 count)
	{
        if (data) {
            T* bufferPtr = lock(0, count, MAP_WRITE);

            // Check if we have a buffer to write to
            if (bufferPtr) {
                memcpy(bufferPtr, data, count*sizeof(T));
			}

			unlock();
		}
	}

    /**
     * @brief Copy partially or totally the content of the the VBO to a given allocated memory.
     * @param data Destination array (must be allocated).
     * @param offset Offset in number of elements.
     * @param count Number of elements to copy from the VBO.
     * @note Bind the VBO if necessary, then the current VBO is changed.
     * @note Not compatible with OpenGL ES.
     */
	void getData(T *data, UInt32 offset, UInt32 count)
	{
		O3D_ASSERT(data);

        if (data) {
			bindBuffer();
			glGetBufferSubData(bufferType, offset*sizeof(T), count*sizeof(T), data);
		}
	}

	//! Update partially or totally the content of the VBO given from a data array.
	//! @note This is the best method to use for update the content of the VBO.
	//! @param data Data to fill.
	//! @param offset Offset in number of elements.
	//! @param count Number of elements to fill.
	//! @note Bind the VBO if necessary, then the current VBO is changed.
	void update(const T* data, UInt32 offset, UInt32 count)
    {
        if (m_lockCount > 0) {
			O3D_ERROR(E_InvalidOperation("Cannot update the content of a locked VBO"));
        }

        if (data) {
			bindBuffer();
			glBufferSubData(bufferType, offset*sizeof(T), count*sizeof(T), data);
		}
	}
    /**
     * @brief Lock this buffer to write in or read from.
     * @param offset Offset in number of element.
     * @param size Number of element to lock. A size of 0 mean until the end.
     * @param flags Mapping mode (read, write, rw...).
     * @return Pointer on locked area.
     * @note Bind the VBO if necessary, then the current VBO is changed.
     */
    inline T *lock(
			UInt32 offset = 0,
			UInt32 size = 0,
            LockFlags flags = MAP_READ | MAP_WRITE)
	{
        if (size == 0) {
            size = (m_count - offset) * sizeof(T);
        }

		// already locked ?
        if (m_lockCount > 0) {
			// with the same mode ?
            if (flags == m_lockFlags) {
				++m_lockCount;
				return m_mapped + offset;
            } else {
				O3D_ERROR(E_InvalidOperation("Cannot lock twice time the same VBO with different policy"));
            }
        } else {
			bindBuffer();

            m_mapped = reinterpret_cast<T*>(glMapBufferRange(bufferType, offset, size, flags));

            if (m_mapped) {
                m_lockFlags = flags;

                ++m_lockCount;
                return m_mapped;
            }

            return nullptr;
        }
	}

    /**
     * @brief Unlock the buffer after a previous Lock.
     * @note Bind the VBO if necessary, then the current VBO is changed.
     */
    inline void unlock()
	{
        if (m_lockCount > 0) {
			--m_lockCount;

            if (m_lockCount == 0) {
				bindBuffer();
				glUnmapBuffer(bufferType);
			}
		}
	}

    /**
     * @brief Check if the vertex buffer object contain data.
     * @note Bind the VBO if necessary, then the current VBO is changed.
     */
    inline Bool checkData()
	{
		bindBuffer();

		Int32 vertBufferSize = 0;
		glGetBufferParameteriv(bufferType, BUFFER_SIZE, (GLint*)&vertBufferSize);
		return (vertBufferSize > 0);
	}

protected:

	T *m_mapped;  //!< Current locked pointer address.
};

//! Release the VBO.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ARRAY_BUFFER, Float>::release()
{
	deleteVertexBuffer();
}

//! Release the VBO.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ARRAY_BUFFER, Double>::release()
{
	deleteVertexBuffer();
}

//! Release the VBO.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ARRAY_BUFFER, UInt16>::release()
{
	deleteVertexBuffer();
}

//! Release the VBO.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ARRAY_BUFFER, UInt32>::release()
{
	deleteVertexBuffer();
}

//! Release the IBO.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ELEMENT_ARRAY_BUFFER, UInt16>::release()
{
	deleteElementArrayBuffer();
}

//! Release the IBO.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ELEMENT_ARRAY_BUFFER, UInt32>::release()
{
	deleteElementArrayBuffer();
}

//! Bind the VBO if necessary.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ARRAY_BUFFER, Float>::bindBuffer() const
{
	bindArrayBuffer();
}

//! Bind the VBO if necessary.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ARRAY_BUFFER, Double>::bindBuffer() const
{
	bindArrayBuffer();
}

//! Bind the VBO if necessary.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ARRAY_BUFFER, UInt16>::bindBuffer() const
{
	bindArrayBuffer();
}

//! Bind the VBO if necessary.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ARRAY_BUFFER, UInt32>::bindBuffer() const
{
	bindArrayBuffer();
}

//! Bind the VBO if necessary.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ELEMENT_ARRAY_BUFFER, UInt16>::bindBuffer() const
{
	bindElementArrayBuffer();
}

//! Bind the VBO if necessary.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ELEMENT_ARRAY_BUFFER, UInt32>::bindBuffer() const
{
	bindElementArrayBuffer();
}

//! Unbind the VBO.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ARRAY_BUFFER, Float>::unbindBuffer() const
{
	unbindArrayBuffer();
}

//! Unbind the VBO.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ARRAY_BUFFER, Double>::unbindBuffer() const
{
	unbindArrayBuffer();
}

//! Unbind the VBO.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ARRAY_BUFFER, UInt16>::unbindBuffer() const
{
	unbindArrayBuffer();
}

//! Unbind the VBO.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ARRAY_BUFFER, UInt32>::unbindBuffer() const
{
	unbindArrayBuffer();
}

//! Unbind the VBO.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ELEMENT_ARRAY_BUFFER, UInt16>::unbindBuffer() const
{
	unbindElementArrayBuffer();
}

//! Unbind the VBO.
template <>
inline void TemplateVertexBuffer<VertexBuffer::ELEMENT_ARRAY_BUFFER, UInt32>::unbindBuffer() const
{
	unbindElementArrayBuffer();
}

/**
 * @brief Vertex buffer object based on array buffer.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-09-25
 */
template <class T>
class O3D_API_TEMPLATE ArrayBuffer : public TemplateVertexBuffer<VertexBuffer::ARRAY_BUFFER, T>
{
public:

	//! Default constructor.
    ArrayBuffer(
            Context *context,
			VertexBuffer::Storage storageType = VertexBuffer::STATIC) :
        TemplateVertexBuffer<VertexBuffer::ARRAY_BUFFER, T>(context, storageType)
	{
	}

	//! Attribute to a specific vertex array by its index.
	//! It bind the VBO if necessary, and enable the vertex attribute array if necessary.
	inline void attribute(
			UInt32 index,
			UInt32 eltSize,
			UInt32 stride,
			UInt32 offset)
	{
		VertexBuffer::attributeVertex(index, eltSize, stride, offset);
	}
};

/**
 * @brief Vertex buffer object based on element array buffer (geometry index).
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-09-25
 */
template <class T>
class O3D_API_TEMPLATE ElementArrayBuffer : public TemplateVertexBuffer<VertexBuffer::ELEMENT_ARRAY_BUFFER, T>
{
public:

	//! Default constructor.
    ElementArrayBuffer(
            Context *context,
			VertexBuffer::Storage storageType = VertexBuffer::STATIC) :
        TemplateVertexBuffer<VertexBuffer::ELEMENT_ARRAY_BUFFER, T>(context, storageType)
	{
	}
};

//---------------------------------------------------------------------------------------
// Predefined array buffer types
//---------------------------------------------------------------------------------------

typedef ArrayBuffer<Float> ArrayBufferf;
typedef ArrayBuffer<Double> ArrayBufferd;
typedef ArrayBuffer<Int32> ArrayBufferi;
typedef ArrayBuffer<UInt32> ArrayBufferui;
typedef ArrayBuffer<Int16> ArrayBuffers;
typedef ArrayBuffer<UInt16> ArrayBufferus;
typedef ArrayBuffer<Int8> ArrayBufferb;
typedef ArrayBuffer<UInt8> ArrayBufferub;

typedef ElementArrayBuffer<UInt32> ElementArrayBufferui;
typedef ElementArrayBuffer<UInt16> ElementArrayBufferus;

} // namespace o3d

#endif // _O3D_VERTEXBUFFER_H
