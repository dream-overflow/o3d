/**
 * @file humanoid.h
 * @brief An humanoid character.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2006-06-28
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_HUMANOID_H
#define _O3D_HUMANOID_H

#include "o3d/core/memorydbg.h"
#include "o3d/core/templatearray.h"
#include "o3d/engine/object/skin.h"
#include "o3d/engine/object/cloth.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Humanoid
//-------------------------------------------------------------------------------------
//! An humanoid is a vertebrate animal, an human like. It is a skinning object and it can
//! wear some special object - cloth - or other skin (like armor, robe...) for games
//! @todo redone as O3DCharacter
//---------------------------------------------------------------------------------------
class O3D_API Humanoid : public Skinning
{
public:

	O3D_DECLARE_CLASS(Humanoid)

	enum HumanoidParts          //!< diff visibles parts of the humanoid (submat)
	{
		HEAD,
		BODY_UP,
		BODY_DOWN,
		NUM_PARTS
	};

	//! Default constructor (create a humanoid with O3DHumanoidModel)
	Humanoid(BaseObject *pParent);

	//! destructor
	virtual ~Humanoid();

	//! you must call it after a SetMaterial
	void updateShaderList();

	//! set/get the tag name
	inline void setTagName(const String& name) { m_TagName = name; }
	inline const String& getTagName()const { return m_TagName; }
	inline String& getTagName() { return m_TagName; }

	//! hide a part
	inline void hidePart(HumanoidParts part) { m_Hidden[part].useIt(); }
	//! unhide a part
	inline void unHidePart(HumanoidParts part) { m_Hidden[part].releaseIt(); }

	//! add a cloth on the humanoid
	Bool useCloth(const ClothModel& pCloth);

	//! remove a cloth from the humanoid (delete its Cloth)
	inline void removeCloth(const String& clothname)
	{
		for (IT_ClothList it = m_ClothList.begin() ; it != m_ClothList.end() ; ++it)
			if ((*it)->getName() == clothname)
			{
				deletePtr(*it);
				m_ClothList.erase(it);
			}
	}

	//! get the cloth list
	inline const T_ClothList& getClothList()const { return m_ClothList; }
	inline       T_ClothList& getClothList()      { return m_ClothList; }

	// update the humanoid
	virtual void update();

	// draw the humanoid
	virtual void draw(const DrawInfo &drawInfo);

protected:

	T_ClothList m_ClothList;  //!< used cloth
	String m_TagName;         //!< cloth supported only by humanoid that have the same tag

	Vector3* m_pBonesLimitP;  //!< positive limits of bones
	Vector3* m_pBonesLimitN;  //!< negative limits of bones

	SmartCounter<BaseObject*> m_Hidden[NUM_PARTS];  //!< states of parts (hidden if != 0)
//	MaterialPass* m_bodyParts[NUM_PARTS];      //!< initial parts materials
};

} // namespace o3d

#endif // _O3D_HUMANOID_H

