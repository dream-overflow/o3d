/**
 * @file ms3d.h
 * @brief Milkshape 3D file loader
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2002-10-12
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MS3D_H
#define _O3D_MS3D_H

#include "../object/skin.h"
#include "ms3dsettings.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class Scene;

//---------------------------------------------------------------------------------------
//! @class Ms3d
//-------------------------------------------------------------------------------------
//! Milkshape 3D file loader
//---------------------------------------------------------------------------------------
class O3D_API Ms3d
{
public:

	//! Import an MS3D file to a scene (helper).
	static Bool import(
		Scene *scene,
        InStream &is,
        const String &path,
        const String &baseName,
		const Ms3dSettings &settings = Ms3dSettings());

	//! Import an MS3D file to a scene (helper).
	static Bool import(
		Scene *scene,
		const String &filename,
		const Ms3dSettings &settings = Ms3dSettings());

	//! Default constructor
	Ms3d(Scene *scene);

	//! Destructor
	virtual ~Ms3d();

	//! Define import settings
	inline void settings(const Ms3dSettings &settings) { m_settings = settings; }

	//! load an MS3D file
    Bool load(const String& filename);
	//! load an MS3D file
    Bool load(InStream& is, const String& baseName, const String &path);

	//! is a mesh or rigging
	inline Bool isMesh() const { return m_mesh; }
	inline Bool isSkin() const { return m_skin; }

	//! get the object id
	inline Int32 getObjectId() const { return m_objectId; }

	//! Unload the data added by the file.
	void destroy();

	//! Is successfully loaded.
	inline Bool isLoaded() const { return m_loaded; }

private:

	Scene *m_scene;
	Ms3dSettings m_settings;

    Int32 m_objectId;        //!< id of the main object in its manager
    Bool m_loaded;		     //!< is the file is loaded

    Node *m_rootNode;       //!< root node that contain imported scene

	Mesh    *m_mesh;        //!< the mesh
	Rigging *m_skin;        //!< or the skin
    UInt32 m_numGroups;     //!< number of meshes
};

} // namespace o3d

#endif // _O3D_MS3D_H

