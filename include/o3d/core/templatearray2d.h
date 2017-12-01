/**
 * @file templatearray2d.h
 * @brief Definition of a template 2D array.
 * @author Luthor
 * @date 2007-02-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEMPLATEARRAY2D_H
#define _O3D_TEMPLATEARRAY2D_H

#include "memorydbg.h"
#include "debug.h"

namespace o3d {

#ifdef _DEBUG
#define O3DARRAY2D_INDEX_DEBUBBING
#endif

//---------------------------------------------------------------------------------------
//! @class TemplateArray2D
//-------------------------------------------------------------------------------------
//! Definition of a template 2D array.
//---------------------------------------------------------------------------------------
template <class T>
class O3D_API_TEMPLATE TemplateArray2D
{
public:

	explicit TemplateArray2D(UInt32 _width = 0, UInt32 _height = 0, const T & _value = T());
	explicit TemplateArray2D(const T * _buffer, UInt32 _width, UInt32 _height);
	TemplateArray2D(const TemplateArray2D<T> & _which);

	virtual ~TemplateArray2D();

	//! Delete the memory allocated by the object
	void destroy();
	//! Idem. Old function. Must not be used any more.
	void free();

	TemplateArray2D<T> & operator = (const TemplateArray2D<T> & _which);

	inline T & operator () (UInt32 _i, UInt32 _j);
	inline const T & operator () (UInt32 _i, UInt32 _j) const;

	inline T & operator[] (UInt32 _index);
	inline T & operator[] (UInt32 _index) const;

	//! Opérateurs
	TemplateArray2D<T> & operator += (const TemplateArray2D<T> & _which);
	TemplateArray2D<T> & operator -= (const TemplateArray2D<T> & _which);
	T & operator /= (const T & _which);
	T & operator *= (const T & _which);

	inline void setSize(UInt32 _w, UInt32 _h);
	inline UInt32 width() const { return m_width; }
	inline UInt32 height() const { return m_height; }
	inline UInt32 elt() const { return m_width * m_height; }
	inline Bool isEmpty() const { return ((m_width == 0) || (m_height == 0)); }

	inline T * getData() { return m_pData; }
	inline const T * getData() const { return m_pData; }

	void fill(const T & _which);

	//! This function fill a subpart of the array with a value.
	//! The set   [_x1 ; _x2[ X [_y1 ; _y2[   will be modified.
	void fill(const T & _which, UInt32 _x1, UInt32 _y1, UInt32 _x2, UInt32 _y2);

	void fastFill(const T & _which);

	//! This function fill a subpart of the array with a value.
	//! The set   [_x1 ; _x2[ X [_y1 ; _y2[   will be modified.
	void fastFill(const T & _which, UInt32 _x1, UInt32 _y1, UInt32 _x2, UInt32 _y2);

	//! This function return a part of an array. The initial array is subdivided into _xdiv * y_div tiles, and
	//! the tile of coordinate (_i,_j) will be store in _target
	Bool getSubArray(UInt32 _xdiv, UInt32 _ydiv, TemplateArray2D<T> & _target, UInt32 _i, UInt32 _j) const;

	//! This function return a part of an array
	Bool getSubArray(Int32 _x1, Int32 _y1, Int32 _x2, Int32 _y2, TemplateArray2D<T> & _target) const;

	//! Function which insert an array at the specified coordinates.
	//! The target array won't be resized
	Bool insert(const TemplateArray2D<T> & _source, UInt32 _i, UInt32 _j);

	//! this function translates the array in the direction (x,y)
	//! Empty cell are then filled with the specified value
	void translate(Int32 _x, Int32 _y, const T & _value);

	//! For scalar type, this function is equivalent to the previous one.
	//! For other types, make sure your objects can be copied with memcpy.
	void fastTranslate(Int32 _x, Int32 _y, const T & _value);

protected:

	T* m_pData;

	UInt32 m_width;
	UInt32 m_height;
};

typedef TemplateArray2D<Int8>	Array2DInt8;
typedef TemplateArray2D<UInt8>	Array2DUInt8;
typedef TemplateArray2D<Int16>	Array2DInt16;
typedef TemplateArray2D<UInt16>	Array2DUInt16;
typedef TemplateArray2D<Int32>	Array2DInt32;
typedef TemplateArray2D<UInt32>	Array2DUInt32;
typedef TemplateArray2D<Float>	Array2DFloat;
typedef TemplateArray2D<Double>	Array2DDouble;

/*---------------------------------------------------------------------------------------
  IMPLEMENTATION : class TemplateArray2d
---------------------------------------------------------------------------------------*/
template <class T>
TemplateArray2D<T>::TemplateArray2D(UInt32 _width, UInt32 _height, const T & _value) :
    m_pData(nullptr),
	m_width(_width),
	m_height(_height)
{
	UInt32 size = m_width * m_height;

	if (size > 0)
		m_pData = new T[size];

	fill(_value);
}

