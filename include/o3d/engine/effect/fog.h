/**
 * @file fog.h
 * @brief Simple OpenGL FOG effect.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-05-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FOG_H
#define _O3D_FOG_H

#include "specialeffects.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Fog
//-------------------------------------------------------------------------------------
//! Simple OpenGL FOG effect.
//---------------------------------------------------------------------------------------
class O3D_API Fog : public SpecialEffects
{
public:

	O3D_DECLARE_CLASS(Fog)

	//! Mode of the fog
	enum FogMode
	{
		FOG_LINEAR,  //!< linear
		FOG_EXP,     //!< exponential
		FOG_EXP2     //!< exponential^2
	};

	//! Default contructor
	Fog(BaseObject *pParent);

	//! Initialise tout les parametre du brouillard
	void create(
			const Color &_col,
			Float _start = 0.f,
			Float _end = 1.f,
			Float _density = 1.f,
			FogMode _mode = FOG_EXP);

	//! initialise la couleur du fog
	void setColor(const Color &_col);
	void setColor(Float r,Float g,Float b,Float a);

	//! initialise le mode du fog
	void setMode(FogMode _mode);
	inline FogMode getMode()const { return mode; }

	//! initialise le début et la fin du fog
	void setDistance(Float _start,Float _end);

	//! renvoie les distance du fog
	inline Float getStart()const { return start; }
	inline Float getEnd()const   { return end; }

	//! initialise la densité du fog
	void setDensity(Float _density);

	//! renvoie la densité du fog
	inline Float getDensity()const { return density; }

	//! active/désactive le fog
	virtual void enable();
	virtual void disable();

	virtual void drawgetPicking() {}

	//! sérialisation
	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

protected:

	Color col;		 //!< couleur du brouillard
	FogMode mode;	     //!< gradiation du brouillard
	Float density;	 //!< intensité du brouillard
	Float start,end; //!< début et fin du brouillard

	Color back;	     //!< couleur de font à mémoriser pour la rétablire
};

} // namespace o3d

#endif // _O3D_FOG_H

