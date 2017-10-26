/**
 * @file skyobjectbase.h
 * @brief Base class for all sky objects.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2008-05-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SKYOBJECTBASE_H
#define _O3D_SKYOBJECTBASE_H

#include "o3d/engine/scene/sceneobject.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class SkyObjectBase
//-------------------------------------------------------------------------------------
//! Base class for all sky objects
//---------------------------------------------------------------------------------------
class O3D_API SkyObjectBase : public SceneObject
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(SkyObjectBase)

	enum CoordinateType
	{
		COORDINATE_RELATIVE = 0,
		COORDINATE_ABSOLUTE
	};

	//! Default constructor
	SkyObjectBase(BaseObject * _pParent);
	//! The destructor
	virtual ~SkyObjectBase();

	//! @brief Return Scene::DrawSkyObject
	virtual UInt32 getDrawType() const;

	//! @brief Returns the position at a specified time using spherical coordinates
	//! It consists in a three dimensional vector containing the two angle in radian and the
	//! distance of the object (in meters)
	//! - The first angle is the rotation angle with respect to the vertical axis, that is
	//!   the azimuth angle.
	//! - The second angle is called inclination (http://en.wikipedia.org/wiki/Spherical_coordinate_system).
	//!   It is the angle between the ground and the object direction.
	virtual void getPosition(Double _time, Vector3 & _position, CoordinateType & _coord) = 0;

	//! @brief Returns the apparent angle
	//! This value is used to set the size of the object in the sky.
	//! Only the first value of the vector is used to scale the object. The second is
	//! used during frustum culling.
	virtual void getApparentAngle(Double _time, Vector2f & _angle) = 0;

	//! @brief Returns the intensity at a specified time
	//! This data is used to know if a star (for example) is worse drawing. During the
	//! day, its intensity is totally negligible.
	virtual void getIntensity(Double _time, Vector3 & _intensity) = 0;

	//! @brief Returns the three wavelengths of the light source (in meters)
	virtual void getWaveLength(Double _time, Vector3 & _wavelength) = 0;

	//! Call when the sky object must be drawn
	virtual void draw(const DrawInfo &drawInfo) = 0;

	//! Call when the object must be updated
	virtual void update();

	//! @Brief Returns the brightness (similar to the contrast) of the object
	//! You must take care not to call this function outside the drawing function, since it could
	//! returns wrong values if you use the same object in different sky engines.
	//! This value allows you to do proper blending when drawing the object (assume to be O3DBlending_SrcA_One)
	//! @return the brightness which is in [0; 1]
	Float getBrightness() const { return m_brightness; }

	//! @brief Returns the physical luminance of the object from camera eye.
	//! You must take care not to call this function outside the drawing function, since it could
	//! returns wrong values if you use the same object in different sky engines.
	const Vector3 & getLuminance() const { return m_luminance; }

	//-----------------------------------------------------------------------------------
	// Internal functions
	//-----------------------------------------------------------------------------------

	//! @brief Internal function.
	//! Called by the sky renderer before each draw.
	void setBrightness(Float _bright) { m_brightness = _bright; }

	//! @brief Internal function.
	//! Called by the sky renderer before each draw.
	void setLuminance(const Vector3 & _lum) { m_luminance = _lum; }

private:

	//-----------------------------------------------------------------------------------
	// Members
	//-----------------------------------------------------------------------------------

	Float m_brightness;	 //!< The user can get this value to do proper blending
	Vector3 m_luminance;  //!< Luminance for each color component that reach the camera
};

} // namespace o3d

#endif // _O3D_SKYOBJECTBASE_H

