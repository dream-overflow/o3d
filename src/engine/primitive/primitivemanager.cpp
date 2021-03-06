/**
 * @file primitivemanager.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/primitive/primitivemanager.h"

#include "o3d/engine/object/geometrydata.h"
#include "o3d/engine/shader/shadermanager.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/drawinfo.h"

#include "o3d/engine/primitive/cube.h"
#include "o3d/engine/primitive/sphere.h"
#include "o3d/engine/primitive/cylinder.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(PrimitiveManager, ENGINE_PRIMITIVE_MANAGER, SceneEntity)

// Default constructor.
PrimitiveAccess::PrimitiveAccess(PrimitiveManager *manager, const DrawInfo &drawInfo) :
	m_manager(manager)
{
	O3D_ASSERT(manager);
    m_manager->bind(drawInfo);
}

// Destructor.
PrimitiveAccess::~PrimitiveAccess()
{
	m_manager->unbind();
}

// Default constructor.
PrimitiveManager::PrimitiveManager(BaseObject *parent) :
		SceneEntity(parent),
    m_pickableId(0),
    m_numUsage(0),
    m_vertices(1024*3, 1024*3),
    m_colors(1024*4, 1024*4),
    m_verticesVbo(getScene()->getContext()),
    m_colorsVbo(getScene()->getContext()),
    m_quadVertices(getScene()->getContext()),
    m_quadTexCoords(getScene()->getContext()),
    m_quadColors(getScene()->getContext())
{
    m_primitives.resize(SOLID_CUBE1+1);

    // @todo remplacer avec des VBO plus globaux
    createPrimitive(WIRE_CYLINDER1, new Cylinder(1.f, 1.f, 1.f, 12, 1, Primitive::WIRED_MODE));
    createPrimitive(WIRE_CYLINDER2, new Cylinder(1.f, 1.f, 1.f, 8, 1, Primitive::WIRED_MODE));
    createPrimitive(SOLID_CYLINDER1, new Cylinder(1.f, 1.f, 1.f, 8, 1, Primitive::FILLED_MODE));
    createPrimitive(WIRE_CONE1, new Cylinder(1.f, 0.f, 1.f, 8, 1, Primitive::WIRED_MODE));
    createPrimitive(SOLID_CONE1, new Cylinder(1.f, 0.f, 1.f, 12, 1, Primitive::FILLED_MODE));
    createPrimitive(WIRE_SPHERE1, new Sphere(1.f, 12, 8, Primitive::WIRED_MODE));
    createPrimitive(WIRE_SPHERE2, new Sphere(1.f, 8, 6, Primitive::WIRED_MODE));
    createPrimitive(SOLID_SPHERE1, new Sphere(1.f, 12, 8, Primitive::FILLED_MODE));
    createPrimitive(WIRE_CUBE1, new Cube(1.f, 0, Cube::GRID_CUBE/*WIRED_MODE*/));
    createPrimitive(SOLID_CUBE1, new Cube(1.f, 0, Primitive::FILLED_MODE));

    //
	// create a simple uniform color shader
    //

	Shader *shader = getScene()->getShaderManager()->addShader("primitiveShader");
	shader->buildInstance(m_colorShader.instance);

    m_colorShader.instance.assign("vertexColor", "vertexColor", "", Shader::BUILD_COMPILE_AND_LINK);

	m_colorShader.u_modelViewProjectionMatrix = m_colorShader.instance.getUniformLocation("u_modelViewProjectionMatrix");
	m_colorShader.u_color = m_colorShader.instance.getUniformLocation("u_color");
	m_colorShader.u_scale = m_colorShader.instance.getUniformLocation("u_scale");
	m_colorShader.a_vertex = m_colorShader.instance.getAttributeLocation("a_vertex");
	m_colorShader.a_color = m_colorShader.instance.getAttributeLocation("a_color");

    if (!m_colorShader.instance.isOperational()) {
		O3D_ERROR(E_InvalidResult("Primitive rendering color shader is not operational"));
    }

    //
    // and a simple shader for the picking mode
    //

    shader = getScene()->getShaderManager()->addShader("primitiveShader");
    shader->buildInstance(m_pickingShader.instance);

    m_pickingShader.instance.assign("vertexPicking", "vertexPicking", "", Shader::BUILD_COMPILE_AND_LINK);

    m_pickingShader.u_modelViewProjectionMatrix = m_pickingShader.instance.getUniformLocation("u_modelViewProjectionMatrix");
    m_pickingShader.u_picking = m_pickingShader.instance.getUniformLocation("u_picking");
    m_pickingShader.u_scale = m_pickingShader.instance.getUniformLocation("u_scale");
    m_pickingShader.a_vertex = m_pickingShader.instance.getAttributeLocation("a_vertex");
    // m_pickingShader.a_picking = m_pickingShader.instance.getAttributeLocation("a_picking");

    if (!m_pickingShader.instance.isOperational()) {
        O3D_ERROR(E_InvalidResult("Primitive rendering picking shader is not operational"));
    }

    //
    // setup the vertex data
    //

	m_verticesVbo.create(1024*3, VertexBuffer::STREAMED);
	m_colorsVbo.create(1024*4, VertexBuffer::STREAMED);

	m_objects.resize(XYZ_AXIS_LINES+1);

	// Y axis aligned quad
	{
		const Float vertices[] = { 0,0,0, 0,0,1, 1,0,1, 1,0,0 };
		const Float colors[] = { 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, };

        Object *object = new Object(getScene()->getContext());

		object->format = P_POINTS;
		object->vertices.create(4*3, VertexBuffer::STATIC, vertices);
		object->colors.create(4*4, VertexBuffer::STATIC, colors);

		m_objects[Y_AXIS_ALIGNED_QUAD] = object;
	}

	// AXIS lines
	{
		const Float vertices[] = { 0,0,0, 1,0,0, 0,0,0, 0,1,0, 0,0,0, 0,0,1 };
        const Float colors[] = { 1,0,0,1, 1,0,0,1, 0,0.6,0,1, 0,0.6,0,1, 0,0,1,1, 0,0,1,1 };

        Object *object = new Object(getScene()->getContext());

		object->format = P_LINES;
		object->vertices.create(6*3, VertexBuffer::STATIC, vertices);
		object->colors.create(6*4, VertexBuffer::STATIC, colors);

		m_objects[XYZ_AXIS_LINES] = object;
	}

	{
		const Float vertices[12] = {
				 1, -1, 0,
				-1, -1, 0,
				 1,  1, 0,
				-1,  1, 0 };
		const Float texCoords[8] = {
				1.f, 0.f,
				0.0f, 0.f,
				1.f, 1.f,
				0.f, 1.f };
        const Float colors[12] = {
                 1, 1, 1,
                 1, 1, 1,
                 1, 1, 1,
                 1, 1, 1 };

		m_quadVertices.create(12, VertexBuffer::STATIC, vertices);
		m_quadTexCoords.create(8, VertexBuffer::STATIC, texCoords);
        m_quadColors.create(12, VertexBuffer::STATIC, colors);
	}
}

