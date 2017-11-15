/**
 * @file multieffect.h
 * @brief Multi-Effect interface.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-08-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MULTIEFFECT_H
#define _O3D_MULTIEFFECT_H

#include "specialeffects.h"
#include "o3d/core/memorydbg.h"
#include <list>

namespace o3d {

typedef std::list<SmartObject<SpecialEffects> > T_SpecialEffectsList;
typedef T_SpecialEffectsList::iterator IT_SpecialEffectsList;
typedef T_SpecialEffectsList::const_iterator CIT_SpecialEffectsList;

//---------------------------------------------------------------------------------------
//! @class MultiEffect
//-------------------------------------------------------------------------------------
//! Use this interface if you have effects composed of more than one effect.
//! For example, a lens effect is composed of :
//!  - more than one glow effect
//!  - a ray effect
//!  - etc
//---------------------------------------------------------------------------------------
class O3D_API MultiEffect
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(MultiEffect)

	//! destructor
	virtual ~MultiEffect();

	//! remove all elements
	void removeAll();

	//! add/remove an effect
	void addEffect(SpecialEffects* pEffect);
	void removeEffect(SpecialEffects* pEffect);

	//! get num of effects
	inline Int32 getNumEffects() const { return (Int32)m_effectlist.size(); }

	//! get effects list
	const T_SpecialEffectsList& getEffectList() const { return m_effectlist; }
	T_SpecialEffectsList& getEffectList() { return m_effectlist; }

	//! draw the effect (ex. particle, lens-flare)
	virtual void drawAll(const DrawInfo &drawInfo);
	//! update the effect (ex. particle, lens-flare)
	virtual void updateAll();

protected:

	T_SpecialEffectsList m_effectlist;
};

} // namespace o3d

#endif // _O3D_MULTIEFFECT_H

