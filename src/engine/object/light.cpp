/**
 * @file light.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/light.h"

#include "o3d/core/debug.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/geom/frustum.h"
#include "o3d/geom/bcone.h"
#include "o3d/engine/object/primitivemanager.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/hierarchy/node.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(Light, ENGINE_LIGHT, SceneObject)

Light::Light(BaseObject *parent, Light::LightType lightType) :
	SceneObject(parent),
	m_lightType(lightType),
	m_exponent(0.0f),
	m_attenuation(1.0f, 0.0f, 0.0f),
	m_attenuationThreshold(0.01f),
	m_cutOff(90.f),
	m_ambient(0.7f, 0.7f, 0.7f, 1.0f),
	m_diffuse(0.7f, 0.7f, 0.7f, 1.0f),
	m_specular(0.7f ,0.7f, 0.7f ,1.0f),
    m_pConeBounding(nullptr),
	m_thresholdDistance(-1.0f) // Negative values are not valid. It means 'must be updated'
{
	if (lightType == Light::SPOT_LIGHT)
    {
        m_pConeBounding = new BCone(
                    Vector3(getWorldPosition().getData()),
                    getWorldDirection(),
                    getThresholdDistance(),
                    o3d::toRadian(m_cutOff),
                    True);
    }

	setUpdatable(True);
	setDrawable(True);
	setPickable(True);
	setAnimatable(True);
	setShadable(False);
}

Light::Light(const Light &dup) :
	SceneObject(dup),
	m_lightType(dup.m_lightType),
	m_exponent(dup.m_exponent),
	m_attenuation(dup.m_attenuation),
	m_attenuationThreshold(dup.m_attenuationThreshold),
	m_cutOff(dup.m_cutOff),
	m_ambient(dup.m_ambient),
	m_diffuse(dup.m_diffuse),
	m_specular(dup.m_specular),
    m_pConeBounding(nullptr),
	m_thresholdDistance(dup.m_thresholdDistance)
{
    if (dup.m_pConeBounding != nullptr)
		m_pConeBounding = new BCone(*dup.m_pConeBounding);
}

//! Copy constructor.
Light::~Light()
{
	deletePtr(m_pConeBounding);
}

// Get the drawing type.
UInt32 Light::getDrawType() const
{
	switch (m_lightType)
	{
		case POINT_LIGHT:
			return Scene::DRAW_POINT_LIGHT;
		case SPOT_LIGHT:
			return Scene::DRAW_SPOT_LIGHT;
		case DIRECTIONAL_LIGHT:
			return Scene::DRAW_DIRECTIONAL_LIGHT;
		default:
			return 0;
	}
}

// Nothing to update.
void Light::update()
{
    clearUpdated();

    if (m_node && (m_node->hasUpdated()))
    {
        setUpdated();

        if (m_lightType == SPOT_LIGHT) // The cone won't update if the position didnt change.
        {
            m_pConeBounding->setOrigin(Vector3(getWorldPosition().getData()));
            // direction on Z component of the matrix
            m_pConeBounding->setDirection(!m_node->getAbsoluteMatrix().getZ());
        }
    }
}

//! Set the constant light attenuation.
void Light::setConstantAttenuation(Float constant)
{
	if (m_attenuation[0] != constant)
		m_thresholdDistance = -1.0f;

	m_attenuation[0] = constant;

    if (m_lightType == SPOT_LIGHT)
        m_pConeBounding->setDepth(getThresholdDistance());
}

//! Set the linear light attenuation.
void Light::setLinearAttenuation(Float linear)
{
	if (m_attenuation[1] != linear)
		m_thresholdDistance = -1.0f;

	m_attenuation[1] = linear;

    if (m_lightType == SPOT_LIGHT)
        m_pConeBounding->setDepth(getThresholdDistance());
}

//! Set the quadratic light attenuation.
void Light::setQuadraticAttenuation(Float quadratic)
{
	if (m_attenuation[2] != quadratic)
		m_thresholdDistance = -1.0f;

	m_attenuation[2] = quadratic;

    if (m_lightType == SPOT_LIGHT)
        m_pConeBounding->setDepth(getThresholdDistance());
}

//! Set the light attenuation from 3 float.
void Light::setAttenuation(Float constant, Float linear, Float quadratic)
{
	if ((m_attenuation[0] != constant) || (m_attenuation[1] != linear) || (m_attenuation[2] != quadratic))
		m_thresholdDistance = -1.0f;

	m_attenuation.set(constant, linear, quadratic);

    if (m_lightType == SPOT_LIGHT)
        m_pConeBounding->setDepth(getThresholdDistance());
}

//! Set the light attenuation from a 3 float vector.
void Light::setAttenuation(const Vector3 att)
{
	if (m_attenuation != att)
		m_thresholdDistance = -1.0f;

	m_attenuation = att;

    if (m_lightType == SPOT_LIGHT)
        m_pConeBounding->setDepth(getThresholdDistance());
}

//! Set the attenuation threshold
void Light::setAttenuationThreshold(Float _value)
{
	if (m_attenuationThreshold != _value)
		m_thresholdDistance = -1.0f;

	m_attenuationThreshold = _value;

    if (m_lightType == SPOT_LIGHT)
        m_pConeBounding->setDepth(getThresholdDistance());
}

// Set the light cutoff in degree [0..90]. 180 mean omnidirectional like a point light.
void Light::setCutOff(Float cutoff)
{
	O3D_ASSERT((cutoff >= 0.f) && ((cutoff <= 90.f) || (cutoff == 180.f)));

	if ((m_lightType == SPOT_LIGHT) && (m_cutOff != cutoff))
		m_pConeBounding->setCutoff(o3d::toRadian(cutoff));

	m_cutOff = cutoff;
}

// Draw according to the light type.
void Light::draw(const DrawInfo &drawInfo)
{
	if (!getActivity() || !getVisibility())
		return;

	switch (m_lightType)
	{
		case POINT_LIGHT:
			drawPointLight(drawInfo);
			return;
		case SPOT_LIGHT:
			drawSpotLight(drawInfo);
			return;
		case DIRECTIONAL_LIGHT:
			drawDirectLight(drawInfo);
			return;
		default:
			return;
	}
}

void Light::drawPointLight(const DrawInfo &drawInfo)
{
	if ((drawInfo.pass == DrawInfo::AMBIENT_PASS) && getScene()->getDrawObject(Scene::DRAW_POINT_LIGHT))
	{
		setUpModelView();

		PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access();

		if (getScene()->getDrawObject(Scene::DRAW_LOCAL_AXIS))
			primitive->drawLocalAxis();

		primitive->setColor(m_diffuse);

        // wire sphere light volume
        if (getScene()->getDrawObject(Scene::DRAW_LIGHT_VOLUME))
            primitive->draw(
                        PrimitiveManager::WIRE_SPHERE1,
                        Vector3(getRadius(), getRadius(), getRadius()));
        else // or little wire sphere
            primitive->draw(PrimitiveManager::WIRE_SPHERE1, Vector3(0.3f,0.3f,0.3f));
	}
}

void Light::drawSpotLight(const DrawInfo &drawInfo)
{
	if ((drawInfo.pass == DrawInfo::AMBIENT_PASS) && getScene()->getDrawObject(Scene::DRAW_SPOT_LIGHT))
	{
		setUpModelView();

		PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access();

		if (getScene()->getDrawObject(Scene::DRAW_LOCAL_AXIS))
			primitive->drawLocalAxis();

		primitive->setColor(m_diffuse);

		if (m_cutOff < 180.f)
		{
			primitive->beginDraw(P_LINES);
				primitive->addVertex(0,0,0);
				primitive->addVertex(0,0,3);
			primitive->endDraw();

			// Source solid sphere
            primitive->modelView().translate(Vector3(0,0,3));
			primitive->draw(PrimitiveManager::SOLID_SPHERE1, Vector3(0.1f,0.1f,0.1f));

            // wire cone light volume
            if (getScene()->getDrawObject(Scene::DRAW_LIGHT_VOLUME))
            {
                // Directional wire cone (Z aligned)
                primitive->modelView().translate(Vector3(0,0,-1+getLength()));
                primitive->modelView().rotateX(o3d::toRadian(-90.f));
                primitive->draw(PrimitiveManager::WIRE_CONE1, Vector3(
                                    getRadius(),
                                    getLength(),
                                    getRadius()) );
            }
            else // or little light cone
            {
                // Directional wire cone (Z aligned)
                primitive->modelView().translate(Vector3(0,0,-1+getLength()));
                primitive->modelView().rotateX(o3d::toRadian(-90.f));
                primitive->draw(PrimitiveManager::WIRE_CONE1, Vector3(
                                    2.f*tanf(o3d::toRadian(m_cutOff)),
                                    2.f,
                                    2.f*tanf(o3d::toRadian(m_cutOff))) );
            }
		}
		else
		{
			// Omnidirectional wire sphere
			primitive->draw(PrimitiveManager::WIRE_SPHERE1, Vector3(0.3f,0.3f,0.3f));
		}
	}
}

void Light::drawDirectLight(const DrawInfo &drawInfo)
{
	if ((drawInfo.pass == DrawInfo::AMBIENT_PASS) && getScene()->getDrawObject(Scene::DRAW_DIRECTIONAL_LIGHT))
	{
		setUpModelView();

		PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access();

		if (getScene()->getDrawObject(Scene::DRAW_LOCAL_AXIS))
			primitive->drawLocalAxis();

		primitive->setColor(m_diffuse);

		// direction line (Z aligned)
		primitive->beginDraw(P_LINES);
			primitive->addVertex(0.f,0.f,0.f);
			primitive->addVertex(0.f,0.f,3.f);
		primitive->endDraw();

		// source solid sphere
		primitive->modelView().translate(Vector3(0.f,0.f,3.f));
		primitive->draw(PrimitiveManager::SOLID_SPHERE1, Vector3(0.1f,0.1f,0.1f));

		// direction wired cylinder (Z aligned)
		primitive->modelView().translate(Vector3(0.f,0.f,-1.f));
		primitive->modelView().rotateX(o3d::toRadian(-90.f));
		primitive->draw(PrimitiveManager::WIRE_CYLINDER1, Vector3(1.f,2.f,1.f));

        // because this is an infinite light we dont have light volume
	}
}

//! Check only with its parent node position.
Geometry::Clipping Light::checkBounding(const AABBox &bbox) const
{
	switch (m_lightType)
	{
	case SPOT_LIGHT:
		return bbox.clip(*m_pConeBounding);
	case POINT_LIGHT:
		return bbox.clip(BSphere(Vector3(getWorldPosition().getData()), getThresholdDistance()));
	default:
		return Geometry::CLIP_INTERSECT;
	}
}

//! Check only with its parent node position.
Geometry::Clipping Light::checkBounding(const Plane &plane) const
{
	switch (m_lightType)
	{
	case SPOT_LIGHT:
        return plane.clip(*m_pConeBounding);
	case POINT_LIGHT:
		return plane.clip(BSphere(Vector3(getWorldPosition().getData()), getThresholdDistance()));
	default:
		return Geometry::CLIP_INTERSECT;
	}
}

//! Check only with its parent node position.
Geometry::Clipping Light::checkBounding(const BSphere &sphere) const
{
	switch (m_lightType)
	{
	case SPOT_LIGHT:
		return sphere.clip(*m_pConeBounding);
	case POINT_LIGHT:
		return sphere.clip(BSphere(Vector3(getWorldPosition().getData()), getThresholdDistance()));
	default:
		return Geometry::CLIP_INTERSECT;
	}
}

Geometry::Clipping Light::checkFrustum(const Frustum &frustum) const
{
	switch (m_lightType)
	{
	case SPOT_LIGHT:
        return frustum.coneInFrustum(*m_pConeBounding);
	case POINT_LIGHT:
		return frustum.sphereInFrustum(Vector3(getWorldPosition().getData()), getThresholdDistance());
	default:
		return Geometry::CLIP_INTERSECT;
	}
}

// Get the light position into the eye space using the current active camera.
Vector4 Light::getPosition() const
{
	if (getScene()->getActiveCamera())
	{
		if (m_lightType == DIRECTIONAL_LIGHT)
		{
			return Vector4(
					!(getScene()->getActiveCamera()->getModelviewMatrix() *
							getAbsoluteMatrix()).getZ(),
					0.0f);
		}
		else
		{
			return Vector4(
					(getScene()->getActiveCamera()->getModelviewMatrix() *
							getAbsoluteMatrix()).getTranslation(),
					1.0f);
		}
	}
	else
	{
		if (m_lightType == DIRECTIONAL_LIGHT)
            return Vector4(!getAbsoluteMatrix().getZ(), 0.0f);
		else
            return Vector4(getAbsoluteMatrix().getTranslation(), 1.0f);
	}
}

// Get the light direction into the eye space using the current active camera.
Vector3 Light::getDirection() const
{
	if (getScene()->getActiveCamera())
	{
		if (m_lightType == POINT_LIGHT)
			return Vector3();
		else
			return !(getScene()->getActiveCamera()->getModelviewMatrix() * getAbsoluteMatrix()).getZ();
	}
	else
        return !getAbsoluteMatrix().getZ();
}

// Get the light position into world space.
Vector4 Light::getWorldPosition() const
{
	if (m_lightType == DIRECTIONAL_LIGHT)
		return Vector4(!getAbsoluteMatrix().getZ(), 0.0f);
	else
		return Vector4(getAbsoluteMatrix().getTranslation(), 1.0f);
}

//! Get the light direction into world space.
Vector3 Light::getWorldDirection() const
{
	if (m_lightType == POINT_LIGHT)
		return Vector3();
	else
        return !getAbsoluteMatrix().getZ();
}

Float Light::getRadius() const
{
    if (m_lightType == POINT_LIGHT)
        return getThresholdDistance();
    else if (m_lightType == SPOT_LIGHT)
        return getThresholdDistance() * tanf(toRadian(m_cutOff));
    else
        return 0.0f;
}

// Serialization
Bool Light::writeToFile(OutStream &os)
{
    if (!SceneObject::writeToFile(os))
		return False;

    os   << static_cast<Int32>(m_lightType)
		 << m_exponent
		 << m_attenuation
		 << m_cutOff
		 << m_ambient
		 << m_diffuse
		 << m_specular;

    return True;
}

Bool Light::readFromFile(InStream &is)
{
    if (!SceneObject::readFromFile(is))
		return False;

	Int32 lightType;

    is   >> lightType
		 >> m_exponent
		 >> m_attenuation
		 >> m_cutOff
		 >> m_ambient
		 >> m_diffuse
		 >> m_specular;

	m_lightType = static_cast<LightType>(lightType);
	m_thresholdDistance = -1.0f; // This value should be computed (negative value means 'not up to date')

	deletePtr(m_pConeBounding);

	if (m_lightType == SPOT_LIGHT)
		m_pConeBounding = new BCone(Vector3(getWorldPosition().getData()), getWorldDirection(), getThresholdDistance(), m_cutOff, True);

    return True;
}

Float Light::getThresholdDistance() const
{
	if (m_thresholdDistance >= 0.0f) // It means that the value is up to date
		return m_thresholdDistance;

	// Assume all components of m_attenuation are positive
	O3D_ASSERT((m_attenuation.x() >= 0.0f) && (m_attenuation.y() >= 0.0f) && (m_attenuation.z() >= 0.0f));

	const Float eps = 1.0f / o3d::clamp(m_attenuationThreshold, 0.0001f, 1.0f);

	const Float a = m_attenuation.z();
	const Float b = m_attenuation.y();
	const Float c = m_attenuation.x();

	if (a <= 0.0f)
	{
		// Solve b.x + c - eps = 0
		if (b <= 0.0f)
			return 0.0f;
		else
			return (eps - c) / b;
	}
	else
	{
		// Solve a.x^2 + b.x + c - eps = 0
		const Float delta = b*b - 4.0f*a*(c-eps);

		if (delta <= 0.0f)
			return 0.0f;
		else
			return (-b + sqrt(delta)) / (2.0f * a);
	}
}

