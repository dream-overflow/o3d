/**
 * @file vertexelement.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/vertexelement.h"
#include "o3d/engine/context.h"
#include "o3d/engine/renderer.h"

#include "o3d/engine/glextdefines.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

using namespace o3d;

//---------------------------------------------------------------------------------------
// class VertexBufferBuilder
//---------------------------------------------------------------------------------------

// Default constructor.
VertexBufferBuilder::VertexBufferBuilder(Bool interleave) :
	m_interleave(interleave),
	m_stride(0),
	m_size(0)
{
}

// Add a vertex element to the VBO.
UInt32 VertexBufferBuilder::addData(
		const SmartArrayFloat &data,
		UInt32 eltSize)
{
	O3D_ASSERT(eltSize > 0);

	Element element;
	element.data = data;
    element.vertexElement = nullptr;
	element.numElt = data.getNumElt() / eltSize;
	element.eltSize = eltSize;

    if (m_interleave) {
		element.offset = m_stride;
    } else {
		element.offset = m_size;
    }

	m_datas.push_back(element);

	m_size += data.getNumElt();
	m_stride += eltSize;

	return element.offset*sizeof(Float);
}

// Add a vertex element to the VBO.
UInt32 VertexBufferBuilder::addData(
		VertexElement *vertexElement,
		Bool keepLocalData)
{
	O3D_ASSERT(vertexElement);

	Element element;
	element.vertexElement = vertexElement;
	element.keepLocalData = keepLocalData;
	element.numElt = vertexElement->getNumElements();
	element.eltSize = vertexElement->getElementSize();

    if (m_interleave) {
		element.offset = m_stride;
    } else {
		element.offset = m_size;
    }

	m_datas.push_back(element);

	m_size += vertexElement->getNumElements()*vertexElement->getElementSize();
	m_stride += vertexElement->getElementSize();

	return element.offset*sizeof(Float);
}

// Create the VBO.
void VertexBufferBuilder::create(VertexBufferObjf &vbo)
{
	vbo.create(m_size, True);

    if (m_interleave) {
		UInt32 numElt = m_datas.begin()->numElt;
		UInt32 dstOfs = 0;

		Float *vboData = vbo.lock(0, 0, VertexBuffer::WRITE_ONLY);

        for (std::vector<Element>::iterator it = m_datas.begin(); it != m_datas.end(); ++it) {
            if (it->vertexElement) {
				it->lockedData = it->vertexElement->lockArray(0, 0);
            }
		}

		// interleave data into the dst VBO
        for (UInt32 s = 0; s < numElt; ++s) {
            for (std::vector<Element>::iterator it = m_datas.begin(); it != m_datas.end(); ++it) {
				memcpy(
						vboData + dstOfs,
						it->lockedData + it->eltSize * s,
						it->eltSize * sizeof(Float));

				dstOfs += it->eltSize;
			}
		}

        for (std::vector<Element>::iterator it = m_datas.begin(); it != m_datas.end(); ++it) {
            if (it->vertexElement) {
				it->vertexElement->unlockArray();

				it->vertexElement->create(vbo, it->offset, m_stride, it->keepLocalData);
			}
		}

		vbo.unlock();
    } else {
        for (std::vector<Element>::iterator it = m_datas.begin(); it != m_datas.end(); ++it) {
            if (it->vertexElement) {
				const Float *data = it->vertexElement->lockArray(0, 0);

				vbo.update(
						data,
						it->offset,
						it->vertexElement->getNumElements()*it->vertexElement->getElementSize());

				it->vertexElement->unlockArray();

				it->vertexElement->create(vbo, it->offset, 0, it->keepLocalData);
            } else {
				vbo.update(it->data.getData(), it->offset, it->data.getNumElt());
			}
		}
	}

	vbo.unbindBuffer();
}

//---------------------------------------------------------------------------------------
// class VertexElement
//---------------------------------------------------------------------------------------

// Default constructor.
VertexElement::VertexElement(
		VertexAttributeArray type,
		UInt32 numElt,
		UInt32 eltSize) :
	m_type(type),
	m_numElt(numElt),
	m_eltSize(eltSize),
    m_vbo(nullptr),
	m_offset(0),
	m_stride(0),
	m_isValid(False),
	m_isDirty(True),
	m_locked(LOCK_NONE)
{
}

// Construct with a data array.
VertexElement::VertexElement(
		VertexAttributeArray type,
		const SmartArrayFloat &data,
		UInt32 eltSize) :
	m_type(type),
	m_numElt(0),
	m_eltSize(eltSize),
    m_vbo(nullptr),
	m_offset(0),
	m_stride(0),
	m_data(data),
	m_isValid(False),
	m_isDirty(True),
	m_locked(LOCK_NONE)
{
	m_numElt = data.getNumElt() / m_eltSize;
}

// Destructor.
VertexElement::~VertexElement()
{
}

// Set the element size > 0. ie (x,y,z)=3 (u,v)=2.
void VertexElement::setElementSize(UInt32 eltSize)
{
	m_eltSize = eltSize;
}

// Replace the local data array.
void VertexElement::setData(const SmartArrayFloat &data, UInt32 eltSize)
{
    if (m_isValid) {
        if ((m_numElt*m_eltSize) != data.getNumElt()) {
			O3D_ERROR(E_InvalidParameter("Number of elements must be equal"));
        }

		m_data = data;
		m_eltSize = eltSize;

		update(m_data.getData(), 0, m_numElt);
    } else {
		m_data = data;

		m_eltSize = eltSize;
		m_numElt = data.getNumElt() / eltSize;

		m_isDirty = True;
	}
}

// Attribute to a specific vertex array by its index.
void VertexElement::attribute(UInt32 index)
{
	// Bind the VBO if necessary, and enable the vertex attribute array if necessary.
	m_vbo->attribute(index, m_eltSize, m_stride, m_offset);
}

// Create and validate the data array
void VertexElement::create(
	VertexBufferObjf &vbo,
	UInt32 offset,
	UInt32 stride,
	Bool keepLocalData)
{
    if (m_eltSize == 0) {
		O3D_ERROR(E_InvalidParameter("Element size must be different from zero"));
    }

    if (/*!(&vbo) || */!vbo.isExist()) {
		O3D_ERROR(E_InvalidParameter("VBO must be defined"));
    }

	O3D_ASSERT(offset < vbo.getCount());

	m_vbo = &vbo;

	m_offset = offset * sizeof(Float);
	m_stride = stride * sizeof(Float);

	// only if we don't want to keep local data
    if (!keepLocalData) {
		m_data.releaseCheckAndDelete();
    }

	m_isValid = True;
	m_isDirty = False;
}

