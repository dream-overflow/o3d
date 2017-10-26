/**
 * @file templatearray.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEMPLATEARRAY_H
#define _O3D_TEMPLATEARRAY_H

#include "debug.h"
#include "memorydbg.h"

#ifdef _MSC_VER
	#pragma warning(disable:4127)
#endif

namespace o3d {

/**
 * @brief Dynamic array, for push/pop elements, delete, insert, replace, sort.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2004-10-12
 * Dynamic array, for push/pop elements, delete, insert, replace, sort.
 * This this class don't have allocator, items are directly accessible such as an array
 * or pointer. it can be used for value array (int,float...) but not for class data
 * whose need to be deleted (use vector for this).
 */
template <class T, Int32 Align = -1>
class O3D_API_TEMPLATE TemplateArray
{
public:

	//! @brief Default constructor.
	//! Data array is set to zero.
	TemplateArray(Int32 isize = 256, Int32 ithres = 256) :
        m_data(nullptr),
		m_default(T()),
		m_size(0),
		m_selected(-1)
	{
		m_maxsize = (isize > 0 ? isize : 0);
		m_threshold = (ithres > 0 ? ithres : 256);

		// allocate array
		if (m_maxsize > 0)
		{
			alloc();
			memset(m_data, 0, m_maxsize * sizeof(T));
		}
	}

	//! @brief Constructor with a default value specifier
	TemplateArray(const T& init, Int32 isize, Int32 ithres) :
        m_data(nullptr),
		m_default(init),
		m_size(0),
		m_selected(-1)
	{
		m_maxsize = (isize > 0 ? isize : 0);
		m_threshold = (ithres > 0 ? ithres : 256);

		// allocate array
		if (m_maxsize > 0)
		{
			alloc();

			// set default value
			for (Int32 i = 0 ; i < m_maxsize ; ++i)
			{
				m_data[i] = m_default;
			}
		}
	}

	//! @brief Constructor that fill the array with a given array
	TemplateArray(const T* init, Int32 size, Int32 ithres) :
        m_data(nullptr),
		m_default(T()),
		m_size(0),
		m_selected(-1)
	{
		m_maxsize = (size > 0 ? size : 0);
		m_threshold = (ithres > 0 ? ithres : 256);

		// allocate array
		if (m_maxsize > 0)
		{
			alloc();

			// fill with the initials values
			memcpy(m_data,init,size * sizeof(T));
		}
	}

	//! @brief Constructor that build from a string with the terminal \0
    //! @warning Works with the size of WChar
	TemplateArray(const String &init) :
        m_data(nullptr),
		m_default(T()),
		m_size(0),
		m_selected(-1)
	{
		// only for 1 byte element size
		O3D_ASSERT(sizeof(T) == sizeof(WChar));

		m_size = init.length();
		m_maxsize = m_size;
		m_threshold = 256;

		// allocate array
		if (m_maxsize > 0)
		{
			alloc();

			// fill with the initials values
			memcpy(m_data, init.getData(), m_size * sizeof(WChar));
		}
	}

	//! @brief Copy constructor
	//! @param dup The template array to duplicate
	TemplateArray(const TemplateArray& dup) :
        m_data(nullptr),
		m_default(dup.m_default),
		m_size(0),
		m_maxsize(0),
		m_threshold(dup.m_threshold),
		m_selected(-1)
	{
		if (dup.m_size != 0)
		{
			setSize(dup.m_size);
			memcpy(m_data,dup.m_data,m_size*sizeof(T));
			m_selected = dup.m_selected;
		}
	}

	//! @brief Destructor
	~TemplateArray() { freeAlloc(); }

	//! delete all elements
	void destroy()
	{
		setSize(m_threshold);

		m_selected = -1;
		m_size = 0;
	}