// Virtual destructor.
PrimitiveManager::~PrimitiveManager()
{
	// delete primitives
    for (IT_GeometryVector it = m_primitives.begin(); it != m_primitives.end(); ++it) {
		deletePtr(*it);
	}

	// delete objects
    for (IT_ObjectVector it = m_objects.begin(); it != m_objects.end(); ++it) {
		deletePtr(*it);
	}

	// delete registered objects
    for (IT_ObjectVector it = m_registeredObjects.begin(); it != m_registeredObjects.end(); ++it) {
		deletePtr(*it);
	}
}

// Setup before draw any primitives.
void PrimitiveManager::bind(const DrawInfo &drawInfo)
{
    if (drawInfo.pass == DrawInfo::AMBIENT_PASS) {
        if (m_pickingShader.instance.isInUse()) {
            O3D_ERROR(E_InvalidOperation("Inconsistency of binding draw mode"));
        }

        // bind the shader
        if (!m_colorShader.instance.isInUse()) {
            getScene()->getContext()->simpleDrawMode();

            m_colorShader.instance.bindShader();

            getScene()->getContext()->bindDefaultVertexArray();
            getScene()->getContext()->enableVertexAttribArray(m_colorShader.a_vertex);
            getScene()->getContext()->enableVertexAttribArray(m_colorShader.a_color);
        }
    } else if (drawInfo.pass == DrawInfo::PICKING_PASS) {
        if (m_colorShader.instance.isInUse()) {
            O3D_ERROR(E_InvalidOperation("Inconsistency of binding draw mode"));
        }

        // bind the shader
        if (!m_pickingShader.instance.isInUse()) {
            getScene()->getContext()->simpleDrawMode();

            m_pickingShader.instance.bindShader();

            getScene()->getContext()->bindDefaultVertexArray();
            getScene()->getContext()->enableVertexAttribArray(m_pickingShader.a_vertex);
        }
    }

	++m_numUsage;
}

