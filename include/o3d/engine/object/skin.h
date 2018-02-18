/**
 * @file skin.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SKIN_H
#define _O3D_SKIN_H

#include "o3d/core/templatearray.h"
#include "o3d/core/matharray.h"
#include "mesh.h"
#include "skeleton.h"
#include "clothmodel.h"
#include "../scene/scenetemplatemanager.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class Cloth;
class Humanoid;

/**
 * @brief Base class for Rigging and Skinning.
 * @date 2003-08-12
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * A skin based object does not have a relative neither an absolute matrix. Its space
 * depend of its parent (generally a skeleton root). This mean if you want to move in
 * the world a skin object, you should move its parent bones, and same to query its
 * world position/rotation (transform).
 */
class O3D_API Skin : public Mesh
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(Skin)

	enum SkinType
	{
		UNDEFINED_TYPE,
		RIGGING_TYPE,
		SKINNING_TYPE
	};

    //! Max size of the matrices array. It limits the number of bones per skin.
    static const UInt32 MAX_SKINNING_MATRIX_ARRAY = O3D_MAX_SKINNING_MATRIX_ARRAY;

	//! Default constructor
	Skin(BaseObject *pParent);
	//! Copy constructor
	Skin(const Skin& dup);
    //! Copy constructor from a Mesh
	Skin(const Mesh& dup);

	//! Destructor
	virtual ~Skin();

	//! Duplicate
	Skin& operator=(const Skin& dup);

	//! set the meshdata
    virtual void setMeshData(MeshData *meshData) override;

	//! precompute matrix (automatically called when draw if not called before)
	void initialize();

	//! Is a skinning or a rigging technique
	inline SkinType getTechnique() const { return m_skinType; }

	//! Get the drawing type
    virtual UInt32 getDrawType() const override;

	//-----------------------------------------------------------------------------------
	// Processing
    //-----------------------------------------------------------------------------------

	//! Setup the modelview matrix.
    virtual void setUpModelView() override;

	//! Draw the skin, according to the skinning mode.
    virtual void draw(const DrawInfo &drawInfo) override;

    virtual void processAllFaces(Shadable::ProcessingPass pass) override;

	//! Update the skin bounding box.
    virtual void update() override;

	//-----------------------------------------------------------------------------------
    // skinning and skeleton
    //-----------------------------------------------------------------------------------

    //! Get the skeleton.
    Skeleton* getSkeleton() { return m_skeleton.get(); }

    //! Get the skeleton (const version).
    const Skeleton* getSkeleton() const { return m_skeleton.get(); }

	//! Enable or disable the auto-regeneration of the bounding box.
    //! @note It does not affect the original bounding box contained by the geometry.
	//! This task is done by the CPU, and it not generally useful. Prefer use a bounding
	//! volume that take the maximal size for the total animation range. But in some case
	//! that can be an optimization to enable it. So try it. Default is false.
	inline void setBoundingAutoRegen(Bool status) { m_boundingAutoRegen = status; }

	//! Get the status of the bounding auto regenerated (@see SetBoundingAutoRegen).
	inline Bool isBoundingAutoRegen() const { return m_boundingAutoRegen; }

	//! Define the number of bones
	virtual void setNumBones(UInt32 numBones);

	//! Get the number of bones
	inline UInt32 getNumBones() const { return m_numBones; }

	//! Set a bones at a specified index
	inline void setBone(UInt32 i, Bones* bones)
	{
        if (i < m_numBones) {
			m_bones[i] = bones;
			m_isPrecomputed = False;
		}
	}

	//! Get a bones at a specified index (read only)
	inline const Bones* getBone(UInt32 i)const
	{
        if (i < m_numBones) {
			return m_bones[i].get();
        } else {
            return nullptr;
        }
	}

	//! Get a bones at a specified index
	inline Bones* getBone(UInt32 i)
	{
        if (i < m_numBones) {
			return m_bones[i].get();
        } else {
            return nullptr;
        }
	}

	//! Search and return a bone by its name
	Bones* searchBone(const String &name);

	//! Set the reference mesh matrix.
	inline void setMeshRefMatrix(const Matrix4 &meshRefMatrix) { m_meshRefMatrix = meshRefMatrix; }
	//! Get the reference mesh matrix.
	inline const Matrix4& getMeshRefMatrix() const { return m_meshRefMatrix; }

	//! Set a reference matrix by it's bone id.
	inline void setRefMatrix(UInt32 i, const Matrix4 &refMatrix)
	{
        if (i < m_numBones) {
			m_refMatrices[i] = refMatrix;
        }
	}

	//! Get a reference matrix given it's bone id.
	inline const Matrix4& getRefMatrix(UInt32 i) const { return m_refMatrices[i]; }

	//! Enable the skinning activity.
    Bool enableSkinning();

	//! Disable the skinning activity.
    Bool disableSkinning();

	//! Toggle the skinning activity.
	inline Bool toggleSkinning()
	{
        if (m_isSkinning) {
			return disableSkinning();
        } else {
            return enableSkinning();
        }
	}

	//! Get the skinning activity.
	inline Bool isSkinning()const { return m_isSkinning; }

    //! Attach Humanoid skeleton to a skin bones list (use name-table for refers it).
	//! @note Work only with Cloth type imported skin.
    void attachToHumanoidSkeleton(class Humanoid *humanoid);

	//! Use software (CPU) mode.
	void setCPUMode();

	//! Use hardware (GPU) mode.
	void setGPUMode();

	//! Is GPU mode.
	inline Bool isGPUMode() const { return m_useHardware; }

    //-----------------------------------------------------------------------------------
	// Material access methods
	//-----------------------------------------------------------------------------------

	//! Get the matrix array for hardware skinning/rigging.
    virtual const Float* getMatrixArray() const override;

	//! Access to a currently active vertex element.
	//! @param type The array type to retrieve.
    //! @return The vertex element or null.
    virtual VertexElement* getVertexElement(VertexAttributeArray type) const override;

	// Serialization
    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

	//! post import pass, called after all objects are imported
    virtual void postImportPass() override;