	//! Detach and return the data pointer.
    //! When detached this class contain a null data ptr,
	//! and the detached ptr will never be deleted.
	//! If the Alignment is not a default alignment aligned free would be used.
	inline T* detach()
	{
		T* result = m_data;
        m_data = nullptr;
		m_size = 0;
		m_selected = -1;
		m_maxsize = 0;

		return result;
	}

	//! duplication operator
	TemplateArray& operator=(const TemplateArray& pTemplateArray)
	{
		if (m_size) destroy();

		if (pTemplateArray.m_size == 0)
			return (*this);

		setSize(pTemplateArray.m_size);

		memcpy(m_data,pTemplateArray.m_data,m_size*sizeof(T));

		m_selected = pTemplateArray.m_selected;

		return (*this);
	}

	//! Get the number of elements.
	inline Int32 getSize() const { return m_size; }
	//! Get the number of elements.
	inline Int32 getNumElt() const { return m_size; }
	//! Get the array capacities (max size).
	inline Int32 getMaxSize() const { return m_maxsize; }
    //! Return the size of our stored data (size*sizeof(T)).
    inline Int32 getDataSize() const { return (m_size*sizeof(T)); }
	//! Get the memory alignment.
	//! @note -1 mean default alignment.
	inline Int32 getAlign() const { return Align; }

	//! Set default initial value.
	inline void setDefaultValue(const T& def_value) { m_default = def_value; }
	//! Get default initial value.
	inline const T& getDefaultValue() const { return m_default; }

	//! Get the element at a given index.
	inline T& operator[] (Int32 n)
	{
		if (n >= m_size)
		{
			setSize(n+1);
			m_selected = n;
		}
		return m_data[n];
	}

	//! Get the element at a given index (read only).
	inline const T& operator[] (Int32 n) const
	{
		if (n >= m_size) O3D_ERROR(E_IndexOutOfRange(""));
		return m_data[n];
	}

	//! Returns the pointer to the beginning of the data array.
	inline T* getData()	{ return m_data; }
	//! Returns the pointer to the beginning of the data array (read only).
	inline const T* getData() const { return m_data; }

	//! Get the element at a given index.
	inline T& get(Int32 n)
	{
		if (n >= m_size)
		{
			setSize(n+1);
			m_selected = n;
		}
		return m_data[n];
	}

	//! Get the element at a given index (read only).
	inline const T& get(Int32 n) const
	{
		if (n >= m_size) O3D_ERROR(E_IndexOutOfRange(""));
		return m_data[n];
	}

	//! Get the last element.
	inline T& getLast()
	{
		if (m_size == 0)
			O3D_ERROR(E_InvalidPrecondition("Array is empty"));

		return m_data[m_size-1];
	}
	//! Get the last element (read only).
	inline const T& getLast() const
	{
		if (m_size == 0)
			O3D_ERROR(E_InvalidPrecondition("Array is empty"));

		return m_data[m_size-1];
	}

	//! Select an element by its index.
	inline void select(Int32 n)
	{
		if (n < 0 || n >= m_size)
			O3D_ERROR(E_IndexOutOfRange(""));

		m_selected = n;
	}

	//! Get the selected element.
	inline T& getSelected()
	{
		if (m_selected < 0)
			O3D_ERROR(E_IndexOutOfRange(""));

		return m_data[m_selected];
	}
	//! Get the selected element (read only).
	inline const T& getSelected() const
	{
		if (m_selected < 0)
			O3D_ERROR(E_IndexOutOfRange(""));

		return m_data[m_selected];
	}

	//! Select the first element of a specified value.
	//! @return TRUE if an occurrence is found and selected, FALSE otherwise.
	inline Bool select(const T& t)
	{
		Int32 i = find(t);
		if (i == -1)
			return False;

		m_selected = i;
		return True;
	}

	//! Select the previous element.
	inline Bool selectPrevious()
	{
		--m_selected;
		if (m_selected < 0)
		{
			m_selected = -1;
			return False;
		}
		return True;
	}