// Lock the data array once it is initialized into VBO
const Float* VertexElement::lockArray(UInt32 offset, UInt32 numElt)
{
	// Locking of local data is faster. But we have to be sure that they are consistent.
    if (m_isValid) {
        if (m_data.isValid() && !m_isDirty) {
			m_locked = LOCK_DATA;
			return m_data.getData() + (offset * m_eltSize);
        } else if (m_vbo && m_vbo->isExist()) {
			m_locked = LOCK_VBO;

			return m_vbo->lock(
					(m_offset / sizeof(Float)) + (offset * m_eltSize),
					numElt * m_eltSize,
					VertexBuffer::READ_ONLY);
        } else {
            return nullptr;
        }
    } else if (m_data.isValid()) {
		m_locked = LOCK_DATA;
		return m_data.getData() + (offset * m_eltSize);
    } else {
        return nullptr;
    }
}

// Lock the data array once it is initialized into VBO
Float* VertexElement::lockArray(
	LockMode mode,
	UInt32 offset,
	UInt32 numElt)
{
	// Locking of local data is faster. But we have to be sure that they are consistent.
	if (m_isValid)
	{
		if (m_data.isValid() && !m_isDirty)
		{
			m_locked = LOCK_DATA;
			return m_data.getData() + (offset * m_eltSize);
		}
		else if (m_vbo && m_vbo->isExist())
		{
			m_locked = LOCK_VBO;

			VertexBuffer::LockMode lockMode = (mode == WRITE_ONLY) ?
					VertexBuffer::WRITE_ONLY : VertexBuffer::READ_WRITE;

			return m_vbo->lock((m_offset / sizeof(Float)) + (offset * m_eltSize), numElt * m_eltSize, lockMode);
		}
		else
            return nullptr;
	}
	else if (m_data.isValid())
	{
		m_locked = LOCK_DATA;
		return m_data.getData() + (offset * m_eltSize);
	}
	else
        return nullptr;
}

