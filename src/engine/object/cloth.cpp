/**
 * @file cloth.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2006-06-30
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/cloth.h"

#include "o3d/engine/object/humanoid.h"
#include "o3d/engine/scene/sceneobjectmanager.h"
#include "o3d/core/xmldoc.h"

#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Cloth, ENGINE_CLOTH_OBJECT, BaseObject)

//! Default constructor
Cloth::Cloth(BaseObject *pParent) :
	SceneEntity(pParent),
	m_locateBone(NULL),
	m_object(NULL)
{
}

/*---------------------------------------------------------------------------------------
  destructor
---------------------------------------------------------------------------------------*/
Cloth::~Cloth() {}

/*---------------------------------------------------------------------------------------
  create the cloth with a model and the humanoid whose use it
---------------------------------------------------------------------------------------*/
Bool Cloth::setClothModel(Humanoid* pHumanoid,const ClothModel& model)
{
	// set the name and tagname
	m_name = model.getName();
	m_tagName = model.m_TagName;

	// find the locate bone
	if ((m_locateBone = pHumanoid->searchBone(model.m_LocateBone)) == NULL)
	{
		O3D_ERROR(E_InvalidParameter("LocateBone is null"));
		return False;
	}

	// find the object
	switch (model.m_ObjectType)
	{
	case ENGINE_MESH:
		if ((m_object = getScene()->getSceneObjectManager()->searchName(model.m_objName)) == NULL)
		{
			O3D_ERROR(E_InvalidParameter("Object is null"));
			return False;
		}
		break;
	case ENGINE_SKINNING:
		if ((m_object = getScene()->getSceneObjectManager()->searchName(model.m_objName)) == NULL)
		{
			O3D_ERROR(E_InvalidParameter("Object is null"));
			return False;
		}

		if (m_object->getType() != ENGINE_SKINNING)
		{
			O3D_ERROR(E_InvalidFormat("Invalid object type"));
			return False;
		}

		// set the cloth skeleton with the humanoid's one
		((Skin*)m_object)->attachToHumanoidSkeleton(pHumanoid);
		break;
	case ENGINE_RIGGING:
		if ((m_object = getScene()->getSceneObjectManager()->searchName(model.m_objName)) == NULL)
		{
			O3D_ERROR(E_InvalidParameter("Object is null"));
			return False;
		}

		if (m_object->getType() != ENGINE_RIGGING)
		{
			O3D_ERROR(E_InvalidFormat("Invalid object type"));
			return False;
		}

		// set the cloth skeleton with the humanoid's one
		((Skin*)m_object)->attachToHumanoidSkeleton(pHumanoid);
		break;
	}

	// copy the bone
	for (CIT_StringList it = model.m_BoneList.begin() ; it != model.m_BoneList.end() ; ++it)
	{
		// find the bones in the humanoid
		//Bones *pBone = ((Skin*)pHumanoid)->searchBone(*it);

		// TODO to find an optimized method
		//if (pBone)
		//{
		//	pBone
		//}
	}

	// hides
	for (Int32 i = 0 ; i < model.m_HideList.getNumElt() ; ++i)
		pHumanoid->hidePart((Humanoid::HumanoidParts)model.m_HideList[i]);

	return True;
}

