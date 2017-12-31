/**
 * @file facearrayvisitor.h
 * @brief Access on triangles from a face array of triangle/strip/fans.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-06-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FACEARRAYVISITOR_H
#define _O3D_FACEARRAYVISITOR_H

#include "facearray.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

/**
 * @brief Access on triangles from a face array of triangle/strip/fans.
 */
class O3D_API FaceArrayIterator
{
	friend class FaceArrayVisitor;
	friend class AccessTriangle16;
	friend class AccessTriangle32;
	friend class AccessTriangleStrip16;
	friend class AccessTriangleStrip32;
	friend class AccessTriangleFans16;
	friend class AccessTriangleFans32;

public:

	//! Default constructor.
	FaceArrayIterator() :
		a(0),
		b(0),
		c(0),
		id(0),
        getData(nullptr),
        m_baseDataUInt16(nullptr),
        m_baseDataUInt32(nullptr),
        m_dataUInt16(nullptr),
        m_dataUInt32(nullptr),
		m_step(False)
	{
	}

	//! Initialization constructor.
	FaceArrayIterator(const FaceArrayIterator &dup) :
		a(dup.a),
		b(dup.b),
		c(dup.c),
		id(dup.id),
		getData(dup.getData),
		m_baseDataUInt16(dup.m_baseDataUInt16),
		m_baseDataUInt32(dup.m_baseDataUInt32),
		m_dataUInt16(dup.m_dataUInt16),
		m_dataUInt32(dup.m_dataUInt32),
		m_step(dup.m_step)
	{
	}

	//! Copy operator.
	inline FaceArrayIterator& operator= (const FaceArrayIterator &dup)
	{
		a = dup.a;
		b = dup.b;
		c = dup.c;
		id = dup.id;

		getData = dup.getData;

		m_baseDataUInt16 = dup.m_baseDataUInt16;
		m_baseDataUInt32 = dup.m_baseDataUInt32;
		m_dataUInt16 = dup.m_dataUInt16;
		m_dataUInt32 = dup.m_dataUInt32;
		m_step = dup.m_step;

		return *this;
	}

	//! Compare if not equal.
	inline Bool operator!= (const FaceArrayIterator &cmp) const
	{
		return ((m_dataUInt16 < cmp.m_dataUInt16) || (m_dataUInt32 < cmp.m_dataUInt32));
	}

	//! Next iterator.
	inline void operator++() { getData(this); }

	union
	{
		struct
		{
			UInt32 a;
			UInt32 b;
			UInt32 c;
		};

		UInt32 v[3];
	};

	UInt32 id;

private:

	class AccessTriangle16
	{
	public:

		static void getData(FaceArrayIterator *in);
	};

	class AccessTriangle32
	{
	public:

		static void getData(FaceArrayIterator *in);
	};

	class AccessTriangleFans16
	{
	public:

		static void getData(FaceArrayIterator *in);
	};

	class AccessTriangleFans32
	{
	public:

		static void getData(FaceArrayIterator *in);
	};

	class AccessTriangleStrip16
	{
	public:

		static void getData(FaceArrayIterator *in);
	};

	class AccessTriangleStrip32
	{
	public:

		static void getData(FaceArrayIterator *in);
	};

	void (*getData)(FaceArrayIterator *in);

	UInt16 *m_baseDataUInt16;  //!< Pointer to the first face index 16 bits.
	UInt32 *m_baseDataUInt32;  //!< Pointer to the first face index 32 bits.

	UInt16 *m_dataUInt16;  //!< Pointer to the current face index 16 bits.
	UInt32 *m_dataUInt32;  //!< Pointer to the current face index 32 bits.

	Bool m_step;           //!< Two step for triangle strip.
};

/**
 * @brief Access on triangles from a face array of triangle/strip/fans.
 */
class O3D_API FaceArrayVisitor : NonCopyable<>
{
	friend class AccessTriangle16;
	friend class AccessTriangle32;
	friend class AccessTriangleStrip16;
	friend class AccessTriangleStrip32;
	friend class AccessTriangleFans16;
	friend class AccessTriangleFans32;

public:

	//! Construct from an O3DFaceArray.
	//! @note Face array format accepted are Triangles, TriangleStrip, TriangleFans.
	//! @param faceArray A valid face array consisting of triangles.
	//! @param offset The offset of the first elements.
	//! @param size If non 0, the number of element to lock for.
	FaceArrayVisitor(FaceArray *faceArray, UInt32 offset = 0, UInt32 size = 0);

