/**
 * @file scatteringmodeldefault.h
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SCATTERINGMODELDEFAULT_H
#define _O3D_SCATTERINGMODELDEFAULT_H

#include "o3d/engine/sky/scatteringmodelbase.h"

namespace o3d {

/**
 * @brief Default physical model
 * @date 2008-05-09
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 */
class O3D_API ScatteringModelDefault : public ScatteringModelBase
{
public:

	O3D_DECLARE_DYNAMIC_CLASS_NO_ASSIGN(ScatteringModelDefault)

	//! Default constructor
	explicit ScatteringModelDefault(BaseObject * _pParent);
	//! The destructor
	virtual ~ScatteringModelDefault();

	//! @brief Entry function of the model
    virtual TaskResult entry(TaskData &) override;

private:

	//! @brief Internal function. Returns the anisotropic coefficient based on Henyey Greenstein function
	Float getHenyeyGreensteinFunction(const Float _lCosAlpha, const Float _lG) const;

	//! @brief Internal function.
	//! Computation of rayleigh coefficients.
	Vector3 getRayleighCrossSectionCoef(const TaskData & _datas, const Vector3 & _lambdas) const;
	//! @brief Internal function.
	//! Computation of mie coefficients.
	Vector3 getMieCrossSectionCoef(const TaskData & _datas, const Vector3 & _lambdas) const;

	//! @brief Internal function.
	//! Returns the intersection of a line with a sphere
	Vector3 getSphereIntersection(const Vector3 & _pos, const Vector3 & _direction, Float _radius) const;

	//! @brief Internal function.
	//! Computes the optical depth along a line by using numerical integration.
	void getOpticalLength(	const TaskData & _datas,
							const Vector3 & _pos,
							const Vector3 & _direction,
							Float & _rayOpticalLength,
							Float & _mieOpticalLength) const;

	//! @brief Internal function.
	//! Convert physical luminance values into rgb colors.
	static void convertToRGB(const TaskData & _datas, Float * _pColor, Float _maxLuminance);

	//! @brief Internal function.
	//! Convert rgb colors to physical luminance
	static void convertRGBToLuminance(const TaskData & _datas, Float * _pColor);
};

} // namespace o3d

#endif // _O3D_SCATTERINGMODELDEFAULT_H
