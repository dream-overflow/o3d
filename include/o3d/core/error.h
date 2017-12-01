/**
 * @file error.h
 * @brief Common errors and exceptions
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-10-12
 * @copyright Copyright (C) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_ERROR_H
#define _O3D_ERROR_H

#include "memorydbg.h"
#include "string.h"
#include <stdexcept>

namespace o3d {

#define O3D_E_DEF(class,DESCR) { m_type = #class; m_descr = DESCR; }

#ifdef _MSC_VER
#define O3D_E_DEF_CLASS(CLASS) \
		public: \
		CLASS & set(Int32 _line, const WChar * _file, const WChar * _function) throw() { \
			m_line = _line; \
			m_file = _file; \
			m_function = _function; \
			return *this; }
#else
#define O3D_E_DEF_CLASS(CLASS) \
		public: \
		CLASS & set(Int32 _line, const Char * _file, const Char * _function) throw() { \
			m_line = _line; \
			m_file.fromUtf8(_file); \
			m_function.fromUtf8(_function); \
			return *this; }
#endif // _MSC_VER

/**
 * @brief Base class for all exceptions
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-10-12
 */
class O3D_API E_BaseException : public std::exception
{
protected:

	//! Ctor
	E_BaseException(const String &msg) throw();

public:

	//! destructor
	virtual ~E_BaseException() throw() {}

	//! Copy operator
	inline const E_BaseException& operator= (const E_BaseException &dup)
	{
		m_msg = dup.m_msg;
		m_filename = dup.m_filename;

		return *this;
	}

	//! get The message of the exception (avoid it)
	virtual const char* what() const throw();

	//! get The message of the exception (preferred method)
	inline const String& getMsg() const { return m_msg; }

	//! Get The type of the exception
	inline String getType() const { return m_type; }

	//! Get The exception description
	inline String getDescr() const { return m_descr; }

	//! Get the file name for a file exception. Not used in non FileException
	inline const String& getFileName() const { return m_filename; }

protected:

	String m_type;
	String m_descr;

	UInt32 m_line;
	String m_file;
	String m_function;
	String m_msg;
	String m_filename;
};

//! @class E_InvalidParameter used for an invalid parameter value
class O3D_API E_InvalidParameter : public E_BaseException
{
	O3D_E_DEF_CLASS(E_InvalidParameter)

	//! Ctor
	E_InvalidParameter(const String& msg) throw() : E_BaseException(msg)
		O3D_E_DEF(E_InvalidParameter,"Invalid parameter value or state")
};

//! @class E_NullPointer used for a null pointer
class O3D_API E_NullPointer : public E_BaseException
{
	O3D_E_DEF_CLASS(E_NullPointer)

	//! Ctor
	E_NullPointer(const String& msg) throw() : E_BaseException(msg)
        O3D_E_DEF(E_NullPointer,"The pointer is null")
};

//! @class E_InvalidFormat used for an invalid parameter format
class O3D_API E_InvalidFormat : public E_BaseException
{
	O3D_E_DEF_CLASS(E_InvalidFormat)

	//! Ctor
	E_InvalidFormat(const String& msg) throw() : E_BaseException(msg)
		O3D_E_DEF(E_InvalidFormat,"Invalid parameter format")
};

//! @class E_InvalidFormat used for an invalid result
class O3D_API E_InvalidResult : public E_BaseException
{
	O3D_E_DEF_CLASS(E_InvalidResult)

	//! Ctor
	E_InvalidResult(const String& msg) throw() : E_BaseException(msg)
		O3D_E_DEF(E_InvalidResult,"Invalid result value")
};

//! @class E_InvalidClassName used for an invalid class name
class O3D_API E_InvalidClassName : public E_BaseException
{
	O3D_E_DEF_CLASS(E_InvalidClassName)

	//! Ctor
	E_InvalidClassName(const String& msg) throw() : E_BaseException(msg)
		O3D_E_DEF(E_InvalidClassName,"Invalid class name")
};

//! @class E_InvalidClassName used for an invalid class name
class O3D_API E_IndexOutOfRange : public E_BaseException
{
	O3D_E_DEF_CLASS(E_IndexOutOfRange)