	//! Construct from a face indices array and a specified triangle format.
	//! @param faceArray Pointer on an 4 bytes elements faces array.
	//! @param numElements Number of elements contains by faceArray.
	//! @param faceType Accepted values are O3DGLTriangles, O3DGLTriangleStrip, O3DTriangleFans.
	FaceArrayVisitor(
			UInt16 *faceArray,
			UInt32 numElements,
			PrimitiveFormat faceType);

	//! Construct from a face indices array and a specified triangle format.
	//! @param faceArray Pointer on an 4 bytes elements faces array.
	//! @param numElements Number of elements contains by faceArray.
	//! @param faceType Accepted values are Triangles, TriangleStrip, TriangleFans.
	FaceArrayVisitor(
			UInt32 *faceArray,
			UInt32 numElements,
			PrimitiveFormat faceType);

	//! Destructor. Unlock the face if not done manually.
	~FaceArrayVisitor();

	//! Get the begin triangle iterator.
	inline const FaceArrayIterator& begin() const { return m_beginIt; }

	//! Get the end triangle iterator.
	inline const FaceArrayIterator& end() const { return m_endIt; }

	//! Random access to a specified triangle index.
	inline void getIndices(UInt32 id, UInt32 &a, UInt32 &b, UInt32 &c)
	{
		getData(this, id, a, b, c);
	}

	//! Random access to a specified triangle index.
	inline void getIndices(UInt32 id, UInt32 *indices)
	{
		getArray(this, id, indices);
	}

	//! Get the number of triangles.
	UInt32 getNumTriangles() const;

	//! Get the triangles format.
	inline PrimitiveFormat getFormat() const { return m_format; }

	//! Get the number of elements.
	inline UInt32 getNumElements() const { return m_numIndices; }

	//! Compute the real number of triangles.
	UInt32 computeNumTriangle();

private:

	class AccessTriangle16
	{
	public:

		static void getData(FaceArrayVisitor *in, UInt32 id, UInt32 &a, UInt32 &b, UInt32 &c);
		static void getArray(FaceArrayVisitor *in, UInt32 id, UInt32 *v);
	};

	class AccessTriangle32
	{
	public:

		static void getData(FaceArrayVisitor *in, UInt32 id, UInt32 &a, UInt32 &b, UInt32 &c);
		static void getArray(FaceArrayVisitor *in, UInt32 id, UInt32 *v);
	};

	class AccessTriangleFans16
	{
	public:

		static void getData(FaceArrayVisitor *in, UInt32 id, UInt32 &a, UInt32 &b, UInt32 &c);
		static void getArray(FaceArrayVisitor *in, UInt32 id, UInt32 *v);
	};

	class AccessTriangleFans32
	{
	public:

		static void getData(FaceArrayVisitor *in, UInt32 id, UInt32 &a, UInt32 &b, UInt32 &c);
		static void getArray(FaceArrayVisitor *in, UInt32 id, UInt32 *v);
	};

	class AccessTriangleStrip16
	{
	public:

		static void getData(FaceArrayVisitor *in, UInt32 id, UInt32 &a, UInt32 &b, UInt32 &c);
		static void getArray(FaceArrayVisitor *in, UInt32 id, UInt32 *v);
	};

	class AccessTriangleStrip32
	{
	public:

		static void getData(FaceArrayVisitor *in, UInt32 id, UInt32 &a, UInt32 &b, UInt32 &c);
		static void getArray(FaceArrayVisitor *in, UInt32 id, UInt32 *v);
	};

	void (*getData)(FaceArrayVisitor *in, UInt32 id, UInt32 &, UInt32 &, UInt32 &);
	void (*getArray)(FaceArrayVisitor *in, UInt32 id, UInt32 *);

    FaceArray *m_faceArray;    //!< Currently locked face array (can be null).

	PrimitiveFormat m_format;     //!< Face format.

	UInt16 *m_dataUInt16;     //!< Pointer of the face indices 16 bits.
	UInt32 *m_dataUInt32;     //!< Pointer of the face indices 32 bits.

	FaceArrayIterator m_beginIt;
	FaceArrayIterator m_endIt;

	UInt32 m_numIndices;      //!< Number of elements.

	Bool m_step;              //!< Two step for triangle strip.
};

} // namespace o3d

#endif // _O3D_FACEARRAYVISITOR_H