// Always restore after draw.
void PrimitiveManager::unbind()
{
    if (m_numUsage <= 0) {
		O3D_ERROR(E_InvalidOperation("Attempt to unbind the primitive manager whereas it was not previously bound"));
    }

	// unbound the shader
    if (m_numUsage == 1) {
        if (m_colorShader.instance.isInUse()) {
            getScene()->getContext()->normalDrawMode();

            getScene()->getContext()->disableVertexAttribArray(m_colorShader.a_vertex);
            getScene()->getContext()->disableVertexAttribArray(m_colorShader.a_color);

            m_colorShader.instance.unbindShader();
        } else if (m_pickingShader.instance.isInUse()) {
            getScene()->getContext()->normalDrawMode();

            getScene()->getContext()->disableVertexAttribArray(m_pickingShader.a_vertex);
            // getScene()->getContext()->disableVertexAttribArray(m_pickingShader.a_picking);

            m_pickingShader.instance.unbindShader();
        }
    }

	--m_numUsage;
}

void PrimitiveManager::createPrimitive(PrimitiveManager::Primitives type, Primitive *primitive)
{
    GeometryData *geometry = new GeometryData(this, *primitive);

    SmartArrayFloat colorArray(primitive->getNumVertices()*4);
    for (UInt32 i = 0; i < colorArray.getNumElt(); ++i) {
		colorArray.getData()[i] = 1.0f;
	}

	geometry->createElement(V_COLOR_ARRAY, colorArray);
	geometry->create();
	geometry->bindFaceArray(0);

	m_primitives[type] = geometry;

    delete primitive;
}

// Register a user object.
UInt32 PrimitiveManager::registerObject(
		PrimitiveFormat format,
		const ArrayFloat &vertices,
		const ArrayFloat &colors)
{
    Object *object = new Object(getScene()->getContext());

	object->format = format;
	object->vertices.create(vertices.getSize(), VertexBuffer::STATIC, vertices.getData());
	object->colors.create(colors.getSize(), VertexBuffer::STATIC, colors.getData());

	m_registeredObjects.push_back(object);

	return m_registeredObjects.size() - 1;
}

// Set the color to use to draw.
void PrimitiveManager::setColor(Float r, Float g, Float b, Float a)
{
	m_color.set(r,g,b,a);

    if (m_colorShader.instance.isInUse()) {
        m_colorShader.instance.setConstColor(m_colorShader.u_color, m_color);
    }
}

void PrimitiveManager::setColor(const Color &color)
{
	m_color = color;

    if (m_colorShader.instance.isInUse()) {
        m_colorShader.instance.setConstColor(m_colorShader.u_color, m_color);
    }
}

void PrimitiveManager::setPickableId(UInt32 id)
{
    m_pickableId = id;

    if (m_pickingShader.instance.isInUse()) {
        m_pickingShader.instance.setConstUInt(m_pickingShader.u_picking, m_pickableId);
    }
}

// Define the modelview*projection matrix using the GLContext current projection*modelview.
void PrimitiveManager::setModelviewProjection()
{
    if (m_colorShader.instance.isInUse()) {
        m_colorShader.instance.setConstMatrix4(
                    m_colorShader.u_modelViewProjectionMatrix,
                    False,
                    getScene()->getContext()->modelViewProjection());
    } else if (m_pickingShader.instance.isInUse()) {
        m_pickingShader.instance.setConstMatrix4(
                    m_pickingShader.u_modelViewProjectionMatrix,
                    False,
                    getScene()->getContext()->modelViewProjection());
    }
}

// Define the modelview*projection matrix before draw.
void PrimitiveManager::setModelviewProjection(const Matrix4 &matrix)
{
    if (m_colorShader.instance.isInUse()) {
        m_colorShader.instance.setConstMatrix4(
                    m_colorShader.u_modelViewProjectionMatrix,
                    False,
                    matrix);
    } else if (m_pickingShader.instance.isInUse()) {
        m_pickingShader.instance.setConstMatrix4(
                    m_pickingShader.u_modelViewProjectionMatrix,
                    False,
                    matrix);
    }
}

