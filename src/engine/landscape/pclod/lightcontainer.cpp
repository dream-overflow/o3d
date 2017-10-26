/**
 * @file lightcontainer.cpp
 * @brief Implementation of LightContainer.h
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2006-04-13
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/landscape/pclod/lightcontainer.h"

#include "o3d/geom/bsphere.h"
#include "o3d/geom/bcone.h"
#include "o3d/engine/object/light.h"

#include "o3d/engine/landscape/pclod/pclodtexturemanager.h"
#include "o3d/engine/landscape/pclod/lightmap.h"
#include "o3d/engine/landscape/pclod/object.h"
#include "o3d/engine/landscape/pclod/terrain.h"
#include "o3d/engine/landscape/pclod/configs.h"

#include "o3d/engine/landscape/pclod/genlightmap.h"

using namespace o3d;

PCLODLight::PCLODLight(BaseObject * _pParent, const TerrainBase::LightInfos & _infos):
	BaseObject(_pParent),
	m_forceRefresh(False),
	m_frameIndex(-1),
	m_lightPosition(),
	m_lightColor(),
	m_lightAmbiant(),
	m_lightDirection(),
	m_spotCutoff(0.0f),
	m_spotExponent(0.0f),
	m_spotCoefficients(),
	m_pLight(this, _infos.pLight),
	m_lightInfos(_infos),
	m_lightmaps()
{
}

PCLODLight::~PCLODLight()
{
	for (IT_LightArray it = m_lightmaps.begin() ; it != m_lightmaps.end() ; it++)
		(*it)->mtRemoveLight(this);

	m_lightmaps.clear();

	m_pLight.releaseCheckAndDelete();
}

void PCLODLight::mtUpdate()
{
	if (isDynamicLight())
		m_forceRefresh = False;
}

void PCLODLight::rtUpdate()
{
	if (needUpdate())
	{
		if (isStaticLight())
			m_forceRefresh = False;

		switch(m_pLight->getLightType())
		{
			case Light::POINT_LIGHT:
				{
					m_lightDirection = m_pLight->getWorldDirection();
					m_lightPosition.set(m_pLight->getWorldPosition().getData());

					m_lightColor.set(m_pLight->getDiffuse().getData());
					m_lightAmbiant.set(m_pLight->getAmbient().getData());
					m_spotExponent = m_pLight->getExponent();
					m_spotCutoff = o3d::PI;
					m_spotCoefficients = m_pLight->getAttenuation();
				}
				break;

			case Light::SPOT_LIGHT:
				{
					m_lightDirection = m_pLight->getWorldDirection();
					m_lightPosition.set(m_pLight->getWorldPosition().getData());

					m_lightColor.set(m_pLight->getDiffuse().getData());
					m_lightAmbiant.set(m_pLight->getAmbient().getData());
					m_spotExponent = m_pLight->getExponent();
					m_spotCutoff = o3d::toRadian(m_pLight->getCutOff());
					m_spotCoefficients = m_pLight->getAttenuation();
				}
				break;

			case Light::DIRECTIONAL_LIGHT:
				{
					m_lightDirection = m_pLight->getWorldDirection();

					if (getTerrain()->getCurrentConfigs().lightningSelfShadowing())
						GenLightMap::buildShadowMapFromDirectionalLight(this, m_lightDirection, m_lightmaps);

					m_lightColor.set(m_pLight->getDiffuse().getData());
					m_lightAmbiant.set(m_pLight->getAmbient().getData());
				}
				break;

			default:
				PCLOD_WARNING(String("Call to O3DPCLODLight::RT_Update(...) but the light type is not supported"));
				break;
		}

		m_frameIndex++;
	}
}

Bool PCLODLight::deleteIt()
{
	return True;
}

Bool PCLODLight::generateLightmap(
		PCLODLightInfo & _info,
		PCLODTopZone * _pTopZone,
		const TemplateArray2D<Vector3> & _normalMap,
		TemplateArray2D<Vector3> & _lightmap,
		Vector2i & _computedPosition,
		Vector2i & _computedSize,
		UInt32 _level)
{
	O3D_ASSERT(_pTopZone != NULL);
	O3D_ASSERT(!_normalMap.isEmpty());
	O3D_ASSERT((_pTopZone->getZoneSize()[X] == _lightmap.width()+1) && (_pTopZone->getZoneSize()[Y] == _lightmap.height()+1));
	O3D_ASSERT(UInt32(_normalMap.elt()) == _lightmap.elt());

	switch(m_pLight->getLightType())
	{
		case Light::POINT_LIGHT: // firebird to Luhtor TODO logiquement pareil car le cutOff a ete definit a o3d::Pi
		case Light::SPOT_LIGHT:
			{
				const Vector2f & lUnits = _pTopZone->getZoneUnits();
				const Float lCosCutoff = cosf(m_spotCutoff);
				const Vector3 lDiffuseColor(m_lightColor * 0.25f);
				const Vector3 lCorrectedDirection(-m_lightDirection[Z], -m_lightDirection[X], -m_lightDirection[Y]);
				const Vector3 * lpNormal = _normalMap.getData();

				SmartArrayFloat lHeightmap;
				Vector2ui lHeightmapSize;
				_pTopZone->getHeightmap(lHeightmap, lHeightmapSize);

				const Float * lpHeight = lHeightmap.getData();

				Vector3 lPosition(_pTopZone->getWorldOrigin());
				Vector3 lTmpVect;
				Float lTmpVal;

				for (UInt32 j = 0 ; j < _lightmap.height() ; ++j, lPosition[X] += lUnits[Y])
				{
					lPosition[Z] = _pTopZone->getWorldOrigin()[Z];

					for (UInt32 i = 0 ; i < _lightmap.width() ; ++i, lPosition[Z] += lUnits[X], ++lpNormal, ++lpHeight)
					{
						lPosition[Y] = *lpHeight;
						lTmpVect = (lPosition - m_lightPosition);
						lTmpVect.normalize();

						lTmpVal = lTmpVect * m_lightDirection;

						if (lTmpVal > lCosCutoff)
						{
							lTmpVal = powf((lTmpVal - lCosCutoff)/(1.0f - lCosCutoff), m_spotExponent);

							lTmpVal *= (lCorrectedDirection * (*lpNormal));

							_lightmap(i,j) += lDiffuseColor * lTmpVal;
						}
					}
				}

				return True;
			}
			break;

		case Light::DIRECTIONAL_LIGHT:
			{
				Int32 lNormalStep = 1 << _info.lightmapLevel;

				const Vector3 * lpNormal = _normalMap.getData();
				Vector3 * lpLight = _lightmap.getData();

				const Vector3 lDiffuseColor(m_lightColor * 0.25f);
				const Vector3 lAmbiantColor(m_lightAmbiant * 0.25f);
				const Vector3 lLightDirection(-m_lightDirection[Z], -m_lightDirection[X], -m_lightDirection[Y]);

				if (getTerrain()->getCurrentConfigs().lightningSelfShadowing())
				{
					if (_info.shadowMap.isEmpty())
						return False;

					Float * lpShadowRow0 = _info.shadowMap.getData();
					Float * lpShadowRow1 = _info.shadowMap.getData() + (_lightmap.width()+1);
					Float lShadowValue = 0.0f;

					for (UInt32 j = _lightmap.height() ; j != 0 ; --j, ++lpShadowRow0, ++lpShadowRow1)
					{
						for (UInt32 i = _lightmap.width() ; i != 0 ; --i, lpNormal += lNormalStep, ++lpLight, ++lpShadowRow0, ++lpShadowRow1)
						{
							lShadowValue =	(*lpShadowRow0 != 0.0f ? 0.0f: 0.25f) +
											(*(lpShadowRow0+1) != 0.0f ? 0.0f: 0.25f) +
											(*lpShadowRow1 != 0.0f ? 0.0f: 0.25f) +
											(*(lpShadowRow1+1) != 0.0f ? 0.0f: 0.25f);

							if (lShadowValue != 0.0f)
								(*lpLight) += lDiffuseColor * o3d::max<Float>(lLightDirection * (*lpNormal), 0.0f) * lShadowValue + lAmbiantColor;
							else
								(*lpLight) += lAmbiantColor;
						}
					}
				}
				else
				{
					for (UInt32 i = _lightmap.elt() ; i != 0 ; --i, lpNormal += lNormalStep, ++lpLight)
						(*lpLight) += lDiffuseColor * o3d::max<Float>(lLightDirection * (*lpNormal), 0.0f) + lAmbiantColor;
				}

				_info.frameIndex = m_frameIndex;

				return True;
			}
			break;

		default:
			PCLOD_WARNING(String("Call to O3DPCLODLight::generateLightmap(...) but the light type is not supported"));
			return False;
	}
}

Bool PCLODLight::needUpdate(const PCLODLightInfo & _lightInfos) const
{
	if (m_lightInfos.update == TerrainBase::LIGHT_UPDATE_MANUAL)
		return False;

	return (_lightInfos.frameIndex != m_frameIndex);
}

Float PCLODLight::getSpotDepth() const
{
	const Float lInvTreshold = 1.0f/0.05f;

	Float lLightDepth = 0.0f; // Means infinity

	// On cherche x tel que x = Coef[0] + Coef[1] * z + Coef[2] * z^2
	if (m_spotCoefficients[0] < lInvTreshold)
	{
		if (m_spotCoefficients[2] != 0)
		{
			lLightDepth = -m_spotCoefficients[1] + sqrt(m_spotCoefficients[1]*m_spotCoefficients[1] + 4.0f*m_spotCoefficients[2]*(lInvTreshold - m_spotCoefficients[0]));
			lLightDepth /= 2.0f*m_spotCoefficients[2];
		}
		else
		{
			lLightDepth = (lInvTreshold - m_spotCoefficients[0])/m_spotCoefficients[1];
		}
	}

	return lLightDepth;
}

void PCLODLight::invalidate()
{
	m_forceRefresh = True;
}

Bool PCLODLight::mtAffectLightmap(const PCLODLightmap * _pLightmap) const
{
	O3D_ASSERT(_pLightmap != NULL);
	O3D_ASSERT(m_pLight);

	switch(m_pLight->getLightType())
	{
		case Light::POINT_LIGHT:
			return _pLightmap->getZone()->getBoundingBox().intersect(BSphere(m_lightPosition, getSpotDepth()));
		case Light::SPOT_LIGHT:
			return BCone(m_lightPosition, m_lightDirection, getSpotDepth(), m_spotCutoff, True).intersect(_pLightmap->getZone()->getBoundingBox());
		case Light::DIRECTIONAL_LIGHT:
			return True;
		default:
			PCLOD_WARNING(String("Call to O3DPCLODLight::MT_AffectLightmap(...) but the light type is not supported"));
			return False;
	}
}

void PCLODLight::mtAddLightmap(PCLODLightmap * _pLightmap)
{
	O3D_ASSERT(std::find(m_lightmaps.begin(), m_lightmaps.end(), _pLightmap) == m_lightmaps.end());

	_pLightmap->mtAddLight(this);
	m_lightmaps.push_back(_pLightmap);

	invalidate();
}

void PCLODLight::mtRemoveLightmap(PCLODLightmap * _pLightmap)
{
	IT_LightArray itLight = std::find(m_lightmaps.begin(), m_lightmaps.end(), _pLightmap);
	O3D_ASSERT(itLight != m_lightmaps.end());

	_pLightmap->mtRemoveLight(this);
	m_lightmaps.erase(itLight);
}

Bool PCLODLight::isStaticLight() const
{
	return (m_lightInfos.type != TerrainBase::LIGHT_TYPE_DYNAMIC);
}

Bool PCLODLight::isDynamicLight() const
{
	return (m_lightInfos.type == TerrainBase::LIGHT_TYPE_DYNAMIC);
}

const PCLODTerrain * PCLODLight::getTerrain() const
{
	return getTextureManager()->getTerrain();
}

const PCLODTextureManager * PCLODLight::getTextureManager() const
{
	O3D_ASSERT(getParent() != NULL);

	return (PCLODTextureManager*)getParent();
}

Bool PCLODLight::needUpdate()
{
	if (m_forceRefresh)
		return True;

	switch(m_pLight->getLightType())
	{
		case Light::POINT_LIGHT:
			{
				Vector3 lDirection = m_pLight->getAbsoluteMatrix().getZ();

				Vector3 lSpotCoefs = m_pLight->getAttenuation();

				Bool lNeedUpdate = False;
				lNeedUpdate = lNeedUpdate || ((m_lightPosition - Vector3(m_pLight->getWorldPosition().getData())).normInf() > 0.1f);
				lNeedUpdate = lNeedUpdate || ((lDirection * m_lightDirection) < getTerrain()->getCurrentConfigs().getLightMinCosAngleVariation());
				lNeedUpdate = lNeedUpdate || ((m_lightColor - Vector3(m_pLight->getDiffuse().getData())).normInf() > 0.01f);
				lNeedUpdate = lNeedUpdate || ((m_lightAmbiant - Vector3(m_pLight->getAmbient().getData())).normInf() > 0.01f);
				lNeedUpdate = lNeedUpdate || o3d::abs<Float>(m_spotExponent - m_pLight->getExponent()) > 1.0f;
				lNeedUpdate = lNeedUpdate || ((m_spotCoefficients - lSpotCoefs).normInf() > 0.01f);
				// firebird to Luhtor => TODO logiquement c'est pareil ici

				return lNeedUpdate;
			}
			break;

		case Light::SPOT_LIGHT:
			{
				Vector3 lDirection = m_pLight->getAbsoluteMatrix().getZ();

				Vector3 lSpotCoefs = m_pLight->getAttenuation();

				Bool lNeedUpdate = False;
				lNeedUpdate = lNeedUpdate || ((m_lightPosition - Vector3(m_pLight->getWorldPosition().getData())).normInf() > 0.1f);
				lNeedUpdate = lNeedUpdate || ((lDirection * m_lightDirection) < getTerrain()->getCurrentConfigs().getLightMinCosAngleVariation());
				lNeedUpdate = lNeedUpdate || ((m_lightColor - Vector3(m_pLight->getDiffuse().getData())).normInf() > 0.01f);
				lNeedUpdate = lNeedUpdate || ((m_lightAmbiant - Vector3(m_pLight->getAmbient().getData())).normInf() > 0.01f);
				lNeedUpdate = lNeedUpdate || o3d::abs<Float>(m_spotExponent - m_pLight->getExponent()) > 1.0f;
				lNeedUpdate = lNeedUpdate || o3d::abs<Float>(m_spotCutoff - m_pLight->getCutOff()) > 0.01f;
				lNeedUpdate = lNeedUpdate || ((m_spotCoefficients - lSpotCoefs).normInf() > 0.01f);
				// firebird to Luhtor TODO => j'ai rajoute la ligne pour mettre a jour le spotCutOff

				return lNeedUpdate;
			}
			break;

		case Light::DIRECTIONAL_LIGHT:
			{
				Vector3 lDirection = m_pLight->getAbsoluteMatrix().getZ();

				return ((lDirection * m_lightDirection < getTerrain()->getCurrentConfigs().getLightMinCosAngleVariation()) ||
						((m_lightColor - Vector3(m_pLight->getDiffuse().getData())).normInf() > 0.01f) ||
						((m_lightAmbiant - Vector3(m_pLight->getAmbient().getData())).normInf() > 0.01f));
			}
			break;

		default:
			PCLOD_WARNING(String("Call to O3DPCLODLight::needUpdate(...) but the light type is not supported"));
			return False;
	}
}

//---------------------------------------------------------------------------------------
// PCLODLightinfo
//---------------------------------------------------------------------------------------

PCLODLightInfo::PCLODLightInfo():
	frameIndex(-1),
	lightmapLevel(0),
	shadowMap(),
	shadowUpToDate(False)
{
}

PCLODLightInfo::~PCLODLightInfo()
{
}

