/**
 * @file skyobject.h
 * @brief Definition of a sun for the sky renderer
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2008-05-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SKYOBJECT_H
#define _O3D_SKYOBJECT_H

#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/sky/skyobjectbase.h"

namespace o3d {

class ShaderInstance;

//---------------------------------------------------------------------------------------
//! @class SkyObject
//-------------------------------------------------------------------------------------
//! A simple sky object class.
//!
//! This class defines static objects, that is object whose position/apparent angle/
//! intensity never changes.
//! If you want to have dynamic objects, you must create you own class inherited from
//! SkyObjectBase and reimplement SkyObjectBase::getPosition.
//---------------------------------------------------------------------------------------
class O3D_API SkyObject : public SkyObjectBase
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(SkyObject)

	//! Default constructor
	SkyObject(BaseObject * _pParent);
	//! The destructor
	virtual ~SkyObject();

	//! Specify the texture to be used for the sun
	void setTexture(Texture2D * _pTexture);
	Texture2D * getTexture() { return m_pTexture.get(); }
	const Texture2D * getTexture() const { return m_pTexture.get(); }

	//! @brief Specify the position of the object using spherical coordinates
	//! The three dimensional vector contains the two angle in radian and the
	//! distance of the object (in meters)
	//! - The first angle is the rotation angle with respect to the vertical axis, that is
	//!   the azimuth angle.
	//! - The second angle is called inclination (http://en.wikipedia.org/wiki/Spherical_coordinate_system).
	//!   It is the angle between the ground and the object direction.
	void setPosition(const Vector3 & _position) { m_position = _position; }
	const Vector3 & getPosition() const { return m_position; }

	//! @brief Specify the apparent angle of the object
	void setApparentAngle(const Vector2f & _angle) { m_apparentAngle = _angle; }
	const Vector2f & getApparentAngle() const { return m_apparentAngle; }

	//! @brief Specify the intensity for each wavelength
	//! Default is (0.0, 0.0, 0.0)
	void setIntensity(const Vector3 & _intensity) { m_intensity = _intensity; }
	const Vector3 & getIntensity() const { return m_intensity; }

	//! @brief Defines the wavelength to use to do (not to use)
	//! Default is (650.0e-9f, 610.0e-9f, 475.0e-9f)
	void setWaveLength(const Vector3 & _waveLength) { m_waveLength = _waveLength; }
	const Vector3 & getWaveLength() const { return m_waveLength; }

	//! @brief Defines the brightness threshold
	//! If the object is not bright, it may not be visible. In this case, it's useless
	//! to draw it. If the brightness is lower than the threshold, the object won't be
	//! draw.
	//! @param _threshold the brightness threshold (default is 0.01)
	void setBrightnessThreshold(Float _threshold) { m_brightnessThreshold = _threshold; }
	Float getBrightnessThreshold() const { return m_brightnessThreshold; }

	//! @brief Convenient function : Converts the position of this object in cartesian coordinates
	const Vector3 getCartesianPosition() const;

	//! @brief Modify the brightness to avoid popping effect
	//! An object could instantly appear when its brightness will be higher than the threshold.
	//! To avoid that, this function smoothes the transition.
	static Float smoothBrightness(Float _brightness, Float _threshold);

protected:

	//! @brief Returns the position at a specified time using spherical coordinates
	//! It consists in a three dimensional vector containing the two angle in radian and the
	//! distance of the object (in meters)
	//! - The first angle is the rotation angle with respect to the vertical axis, that is
	//!   the azimuth angle.
	//! - The second angle is called inclination (http://en.wikipedia.org/wiki/Spherical_coordinate_system).
	//!   It is the angle between the ground and the object direction.
    virtual void getPosition(Double _time, Vector3 & _position, CoordinateType & _coord) override;

	//! @brief Return the apparent angle
	//! This value is used to set the size of the object in the sky
    virtual void getApparentAngle(Double _time, Vector2f & _angle) override;

	//! @brief Return the intensity of the sun at a specified time
    virtual void getIntensity(Double _time, Vector3 & _intensity) override;

	//! @brief Returns the three wavelengths of the light source
    virtual void getWaveLength(Double _time, Vector3 & _wavelength) override;

	//! Call when the sky object must be drawn
    virtual void draw(const DrawInfo &drawInfo) override;

private:

	// Members //

	SmartObject<Texture2D> m_pTexture;

	ShaderInstance m_shader;

	Vector3 m_position;
	Vector2f m_apparentAngle;

	Vector3 m_intensity;
	Vector3 m_waveLength;

	Float m_brightnessThreshold;
};

} // namespace o3d

#endif // _O3D_SKYOBJECT_H