	//! Select the next element.
	inline Bool selectNext()
	{
		++m_selected;
		if (m_selected >= m_size)
		{
			m_selected = -1;
			return False;
		}
		return True;
	}

	//! Select the last element.
	inline Bool selectLast()
	{
		if (m_size == 0)
		{
			m_selected = -1;
			return False;
		}
		m_selected = m_size - 1;
		return True;
	}

	//! Get selected element index.
	inline Int32 getSelectedNum() const { return m_selected; }

	//! Delete an element given its index
	Bool remove(Int32 n)
	{
		if (n < m_size)
		{
			if ((m_size-1-n) > 0)
			{
				memmove(&m_data[n],&m_data[n+1],(m_size-1-n) * sizeof(T));
				m_data[m_size-1] = m_default;
			}
			setSize(m_size-1);

			if (n == m_selected) m_selected = -1;

			return True;
		}
		return False;
	}

	//! Remove the first occurrence of an elements given its value.
	Bool remove(const T& t)
	{
		Int32 found = find(t);

		if (found != -1)
			return remove();

		return False;
	}

	//! Delete selected element.
	inline Bool remove()
	{
		if (m_selected < 0) return False;
		return remove(m_selected);
	}

	//! Allocate the maxsize base.
	void allocate(Int32 n)
	{
		setSize(n);
		m_size = 0;
	}

	//! Grow the empty space size (not change its size).
	//! @param n the minimal empty space desired (the result can be more, depend of
	//!        threshold and current maxsize)
	void growMaxSize(Int32 n)
	{
		if (n > m_maxsize)
		{
			n += m_maxsize;
			n = ((n + m_threshold - 1) / m_threshold) * m_threshold;
			reAlloc(n);
			m_maxsize = n;
		}
	}

	//! Resize the array.
	void setSize(Int32 n)
	{
		Int32 oldsize = m_maxsize;

		if (n > m_maxsize || (m_maxsize > m_threshold && n < m_maxsize - m_threshold))
		{
			Int32 newMaxSize = ((n + m_threshold - 1) / m_threshold) * m_threshold;
			if (!newMaxSize)
			{
				freeAlloc();
				return;
			}
			else
			{
				reAlloc(newMaxSize);

				if (newMaxSize > oldsize)
				{
					// set default value
					for (Int32 i = oldsize ; i < newMaxSize ; ++i)
					{
						m_data[i] = m_default;
					}
				}
			}

			m_maxsize = newMaxSize;
		}

		// set the new used size
		m_size = n;
	}

	//! Force the size of the array.
	inline void forceSize(Int32 n)
	{
		O3D_ASSERT(n <= m_maxsize);
		m_size = n;
	}

	//! Force the use maxsize of the array. Reduce size if maxsize < size.
	inline void forceMaxSize(Int32 n)
	{
		if (n != m_maxsize)
		{
			reAlloc(n);
			if (m_size > n)
				m_size = n;
			m_maxsize = n;
		}
	}

	//! Push an element at the top of the stack and return its index.
	inline Int32 push(const T& t)
	{
		setSize(m_size+1);

		m_data[m_size-1] = t;

		m_selected = m_size - 1;
		return m_selected;
	}

	//! Pop an element from the top of the stack.
	inline void pop()
	{
		if (m_selected == m_size - 1)
		    m_selected = -1;

		setSize(m_size-1);
	}

	//! Push an array of element at back.
	inline void pushArray(const T* _array, UInt32 size)
	{
        O3D_ASSERT(_array != nullptr);

		if (_array)
		{
			setSize(m_size+size);
			memcpy(&m_data[m_size-size],_array,size * sizeof(T));
		}
	}

