/**
 * @file picture.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/engine/glextdefines.h"
#include "o3d/gui/widgets/picture.h"
#include "o3d/gui/guitype.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/gui/gui.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/shader/shadermanager.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Picture, GUI_PICTURE, Widget)

// Default initialization constructor from a parent base object.
Picture::Picture(BaseObject *parent) :
    Widget(parent)
{
}

// Default initialization constructor from a parent.
Picture::Picture(
        Widget *parent,
        Image *image,
        const Vector2i &pos,
        const Vector2i &size,
        const String &name) :
    Widget(parent, pos, size, name)
{
    O3D_ASSERT(image != nullptr);

    if ((image == nullptr) || !image->isValid())
        O3D_ERROR(E_InvalidParameter("Image is not valid"));

    m_size = m_minSize = m_maxSize = Vector2i((Int32)image->getWidth(), (Int32)image->getHeight());

    m_texture.setImage(*image);
    m_texture.create(False, True);

    m_capacities.disable(CAPS_FOCUSABLE);
}

// Virtual destructor.
Picture::~Picture()
{

}

// Draw
void Picture::draw()
{
    // empty surface
    if (m_size.x() <= 0 || m_size.y() <= 0)
        return;

    if (!m_texture.isValid())
        return;

    Context *glContext = getScene()->getContext();

    glContext->blending().setFunc(Blending::SRC_A__ONE_MINUS_SRC_A);
    glContext->disableDepthTest();

    m_shader.shaderInstance.bindShader();

    m_shader.shaderInstance.setConstMatrix4(
            m_shader.u_modelViewProjectionMatrix,
            False,
            glContext->modelViewProjection());

    m_shader.shaderInstance.setConstFloat(m_shader.u_alpha, m_alpha);
    m_shader.shaderInstance.setConstTexture(m_shader.u_texture, &m_texture, 0);

    const Float vertices[] = {
            (Float)m_pos.x()+m_size.x(), (Float)m_pos.y(), 0.f,
            (Float)m_pos.x(), (Float)m_pos.y(), 0.f,
            (Float)m_pos.x()+m_size.x(), (Float)m_pos.y()+m_size.y(), 0.f,
            (Float)m_pos.x(), (Float)m_pos.y()+m_size.y(), 0.f };

    m_shader.vertices.create(12, VertexBuffer::DYNAMIC, vertices, True);
    m_shader.vertices.attribute(m_shader.a_vertex, 3, 0, 0);

    const Float texCoords[] = {
            1.f, 0.f,
            0.f, 0.f,
            1.f, 1.f,
            0.f, 1.f };

    m_shader.texCoords.create(8, VertexBuffer::DYNAMIC, texCoords, True);
    m_shader.texCoords.attribute(m_shader.a_texCoords, 2, 0, 0);

    getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);

    glContext->disableVertexAttribArray(m_shader.a_vertex);
    glContext->disableVertexAttribArray(m_shader.a_texCoords);

    m_shader.shaderInstance.unbindShader();
}

void Picture::updateCache()
{
}

Vector2i Picture::getDefaultSize()
{
    return Vector2i(1, 1);
}
