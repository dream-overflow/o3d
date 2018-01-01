/**
 * @file lodstrategy.cpp
 * @brief Implementation of LodStrategy.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-02-16
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/lodstrategy.h"

#include <cmath>

using namespace o3d;

// Default constructor.
LodStrategy::LodStrategy() :
	m_bias(1.f),
	m_base(0.f)
{

}

// Virtual destructor.
LodStrategy::~LodStrategy()
{

}

// Get the LOD level for list of LOD level with distance.
UInt32 LodStrategy::getIndex(Float value, const std::vector<Float> &lodVector)
{
	UInt32 id = 0;
	UInt32 numIndex = static_cast<UInt32>(lodVector.size());

    for (id = 0; id < numIndex; ++id) {
        if ((id+1 < numIndex) && (lodVector[id+1] > value)) {
			break;
        }
	}

	UInt32 index = o3d::min<UInt32>(
			static_cast<UInt32>(lodVector.size()-1),
			static_cast<UInt32>(floor((m_base + m_bias * id) + 0.5)));

	return index;
}