	//! insert an array of element at a specified position.
	//! @param _array The array to insert
	//! @param size the size (in elements, not bytes) of the array to insert
	//! @param pos The position where to insert these data
	inline void insertArray(const T* _array, UInt32 size, UInt32 pos)
	{
        O3D_ASSERT(_array != nullptr);

		if (_array)
		{
			if (pos+size > (UInt32)m_size)
				setSize(pos+size);

			memcpy(&m_data[pos],_array,size * sizeof(T));
		}
	}

	//! Add a unique value (only if not already added), and return its index (-1 otherwise).
	Int32 addUnique(const T& t)
	{
		Int32 found = find(t);

		if (found == -1)
			return push(t);

		return -1;
	}

	//! insert an element at n.
	Bool insert(Int32 n, const T& t)
	{
		if (n > m_size)
			return False;

		setSize(m_size+1);

		if ((m_size-n-1) > 0)
			memmove(&m_data[n+1],&m_data[n],(m_size-n-1) * sizeof(T));

		m_data[n] = t;
		m_selected = n;

		return True;
	}

	//! Swap two elements at n1 and n2.
	inline void exchange(Int32 n1, Int32 n2)
	{
		T tmp = m_data[n1];
		m_data[n1] = m_data[n2];
		m_data[n2] = tmp;
	}

	//! Find the first occurrence of an element.
	//! @return Index of the element or -1 if not found.
	Int32 find(const T& t) const
	{
		for (Int32 i = 0 ; i < m_size ; ++i)
			if (m_data[i] == t)
				return i;
		return -1;
	}

	//! Is selected element is empty.
	inline Bool isSelectedEmpty() const
	{
		if ((m_selected >= 0) && (m_selected < m_size))
			return True;
		return False;
	}

	//! Empty array.
	inline Bool isEmpty() const { return m_size == 0; }
	//! Is valid array (non null).
	inline Bool isValid() const { return m_data && m_size > 0; }
	//! Is null array.
	inline Bool isNull() const { return !m_data; }

	//! Replace an item data at n by t.
	inline Bool replace(Int32 n, const T& t)
	{
		if (n < m_size)
		{
			m_data[n] = t;
			return True;
		}
		return False;
	}

	//! Partition used for QuickSort.
	inline Int32 partition(Int32 p, Int32 r)
	{
#if 0
		T t = m_data[p];
		Int32 i = p - 1;
		Int32 j = r + 1;

		while (1)
		{
			do --j; while(m_data[j] > t);
			do ++i; while(m_data[i] < t);

			if (i < j) exchange(i,j);
			else return j;
		}
#else
		// Fix the pivot
		Int32 m = p + (r - p) / 2;
		if (m_data[p] > m_data[r])
			exchange(p, r);
		if (m_data[p] > m_data[m])
			exchange(p, m);
		else if (m_data[m] > m_data[r])
			exchange(m, r);

		Int32 i = p;
		Int32 j = r;

		while (i < j)
		{
			while (i < m && m_data[i] <= m_data[m]) ++i;
			while (j > m && m_data[j] >= m_data[m]) --j;

			if (i < j)
			{
				exchange(i, j);
				if (i == m)
					m = j;
				else if (j == m)
					m = i;
			}
		}
		
		return m;
	}
#endif

	//! Sort sub-arrays [p..r] using quick-sort.
	void quickSort(Int32 p, Int32 r)
	{
	#if 0
		if (p < r)
		{
			Int32 q = partition(p, r);
			quickSort(p, q);
			quickSort(q+1, r);
		}
	#else
		// direct sorting
		if (r - p < 2)
		{
			// already sorted
			if (r - p < 1)
				return;
			if (m_data[p] > m_data[r])
				exchange(p, r);
			return;
		}

		Int32 q = partition(p, r);
		quickSort(p, q-1);
		quickSort(q+1, r);
	#endif
	}

	//! Sort entirely the array using quick-sort.
	inline void sort() { quickSort(0, m_size-1); }

