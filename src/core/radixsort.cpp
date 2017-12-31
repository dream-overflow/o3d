/**
 * @file radixsort.cpp
 * @brief Thanks to Pierre Terdiman for its revisited RadixSort.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-03-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/radixsort.h"

#include "o3d/core/debug.h"

using namespace o3d;

/*---------------------------------------------------------------------------------------
  constructor
---------------------------------------------------------------------------------------*/
RadixSort::RadixSort() :
    m_pIndices(nullptr),
    m_pIndices2(nullptr),
	m_CurrentSize(0),
	m_PrevSize(0),
	m_TotalCalls(0),
	m_NbHits(0)
{
	// Allocate input-independent ram
#ifndef O3D_RADIX_LOCAL_RAM
	m_pHistogram = new UInt32[256*4];
	m_pOffset    = new UInt32[254];
#endif
	// Initialize indices
	resetIndices();
}

/*---------------------------------------------------------------------------------------
  destructor
---------------------------------------------------------------------------------------*/
RadixSort::~RadixSort()
{
#ifndef O3D_RADIX_LOCAL_RAM
	deleteArray(m_pOffset);
	deleteArray(m_pHistogram);
#endif
	deleteArray(m_pIndices);
	deleteArray(m_pIndices2);
}

/*---------------------------------------------------------------------------------------
  resize arrays
---------------------------------------------------------------------------------------*/
Bool RadixSort::resize(UInt32 size)
{
	// Free previously used ram
	deleteArray(m_pIndices);
	deleteArray(m_pIndices2);

	// Get some fresh one
	m_pIndices	= new UInt32[size];
	m_pIndices2	= new UInt32[size];

	m_CurrentSize = size;

	// Initialize indices so that the input buffer is read in sequential order
	resetIndices();
	return True;
}

#define CHECK_RESIZE(n)                                                                         \
	if(n != m_PrevSize)                                                                         \
	{                                                                                           \
		if(n > m_CurrentSize) resize(n);                                                        \
		else                  resetIndices();                                                   \
		m_PrevSize = n;                                                                         \
	}

#define CREATE_HISTOGRAMS(type,buffer)                                                          \
	/* Clear counters */                                                                        \
	memset(m_pHistogram,0,256*4*sizeof(UInt32));                                            \
	                                                                                            \
	/* Prepare for temporal coherence */                                                        \
	type prevVal = (type)buffer[m_pIndices[0]];                                                 \
	Bool AlreadySorted = True;                                                          \
	UInt32* Indices = m_pIndices;                                                           \
	                                                                                            \
	/* Prepare to count */                                                                      \
	UInt8* p = (UInt8*)input;                                                           \
	UInt8* pe = &p[nbelt*4];                                                                \
	UInt32* h0 = &m_pHistogram[0];      /* histogram for first pass (LSB) */                \
	UInt32* h1 = &m_pHistogram[256];    /* Histogram for second pass      */                \
	UInt32* h2 = &m_pHistogram[512];    /* Histogram for third pass       */                \
	UInt32* h3 = &m_pHistogram[768];    /* histogram for last pass (MSB)  */                \
	                                                                                            \
	while (p != pe)                                                                             \
	{                                                                                           \
		/* Read input buffer in previous sorted order */                                        \
		type val = (type)buffer[*Indices++];                                                    \
		/* Check whether already sorted or not */                                               \
		if (val < prevVal) { AlreadySorted = False; break; } /* Early out */                \
		/* Update for next iteration */                                                         \
		prevVal = val;                                                                          \
	                                                                                            \
		/* Create histograms */                                                                 \
		h0[*p++]++;	h1[*p++]++;	h2[*p++]++;	h3[*p++]++;                                         \
	}                                                                                           \
	                                                                                            \
	/* If all input values are already sorted, we just have to return and leave the */          \
	/* previous list unchanged. That way the routine may take advantage of temporal */          \
	/* coherence, for example when used to sort transparent faces.                  */          \
	if (AlreadySorted) { m_NbHits++; return *this; }                                            \
	                                                                                            \
	/* Else there has been an early out and we must finish computing the histograms */          \
	while (p != pe)                                                                             \
	{                                                                                           \
		/* Create histograms without the previous overhead */                                   \
		h0[*p++]++;	h1[*p++]++;	h2[*p++]++;	h3[*p++]++;                                         \
	}

#define CHECK_PASS_VALIDITY(pass)                                                               \
	/* Shortcut to current counters */                                                          \
	UInt32* CurCount = &m_pHistogram[pass<<8];                                              \
	                                                                                            \
	/* Reset flag. The sorting pass is supposed to be performed. (default) */                   \
	Bool PerformPass = True;                                                            \
	                                                                                            \
	/* Check pass validity */                                                                   \
	                                                                                            \
	/* If all values have the same byte, sorting is useless. */                                 \
	/* It may happen when sorting bytes or words instead of dwords. */                          \
	/* This routine actually sorts words faster than dwords, and bytes */                       \
	/* faster than words. Standard running time (O(4*n))is reduced to O(2*n) */                 \
	/* for words and O(n) for bytes. Running time for floats depends on actual values... */     \
	                                                                                            \
	/* Get first byte */                                                                        \
	UInt8 uniqueVal = *(((UInt8*)input)+pass);                                          \
	                                                                                            \
	/* Check that byte's counter */                                                             \
	if (CurCount[uniqueVal] == nbelt) PerformPass = False;

