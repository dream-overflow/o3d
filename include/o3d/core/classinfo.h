/**
 * @file classinfo.h
 * @brief Class information object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-12-05
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CLASSINFO_H
#define _O3D_CLASSINFO_H

#include "string.h"

namespace o3d {

class BaseObject;

//! Object creation functor
typedef BaseObject *(*ObjectConstructorFn)(BaseObject *);

/**
 * @brief Class info for run time type checking.
 */
class O3D_API ClassInfo : NonCopyable<>
{
public:

	//! Default constructor
	ClassInfo(
		const Char *className,
		UInt32 classType,
		size_t size,
		ObjectConstructorFn ctor) :
			m_className(className),
			m_classType(classType),
			m_objectSize(size),
			m_objectConstructor(ctor),
			m_numParents(0),
			m_next(ms_first)
	{
		ms_first = this;
		registerClass();
	}

	//! Build with one parents classes
	ClassInfo(
		const Char *className,
		UInt32 classType,
		const ClassInfo *parent1,
		size_t size,
		ObjectConstructorFn ctor) :
			m_className(className),
			m_classType(classType),
			m_objectSize(size),
			m_objectConstructor(ctor),
			m_numParents(1),
			m_next(ms_first)
	{
		ms_first = this;
		registerClass();

		m_parents[0] = const_cast<ClassInfo*>(parent1);
	}

	//! Build with two parents classes
	ClassInfo(
		const Char *className,
		UInt32 classType,
		const ClassInfo *parent1,
		const ClassInfo *parent2,
		size_t size,
		ObjectConstructorFn ctor) :
			m_className(className),
			m_classType(classType),
			m_objectSize(size),
			m_objectConstructor(ctor),
			m_numParents(2),
			m_next(ms_first)
	{
		ms_first = this;
		registerClass();

		m_parents[0] = const_cast<ClassInfo*>(parent1);
		m_parents[1] = const_cast<ClassInfo*>(parent2);
	}

	//! Build with 3 parents classes
	ClassInfo(
		const Char *className,
		UInt32 classType,
		const ClassInfo *parent1,
		const ClassInfo *parent2,
		const ClassInfo *parent3,
		size_t size,
		ObjectConstructorFn ctor) :
			m_className(className),
			m_classType(classType),
			m_objectSize(size),
			m_objectConstructor(ctor),
			m_numParents(3),
			m_next(ms_first)
	{
		ms_first = this;
		registerClass();

		m_parents[0] = const_cast<ClassInfo*>(parent1);
		m_parents[1] = const_cast<ClassInfo*>(parent2);
		m_parents[2] = const_cast<ClassInfo*>(parent3);
	}

	//! Build with 4 parents classes
	ClassInfo(
		const Char *className,
		UInt32 classType,
		const ClassInfo *parent1,
		const ClassInfo *parent2,
		const ClassInfo *parent3,
		const ClassInfo *parent4,
		size_t size,
		ObjectConstructorFn ctor) :
			m_className(className),
			m_classType(classType),
			m_objectSize(size),
			m_objectConstructor(ctor),
			m_numParents(4),
			m_next(ms_first)
	{
		ms_first = this;
		registerClass();

		m_parents[0] = const_cast<ClassInfo*>(parent1);
		m_parents[1] = const_cast<ClassInfo*>(parent2);
		m_parents[2] = const_cast<ClassInfo*>(parent3);
		m_parents[3] = const_cast<ClassInfo*>(parent4);
	}

	//! Build with 5 parents classes
	ClassInfo(
		const Char *className,
		UInt32 classType,
		const ClassInfo *parent1,
		const ClassInfo *parent2,
		const ClassInfo *parent3,
		const ClassInfo *parent4,
		const ClassInfo *parent5,
		size_t size,
		ObjectConstructorFn ctor) :
			m_className(className),
			m_classType(classType),
			m_objectSize(size),
			m_objectConstructor(ctor),
			m_numParents(5),
			m_next(ms_first)
	{
		ms_first = this;
		registerClass();

		m_parents[0] = const_cast<ClassInfo*>(parent1);
		m_parents[1] = const_cast<ClassInfo*>(parent2);
		m_parents[2] = const_cast<ClassInfo*>(parent3);
		m_parents[3] = const_cast<ClassInfo*>(parent4);
		m_parents[4] = const_cast<ClassInfo*>(parent5);
	}

