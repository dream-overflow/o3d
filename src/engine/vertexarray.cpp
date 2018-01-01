/**
 * @file vertexarray.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/glextdefines.h"

#include "o3d/engine/vertexarray.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/context.h"
#include "o3d/engine/shader/shadable.h"
#include "o3d/engine/object/vertexelement.h"

using namespace o3d;

// Default constructor.
VertexArray::VertexArray(Context *context) :
    m_context(context),
	m_vaoId(O3D_UNDEFINED)
{
    O3D_ASSERT(m_context != nullptr);
}

// Destructor.
VertexArray::~VertexArray()
{
	release();
}

// Create/update the VAO using source from a shadable object.
void VertexArray::create(const T_ElementsList &attributes)
{
    if (m_vaoId != O3D_UNDEFINED) {
		release();
    }

    if (m_vaoId == O3D_UNDEFINED) {
		glGenVertexArrays(1, (GLuint*)&m_vaoId);
    }

	m_context->bindVertexArray(m_vaoId, &m_state);

	UInt32 vbo = 0;

	size_t size = attributes.size();
    for (size_t i = 0; i < size; ++i) {
        m_context->forceVertexAttribArray(attributes[i].array, True);

        if (attributes[i].vbo != vbo) {
			m_context->bindVertexBuffer(attributes[i].vbo);
			vbo = attributes[i].vbo;
		}

		glVertexAttribPointer(
				attributes[i].array,
				attributes[i].eltSize,
                attributes[i].type,
				GL_FALSE,
                attributes[i].stride,
                (const GLvoid*) ((GLubyte*) 0 + attributes[i].offset));
	}

    m_context->bindDefaultVertexArray();

    if (vbo != 0) {
        m_context->bindVertexBuffer(0);
    }
}

// Create/update the VAO using source from a shadable object and a list of vertex attribute.
void VertexArray::create(const T_VertexAttributeList &attributes, Shadable &shadable)
{
    if (m_vaoId != O3D_UNDEFINED) {
		release();
    }

    if (m_vaoId == O3D_UNDEFINED) {
		glGenVertexArrays(1, (GLuint*)&m_vaoId);
    }

	m_context->bindVertexArray(m_vaoId, &m_state);

	UInt32 vbo = 0;

    VertexElement *element = nullptr;

	size_t size = attributes.size();
    for (size_t i = 0; i < size; ++i) {
		element = shadable.getVertexElement(attributes[i]);

        if (element) {
            m_context->forceVertexAttribArray(attributes[i], True);

            if (element->getVbo().getBufferId() != vbo) {
				m_context->bindVertexBuffer(element->getVbo().getBufferId());
                vbo = element->getVbo().getBufferId();
			}

			glVertexAttribPointer(
					attributes[i],
					element->getElementSize(),
					element->getDataType(),
					GL_FALSE,
					element->getStride() * sizeof(Float),
					(const GLvoid*) ((GLubyte*) 0 + element->getOffset() * sizeof(Float)));            
        }
	}

    m_context->bindDefaultVertexArray();

    if (vbo != 0) {
        m_context->bindVertexBuffer(0);
    }
}

// Release the VAO.
void VertexArray::release()
{
    if (m_vaoId != O3D_UNDEFINED) {
		m_context->deleteVertexArray(m_vaoId);
		m_vaoId = O3D_UNDEFINED;
		m_state = VertexArrayState();
	}
}

// Bind the VAO if necessary.
void VertexArray::bindArray() const
{
    m_context->bindVertexArray(m_vaoId, &m_state);
}

// Unbind the VAO (bind the VAO 0).
void VertexArray::unbindArray() const
{
    m_context->bindVertexArray(0, nullptr);
}
