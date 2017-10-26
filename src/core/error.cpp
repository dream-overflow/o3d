/**
 * @file error.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/error.h"

#include "o3d/core/debug.h"

using namespace o3d;

// E_BaseException base class for all exceptions
E_BaseException::E_BaseException(const String& msg) throw()
#ifdef O3D_VC_COMPILER
    : std::exception(msg.toUtf8().getData())
#endif
{
	m_msg = msg;
	m_type = "E_BaseException";
	m_descr = "Unknown exception";

    //O3D_DBGBREAK();
}

// get The message of the exception (avoid it)
const char* E_BaseException::what() const throw()
{
	static CString what;

	if (m_filename.isValid())
		what = (m_file + String::print(":%u ",m_line) + m_function + " | " +
				m_descr + (m_descr.length() > 0 ? ", " : "") + getMsg() +
				"while processing \"" + m_filename + "\"").toUtf8();
	else
		what = (m_file + String::print(":%u ",m_line) + m_function + " | " +
				m_descr + (m_descr.length() > 0 ? ", " : "") + getMsg()).toUtf8();

	return what.getData();
}