template <class T>
TemplateArray2D<T>::TemplateArray2D(const T * _buffer, UInt32 _width, UInt32 _height) :
    m_pData(nullptr),
	m_width(_width),
	m_height(_height)
{
#ifdef _DEBUG
    if (_buffer == nullptr)
	{
		O3D_ASSERT((_width == 0) && (_height == 0));
	}
	else
	{
		O3D_ASSERT((_width > 0) && (_height > 0));
	}
#endif

    if (_buffer != nullptr)
	{
		UInt32 size = _width * _height;
		m_pData = new T[size];

		for (UInt32 k = 0 ; k < size; ++k)
			m_pData[k] = _buffer[k];
	}
}

template <class T>
TemplateArray2D<T>::TemplateArray2D(const TemplateArray2D<T> & _which) :
	m_width(_which.m_width),
	m_height(_which.m_height)
{
	UInt32 size = m_width * m_height;

	if (size > 0)
	{
		m_pData = new T[size];
		T * pTarget = m_pData;
		const T * pSource = _which.m_pData;

		UInt32 k = size;
		while (k-- > 0)
		{
			*pTarget = *pSource;
			++pTarget;
			++pSource;
		}
	}
	else
        m_pData = nullptr;
}

template <class T>
TemplateArray2D<T>::~TemplateArray2D()
{
	deleteArray(m_pData);
}

template <class T>
TemplateArray2D<T> & TemplateArray2D<T>::operator = (const TemplateArray2D<T> & _which)
{
	if (this != &_which)
	{
		deleteArray(m_pData);

		m_width = _which.m_width;
		m_height = _which.m_height;

		UInt32 size = m_width * m_height;

		if (size > 0)
		{
			m_pData = new T[m_width*m_height];
			T * pTarget = m_pData;
			const T * pSource = _which.m_pData;

			UInt32 k = size;
			while (k-- > 0)
			{
				*pTarget = *pSource;
				++pTarget;
				++pSource;
			}
		}
		else
            m_pData = nullptr;
	}

	return *this;
}

template <class T>
T & TemplateArray2D<T>::operator () (UInt32 _i, UInt32 _j)
{
	#ifdef O3DARRAY2D_INDEX_DEBUBBING
		O3D_ASSERT(_i < m_width);
		O3D_ASSERT(_j < m_height);
	#endif

	return m_pData[_i + _j * m_width];
}

template <class T>
const T & TemplateArray2D<T>::operator () (UInt32 _i, UInt32 _j) const
{
	#ifdef O3DARRAY2D_INDEX_DEBUBBING
		O3D_ASSERT(_i < m_width);
		O3D_ASSERT(_j < m_height);
	#endif

	return m_pData[(_i) + (_j) * m_width];
}

template <class T>
T & TemplateArray2D<T>::operator[] (UInt32 _index)
{
	#ifdef ARRAY2D_INDEX_DEBUBBING
		O3D_ASSERT(_index < m_width * m_height);
		O3D_ASSERT(_index >= 0);
	#endif

	return m_pData[_index];
}

template <class T>
T & TemplateArray2D<T>::operator[] (UInt32 _index) const
{
	#ifdef ARRAY2D_INDEX_DEBUBBING
		O3D_ASSERT(_index < m_width * m_height);
		O3D_ASSERT(_index >= 0);
	#endif

	return m_pData[_index];
}

template <class T>
void TemplateArray2D<T>::setSize(UInt32 _w, UInt32 _h)
{
	if ((m_width == _w) && (m_height == _h)) return;

	m_width = _w;
	m_height = _h;

	deleteArray(m_pData);

	if (_w * _h > 0)
		m_pData = new T[_w * _h];
}

template <class T>
void TemplateArray2D<T>::free()
{
	destroy();
}

template <class T>
void TemplateArray2D<T>::destroy()
{
	deleteArray(m_pData);

	m_width = 0;
	m_height = 0;
}

template <class T>
void TemplateArray2D<T>::fill(const T & _which)
{
    if (m_pData != nullptr)
	{
		T * pTarget = m_pData;

		UInt32 k = elt();
		while (k-- > 0)
		{
			*pTarget = _which;
			++pTarget;
		}
	}
}