	//! Build with 6 parents classes
	ClassInfo(
		const Char *className,
		UInt32 classType,
		const ClassInfo *parent1,
		const ClassInfo *parent2,
		const ClassInfo *parent3,
		const ClassInfo *parent4,
		const ClassInfo *parent5,
		const ClassInfo *parent6,
		size_t size,
		ObjectConstructorFn ctor) :
			m_className(className),
			m_classType(classType),
			m_objectSize(size),
			m_objectConstructor(ctor),
			m_numParents(6),
			m_next(ms_first)
	{
		ms_first = this;
		registerClass();

		m_parents[0] = const_cast<ClassInfo*>(parent1);
		m_parents[1] = const_cast<ClassInfo*>(parent2);
		m_parents[2] = const_cast<ClassInfo*>(parent3);
		m_parents[3] = const_cast<ClassInfo*>(parent4);
		m_parents[4] = const_cast<ClassInfo*>(parent5);
		m_parents[5] = const_cast<ClassInfo*>(parent6);
	}

	//! Build with 7 parents classes
	ClassInfo(
		const Char *className,
		UInt32 classType,
		const ClassInfo *parent1,
		const ClassInfo *parent2,
		const ClassInfo *parent3,
		const ClassInfo *parent4,
		const ClassInfo *parent5,
		const ClassInfo *parent6,
		const ClassInfo *parent7,
		size_t size,
		ObjectConstructorFn ctor) :
			m_className(className),
			m_classType(classType),
			m_objectSize(size),
			m_objectConstructor(ctor),
			m_numParents(7),
			m_next(ms_first)
	{
		ms_first = this;
		registerClass();

		m_parents[0] = const_cast<ClassInfo*>(parent1);
		m_parents[1] = const_cast<ClassInfo*>(parent2);
		m_parents[2] = const_cast<ClassInfo*>(parent3);
		m_parents[3] = const_cast<ClassInfo*>(parent4);
		m_parents[4] = const_cast<ClassInfo*>(parent5);
		m_parents[5] = const_cast<ClassInfo*>(parent6);
		m_parents[6] = const_cast<ClassInfo*>(parent7);
	}

	//! Build with 8 parents classes
	ClassInfo(
		const Char *className,
		UInt32 classType,
		const ClassInfo *parent1,
		const ClassInfo *parent2,
		const ClassInfo *parent3,
		const ClassInfo *parent4,
		const ClassInfo *parent5,
		const ClassInfo *parent6,
		const ClassInfo *parent7,
		const ClassInfo *parent8,
		size_t size,
		ObjectConstructorFn ctor) :
			m_className(className),
			m_classType(classType),
			m_objectSize(size),
			m_objectConstructor(ctor),
			m_numParents(8),
			m_next(ms_first)
	{
		ms_first = this;
		registerClass();

		m_parents[0] = const_cast<ClassInfo*>(parent1);
		m_parents[1] = const_cast<ClassInfo*>(parent2);
		m_parents[2] = const_cast<ClassInfo*>(parent3);
		m_parents[3] = const_cast<ClassInfo*>(parent4);
		m_parents[4] = const_cast<ClassInfo*>(parent5);
		m_parents[5] = const_cast<ClassInfo*>(parent6);
		m_parents[6] = const_cast<ClassInfo*>(parent7);
		m_parents[7] = const_cast<ClassInfo*>(parent8);
	}

