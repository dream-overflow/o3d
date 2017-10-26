/**
 * @file primitivemanager.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PRIMITIVEMANAGER_H
#define _O3D_PRIMITIVEMANAGER_H

#include "o3d/core/baseobject.h"
#include "primitive.h"
#include "../scene/sceneentity.h"
#include "../vertexbuffer.h"
#include "../matrix.h"
#include "../shader/shader.h"
#include "../enginetype.h"
#include "o3d/core/debug.h"

#include <vector>

namespace o3d {

class GeometryData;
class PrimitiveManager;

class BSphere;
class AABBox;
class AABBoxExt;

/**
 * @brief Manager manager usage helper.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2009-09-16
 * A simple object that set at constructor and automatically reset at destructor,
 * the usage of the primitive rendering.
 */
class O3D_API PrimitiveAccess
{
public:

	//! Default constructor.
	PrimitiveAccess(PrimitiveManager *manager);

	//! Destructor.
	~PrimitiveAccess();

	//! Operator () to access transparently the the O3DPrimitiveManager.
	PrimitiveManager* operator-> () { return m_manager; }

	//! Operator () to access transparently the the O3DPrimitiveManager
	//! (read only).
	const PrimitiveManager* operator-> () const { return m_manager; }

	//! Operator () to access transparently the the O3DPrimitiveManager.
	PrimitiveManager& operator* () { return *m_manager; }

	//! Operator () to access transparently the the O3DPrimitiveManager
	//! (read only).
	const PrimitiveManager& operator* () const { return *m_manager; }

private:

	PrimitiveManager *m_manager;
};

/**
 * @brief Manager that offers many primitives objects for easy rendering.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2009-09-16
 */
class O3D_API PrimitiveManager: public SceneEntity
{
public:

	enum Primitives
	{
		WIRE_CYLINDER1 = 0,
		WIRE_CYLINDER2,
		SOLID_CYLINDER1,
		WIRE_CONE1,
		SOLID_CONE1,
		WIRE_SPHERE1,
		WIRE_SPHERE2,
		SOLID_SPHERE1,
		WIRE_CUBE1
	};

	enum Objects
	{
		Y_AXIS_ALIGNED_QUAD,
		XYZ_AXIS_LINES
	};

	O3D_DECLARE_DYNAMIC_CLASS(PrimitiveManager)

	//! Default constructor.
	PrimitiveManager(BaseObject *parent);

	//! Virtual destructor.
	virtual ~PrimitiveManager();

	//! Setup before draw any primitives.
	void bind();

	//! Always restore after draw.
	void unbind();

	//! Create an instance of PrimitiveAccess and return it.
	PrimitiveAccess access() { return PrimitiveAccess(this); }

	//! Register a user object.
	//! @param format Format of the object draw primitive.
	//! @param vertices Array of vertices.
	//! @param colors Array of colors.
	//! @return The object identifier.
	UInt32 registerObject(
			PrimitiveFormat format,
			const ArrayFloat &vertices,
			const ArrayFloat &colors);

	//! Get the vertices quad VBOs for a 4 indices P_TRIANGLE_STRIP.
	inline VertexBufferObjf& getQuadVerticesVBO() { return m_quadVertices; }
	//! Get the vertices quad VBOs for a 4 indices P_TRIANGLE_STRIP (read only).
	inline const VertexBufferObjf& getQuadVerticesVBO() const { return m_quadVertices; }

	//! Get the textures coordinates quad VBOs for a 4 indices P_TRIANGLE_STRIP.
	inline VertexBufferObjf& getQuadTexCoordsVBO() { return m_quadTexCoords; }
	//! Get the textures coordinates quad VBOs for a 4 indices P_TRIANGLE_STRIP (read only).
	inline const VertexBufferObjf& getQuadTexCoordsVBO() const { return m_quadTexCoords; }

    //! Get the color quad VBOs for a 4 indices P_TRIANGLE_STRIP.
    inline VertexBufferObjf& getQuadColorsVBO() { return m_quadColors; }
    //! Get the color quad VBOs for a 4 indices P_TRIANGLE_STRIP (read only).
    inline const VertexBufferObjf& getQuadColorsVBO() const { return m_quadColors; }


	//-----------------------------------------------------------------------------------
	// Accessors.
	//-----------------------------------------------------------------------------------

	//! Set the global color to use to draw.
	void setColor(Float r, Float g, Float b, Float a = 1.0f);

	//! Set the global color to use to draw.
	void setColor(const Color &color);

	//! Set global color as default color (white)
	inline void setDefaultColor() { setColor(1.f,1.f,1.f,1.f); }

	//! Get the global color used to draw.
	inline const Color& getColor() const { return m_color; }

	//! access to the ModelView matrix. Same as from Context.
	ModelViewMatrix& modelView();

	//! access to the ModelView matrix (read only). Same from Context.
	const ModelViewMatrix& modelView() const;

	//! access to the Projection matrix. Same from Context.
	ProjectionMatrix& projection();

	//! access to the Projection matrix (read only). Same from Context.
	const ProjectionMatrix& projection() const;

	//! Define the modelview*projection matrix using the GLContext current projection*modelview.
	void setModelviewProjection();

	//! Define the modelview*projection matrix before draw.
	void setModelviewProjection(const Matrix4 &matrix);