/*---------------------------------------------------------------------------------------
  sort methods for int
---------------------------------------------------------------------------------------*/
RadixSort& RadixSort::sort(const UInt32* input,UInt32 nbelt,Bool signedvalues)
{
	if(!input || !nbelt) return *this;

	// Stats
	m_TotalCalls++;

	// Resize lists if needed
	CHECK_RESIZE(nbelt);

#ifdef O3D_RADIX_LOCAL_RAM
	// Allocate histograms & offsets on the stack
	UInt32 m_pHistogram[256*4];
	UInt32 m_pOffset[256];
#endif

	// Create histograms (counters). Counters for all passes are created in one run.
	// Pros:    read input buffer once instead of four times
	// Cons:    pHistogram is 4Kb instead of 1Kb
	// We must take care of signed/unsigned values for temporal coherence.... I just
	// have 2 code paths even if just a single opcode changes. Self-modifying code, someone?
	if (!signedvalues)  { CREATE_HISTOGRAMS(UInt32,input);  }
	else                { CREATE_HISTOGRAMS(Int16,input); }

	// Compute #negative values involved if needed
	UInt32 NbNegativeValues = 0;
	if(signedvalues)
	{
		// An efficient way to compute the number of negatives values we'll have to deal with is simply to sum the 128
		// last values of the last histogram. Last histogram because that's the one for the Most Significant Byte,
		// responsible for the sign. 128 last values because the 128 first ones are related to positive numbers.
		UInt32* h3= &m_pHistogram[768];
		for(UInt32  i = 128 ; i < 256 ; i++) NbNegativeValues += h3[i]; // 768 for last histogram, 128 for negative part
	}

	// radix sort, j is the pass number (0=LSB, 3=MSB)
	for (UInt32 j = 0 ; j < 4 ; j++)
	{
		CHECK_PASS_VALIDITY(j);

		// Sometimes the fourth (negative) pass is skipped because all numbers are negative and the MSB is 0xFF (for example). This is
		// not a problem, numbers are correctly sorted anyway.
		if (PerformPass)
		{
			// Should we care about negative values?
			if ( j!=3 || !signedvalues )
			{
				// Here we deal with positive values only

				// Create offsets
				m_pOffset[0] = 0;
				for (UInt32 i = 1 ; i < 256 ; i++) m_pOffset[i] = m_pOffset[i-1] + CurCount[i-1];
			}
			else
			{
				// This is a special case to correctly handle negative integers. They're sorted in the right order but at the wrong place.

				// Create biased offsets, in order for negative numbers to be sorted as well
				m_pOffset[0] = NbNegativeValues;   // First positive number takes place after the negative ones
				for(UInt32 i = 1 ; i < 128 ; i++)	m_pOffset[i] = m_pOffset[i-1] + CurCount[i-1];  // 1 to 128 for positive numbers

				// Fixing the wrong place for negative values
				m_pOffset[128] = 0;
				for(UInt32 i = 129 ; i < 256 ; i++) m_pOffset[i] = m_pOffset[i-1] + CurCount[i-1];
			}

			// Perform Radix Sort
			UInt8* inputBytes = (UInt8*)input;
			UInt32*  Indices    = m_pIndices;
			UInt32*  IndicesEnd = &m_pIndices[nbelt];

			inputBytes += j;

			while (Indices != IndicesEnd)
			{
				UInt32 id = *Indices++;
				m_pIndices2[m_pOffset[inputBytes[id<<2]]++] = id;
			}

			// Swap pointers for next pass. Valid indices - the most recent ones - are in pIndices after the swap.
			UInt32* Tmp = m_pIndices; m_pIndices = m_pIndices2; m_pIndices2 = Tmp;
		}
	}
	return *this;
}

