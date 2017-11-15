/**
 * @file evaluator.cpp
 * @brief Implementation of Evaluator.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 03-02-2005
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/animation/evaluator.h"

#include "o3d/core/debug.h"

using namespace o3d;

/*---------------------------------------------------------------------------------------
  O3DEvaluator1D
---------------------------------------------------------------------------------------*/
Evaluator1D::Evaluator1D(Int32 precision) :
	Evaluator(precision)
{
	m_xdata = new Float[m_precision];
	m_ydata = new Float[m_precision];
}


Evaluator1D::~Evaluator1D()
{
	deleteArray(m_xdata);
	deleteArray(m_ydata);
}

Float Evaluator1D::evaluate(Float x) const
{
	if (!m_IsComputed)
	{
		O3D_WARNING("Not evaluated");
		return 0.f;
	}

	if (x < o3d::Limits<Float>::epsilon())
		return m_ydata[0];

	if (x < m_xdata[0])
		return m_ydata[0];

	Int32 i = 1;
	while (i <= m_precision-1) // search
	{
		if (x < m_xdata[i]) // x is in the list
		{
			// interpolate for have the correct value
			Float t = (x-m_xdata[i-1])/(m_xdata[i]-m_xdata[i-1]);
			return ((1-t)*m_ydata[i-1]+t*m_ydata[i]);
		}
		i++;
	}

	// if the value is after the last key return the last
	return m_ydata[m_precision-1];
}

/*---------------------------------------------------------------------------------------
  O3DEvaluator1D_Bezier
---------------------------------------------------------------------------------------*/
void Evaluator1D_Bezier::initialize(Float P0,Vector2f P1,Vector2f P2,Float P3)
{
	Float t = 0;
	Float step = (1.f)/((Float)m_precision-1);
	Float b0,b1,b2,b3;

	for (Int32 i = 0 ; i < m_precision ; ++i)
	{
		// compute berstein values
		b0 = (1-t)*(1-t)*(1-t); //(1-t)^3
		b1 = 3*t*(1-t)*(1-t);   //3t*(1-t)²
		b2 = 3*t*t*(1-t);       //3t²*(1-t)
		b3 = t*t*t;             //t^3

		m_xdata[i] =         b1*P1[X] + b2*P2[X] + b3;
		m_ydata[i] = b0*P0 + b1*P1[Y] + b2*P2[Y] + b3*P3;

		t += step;
	}

	m_IsComputed = True;
}

/*---------------------------------------------------------------------------------------
  O3DEvaluator1D_TCB
---------------------------------------------------------------------------------------*/
void Evaluator1D_TCB::initialize(Float P0,Float P1,Float P2,Float P3,
								    Float T1,Float C1,Float B1,
							    	Float T2,Float C2,Float B2)
{
	Float t = 0;
	Float step = (1.0f)/((Float)m_precision-1);
	Float h0,h1,h2,h3;

	Float fact1,fact2;
	Float TD1[2]; // first tangent (from P1)
	Float TS2[2]; // second tangent (to P2)

	// compute the first tangent
	fact1 = 0.5f*(1-T1)*(1+C1)*(1+B1);
	fact2 = 0.5f*(1-T1)*(1+C1)*(1-B1);
	TD1[X] = fact1         + fact2;
	TD1[Y] = fact1*(P1-P0) + fact2*(P2-P1);

	// compute the second tangent
	fact1 = 0.5f*(1-T2)*(1+C2)*(1+B2);
	fact2 = 0.5f*(1-T2)*(1-C2)*(1-B2);
	TS2[X] = fact1         + fact2;
	TS2[Y] = fact1*(P1-P0) + fact2*(P2-P1);

	for (Int32 i = 0 ; i < m_precision ; ++i)
	{
		// compute hermite values
		h0 =  2*t*t*t - 3*t*t + 1;  // 2t^3 - 3t² + 1
		h1 = -2*t*t*t + 3*t*t;      // (-2)t^3 + 3t²
		h2 =    t*t*t - 2*t*t  + t; // t^3 - 2t² + t
		h3 =    t*t*t -   t*t;      // t^3 - t²

		m_xdata[i] =         h1*TD1[X] + h2*TS2[X] + h3;
		m_ydata[i] = h0*P0 + h1*TD1[Y] + h2*TS2[Y] + h3*P3;

		t += step;
	}

	m_IsComputed = True;
}

