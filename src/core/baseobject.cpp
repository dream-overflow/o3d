/**
 * @file baseobject.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"
#include "o3d/core/baseobject.h"
#include "o3d/core/smartobject.h"
#include "o3d/core/debug.h"

#include "o3d/core/memorydbg.h"

#include <algorithm>

using namespace o3d;

O3D_IMPLEMENT_CLASS_COMMON(Sharable, CORE_SHARABLE, nullptr)
O3D_IMPLEMENT_ABSTRACT_CLASS2(BaseObject, CORE_BASE_OBJECT, EvtHandler, Sharable)

// Default contructor
BaseObject::BaseObject(BaseObject* parent) :
	EvtHandler(),
		m_id(-1),
		m_name("undefined"),
		m_serializeId(O3D_UNDEFINED),
        m_parent(nullptr),
        m_topLevelParent(nullptr)
{
	setParent(parent);
}

// Copy constructor
BaseObject::BaseObject(const BaseObject &dup) :
	EvtHandler(),
		m_id(-1),
		m_name(dup.m_name),
		m_serializeId(O3D_UNDEFINED),
		m_parent(dup.m_parent),
		m_topLevelParent(dup.m_topLevelParent)
{
	if (m_parent)
	{
		// Check for the top-level parent type
		if (!hasTopLevelParentTypeOf())
			O3D_ERROR(E_InvalidParameter("Parent has an invalid top-level parent type"));
	}
}

// Duplicator
BaseObject& BaseObject::operator= (const BaseObject &dup)
{
	//m_id = -1;
	m_name = dup.m_name;
	m_serializeId = O3D_UNDEFINED;
	//m_parent = dup.m_parent;
	m_usersList.clear();

	if (m_topLevelParent && (m_topLevelParent != dup.m_topLevelParent))
		O3D_ERROR(E_InvalidParameter("The top-level parent of the new parent is different of the old"));
	else
		m_topLevelParent = dup.m_topLevelParent;

	return *this;
}

// Destructor. Release all users.
BaseObject::~BaseObject()
{
	releaseAll();
}

// Check for a class type
Bool BaseObject::getTypeOf(ClassInfo *info) const
{
	ClassInfo *thisInfo = getClassInfo();
    return (thisInfo) ? thisInfo->getTypeOf(info) : False;
}

// Change the parent object
void BaseObject::setParent(BaseObject *parent)
{
	// is the same parent
	if (m_parent == parent)
		return;

	// un-parent if parent
	if (m_parent)
		m_parent->unparentIt(this);

	m_parent = parent;

	// re-parent if parent
	if (m_parent)
	{
		if (m_topLevelParent && (m_topLevelParent != parent->m_topLevelParent))
			O3D_ERROR(E_InvalidParameter("The top-level parent of the new parent is different of the old"));
		else
			m_topLevelParent = parent->m_topLevelParent;

		m_parent->parentIt(this);
	}
	else
        m_topLevelParent = nullptr;
}

// Delete the object from its parent
Bool BaseObject::deleteIt()
{
	O3D_ASSERT(m_usersList.empty());

	if (m_parent)
	{
		return m_parent->deleteChild(this);
	}
	else
	{
		// no parent so immediate delete
		deletePtr(const_cast<BaseObject*>(this));
        return True;
	}
}

// Delete a child object
Bool BaseObject::deleteChild(BaseObject *child)
{
	if (child)
	{
		if (child->getParent() != this)
			O3D_ERROR(E_InvalidParameter("The parent child differ from this"));
		else
		{
			deletePtr(child);
            return True;
		}
	}
    return False;
}

// Set this object as persistent. That mean this object is kept even if it has no user
void BaseObject::setPersistant(Bool persistant)
{
	if (persistant)
	{
		if (!isPersistant())
            m_usersList.push_front(nullptr);
	}
	else
	{
		if (isPersistant())
			m_usersList.pop_front();
	}
}

// An object become child of this.
void BaseObject::parentIt(BaseObject *child)
{
	// nothing by default
}

// An child change of parent.
void BaseObject::unparentIt(BaseObject *child)
{
	// nothing by default
}

// Register a user for this object.
void BaseObject::useIt(BaseSmartObject &smartObject)
{
	m_usersList.push_back(&smartObject);
}

// Unregister a previous user of this object.
Bool BaseObject::releaseIt(BaseSmartObject &smartObject)
{
	IT_UserList it = std::find(m_usersList.begin(), m_usersList.end(), &smartObject);

	if (it == m_usersList.end())
		O3D_ERROR(E_InvalidParameter("Unknown user"));
	else
		m_usersList.erase(it);

	if (m_usersList.empty())
	{
		onDeleteObject(this);
        return True;
	}

    return False;
}

// Detach all users.
void BaseObject::releaseAll()
{
	if (m_usersList.empty())
		return;

	IT_UserList it = m_usersList.begin();
	BaseSmartObject *object;

	// bypass the persistent user
	if (isPersistant())
		++it;

	// release each user
	for (; it != m_usersList.end(); ++it)
	{
		O3D_ASSERT(*it);

		object = *it;
        object->m_pUser->detachObject(reinterpret_cast<BaseObject*>(object->m_pData));
        object->m_pData = nullptr;
	}

	m_usersList.clear();
}

// An object is detached.
void BaseObject::detachObject(BaseObject *object)
{
	// nothing by default
}

// De-serialize the object from a file
Bool BaseObject::readFromFile(InStream &istream)
{
    UInt32 type;
    istream >> type;

	// compare the type
	if (type != this->getType())
        O3D_ERROR(E_InvalidFormat("Readed class type and instancied class type differs"));

    istream >> m_name
            >> m_serializeId;

    return True;
}

// Serialize the object to a file
Bool BaseObject::writeToFile(OutStream &ostream)
{
    ostream << getType()
            << m_name
            << m_serializeId;

    return True;
}
