/**
 * @file callback.h
 * @brief C-style function and class method callback objects.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-01-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CALLBACK_H
#define _O3D_CALLBACK_H

#include "base.h"
#include "memorydbg.h"

namespace o3d {

/**
 * @class The Callback class
 * @brief Abstract Base class for callback
 */
class O3D_API Callback
{
public:

	//! Call the callback with an optional parameter ptr
    virtual Int32 call(void* info) = 0;
	virtual ~Callback();
};

/**
 * @brief The CallbackFunction class
 * Generic callback base class container with one or two argument.
 */
class O3D_API CallbackFunction : public Callback
{
public:

	//! Construct with one argument callback
    CallbackFunction(Int32 (*function)(void*)):
        m_function(function),
        m_function2(nullptr),
        m_util(nullptr),
		m_two(False) {}

	//! Construct with two arguments callback
	//! @param util The second argument given to the callback call
    CallbackFunction(Int32 (*function)(void*, void*), void* util):
        m_function(nullptr),
		m_function2(function),
        m_util(util),
		m_two(True) {}

	//! Destructor
	virtual ~CallbackFunction();

	//! call the callback with an optional parameter ptr (null if nothing to give)
	//! @param info Correspond to the first argument of the callback
    virtual Int32 call(void* info);

protected:

    Int32 (*m_function )(void*);
    Int32 (*m_function2)(void*, void*);

    void* m_util;

    Bool m_two;
};

/**
 * @brief The CallbackMethod class
 * @brief Generic callback base class container with one or two argument.
 */
template<class T>
class O3D_API_TEMPLATE CallbackMethod : public Callback
{
public:

	//! Construct with one argument callback
    CallbackMethod(T* object, Int32 (T::*method)(void*)):
        m_method(method),
        m_method2(nullptr),
        m_object(object),
        m_util(nullptr),
		m_two(False) {}

	//! Construct with two arguments callback
	//! @param util The second argument given to the callback call
    CallbackMethod(T* object, Int32 (T::*method)(void*, void*), void* util):
        m_method(nullptr),
		m_method2(method),
        m_object(object),
        m_util(util),
		m_two(True) {}

	//! Destructor
	virtual ~CallbackMethod() {}

	//! call the callback with an optional parameter ptr (null if nothing to give)
	//! @param info Correspond to the first argument of the callback
    virtual Int32 call(void* info)
	{
        if (m_two) {
            return (m_object->*m_method2)(info,m_util);
        } else {
            return (m_object->*m_method)(info);
        }
	}

protected:

    Int32 (T::*m_method)(void*);
    Int32 (T::*m_method2)(void*, void*);

    T* m_object;
    void* m_util;

    Bool m_two;
};

} // namespace o3d

#endif // _O3D_CALLBACK_H
