/**
 * @file radixsort.h
 * @brief Thanks to Pierre Terdiman for its revisited RadixSort.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2002-03-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_RADIXSORT_H
#define _O3D_RADIXSORT_H

#include "memorydbg.h"
#include "base.h"

#define O3D_RADIX_LOCAL_RAM

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class RadixSort
//-------------------------------------------------------------------------------------
//! Class for sort value (float,int), signed or unsigned, in growing order.
//! Sort by radix. Used frequently for sort when the speed is crucial.
//! (ie: Alpha blended surface). It use temporal coherence for test if sort is needed.
//! Thanks to Pierre Terdiman for its revisited RadixSort.
//---------------------------------------------------------------------------------------
class O3D_API RadixSort
{
private:

	Int32 m_counter;

	UInt32* m_pIndices;     //!< Two lists, swapped each pass
	UInt32* m_pIndices2;

	UInt32 m_CurrentSize;   //!< Current size of the indices list
	UInt32 m_PrevSize;      //!< Size involved in previous call

	UInt32 m_TotalCalls;
	UInt32 m_NbHits;

	Bool resize(UInt32 size);
	void resetIndices();

#ifndef O3D_RADIX_LOCAL_RAM
	UInt32* m_pHistogram;   //!< Counters for each byte
	UInt32* m_pOffset;      //!< Counters for each byte
#endif

public:

	//! default constructor
	RadixSort();

	~RadixSort();

	//! sort methods
	RadixSort& sort(const UInt32* input,UInt32 nbelt,Bool signedvalues=True);
	RadixSort& sort(const Float* input2,UInt32 nbelt);

	//! Access to results. mIndices is a list of indices in sorted order, i.e. in the order you may further process your data
	inline UInt32* getIndices()const { return m_pIndices; }
	//! mIndices2 gets trashed on calling the sort routine, but otherwise you can recycle it the way you want.
	inline UInt32* getRecyclabe()const { return m_pIndices2; }

    //! Stats
	UInt32 getUsedRam()const;
	inline UInt32 getNumTotalCalls()const { return m_TotalCalls; }
	//! Returns the number of premature exits due to temporal coherence.
	inline UInt32 getNumHits()const { return m_NbHits; }
};

} // namespace o3d

#endif // _O3D_RADIXSORT_H