	//! Ctor
	E_IndexOutOfRange(const String& msg) throw() : E_BaseException(msg)
		O3D_E_DEF(E_IndexOutOfRange,"Index is out of range")
};

//! @class E_ValueRedefinition used when a value already existing
class O3D_API E_ValueRedefinition : public E_BaseException
{
	O3D_E_DEF_CLASS(E_ValueRedefinition)

	//! Ctor
	E_ValueRedefinition(const String& msg) throw() : E_BaseException(msg)
		O3D_E_DEF(E_ValueRedefinition,"Redefinition of an existing value")
};

//! @class E_InvalidPrecondition used when a precondition value is not valid
class O3D_API E_InvalidPrecondition : public E_BaseException
{
	O3D_E_DEF_CLASS(E_InvalidPrecondition)

	//! Ctor
	E_InvalidPrecondition(const String& msg) throw() : E_BaseException(msg)
		O3D_E_DEF(E_InvalidPrecondition,"The precondition is not observed")
};

//! @class E_InvalidAllocation used when an object allocation return NULL
class O3D_API E_InvalidAllocation : public E_BaseException
{
	O3D_E_DEF_CLASS(E_InvalidAllocation)

	//! Ctor
	E_InvalidAllocation(const String& msg) throw() : E_BaseException(msg)
		O3D_E_DEF(E_InvalidAllocation,"The allocation failed")
};

//! @class E_FileNotFoundOrInvalidRights used when a file is not found or opening rights are not allowed for it
class O3D_API E_FileNotFoundOrInvalidRights : public E_BaseException
{
	O3D_E_DEF_CLASS(E_FileNotFoundOrInvalidRights)

	//! Ctor
	E_FileNotFoundOrInvalidRights(const String& msg, const String& filename) throw() : E_BaseException(msg)
		{ m_type = "E_FileNotFoundOrInvalidRights" ; m_descr = "The file does not exist or access rights are not allowed"; m_filename = filename; }
		//O3D_E_DEF(E_FileNotFoundOrInvalidRights,"The file does not exist or access rights are not allowed")
};

//! @class E_StringOverflow used when a string read operation from a file lead to a bad string end (no \0)
class O3D_API E_StringOverflow : public E_BaseException
{
    O3D_E_DEF_CLASS(E_StringOverflow)

	//! Ctor
    E_StringOverflow(const String& msg) throw() : E_BaseException(msg)
        O3D_E_DEF(E_StringOverflow,"The readed string overflow")
};

//! @class E_StringUnderflow used when a string read operation from a file lead to a bad string end (no \0)
class O3D_API E_StringUnderflow : public E_BaseException
{
    O3D_E_DEF_CLASS(E_StringUnderflow)

	//! Ctor
    E_StringUnderflow(const String& msg) throw() : E_BaseException(msg)
        O3D_E_DEF(E_StringUnderflow,"The readed string underflow")
};

//! @class E_UnsuportedFeature used when a feature behavior is not supported
class O3D_API E_UnsuportedFeature : public E_BaseException
{
	O3D_E_DEF_CLASS(E_UnsuportedFeature)

	//! Ctor
	E_UnsuportedFeature(const String& msg) throw() : E_BaseException(msg)
		O3D_E_DEF(E_UnsuportedFeature,"The requested feature is not supported")
};

//! @class E_EmptyStack used when a value from an empty stack is queried
class O3D_API E_EmptyStack : public E_BaseException
{
	O3D_E_DEF_CLASS(E_EmptyStack)

	//! Ctor
	E_EmptyStack(const String& msg) throw()  : E_BaseException(msg)
		O3D_E_DEF(E_EmptyStack,"The stack is empty")
};

//! @class E_InvalidOperation used when an invalid operation is performed
class O3D_API E_InvalidOperation : public E_BaseException
{
	O3D_E_DEF_CLASS(E_InvalidOperation)

	//! Ctor
	E_InvalidOperation(const String& msg) throw() : E_BaseException(msg)
		O3D_E_DEF(E_InvalidOperation,"The performed operation is invalid")
};

} // namespace o3d

#endif // _O3D_ERROR_H
