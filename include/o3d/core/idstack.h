/**
 * @file idstack.h
 * @brief availability.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-11-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_IDSTACK_H
#define _O3D_IDSTACK_H

#include "base.h"
#include "memorydbg.h"
#include "debug.h"
#include <stack>

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class IDStack
//-------------------------------------------------------------------------------------
//! ID stack contain a list of ID to attribute according to theirs availability.
//---------------------------------------------------------------------------------------
template <class T>
class O3D_API IDStack
{
public:

	//! default constructor
	IDStack() {}

	//! set ID list, beginning by start value to end value (or inverse if start > end)
	void init(T start, T end)
	{
	    if (end < start)
			O3D_ERROR(E_InvalidParameter("start must be lesser than end"));

		for (T i = start ; i < end ; ++i)
		    m_IDStack.push(i);
    }

	//! get free ID
	T getNewID()
    {
		// if no ID available throws an exception
	    if (m_IDStack.empty())
			O3D_ERROR(E_EmptyStack(""));

	    // Pop value from stack
		T id = m_IDStack.top();
	    m_IDStack.pop();
		return id;
    }

	//! release an used ID
	inline void releaseID(T Id) { m_IDStack.push(Id); }

	//! test is the stack contain some available ID
	inline Bool isAvailableID() const { return !m_IDStack.empty(); }

protected:

	std::stack<T> m_IDStack; //!< list that contain ordered ID
};

} // namespace o3d

#endif // _O3D_IDSTACK_H

