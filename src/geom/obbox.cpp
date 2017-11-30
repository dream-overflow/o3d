/**
 * @file obbox.cpp
 * @brief Implementation of OBBox.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-07-29
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/geom/obbox.h"
#include "o3d/geom/plane.h"
#include "o3d/core/debug.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

using namespace o3d;

//---------------------------------------------------------------------------------------
// class OBBox
//---------------------------------------------------------------------------------------

// Build the equivalent set of planes (left, right, bottom, top, front, back).
void OBBox::toPlanes(Plane planes[6]) const
{
	// TODO
}

// Serialization
Bool OBBox::writeToFile(OutStream &os) const
{
    os	<< m_data
        << m_center
        << m_halfSize;

	return True;
}

Bool OBBox::readFromFile(InStream &is)
{
    is	>> m_data
        >> m_center
        >> m_halfSize;

	return True;
}

//---------------------------------------------------------------------------------------
// class OBBoxExt
//---------------------------------------------------------------------------------------

// Serialization
Bool OBBoxExt::writeToFile(OutStream &os) const
{
    OBBox::writeToFile(os);
	return True;
}

Bool OBBoxExt::readFromFile(InStream &is)
{
    OBBox::readFromFile(is);
	updateRadius();
	return True;
}