// Draw the triplet of local axis
void PrimitiveManager::drawLocalAxis()
{
    Context *glContext = getScene()->getContext();
    setModelviewProjection();

    if (m_colorShader.instance.isInUse()) {
        setColor(1.0f, 1.0f, 1.0f);
        drawXYZAxis(Vector3(1,1,1));

//        return;
//        Matrix4 x(glContext->modelView().get()),y,z;
//        Matrix4 m;
//        m.Translate(0.8, 0, 0);
//        x *= m;
//        m.Identity(); m.RotateZ(o3d::toRadian(-90.f));
//        x *= m;

        glContext->modelView().push();
            //glContext->ModelView().set(x);
            glContext->modelView().translate(Vector3(0.8f, 0, 0));
            glContext->modelView().rotateZ(o3d::toRadian(-90.f));
            setColor(0.8f, 0.f, 0.f);
            draw(SOLID_CONE1, Vector3(0.1f, 0.2f, 0.1f));
        glContext->modelView().pop();

        glContext->modelView().push();
            glContext->modelView().translate(Vector3(0, 0.8f, 0));
            setColor(0.f, 0.6f, 0.f);
            draw(SOLID_CONE1, Vector3(0.1f,0.2f,0.1f));
        glContext->modelView().pop();

        glContext->modelView().push();
            glContext->modelView().translate(Vector3(0, 0, 0.8f));
            glContext->modelView().rotateX(o3d::toRadian(90.f));
            setColor(0.f, 0.f, 0.6f);
            draw(SOLID_CONE1, Vector3(0.1f, 0.2f, 0.1f));
        glContext->modelView().pop();
    } else if (m_pickingShader.instance.isInUse()) {
        drawXYZAxis(Vector3(1,1,1));

        glContext->modelView().push();
            glContext->modelView().translate(Vector3(0.8f, 0, 0));
            glContext->modelView().rotateZ(o3d::toRadian(-90.f));
            draw(SOLID_CONE1, Vector3(0.1f, 0.2f, 0.1f));
        glContext->modelView().pop();

        glContext->modelView().push();
            glContext->modelView().translate(Vector3(0, 0.8f, 0));
            draw(SOLID_CONE1, Vector3(0.1f, 0.2f, 0.1f));
        glContext->modelView().pop();

        glContext->modelView().push();
            glContext->modelView().translate(Vector3(0, 0, 0.8f));
            glContext->modelView().rotateX(o3d::toRadian(90.f));
            draw(SOLID_CONE1, Vector3(0.1f, 0.2f, 0.1f));
        glContext->modelView().pop();
    }
}

// Draw a wire bounding box
void PrimitiveManager::boundingBox(const AABBox &bbox, const Color &color)
{
    if (m_colorShader.instance.isInUse()) {
        setColor(color);
    }

    Context *glContext = getScene()->getContext();

    glContext->modelView().push();
        glContext->modelView().translate(bbox.getCenter());
        draw(WIRE_CUBE1, bbox.getHalfSize() * 2.0f);
    glContext->modelView().pop();
}

// Draw a wire bounding box extended
void PrimitiveManager::boundingBox(const AABBoxExt &bbox, const Color &color)
{
    if (m_colorShader.instance.isInUse()) {
        setColor(color);
    }

    Context *glContext = getScene()->getContext();

    glContext->modelView().push();
        glContext->modelView().translate(bbox.getCenter());
        draw(WIRE_CUBE1, bbox.getHalfSize() * 2.0f);
    glContext->modelView().pop();

    glContext->modelView().push();
        glContext->modelView().translate(bbox.getCenter());
        draw(WIRE_SPHERE2, Vector3(bbox.getRadius(),bbox.getRadius(),bbox.getRadius()));
    glContext->modelView().pop();
}

// Draw a wire bounding sphere
void PrimitiveManager::boundingSphere(const BSphere &bsphere, const Color &color)
{
    if (m_colorShader.instance.isInUse()) {
        setColor(color);
    }

	Context *glContext = getScene()->getContext();

	glContext->modelView().push();
		glContext->modelView().translate(bsphere.getCenter());
		draw(WIRE_SPHERE2, Vector3(bsphere.getRadius(),bsphere.getRadius(),bsphere.getRadius()));
	glContext->modelView().pop();
}