	//! find an element index in array in O(log2(n)) if data array is sorted
	Int32 findSorted(const T& t)const
	{
		Int32 l = 0;
		Int32 r = m_size - 1;

		while (l <= r)
		{
			Int32 m = (l + r) / 2;

			if (m_data[m] == t)
				return m;
			else if (m_data[m] < t)
				l = m + 1;
			else
				r = m - 1;
		}
		return -1;
	}

	//! insert an element (t1,t2) if data array is sorted (in O(log2(n)) )
	Int32 insertSorted(const T& t)
	{
		if (m_size==0)
		{
			insert(0,t);
			return 0;
		}

		Int32 l = 0,m = 0;
		Int32 r = m_size - 1;

		while (l <= r)
		{
			m = (l + r) / 2;

			if (m_data[m] == t)
			{
				insert(++m,t);
				return m;
			}
			else if (m_data[m] < t)
				l = m + 1;
			else
				r = m - 1;
		}
		if (r == m) ++m;
		insert(m,t);
		return m;
	}

private:

	inline void alloc()
	{
		if (!m_maxsize)
			return;

		if (Align == -1)
		{
			m_data = (T*)O3D_MALLOC((size_t)m_maxsize * sizeof(T));
			if (!m_data)
				O3D_ERROR(E_InvalidAllocation(""));
		}
		else
		{
			m_data = (T*)O3D_ALIGNED_MALLOC((size_t)m_maxsize * sizeof(T), (size_t)Align);
			if (!m_data)
				O3D_ERROR(E_InvalidAllocation(""));

			if ((size_t)(m_data) & (Align-1))
				O3D_ERROR(E_InvalidAllocation("Data is not aligned"));
		}
	}

	inline void reAlloc(size_t n)
	{
		if (!n)
		{
			freeAlloc();
			return;
		}

		if (Align == -1)
		{
			m_data = (T*)O3D_REALLOC(m_data, n * sizeof(T));
			if (!m_data)
				O3D_ERROR(E_InvalidAllocation(""));
		}
		else
		{
			m_data = (T*)O3D_ALIGNED_REALLOC(m_data, n * sizeof(T), (size_t)Align);
			if (!m_data)
				O3D_ERROR(E_InvalidAllocation(""));

			if ((size_t)(m_data) & (Align-1))
				O3D_ERROR(E_InvalidAllocation("Data is not aligned"));
		}
	}

	inline void freeAlloc()
	{
		if (m_data)
		{
			if (Align == -1)
				O3D_FREE(m_data);
			else
				O3D_ALIGNED_FREE(m_data);

            m_data = nullptr;
		}
		m_maxsize = 0;
	}

protected:

    T* m_data;               //!< Elements array.
    T m_default;             //!< Initial value.
    Int32 m_size,m_maxsize;  //!< Current element size / array size.
    Int32 m_threshold;       //!< Threshold size for arrays.

    Int32 m_selected;        //!< Selected element.
};

//
// Usually typed arrays
//

typedef TemplateArray<Char>   ArrayChar;   //!< 8 bits char array.
typedef TemplateArray<Int8>   ArrayInt8;   //!< 8 bits integer array.
typedef TemplateArray<UInt8>  ArrayUInt8;  //!< 8 bits unsigned integer array.
typedef TemplateArray<Int16>  ArrayInt16;  //!< 16 bits integer array.
typedef TemplateArray<UInt16> ArrayUInt16; //!< 16 bits unsigned integer array.
typedef TemplateArray<Int32>  ArrayInt32;  //!< 32 bits integer array.
typedef TemplateArray<UInt32> ArrayUInt32; //!< 32 bits unsigned integer array.
typedef TemplateArray<Float>  ArrayFloat;  //!< 32 bits float array.
typedef TemplateArray<Double> ArrayDouble; //!< 64 bits double array.

} // namespace o3d

#ifdef _MSC_VER
	#pragma warning(default:4127)
#endif

#endif // _O3D_TEMPLATEARRAY_H

