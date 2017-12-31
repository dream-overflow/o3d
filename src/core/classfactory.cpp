/**
 * @file classfactory.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/classfactory.h"

#include "o3d/core/baseobject.h"
#include "o3d/core/debug.h"
#include "o3d/core/memory.h"

using namespace o3d;

//ClassFactory::T_ClassFactoryMap ClassFactory::m_Map;
ClassFactory::T_ClassInfoMap *ClassFactory::m_mapInfo = nullptr;
ClassFactory::T_IdClassInfoMap *ClassFactory::m_idMapInfo = nullptr;

// instanciation
/*Serialize* ClassFactory::getInstanceOfSerialize(const String &className)
{
	T_ClassFactoryMap::iterator it = m_Map.find(className);
	if (it == m_Map.end())
	{
		O3D_ERROR(E_InvalidClassName(className));
	}

	return (*it).second;
}*/

ClassInfo* ClassFactory::getInstanceOfClassInfo(const String &className)
{
    T_ClassInfoMap::iterator it = m_mapInfo->find(className);
    if (it == m_mapInfo->end())
	{
		O3D_ERROR(E_InvalidClassName(className));
	}

	return (*it).second;
}

// delete the map
void ClassFactory::destroy()
{
    deletePtr(m_mapInfo);
    deletePtr(m_idMapInfo);
}

// serialisation
/*Bool ClassFactory::writeToFile(OutStream& os, Serialize &object)
{
	// export class name
	String className = object.getClassName();
	file << className;

	// export object
	return object.writeToFile(file);
}

Serialize* ClassFactory::readFromFile(InStream& is)
{
	// import class name
	String className;
	file >> className;

	// create a class instance
	Serialize* object = getInstanceOfSerialize(className)->makeInstance();

	// and read the object
	if (object->readFromFile(file))
		return object;
	else
	{
		deletePtr(object);
        return nullptr;
	}
}*/

//! Write an object to a file for a further class factory read
Bool ClassFactory::writeToFile(OutStream& os, BaseObject &object)
{
	// export class name
	String className = object.getClassInfo()->getClassName();
    os << className;

	// export object
    return object.writeToFile(os);
}

BaseObject* ClassFactory::readFromFile(InStream& is, BaseObject *parent)
{
	// import class name
	String className;
    is >> className;

	// create a class instance
	BaseObject *object = getInstanceOfClassInfo(className)->createInstance(parent);

	// and read the object
    if (object->readFromFile(is))
		return object;
	else
	{
		deletePtr(object);
        return nullptr;
	}
}

// register classes
/*void ClassFactory::registerSerialize(const String& className, Serialize *obj)
{
	m_Map[className] = obj;
}

void ClassFactory::unregisterSerialize(const String& className)
{
	T_ClassFactoryMap::iterator it = m_Map.find(className);
	if (it != m_Map.end())
		m_Map.erase(it);
}*/

void ClassFactory::registerClassInfo(const String &className, ClassInfo *obj)
{
    if (!m_mapInfo)
        m_mapInfo = new T_ClassInfoMap;

    if (!m_idMapInfo)
        m_idMapInfo = new T_IdClassInfoMap;

    if (m_mapInfo->find(obj->getClassName()) != m_mapInfo->end())
		O3D_ERROR(E_InvalidOperation(String("Class name is already registred: ") + obj->getClassName()));

    if (m_idMapInfo->find(obj->getClassType()) != m_idMapInfo->end())
		O3D_ERROR(E_InvalidOperation(String("Class type is already registred: ") << obj->getClassType()));

    m_idMapInfo->insert(std::pair<UInt32, ClassInfo*>(obj->getClassType(), obj));
    m_mapInfo->insert(std::pair<String, ClassInfo*>(className, obj));
}

void ClassFactory::unRegisterClassInfo(const String& className)
{
    if (!m_mapInfo)
        return;

    if (!m_idMapInfo)
        return;

    T_ClassInfoMap::iterator it = m_mapInfo->find(className);
	ClassInfo *obj = nullptr;

    if (it != m_mapInfo->end())
	{
		obj = it->second;
        m_mapInfo->erase(it);
	}

	if (obj != nullptr)
	{
        T_IdClassInfoMap::iterator it2 = m_idMapInfo->find(obj->getClassType());
        if (it2 != m_idMapInfo->end())
            m_idMapInfo->erase(it2);

		deletePtr(obj);
	}
}