// Draw an Y axis aligned quad.
void PrimitiveManager::drawYAxisAlignedQuad(PrimitiveFormat format, const Vector3 &scale)
{
	Object *object = m_objects[Y_AXIS_ALIGNED_QUAD];
	drawArray(format, object->vertices, object->colors, scale);
}

// Draw an XYZ axis lines.
void PrimitiveManager::drawXYZAxis(const Vector3 &scale)
{
	Object *object = m_objects[XYZ_AXIS_LINES];
	drawArray(P_LINES, object->vertices, object->colors, scale);
}

// Draw an object.
void PrimitiveManager::drawObject(UInt32 objectId, const Vector3 &scale)
{
    if (objectId >= m_registeredObjects.size()) {
		O3D_ERROR(E_InvalidParameter("Unregistered object identifier"));
    }

	Object *object = m_registeredObjects[objectId];
    if (!object) {
		O3D_ERROR(E_InvalidParameter("Null object"));
    }

    drawArray(object->format, object->vertices, object->colors, scale);
}

void PrimitiveManager::setScale(const Vector3 &scale)
{
    if (m_colorShader.instance.isInUse()) {
        m_colorShader.instance.setConstVector3(m_colorShader.u_scale, scale);
    } else if (m_pickingShader.instance.isInUse()) {
        m_pickingShader.instance.setConstVector3(m_pickingShader.u_scale, scale);
    }
}

// Access to the ModelView matrix.
ModelViewMatrix& PrimitiveManager::modelView()
{
	return getScene()->getContext()->modelView();
}

// Access to the ModelView matrix (read only).
const ModelViewMatrix& PrimitiveManager::modelView() const
{
	return getScene()->getContext()->modelView();
}

// Access to the Projection matrix.
ProjectionMatrix& PrimitiveManager::projection()
{
	return getScene()->getContext()->projection();
}

// Access to the Projection matrix (read only).
const ProjectionMatrix& PrimitiveManager::projection() const
{
	return getScene()->getContext()->projection();
}

void PrimitiveManager::draw(PrimitiveManager::Primitives type, const Vector3 &scale)
{
	GeometryData *geometry = m_primitives[type];
    setModelviewProjection();

    if (m_colorShader.instance.isInUse()) {
        m_colorShader.instance.setConstVector3(m_colorShader.u_scale, scale);
        geometry->attribute(V_VERTICES_ARRAY, m_colorShader.a_vertex);
        geometry->attribute(V_COLOR_ARRAY, m_colorShader.a_color);

        geometry->draw();

        getScene()->getContext()->disableVertexAttribArray(m_colorShader.a_vertex);
        getScene()->getContext()->disableVertexAttribArray(m_colorShader.a_color);
    } else if (m_pickingShader.instance.isInUse()) {
        m_pickingShader.instance.setConstVector3(m_pickingShader.u_scale, scale);
        geometry->attribute(V_VERTICES_ARRAY, m_pickingShader.a_vertex);

        geometry->draw();

        getScene()->getContext()->disableVertexAttribArray(m_pickingShader.a_vertex);
    }
}

// Draw a set of VBO (need vertex(XYZ) and color(RGBA).
void PrimitiveManager::drawArray(
		PrimitiveFormat format,
		const ArrayBufferf &vertices,
		const ArrayBufferf &colors,
		const Vector3 &scale)
{
    if (m_colorShader.instance.isInUse()) {
        m_colorShader.instance.setConstVector3(m_colorShader.u_scale, scale);

        // vertices
        vertices.bindBuffer();
        getScene()->getContext()->vertexAttribArray(m_colorShader.a_vertex, 3, 0, 0);

        // colors
        colors.bindBuffer();
        getScene()->getContext()->vertexAttribArray(m_colorShader.a_color, 4, 0, 0);

        // and draw
        getScene()->drawArrays(format, 0, vertices.getCount() / 3);

        getScene()->getContext()->disableVertexAttribArray(m_colorShader.a_vertex);
        getScene()->getContext()->disableVertexAttribArray(m_colorShader.a_color);
    } else if (m_pickingShader.instance.isInUse()) {
        m_pickingShader.instance.setConstVector3(m_pickingShader.u_scale, scale);

        // vertices
        vertices.bindBuffer();
        getScene()->getContext()->vertexAttribArray(m_pickingShader.a_vertex, 3, 0, 0);

        // and draw
        getScene()->drawArrays(format, 0, vertices.getCount() / 3);

        getScene()->getContext()->disableVertexAttribArray(m_pickingShader.a_vertex);
    }
}

