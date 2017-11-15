/**
 * @file deflectorcollider.h
 * @brief Deflector Collider.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-06-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DEFLECTORCOLLIDER_H
#define _O3D_DEFLECTORCOLLIDER_H

#include "o3d/core/memorydbg.h"
#include "abccollider.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class DeflectorCollider
//--------------------------------------------------------------------------------------
//! Deflector Collider
//---------------------------------------------------------------------------------------
class O3D_API DeflectorCollider : public ABCCollider
{
public:

	//! default constructor
	DeflectorCollider();
	//! copy constructor
	DeflectorCollider(const DeflectorCollider& dup);
	//! destructor
	virtual ~DeflectorCollider();

protected:


};

} // namespace o3d

#endif // _O3D_DEFLECTORCOLLIDER_H

