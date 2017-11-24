/**
 * @file threadfactory.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_THREADFACTORY_H
#define _O3D_THREADFACTORY_H

#include "thread.h"

namespace o3d {

/**
 * @brief ThreadFactory A simple thread factory.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-28
 */
class O3D_API ThreadFactory
{
public:

    virtual ~ThreadFactory() = 0;

    virtual Thread *createThread(Runnable *runnable) = 0;
};

/**
 * @brief DefaultThreadFactory A simple thread factory.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-28
 */
class O3D_API DefaultThreadFactory
{
public:

    virtual ~DefaultThreadFactory();

    virtual Thread *createThread(Runnable *runnable);
};

} // namespace o3d

#endif // _O3D_THREADFACTORY_H