template <class T>
void TemplateArray2D<T>::fill(const T & _which, UInt32 _x1, UInt32 _y1, UInt32 _x2, UInt32 _y2)
{
    if (m_pData != nullptr)
	{
		_x1 = o3d::min<Int32>(_x1, m_width);
		_y1 = o3d::min<Int32>(_y1, m_height);
		_x2 = o3d::min<Int32>(_x2, m_width);
		_y2 = o3d::min<Int32>(_y2, m_height);

		if ((_x2 <= _x1) || (_y2 <= _y1))
			return;

		const UInt32 lLineSize = _x2 - _x1;
        T * pTarget = nullptr;

		for (UInt32 j = _y1 ; j < _y2 ; ++j)
		{
			pTarget = &((*this)(_x1, j));

			for (UInt32 k = lLineSize ; k > 0 ; --k, ++pTarget)
				*pTarget = _which;
		}
	}
}

template <class T>
void TemplateArray2D<T>::fastFill(const T & _which)
{
    if (m_pData != nullptr)
	{
		T* pTarget = m_pData;

		UInt32 k = m_width;
		while (k-- > 0)
		{
			*pTarget = _which;
			++pTarget;
		}

		k = m_height - 1;
		while (k-- > 0)
		{
			memcpy((void*)pTarget, (const void*)m_pData, m_width * sizeof(T));
			pTarget += m_width;
		}
	}
}

template <class T>
void TemplateArray2D<T>::fastFill(const T & _which, UInt32 _x1, UInt32 _y1, UInt32 _x2, UInt32 _y2)
{
    if (m_pData != nullptr)
	{
		_x1 = o3d::min<Int32>(_x1, m_width);
		_y1 = o3d::min<Int32>(_y1, m_height);
		_x2 = o3d::min<Int32>(_x2, m_width);
		_y2 = o3d::min<Int32>(_y2, m_height);

		if ((_x2 <= _x1) || (_y2 <= _y1))
			return;

		T* const pSource = m_pData + _x1;
		T* pPtr = pSource;

		UInt32 k = _x2 - _x1;
		while (k-- > 0)
		{
			*pPtr = _which;
			++pPtr;
		}

		const Int32 lLineSize = (_x2 - _x1) * sizeof(T);

		pPtr = pSource + (_y1+1) * m_width;
		k = _y2 - _y1 - 1;
		while (k-- > 0)
		{
			memcpy((void*)pPtr, (const void*)pSource, lLineSize);
			pPtr += m_width;
		}
	}
}

//! This function return a part of an array. The initial array is subdivided into _xdiv * y_div tiles, and
//! the tile of coordinate (_i,_j) will be store in _target
template <class T>
Bool TemplateArray2D<T>::getSubArray(UInt32 _xdiv, UInt32 _ydiv, TemplateArray2D<T> & _target, UInt32 _i, UInt32 _j) const
{
	if (elt() == 0)
	{
		_target.free();
		return False;
	}

	// This current size does not allow the requested subdivision
	if ((m_width - 1) % _xdiv != 0) return False;
	if ((m_height - 1) % _ydiv != 0) return False;

	UInt32 sizex = (width() + _xdiv - 1)/_xdiv;
	UInt32 sizey = (height() + _ydiv - 1)/_ydiv;

	_target.setSize(sizex, sizey);

	for (UInt32 y = 0 ; y < sizey ; ++y)
		for (UInt32 x = 0 ; x < sizex ; ++x)
			_target(x,y) = this->operator ()(_i * (sizex-1) + x, _j * (sizey-1) + y);

	return True;
}

//! This function return a part of an array
template <class T>
Bool TemplateArray2D<T>::getSubArray(Int32 _x1, Int32 _y1, Int32 _x2, Int32 _y2, TemplateArray2D<T> & _target) const
{
	if (_x1 > _x2) return False;
	if (_y1 > _y2) return False;
	if (_x1 >= Int32(width())) return False;
	if (_y1 >= Int32(height())) return False;

	_target.setSize(_x2 - _x1 + 1, _y2 - _y1 + 1);

    T * lTargetPtr = nullptr;
    const T * lSourcePtr = nullptr;

	for (UInt32 y = 0 ; y < _target.height() ; ++y)
	{
		lTargetPtr = &_target(0, y);
		lSourcePtr = &(*this)(_x1, _y1 + y);

		for (UInt32 k = _target.width() ; k > 0 ; --k, ++lTargetPtr, ++lSourcePtr)
			*lTargetPtr = *lSourcePtr;
	}

	return True;
}