/*---------------------------------------------------------------------------------------
  sort methods for float
---------------------------------------------------------------------------------------*/
RadixSort& RadixSort::sort(const Float* input2,UInt32 nbelt)
{
	if(!input2 || !nbelt) return *this;

	// Stats
	m_TotalCalls++;

	UInt32* input = (UInt32*)input2;

	// Resize lists if needed
	CHECK_RESIZE(nbelt);

#ifdef O3D_RADIX_LOCAL_RAM
	// Allocate histograms & offsets on the stack
	UInt32 m_pHistogram[256*4];
	UInt32 m_pOffset[256];
#endif

	// Create histograms (counters). Counters for all passes are created in one run.
	// Pros:    read input buffer once instead of four times
	// Cons:    mHistogram is 4Kb instead of 1Kb
	// Floating-point values are always supposed to be signed values, so there's only one code path there.
	// Please note the floating point comparison needed for temporal coherence! Although the resulting asm code
	// is dreadful, this is surprisingly not such a performance hit - well, I suppose that's a big one on first
	// generation Pentiums....We can't make comparison on integer representations because, as Chris said, it just
	// wouldn't work with mixed positive/negative values....
	{ CREATE_HISTOGRAMS(Float,input2); }

	// Compute #negative values involved if needed
	UInt32 NbNegativeValues = 0;
	// An efficient way to compute the number of negatives values we'll have to deal with is simply to sum the 128
	// last values of the last histogram. Last histogram because that's the one for the Most Significant Byte,
	// responsible for the sign. 128 last values because the 128 first ones are related to positive numbers.
	UInt32* h3= &m_pHistogram[768];
	for(UInt32 i = 128 ; i < 256 ; i++) NbNegativeValues += h3[i]; // 768 for last histogram, 128 for negative part

	// radix sort, j is the pass number (0=LSB, 3=MSB)
	for(UInt32 j = 0 ; j < 4 ; j++)
	{
		// Should we care about negative values?
		if (j != 3)
		{
			// Here we deal with positive values only
			CHECK_PASS_VALIDITY(j);

			if(PerformPass)
			{
				// Create offsets
				m_pOffset[0] = 0;
				for(UInt32 i = 1 ; i < 256 ; i++) m_pOffset[i] = m_pOffset[i-1] + CurCount[i-1];

				// Perform Radix Sort
				UInt8* inputBytes = (UInt8*)input;
				UInt32*  Indices    = m_pIndices;
				UInt32*  IndicesEnd = &m_pIndices[nbelt];

				inputBytes += j;

				while (Indices != IndicesEnd)
				{
					UInt32 id = *Indices++;
					m_pIndices2[m_pOffset[inputBytes[id<<2]]++] = id;
				}

				// Swap pointers for next pass. Valid indices - the most recent ones - are in mIndices after the swap.
				UInt32* Tmp = m_pIndices; m_pIndices = m_pIndices2; m_pIndices2 = Tmp;
			}
		}
		else
		{
			// This is a special case to correctly handle negative values
			CHECK_PASS_VALIDITY(j);

			if (PerformPass)
			{
				// Create biased offsets, in order for negative numbers to be sorted as well
				m_pOffset[0] = NbNegativeValues; // First positive number takes place after the negative ones
				for(UInt32 i = 1 ; i < 128 ; i++) m_pOffset[i] = m_pOffset[i-1] + CurCount[i-1]; // 1 to 128 for positive numbers

				// We must reverse the sorting order for negative numbers!
				m_pOffset[255] = 0;
				for (UInt32 i=0;i<127;i++)   m_pOffset[254-i] = m_pOffset[255-i] + CurCount[255-i]; // Fixing the wrong order for negative values
				for (UInt32 i=128;i<256;i++) m_pOffset[i] += CurCount[i];  // Fixing the wrong place for negative values

				// Perform Radix Sort
				for (UInt32 i = 0 ; i < nbelt ; i++)
				{
					UInt32 Radix = input[m_pIndices[i]]>>24; // Radix byte, same as above. AND is useless here (OR_U32).
					// ### cmp to be killed. Not good. Later.
					if (Radix < 128) m_pIndices2[m_pOffset[Radix]++] = m_pIndices[i]; // Number is positive, same as above
					else             m_pIndices2[--m_pOffset[Radix]] = m_pIndices[i]; // Number is negative, flip the sorting order
				}
				// Swap pointers for next pass. Valid indices - the most recent ones - are in pIndices after the swap.
				UInt32* Tmp = m_pIndices; m_pIndices = m_pIndices2; m_pIndices2 = Tmp;
			}
			else
			{
				// The pass is useless, yet we still have to reverse the order of current list if all values are negative.
				if (uniqueVal >= 128)
				{
					for(UInt32 i = 0 ; i < nbelt ; i++) m_pIndices2[i] = m_pIndices[nbelt-i-1];

					// Swap pointers for next pass. Valid indices - the most recent ones - are in pIndices after the swap.
					UInt32* Tmp = m_pIndices; m_pIndices = m_pIndices2; m_pIndices2 = Tmp;
				}
			}
		}
	}
	return *this;
}

/*---------------------------------------------------------------------------------------
  Resets the inner indices. After the call, mIndices is reset
---------------------------------------------------------------------------------------*/
void RadixSort::resetIndices()
{
	for (UInt32 i = 0 ; i < m_CurrentSize ; ++i) m_pIndices[i] = i;
}

/*--------------------------------------------------------------------------------------
  Gets the ram used
---------------------------------------------------------------------------------------*/
UInt32 RadixSort::getUsedRam()const
{
	UInt32 usedRam = sizeof(RadixSort);
#ifndef O3D_RADIX_LOCAL_RAM
	usedRam += 256*4*4;         // histograms
	usedRam += 256*4;           // offsets
#endif
	usedRam += 2*m_CurrentSize*4; // 2 lists of indices
	return usedRam;
}

