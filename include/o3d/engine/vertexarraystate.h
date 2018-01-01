/**
 * @file vertexarraystate.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_VERTEXARRAYSTATE_H
#define _O3D_VERTEXARRAYSTATE_H

#include "o3d/core/debug.h"
#include "o3d/core/memorydbg.h"

#include "o3d/core/templatebitset.h"

namespace o3d {

/**
 * @brief Sharable object containing the status of a specific vertex array object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-12-06
 * It keek in mind if a VBO or IBO is bound and to which vertex array indexes (zero, one
 * or more).
 */
class O3D_API VertexArrayState
{
public:

	//! Default constructor.
	VertexArrayState() :
		vbo(O3D_UNDEFINED),
		ibo(O3D_UNDEFINED)
	{
	}

	//! Copy constructor.
	VertexArrayState(const VertexArrayState &dup) :
		vbo(dup.vbo),
		ibo(dup.ibo),
		attributeArray(dup.attributeArray)
	{
	}

	//! Copy operator.
	VertexArrayState& operator=(const VertexArrayState &dup)
	{
		vbo = dup.vbo;
		ibo = dup.ibo;
		attributeArray = dup.attributeArray;

		return *this;
	}

	UInt32 vbo;
	UInt32 ibo;
	BitSet32 attributeArray;
};

} // namespace o3d

#endif // _O3D_VERTEXARRAYSTATE_H
