/**
 * @file skyobject.cpp
 * @brief Definition of a sun for the sky renderer
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2008-05-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"

#include "o3d/engine/shader/shadermanager.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/sky/skyobject.h"
#include "o3d/engine/object/primitivemanager.h"

#include "o3d/engine/context.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(SkyObject, ENGINE_SKY_OBJECT, SkyObjectBase)

//! Default constructor
SkyObject::SkyObject(BaseObject * _pParent):
	SkyObjectBase(_pParent),
	m_pTexture(this),
	m_shader(),
	m_position(),
	m_apparentAngle(),
	m_intensity(),
	m_waveLength(650.0e-9f, 610.0e-9f, 475.0e-9f),
	m_brightnessThreshold(0.01f)
{
	Shader * lpShading = getScene()->getShaderManager()->addShader("skyObject");
	O3D_ASSERT(lpShading);

	lpShading->buildInstance(m_shader);
    m_shader.assign(0, 0, "ALPHA_TEST_REF=0.0;ALPHA_FUNC_GREATER;");
}

//! The destructor
SkyObject::~SkyObject()
{
}

//! @brief Convenient function : Converts the position of this object in cartesian coordinates
const Vector3 SkyObject::getCartesianPosition() const
{
	const Float lCosAlpha = cosf(m_position[0]); // Azimuth angle
	const Float lSinAlpha = sinf(m_position[0]);

	const Float lCosBeta = cosf(m_position[1]); // Inclination angle
	const Float lSinBeta = sinf(m_position[1]);

	const Float lDistance = m_position[2];

	return Vector3(lDistance*lSinAlpha*lCosBeta, lDistance*lSinBeta, lDistance*lCosAlpha*lCosBeta);
}

//! @brief Returns the position at a specified time using spherical coordinates
//! It consists in a three dimensional vector containing the two angle in radian and the
//! distance of the object (in meters)
//! - The first angle is the rotation angle with respect to the vertical axis, that is
//!   the azimuth angle.
//! - The second angle is called inclination (http://en.wikipedia.org/wiki/Spherical_coordinate_system).
//!   It is the angle between the ground and the object direction.
void SkyObject::getPosition(Double _time, Vector3 & _position, CoordinateType& _coord)
{
	_position = m_position;
}

//! @brief Return the apparent angle
//! This value is used to set the size of the object in the sky
void SkyObject::getApparentAngle(Double _time, Vector2f & _angle)
{
	_angle = m_apparentAngle;
}

//! @brief Return the intensity of the sun at a specified time
void SkyObject::getIntensity(Double _time, Vector3 & _intensity)
{
	_intensity = m_intensity;
}

//! @brief Returns the three wavelengths of the light source
void SkyObject::getWaveLength(Double _time, Vector3 & _wavelength)
{
	_wavelength = m_waveLength;
}

//! Call when the sky object must be drawn
void SkyObject::draw(const DrawInfo &drawInfo)
{
	if (m_pTexture && m_shader.isOperational() && (getBrightness() > m_brightnessThreshold))
	{
		Context *glContext = getScene()->getContext();

        glContext->blending().setFunc(Blending::SRC_A__ONE);

		m_shader.bindShader();
		m_shader.setConstMatrix4("u_modelViewProjectionMatrix", False, glContext->modelViewProjection());
		m_shader.setConstTexture("u_objectTexture", m_pTexture.get(), 0);
		m_shader.setConstFloat("u_brightness", smoothBrightness(getBrightness(), getBrightnessThreshold()));

		getScene()->getPrimitiveManager()->getQuadTexCoordsVBO().attribute(
				V_UV_MAP_ARRAY,
				2,
				0,
				0);

		getScene()->getPrimitiveManager()->getQuadVerticesVBO().attribute(
				V_VERTICES_ARRAY,
				3,
				0,
				0);

		getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);

		glContext->disableVertexAttribArray(V_UV_MAP_ARRAY);
		glContext->disableVertexAttribArray(V_VERTICES_ARRAY);

		m_shader.unbindShader();

        glContext->blending().setDefaultFunc();
	}
}

//! Specify the texture to be used for the sun
void SkyObject::setTexture(Texture2D * _pTexture)
{
	m_pTexture = _pTexture;
}

//! @brief Modify the brightness to avoid popping effect
Float SkyObject::smoothBrightness(Float _brightness, Float _threshold)
{
	return (1.0f - expf(-100.0f * (_brightness - _threshold))) * _brightness;
}

