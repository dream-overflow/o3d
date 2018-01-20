/**
 * @file assert.h
 * @brief Debug dynamic and static assertions.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-11-21
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ASSERT_H
#define _O3D_ASSERT_H

#include "objective3dconfig.h"

//---------------------------------------------------------------------------------------
// Debug Assertion
//---------------------------------------------------------------------------------------

namespace o3d
{
//! Empty method.
inline void foo() {}

//! Static assert generic struct.
template<bool>
struct CompileCheckTime
{
	CompileCheckTime(...) {}
};

//! Static assert false specialization
template<>
struct CompileCheckTime<false> {};

} // namespace o3d

#ifdef _MSC_VER
	#define O3D_FOO() o3d::foo()
#else
	#define O3D_FOO() do {} while(0)
#endif

#ifdef _DEBUG
	#ifdef _MSC_VER
		#if (O3D_USE_CRT_WNDREPORT == 1)
			#include <crtdbg.h>
			#define O3D_ASSERT(expr) _ASSERTE((expr))
			#define O3D_ASSERT_MSG(expr, msg) _ASSERT_EXPR((expr), (msg))
			#define O3D_DBGBREAK() _CrtDbgBreak()
		#else
			#include <crtdbg.h>
			#define O3D_ASSERT(expr) (void) ((!!(expr)) || (_CrtDbgBreak(), 0))
			#define O3D_ASSERT_MSG(expr, msg) _ASSERT_EXPR((expr), (msg))
			#define O3D_DBGBREAK() _CrtDbgBreak()
		#endif
	#else
		#include <assert.h>
		#define O3D_ASSERT(expr) assert((expr))
		#define O3D_DBGBREAK() do { __asm__ __volatile__ ("int3; jmp 1f; 1:"); } while(0)
	#endif
#else
	#define O3D_ASSERT(expr) O3D_FOO()
	#define O3D_DBGBREAK() O3D_FOO()
#endif // _DEBUG

//! Similar to assert, but check expression at compile time. The error message
//! must be a valid C++ classname (ie. no spaces, functions or reserved keywords).
#ifdef _DEBUG
	#define O3D_STATIC_ASSERT(expr, errormsg)                   \
		do {                                                    \
		struct ERROR_##errormsg {};                             \
		typedef o3d::CompileCheckTime< (expr) != 0 > checkType; \
		checkType check = checkType(ERROR_##errormsg());        \
		sizeof(check);                                          \
	} while (0)
#else
	#define O3D_STATIC_ASSERT(expr, errormsg) O3D_FOO()
#endif

#endif // _O3D_ASSERT_H
