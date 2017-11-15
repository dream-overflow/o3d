/**
 * @file lodstrategy.h
 * @brief Level of detail strategy
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-02-16
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_LODSTRATEGY_H
#define _O3D_LODSTRATEGY_H

#include "o3d/core/base.h"
#include <vector>

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class LodStrategy
//-------------------------------------------------------------------------------------
//! Level of detail strategy for any LOD level according to the distance.
//! You can inherit from this class and override the GetIndex method to make your own
//! LOD index computation.
//---------------------------------------------------------------------------------------
class O3D_API LodStrategy
{
public:

	//! Default constructor.
	//! Construct with a bias of 10.0 and and scale factor of 1.0.
	LodStrategy();

	//! Virtual destructor.
	virtual ~LodStrategy();

	//! Define the LOD bias (default is 1.f).
	//! This value is multiplied to the LOD value.
	inline void setBias(Float bias) { m_bias = bias; }
	//! Get the LOD bias (default is 1.f).
	inline Float getBias() const { return m_bias; }

	//! Define the LOD base value (default is 0.f).
	//! This value is add to the LOD value. This determine the highest LOD level.
	inline void setBaseValue(Float base) { m_base = base; }
	//! Get the LOD base value (default is 0.f).
	inline Float getBaseValue() const { return m_base; }

	//! Get the LOD level for list of LOD level with distance.
	//! @param value Value needed to compute the LOD index, can be the squared distance.
	//! @param lodVector A vector that define a value (can be distance) for each LOD index.
	//!        The values must be ordered from lesser to greater.
	//! @return A LOD level index between 0 and lodList::size()-1.
	virtual UInt32 getIndex(Float value, const std::vector<Float> &lodVector);

protected:

	Float m_bias;
	Float m_base;
};

} // namespace o3d

#endif // _O3D_LODSTRATEGY_H

