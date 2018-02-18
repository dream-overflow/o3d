/**
 * @file physicmodel.h
 * @brief A physic model is used for collision detection.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-05-30
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PHYSICMODEL_H
#define _O3D_PHYSICMODEL_H

#include "o3d/core/memorydbg.h"
#include "o3d/core/baseobject.h"

//#include "o3d/engine/object/mesh.h"

#include <list>
#include <vector>

#include "o3d/geom/aabbox.h"
#include "o3d/geom/bsphere.h"
#include "o3d/geom/plane.h"

namespace o3d {

typedef std::vector<AABBox> T_AABBoxVector;
typedef T_AABBoxVector::iterator IT_AABBoxVector;

typedef std::vector<BSphere> T_BSphereVector;
typedef T_BSphereVector::iterator IT_BSphereVector;

typedef std::vector<Plane> T_PlaneVector;
typedef T_PlaneVector::iterator IT_PlaneVector;

/**
 * @brief A physic model is used for collision detection
 */
class O3D_API PhysicModel : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(PhysicModel)

	//! constructor
	PhysicModel(
		BaseObject *pParent,
		Bool approximativeModel = True);

	//! destructor
	virtual ~PhysicModel();

	//! add a bounding box
	inline void addBBox(const AABBox& bbox) { m_AABBox.push_back(bbox); }
	inline void addBBox(Vector3 Center,Vector3 HalfSize) { m_AABBox.push_back(AABBox(Center,HalfSize)); }

	//! add a bounding box
	inline void addBSphere(const BSphere& bsphere) { m_BSphere.push_back(bsphere); }
	inline void addBSphere(Vector3 Center,Float Radius) { m_BSphere.push_back(BSphere(Center,Radius)); }

	//! add a bounding box
	inline void addPlane(const Plane& plane) { m_Plane.push_back(plane); }
	inline void addPlane(const Vector3 &Normal,Float Distance) { m_Plane.push_back(Plane(Normal,Distance)); }
	inline void addPlane(Float nx,Float ny,Float nz,Float dist) { m_Plane.push_back(Plane(nx,ny,nz,dist)); }

	//! get nbr of bbox/bpshere/bcylinder/plane
	inline UInt32 getNumBBox()const { return (UInt32)m_AABBox.size(); }
	inline UInt32 getNumBSphere()const { return (UInt32)m_BSphere.size(); }
	inline UInt32 getNumPlane()const { return (UInt32)m_Plane.size(); }

	//! get a bbox/bpshere/bcylinder/plane
	inline const AABBox& getBBox(UInt32 id)const { return m_AABBox[id]; }
	inline const BSphere& getBSphere(UInt32 id)const { return m_BSphere[id]; }
	inline const Plane& getPlane(UInt32 id)const { return m_Plane[id]; }

	//! serialisation
    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

protected:

//	SmartObject<Mesh> m_pMesh;    //!< the mesh collider model

    Bool m_isApprox;  //!< is the model use of bounding volume and not polygon detection

	T_AABBoxVector    m_AABBox;   //!< vector that contain all bounding box of the physic model for collision
	T_BSphereVector   m_BSphere;  //!< vector that contain all bounding sphere of the physic model for collision
	T_PlaneVector     m_Plane;    //!< vector that contain all plane of the physic model for collision
};

typedef std::list<PhysicModel*> T_PhysicModelList;
typedef T_PhysicModelList::iterator IT_PhysicModelList;
typedef T_PhysicModelList::const_iterator CIT_PhysicModelList;

} // namespace o3d

#endif // _O3D_PHYSICMODEL_H
