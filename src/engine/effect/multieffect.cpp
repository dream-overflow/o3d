/**
 * @file multieffect.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2002-08-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/effect/multieffect.h"

#include "o3d/core/debug.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS(MultiEffect, ENGINE_EFFECT_MULTI)

/*---------------------------------------------------------------------------------------
  destructor
---------------------------------------------------------------------------------------*/
MultiEffect::~MultiEffect()
{
	removeAll();
}

/*---------------------------------------------------------------------------------------
  remove all elements
---------------------------------------------------------------------------------------*/
void MultiEffect::removeAll()
{
	m_effectlist.clear();
}

/*---------------------------------------------------------------------------------------
  add/remove an effect
---------------------------------------------------------------------------------------*/
void MultiEffect::addEffect(SpecialEffects* pEffect)
{
	if (pEffect)
		m_effectlist.push_back(SmartObject<SpecialEffects>((BaseObject*)this, pEffect));
}

void MultiEffect::removeEffect(SpecialEffects* pEffect)
{
	if (pEffect)
		m_effectlist.remove(pEffect);
}

// draw the effect (ex. particle, lens-flare)
void MultiEffect::drawAll(const DrawInfo &drawInfo)
{
	for (IT_SpecialEffectsList it = m_effectlist.begin(); it != m_effectlist.end(); ++it)
		(*it)->draw(drawInfo);
}

// update the effect (ex. particle, lens-flare)
void MultiEffect::updateAll()
{
	for (IT_SpecialEffectsList it = m_effectlist.begin(); it != m_effectlist.end(); ++it)
		(*it)->update();
}