// Start a draw list.
void PrimitiveManager::beginDraw(PrimitiveFormat format)
{
	m_format = format;

	m_vertices.forceSize(0);
	m_colors.forceSize(0);
}

// End a draw list, called after using BeginDraw.
void PrimitiveManager::endDraw()
{
    if ((m_vertices.getNumElt() == 0) || (m_colors.getNumElt() == 0)) {
		return;
    }

	setModelviewProjection();

    if (m_colorShader.instance.isInUse()) {
        m_colorShader.instance.setConstVector3(m_colorShader.u_scale, Vector3(1,1,1));

        // vertices
        if ((UInt32)m_vertices.getSize() <= m_verticesVbo.getCount())
            m_verticesVbo.update(m_vertices.getData(), 0, m_vertices.getSize());
        else
            m_verticesVbo.create(
                        m_vertices.getSize(),
                        VertexBuffer::STREAMED,
                        m_vertices.getData(),
                        True);

        getScene()->getContext()->vertexAttribArray(m_colorShader.a_vertex, 3, 0, 0);

        // colors
        if ((UInt32)m_colors.getSize() <= m_colorsVbo.getCount())
            m_colorsVbo.update(m_colors.getData(), 0, m_colors.getSize());
        else
            m_colorsVbo.create(
                        m_colors.getSize(),
                        VertexBuffer::STREAMED,
                        m_colors.getData(),
                        True);

        getScene()->getContext()->vertexAttribArray(m_colorShader.a_color, 4, 0, 0);

        // and draw
        getScene()->drawArrays(m_format, 0, m_vertices.getNumElt() / 3);

        getScene()->getContext()->disableVertexAttribArray(m_colorShader.a_vertex);
        getScene()->getContext()->disableVertexAttribArray(m_colorShader.a_color);
    } else if (m_pickingShader.instance.isInUse()) {
        m_pickingShader.instance.setConstVector3(m_pickingShader.u_scale, Vector3(1,1,1));

        // vertices
        if ((UInt32)m_vertices.getSize() <= m_verticesVbo.getCount())
            m_verticesVbo.update(m_vertices.getData(), 0, m_vertices.getSize());
        else
            m_verticesVbo.create(
                        m_vertices.getSize(),
                        VertexBuffer::STREAMED,
                        m_vertices.getData(),
                        True);

        getScene()->getContext()->vertexAttribArray(m_pickingShader.a_vertex, 3, 0, 0);

        // and draw
        getScene()->drawArrays(m_format, 0, m_vertices.getNumElt() / 3);

        getScene()->getContext()->disableVertexAttribArray(m_pickingShader.a_vertex);
    }
}

// Add a vertex with a color into the draw list.
void PrimitiveManager::addVertex(const Float *vertex)
{
	static Float whiteColor[4] = { 1.f, 1.f, 1.f, 1.f };

	m_vertices.pushArray(vertex, 3);
	m_colors.pushArray(whiteColor, 4);
}

// Add a vertex with a color into the draw list.
void PrimitiveManager::addVertex(const Vector3 &vertex)
{
	static Float whiteColor[4] = { 1.f, 1.f, 1.f, 1.f };

	m_vertices.pushArray(vertex.getData(), 3);
	m_colors.pushArray(whiteColor, 4);
}

// Add a vertex into the draw list.
void PrimitiveManager::addVertex(const Vector3 &vertex, const Color &color)
{
	m_vertices.pushArray(vertex.getData(), 3);
	m_colors.pushArray(color.getData(), 4);
}

// Add a vertex into the draw list.
void PrimitiveManager::addVertex(const Float *vertex, const Color &color)
{
	m_vertices.pushArray(vertex, 3);
	m_colors.pushArray(color.getData(), 4);
}

// Add a vertex into the draw list.
void PrimitiveManager::addVertex(const Float *vertex, const Float *color)
{
	m_vertices.pushArray(vertex, 3);
	m_colors.pushArray(color, 4);
}
