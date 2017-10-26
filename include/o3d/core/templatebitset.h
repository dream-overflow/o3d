/**
 * @file templatebitset.h
 * @brief Template bitset container.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2007-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEMPLATEBITSET_H
#define _O3D_TEMPLATEBITSET_H

#include "o3d/core/templatearray.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class TemplateBitSet
//-------------------------------------------------------------------------------------
//! Template bitset container. The template type define the size of the bitset.
//! For example an UInt32 bitset container can manage up to 32 bits.
//! At this time it does not support complex type.
//---------------------------------------------------------------------------------------
template <class T>
class O3D_API_TEMPLATE TemplateBitSet
{
public:

	//! Default constructor. All bits are set to zero
	TemplateBitSet() :
		m_BitSet(0)
	{}

	//! Copy constructor.
	TemplateBitSet(const TemplateBitSet<T> &dup) :
		m_BitSet(dup.m_BitSet)
	{}

	//! Duplicate
	TemplateBitSet& operator= (const TemplateBitSet<T> &dup)
	{
		m_BitSet = dup.m_BitSet;
		return *this;
	}

	//! Initialise from type T value.
	void set(const T &dup)
	{
		m_BitSet = dup;
	}

	//! Return the number bits
	inline UInt32 getSize() const { return (UInt32)sizeof(T)<<3; }

	//! Reset all bits to zero
	inline void zero() { m_BitSet = 0; }

	//! Fill all bits to one
	inline void fillTrue() { m_BitSet = Limits<T>::max(); }

	//! Return a copy of the managed object
	inline T getValue() const { return m_BitSet; }

	//! Set a bit
	inline void setBit(UInt32 bit, Bool value)
	{
		if (bit >= getSize())
			O3D_ERROR(E_IndexOutOfRange("bit"));

		// set the bit to 1
		if (value)
			m_BitSet |= (T(1) << bit);
		// set the bit to 0
		else
			m_BitSet &= ~(T(1) << bit);
	}

	//! Get a bit
	inline Bool getBit(UInt32 bit) const
	{
		if (bit >= getSize())
			O3D_ERROR(E_IndexOutOfRange("bit"));

		return (m_BitSet & (T(1) << bit));
	}

	//! Enable a bit (set to 1).
	inline void enable(UInt32 bit)
	{
		if (bit >= getSize())
			O3D_ERROR(E_IndexOutOfRange("bit"));

		// set the bit to 1
		m_BitSet |= (T(1) << bit);
	}

	//! Disable a bit (set to 0).
	inline void disable(UInt32 bit)
	{
		if (bit >= getSize())
			O3D_ERROR(E_IndexOutOfRange("bit"));

		// set the bit to 0
		m_BitSet &= ~(T(1) << bit);
	}

	//! Toggle a bit
	inline void toggle(UInt32 bit)
	{
		if (bit >= getSize())
			O3D_ERROR(E_IndexOutOfRange("bit"));

		if (m_BitSet & (T(1) << bit))
			// set the bit to 0
			m_BitSet &= ~(T(1) << bit);
		else
			// set the bit to 1
			m_BitSet |= (T(1) << bit);
	}

	//! Get a bit (read only).
	inline Bool operator[](UInt32 bit) const  { return getBit(bit); }

	//! Conversion operator to type T reference
    inline operator T&() const { return m_BitSet; }

	//! Conversion operator to type T reference
    inline operator T&() { return m_BitSet; }

	//! Deserialize.
	Bool readFromFile(InStream &is)
	{
        is >> m_BitSet;
		return True;
	}

	//! Serialize.
	Bool writeToFile(OutStream &os)
	{
        os << m_BitSet;
		return True;
	}

	//! convert to a string
	inline operator String() const
	{
		String temp("{ ");
		for (Int32 i = 0; i < getSize()-1; ++i)
		{
			temp << getBit(i) << String(", ");
		}
		temp << getBit(getSize()-1) << String(" }");

		return temp;
	}

private:

	T m_BitSet;  //!< The bits container
};

//! 8 bits bitset container
typedef TemplateBitSet<UInt8> BitSet8;
//! 16 bits bitset container
typedef TemplateBitSet<UInt16> BitSet16;
//! 32 bits bitset container
typedef TemplateBitSet<UInt32> BitSet32;
//! 64 bits bitset container
typedef TemplateBitSet<UInt64> BitSet64;

} // namespace o3d

#endif // _O3D_TEMPLATEBITSET_H