	//! Build with 9 parents classes
	ClassInfo(
		const Char *className,
		UInt32 classType,
		const ClassInfo *parent1,
		const ClassInfo *parent2,
		const ClassInfo *parent3,
		const ClassInfo *parent4,
		const ClassInfo *parent5,
		const ClassInfo *parent6,
		const ClassInfo *parent7,
		const ClassInfo *parent8,
		const ClassInfo *parent9,
		size_t size,
		ObjectConstructorFn ctor) :
			m_className(className),
			m_classType(classType),
			m_objectSize(size),
			m_objectConstructor(ctor),
			m_numParents(9),
			m_next(ms_first)
	{
		ms_first = this;
		registerClass();

		m_parents[0] = const_cast<ClassInfo*>(parent1);
		m_parents[1] = const_cast<ClassInfo*>(parent2);
		m_parents[2] = const_cast<ClassInfo*>(parent3);
		m_parents[3] = const_cast<ClassInfo*>(parent4);
		m_parents[4] = const_cast<ClassInfo*>(parent5);
		m_parents[5] = const_cast<ClassInfo*>(parent6);
		m_parents[6] = const_cast<ClassInfo*>(parent7);
		m_parents[7] = const_cast<ClassInfo*>(parent8);
		m_parents[8] = const_cast<ClassInfo*>(parent9);
	}

	//! Destructor
	~ClassInfo();

	//! Create an object of this type
	inline BaseObject* createInstance(BaseObject *parent) const
	{
        return m_objectConstructor ? (*m_objectConstructor)(parent) : nullptr;
	}

	//! Is the object is a dynamic class
    inline Bool isDynamic() const { return (m_objectConstructor != nullptr); }

	//! Get the class name
	inline String getClassName() const { return m_className; }

	//! Unique class type
	inline UInt32 getClassType() const { return m_classType; }

	//! Get the n-th inheritance class name
	inline String getBaseClassName(size_t n) const
	{
        return n < m_numParents ? m_parents[n]->m_className : nullptr;
	}

	//! Get the n-th inheritance class info
    inline const ClassInfo* getBaseClass(size_t n) const { return n < m_numParents ? m_parents[n] : nullptr; }

	//! Get the object size
	inline size_t getSize() const { return m_objectSize; }

	//! Get the object constructor function
	inline ObjectConstructorFn getConstructor() const { return m_objectConstructor; }

	//! get the first class info (linked list front)
	static const ClassInfo* getFirst() { return ms_first; }

	//! get the next class info (linked list next)
	inline const ClassInfo* getNext() const { return m_next; }

	//! Find a class by its name
	inline static ClassInfo* findClass(const String &className)
	{
		for (ClassInfo *info = ms_first; info ; info = info->m_next)
		{
			if (className == info->m_className)
				return info;
		}
        return nullptr;
	}

	//! Check if the class info have a type of info
	Bool getTypeOf(const ClassInfo *info) const
	{
		if (info == this)
			return True;

		for (size_t i = 0; i < m_numParents; ++i)
		{
			if (m_parents[i]->getTypeOf(info))
				return True;
		}

		return False;
	}

protected:

    const Char *m_className;
	UInt32 m_classType;

	size_t m_objectSize;

    ObjectConstructorFn m_objectConstructor;

    const ClassInfo *m_parents[9];
    const size_t m_numParents;

    //! class info object live in a linked list
    static ClassInfo *ms_first;
    ClassInfo *m_next;

    static ClassInfo* getBaseByName(const Char *name);

    //! registers the class
    void registerClass();
    //! un-registers the class
    void unRegisterClass();
};

} // namespace o3d

//---------------------------------------------------------------------------------------
// Dynamic class macros
//---------------------------------------------------------------------------------------

#define O3D_DECLARE_INTERFACE(name)                                           \
    public:                                                                   \
        static o3d::ClassInfo ms_classInfo;

#define O3D_DECLARE_ABSTRACT_CLASS(name)                                      \
    public:                                                                   \
        static o3d::ClassInfo ms_classInfo;                                   \
        virtual o3d::ClassInfo *getClassInfo() const { return &name::ms_classInfo; }

//! declare a non assignable class (no operator= )
#define O3D_DECLARE_DYNAMIC_CLASS_NO_ASSIGN(name)                             \
    O3D_NONASSIGNABLE(name)                                                   \
    O3D_DECLARE_DYNAMIC_CLASS(name)

//! declare a non copyable class (no copy constructor)
#define O3D_DECLARE_DYNAMIC_CLASS_NO_COPY(name)                               \
    O3D_NONCOPYABLE(name)                                                     \
    O3D_DECLARE_DYNAMIC_CLASS(name)

//! Declare a dynamic class
#define O3D_DECLARE_DYNAMIC_CLASS(name)                                       \
    O3D_DECLARE_ABSTRACT_CLASS(name)                                          \
    static o3d::BaseObject* createInstance(o3d::BaseObject *parent);

