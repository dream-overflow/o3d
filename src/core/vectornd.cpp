/**
 * @file vectornd.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-07-2
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/vectornd.h"
#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"
#include "o3d/core/math.h"

#include <math.h>

using namespace o3d;

void VectorND::setValue(Float val)
{
    for (UInt32 i = 0 ; i < dim ; ++i)
		V[i] = val;
}

Float VectorND::length()const
{
	Float val = 0;

    for (UInt32 i = 0 ; i < dim ; ++i) {
		val += (V[i] * V[i]);
    }

    return Math::sqrt(val);
}
	
Float VectorND::squareLength()const
{
	Float val = 0;

    for (UInt32 i = 0 ; i < dim ; ++i) {
		val += (V[i] * V[i]);
    }

	return val;
}

Float VectorND::normInf()const
{
	Float val = 0;

	for (UInt32 i = 0 ; i < dim ; ++i)
	{
		if (V[i] > val)
			val = V[i];
		if (-V[i] > val)
			val = -V[i];
	}

	return val;
}

Float VectorND::normAbs()const
{
	Float val = 0;

	for (UInt32 i = 0 ; i < dim ; ++i)
		val += o3d::abs(V[i]);

	return val;
}

VectorND& VectorND::operator+= (const VectorND& vec)
{
	if (dim != vec.getDim()) return (*this);

	for (UInt32 i = 0 ; i < dim ; ++i)
		V[i] += vec.getData(i);

	return (*this);
}

VectorND& VectorND::operator-= (const VectorND& vec)
{
	if (dim != vec.getDim()) return (*this);

	for (UInt32 i = 0 ; i < dim ; ++i)
		V[i] -= vec.getData(i);

	return (*this);
}

VectorND& VectorND::operator*= (const Float scal)
{
	for (UInt32 i = 0 ; i < dim ; ++i)
		V[i] *= scal;

	return (*this);
}

VectorND& VectorND::operator/= (const Float scal)
{
	for (UInt32 i = 0 ; i < dim ; ++i)
		V[i] /= scal;

	return (*this);
}

Float VectorND::operator* (const VectorND& v)const
{
	if (dim != v.getDim()) return 0;

	Float val = 0;

	for (UInt32 i = 0 ; i < dim ; ++i)
		val += V[i] * v.getData(i);
    
	return val;
}

VectorND VectorND::operator* (const Float scal)const
{
	VectorND temp(dim);

	for (UInt32 i = 0 ; i < dim ; ++i)
		temp.setData(i,V[i]*scal);

	return temp;
}

VectorND VectorND::operator/ (const Float scal)const
{
	VectorND temp(dim);

	for (UInt32 i = 0 ; i < dim ; ++i)
		temp.setData(i,V[i]/scal);

	return temp;
}

VectorND VectorND::operator+ (const VectorND& v)const
{
	if (dim != v.getDim()) return VectorND(dim);

	VectorND temp(dim);

	for (UInt32 i = 0 ; i < dim ; ++i)
		temp.setData(i,V[i]+v.getData(i));

	return temp;
}

VectorND VectorND::operator- (const VectorND& v)const
{
	if (dim != v.getDim()) return VectorND(dim);

	VectorND temp(dim);

	for (UInt32 i = 0 ; i < dim ; ++i)
		temp.setData(i,V[i]-v.getData(i));

	return temp;
}

VectorND VectorND::operator- ()const
{
	VectorND temp(dim);

	for (UInt32 i = 0 ; i < dim ; ++i)
		temp.setData(i,-V[i]);

	return temp;
}

Bool VectorND::writeToFile(OutStream &os)const
{
    os << dim;

	for (UInt32 i = 0 ; i < dim ; ++i)
        os << V[i];

	return True;
}

Bool VectorND::readFromFile(InStream &is)
{
    is >> dim;
	reset(dim);

	for (UInt32 i = 0 ; i < dim ; ++i)
        is >> V[i];

	return True;
}