	//-----------------------------------------------------------------------------------
	// Helper drawing objects. All uses of the context Modelview*Projection matrix.
	//-----------------------------------------------------------------------------------

	//! Draw a primitive using the current defined color.
	//! @param type Type of the primitive to draw.
	//! @param scale Size scale.
	void draw(Primitives type, const Vector3 &scale);

	//! Draw the triplet of local axis.
	void drawLocalAxis();

	//! Draw a wire bounding box.
	void boundingBox(const AABBox &bbox, const Color &color = Color(1.f,1.f,0.f));

	//! Draw a wire bounding box extended.
	void boundingBox(const AABBoxExt &bbox, const Color &color = Color(1.f,1.f,0.f));

	//! Draw a wire bounding sphere.
	void boundingSphere(const BSphere &bsphere, const Color &color = Color(1.f,1.f,0.f));

	//! Draw an Y axis aligned quad.
	void drawYAxisAlignedQuad(PrimitiveFormat format, const Vector3 &scale);

	//! Draw an XYZ axis lines.
	void drawXYZAxis(const Vector3 &scale);

	//! Draw a registered object given its identifier.
	void drawObject(UInt32 objectId, const Vector3 &scale);

	//-----------------------------------------------------------------------------------
	// Draw a set of VBO (need vertex(XYZ) and color(RGBA).
	//-----------------------------------------------------------------------------------

	//! Draw a set of VBO (need vertex(XYZ) and color(RGBA).
	//! @param format Format of the primitive (line, triangle...)
	//! @param vertices A valid VBO containing a set of vertices (XYZ).
	//! @param vertices A valid VBO containing a set of colors (RGBA).
	//! @param scale Scale over the three directions.
	void drawArray(
			PrimitiveFormat format,
			const VertexBufferObjf &vertices,
			const VertexBufferObjf &colors,
			const Vector3 &scale = Vector3(1,1,1));

	//-----------------------------------------------------------------------------------
	// Draw a set of primitives.
	//-----------------------------------------------------------------------------------

	//! Start a draw list. It use of the SetColor parameter for the object color.
	//! @param format Format of the primitive to draw (point, line, triangle).
	//! @note A EndDraw() must be performed when finished to fill the list.
	void beginDraw(PrimitiveFormat format);

	//! End a draw list, called after using BeginDraw.
	//! The draw is process at this call.
	void endDraw();

	//! Add a vertex into the draw list. Color is set to 1,1,1,1 and will be multiplied with the
	//! default color (@see SetColor).
	void addVertex(const Vector3 &vertex);

	//! Add a vertex into the draw list. Color is set to 1,1,1,1 and will be multiplied with the
	//! default color (@see SetColor).
	inline void addVertex(Float x, Float y, Float z) { addVertex(Vector3(x,y,z)); }

	//! Add a vertex into the draw list. Color is set to 1,1,1,1 and will be multiplied with the
	//! default color (@see SetColor).
	//! @param vertex An array that contain a 3 floats XYZ vertex.
	void addVertex(const Float *vertex);

	//! Add a vertex into the draw list.
	void addVertex(const Vector3 &vertex, const Color &color);

	//! Add a vertex into the draw list.
	//! @param vertex An array that contain a 3 floats XYZ vertex.
	void addVertex(const Float *vertex, const Color &color);

	//! Add a vertex into the draw list.
	//! @param vertex An array that contain a 3 floats XYZ vertex.
	//! @param vertex An array that contain a 4 floats RGBA color.
	void addVertex(const Float *vertex, const Float *color);

protected:

	typedef std::vector<GeometryData*> T_GeometryVector;
	typedef T_GeometryVector::iterator IT_GeometryVector;
	typedef T_GeometryVector::const_iterator CIT_GeometryVector;

	T_GeometryVector m_primitives;

	Color m_color;

	struct ColorShader
	{
		ShaderInstance instance;

		Int32 a_vertex;
		Int32 a_color;
		Int32 u_modelViewProjectionMatrix;
		Int32 u_color;
		Int32 u_scale;
	};

	ColorShader m_colorShader;
	Int32 m_numUsage;

	Cylinder m_wireCylinder1;
	Cylinder m_wireCylinder2;
	Cylinder m_solidCylinder1;
	Cylinder m_wireCone1;
	Cylinder m_solidCone1;
	Sphere m_wireSphere1;
	Sphere m_wireSphere2;
	Sphere m_solidSphere1;
	Cube m_wireCube1;

	ArrayFloat m_vertices;
	ArrayFloat m_colors;

	VertexBufferObjf m_verticesVbo;
	VertexBufferObjf m_colorsVbo;

	PrimitiveFormat m_format;

	struct Object
	{
		PrimitiveFormat format;
		VertexBufferObjf vertices;
		VertexBufferObjf colors;

        Object(Context *context) :
            vertices(context),
            colors(context)
		{
		}
	};

	typedef std::vector<Object*> T_ObjectVector;
	typedef T_ObjectVector::iterator IT_ObjectVector;
	typedef T_ObjectVector::const_iterator CIT_ObjectVector;

	T_ObjectVector m_objects;
	T_ObjectVector m_registeredObjects;

	VertexBufferObjf m_quadVertices;
	VertexBufferObjf m_quadTexCoords;
    VertexBufferObjf m_quadColors;

	void createPrimitive(Primitives type, Primitive &primitive);
};

} // namespace o3d

#endif // _O3D_PRIMITIVEMANAGER_H

