/**
 * @file configs.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/landscape/pclod/configs.h"

#include "o3d/engine/landscape/pclod/terrain.h"
#include "o3d/engine/shader/shadermanager.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/image/image.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(PCLODConfigs, ENGINE_PCLOD_CONFIG, SceneEntity)

/* Constructors */
PCLODConfigs::PCLODConfigs(BaseObject *pParent) :
	SceneEntity(pParent),
	m_distanceOnlyMaterial(15.0f),
	m_distanceOnlyColormap(50.0f),
	m_shadersPath(),
	m_useLog(True),
	m_showMessage(True),
	m_showWarning(True),
	m_showError(True),
	m_colormapPolicy(COLORMAP_AUTO),
	m_colormapPrecision(1),
	m_useColormapStaticNoise(False),
	m_pColormapNoisePicture(NULL),
	m_colormapStaticNoiseFactor(0.1f),
	m_useWireFrame(False),
	m_viewDistance(150),
	m_useAsynchRefresh(True),
	m_useFrustumCulling(True),
	m_useFrontToBack(True),
	m_frontToBackMinViewMove(10.0f),
	m_frontToBackRefreshPeriodicity(1),
	m_useDistance2d(True),
	m_useLightning(True),
	m_useSelfShadowing(True),
	m_lightMinCosAngleVariation(cosf(0.01f)),
    m_text(),
	m_useDebugLabel(False)
{
}

PCLODConfigs::~PCLODConfigs()
{
	deletePtr(m_pColormapNoisePicture);
}

/* Accessors */

PCLODTerrain* PCLODConfigs::getTerrain()
{
	return ((PCLODTerrain*)m_parent)->getTerrain();
}

void PCLODConfigs::init()
{
	PCLOD_MESSAGE(String("distanceOnlyMaterial set to ") << m_distanceOnlyMaterial);
	PCLOD_MESSAGE(String("distanceOnlyColormap set to ") << m_distanceOnlyColormap);
	PCLOD_MESSAGE(String("m_shadersFile set to ") << m_shadersPath);
	PCLOD_MESSAGE(String("useLog set to ") << m_useLog);
	PCLOD_MESSAGE(String("showMessage set to ") << m_showMessage);
	PCLOD_MESSAGE(String("showWarning set to ") << m_showWarning);
	PCLOD_MESSAGE(String("showError set to ") << m_showError);
	PCLOD_MESSAGE(String("colormapPolicy set to COLORMAP_AUTO"));
	PCLOD_MESSAGE(String("colormapPrecision set to ") << m_colormapPrecision);
	PCLOD_MESSAGE(String("colormapStaticNoise set to ") << m_useColormapStaticNoise);
	PCLOD_MESSAGE(String("colormapStaticNoiseFactor set to ") << m_colormapStaticNoiseFactor);
	PCLOD_MESSAGE(String("viewDistance set to ") << m_viewDistance);
	PCLOD_MESSAGE(String("useAsynchRefresh set to ") << m_useAsynchRefresh);
	PCLOD_MESSAGE(String("useFrustumCulling set to ") << m_useFrustumCulling);
	PCLOD_MESSAGE(String("useFrontToBack set to ") << m_useFrontToBack);
	PCLOD_MESSAGE(String("frontToBackMinViewMove set to ") << m_frontToBackMinViewMove);
	PCLOD_MESSAGE(String("frontToBackRefreshPeriodicity set to ") << m_frontToBackRefreshPeriodicity);
	PCLOD_MESSAGE(String("useDistance2d set to ") << m_useDistance2d);
	PCLOD_MESSAGE(String("useLightning set to ") << m_useLightning);
	PCLOD_MESSAGE(String("useSelfShadowing set to ") << m_useSelfShadowing);
	PCLOD_MESSAGE(String("lightMinCosAngleVariation set to ") << m_lightMinCosAngleVariation);
    PCLOD_MESSAGE(String("text2D set to ") << (m_text ? "DEFINED" : "UNDEFINED"));
	PCLOD_MESSAGE(String("debugLabel set to ") << m_useDebugLabel);
}

void PCLODConfigs::setShadersPath(const String & _path)
{
	Shader * lShader0 = NULL;
	Shader * lShader1 = NULL;
	Shader * lShaderSimple = NULL;

	String lPath(_path);
	lPath.trimRight('/');

	try
	{
		lShader0 = getScene()->getShaderManager()->addShader(lPath + '/' + "shader1");
		lShader1 = getScene()->getShaderManager()->addShader(lPath + '/' + "shader2");
		lShaderSimple = getScene()->getShaderManager()->addShader("ambient");
	}
	catch(const E_FileNotFoundOrInvalidRights & _ex)
	{
		PCLOD_ERROR(String("Configs : Error during shaders file loading <") << _ex.getMsg() << ">");
	}
	catch(const E_InvalidParameter & _ex)
	{
		PCLOD_ERROR(String("Configs : Error during shaders loading <") << _ex.getMsg() << ">");
	}

	for (UInt32 k = 0 ; k < 3 ;++k)
		m_shaders[k].detach();

	m_simpleShader.detach();

	if ((lShader0 == NULL) || (lShader1 == NULL) || (lShaderSimple == NULL))
	{
		PCLOD_ERROR(String("Configs : Error during shaders loading from <") << lPath << ">. Shaders won't be available.");
	}
	else
	{
		lShader0->buildInstance(m_shaders[0]);
		lShader1->buildInstance(m_shaders[1]);
		lShaderSimple->buildInstance(m_simpleShader);

		m_shaders[0].assign(0, 0, -1, "");
		m_shaders[1].assign(0, 0, -1, "");
		m_simpleShader.assign("default", "default", "", "MESH");

		m_shadersPath = lPath;

		PCLOD_MESSAGE(String("Configs : Shaders from <") << lPath << "> successfully opened.");
	}
}

