/**
 * @file callback.cpp
 * @brief Implementation of Callback.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-01-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/callback.h"

using namespace o3d;

Callback::~Callback()
{
}

CallbackFunction::~CallbackFunction()
{
}

Int32 CallbackFunction::call(void* info)
{
	if (m_two)
		return m_function2(info,m_util);
	else
		return m_function(info);
}

/*template <class T>


template <class T>
Int32 CallbackMethod<T>::call(void* info)
{
	if (m_two) return (m_object->*m_method2)(info,m_util);
	else return (m_object->*m_method)(info);
}*/

