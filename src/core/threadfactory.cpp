/**
 * @file threadfactory.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/threadfactory.h"

using namespace o3d;

ThreadFactory::~ThreadFactory()
{

}

DefaultThreadFactory::~DefaultThreadFactory()
{

}

Thread *DefaultThreadFactory::createThread(Runnable *runnable)
{
    return new Thread(runnable);
}

