/**
 * @file dynamiclibrary.h
 * @brief Give the capacity to dynamically load a library (.so, .dll).
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-07-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DYNAMICLIBRARY_H
#define _O3D_DYNAMICLIBRARY_H

#include "memorydbg.h"
#include "string.h"
#include "error.h"

#include <functional>

namespace o3d {

/**
 * @brief Exception on loading a dynamic library
 */
class O3D_API E_DynamicLibraryException : public E_BaseException
{
	O3D_E_DEF_CLASS(E_DynamicLibraryException)

	//! Ctor
	E_DynamicLibraryException(const String& msg) throw() : E_BaseException(msg)
		O3D_E_DEF(E_DynamicLibraryException,"Unable to load the dynamic library")
};

/**
 * @brief Give the capacity to dynamically load a library (.so, .dll). The loaded library
 * must implement this class and add the O3D_DYNAMIC_LIBRARY(CLASSNAME).
 */
class O3D_API DynamicLibrary
{
public:

	typedef std::list<DynamicLibrary*> T_LibraryList;
	typedef T_LibraryList::iterator IT_LibraryList;
	typedef T_LibraryList::const_iterator CIT_LibraryList;

	/**
	 * @brief load a dynamic library. throws an exception in the library is not found or
	 * cannot be loaded. throws an exception if it cannot execute the init function.
	 * @param name Library filename.
	 * @return Instance of an object describing an handle to its library.
	 */
	static DynamicLibrary* load(const String &name);

	/**
	 * @brief unload a previously dynamic library and delete the object.
	 * @param library A valid object.
	 */
	static void unload(DynamicLibrary *library);

	/**
	 * @brief Check if a library is already loaded.
	 */
	static Bool isLoaded(const String &name);

	/**
	 * @brief Get the list of currently loaded libraries.
	 */
	static const T_LibraryList& getLoadedLibraries();

	/**
	 * @brief getNumLoadedLibraries
	 */
	static UInt32 getNumLoadedLibraries();

public:

	/**
	 * Get a function pointer of a function named by foo on the loaded library
	 */
	void* getFunctionPtr(const CString &foo) const;

	/**
	 * Get a functor of a function named by foo on the loaded library.
	 * R is the return type, Args are the arguments of the function.
	 * @return A std::function<R(Args...)>.
	 */
#ifndef _MSC_VER
	template<class R, class... Args>
	std::function<R(Args...)> getFunction(const CString &foo) const
	{
		typedef R Foo(Args...);
		Foo *fptr = (Foo*)getFunctionPtr(foo);
		return fptr;
	}

	/**
	 * Get a pointer of function named by foo on the loaded library.
	 * R is the function type.
	 * @return T typed function pointer
	 */
	template<class T>
    T getFunctionPtr(const CString &foo) const
	{
		return (T*)getFunctionPtr(foo);
	}
#endif

    /**
     * @brief Full path name of the module.
     */
    inline const String& getName() const
    {
        return m_name;
    }

private:

	String m_name;
	_HINSTANCE m_instance;

	static std::list<DynamicLibrary*> ms_libraries;
};

} // namespace o3d

#endif // _O3D_DYNAMICLIBRARY_H
