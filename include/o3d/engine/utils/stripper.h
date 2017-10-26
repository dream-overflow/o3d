/**
 * @file stripper.h
 * @brief Primitive stripper. Transform triangles into optimized triangles or triangle strips.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2007-07-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_STRIPPER_H
#define _O3D_STRIPPER_H

#include "o3d/core/smartpointer.h"
#include "o3d/core/string.h"

#include <vector>

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Stripper
//-------------------------------------------------------------------------------------
//! You should take care of not using more that one object of this class at the same time,
//! because it uses Nvidia stripper which declares global static variables.
//---------------------------------------------------------------------------------------
class O3D_API Stripper
{
public:

	static const Int32 STRIPER_DEFAULT_CACHE_SIZE = 16;

	// Type definitions
	typedef std::vector<UInt16>		T_Ui16Index;
	typedef std::vector<UInt32>		T_Ui32Index;

	typedef std::vector<T_Ui16Index>	T_Ui16ListArray;
	typedef std::vector<T_Ui32Index>	T_Ui32ListArray;

	enum ResultType {
		RESULT_OPTIMIZED_TRIANGLES = 0,
		RESULT_SINGLE_TRIANGLE_STRIP,
		RESULT_TRIANGLE_STRIPS };

	enum StripperType {
		STRIPPER_AUTO = 0,
		STRIPPER_INVALID,
		STRIPPER_NVTRISTRIP,
		STRIPPER_TRISTRIPPER };

	// Constructors
	Stripper(StripperType = STRIPPER_AUTO);
	~Stripper();

	// Options
	void setCacheSize(UInt8 _size) { m_cacheSize = _size; }
	UInt8 getCacheSize() const { return m_cacheSize; }

	void setUseCache(Bool _state) { m_useCache = _state; }
	Bool getUseCache() const { return m_useCache; }

	// Fonctions
    Bool start(T_Ui16Index, ResultType, T_Ui16ListArray &, T_Ui16Index * = nullptr);
    Bool start(T_Ui32Index, ResultType, T_Ui32ListArray &, T_Ui32Index * = nullptr);

	//! Return the stripper which will be used for computation given the current configs
	//! and the specified result
	StripperType getStripper(ResultType) const;

	Bool isError() const { return (m_errorString.length() > 0); }

	String errorString() const
	{
		String errString(m_errorString);
		const_cast<Stripper*>(this)->m_errorString.destroy();
		return errString;
	}

private:

	// Members
	StripperType m_stripper; //!< Define which striper will be used (DEFAULT: auto)

	Bool m_useCache;	 //!< Activate the vertex cache optimization (DEFAULT: false) (Used if it's possible)
	UInt8 m_cacheSize;	 //!< Define the cache size (DEFAULT: 16) (Used if it's possible)

	String m_errorString;
};

} // namespace o3d

#endif // _O3D_STRIPER_H

