/**
 * @file clothmodel.cpp
 * @brief A model for define and create a cloth object
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-06-30
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/cloth.h"

#include "o3d/engine/object/humanoid.h"
#include "o3d/core/xmldoc.h"

using namespace o3d;

//ENGINE_CLOTH_MODEL

ClothModel::ClothModel()
{
}

/*---------------------------------------------------------------------------------------
  XML serialisation
---------------------------------------------------------------------------------------*/
Bool ClothModel::writeToFile(TiXmlElement& node)
{
	node.SetAttribute("name", m_name.toUtf8().getData());
	node.SetAttribute("relativeBone", m_LocateBone.toUtf8().getData());

	// tag element
	if (m_TagName.length() > 0)
	{
		TiXmlElement tagName("Tag");
		tagName.InsertEndChild(TiXmlText(m_TagName.toUtf8().getData()));
		node.InsertEndChild(tagName);
	}

	// object element
	TiXmlElement object("Object");

	if (m_ObjectType == ENGINE_MESH)
		object.SetAttribute("type","Mesh");
	else if (m_ObjectType == ENGINE_RIGGING)
		object.SetAttribute("type","Skinning");
	else if (m_ObjectType == ENGINE_RIGGING)
		object.SetAttribute("type","Rigging");
	else
		object.SetAttribute("type","Undefined");

	object.InsertEndChild(TiXmlText(m_objName.toUtf8().getData()));
	node.InsertEndChild(object);

	// material element
	if (m_materialName.length() > 0)
	{
		TiXmlElement material("Material");
		material.InsertEndChild(TiXmlText(m_materialName.toUtf8().getData()));
		node.InsertEndChild(material);
	}

	// bones element
	if (m_BoneList.size())
	{
		TiXmlElement bones("Bones");

		IT_StringList strIt = m_BoneList.begin();

		IT_Vector3List plimIt = m_RotPLimitList.begin();
		IT_Vector3List nlimIt = m_RotNLimitList.begin();

		Bool insert;

		for (UInt32 i = 0 ; i < m_BoneList.size() ; ++i)
		{
			insert = False;

			TiXmlElement bone("Bone");
			bone.SetAttribute("name", (*strIt).toUtf8().getData());

			if ((*plimIt)[X] != o3d::Limits<Float>::max()) { bone.SetDoubleAttribute("limitXP",(*plimIt)[X]); insert = True; }
			if ((*plimIt)[Y] != o3d::Limits<Float>::max()) { bone.SetDoubleAttribute("limitYP",(*plimIt)[Y]); insert = True; }
			if ((*plimIt)[Z] != o3d::Limits<Float>::max()) { bone.SetDoubleAttribute("limitZP",(*plimIt)[Z]); insert = True; }

			if ((*nlimIt)[X] != o3d::Limits<Float>::min()) { bone.SetDoubleAttribute("limitXN",(*nlimIt)[X]); insert = True; }
			if ((*nlimIt)[Y] != o3d::Limits<Float>::min()) { bone.SetDoubleAttribute("limitYN",(*nlimIt)[Y]); insert = True; }
			if ((*nlimIt)[Z] != o3d::Limits<Float>::min()) { bone.SetDoubleAttribute("limitZN",(*nlimIt)[Z]); insert = True; }

			if (insert) bones.InsertEndChild(bone);

			strIt++;
			plimIt++;
			nlimIt++;
		}

		node.InsertEndChild(bones);
	}

	// hide element
	if (m_HideList.getNumElt() > 0)
	{
		TiXmlElement hides("Hides");

		for (Int32 i = 0 ; i < m_HideList.getNumElt() ; ++i)
		{
			TiXmlElement hide("Hide");

			if (m_HideList[i] == Humanoid::BODY_UP) hide.InsertEndChild(TiXmlText("body_up"));
			else if (m_HideList[i] == Humanoid::BODY_DOWN) hide.InsertEndChild(TiXmlText("body_down"));
			//else ... // TODO

			hides.InsertEndChild(hide);
		}

		node.InsertEndChild(hides);
	}

	return True;
}

Bool ClothModel::readFromFile(TiXmlNode* node)
{
	// read xml content
	// element TextureFont
	if (!node)
		return False;

	m_name = node->ToElement()->Attribute("name");
	m_LocateBone = node->ToElement()->Attribute("relativeBone");

	// tag element
	TiXmlElement* pElt = node->FirstChildElement("Tag");
	if (pElt) m_TagName = tinyString(pElt->FirstChild());

	// object element
	pElt = node->FirstChildElement("Object");
	if (pElt)
	{
		m_objName = tinyString(pElt->FirstChild());
		String str = pElt->Attribute("type");

		if (str == "Mesh") m_ObjectType = ENGINE_MESH;
		else if (str == "Skinning") m_ObjectType = ENGINE_SKINNING;
		else if (str == "Rigging") m_ObjectType = ENGINE_RIGGING;
		else m_ObjectType = O3D_UNDEFINED;
	}

	// shader element
	pElt = node->FirstChildElement("Material");
	if (pElt) m_materialName = tinyString(pElt->FirstChild());

	// bones element
	pElt = node->FirstChildElement("Bones");
	if (pElt)
	{
		TiXmlNode *pBoneNode = NULL;

		while ((pBoneNode = pElt->IterateChildren("Bone",pBoneNode)) != NULL)
		{
			Vector3 Plimit(o3d::Limits<Float>::max(), o3d::Limits<Float>::max(), o3d::Limits<Float>::max());
			Vector3 Nlimit(o3d::Limits<Float>::min(), o3d::Limits<Float>::min(), o3d::Limits<Float>::min());

			pBoneNode->ToElement()->QueryFloatAttribute("limitXP",&Plimit[X]);
			pBoneNode->ToElement()->QueryFloatAttribute("limitYP",&Plimit[Y]);
			pBoneNode->ToElement()->QueryFloatAttribute("limitZP",&Plimit[Z]);

			pBoneNode->ToElement()->QueryFloatAttribute("limitXN",&Nlimit[X]);
			pBoneNode->ToElement()->QueryFloatAttribute("limitYN",&Nlimit[Y]);
			pBoneNode->ToElement()->QueryFloatAttribute("limitZN",&Nlimit[Z]);

			if ((Plimit[X] != o3d::Limits<Float>::max()) ||
				(Plimit[Y] != o3d::Limits<Float>::max()) ||
				(Plimit[Z] != o3d::Limits<Float>::max()) ||
				(Nlimit[X] != o3d::Limits<Float>::min()) ||
				(Nlimit[Y] != o3d::Limits<Float>::min()) ||
				(Nlimit[Z] != o3d::Limits<Float>::min()))
			{
				m_RotPLimitList.push_back(Plimit);
				m_RotNLimitList.push_back(Nlimit);

				m_BoneList.push_back(String(pBoneNode->ToElement()->Attribute("name")));
			}
		}
	}

	// hide element
	pElt = node->FirstChildElement("Hides");
	if (pElt)
	{
		TiXmlNode *pMaskNode = NULL;
		String str;

		while ((pMaskNode = pElt->IterateChildren("Hide",pMaskNode)) != NULL)
		{
			str = tinyString(pMaskNode->ToElement()->FirstChild());

			if (str == "body_up") m_HideList.push(Humanoid::BODY_UP);
			else if (str == "body_down") m_HideList.push(Humanoid::BODY_DOWN);
			//else ... // TODO
		}
	}

	return True;
}

