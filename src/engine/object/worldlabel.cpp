/**
 * @file worldlabel.cpp
 * @brief Implementation of WorldLabel.h
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2008-04-29
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/worldlabel.h"

#include "o3d/engine/object/camera.h"
#include "o3d/engine/context.h"
#include "o3d/engine/hierarchy/node.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(WorldLabel, ENGINE_WORLD_LABEL, SceneObject)

// Get the drawing type
UInt32 WorldLabel::getDrawType() const
{
	return Scene::DRAW_WORLD_LABEL;
}

//! default contructor
WorldLabel::WorldLabel(BaseObject * _pParent):
	SceneObject(_pParent),
	m_screenSize(),
	m_eqnCoefficient(1.0f, 0.0f, 0.0f, 0.0f),
	m_distanceBounds(),
	m_showAlways(False),
	m_relativeCoordinate(False),
	m_clipping(False),
	m_borderSize(0)
{
	setUpdatable(True);
	setDrawable(True);
	setPickable(False);
	setAnimatable(False);
	setShadable(False);
}

//! copy contructor
WorldLabel::WorldLabel(const WorldLabel & _dup):
	SceneObject(_dup),
	m_screenSize(_dup.m_screenSize),
	m_eqnCoefficient(_dup.m_eqnCoefficient),
	m_distanceBounds(_dup.m_distanceBounds),
	m_showAlways(_dup.m_showAlways),
	m_relativeCoordinate(_dup.m_relativeCoordinate),
	m_clipping(_dup.m_clipping),
	m_borderSize(_dup.m_borderSize)
{
}

//! destructor
WorldLabel::~WorldLabel()
{
}

//! duplicator
WorldLabel& WorldLabel::operator = (const WorldLabel & _dup)
{
	SceneObject::operator = (_dup);

	m_screenSize = _dup.m_screenSize;
	m_eqnCoefficient = _dup.m_eqnCoefficient;
	m_distanceBounds = _dup.m_distanceBounds;
	m_showAlways = _dup.m_showAlways;
	m_relativeCoordinate = _dup.m_relativeCoordinate;
	m_clipping = _dup.m_clipping;
	m_borderSize = _dup.m_borderSize;

	return *this;
}

//! @brief Prepare the drawing of the label
void WorldLabel::draw(const DrawInfo &drawInfo)
{
    if (!getActivity() || !getVisibility()) {
		return;
    }

    if (!hasTopLevelParentTypeOf()) {
		return;
    }

    if ((drawInfo.pass == DrawInfo::AMBIENT_PASS) && getScene()->getDrawObject(Scene::DRAW_WORLD_LABEL)) {
		getScene()->getContext()->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix());

		const Vector4 lWorldPosition(getAbsoluteMatrix().m14(),
										getAbsoluteMatrix().m24(),
										getAbsoluteMatrix().m34(),
										getAbsoluteMatrix().m44());
		const Float lCamDistance = (getScene()->getActiveCamera()->getAbsoluteMatrix().getTranslation() -
			Vector3(lWorldPosition[X], lWorldPosition[Y], lWorldPosition[Z])).length();

		// Check if the distance between the center of the label and the camera is not null.
        if (lCamDistance == 0.0f) {
			return;
        }

		// Next check if the label is in the valid range [m_distanceBounds[0] ; m_distanceBounds[1]
		// The previous test is only done if the bounds are defined, that is if (m_distanceBounds[1] > 0)
        if ((m_distanceBounds[1] > m_distanceBounds[0]) && ((lCamDistance < m_distanceBounds[0]) || (lCamDistance > m_distanceBounds[1]))) {
			return;
        }

		Context * lpContext = getScene()->getContext();

		const Vector2f lDepthRange(lpContext->getNearDepthRange(), lpContext->getFarDepthRange());
		const Bool lScissorEnabled = lpContext->getScissorTest();

		Int32 lViewPort[4];
		Int32 lScissorBox[4];

		Matrix4 lModelview;
		Matrix4 lProjection;

		const Float lSizeFactor = computeSizeFactor(lCamDistance);

		lpContext->getScissor(lScissorBox);
		lpContext->getViewPort(lViewPort);

		lModelview = lpContext->modelView().get();
		lProjection = lpContext->projection().get();

		Vector4 lVertex(lProjection * (lModelview * lWorldPosition));

        if (lVertex[Z] < 0.0f) {
			return;
        }

		const Float lInverseW = 1.0f / lVertex[W];

		lVertex[X] *= lInverseW;
		lVertex[Y] *= lInverseW;
		lVertex[Z] *= lInverseW;

        const Vector2i lScreenPos(
                    lViewPort[0] + Int32((lVertex[0] * 0.5f + 0.5f) * lViewPort[2]),
                    lViewPort[1] + Int32((lVertex[1] * 0.5f + 0.5f) * lViewPort[3]));

        const Vector2i lScreenSize(
                    Int32(m_screenSize[X] * lSizeFactor),
                    Int32(m_screenSize[Y] * lSizeFactor));

		const Int32 lBorderSize(Int32(m_borderSize * lSizeFactor));

        if (o3d::abs<Int32>(lScreenPos[X] - lViewPort[2]/2) > (lViewPort[2] + lScreenSize[X])/2 + (m_borderSize < 0 ? -m_borderSize : 0)) {
			return;
        }

        if (o3d::abs<Int32>(lScreenPos[Y] - lViewPort[3]/2) > (lViewPort[3] + lScreenSize[Y])/2 + (m_borderSize < 0 ? -m_borderSize : 0)) {
			return;
        }

		Matrix4 lOrtho2D(
			2.0f / lViewPort[2],	0.0f,					0.0f,		-1.0f,
			0.0f,					2.0f / lViewPort[3],	0.0f,		-1.0f,
			0.0f,					0.0f,					-1.0f,		0.0f,
			0.0f,					0.0f,					0.0f,		1.0f);

		lOrtho2D.setData(0, 3, lOrtho2D.m11() * ((lScreenPos[X] - lScreenSize[X]/2) + 0.5f) + lOrtho2D.m14());
		lOrtho2D.setData(1, 3, lOrtho2D.m22() * ((lScreenPos[Y] - lScreenSize[Y]/2) + 0.5f) + lOrtho2D.m24());

        if (isRelativeCoordinate()) {
			lOrtho2D.setData(0, 0, lScreenSize[X] * lOrtho2D.m11());
			lOrtho2D.setData(1, 1, lScreenSize[Y] * lOrtho2D.m22());
		}

		lpContext->projection().set(lOrtho2D);
		lpContext->modelView().identity();

        if (isShowAlways()) {
			lpContext->setDepthRange(lDepthRange[0], lDepthRange[0]);
        } else {
			Float lT = 0.5f * lVertex[Z] + 0.5f;
			lT = lDepthRange[1] * lT + (1.0f - lT) * lDepthRange[0];

			lpContext->setDepthRange(lT, lT);
		}

        if (isClipping()) {
			Int32 lBox[4];

            if (lScissorEnabled) {
				Int32 lContextBox[4] = {	lScissorBox[0],
											lScissorBox[1],
											lScissorBox[0] + lScissorBox[2],
											lScissorBox[1] + lScissorBox[3] };

				lBox[0] = o3d::max<Int32>(lContextBox[0], lScreenPos[X] - lScreenSize[X]/2 + lBorderSize);
				lBox[1] = o3d::max<Int32>(lContextBox[1], lScreenPos[Y] - lScreenSize[Y]/2 + lBorderSize);
				lBox[2] = o3d::min<Int32>(lContextBox[2], lScreenPos[X] + lScreenSize[X]/2 - lBorderSize);
				lBox[3] = o3d::min<Int32>(lContextBox[3], lScreenPos[Y] + lScreenSize[Y]/2 - lBorderSize);
            } else {
				lBox[0] = lScreenPos[X] - lScreenSize[X]/2 + lBorderSize;
				lBox[1] = lScreenPos[Y] - lScreenSize[Y]/2 + lBorderSize;
				lBox[2] = lScreenPos[X] + lScreenSize[X]/2 - lBorderSize;
				lBox[3] = lScreenPos[Y] + lScreenSize[Y]/2 - lBorderSize;
			}

			lpContext->setScissor(lBox[0], lBox[1], lBox[2] - lBox[0], lBox[3] - lBox[1]);
			lpContext->enableScissorTest();
		}

        Context::AntiAliasingMethod aa = lpContext->setAntiAliasing(Context::AA_NONE);

		drawContent();

        lpContext->setAntiAliasing(aa);

        if (isClipping()) {
            if (!lScissorEnabled) {
				lpContext->disableScissorTest();
            }

			lpContext->setScissor(lScissorBox);
		}

		lpContext->setDepthRange(lDepthRange[0], lDepthRange[1]);

		lpContext->projection().set(lProjection);
		lpContext->modelView().set(lModelview);
	}
}

Float WorldLabel::computeSizeFactor(Float _distance)
{
	//f(z) = A/max(1,B + C.z + D.z^2)
	return m_eqnCoefficient[0] /
		o3d::max<Float>(1.0f, m_eqnCoefficient[1] +
			_distance * (m_eqnCoefficient[2] + m_eqnCoefficient[3] * _distance));
}

Bool WorldLabel::writeToFile(OutStream &os)
{
    if (SceneObject::writeToFile(os))
	{
        os << m_screenSize;
        os << m_eqnCoefficient;
        os << m_distanceBounds;
        os << m_showAlways;
        os << m_relativeCoordinate;
        os << m_clipping;
        os << m_borderSize;

		return True;
	}

	return False;
}

Bool WorldLabel::readFromFile(InStream &is)
{
    if (SceneObject::readFromFile(is))
	{
        is >> m_screenSize;
        is >> m_eqnCoefficient;
        is >> m_distanceBounds;
        is >> m_showAlways;
        is >> m_relativeCoordinate;
        is >> m_clipping;
        is >> m_borderSize;

		return True;
	}

	return False;
}