//! Function which insert an array at the specified coordinates.
//! The target array won't be resized
template <class T>
Bool TemplateArray2D<T>::insert(const TemplateArray2D<T> & _source, UInt32 _i, UInt32 _j)
{
	if (_i + _source.width() - 1 >= m_width) return False;
	if (_j + _source.height() - 1 >= m_height) return False;

    T * lTargetPtr = nullptr;
    const T * lSourcePtr = nullptr;

	for (UInt32 y = 0 ; y < _source.height() ; ++y)
	{
		lTargetPtr = &(*this)(_i, _j + y);
		lSourcePtr = &_source(0, y);

		for (UInt32 k = _source.width(); k > 0 ; --k, ++lTargetPtr, ++lSourcePtr)
			*lTargetPtr = *lSourcePtr;
	}

	return True;
}

//! this function translates the array in the direction (x,y)
//! Empty cell are then filled with the specified value
template <class T>
void TemplateArray2D<T>::translate(Int32 _x, Int32 _y, const T & _value)
{
	if ((UInt32(o3d::abs(_x)) >= m_width) || (UInt32(o3d::abs(_y)) >= m_height))
	{
		fill(_value);
		return ;
	}

	Int32 xStart, xEnd, yStart, yEnd;
	Int32 xFillStart, xFillEnd, yFillStart, yFillEnd;

	if (_x >= 0)
	{
		xStart = _x;
		xEnd = m_width;
		xFillStart = 0;
		xFillEnd = xStart;
	}
	else // <=> _x < 0
	{
		xStart = 0;
		xEnd = m_width + _x;
		xFillStart = xEnd;
		xFillEnd = m_width;
	}

	if (_y >= 0)
	{
		yStart = _y;
		yEnd = m_height;
		yFillStart = 0;
		yFillEnd = yStart;
	}
	else // <=> _y < 0
	{
		yStart = 0;
		yEnd = m_height + _y;
		yFillStart = yEnd;
		yFillEnd = m_height;
	}

	if (_y < 0)
	{
		if (_x < 0)
		{
			for (Int32 j = yStart ; j < yEnd ; ++j)
				for (Int32 i = xStart ; i < xEnd ; ++i)
				{
					O3D_ASSERT((i - _x < Int32(m_width)) && (j - _y < Int32(m_height)));
					(*this)(i,j) = (*this)(i - _x, j - _y);
				}
		}
		else
		{
			for (Int32 j = yStart ; j < yEnd ; ++j)
				for (Int32 i = xEnd-1 ; i >= xStart ; --i)
				{
					O3D_ASSERT((i - _x < Int32(m_width)) && (j - _y < Int32(m_height)));
					(*this)(i,j) = (*this)(i - _x, j - _y);
				}
		}
	}
	else
	{
		if (_x < 0)
		{
			for (Int32 j = yEnd-1 ; j >= yStart ; --j)
				for (Int32 i = xStart ; i < xEnd ; ++i)
				{
					O3D_ASSERT((i - _x < Int32(m_width)) && (j - _y < Int32(m_height)));
					(*this)(i,j) = (*this)(i - _x, j - _y);
				}
		}
		else
		{
			for (Int32 j = yEnd-1 ; j >= yStart ; --j)
				for (Int32 i = xEnd-1 ; i >= xStart ; --i)
				{
					O3D_ASSERT((i - _x < Int32(m_width)) && (j - _y < Int32(m_height)));
					(*this)(i,j) = (*this)(i - _x, j - _y);
				}
		}
	}


	// Now we must replace some cells
	for (Int32 j = 0; j < Int32(m_height) ; ++j)
		for (Int32 i = xFillStart ; i < xFillEnd ; ++i)
			(*this)(i,j) = _value;

	for (Int32 j = yFillStart; j < yFillEnd ; ++j)
		for (Int32 i = 0 ; i < Int32(m_width) ; ++i)
			(*this)(i,j) = _value;
}

