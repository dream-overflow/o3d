/**
 * @file cloth.h
 * @brief A cloth skin or rigid object used by Humanoid.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-06-30
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CLOTH_H
#define _O3D_CLOTH_H

#include "o3d/engine/object/skin.h"
#include "o3d/core/memorydbg.h"
#include <list>

namespace o3d {

typedef std::list<Vector3> T_Vector3List;
typedef T_Vector3List::iterator IT_Vector3List;
typedef T_Vector3List::const_iterator CIT_Vector3List;

//---------------------------------------------------------------------------------------
//! @class Cloth
//-------------------------------------------------------------------------------------
//! A cloth skin or rigid object used by Humanoid
//---------------------------------------------------------------------------------------
class O3D_API Cloth : public SceneEntity
{
public:

	O3D_DECLARE_CLASS(Cloth)

	//! Default constructor
	Cloth(BaseObject *pParent);

	//! Destructor
	virtual ~Cloth();

	//! set/get the tag name
	inline void setTagName(const String& name) { m_tagName = name; }
	inline const String& getTagName()const { return m_tagName; }
	inline String& getTagName() { return m_tagName; }

	//! create the cloth with a model and the humanoid whose use it
	Bool setClothModel(Humanoid* pHumanoid,const ClothModel& model);

	//! update the cloth
	inline void update() { O3D_ASSERT(m_object); m_object->update(); }

	//! draw the cloth
	inline void draw(const DrawInfo &drawInfo)
	{
		O3D_ASSERT(m_object);
		m_object->draw(drawInfo);
	}

protected:

	String m_tagName;      //!< cloth supported only by humanoid that have the same tag
	Bones* m_locateBone;   //!< the locate bone for put the cloth on the humanoid

	SceneObject* m_object; //!< used object for draw (O3DMesh,O3DSkinning,O3DRigging)
};

typedef std::list<Cloth*> T_ClothList;
typedef T_ClothList::iterator IT_ClothList;
typedef T_ClothList::const_iterator CIT_ClothList;

} // namespace o3d

#endif // _O3D_CLOTH_H

