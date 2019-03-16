/**
 * @file baseobject.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2014-02-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_BASEOBJECT_H
#define _O3D_BASEOBJECT_H

#include <list>

#include "string.h"
#include "evt.h"
#include "evthandler.h"
#include "classinfo.h"

namespace o3d
{

class BaseSmartObject;
class InStream;
class OutStream;

/**
 * @brief Interface for sharable objects (smart object reference).
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2014-02-24
 */
class O3D_API Sharable
{
public:

    O3D_DECLARE_INTERFACE(Sharable)

    virtual ~Sharable() = 0;

    /**
     * @brief useIt Register a user for this object.
     * @param smartObject The smart object containing the link to register.
     */
    virtual void useIt(BaseSmartObject &smartObject) = 0;

    /**
     * @brief releaseIt Unregister a previous user of this object.
     * @param smartObject The smart object containing the link to unregister.
     * @return True if the object is no longer used.
     */
    virtual Bool releaseIt(BaseSmartObject &smartObject) = 0;

    /**
     * @brief deleteIt Delete the object using its parent if one. All attachment (users)
     * must be previously detached. By default if the object have no parent it is
     * immediately deleted. If it have a parent, DeleteChild is called.
     * @note After this operation can invalid the object.
     * @return True if deleted.
     */
    virtual Bool deleteIt() = 0;
};

/**
 * @brief Common base class. All typical object must inherit from it, except atomics or
 * other very specialized objects.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-12-05
 */
class O3D_API BaseObject : public EvtHandler, public virtual Sharable
{
    O3D_DECLARE_BASE_ABSTRACT_CLASS(BaseObject)

public:

	typedef std::list<BaseSmartObject*> T_UserList;
	typedef T_UserList::iterator IT_UserList;
	typedef T_UserList::const_iterator CIT_UserList;

	//! Default constructor.
	//! Name is "undefined", Identifier set to -1, and Type to O3D_UNDEFINED.
    BaseObject(BaseObject *parent = nullptr);

	//! Copy constructor.
	//! Simply duplicate the Name and Type. Id is set to -1.
	BaseObject(const BaseObject& dup);

	//! Destructor. Release all users.
	virtual ~BaseObject();

	//! Duplicator
	//! Simply duplicate the Name and Type. Id is set to -1.
	BaseObject& operator= (const BaseObject& dup);

	//! Check for a class type.
    Bool getTypeOf(ClassInfo *info) const;


	//-----------------------------------------------------------------------------------
	// Properties
	//-----------------------------------------------------------------------------------

	//! Get the type of the object.
    inline UInt32 getType() const { return getClassInfo()->getClassType(); }

	//! Define the unique identifier (-1 mean un-identified).
    inline void setId(Int32 id) { m_id = id; }
	//! Get the unique object identifier.
    inline Int32 getId() const { return m_id; }

	//! Set the name of the object.
	inline void setName(const String& name) { m_name = name; }
	//! Get the name of the object (read only).
	inline const String& getName() const { return m_name; }
	//! Get the name of the object.
    inline String& getName() { return m_name; }

	//! Define the serialize identifier.
    inline void setSerializeId(Int32 id) { m_serializeId = id; }
	//! Get the serialize identifier.
    inline Int32 getSerializeId() const { return m_serializeId; }


	//-----------------------------------------------------------------------------------
	// Parent<=>Child definition
	//-----------------------------------------------------------------------------------

	//! Change the parent object.
	virtual void setParent(BaseObject *parent);
	//! Get the parent object.
	inline       BaseObject* getParent()       { return m_parent; }
	//! Get the parent object (read only).
	inline const BaseObject* getParent() const { return m_parent; }

	//! Get the top-level parent object.
    inline BaseObject* getTopLevelParent() { return m_topLevelParent; }
	//! Get the top-level parent object (read only).
	inline const BaseObject* getTopLevelParent() const { return m_topLevelParent; }

	//! Compare the top-level parent object type.
    virtual Bool hasTopLevelParentTypeOf() const { return True; }

	//! Check if the object has a parent.
    inline Bool hasParent() const { return m_parent != nullptr; }

	//! Delete the object using its parent if one. All attachment (users) must be
	//! previously detached. By default if the object have no parent it is immediately deleted.
	//! If it have a parent, DeleteChild is called.
	//! @note After this operation can invalid the object.
    virtual Bool deleteIt();

	//! Delete a child object.
    virtual Bool deleteChild(BaseObject *child);

	//! An object become child of this.
	virtual void parentIt(BaseObject *child);

	//! An child change of this parent.
	virtual void unparentIt(BaseObject *child);


	//-----------------------------------------------------------------------------------
	// Users
	//-----------------------------------------------------------------------------------

	//! Set this object as persistent. That mean this object is kept even if it has no user
	//! Only ReleaseAll can remove the persistent user (null).
    void setPersistant(Bool persistant);

	//! Is the object is persistent.
    inline Bool isPersistant() const
    {
        return (!m_usersList.empty() && (m_usersList.front() == nullptr));
    }

    virtual void useIt(BaseSmartObject &smartObject);

    virtual Bool releaseIt(BaseSmartObject &smartObject);

	//! Check for the presence of an user.
	//! @return TRUE if there is at least one user.
    inline Bool hasUser() const { return !m_usersList.empty(); }

