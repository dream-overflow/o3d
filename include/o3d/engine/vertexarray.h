/**
 * @file vertexarray.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_VERTEXARRAY_H
#define _O3D_VERTEXARRAY_H

#include "o3d/core/debug.h"
#include "o3d/core/memorydbg.h"

#include "vertexarraystate.h"
#include <vector>

#include "enginetype.h"

namespace o3d {

class Context;
class Shadable;

/**
 * @brief Vertex array object.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2010-11-04
 */
class O3D_API VertexArray
{
public:

	//! Vertex array element.
	struct Element
	{
		VertexAttributeArray array;  //!< Vertex attribute array.
		DataType type;               //!< Data type.
		UInt32 vbo;              //!< Vertex buffer object identifier.
		UInt32 stride;           //!< Stride in bytes.
		UInt32 offset;           //!< Offset in bytes.
		UInt32 eltSize;          //!< Number of element.

		Element() :
			array(V_VERTICES_ARRAY),
			type(DATA_FLOAT),
			vbo(0),
			stride(0),
			offset(0),
			eltSize(0)
		{
		}
	};

	//! Vertex array element list.
	typedef std::vector<Element> T_ElementsList;
	//! Vertex array element list iterator.
	typedef T_ElementsList::iterator IT_ElementsList;
	//! Vertex array element list const iterator.
	typedef T_ElementsList::const_iterator CIT_ElementsList;

	//! Vertex attribute list.
	typedef std::vector<VertexAttributeArray> T_VertexAttributeList;
	//! Vertex arattribute list iterator.
	typedef T_VertexAttributeList::iterator IT_VertexAttributeList;
	//! Vertex attribute list const iterator.
	typedef T_VertexAttributeList::const_iterator CIT_VertexAttributeList;

    //! Constructor.
    VertexArray(Context *context);

	//! Destructor.
	~VertexArray();

	//! Get the gl context (read only).
	inline const Context* getContext() const { return m_context; }
	//! Get the gl context.
	inline Context* getContext() { return m_context; }

	//! Create/update the VAO using source an array of attribute.
	//! @param attributes The list of attribute elements to set into the VAO.
	void create(const T_ElementsList &attributes);

	//! Create/update the VAO using source from a shadable object and a list of vertex attribute.
	//! @param attributes The list of vertex attribute to initialize from the shadable into the VAO.
	//! @param shadable A shadable object.
	void create(const T_VertexAttributeList &attributes, Shadable &shadable);

	//! Release the VAO.
	void release();

	//! Bind the vertex array object.
	void bindArray() const;

	//! Unbind the vertex array object (bind 0).
	void unbindArray() const;

protected:

    Context *m_context;       //!< Related OpenGL context.

    UInt32 m_vaoId;           //!< OpenGL vertex array object identifier.
	mutable VertexArrayState m_state;  //!< Vertex array state.
};

} // namespace o3d

#endif // _O3D_VERTEXARRAY_H