// Unlock the data array
void VertexElement::unlockArray()
{
    if (m_locked == LOCK_VBO) {
        if (m_vbo && m_vbo->isExist() && m_vbo->isLocked()) {
            m_vbo->unlock();
        } else {
			O3D_ERROR(E_InvalidOperation("Inconsistent unlock operation"));
        }
	}

	m_locked = LOCK_NONE;
}

// Update partially or entirely the content of the VBO.
void VertexElement::update(const Float *data, UInt32 offset, UInt32 numElt)
{
    if (m_locked == LOCK_VBO) {
		O3D_ERROR(E_InvalidOperation("Unable to update when data are currently locked"));
    }

	if (m_data.isValid() && !m_isValid)	{
		memcpy(m_data.getData() + (offset * m_eltSize), data, numElt * m_eltSize);
    } else if (m_vbo && m_vbo->isExist()) {
		m_vbo->update(data, (m_offset / sizeof(Float)) + (offset * m_eltSize), numElt * m_eltSize);
    } else {
		O3D_ERROR(E_InvalidOperation("Cannot update an invalid array"));
    }
}

Bool VertexElement::writeToFile(OutStream &os)
{
    os   << m_type
		 << m_numElt
		 << m_eltSize;

	const Float *data = lockArray(0, 0);
    if (data) {
		UInt32 count = m_numElt * m_eltSize;

        os << count;
        os.write(data, count);

		unlockArray();
    } else {
        os << UInt32(0);
	}

	return True;
}

Bool VertexElement::readFromFile(InStream &is)
{
	O3D_ASSERT(m_locked == LOCK_NONE);

	m_vbo = NULL;
	m_locked = LOCK_NONE;

    is   >> m_type
		 >> m_numElt
		 >> m_eltSize;

    is >> m_data;

	O3D_ASSERT(m_data.getNumElt() == (m_numElt * m_eltSize));

	m_isValid = False;
	return True;
}


//---------------------------------------------------------------------------------------
// class VertexBlend
//---------------------------------------------------------------------------------------

// Default constructor
VertexBlend::VertexBlend(
        Context *context,
        VertexElement *vertices,
        VertexElement *normals) :
    m_vbo(context, VertexBuffer::DYNAMIC),
    m_vertices(nullptr),
    m_normals(nullptr)
{
    VertexBufferBuilder builder(False);//True); simplest with non interleaved

    if (vertices) {
		const Float *data = vertices->lockArray(0, 0);
		SmartArrayFloat smart(data, vertices->getNumElements()*vertices->getElementSize());
		vertices->unlockArray();

		m_vertices = new VertexElement(V_VERTICES_ARRAY, smart, vertices->getElementSize());
		builder.addData(m_vertices, True);
	}

    if (normals) {
		const Float *data = normals->lockArray(0, 0);
		SmartArrayFloat smart(data, normals->getNumElements()*normals->getElementSize());
		normals->unlockArray();

		m_normals = new VertexElement(V_NORMALS_ARRAY, smart, normals->getElementSize());
		builder.addData(m_normals, True);
	}

	builder.create(m_vbo);
}
