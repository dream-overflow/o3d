/**
 * @file facearrayvisitor.cpp
 * @brief Implementation of FaceArrayVisitor.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-06-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/facearrayvisitor.h"

using namespace o3d;

//---------------------------------------------------------------------------------------
// FaceArrayIterator
//---------------------------------------------------------------------------------------

void FaceArrayIterator::AccessTriangle16::getData(FaceArrayIterator *in)
{
	in->a = *in->m_dataUInt16++;
	in->b = *in->m_dataUInt16++;
	in->c = *in->m_dataUInt16++;

	++(in->id);
}

void FaceArrayIterator::AccessTriangle32::getData(FaceArrayIterator *in)
{
	in->a = *in->m_dataUInt32++;
	in->b = *in->m_dataUInt32++;
	in->c = *in->m_dataUInt32++;
	++(in->id);
}

void FaceArrayIterator::AccessTriangleFans16::getData(FaceArrayIterator *in)
{
	in->a = *in->m_baseDataUInt16;
	in->b = *in->m_dataUInt16++;
	in->c = *in->m_dataUInt16++;
	++(in->id);
}

void FaceArrayIterator::AccessTriangleFans32::getData(FaceArrayIterator *in)
{
	in->a = *in->m_baseDataUInt32;
	in->b = *in->m_dataUInt32++;
	in->c = *in->m_dataUInt32++;
	++(in->id);
}

void FaceArrayIterator::AccessTriangleStrip16::getData(FaceArrayIterator *in)
{
	do
	{
		if (in->m_step)
		{
			in->a = *(in->m_dataUInt16+1);
			in->b = *in->m_dataUInt16;
			in->c = *(in->m_dataUInt16+2);

			++(in->m_dataUInt16);
			++(in->id);
		}
		else
		{
			in->a = *in->m_dataUInt16;
			in->b = *(in->m_dataUInt16+1);
			in->c = *(in->m_dataUInt16+2);

			++(in->m_dataUInt16);
			++(in->id);
		}

		in->m_step = !in->m_step;
	}
	while ((in->a == in->b) || (in->b == in->c));
}

void FaceArrayIterator::AccessTriangleStrip32::getData(FaceArrayIterator *in)
{
	do
	{
		if (in->m_step)
		{
			in->a = *(in->m_dataUInt32+1);
			in->b = *in->m_dataUInt32;
			in->c = *(in->m_dataUInt32+3);

			++(in->m_dataUInt32);
			++(in->id);
		}
		else
		{
			in->a = *in->m_dataUInt32;
			in->b = *(in->m_dataUInt32+1);
			in->c = *(in->m_dataUInt32+2);

			++(in->m_dataUInt32);
			++(in->id);
		}

		in->m_step = !in->m_step;
	}
	while ((in->a == in->b) || (in->b == in->c));
}


//---------------------------------------------------------------------------------------
// O3DFaceArrayVisitor
//---------------------------------------------------------------------------------------

// Construct from an O3DFaceArray.
FaceArrayVisitor::FaceArrayVisitor(
		FaceArray *faceArray,
		UInt32 offset,
		UInt32 size) :
			m_faceArray(faceArray),
			m_dataUInt16(NULL),
			m_dataUInt32(NULL),
			m_numIndices(0),
			m_step(False)
{
	if (!faceArray)
		O3D_ERROR(E_InvalidParameter("Face array must be valid"));

	switch (faceArray->getFormat())
	{
		case P_TRIANGLES:
			if (faceArray->getTypeSize() == 2)
			{
				getData = AccessTriangle16::getData;
				getArray = AccessTriangle16::getArray;
				m_beginIt.getData = FaceArrayIterator::AccessTriangle16::getData;
				m_endIt.getData = FaceArrayIterator::AccessTriangle16::getData;
			}
			else
			{
				getData = AccessTriangle32::getData;
				getArray = AccessTriangle32::getArray;
				m_beginIt.getData = FaceArrayIterator::AccessTriangle32::getData;
				m_endIt.getData = FaceArrayIterator::AccessTriangle32::getData;
			}
			break;
		case P_TRIANGLE_FAN:
			if (faceArray->getTypeSize() == 2)
			{
				getData = AccessTriangleFans16::getData;
				getArray = AccessTriangleFans16::getArray;
				m_beginIt.getData = FaceArrayIterator::AccessTriangleFans16::getData;
				m_endIt.getData = FaceArrayIterator::AccessTriangleFans16::getData;
			}
			else
			{
				getData = AccessTriangleFans32::getData;
				getArray = AccessTriangleFans32::getArray;
				m_beginIt.getData = FaceArrayIterator::AccessTriangleFans32::getData;
				m_endIt.getData = FaceArrayIterator::AccessTriangleFans32::getData;
			}
			break;
		case P_TRIANGLE_STRIP:
			if (faceArray->getTypeSize() == 2)
			{
				getData = AccessTriangleStrip16::getData;
				getArray = AccessTriangleStrip16::getArray;
				m_beginIt.getData = FaceArrayIterator::AccessTriangleStrip16::getData;
				m_endIt.getData = FaceArrayIterator::AccessTriangleStrip16::getData;
			}
			else
			{
				getData = AccessTriangleStrip32::getData;
				getArray = AccessTriangleStrip32::getArray;
				m_beginIt.getData = FaceArrayIterator::AccessTriangleStrip32::getData;
				m_endIt.getData = FaceArrayIterator::AccessTriangleStrip32::getData;
			}
			break;
		default:
			O3D_ERROR(E_InvalidFormat("Face array must be triangle"));
			break;
	}

	m_format = faceArray->getFormat();

	if (faceArray->getTypeSize() == 2)
	{
		m_dataUInt16 = reinterpret_cast<UInt16*>(faceArray->lockArray(offset, size));
	}
	else
	{
		m_dataUInt32 = reinterpret_cast<UInt32*>(faceArray->lockArray(offset, size));
	}

	O3D_ASSERT(size <= (m_faceArray->getNumElements() - offset));
	m_numIndices = (size > 0 ? size : m_faceArray->getNumElements()) - offset;

	m_beginIt.m_dataUInt16 = m_dataUInt16;
	m_beginIt.m_dataUInt32 = m_dataUInt32;
	m_beginIt.m_baseDataUInt16 = m_dataUInt16;
	m_beginIt.m_baseDataUInt32 = m_dataUInt32;

	m_endIt.m_baseDataUInt16 = m_dataUInt16;
	m_endIt.m_baseDataUInt32 = m_dataUInt32;

	if (m_dataUInt16)
	{
		m_endIt.m_dataUInt16 = m_dataUInt16 + m_numIndices - 1;
		m_beginIt.getData(&m_beginIt);
	}
	else if (m_dataUInt32)
	{
		m_endIt.m_dataUInt32 = m_dataUInt32 + m_numIndices - 1;
		m_beginIt.getData(&m_beginIt);
	}
	else
	{
		m_endIt.m_dataUInt16 = NULL;
		m_endIt.m_dataUInt32 = NULL;
	}
}

// Construct from a face indices array and a specified triangle format.
FaceArrayVisitor::FaceArrayVisitor(
		UInt16 *faceArray,
		UInt32 numElements,
		PrimitiveFormat faceType) :
			m_faceArray(NULL),
			m_dataUInt16(NULL),
			m_dataUInt32(NULL),
			m_numIndices(0),
			m_step(False)
{
	if (!faceArray)
		O3D_ERROR(E_InvalidParameter("Face array must be valid"));

	switch (faceType)
	{
		case P_TRIANGLES:
			getData = AccessTriangle16::getData;
			getArray = AccessTriangle16::getArray;
			m_beginIt.getData = FaceArrayIterator::AccessTriangle16::getData;
			m_endIt.getData = FaceArrayIterator::AccessTriangle16::getData;
			break;
		case P_TRIANGLE_FAN:
			getData = AccessTriangleFans16::getData;
			getArray = AccessTriangleFans16::getArray;
			m_beginIt.getData = FaceArrayIterator::AccessTriangleFans16::getData;
			m_endIt.getData = FaceArrayIterator::AccessTriangleFans16::getData;
			break;
		case P_TRIANGLE_STRIP:
			getData = AccessTriangleStrip16::getData;
			getArray = AccessTriangleStrip16::getArray;
			m_beginIt.getData = FaceArrayIterator::AccessTriangleStrip16::getData;
			m_endIt.getData = FaceArrayIterator::AccessTriangleStrip16::getData;
			break;
		default:
			O3D_ERROR(E_InvalidFormat("Face array must be triangle"));
			break;
	}

	m_format = faceType;
	m_dataUInt16 = faceArray;
	m_numIndices = numElements;

	m_beginIt.m_dataUInt16 = m_dataUInt16;
	m_beginIt.m_baseDataUInt16 = m_dataUInt16;

	m_endIt.m_baseDataUInt16 = m_dataUInt16;

	if (m_dataUInt16)
	{
		m_endIt.m_dataUInt16 = m_dataUInt16 + m_numIndices - 1;
		m_beginIt.getData(&m_beginIt);
	}
	else
		m_endIt.m_dataUInt16 = NULL;
}

// Construct from a face indices array and a specified triangle format.
FaceArrayVisitor::FaceArrayVisitor(
		UInt32 *faceArray,
		UInt32 numElements,
		PrimitiveFormat faceType) :
			m_faceArray(NULL),
			m_dataUInt16(NULL),
			m_dataUInt32(NULL),
			m_numIndices(0),
			m_step(False)
{
	if (!faceArray)
		O3D_ERROR(E_InvalidParameter("Face array must be valid"));

	switch (faceType)
	{
		case P_TRIANGLES:
			getData = AccessTriangle32::getData;
			getArray = AccessTriangle32::getArray;
			m_beginIt.getData = FaceArrayIterator::AccessTriangle32::getData;
			m_endIt.getData = FaceArrayIterator::AccessTriangle32::getData;
			break;
		case P_TRIANGLE_FAN:
			getData = AccessTriangleFans32::getData;
			getArray = AccessTriangleFans32::getArray;
			m_beginIt.getData = FaceArrayIterator::AccessTriangleFans32::getData;
			m_endIt.getData = FaceArrayIterator::AccessTriangleFans32::getData;
			break;
		case P_TRIANGLE_STRIP:
			getData = AccessTriangleStrip32::getData;
			getArray = AccessTriangleStrip32::getArray;
			m_beginIt.getData = FaceArrayIterator::AccessTriangleStrip32::getData;
			m_endIt.getData = FaceArrayIterator::AccessTriangleStrip32::getData;
			break;
		default:
			O3D_ERROR(E_InvalidFormat("Face array must be triangle"));
			break;
	}

	m_format = faceType;
	m_dataUInt32 = faceArray;
	m_numIndices = numElements;

	m_beginIt.m_dataUInt32 = m_dataUInt32;
	m_beginIt.m_baseDataUInt32 = m_dataUInt32;

	m_endIt.m_baseDataUInt32 = m_dataUInt32;

	if (m_dataUInt16 || m_dataUInt32)
	{
		m_endIt.m_dataUInt32 = m_dataUInt32 + m_numIndices - 1;
		m_beginIt.getData(&m_beginIt);
	}
	else
	{
		m_endIt.m_dataUInt32 = NULL;
	}
}

// Destructor. Unlock the face if not done manually.
FaceArrayVisitor::~FaceArrayVisitor()
{
	if (m_faceArray)
		m_faceArray->unlockArray();
}

UInt32 FaceArrayVisitor::getNumTriangles() const
{
	switch (m_format)
	{
		case P_POINTS:
			return m_numIndices;
		case P_LINES:
			return m_numIndices / 2;
		case P_LINE_LOOP:
			return m_numIndices;
		case P_LINE_STRIP:
			return m_numIndices - 1;
		case P_TRIANGLES:
			return m_numIndices / 3;
		case P_TRIANGLE_FAN:
			return m_numIndices - 2;
		case P_TRIANGLE_STRIP:
			return m_numIndices - 2;
		default:
			return 0;
	};
}

// Compute the real number of triangles.
UInt32 FaceArrayVisitor::computeNumTriangle()
{
	switch (m_format)
	{
		case P_TRIANGLES:
			return m_numIndices / 3;
		case P_TRIANGLE_FAN:
			return m_numIndices - 2;
		case P_TRIANGLE_STRIP:
		{
			UInt32 numTriangles = m_numIndices - 2;

			// remove degenerated triangles

			if (m_dataUInt16)
			{
				for (UInt32 i = 0; i < m_numIndices-1; ++i)
				{
					if (m_dataUInt16[i] == m_dataUInt16[i+1])
						--numTriangles;
				}
			}
			else if (m_dataUInt32)
			{
				for (UInt32 i = 0; i < m_numIndices-1; ++i)
				{
					if (m_dataUInt32[i] == m_dataUInt32[i+1])
						--numTriangles;
				}
			}
			return numTriangles;
		}
		default:
			return 0;
	}
}

void FaceArrayVisitor::AccessTriangle16::getData(FaceArrayVisitor *in, UInt32 id, UInt32 &a, UInt32 &b, UInt32 &c)
{
	UInt16 *data = in->m_dataUInt16 + id * 3;

	a = *data;
	b = *data+1;
	c = *data+2;
}

void FaceArrayVisitor::AccessTriangle16::getArray(FaceArrayVisitor *in, UInt32 id, UInt32 *v)
{
	UInt16 *data = in->m_dataUInt16 + id * 3;

	v[0] = *data;
	v[1] = *data+1;
	v[2] = *data+2;
}

void FaceArrayVisitor::AccessTriangle32::getData(FaceArrayVisitor *in, UInt32 id, UInt32 &a, UInt32 &b, UInt32 &c)
{
	UInt32 *data = in->m_dataUInt32 + id * 3;

	a = *data;
	b = *data+1;
	c = *data+2;
}

void FaceArrayVisitor::AccessTriangle32::getArray(FaceArrayVisitor *in, UInt32 id, UInt32 *v)
{
	memcpy(v, in->m_dataUInt32 + id * 3, 3*sizeof(UInt32));
}

void FaceArrayVisitor::AccessTriangleFans16::getData(FaceArrayVisitor *in, UInt32 id, UInt32 &a, UInt32 &b, UInt32 &c)
{
	UInt16 *data = in->m_dataUInt16 + id;

	a = *in->m_dataUInt16;
	b = *data+1;
	c = *data+2;
}

void FaceArrayVisitor::AccessTriangleFans16::getArray(FaceArrayVisitor *in, UInt32 id, UInt32 *v)
{
	UInt16 *data = in->m_dataUInt16 + id;

	v[0] = *in->m_dataUInt16;
	v[1] = *data+1;
	v[2] = *data+2;
}

void FaceArrayVisitor::AccessTriangleFans32::getData(FaceArrayVisitor *in, UInt32 id, UInt32 &a, UInt32 &b, UInt32 &c)
{
	UInt32 *data = in->m_dataUInt32 + id;

	a = *in->m_dataUInt32;
	b = *data+1;
	c = *data+2;
}

void FaceArrayVisitor::AccessTriangleFans32::getArray(FaceArrayVisitor *in, UInt32 id, UInt32 *v)
{
	UInt32 *data = in->m_dataUInt32 + id;

	v[0] = *in->m_dataUInt32;
	v[1] = *data+1;
	v[2] = *data+2;
}

void FaceArrayVisitor::AccessTriangleStrip16::getData(FaceArrayVisitor *in, UInt32 id, UInt32 &a, UInt32 &b, UInt32 &c)
{
	UInt16 *data = in->m_dataUInt16 + id;

	if (id % 2)
	{
		a = *(data+2);
		b = *(data+1);
		c = *(data+3);
	}
	else
	{
		a = *data;
		b = *(data+1);
		c = *(data+2);
	}
}

void FaceArrayVisitor::AccessTriangleStrip16::getArray(FaceArrayVisitor *in, UInt32 id, UInt32 *v)
{
	UInt16 *data = in->m_dataUInt16 + id;

	if (id % 2)
	{
		v[0] = *(data+2);
		v[1] = *(data+1);
		v[2] = *(data+3);
	}
	else
	{
		v[0] = *data;
		v[1] = *(data+1);
		v[2] = *(data+2);
	}
}

void FaceArrayVisitor::AccessTriangleStrip32::getData(FaceArrayVisitor *in, UInt32 id, UInt32 &a, UInt32 &b, UInt32 &c)
{
	UInt32 *data = in->m_dataUInt32 + id;

	if (id % 2)
	{
		a = *(data+2);
		b = *(data+1);
		c = *(data+3);
	}
	else
	{
		a = *data;
		b = *(data+1);
		c = *(data+2);
	}
}

void FaceArrayVisitor::AccessTriangleStrip32::getArray(FaceArrayVisitor *in, UInt32 id, UInt32 *v)
{
	UInt32 *data = in->m_dataUInt32 + id;

	if (id % 2)
	{
		v[0] = *(data+2);
		v[1] = *(data+1);
		v[2] = *(data+3);
	}
	else
	{
		v[0] = *data;
		v[1] = *(data+1);
		v[2] = *(data+2);
	}
}

