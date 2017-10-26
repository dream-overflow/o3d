/**
 * @file humanoid.cpp
 * @brief An humanoid character.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2006-06-28
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/humanoid.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/material/material.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Humanoid, ENGINE_HUMANOID, Skinning)

/*---------------------------------------------------------------------------------------
  constructor
---------------------------------------------------------------------------------------*/
Humanoid::Humanoid(BaseObject *pParent) :
	Skinning(pParent),
	m_pBonesLimitP(NULL),
	m_pBonesLimitN(NULL)
{
	updateShaderList();
}

void Humanoid::updateShaderList()
{
	/*for (Int32 i = 0; i < NbrParts; ++i)
		m_bodyParts[i] = NULL;

	UInt32 numMaterials = getNumMaterials();
	for (UInt32 i = 0; i < numMaterials; ++i)
	{
		Material *material = getMaterial(i);

		if (material)
		{
			if (material->getName() == "bodyUp") m_bodyParts[BodyUp] = material;
			else if (material->getName() == "bodyDown") m_bodyParts[BodyDown] = material;
			//else if // TODO
		}
	}*/
}

/*---------------------------------------------------------------------------------------
  destructor
---------------------------------------------------------------------------------------*/
Humanoid::~Humanoid()
{
	for (IT_ClothList it = m_ClothList.begin(); it != m_ClothList.end(); ++it)
		deletePtr(*it);
}

/*---------------------------------------------------------------------------------------
  update the humanoid
---------------------------------------------------------------------------------------*/
void Humanoid::update()
{
	Skinning::update();

	// update each cloth
	for (IT_ClothList it = m_ClothList.begin(); it != m_ClothList.end(); ++it)
		(*it)->update();
}

/*---------------------------------------------------------------------------------------
  draw the humanoid
---------------------------------------------------------------------------------------*/
void Humanoid::draw(const DrawInfo &drawInfo)
{/*
	// set
	for (Int32 i = 0; i < NbrParts; ++i)
		if (m_Hidden[i].getReferenceCounter() > 0)
			m_bodyParts[i]->disable();

	Skinning::draw(drawInfo);

	// draw each cloth
	for (IT_ClothList it = m_ClothList.begin(); it != m_ClothList.end(); ++it)
		(*it)->draw(drawInfo);

	// reset
	for (Int32 i = 0; i < NbrParts; ++i)
		m_bodyParts[i]->enable();*/
}

// add a cloth on the humanoid
Bool Humanoid::useCloth(const ClothModel& pClothModel)
{
	for (IT_ClothList it = m_ClothList.begin(); it != m_ClothList.end(); ++it)
	{
		// already used
		if ((*it)->getName() == pClothModel.getName())
			return False;
	}

	Cloth* pCloth = new Cloth(getScene()->getClothManager());
	pCloth->setClothModel(this, pClothModel);

	return True;
}

