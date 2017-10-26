/**
 * @file ms3dsettings.h
 * @brief Milkshape 3D file loader
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-06-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MS3DSETTINGS_H
#define _O3D_MS3DSETTINGS_H

#include "o3d/engine/texture/texture.h"
#include "o3d/engine/object/geometrydata.h"
#include "o3d/engine/vertexbuffer.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class Node;
class Animation;
class AnimationPlayer;
class SceneObject;
class Mesh;

//---------------------------------------------------------------------------------------
//! @class Ms3dResult
//-------------------------------------------------------------------------------------
//! Milkshape 3D file loader import result.
//---------------------------------------------------------------------------------------
class O3D_API Ms3dResult
{
public:

	friend class Ms3d;

	//! Default constructor
	Ms3dResult() :
        m_meshOrSkin(nullptr),
        m_rootNode(nullptr),
        m_animation(nullptr),
        m_animationPlayer(nullptr),
		m_numBones(0),
		m_isRigging(False),
		m_numFaces(0),
		m_numVertices(0)
	{}

	//! Return the mesh or skin object.
	//! @note Test the type using IsRigging() or o3d::typeOf(object,O3DRigging);
	inline Mesh* getMesh() const { return m_meshOrSkin; }

	//! Get the root node.
	inline Node* getRootNode() const { return m_rootNode; }

	//! Get the number of bones (0 mean no skeleton)
	inline UInt32 getNumBones() const { return m_numBones; }

	//! Is the meshOrSkin is a O3DRigging, else O3DMesh
	inline Bool isRigging() const { return m_isRigging; }

	//! Number of imported faces
	inline UInt32 getNumFaces() const { return m_numFaces; }

	//! Number of imported vertices
	inline UInt32 getNumVertices() const { return m_numVertices; }

	//! Get the animation (null if none)
	inline Animation* getAnimation() const { return m_animation; }

	//! Get the animation player (null if none)
	inline AnimationPlayer* getAnimationPlayer() const { return m_animationPlayer; }

protected:

	Mesh *m_meshOrSkin;         //!< Mesh or Rigging object
    Node *m_rootNode;           //!< A node containing the mesh/skin

	Animation *m_animation;
	AnimationPlayer *m_animationPlayer;

	UInt32 m_numBones;         //!< if skeleton, then the number of bones
	Bool m_isRigging;          //!< is the meshOrSkin is a O3DRigging object

	UInt32 m_numFaces;
	UInt32 m_numVertices;
};


//---------------------------------------------------------------------------------------
//! @class Ms3dSettings
//-------------------------------------------------------------------------------------
//! Milkshape 3D file loader import settings.
//---------------------------------------------------------------------------------------
class O3D_API Ms3dSettings
{
public:

	friend class Ms3d;

	//! Default constructor
	Ms3dSettings() :
		m_filtering(Texture::LINEAR_FILTERING),
		m_anisotropy(1.f),
		m_bmode(GeometryData::BOUNDING_AUTO),
        m_parentNode(nullptr),
        m_result(nullptr)
	{}

	//! Copy constructor
	Ms3dSettings(const Ms3dSettings &dup) :
		m_filtering(dup.m_filtering),
		m_anisotropy(dup.m_anisotropy),
		m_bmode(dup.m_bmode),
		m_parentNode(dup.m_parentNode),
		m_result(dup.m_result)
	{}

	//! Assign operator
	Ms3dSettings& operator= (const Ms3dSettings &dup)
	{
		m_filtering = dup.m_filtering;
		m_anisotropy = dup.m_anisotropy;
		m_bmode = dup.m_bmode;
		m_parentNode = dup.m_parentNode;
		m_result = dup.m_result;

		return *this;
	}

	//! Set/get the format of bounding volume generation
	inline void setBoundingVolumeGen(GeometryData::BoundingMode bmode) { m_bmode = bmode; }
	//! Get the format of bounding volume generation
	inline GeometryData::BoundingMode getBoundingVolumeGen() const { return m_bmode; }

	//! root parent node. NULL mean scene root.
	inline void setParentNode(Node* node) { m_parentNode = node; }

	//! Texture filtering mode for any imported textures
	inline void setTextureFiltering(Texture::FilteringMode filtering) { m_filtering = filtering; }

	//! Texture anisotropy level for any imported textures
	inline void setTextureAnisotropy(Float anisotropy) { m_anisotropy = anisotropy; }

	//! Set a result class or NULL if none. The delete of this object is at your responsibility.
	inline void setResultContainer(Ms3dResult *result) { m_result = result; }

	//! Get the result class or NULL if none. The delete of this object is at your responsibility.
	inline Ms3dResult* setResultContainer() const { return m_result; }

private:

	Texture::FilteringMode m_filtering;
	Float m_anisotropy;

	GeometryData::BoundingMode m_bmode;
	Node *m_parentNode;

	Ms3dResult *m_result;
};

} // namespace o3d

#endif // _O3D_MS3DSETTINGS_H