Bool PCLODConfigs::shadersInit() const
{
	return (m_shaders[0].isValid());
}

void PCLODConfigs::getShader(UInt8 index, ShaderInstance & _pInstance) const
{
	O3D_ASSERT(index < 3);

	_pInstance = m_shaders[index];
}

void PCLODConfigs::getSimpleShader(ShaderInstance & _pInstance) const
{
	_pInstance = m_simpleShader;
}

void PCLODConfigs::enableLog(Bool _state)
{
	m_useLog = _state;
}

Bool PCLODConfigs::logEnabled() const
{
	return m_useLog;
}

void PCLODConfigs::showMessage(Bool _state)
{
	m_showMessage = _state;
}

Bool PCLODConfigs::messageShown() const
{
	return m_showMessage;
}

void PCLODConfigs::showWarning(Bool _state)
{
	m_showWarning = _state;
}

Bool PCLODConfigs::warningShown() const
{
	return m_showWarning;
}

void PCLODConfigs::showError(Bool _state)
{
	m_showError = _state;
}

Bool PCLODConfigs::errorShown() const
{
	return m_showError;
}

void PCLODConfigs::setColormapPolicy(PCLODColormapPolicy _policy)
{
	m_colormapPolicy = _policy;
}

PCLODConfigs::PCLODColormapPolicy PCLODConfigs::colormapPolicy() const
{
	return m_colormapPolicy;
}

void PCLODConfigs::setColormapPrecision(UInt8 _precision)
{
	if (_precision > 4)
		_precision = 4;

	m_colormapPrecision = _precision;
}

UInt8 PCLODConfigs::colormapPrecision() const
{
	return m_colormapPrecision;
}

void PCLODConfigs::enableColormapStaticNoise(Bool _state)
{
	m_useColormapStaticNoise = _state;

    if (_state && (!m_pColormapNoisePicture || m_pColormapNoisePicture->isEmpty()))
        PCLOD_WARNING("Configs : Colormap noise used but no picture is defined");
}

Bool PCLODConfigs::useColormapStaticNoise() const
{
	return m_useColormapStaticNoise;
}

void PCLODConfigs::setColormapStaticNoise(const Image & _picture)
{
	if ((m_pColormapNoisePicture == NULL) && (!_picture.isEmpty()))
		m_pColormapNoisePicture = new Image;

	*m_pColormapNoisePicture = _picture;
}

const Image * PCLODConfigs::colormapStaticNoisePicture() const
{
	return m_pColormapNoisePicture;
}

void PCLODConfigs::setColormapStaticNoiseFactor(Float _fact)
{
	m_colormapStaticNoiseFactor = _fact;
}

Float PCLODConfigs::colormapStaticNoiseFactor() const
{
	return m_colormapStaticNoiseFactor;
}

void PCLODConfigs::setViewDistance(UInt32 _distance)
{
	m_viewDistance = _distance;
}

UInt32 PCLODConfigs::viewDistance() const
{
	return m_viewDistance;
}

void PCLODConfigs::enableWireFrame(Bool _state)
{
	m_useWireFrame = _state;
}

Bool PCLODConfigs::wireFrame() const
{
	return m_useWireFrame;
}

void PCLODConfigs::enableAsynchRefresh(Bool _state)
{
	m_useAsynchRefresh = _state;
}

Bool PCLODConfigs::asynchRefresh() const
{
	return m_useAsynchRefresh;
}

void PCLODConfigs::setRefreshFrequency(UInt32 _frequency)
{
	m_refreshFrequency = _frequency;
}

UInt32 PCLODConfigs::refreshFrequency() const
{
	return m_refreshFrequency;
}

void PCLODConfigs::enableLightmapLod(Bool _state)
{
	m_useLightmapLod = _state;
}

Bool PCLODConfigs::lightmapLod() const
{
	return m_useLightmapLod;
}

void PCLODConfigs::clearLightmapCurve()
{
	m_lightmapLodCurve.clear();
}

Bool PCLODConfigs::setLightmapPoint(Float _x, Int8 _level)
{
	if ((_level > 0) || (_x < 0.0f))
		return False;

	m_lightmapLodCurve[_x] = _level;
	return True;
}

