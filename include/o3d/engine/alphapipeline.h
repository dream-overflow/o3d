/**
 * @file alphapipeline.h
 * @brief Alpha blended face sorting manager.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-03-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ALPHAPIPELINE_H
#define _O3D_ALPHAPIPELINE_H

#include "o3d/core/memorydbg.h"
#include "o3d/core/radixsort.h"
#include "o3d/core/templatearray.h"

#include "shader/shadable.h"
#include "scene/sceneentity.h"

namespace o3d {

class MaterialTechnique;

//---------------------------------------------------------------------------------------
//! @class AlphaPipelineToken
//-------------------------------------------------------------------------------------
//! Alpha pipeline token. It contain three vertex index that define a face, a shadable
//! object with a shader object for rendering. Define an O3DGLTriangles format.
//---------------------------------------------------------------------------------------
class O3D_API AlphaPipelineToken
{
public:

	//! Default constructor
	AlphaPipelineToken(
            Shadable* object = nullptr,
            MaterialTechnique* material = nullptr,
			UInt32 p1 = 0,
			UInt32 p2 = 0,
            UInt32 p3 = 0)
	{
		m_shadable		 = object;
		m_material       = material;
		m_vertexIndex[0] = p1;
		m_vertexIndex[1] = p2;
		m_vertexIndex[2] = p3;
	}

	//! Set parameters
	void set(
            Shadable* object = nullptr,
            MaterialTechnique* material = nullptr,
			UInt32 p1 = 0,
			UInt32 p2 = 0,
			UInt32 p3 = 0)
	{
		m_shadable		 = object;
		m_material       = material;
		m_vertexIndex[0] = p1;
		m_vertexIndex[1] = p2;
		m_vertexIndex[2] = p3;
	}

	// Get vertices.
	inline void getVertices(UInt32 *input) const
	{
		memcpy(input, m_vertexIndex, 3*sizeof(UInt32));
	}

	// Get vertices.
	inline void getVertices(UInt16 *input) const
	{
		input[0] = static_cast<UInt16>(m_vertexIndex[0]);
		input[1] = static_cast<UInt16>(m_vertexIndex[1]);
		input[2] = static_cast<UInt16>(m_vertexIndex[2]);
	}

	//! Return the shadable object
	inline Shadable* getShadable() {	return m_shadable; }

	//! Return the shader object
	inline MaterialTechnique* getMaterial() { return m_material; }

private:

	Shadable* m_shadable;           //!< Shadable object to draw.
	MaterialTechnique* m_material;  //!< Material technique used for this face.
	UInt32 m_vertexIndex[3];       //!< The 3 faces index.
};

class DrawInfo;

//---------------------------------------------------------------------------------------
//! @class AlphaPipeline
//-------------------------------------------------------------------------------------
//! Alpha blended face sorting manager.
//---------------------------------------------------------------------------------------
class O3D_API AlphaPipeline : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(AlphaPipeline)

	//! Default constructor
	AlphaPipeline(BaseObject *pParent);

	//! Destructor
	virtual ~AlphaPipeline();

	//! Reset the inputs and outputs
	void reset();

	//! Resize the input list
	void setInputListSize(UInt32 size);

	//! Add a new blended face
	inline void addFace(
			Shadable* object,
			MaterialTechnique* material,
			UInt32 p1,
			UInt32 p2,
			UInt32 p3,
			Float z)
	{
		m_numFaces++;
		setInputListSize(m_numFaces);
		m_inputList[m_numFaces-1].set(object,material,p1,p2,p3);
		m_inputZ[m_numFaces-1] = z;
	}

	//! Add a new blended face
	inline void addFace(
			Shadable* object,
			MaterialTechnique* material,
			UInt32 *p,
			Float z)
	{
		m_numFaces++;
		setInputListSize(m_numFaces);
		m_inputList[m_numFaces-1].set(object,material,p[0],p[1],p[2]);
		m_inputZ[m_numFaces-1] = z;
	}

	//! Add a new blended face
	inline void addFace(
			Shadable* object,
			MaterialTechnique* material,
			UInt16 *p,
			Float z)
	{
		m_numFaces++;
		setInputListSize(m_numFaces);
		m_inputList[m_numFaces-1].set(object,material,p[0],p[1],p[2]);
		m_inputZ[m_numFaces-1] = z;
	}

	//! Sort all blended faces and pack sorted faces by object.
	void sort();

	//! Process the draw list.
	void draw(const DrawInfo &drawInfo);

	//! Return the total number of call to radix sort (measure performance)
	inline UInt32 getNumTotalCalls()const { return m_radixSort.getNumTotalCalls(); }

	//! Return the number of premature output due to the temporal coherence
	inline UInt32 getNumHints()const { return m_radixSort.getNumHits(); }

protected:

	struct DrawSet
	{
		Shadable *shadable;
		MaterialTechnique *material;
		UInt32 firstIndex;
		UInt32 lastIndex;
		UInt32 countIndex;
		UInt32 minVertex;
		UInt32 maxVertex;
		UInt32 numVertices;
	};

	UInt32 m_numFaces;				//!< Total faces number.
	UInt32 m_inputSize;				//!< Input faces list size.

	AlphaPipelineToken* m_inputList; //!< Input list of token.
	Float* m_inputZ;				//!< Z for each face entry.

	ArrayUInt16 m_outputFacesUInt16; //!< Output face array 32.
	ArrayUInt32 m_outputFacesUInt32; //!< Output face array 16.

	RadixSort m_radixSort;			//!< Radix Sort algorithm.

	FaceArrayUInt16 m_faceArrayUInt16;
	FaceArrayUInt32 m_faceArrayUInt32;

	std::list<DrawSet> m_processList;   //!< Final process draw list.
};

} // namespace o3d

#endif // _O3D_ALPHAPIPELINE_H

