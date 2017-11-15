/**
 * @file runnable.h
 * @brief Interface for process execution.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-08-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_RUNNABLE_H
#define _O3D_RUNNABLE_H

#include "base.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Runnable
//-------------------------------------------------------------------------------------
//! Interface for process execution.
//---------------------------------------------------------------------------------------
class O3D_API Runnable
{
public:

    virtual ~Runnable() = 0;

	virtual Int32 run(void *) = 0;
};

}

#endif // _O3D_RUNNABLE_H