//! For scalar type, this function is equivalent to the previous one.
//! For other types, make sure your objects can be copied with memcpy.
template <class T>
void TemplateArray2D<T>::fastTranslate(Int32 _x, Int32 _y, const T & _value)
{
	if ((UInt32(o3d::abs(_x)) >= m_width) || (UInt32(o3d::abs(_y)) >= m_height))
	{
		fill(_value);
		return ;
	}

	Int32 xStart, xEnd, yStart, yEnd;
	Int32 xFillStart, xFillEnd, yFillStart, yFillEnd;

	if (_x >= 0)
	{
		xStart = _x;
		xEnd = m_width - 1;
		xFillStart = 0;
		xFillEnd = xStart;
	}
	else // <=> _x < 0
	{
		xStart = 0;
		xEnd = m_width - 1 + _x;
		xFillStart = m_width + _x;
		xFillEnd = m_width;
	}

	if (_y >= 0)
	{
		yStart = _y;
		yEnd = m_height - 1;
		yFillStart = 0;
		yFillEnd = yStart;
	}
	else // <=> _y < 0
	{
		yStart = 0;
		yEnd = m_height - 1 + _y;
		yFillStart = m_height + _y;
		yFillEnd = m_height;
	}

	if (_y < 0)
	{
		T * pTarget = &m_pData[xStart + yStart * m_width];
		T * pSource = &m_pData[xStart - _x + (yStart - _y) * m_width];

		UInt32 size = (xEnd - xStart + 1) * sizeof(T);
		UInt32 k = yEnd - yStart + 1;

		while (k-- > 0)
		{
			memcpy((void*)pTarget, (const void*)pSource, size);
			pTarget += m_width;
			pSource += m_width;
		}
	}
	else if (_y > 0)
	{
		T * pTarget = &m_pData[xStart + yEnd * m_width];
		T * pSource = &m_pData[xStart - _x + (yEnd - _y) * m_width];

		UInt32 size = (xEnd - xStart + 1) * sizeof(T);
		UInt32 k = yEnd - yStart + 1;

		while (k-- > 0)
		{
			memcpy((void*)pTarget, (const void*)pSource, size);
			pTarget -= m_width;
			pSource -= m_width;
		}
	}
	else if (_y == 0)
	{
		T * pTarget = &m_pData[xStart];
		T * pSource = &m_pData[xStart - _x];

		UInt32 size = (xEnd - xStart + 1) * sizeof(T);
		UInt32 k = m_height;

		while (k-- > 0)
		{
			memmove((void*)pTarget, (const void*)pSource, size);
			pTarget += m_width;
			pSource += m_width;
		}

	}

	// Now we must replace some cells
	for (Int32 j = 0; j < Int32(m_height) ; ++j)
		for (Int32 i = xFillStart ; i < xFillEnd ; ++i)
			(*this)(i,j) = _value;

	for (Int32 j = yFillStart; j < yFillEnd ; ++j)
		for (Int32 i = 0 ; i < Int32(m_width) ; ++i)
			(*this)(i,j) = _value;
}

template <class T>
TemplateArray2D<T> & TemplateArray2D<T>::operator += (const TemplateArray2D<T> & _which)
{
	const UInt32 lWidth = o3d::min<UInt32>(width(), _which.width());
	const UInt32 lHeight = o3d::min<UInt32>(height(), _which.height());

	for (UInt32 y = 0 ; y < lHeight ; ++y)
	{
		const T * lSourcePtr = &_which(0, y);
		T * lTargetPtr = &(*this)(0, y);

		for (UInt32 x = lWidth ; x != 0 ; --x, ++lSourcePtr, ++lTargetPtr)
			*lTargetPtr += *lSourcePtr;
	}

	return *this;
}

template <class T>
TemplateArray2D<T> & TemplateArray2D<T>::operator -= (const TemplateArray2D<T> & _which)
{
	const UInt32 lWidth = o3d::min<UInt32>(width(), _which.width());
	const UInt32 lHeight = o3d::min<UInt32>(height(), _which.height());

	for (UInt32 y = 0 ; y < lHeight ; ++y)
	{
		const T * lSourcePtr = _which(0, y);
		T * lTargetPtr = (*this)(0, y);

		for (UInt32 x = lWidth ; x != 0 ; --x, ++lSourcePtr, ++lTargetPtr	)
			*lTargetPtr -= *lSourcePtr;
	}

	return *this;
}

template <class T>
T & TemplateArray2D<T>::operator /= (const T & _which)
{
	T * lPtr = m_pData;

	for (UInt32 k = width() * height() ; k != 0; --k, ++lPtr)
		*lPtr /= _which;
}

template <class T>
T & TemplateArray2D<T>::operator *= (const T & _which)
{
	T * lPtr = m_pData;

	for (UInt32 k = width() * height() ; k != 0; --k, ++lPtr)
		*lPtr *= _which;
}

} // namespace o3d

#endif // _O3D_TEMPLATEARRAY2D_H

