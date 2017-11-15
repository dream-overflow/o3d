/**
 * @file evaluator.h
 * @brief Mathematical function evaluator.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-02-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_EVALUATOR_H
#define _O3D_EVALUATOR_H

#include "o3d/core/math.h"
#include "o3d/core/vector2.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Evaluator
//-------------------------------------------------------------------------------------
//! base class for all evaluator
//---------------------------------------------------------------------------------------
class O3D_API Evaluator
{
protected:

	Int32 m_precision;   //!< number of step for evaluate
	Bool m_IsComputed;   //!< is the evaluator computed

public:

	//! Evaluator type.
	enum Type
	{
		LINEAR,
		SMOOTH,
		CONSTANT,
		BEZIER,
		TCB,
		UNDEFINED
	};

	// constructor
	Evaluator(Int32 precision=O3D_MAX_EVALUATOR_PRECISION)
	{
		m_precision = precision;
		m_IsComputed = False;
	}
};

//---------------------------------------------------------------------------------------
//! @class Evaluator1D
//-------------------------------------------------------------------------------------
//! base class for all one dimension evaluator.
//---------------------------------------------------------------------------------------
class O3D_API Evaluator1D : public Evaluator
{
protected:

	Float* m_xdata;   // array of the X and Y of the curve
	Float* m_ydata;

public:

	// constructor
	Evaluator1D(Int32 precision=O3D_MAX_EVALUATOR_PRECISION);

	// destructor
	virtual ~Evaluator1D();

	// compute the value at a given X
	Float evaluate(Float x) const;
};

//---------------------------------------------------------------------------------------
//! @class Evaluator1D_Bezier
//-------------------------------------------------------------------------------------
//! one dimension bezier curve evaluator defined by 4 points :
//!	- P0: start point
//!	- P3: end point
//!	- P0->P1: start tangent
//!	- P3->P2: end tangent
//---------------------------------------------------------------------------------------
class O3D_API Evaluator1D_Bezier : public Evaluator1D
{
protected:

public:

	// force the computation of the whole curve values
	void initialize(Float P0,Vector2f P1,Vector2f P2,Float P3);
};

//---------------------------------------------------------------------------------------
//! @class Evaluator1D_TCB
//-------------------------------------------------------------------------------------
//! one dimension TCB evaluator also called Kochanek-Bartels Splines (KBS).
//!	-T: for tension
//!	-C: for continuity
//!	-B: for bias
//!
//!  -P0: start point of the curve
//!  -P3: end point of the curve
//!
//!  TCB use the equation of Hermits curves.
//!	-P1: first control point
//!	-T1: tangent from P1
//!	-P2: second control point
//!	-T2: tangent to P2
//---------------------------------------------------------------------------------------
class O3D_API Evaluator1D_TCB : public Evaluator1D
{
protected:

public:

	// force the computation of the whole curve values
	void initialize(Float P0,Float P1,Float P2,Float P3,
		            Float T1,Float C1,Float B1,
					Float T2,Float C2,Float B2);
};

} // namespace o3d

#endif // _O3D_EVALUATOR_H

