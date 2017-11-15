/**
 * @file classfactory.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CLASSFACTORY_H
#define _O3D_CLASSFACTORY_H

#include "instream.h"
#include "outstream.h"
#include "memorydbg.h"

#include <map>

namespace o3d {

class BaseObject;
class ClassInfo;

/**
 * @brief The ClassFactory class. It create class instance according to a class name.
 * Cloned class must inherit from Serialize or BaseObject.
 * @date 2003-02-25
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class O3D_API ClassFactory
{
public:

	//! Get class instance for a given class name
	//! @exception E_InvalidClassName
	//static Serialize* getInstanceOfSerialize(const String &className);

	//! Get class instance for a given class name
	//! @exception E_InvalidClassName
	static ClassInfo* getInstanceOfClassInfo(const String &className);

	//! delete the map
	static void destroy();

	//! Write an object to a file for a further class factory read
    //static Bool writeToFile(OutStream& os, Serialize &object);

	//! Read an unknown object type from a file
    //static Serialize* readFromFile(InStream& is);

	//! Write an object to a file for a further class factory read
    static Bool writeToFile(OutStream& os, BaseObject &object);

	//! Read an unknown object type from a file
	//! @param parent The parent object. Can be null if none
    static BaseObject* readFromFile(InStream& is, BaseObject *parent);

	//! classes register
	//static void registerSerialize(const String &className, Serialize *obj);
	//static void unregisterSerialize(const String &className);

	static void registerClassInfo(const String &className, ClassInfo *obj);
	static void unRegisterClassInfo(const String &className);

private:

	//typedef std::map<String, Serialize*> T_ClassFactoryMap;
	typedef std::map<String, ClassInfo*> T_ClassInfoMap;
	typedef std::map<UInt32, ClassInfo*> T_IdClassInfoMap;

    //static T_ClassFactoryMap m_map;      //!< contains class instances
    static T_ClassInfoMap *m_mapInfo;      //!< contains class info instances
    static T_IdClassInfoMap *m_idMapInfo;
};

} // namespace o3d

//#define O3D_REGISTER_CLASS(class) ClassFactory::registerSerialize(String(#class),class::createInstance())

#endif // _O3D_CLASSFACTORY_H