//! declare a non assignable abstract class (no operator= )
#define O3D_DECLARE_ABSTRACT_CLASS_NO_ASSIGN(name)                            \
    O3D_NONASSIGNABLE(name)                                                   \
    O3D_DECLARE_ABSTRACT_CLASS(name)

//! declare a non copyable abstract class (no copy constructor)
#define O3D_DECLARE_ABSTRACT_CLASS_NO_COPY(name)                              \
    O3D_NONCOPYABLE(name)                                                     \
    O3D_DECLARE_ABSTRACT_CLASS(name)

//! Declare a class
#define O3D_DECLARE_CLASS(name) O3D_DECLARE_DYNAMIC_CLASS(name)

//! common part of the macros below
#define O3D_IMPLEMENT_CLASS_COMMON(name, type, func) \
	ClassInfo name::ms_classInfo(#name, type, sizeof(name), (ObjectConstructorFn) func);

#define O3D_IMPLEMENT_CLASS_COMMON1(name, type, basename, func) \
	ClassInfo name::ms_classInfo(#name, type, &basename::ms_classInfo, sizeof(name), \
            (ObjectConstructorFn) func);

#define O3D_IMPLEMENT_CLASS_COMMON2(name, type, basename, baseclsinfo2, func) \
	ClassInfo name::ms_classInfo(#name, type, &basename::ms_classInfo, &baseclsinfo2::ms_classInfo, \
			sizeof(name), \
            (ObjectConstructorFn) func);

#define O3D_IMPLEMENT_CLASS_COMMON3(name, type, basename, baseclsinfo2, bc3, func) \
	ClassInfo name::ms_classInfo(#name, type, &basename::ms_classInfo, &baseclsinfo2::ms_classInfo, \
			&bc3::ms_classInfo, sizeof(name), \
            (ObjectConstructorFn) func);

#define O3D_IMPLEMENT_CLASS_COMMON4(name, type, basename, baseclsinfo2, bc3, bc4, func) \
	ClassInfo name::ms_classInfo(#name, type, &basename::ms_classInfo, &baseclsinfo2::ms_classInfo, \
			&bc3::ms_classInfo, bc4, sizeof(name), \
            (O3DObjectConstructorFn) func);

#define O3D_IMPLEMENT_CLASS_COMMON5(name, type, basename, baseclsinfo2, bc3, bc4, bc5, func) \
	ClassInfo name::ms_classInfo(#name, type, &basename::ms_classInfo, &baseclsinfo2::ms_classInfo, \
			&bc3::ms_classInfo, &bc4::ms_classInfo, \
			&bc5::ms_classInfo, sizeof(name), \
            (ObjectConstructorFn) func);

#define O3D_IMPLEMENT_CLASS_COMMON6(name, type, basename, baseclsinfo2, bc3, bc4, bc5, bc6, func) \
	ClassInfo name::ms_classInfo(#name, type, &basename::ms_classInfo, &baseclsinfo2::ms_classInfo, \
			&bc3::ms_classInfo, &bc4::ms_classInfo, \
			&bc5::ms_classInfo, &bc6::ms_classInfo, sizeof(name), \
            (ObjectConstructorFn) func);

#define O3D_IMPLEMENT_CLASS_COMMON7(name, type, basename, baseclsinfo2, bc3, bc4, bc5, bc6, bc7, func) \
	ClassInfo name::ms_classInfo(#name, type, &basename::ms_classInfo, &baseclsinfo2::ms_classInfo, \
			&bc3::ms_classInfo, &bc4::ms_classInfo, \
			&bc5::ms_classInfo, &bc6::ms_classInfo, \
			&bc7::ms_classInfo, sizeof(name), \
            (ObjectConstructorFn) func);

#define O3D_IMPLEMENT_CLASS_COMMON8(name, type, basename, baseclsinfo2, bc3, bc4, bc5, bc6, bc7, bc8, func) \
	ClassInfo name::ms_classInfo(#name, type, &basename::ms_classInfo, &baseclsinfo2::ms_classInfo, \
			&bc3::ms_classInfo, &bc4::ms_classInfo, \
			&bc5::ms_classInfo, &bc6::ms_classInfo, \
			&bc7::ms_classInfo, &bc8::ms_classInfo, sizeof(name), \
            (ObjectConstructorFn) func);

#define O3D_IMPLEMENT_CLASS_COMMON9(name, type, basename, baseclsinfo2, bc3, bc4, bc5, bc6, bc7, bc8, bc9, func) \
	ClassInfo name::ms_classInfo(#name, type, &basename::ms_classInfo, &baseclsinfo2::ms_classInfo, \
			&bc3::ms_classInfo, &bc4::ms_classInfo, \
			&bc5::ms_classInfo, &bc6::ms_classInfo, \
			&bc7::ms_classInfo, &bc8::ms_classInfo, \
			&bc9::ms_classInfo, sizeof(name), \
            (ObjectConstructorFn) func);

//---------------------------------------------------------------------------------------
// For instanciable classes
//---------------------------------------------------------------------------------------

//! Base class
#define O3D_IMPLEMENT_DYNAMIC_CLASS(name, type)                                     \
	O3D_IMPLEMENT_CLASS_COMMON(name, type, name::createInstance)                    \
    BaseObject* name::createInstance(BaseObject *parent) { return (BaseObject*)new name(parent); }

//! Single inheritance with one base class
#define O3D_IMPLEMENT_DYNAMIC_CLASS1(name, type, basename)                          \
	O3D_IMPLEMENT_CLASS_COMMON1(name, type, basename, name::createInstance)         \
    BaseObject* name::createInstance(BaseObject *parent) { return (BaseObject*)new name(parent); }

//! Multiple inheritance with two base classes
#define O3D_IMPLEMENT_DYNAMIC_CLASS2(name, type, basename1, basename2)                  \
	O3D_IMPLEMENT_CLASS_COMMON2(name, type, basename1, basename2, name::createInstance) \
    BaseObject* name::createInstance(BaseObject *parent) { return (BaseObject*)new name(parent); }

//! Multiple inheritance with 3 base classes
#define O3D_IMPLEMENT_DYNAMIC_CLASS3(name, type, basename1, bn2, bn3)                  \
	O3D_IMPLEMENT_CLASS_COMMON3(name, type, basename1, bn2, bn3, name::createInstance) \
    BaseObject* name::createInstance(BaseObject *parent) { return (BaseObject*)new name(parent); }

//! Multiple inheritance with 4 base classes
#define O3D_IMPLEMENT_DYNAMIC_CLASS4(name, type, basename1, bn2, bn3, bn4)                  \
	O3D_IMPLEMENT_CLASS_COMMON4(name, type, basename1, bn2, bn3, bn4, name::createInstance) \
    BaseObject* name::createInstance(BaseObject *parent) { return (BaseObject*)new name(parent); }

//! Multiple inheritance with 5 base classes
#define O3D_IMPLEMENT_DYNAMIC_CLASS5(name, type, basename1, bn2, bn3, bn4, bn5)                  \
	O3D_IMPLEMENT_CLASS_COMMON5(name, type, basename1, bn2, bn3, bn4, bn5, name::createInstance) \
    BaseObject* name::createInstance(BaseObject *parent) { return (BaseObject*)new name(parent); }

//! Multiple inheritance with 6 base classes
#define O3D_IMPLEMENT_DYNAMIC_CLASS6(name, type, basename1, bn2, bn3, bn4, bn5, bn6)                  \
	O3D_IMPLEMENT_CLASS_COMMON6(name, type, basename1, bn2, bn3, bn4, bn5, bn6, name::createInstance) \
    BaseObject* name::createInstance(BaseObject *parent) { return (BaseObject*)new name(parent); }

//! Multiple inheritance with 7 base classes
#define O3D_IMPLEMENT_DYNAMIC_CLASS7(name, type, basename1, bn2, bn3, bn4, bn5, bn6, bn7)                  \
	O3D_IMPLEMENT_CLASS_COMMON7(name, type, basename1, bn2, bn3, bn4, bn5, bn6, nb7, name::createInstance) \
    BaseObject* name::createInstance(BaseObject *parent) { return (BaseObject*)new name(parent); }

//! Multiple inheritance with 8 base classes
#define O3D_IMPLEMENT_DYNAMIC_CLASS8(name, type, basename1, bn2, bn3, bn4, bn5, bn6, bn7, bn8)                  \
	O3D_IMPLEMENT_CLASS_COMMON8(name, type, basename1, bn2, bn3, bn4, bn5, bn6, nb7, bn8, name::createInstance) \
    BaseObject* name::createInstance(BaseObject *parent) { return (BaseObject*)new name(parent); }

//! Multiple inheritance with 9 base classes
#define O3D_IMPLEMENT_DYNAMIC_CLASS9(name, type, basename1, bn2, bn3, bn4, bn5, bn6, bn7, bn8, bn9)                  \
	O3D_IMPLEMENT_CLASS_COMMON9(name, type, basename1, bn2, bn3, bn4, bn5, bn6, nb7, bn8, bn9, name::createInstance) \
    BaseObject* name::createInstance(BaseObject *parent) { return (BaseObject*)new name(parent); }

//---------------------------------------------------------------------------------------
// For abstract classes
//---------------------------------------------------------------------------------------

//! Base abstract class
#define O3D_IMPLEMENT_ABSTRACT_CLASS(name, type) \
    O3D_IMPLEMENT_CLASS_COMMON(name, type, nullptr)

//! Single inheritance with one base class
#define O3D_IMPLEMENT_ABSTRACT_CLASS1(name, type, basename) \
    O3D_IMPLEMENT_CLASS_COMMON1(name, type, basename, nullptr)

//! Multiple inheritance with two base classes
#define O3D_IMPLEMENT_ABSTRACT_CLASS2(name, type, basename1, basename2) \
    O3D_IMPLEMENT_CLASS_COMMON2(name, type, basename1, basename2, nullptr)

//! Multiple inheritance with 3 base classes
#define O3D_IMPLEMENT_ABSTRACT_CLASS3(name, type, basename1, basename2, basename3) \
    O3D_IMPLEMENT_CLASS_COMMON3(name, type, basename1, basename2, basename3, nullptr)

//! Multiple inheritance with 4 base classes
#define O3D_IMPLEMENT_ABSTRACT_CLASS4(name, type, bn1, bn2, bn3, bn4) \
    O3D_IMPLEMENT_CLASS_COMMON4(name, type, bn1, bn2, bn3, bn4, nullptr)

//! Multiple inheritance with 5 base classes
#define O3D_IMPLEMENT_ABSTRACT_CLASS5(name, type, bn1, bn2, bn3, bn4, bn5) \
    O3D_IMPLEMENT_CLASS_COMMON5(name, type, bn1, bn2, bn3, bn4, bn5, nullptr)

//! Multiple inheritance with 6 base classes
#define O3D_IMPLEMENT_ABSTRACT_CLASS6(name, type, bn1, bn2, bn3, bn4, bn5, bn6) \
    O3D_IMPLEMENT_CLASS_COMMON6(name, type, bn1, bn2, bn3, bn4, bn5, bn6, nullptr)

//! Multiple inheritance with 7 base classes
#define O3D_IMPLEMENT_ABSTRACT_CLASS7(name, type, bn1, bn2, bn3, bn4, bn5, bn6, bn7) \
    O3D_IMPLEMENT_CLASS_COMMON7(name, type, bn1, bn2, bn3, bn4, bn5, bn6, bn7, nullptr)

//! Multiple inheritance with 8 base classes
#define O3D_IMPLEMENT_ABSTRACT_CLASS8(name, type, bn1, bn2, bn3, bn4, bn5, bn6, bn7, bn8) \
    O3D_IMPLEMENT_CLASS_COMMON8(name, type, bn1, bn2, bn3, bn4, bn5, bn6, bn7, bn8, nullptr)

//! Multiple inheritance with 9 base classes
#define O3D_IMPLEMENT_ABSTRACT_CLASS9(name, type, bn1, bn2, bn3, bn4, bn5, bn6, bn7, bn8, bn9) \
    O3D_IMPLEMENT_CLASS_COMMON9(name, type, bn1, bn2, bn3, bn4, bn5, bn6, bn7, bn8, bn9, nullptr)

#endif // _O3D_CLASSINFO_H
