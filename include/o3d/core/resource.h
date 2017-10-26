/**
 * @file resource.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_RESOURCE_H
#define _O3D_RESOURCE_H

#include "baseobject.h"
#include "memorydbg.h"

namespace o3d {

/**
 * @brief Registrable resource type. (@note unused)
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-11-23
 * A resource must have a resource type. A resource type should be previously registered
 * one time to the resource manager.
 */
class O3D_API ResourceType
{
public:

	//! Default constructor
	ResourceType(const String &typeName, Int32 identifier) :
		m_typeName(typeName),
		m_id(identifier)
	{}

	//! Get the resource typename.
	inline const String& getName() const {return m_typeName; }

	//! Get the resource type identifier.
	inline Int32 getId() const { return m_id; }

private:

	String m_typeName;
	Int32 m_id;
};

class ResourceManager;


/**
 * @brief A resource is an entity that associate an object to a file.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-11-23
 * A resource is an entity that associate an object to a file. The resource can be
 * anywhere (diskfile, memfile, database, in a manager, real or virtual...).
 * It is based on BaseObject class.
 */
class O3D_API Resource : public BaseObject
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(Resource)

	//! Default constructor.
	//! @param parent Parent object.
	Resource(BaseObject *parent) :
		BaseObject(parent),
        m_manager(nullptr)
	{
	}

	//! Virtual destructor.
	virtual ~Resource() {}

	//! Set the resource name. (only for the manager).
	inline void setResourceName(const String &resourceName) { m_resourceName = resourceName; }

	//! Get the resource name.
	inline const String& getResourceName() const { return m_resourceName; }

	//! Set the manager.
	inline void setManager(ResourceManager *manager) { m_manager = manager; }

	//! Get the manager (read only).
	inline const ResourceManager* getManager() const { return m_manager; }

	//! Get the manager.
	inline ResourceManager* getManager() { return m_manager; }

protected:

	ResourceManager *m_manager;   //!< A manager that inherit from resource manager.

	String m_resourceName;        //!< Resource name (url, filename...).
};

} // namespace o3d

#endif // _O3D_RESOURCE_H

