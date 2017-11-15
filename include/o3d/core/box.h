/**
 * @file box.h
 * @brief Two dimensional bounding box
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-01-21
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_BOX_H
#define _O3D_BOX_H

#include "vector2.h"
#include "string.h"
#include "memorydbg.h"

namespace o3d {

template <class TYPE> class Box;

// Type definitions
typedef Box<Double>	Box2d;
typedef Box<Float>	Box2f;
typedef Box<Int32>	Box2i;
typedef Box<UInt32>	Box2ui;
typedef Box<Int16>	Box2s;
typedef Box<UInt16>	Box2us;
typedef Box<Int8>	Box2c;
typedef Box<UInt8>	Box2uc;

class InStream;
class OutStream;

//---------------------------------------------------------------------------------------
//! @class Box
//-------------------------------------------------------------------------------------
//! Template class of a two dimensional bounding box. Width and height must be greater
//! or equal to zero. TODO epsilon for float and double specializations
//---------------------------------------------------------------------------------------
template <class TYPE>
class O3D_API_TEMPLATE Box
{
public:

	//! default constructor
	Box() {}

	//! initialisation constructor
	Box(TYPE x, TYPE y, TYPE w, TYPE h) :
		P(x,y),
		S(w,h)
	{
		O3D_ASSERT(w >= 0 && h >= 0);
	}

	//! initialisation constructor
	Box(const Vector2<TYPE> &pos, const Vector2<TYPE> &size) :
		P(pos),
		S(size)
	{
		O3D_ASSERT(size.x() >= 0 && size.y() >= 0);
	}

	//! construct from another template box
	template <class U>
	Box(const Box<U> &box)
	{
		P = static_cast<TYPE>(box.P);
		S = static_cast<TYPE>(box.S);
	}

	//! construct from a ptr
	Box(const TYPE *_ptr)
	{
		O3D_ASSERT(_ptr);
		O3D_ASSERT(_ptr[2] >= 0 && _ptr[3] >= 0);

		P.set(_ptr[0], _ptr[1]);
		S.set(_ptr[2], _ptr[3]);
	}

	//! copy constructor
	Box(const Box &box)
	{
		P = box.P;
		S = box.S;
	}

	//! define the box
	inline void set(TYPE x, TYPE y, TYPE w, TYPE h)
	{
		O3D_ASSERT(w >= 0 && h >= 0);

		P.set(x,y);
		S.set(w,h);
	}

	//! define the box
	inline void set(const Vector2<TYPE> &pos, const Vector2<TYPE> &size)
	{
		O3D_ASSERT(size.x() >= 0 && size.y() >= 0);

		P = pos;
		S = size;
	}

	//! define the box from a ptr [4]
	inline void set(const TYPE *_ptr)
	{
		O3D_ASSERT(_ptr);
		O3D_ASSERT(_ptr[2] >= 0 && _ptr[3] >= 0);

		P.set(_ptr[0], _ptr[1]);
		S.set(_ptr[2], _ptr[3]);
	}

	//! set the position of the box
	inline void setPos(const Vector2<TYPE> &pos) { P = pos; }
	//! set the size of the box
	inline void setSize(const Vector2<TYPE> &size) { S = size; }

	//! read the box content
	inline void read(TYPE &x, TYPE &y, TYPE &w, TYPE &h) const
	{
		x = P.x();
		y = P.y();
		w = S.x();
		h = S.y();
	}

	//! read the box content
	inline void read(Vector2<TYPE> &pos, Vector2<TYPE> &size) const
	{
		pos = P;
		size = S;
	}

	//! get the position of the box
	inline const Vector2<TYPE>& pos() const { return P; }
	//! get the size of the box
	inline const Vector2<TYPE>& size() const { return S; }

	//! Get the X component (const).
	inline const TYPE& x() const { return P.x(); }
	//! Get the X ref component.
	inline TYPE& x() { return P.x(); }
	//! Get the Y component (const).
	inline const TYPE& y() const { return P.y(); }
	//! Get the Y ref component.
	inline TYPE& y() { return P.y(); }

	//! Get the width (const).
	inline const TYPE& width() const { return S.x(); }
	//! Get the width ref.
	inline TYPE& width() { return S.x(); }
	//! Get the height (const).
	inline const TYPE& height() const { return S.y(); }
	//! Get the height ref.
	inline TYPE& height() { return S.y(); }

    //! Get the x' (x + width).
    inline TYPE x2() const { return P.x() + S.x(); }
    //! Get the y' (y + height).
    inline TYPE y2() const { return P.y() + S.y(); }

	//! duplicate
	inline Box<TYPE>& operator= (const Box<TYPE> & box)
	{
		P = box.P;
		S = box.S;

		return *this;
	}

	//! set the vector from another template 2d vector
	template <class U>
	inline Box<TYPE>& operator= (const Box<U> & box)
	{
		P = static_cast<TYPE>(box.P);
		S = static_cast<TYPE>(box.S);

		return *this;
	}

	//! compare this to another given 2d box
	inline Bool operator== (const Box& box) const { return ((P == box.P) && (S == box.S)); }
	inline Bool operator!= (const Box& box) const { return ((P != box.P) || (P != box.P)); }

	//! include a box into this box (see operators + and += )
	inline void include(const Box &box)
	{
		P[X] = o3d::min(P[X], box.P[X]);
		P[Y] = o3d::min(P[Y], box.P[Y]);

        S[X] = o3d::max(P[X]+S[X], box.P[X]+box.S[X]) - P[X];
        S[Y] = o3d::max(P[Y]+S[Y], box.P[Y]+box.S[Y]) - P[Y];
	}

	//! include a box into this box (see operators + and Include )
	inline Box& operator+= (const Box& box)
	{
		include(box);
		return *this;
	}

	//! include a box with this box and return the new box (see operators += and Include )
	inline Box operator+ (const Box& box) const
	{
		Box result(*this);
		result.include(box);
		return result;
	}

	//! exclude a box from this box (@see operators - and -= )
	//! @note exclude only if possible
	inline void exclude(const Box &box)
	{
		O3D_ASSERT(0); // TODO
	}

	//! exclude a box into this box (see operators - and Exclude )
	//! @note exclude only if possible
	inline Box& operator-= (const Box& box)
	{
		exclude(box);
		return *this;
	}

	//! exclude a box with this box and return the new box (see operators -= and Exclude )
	//! @note exclude only if possible
	inline Box operator- (const Box& box) const
	{
		Box result(*this);
		result.exclude(box);
		return result;
	}

	//! scale the position and size of the box and return as a new box
	inline Box operator* (TYPE scale) const
	{
		O3D_ASSERT(scale > 0);
		return Box(P * scale, S * scale);
	}

	//! scale the position and size of the box and return it
	inline Box& operator*= (TYPE scale)
	{
		O3D_ASSERT(scale > 0);

		P *= scale;
		S *= scale;

		return *this;
	}

	//! divide this by a scalar and return as a new box
	inline Box operator/ (TYPE scale) const
	{
		O3D_ASSERT(scale > 0);
		return Box(P / scale, S / scale);
	}

	//! divide the position and size of the box and return it
	inline Box& operator/= (TYPE scale)
	{
		O3D_ASSERT(scale > 0);

		P /= scale;
		S /= scale;

		return *this;
	}

	//! set the box components to zero
	inline void zero()
	{
		P.zero();
		S.zero();
	}

	//! Check if a box is inside this box
	inline Bool isInside(const Box &box) const
	{
		if ((box.P[X] < P[X]) || (box.P[Y] < P[Y]) ||
			(box.P[X]+box.S[X] > P[X]+S[X]) || (box.P[Y]+box.S[Y] > P[Y]+S[Y]))
			return False;

		return True;
	}

	//! Check if the point is inside this box
	inline Bool isInside(const Vector2<TYPE> &point) const
	{
		if ((point[X] < P[X]) || (point[Y] < P[Y]) ||
            (point[X] > P[X]+S[X]) || (point[Y] > P[Y]+S[Y]))
			return False;

		return True;
	}

	//! Check if this point is include onto a border of this box
	inline Bool isOnBorder(const Vector2<TYPE> &point) const
	{
		return ((point[X] == P[X]) || (point[Y] == P[Y]) ||
                (point[X] == P[X]+S[X]) || (point[Y] == P[Y]+S[Y]));
	}

	//! Check if the box intersect with this box
	inline Bool isIntersect(const Box &box) const
	{
//		return !((P[X] > box.P[X]+box.S[X]) || (P[X]+S[X] > box.P[X]) ||
//				 (P[Y] > box.P[Y]+box.S[Y]) || (P[Y]+S[Y] > box.P[Y]));
        return !((P[X] > box.P[X]+box.S[X]) || (P[X]+S[X] < box.P[X]) ||
                 (P[Y] > box.P[Y]+box.S[Y]) || (P[Y]+S[Y] < box.P[Y]));
	}

	//! Clamp a vector into the box area, and return the new clamped vector.
	inline Vector2<TYPE> clamp(const Vector2<TYPE> &point) const
	{
		Vector2<TYPE> result;

		result.x() = o3d::max<TYPE>(point.x(), P.x());
		result.y() = o3d::max<TYPE>(point.y(), P.y());

		result.x() = o3d::min<TYPE>(result.x(), x2());
		result.y() = o3d::min<TYPE>(result.y(), y2());

		return result;
	}

	//! Clamp a box into the box area, and return the new clamped box.
	inline Box<TYPE> clamp(const Box<TYPE> &box) const
	{
		Vector2<TYPE> p1 = this->clamp(box.pos());
		Vector2<TYPE> p2 = this->clamp(Vector2<TYPE>(box.x2(), box.y2()));

		return Box<TYPE>(p1, p2-p1);
	}

	//! convert to a string
	inline operator String() const
	{
		String temp;
		temp << String("{ ") << P[X] << String(", ") << P[Y] << String(" | ") <<
								S[X] << String(", ") << S[Y] << String(" }");
		return temp;
	}

	// Serialization
	inline Bool readFromFile(InStream &is)
	{
        is   >> P[X]
			 >> P[Y]
			 >> S[X]
			 >> S[Y];

		return True;
	}

	inline Bool writeToFile(OutStream &os) const
	{
        os   << P[X]
			 << P[Y]
			 << S[X]
			 << S[Y];

		return True;
	}

private:

	Vector2<TYPE> P;  //!< position
	Vector2<TYPE> S;  //!< size
};

} // namespace o3d

#endif // _O3D_BOX_H

