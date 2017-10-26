/**
 * @file clothmodel.h
 * @brief A model for define and create a cloth object.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2006-06-30
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CLOTHMODEL_H
#define _O3D_CLOTHMODEL_H

#include "o3d/core/memorydbg.h"
#include "o3d/core/templatearray.h"
#include "o3d/core/stringlist.h"
#include "o3d/core/xmldoc.h"
#include <list>

namespace o3d {

typedef std::list<Vector3> T_Vector3List;
typedef T_Vector3List::iterator IT_Vector3List;
typedef T_Vector3List::const_iterator CIT_Vector3List;

//---------------------------------------------------------------------------------------
//! @class ClothModel
//-------------------------------------------------------------------------------------
//! A model for define and create a cloth object
//---------------------------------------------------------------------------------------
class O3D_API ClothModel : public BaseObject
{
public:

	ClothModel();

	String m_TagName;         //!< tag name for limit use of this cloth on certain humanoid only

	T_StringList m_BoneList;  //!< name list of used bone for this model
	String m_LocateBone;      //!< the bone name where locate the cloth

	UInt32 m_ObjectType;  //!< the type of the cloth objet (mesh,skinning,rigging) or other...

	ArrayUInt32 m_HideList;   //!< id list of masked submesh of the humanoid

	T_Vector3List m_RotPLimitList; //!< list for each bone of rotations limits (pos limit)
	T_Vector3List m_RotNLimitList; //!< list for each bone of rotations limits (neg limit)

	String m_objName;         //!< name of the used objet
	String m_materialName;    //!< name of the used material if specific one otherwise ""

	// XML serialisation
	Bool writeToFile(TiXmlElement& node);
	Bool readFromFile(TiXmlNode* node);
};

} // namespace o3d

#endif // _O3D_CLOTHMODEL_H