	//! Return true if there is no users using the object.
    inline Bool noLongerUsed() const { return m_usersList.empty(); }

	//! Return the number of users for the object. The parent is not count as a user.
    inline UInt32 getNumUsers() const { return static_cast<UInt32>(m_usersList.size()); }

	//! Detach all users.
	void releaseAll();

	//! Get the list of users.
	inline const T_UserList& getUsersList() const { return m_usersList; }

	//! An object is detached. Implement this method in a user object to know when
	//! an object is released.
	virtual void detachObject(BaseObject *object);


	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	//! De-serialize the object from a file.
    virtual Bool readFromFile(InStream &istream);
	//! Serialize the object to a file.
    virtual Bool writeToFile(OutStream &ostream);


	//-----------------------------------------------------------------------------------
	// Memory management
	//-----------------------------------------------------------------------------------

/*	inline void* operator new(std::size_t sz, const Char* file, Int32 line)
	{
        return O3D_NEW(sz,file,line,False);
	}

    inline void* operator new[] (std::size_t sz, const Char* file, Int32 line)
	{
        return O3D_NEW(sz,file,line,True);
	}

	inline void operator delete(void* ptr)
	{
        O3D_FREE(ptr,False);
	}

    inline void operator delete(void* ptr, const Char* file, Int32 line)
	{
		MemoryManager::Instance()->nextDelete(MemoryManager::MemoryCentral,file,line);
        O3D_FREE(ptr,False);
	}

	inline void operator delete[] (void* ptr)
	{
        O3D_FREE(ptr,True);
	}

    inline void operator delete[] (void* ptr, const Char* file, Int32 line)
	{
		MemoryManager::Instance()->nextDelete(MemoryManager::MemoryCentral,file,line);
        O3D_FREE(ptr,True);
	}*/

public:

	//! Called when a used object is removed or deleted.
    Signal<BaseObject*> onDeleteObject{this};

protected:

    Int32 m_id;             //!< unique object identifier (default is -1).
    String m_name;          //!< Object name (default is "undefined").

    Int32 m_serializeId;    //!< Temporary identifier, used for IO indexing.

	BaseObject *m_parent;          //!< Parent object (null if none).
    BaseObject *m_topLevelParent;  //!< Parent top level (null if none).

    T_UserList m_usersList; //!< Each user of this object is registered in this list.
};

//! Check if an object is a type of classInfo, and return the object himself if true,
//! else return false.
inline BaseObject* checkDynamicCast(BaseObject *object, ClassInfo *classInfo)
{
    return object && object->getClassInfo()->getTypeOf(classInfo) ? object : nullptr;
}

template <class _T, class _OBJ>
struct DynamicCastPtr
{
	static inline _T cast(_OBJ object)
	{
		typedef typename PtrCast<_T>::PointedType ClassInfo;

		return (_T)o3d::checkDynamicCast(
				const_cast<BaseObject*>(static_cast<const BaseObject*>(const_cast<_T>(static_cast<const _T>(object)))),
				&ClassInfo::ms_classInfo);
	}
};

template <class _T, class _OBJ>
struct DynamicCastRef
{
	static inline _T cast(const _OBJ &object)
	{
		typedef typename ReferenceCast<_T>::Result ClassInfo;
		typedef ClassInfo* PtrType;

		_OBJ *objectPtr = const_cast<_OBJ*>(&object);

		return (_T)*o3d::checkDynamicCast(
				const_cast<BaseObject*>(static_cast<const BaseObject*>(const_cast<PtrType>(static_cast<const PtrType>(objectPtr)))),
				&ClassInfo::ms_classInfo);
	}
};

//! Dynamic cast to _T.
template <class _T, class _OBJ>
inline _T dynamicCast(_OBJ const &object)
{
	static const bool result = o3d::RefType<_T>::Val;
	typedef typename ConstCast<_OBJ>::Result ObjectType;

	typedef typename StaticCond<result, DynamicCastRef<_T, ObjectType>, DynamicCastPtr<_T, ObjectType> >::Result CastType;

	return (_T)CastType::cast(object);
}

//! Is an object if a type of _T.
template <class _OBJ, class _T>
inline Bool typeOf(_OBJ /*object*/)
{
    return False;
}

//! Is a const BaseObject* a type of _T.
template <class _T>
inline Bool typeOf(const BaseObject *object)
{
    return object ? object->getTypeOf(&PtrCast<_T>::PointedType::ms_classInfo) : False;
}

//! Is a BaseObject* a type of _T.
template <class _T>
inline Bool typeOf(BaseObject *object)
{
    return object ? object->getTypeOf(&PtrCast<_T>::PointedType::ms_classInfo) : False;
}

//! Is a const BaseObject& a type of _T.
template <class _T>
inline Bool typeOf(const BaseObject &object)
{
    return object.getTypeOf(&PtrCast<_T>::PointedType::ms_classInfo);
}

//! Is a BaseObject& a type of _T.
template <class _T>
inline Bool typeOf(BaseObject &object)
{
    return object.getTypeOf(&PtrCast<_T>::PointedType::ms_classInfo);
}

} // namespace o3d

//#include "MemoryDbg.h"

#endif // _O3D_BASEOBJECT_H
