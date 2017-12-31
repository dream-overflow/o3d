/**
 * @file fog.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-05-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/effect/fog.h"

#include "o3d/core/debug.h"
#include "o3d/engine/context.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Fog, ENGINE_EFFECT_FOG, SpecialEffects)

/*---------------------------------------------------------------------------------------
  Contructeur
---------------------------------------------------------------------------------------*/
Fog::Fog(BaseObject *pParent) :
	SpecialEffects(pParent)
{
	mode = FOG_EXP;
	density = 1.f;
	start = 0.f;
	end = 1.f;
}

/*---------------------------------------------------------------------------------------
  initialise tout les parametre du brouillard
---------------------------------------------------------------------------------------*/
void Fog::create(
		const Color &_col,
		Float _start,
		Float _end,
		Float _density,
		FogMode _mode)
{
	col = _col;
	mode = _mode;
	start = _start;
	end = _end;
	density = _density;

	disable();
}

/*---------------------------------------------------------------------------------------
  initialise la couleur du fog
---------------------------------------------------------------------------------------*/
void Fog::setColor(const Color &_col)
{
	col = _col;
}

void Fog::setColor(Float r,Float g,Float b,Float a)
{
	col.set(r,g,b,a);
}

/*---------------------------------------------------------------------------------------
  initialise le mode du fog
---------------------------------------------------------------------------------------*/
void Fog::setMode(FogMode _mode)
{
	mode = _mode;
}

/*---------------------------------------------------------------------------------------
 initialise le début et la fin du fog
---------------------------------------------------------------------------------------*/
void Fog::setDistance(Float _start,Float _end)
{
	start = _start;
	end = _end;
}

/*---------------------------------------------------------------------------------------
 initialise la densité du fog
---------------------------------------------------------------------------------------*/
void Fog::setDensity(Float _density)
{
	density = _density;
}

/*---------------------------------------------------------------------------------------
  active/désactive le fog
---------------------------------------------------------------------------------------*/
void Fog::enable()
{
	if (m_capacities.getBit(STATE_ACTIVITY))
		return;

	back = getScene()->getContext()->getBackgroundColor();

	getScene()->getContext()->setBackgroundColor(col);

	m_capacities.setBit(STATE_ACTIVITY, True);
}

void Fog::disable()
{
	if (!m_capacities.getBit(STATE_ACTIVITY))
		return;

	getScene()->getContext()->setBackgroundColor(back);

    m_capacities.setBit(STATE_ACTIVITY, False);
}

// serialisation
Bool Fog::writeToFile(OutStream &os)
{
    SpecialEffects::writeToFile(os);

    os   << col
		 << mode
		 << density;

    os   << start
		 << end;

	return True;
}

Bool Fog::readFromFile(InStream &is)
{
    SpecialEffects::readFromFile(is);

    is   >> col
		 >> mode
		 >> density;

    is   >> start
		 >> end;

	return True;
}
