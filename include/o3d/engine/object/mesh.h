/**
 * @file mesh.h
 * @brief A 3d static mesh object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-05-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MESH_H
#define _O3D_MESH_H

#include "../material/material.h"
#include "../material/materialprofile.h"
#include "meshdata.h"
#include "shadableobject.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

/**
 * @brief A 3d static mesh object
 * @details It use a MeshData object which contains the geometry.
 * For skin object mesh data contain the vertex weighting too.
 * The object is draw by it O3DMeshData object using the most adapted method.
 * @note If no shader is defined it use the default one.
 */
class O3D_API Mesh : public ShadableObject
{
public:

	O3D_DECLARE_CLASS(Mesh)

	//! Default constructor.
	Mesh(BaseObject *parent);
	//! Copy constructor.
	Mesh(const Mesh &dup);

	//! Virtual destructor.
	virtual ~Mesh();

	//! Copy operator.
	Mesh& operator=(const Mesh &dup);

	//-----------------------------------------------------------------------------------
	// Settings
	//-----------------------------------------------------------------------------------

	//! Set the mesh data (sharable mesh geometry object).
	//! @note Take care to be consistent with material profiles.
	virtual void setMeshData(MeshData* meshData);

	//! Get the mesh data (sharable mesh geometry object).
	inline MeshData* getMeshData() { return m_meshData.get(); }

	//! Set the number of material profile (and number of sub-mesh to profile).
	//! @note If numProfiles is lesser than previous, some profiles are deleted, and if it
	//! is greater new profiles are created. A profile index correspond to a sub-mesh
	//! (face array) identifier into the mesh geometry.
	void setNumMaterialProfiles(UInt32 numProfiles);

	//! Get the profile at a specified index (correspond to a sub-mesh index) (read only).
	const MaterialProfile& getMaterialProfile(UInt32 index) const;
	//! Get the profile at a specified index (correspond to a sub-mesh index).
	MaterialProfile& getMaterialProfile(UInt32 index);

	//! Get the number of profiles.
	inline UInt32 getNumMaterialProfiles() const
	{
		return static_cast<UInt32>(m_matProfiles.size());
	}

	//! Initialize any material profiles.
	void initMaterialProfiles();

	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	//! Transform the bounding box if necessary.
	virtual void update();

	//! Draw the mesh.
	virtual void draw(const DrawInfo &drawInfo);

	//! Check the global bounding volume with an AABBox.
	//! The outside is pointed by the normal direction of the plane.
	//! @return Geometry::CLIP_INSIDE if the plane clip back with the object bounding,
	//!         Geometry::CLIP_INTERSECT if the plane clip front and back,
	//!         Geometry::CLIP_OUTSIDE otherwise.
	virtual Geometry::Clipping checkBounding(const AABBox &bbox) const;

	//! Check the global bounding volume with an infinite plane.
	//! The outside is pointed by the normal direction of the plane.
	//! @return Geometry::CLIP_INSIDE if the plane clip back with the object bounding,
	//!         Geometry::CLIP_INTERSECT if the plane clip front and back,
	//!         Geometry::CLIP_OUTSIDE otherwise.
	virtual Geometry::Clipping checkBounding(const Plane &plane) const;

	//! Check the global bounding volume with the frustum.
	//! @return Geometry::CLIP_INSIDE if the plane clip back with the object bounding,
	//!         Geometry::CLIP_INTERSECT if the plane clip front and back,
	//!         Geometry::CLIP_OUTSIDE otherwise.
	virtual Geometry::Clipping checkFrustum(const Frustum &frustum) const;

	//! Get the drawing type.
	virtual UInt32 getDrawType() const;

	//-----------------------------------------------------------------------------------
	// Rendering
	//-----------------------------------------------------------------------------------

	virtual void processAllFaces(Shadable::ProcessingPass pass);

	//-----------------------------------------------------------------------------------
	// Shadable
	//-----------------------------------------------------------------------------------

	//! Access to a currently active vertex element.
	//! @param type The array type to retrieve.
    //! @return The vertex element or null.
	virtual VertexElement* getVertexElement(VertexAttributeArray type) const;

	//! Access to the currently active face array.
    //! @return The face array or null.
	virtual FaceArray* getFaceArray() const;

	//! Bind the array in a specified attribute array location.
	//! @note The destination array must be enabled before to bind.
	virtual void attribute(VertexAttributeArray mode, UInt32 location);

	//! Make an operation.
	virtual void operation(Operations what);

	//! Is an operation is already done. For example ask if the tangent space is defined,
	//! or is the geometry data is create.
	virtual Bool isOperation(Operations what) const;

	//! Get the matrix array for hardware skinning/rigging.
    //! @return null.
	virtual const Float* getMatrixArray() const;

	//! Get the minimal square distance of the object bounding volume from the given point.
	virtual Float getDistanceFrom(const Vector3 &point) const;

    //-----------------------------------------------------------------------------------
	// Shadowable
	//-----------------------------------------------------------------------------------

	//! Project the silhouette according to a specified light.
	virtual void projectSilhouette(const DrawInfo &drawInfo);

    //-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

protected:

	AABBoxExt m_localBoundingBox;       //!< Axis aligned bounding box in local space.

	AABBoxExt m_boundingBox;            //!< Axis aligned bounding box in world space.
	BSphere   m_boundingSphere;         //!< Bounding sphere in world space.

	SmartObject<MeshData> m_meshData;   //!< Used MeshData.
	UInt32 m_faceArrayId;           //!< Current processed face array identifier.

	Bool m_boundingDirty;           //!< TRUE if the bounding need to be updated.

	std::vector<MaterialProfile*> m_matProfiles;  //! One material profile per sub-mesh.

	//! Update the global bounding volume.
	virtual void updateBounding();

	//! Draw the symbolics objects.
	void drawSymbolics();
};

} // namespace o3d

#endif // _O3D_MESH_H
