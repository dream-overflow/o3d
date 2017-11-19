/**
 * @file smartarray.h
 * @brief Smart array container.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-07-31
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SMARTARRAY_H
#define _O3D_SMARTARRAY_H

#include "base.h"
#include "memorydbg.h"

namespace o3d {

class InStream;
class OutStream;

//---------------------------------------------------------------------------------------
//! @class SmartArray
//-------------------------------------------------------------------------------------
//! Static array manager with reference counter and auto-destruction.
//---------------------------------------------------------------------------------------
template<class T>
class O3D_API_TEMPLATE SmartArray
{
public:

	//! default constructor.
	SmartArray() :
        m_counter(nullptr),
		m_size(0),
        m_data(nullptr)
	{
	}

	//! initialization constructor.
    SmartArray(UInt32 size) :
        m_counter(nullptr),
		m_size(size)
	{
        //O3D_ASSERT(size);

        m_counter = new Int32;
		(*m_counter) = 1;

        m_data = new T[m_size];
	}

	//! copy constructor
	SmartArray(const SmartArray &dup) :
		m_size(dup.m_size),
		m_data(dup.m_data)
	{
		m_counter = dup.m_counter;
		useIt();
	}

	//! Initialization constructor from const or non const data.
	//! @param data Non const array to duplicate or to own.
	//! @param size Number of element of data.
	//! @param own If true this smart object will own it, in other case it simply duplicate it.
	//! @note If own if true, the given data ptr should be previously allocated using a new [].
    SmartArray(T *data, UInt32 size, Bool own) :
        m_counter(nullptr),
		m_size(size)
	{
		O3D_ASSERT(data && size);

        m_counter = new Int32;
		(*m_counter) = 1;

        if (!own) {
            m_data = new T[m_size];
			memcpy(m_data, data, m_size * sizeof(T));
        } else {
			m_data = data;
        }
	}

	//! Initialization constructor from const data.
	//! @param data Const array to duplicate.
	//! @param size Number of element of data.
    SmartArray(const T *data, UInt32 size) :
        m_counter(nullptr),
		m_size(size)
	{
		O3D_ASSERT(data && size);

        m_counter = new Int32;
		(*m_counter) = 1;

        m_data = new T[m_size];
		memcpy(m_data, data, m_size * sizeof(T));
	}

	//! Destructor.
	~SmartArray()
	{
		releaseIt();

        if (noLongerUsed()) {
			deleteArray(m_data);
			deletePtr(m_counter);
		}
	}

	//! Affectation.
	inline const SmartArray& operator=(const SmartArray &ptr)
	{
		// Check if it's the same data
        if (m_data != ptr.m_data) {
            if (m_data) {
				// If not, release the old pointer
				releaseIt();

                if (noLongerUsed()) {
                    deleteArray(m_data);
					deletePtr(m_counter);

					m_size = 0;
				}
			}

			m_data = ptr.m_data;
			m_size = ptr.m_size;

			m_counter = ptr.m_counter;
			useIt();
		}
		return *this;
	}

	//! Access to an element of the array.
    inline T& operator[](UInt32 pos)
	{
		O3D_ASSERT(pos < m_size);
		return m_data[pos];
	}
	//! Access to an element of the array (read only).
    inline const T& operator[](UInt32 pos) const
	{
		O3D_ASSERT(pos < m_size);
		return m_data[pos];
	}

	//! Access to the data pointer.
	inline T* getData() { return m_data; }
	//! Access to the data pointer (read only).
	inline const T* getData() const { return m_data; }

	//! Get reference counter value.
    inline Int32 getReferenceCounter() const { return (*m_counter); }

	//! Return true if counter value is 0. If the object is totally released.
    inline Bool noLongerUsed() const
	{
        if (m_counter) {
			return ((*m_counter) == 0);
        } else {
            return True;
        }
	}

	//! Get the length of the array.
    inline UInt32 getNumElt() const { return m_size; }

	//! Get the length of the array.
    inline UInt32 getSize() const { return m_size; }

	//! Get the length of the array in bytes.
    inline UInt32 getSizeInBytes() const { return m_size * sizeof(T); }

    //! Say whether or not the array data is valid.
    inline Bool isValid() const { return (m_data != nullptr); }

    //! Say whether or not the array size is empty.
    inline Bool isEmpty() const { return (m_size == 0); }

    //! Say whether or not the array is null.
    inline Bool isNull() const { return (m_data == nullptr); }

    //! Check if the object is totally released and then delete it.
    Bool releaseCheckAndDelete()
	{
        if (m_data == nullptr) {
            return False;
        }

		releaseIt();

        if (noLongerUsed()) {
            deleteArray(m_data);
			deletePtr(m_counter);

			m_size = 0;
            return True;
		}

        m_data = nullptr;
        m_counter = nullptr;
		m_size = 0;

        return False;
	}

	//! Allocate the data array of the given size.
    inline void allocate(UInt32 size)
	{
		releaseCheckAndDelete();

        m_counter = new Int32;
		(*m_counter) = 1;

        m_data = new T[size];
		m_size = size;
	}

    Bool writeToFile(OutStream &os) const;
    Bool readFromFile(InStream &is);

protected:

    Int32 *m_counter;  //!< Reference counter.

    UInt32 m_size;     //!< Size of the array.
    T *m_data;         //!< Array data.

	//! Use object.
	inline void useIt()
	{
        if (m_counter) {
			(*m_counter)++;
        }
	}

	//! Release object.
	inline void releaseIt()
	{
        if (m_counter) {
			(*m_counter)--;
			O3D_ASSERT((*m_counter) >=0);
		}
	}
};

template<> O3D_API Bool SmartArray<Char>::writeToFile(OutStream &os) const;
template<> O3D_API Bool SmartArray<Char>::readFromFile(InStream &is);

//template<> O3D_API Bool SmartArray<Int8>::writeToFile(OutStream &os) const;
//template<> O3D_API Bool SmartArray<Int8>::readFromFile(InStream &is);

template<> O3D_API Bool SmartArray<UInt8>::writeToFile(OutStream &os) const;
template<> O3D_API Bool SmartArray<UInt8>::readFromFile(InStream &is);

template<> O3D_API Bool SmartArray<Int16>::writeToFile(OutStream &os) const;
template<> O3D_API Bool SmartArray<Int16>::readFromFile(InStream &is);

template<> O3D_API Bool SmartArray<UInt16>::writeToFile(OutStream &os) const;
template<> O3D_API Bool SmartArray<UInt16>::readFromFile(InStream &is);

template<> O3D_API Bool SmartArray<Int32>::writeToFile(OutStream &os) const;
template<> O3D_API Bool SmartArray<Int32>::readFromFile(InStream &is);

template<> O3D_API Bool SmartArray<UInt32>::writeToFile(OutStream &os) const;
template<> O3D_API Bool SmartArray<UInt32>::readFromFile(InStream &is);

template<> O3D_API Bool SmartArray<Float>::writeToFile(OutStream &os) const;
template<> O3D_API Bool SmartArray<Float>::readFromFile(InStream &is);

template<> O3D_API Bool SmartArray<Double>::writeToFile(OutStream &os) const;
template<> O3D_API Bool SmartArray<Double>::readFromFile(InStream &is);

typedef SmartArray<Char>    SmartArrayChar;
typedef SmartArray<Int8>    SmartArrayInt8;
typedef SmartArray<UInt8>	SmartArrayUInt8;
typedef SmartArray<Int16>	SmartArrayInt16;
typedef SmartArray<UInt16>	SmartArrayUInt16;
typedef SmartArray<Int32>	SmartArrayInt32;
typedef SmartArray<UInt32>	SmartArrayUInt32;
typedef SmartArray<Float>	SmartArrayFloat;
typedef SmartArray<Double>	SmartArrayDouble;

} // namespace o3d

#endif // _O3D_SMARTARRAY_H