Bool PCLODConfigs::getLightmapPoint(UInt32 _index, Float & _x, Int8 & _level) const
{
	if (m_lightmapLodCurve.size() == 0)
	{
		_x = 0.0f;
		_level = 0;

		return False;
	}

	if (_index >= m_lightmapLodCurve.size())
	{
		_x = m_lightmapLodCurve.rbegin()->first;
		_level = m_lightmapLodCurve.rbegin()->second;

		return False;
	}
	else
	{
		CIT_Curve it = m_lightmapLodCurve.begin();

		for (UInt32 k = 0 ; (k < _index) && (k < UInt32(m_lightmapLodCurve.size())) ; ++k, it++) {}

		_x = it->first;
		_level = it->second;

		return True;
	}
}

void PCLODConfigs::getLightmapPoint(Float _distance, Int8 & _value) const
{
	if (m_lightmapLodCurve.size() == 0)
		_value = 0;
	else
	{
		CIT_Curve it = m_lightmapLodCurve.lower_bound(_distance);

		if (it == m_lightmapLodCurve.end())
			it = m_lightmapLodCurve.begin();

		_value = it->second;
	}
}

UInt32 PCLODConfigs::lightmapCurvePointCount() const
{
	return UInt32(m_lightmapLodCurve.size());
}

void PCLODConfigs::enableColormapLod(Bool _state)
{
	m_useColormapLod = _state;
}

Bool PCLODConfigs::colormapLod() const
{
	return m_useColormapLod;
}

void PCLODConfigs::clearColormapCurve()
{
	m_colormapLodCurve.clear();
}

Bool PCLODConfigs::setColormapPoint(Float _x, Int8 _level)
{
	if ((_level > 0) || (_x < 0.0f))
		return False;

	m_colormapLodCurve[_x] = _level;
	return True;
}

Bool PCLODConfigs::getColormapPoint(UInt32 _index, Float & _x, Int8 & _level) const
{
	if (m_colormapLodCurve.size() == 0)
	{
		_x = 0.0f;
		_level = 0;

		return False;
	}

	if (_index >= m_colormapLodCurve.size())
	{
		_x = m_colormapLodCurve.rbegin()->first;
		_level = m_colormapLodCurve.rbegin()->second;

		return False;
	}
	else
	{
		CIT_Curve it = m_colormapLodCurve.begin();

		for (UInt32 k = 0 ; (k < _index) && (k < UInt32(m_colormapLodCurve.size())) ; ++k, it++) {}

		_x = it->first;
		_level = it->second;

		return True;
	}
}

void PCLODConfigs::getColormapPoint(Float _distance, Int8 & _value) const
{
	if (m_colormapLodCurve.size() == 0)
		_value = 0;
	else
	{
		CIT_Curve it = m_colormapLodCurve.lower_bound(_distance);

		if (it == m_colormapLodCurve.end())
			it = m_colormapLodCurve.begin();

		_value = it->second;
	}
}

UInt32 PCLODConfigs::colormapCurvePointCount() const
{
	return UInt32(m_colormapLodCurve.size());
}

void PCLODConfigs::enableFrustumCulling(Bool _state)
{
	m_useFrustumCulling = _state;
}

Bool PCLODConfigs::frustumCulling() const
{
	return m_useFrustumCulling;
}

void PCLODConfigs::enableFrontToBack(Bool _state)
{
	m_useFrontToBack = _state;
}

Bool PCLODConfigs::frontToBack() const
{
	return m_useFrontToBack;
}

void PCLODConfigs::setFrontToBackMinViewMove(Float _value)
{
	m_frontToBackMinViewMove = _value;
}

Float PCLODConfigs::frontToBackMinViewMove() const
{
	return m_frontToBackMinViewMove;
}

void PCLODConfigs::setFrontToBackRefreshPeriodicity(UInt32 _period)
{
	m_frontToBackRefreshPeriodicity = _period;
}

UInt32 PCLODConfigs::frontToBackRefreshPeriodicity() const
{
	return m_frontToBackRefreshPeriodicity;
}

void PCLODConfigs::setDistance2d(Bool _state)
{
	m_useDistance2d = _state;
}

Bool PCLODConfigs::distance2d() const
{
	return m_useDistance2d;
}

void PCLODConfigs::setLightMinAngleVariation(Float _angle)
{
	m_lightMinCosAngleVariation = cosf(_angle);
}

void PCLODConfigs::enableLightning(Bool _state)
{
	m_useLightning = _state;
}

Bool PCLODConfigs::lightningEnabled() const
{
	return m_useLightning;
}

void PCLODConfigs::enableSelfShadowing(Bool _state)
{
	m_useSelfShadowing = _state;
}

Bool PCLODConfigs::lightningSelfShadowing() const
{
	return m_useSelfShadowing;
}

Float PCLODConfigs::getLightMinAngleVariation() const
{
	return acos(m_lightMinCosAngleVariation);
}

Float PCLODConfigs::getLightMinCosAngleVariation() const
{
	return m_lightMinCosAngleVariation;
}

void PCLODConfigs::enableDebugLabel(Bool _state)
{
	m_useDebugLabel = _state;
}

Bool PCLODConfigs::debugLabelEnabled() const
{
	return m_useDebugLabel;
}

