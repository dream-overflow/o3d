/**
 * @file vector3.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-12-12
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/vector3.h"

#include "o3d/core/matrix3.h"
#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"
#include "o3d/core/string.h"

using namespace o3d;

Vector3::operator String() const
{
	String temp;
	temp << String("{ ") << V[0] << String(", ") << V[1]<< String(", ") << V[2] << String(" }");
	return temp;
}

Matrix3 Vector3::crossProductMatrix() const
{
	return Matrix3( 0.f, -V[2], V[1],
					   V[2], 0.f, -V[0],
					  -V[1], V[0], 0.f);
}

Bool Vector3::writeToFile(OutStream &os) const
{
    os << V[0]
	     << V[1]
		 << V[2];

	return True;
}

Bool Vector3::readFromFile(InStream &is)
{
    is >> V[0]
	     >> V[1]
		 >> V[2];

	return True;
}
