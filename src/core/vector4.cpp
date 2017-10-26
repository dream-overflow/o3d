/**
 * @file vector4.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2003-07-21
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/vector4.h"

#include "o3d/core/vector3.h"
#include "o3d/core/vector4.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

using namespace o3d;

// Initialize from a vector3 and a w component.
Vector4::Vector4(const Vector3 &vec3, Float w)
{
	V[X] = vec3[X];
	V[Y] = vec3[Y];
	V[Z] = vec3[Z];
	V[W] = w;
}

// Convert to a string
Vector4::operator String()const
{
	String temp;
	temp << String("{ ") << V[0] << String(", ") << V[1]<< String(", ") << V[2] << String(", ") << V[3] << String(" }");
	return temp;
}

/*---------------------------------------------------------------------------------------
  Serialization
---------------------------------------------------------------------------------------*/
Bool Vector4::writeToFile(OutStream &os)const
{
    os   << V[0]
	     << V[1]
		 << V[2]
		 << V[3];

	return True;
}

Bool Vector4::readFromFile(InStream &is)
{
    is   >> V[0]
	     >> V[1]
		 >> V[2]
		 >> V[3];

	return True;
}