protected:

	SkinType m_skinType;         //!< skin technique mode

	VertexBlend *m_vertexBlend;  //!< vertex blender

	Bool m_recompute;           //!< need to recompute the skinning (for software and bbox)
	Bool m_boundingAutoRegen;   //!< auto-regen the bounding box if true

	Bool m_isSkinning;          //!< skinning activity
	UInt32 m_numBones;          //!< bones num

    SmartObject<Bones> *m_bones;   //!< bones list
    AutoPtr<Skeleton> m_skeleton;  //!< skeleton object

    Matrix4 m_meshRefMatrix;    //!< mesh (object) reference matrix

	Matrix4Array m_refMatrices;             //!< bones references matrix
	Matrix4Array m_precomputedRefMatrices;  //!< pre-computation matrix used for bone
	Matrix4Array m_skinMatrices;            //!< skinning matrix list for each used bones

	String *m_boneImportName;  //!< import using bones ref name
    Int32 *m_boneImportId;     //!< import using bones ref id

    Bool m_isPrecomputed;      //!< preComputeRefMatrices() have been called

    Bool m_useHardware;        //!< use hardware rigging

	//! Compute all PrecomputedMatrix
	inline void preComputeRefMatrices()
	{
        for (UInt32 i = 0 ; i < m_numBones ; ++i) {
			m_precomputedRefMatrices[i] = m_meshRefMatrix * m_refMatrices[i].invert();
        }

		m_isPrecomputed = True;
	}

	//! Prepare all data before to be drawn if software skinning.
	virtual void prepareDrawing() = 0;

	//! Update the global bounding volume.
    virtual void updateBounding() override;
};

/**
 * @brief A skinning use of weights on vertices. A bone have a weight (0..1) on a vertex.
 * This is more efficient than the Rigging but need more time to compute.
 */
class O3D_API Skinning : public Skin
{
public:

	O3D_DECLARE_CLASS(Skinning)

	//! Default constructor
	Skinning(BaseObject *pParent);
	//! Copy constructor
	Skinning(const Skinning& dup);
	//! Copy constructor from an O3DMesh
	Skinning(const Mesh& dup);

	//! Copy
	Skinning& operator=(const Skinning& dup);

	// Serialization
    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

protected:

    virtual void prepareDrawing() override;
};

/**
 * @brief A rigging is more simple than skinning. There is no weights, but only one bone
 * attribution for each vertex.
 */
class O3D_API Rigging : public Skin
{
public:

	O3D_DECLARE_CLASS(Rigging)

	//! Default constructor
	Rigging(BaseObject *pParent);
	//! Copy constructor
	Rigging(const Rigging& dup);
	//! Duplicate from an O3DMesh
	Rigging(const Mesh& dup);

	//! Copy
	Rigging& operator=(const Rigging& dup);

	// Serialization
    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

protected:

    virtual void prepareDrawing() override;
};

typedef std::map<String,class ClothModel*> T_ClothModelMap;
typedef T_ClothModelMap::iterator IT_ClothModelMap;
typedef T_ClothModelMap::const_iterator CIT_ClothModelMap;

/**
 * @brief Manager for Cloth objects
 * @deprecated
 */
class O3D_API ClothManager : public SceneTemplateManager<ClothModel>
{
public:

	O3D_DECLARE_CLASS(ClothManager)

	ClothManager(BaseObject *pParent) :
		SceneTemplateManager<ClothModel>(pParent)
	{
		m_refName = False;
	}

	//! add a cloth to the manager (an XML file that contain def for cloths (insert and not delete current models)
	Bool importClothModelDef(const String& filename);
	//! export the cloths models defintion in an XML file
	Bool exportClothModelDef(const String& filename);
	//! clear all cloth defintion
	void clearClothModelDef();

	//! export skin bones ref by their name (dont search bone, only save name, useful for unlinked skin/skeleton)
	inline void exportRefBoneName() { m_refName = True;  }
	//! export skin bones ref by their export id (retreive automaticaly all bones)
	inline void exportRefBoneId()   { m_refName = False; }

	//! is export ref bone by name
	inline Bool isExportRefBoneName()const { return m_refName; }

	//! add a humanoid to the manager
	//Int32 addHumanoid(Humanoid

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	Bool exportClothModels(const String& path);

	//! Amport a file that contain a list of cloth model file (XML format)
    Int32 importClothModelList(InStream& is);

protected:

	T_ClothModelMap m_ClothModelMap; //!< a map that contain loaded cloths models
	Bool m_refName;              //!< skin export mode (default false, export with io ID)
};

} // namespace o3d

#endif // _O3D_SKIN_H
