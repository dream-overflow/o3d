/**
 * @file scatteringmodeldefault.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/sky/scatteringmodeldefault.h"

#include "o3d/core/smartarray.h"
#include "o3d/engine/primitive/dome.h"
#include "o3d/image/imagetype.h"

using namespace o3d;

static inline Float getLuminance(const Float * _components)
{
	return 0.5f * (	o3d::max<Float>(_components[R], o3d::max<Float>(_components[G], _components[B])) + 
					o3d::min<Float>(_components[R], o3d::min<Float>(_components[G], _components[B])));
}

static inline Float getPhysicalBrightness(const ScatteringModelBase::TaskData & _datas, Float _luminance, Float _ref)
{
	return (_datas.postCoefA - _datas.postCoefC*expf(-_datas.postCoefB*_luminance - 10.0f * _luminance / _ref));
}

O3D_IMPLEMENT_DYNAMIC_CLASS1(ScatteringModelDefault, ENGINE_SCATTERING_MODEL_DEFAULT, ScatteringModelBase)

//! Default constructor
ScatteringModelDefault::ScatteringModelDefault(BaseObject * _pParent):
	ScatteringModelBase(_pParent)
{
}

//! The destructor
ScatteringModelDefault::~ScatteringModelDefault()
{
}

//! @brief Entry function of the model
ScatteringModelBase::TaskResult ScatteringModelDefault::entry(TaskData & _datas)
{
	O3D_ASSERT(_datas.pDome->isValid());
	O3D_ASSERT(_datas.colorArray.isEmpty() || (UInt32(_datas.colorArray.getNumElt()) == 3*_datas.pDome->getNumVertices()));

	if (_datas.colorArray.isEmpty())
		_datas.colorArray = SmartArrayFloat(3*_datas.pDome->getNumVertices());

	if (_datas.workingArray.getNumElt() != _datas.colorArray.getNumElt())
		_datas.workingArray = SmartArrayFloat(3*_datas.pDome->getNumVertices());

	// Constants computation
	const Float lPlanetRadiusM = 1000.0f*_datas.planetRadius;

	const UInt32 lStackCount = _datas.pDome->getStackCount();

	const Float lStepStart = o3d::min<Float>(_datas.rayDensityDecFactor, _datas.mieDensityDecFactor) / _datas.stepIndex;

	const UInt32 lColorThreshold1 = 16;
	const UInt32 lColorThreshold2 = 8;

	const Vector3 lAbsCamPosition(0.0f, _datas.camPosition[Y] + lPlanetRadiusM, 0.0f);

	memset((void*)_datas.colorArray.getData(), 0, 3*_datas.pDome->getNumVertices() * sizeof(Float));

	for (IT_ObjectArray itObject = _datas.objectArray.begin() ; itObject != _datas.objectArray.end() ; itObject++)
	{
		memset((void*)_datas.workingArray.getData(), 0, 3*_datas.pDome->getNumVertices() * sizeof(Float));

		const Vector3 lDirection(sinf(itObject->position[X]) * cosf(itObject->position[Y]),
									sinf(itObject->position[Y]),
									cosf(itObject->position[X]) * cosf(itObject->position[Y]));

		// On suppose pour le moment que l'astre est tres loin et donc que la direction par rapport
		// au sol est la meme que par rapport au centre de la terre
		const Vector3 lDirectionFromPlanet(lDirection);

		const Vector3 lRayCrossSection(getRayleighCrossSectionCoef(_datas, itObject->waveLength));
		const Vector3 lMieCrossSection(getMieCrossSectionCoef(_datas, itObject->waveLength));

		// On calcul le chemin optique entre l'objet et la camera
		// Dans ce bloc, on calcule la luminance de l'objet. Pour calculer le brightness, il
		// faut connaitre la luminance totale dans cette direction, c'est a dire celle due aux 
		// autres sources de lumiere.
		{
			Float lRayOptical, lMieOptical;

			getOpticalLength(	_datas,
								Vector3(0.0f, lPlanetRadiusM, 0.0f),
								lDirection,
								lRayOptical,
								lMieOptical);

			const Vector3 lOpticalLength(
				lRayCrossSection[R] * lRayOptical + lMieCrossSection[R] * lMieOptical,
				lRayCrossSection[G] * lRayOptical + lMieCrossSection[G] * lMieOptical,
				lRayCrossSection[B] * lRayOptical + lMieCrossSection[B] * lMieOptical);

			itObject->luminance.set(itObject->intensity[R] * expf(-lOpticalLength[R]),
									itObject->intensity[G] * expf(-lOpticalLength[G]),
									itObject->intensity[B] * expf(-lOpticalLength[B]));
		}

		// Conteneur pour stocker les pas d'integration
		std::vector<Float> lIntegrationDistance;
		std::vector<Float> lIntegrationStep;

		std::vector<Float> lRayIntegrationDensity;
		std::vector<Float> lRayIntegrationValue;

		std::vector<Float> lMieIntegrationDensity;
		std::vector<Float> lMieIntegrationValue;

		for (UInt32 k = 0 ; k < lStackCount ; ++k)
		{
			const UInt32 lCurrentStack = k; //...
			const UInt32 lVertexCount = _datas.pDome->getSliceCount(lCurrentStack);
			const Float * const lpVertexStart = _datas.pDome->getVerticesAtStack(lCurrentStack);
			Float * const lpColorStart = _datas.workingArray.getData() + (lpVertexStart - _datas.pDome->getVertices());
			const UInt32 lVertexStepStart = (lVertexCount >= lColorThreshold1 ? 4 : (lVertexCount >= lColorThreshold2 ? 2 : 1));
			UInt32 lVertexStep = lVertexStepStart;

			// Donnees geometriques constantes sur une meme stack
			const Float lVertexDistanceM = 1000.0f * (_datas.camPosition - Vector3(lpVertexStart)).length();

			// Integration numerique par trapezes
			{
				lIntegrationDistance.clear();
				lIntegrationStep.clear();

				lRayIntegrationDensity.clear();
				lRayIntegrationValue.clear();		// Integration de RhoR0 * exp(-h/T).ds

				lMieIntegrationDensity.clear();
				lMieIntegrationValue.clear();

				const Float lIRayDecFactorM = 1.0f/_datas.rayDensityDecFactor;
				const Float lIMieDecFactorM = 1.0f/_datas.mieDensityDecFactor;

				Vector3 lVertexDirection(lpVertexStart[0], lpVertexStart[1], lpVertexStart[2]);
				lVertexDirection.normalize();

				Float lStep = lStepStart;
				Float lDistance = 0.0f;
				Float lAltitude = 0.0f;

				Bool lContinue = True;

				for (;;)
				{
					lAltitude = (lAbsCamPosition + lVertexDirection * lDistance).length() - lPlanetRadiusM;

					lIntegrationDistance.push_back(lDistance);
					lIntegrationStep.push_back(lStep);

					lRayIntegrationDensity.push_back(expf(-lAltitude*lIRayDecFactorM));
					lMieIntegrationDensity.push_back(expf(-lAltitude*lIMieDecFactorM));

					lDistance += lStep;
					lStep *= _datas.stepFactor;

					if (!lContinue)
						break;
					else if (lDistance >= lVertexDistanceM)
					{
						lIntegrationStep.back() -= lDistance - lVertexDistanceM;
						lDistance = lVertexDistanceM;
						lContinue = False;
					}
				}

				lRayIntegrationValue.reserve(lIntegrationDistance.size() - 1);
				lMieIntegrationValue.reserve(lIntegrationDistance.size() - 1);

				std::vector<Float>::iterator lDistanceIt = lIntegrationDistance.begin();
				std::vector<Float>::iterator lStepIt = lIntegrationStep.begin();
				std::vector<Float>::iterator lRayDensityIt = lRayIntegrationDensity.begin();
				std::vector<Float>::iterator lMieDensityIt = lMieIntegrationDensity.begin();

				for ( ; lDistanceIt != lIntegrationDistance.end() - 1 ; lStep *= _datas.stepFactor,
																		lDistanceIt++,
																		lStepIt++,
																		lRayDensityIt++,
																		lMieDensityIt++)
				{
					*lDistanceIt = 0.5f * (*lDistanceIt + *(lDistanceIt + 1));
					*lRayDensityIt = 0.5f * _datas.rayDensityAtGround * (*lRayDensityIt + *(lRayDensityIt + 1));
					*lMieDensityIt = 0.5f * _datas.mieDensityAtGround * (*lMieDensityIt + *(lMieDensityIt + 1));

					if (lRayIntegrationValue.empty())
					{
						lRayIntegrationValue.push_back(*lStepIt * (*lRayDensityIt));
						lMieIntegrationValue.push_back(*lStepIt * (*lMieDensityIt));
					}
					else
					{
						lRayIntegrationValue.push_back(lRayIntegrationValue.back() + *lStepIt * (*lRayDensityIt));
						lMieIntegrationValue.push_back(lMieIntegrationValue.back() + *lStepIt * (*lMieDensityIt));
					}
				}

				lIntegrationStep.pop_back();
				lIntegrationDistance.pop_back();
				lRayIntegrationDensity.pop_back();
				lMieIntegrationDensity.pop_back();
			}

			while (lVertexStep > 0)
			{
				const Float * lpVertex = lpVertexStart;
				Float * lpColor = lpColorStart;

				for (Int32 i = 0 ; i < Int32(lVertexCount) ; i += lVertexStep , lpVertex += 3*lVertexStep, lpColor += 3*lVertexStep)
				{
					switch(lVertexStep)
					{
					case 4:
						if (i % 4 != 0)
							continue;
						break;
					case 2:
						if ((i % 2 != 0) || ((lVertexStep < lVertexStepStart) && (i % 4 == 0)))
							continue;
						break;
					case 1:
						if ((lVertexStep < lVertexStepStart) && (i % 2 == 0))
							continue;
						break;
					default:
						O3D_ASSERT(0);
						break;
					}

					if ((lVertexStep < lVertexStepStart) && (_datas.colorInterpolation))
					{
						Int32 lNeighbor1 = i - (i % (2*lVertexStep));
						Int32 lNeighbor2 = lNeighbor1 + 2*lVertexStep;

						if (lNeighbor1 < 0)
							lNeighbor1 += lVertexCount - 1;
						if (lNeighbor2 > Int32(lVertexCount - 1))
							lNeighbor2 -= lVertexCount - 1;

						Vector3 lColor1(lpColorStart + 3*lNeighbor1);
						Vector3 lColor2(lpColorStart + 3*lNeighbor2);
						
						if ((lColor1 - lColor2).normInf() <= _datas.colorThreshold)
						{
							lpColor[R] = 0.5f*(lColor1[R]+lColor2[R]);
							lpColor[G] = 0.5f*(lColor1[G]+lColor2[G]);
							lpColor[B] = 0.5f*(lColor1[B]+lColor2[B]);

							continue;
						}
					}

					Vector3 lRayOpticalLength, lMieOpticalLength;
					Vector3 lVertexDirection(lpVertex[0], lpVertex[1], lpVertex[2]);
					lVertexDirection.normalize();

					const Float lCosAlpha = lVertexDirection * lDirection;
					const Float lRayPhaseFunction = 3.0f/(16.0f*o3d::PI) * (_datas.rayleighPhaseFunctionCoef1 + _datas.rayleighPhaseFunctionCoef2 * lCosAlpha*lCosAlpha);
					const Float lMiePhaseFunction = getHenyeyGreensteinFunction(lCosAlpha, _datas.mieAniso);

					const Vector3 lPrecomputedRayFactor(	lRayCrossSection[R]*lRayPhaseFunction,
															lRayCrossSection[G]*lRayPhaseFunction,
															lRayCrossSection[B]*lRayPhaseFunction);
					
					const Vector3 lPrecomputedMieFactor(	lMieCrossSection[R]*lMiePhaseFunction,
															lMieCrossSection[G]*lMiePhaseFunction,
															lMieCrossSection[B]*lMiePhaseFunction);

					Int32 lStartIndex = 0;

					// Signifie que le soleil est sous l'horizon et qu'il faut des calculs supplementaires
					if (lDirection[Y] < 0.0f)
					{
						Vector3 lSkyVertex(lpVertex);
						lSkyVertex += lAbsCamPosition * 0.001f;

						Vector3 lSkyToObject(lDirectionFromPlanet * itObject->distance - lSkyVertex);
						lSkyToObject.normalize();

						//lDirectionFromPlanet
						Vector3 lU3(lSkyVertex ^ lDirectionFromPlanet);
						Vector3 lRadiusDir(lSkyToObject ^ lU3);

						lRadiusDir.normalize();

						if (o3d::abs<Float>(lSkyVertex * lRadiusDir) <= _datas.planetRadius)
							continue;

						const Float lStartDistance = (lPlanetRadiusM - lAbsCamPosition[Y] * lRadiusDir[Y]) / (lVertexDirection * lRadiusDir);

						for (Int32 k = 0 ; k < Int32(lIntegrationDistance.size()) ; ++k)
						{
							if (lIntegrationDistance[k] > lStartDistance)
							{
								lStartIndex = o3d::max<Int32>(k - 1, 0);
								break;
							}
						}
					}

					for (Int32 p = lStartIndex ; p < Int32(lIntegrationDistance.size()) ; ++p)
					{
						const Vector3 lPosition(Vector3(0.0f, lPlanetRadiusM, 0.0f) + lVertexDirection * lIntegrationDistance[p]);

						Float lRayOpticalLength, lMieOpticalLength;
						getOpticalLength(	_datas,
											lPosition,
											lDirection,
											lRayOpticalLength,
											lMieOpticalLength);

						const Float lRayDensity = lRayIntegrationDensity[p];
						const Float lMieDensity = lMieIntegrationDensity[p];

						const Float & lRayOpticalLengthCameraToPoint = lRayIntegrationValue[p];
						const Float & lMieOpticalLengthCameraToPoint = lMieIntegrationValue[p];

						const Vector3 lPathCamPosition(
							lRayCrossSection[R] * lRayOpticalLengthCameraToPoint + lMieCrossSection[R] * lMieOpticalLengthCameraToPoint,
							lRayCrossSection[G] * lRayOpticalLengthCameraToPoint + lMieCrossSection[G] * lMieOpticalLengthCameraToPoint,
							lRayCrossSection[B] * lRayOpticalLengthCameraToPoint + lMieCrossSection[B] * lMieOpticalLengthCameraToPoint);

						const Vector3 lPathPositionSky(
							lRayCrossSection[R] * lRayOpticalLength + lMieCrossSection[R] * lMieOpticalLength,
							lRayCrossSection[G] * lRayOpticalLength + lMieCrossSection[G] * lMieOpticalLength,
							lRayCrossSection[B] * lRayOpticalLength + lMieCrossSection[B] * lMieOpticalLength);

						lpColor[R] += (lRayDensity * lPrecomputedRayFactor[R] + lMieDensity * lPrecomputedMieFactor[R]) * expf( - lPathCamPosition[R] - lPathPositionSky[R]) * lIntegrationStep[p];
						lpColor[G] += (lRayDensity * lPrecomputedRayFactor[G] + lMieDensity * lPrecomputedMieFactor[G]) * expf( - lPathCamPosition[G] - lPathPositionSky[G]) * lIntegrationStep[p];
						lpColor[B] += (lRayDensity * lPrecomputedRayFactor[B] + lMieDensity * lPrecomputedMieFactor[B]) * expf( - lPathCamPosition[B] - lPathPositionSky[B]) * lIntegrationStep[p];
					}

					lpColor[R] *= itObject->intensity[R];
					lpColor[G] *= itObject->intensity[G];
					lpColor[B] *= itObject->intensity[B];
				}

				lVertexStep /= 2;
			}
		}

		const Float * lpColorSource = _datas.workingArray.getData();
		Float * lpColorPtr = _datas.colorArray.getData();

		for (UInt32 k = _datas.colorArray.getNumElt() ; k > 0 ; --k, ++lpColorPtr, ++lpColorSource)
			*lpColorPtr += *lpColorSource;
	}

	// On determine la luminance maximale qui sert de reference pour la suite
	Float lMaxLuminance = 0.0f;
	Float * lpColorPtr = _datas.colorArray.getData();

	for (UInt32 k = _datas.colorArray.getNumElt()/3 ; k > 0 ; --k, lpColorPtr += 3)
		lMaxLuminance = o3d::max<Float>(lMaxLuminance, 
			0.5f*(o3d::max<Float>(o3d::max<Float>(lpColorPtr[R], lpColorPtr[G]), lpColorPtr[B]) + o3d::min<Float>(o3d::min<Float>(lpColorPtr[R], lpColorPtr[G]), lpColorPtr[B])));

	// Avant de convertir les luminances en couleurs rgb, on calcule la brillance de chaque objet
	for (IT_ObjectArray itObject = _datas.objectArray.begin() ; itObject != _datas.objectArray.end() ; itObject++)
	{
		Vector2f lAngle(itObject->position);

		const Vector3 lDirection(sinf(itObject->position[X]) * cosf(itObject->position[Y]),
									sinf(itObject->position[Y]),
									cosf(itObject->position[X]) * cosf(itObject->position[Y]));

		if (lAngle[Y] > 0.5f * o3d::PI)
			lAngle[Y] = o3d::PI - lAngle[Y];

		lAngle[Y] = o3d::max<Float>(lAngle[Y], 0.0f);
		lAngle[Y] = o3d::min<Float>(lAngle[Y], o3d::PI);

		lAngle[X] = o3d::max<Float>(lAngle[X], 0.0f);
		lAngle[X] = o3d::min<Float>(lAngle[X], 2.0f*o3d::PI);

		const UInt32 lStackCount = _datas.pDome->getStackCount();
		const Float lStackIndexf = 2.0f / o3d::PI * lAngle[Y] * lStackCount;
		const UInt32 lStackFirst = UInt32(floorf(lStackIndexf));
		const UInt32 lStackNext = UInt32(ceilf(lStackIndexf));

		O3D_ASSERT(lStackFirst < lStackCount);
		O3D_ASSERT(lStackNext < lStackCount);

		// On connait maintenant quelle rangee de vertex sera utilisee pour l'interpolation
		// On determine maintenant les slices.
		const Float lSliceIndex1f = lAngle[0] / (2.0f * o3d::PI) * Float(_datas.pDome->getSliceCount(lStackFirst));
		const Float lSliceIndex2f = lAngle[0] / (2.0f * o3d::PI) * Float(_datas.pDome->getSliceCount(lStackNext));

		const Vector2f lSlices1(floorf(lSliceIndex1f), ceilf(lSliceIndex1f));
		const Vector2f lSlices2(floorf(lSliceIndex2f), ceilf(lSliceIndex2f));

		Float * lpColors[4];
		lpColors[0] = _datas.colorArray.getData() + ((_datas.pDome->getVerticesAtStack(lStackFirst) + 3*Int32(lSlices1[0])) - _datas.pDome->getVertices());
		lpColors[1] = _datas.colorArray.getData() + ((_datas.pDome->getVerticesAtStack(lStackFirst) + 3*(Int32(lSlices1[1]) % _datas.pDome->getSliceCount(lStackFirst))) - _datas.pDome->getVertices());
		lpColors[2] = _datas.colorArray.getData() + ((_datas.pDome->getVerticesAtStack(lStackNext) + 3*Int32(lSlices2[0])) - _datas.pDome->getVertices());
		lpColors[3] = _datas.colorArray.getData() + ((_datas.pDome->getVerticesAtStack(lStackNext) + 3*(Int32(lSlices2[1]) % _datas.pDome->getSliceCount(lStackNext))) - _datas.pDome->getVertices());

		const Float lLuminance[4] = {	getLuminance(lpColors[0]),
											getLuminance(lpColors[1]),
											getLuminance(lpColors[2]),
											getLuminance(lpColors[3]) };

		const Float lSlices1Inter = (lSliceIndex1f - lSlices1[0]) * lLuminance[1] + (lSlices1[1] - lSliceIndex1f) * lLuminance[0];
		const Float lSlices2Inter = (lSliceIndex2f - lSlices2[0]) * lLuminance[3] + (lSlices2[1] - lSliceIndex2f) * lLuminance[2];

		const Float lInterpolateLuminance = (lStackIndexf - lStackFirst) * lSlices2Inter + (lStackNext - lStackIndexf) * lSlices1Inter;

		itObject->brightness =	getPhysicalBrightness(_datas, lInterpolateLuminance, lMaxLuminance) -
								getPhysicalBrightness(_datas, o3d::max<Float>(0.0f, lInterpolateLuminance - getLuminance(itObject->luminance.getData())), lMaxLuminance);
	}

	// Conversion des luminances physiques en couleurs rgb
	if (!_datas.objectArray.empty())
	{
		lpColorPtr = _datas.colorArray.getData();

		for (UInt32 k = _datas.colorArray.getNumElt()/3 ; k > 0 ; --k, lpColorPtr += 3)
				convertToRGB(_datas, lpColorPtr, lMaxLuminance);
	}

	return ScatteringModelBase::RESULT_COMPLETED;
}

Float ScatteringModelDefault::getHenyeyGreensteinFunction(const Float _lCosAlpha, const Float _lG) const
{
	const Float lGSquare = _lG*_lG;

	return (1.0f - lGSquare)/(4.0f*o3d::PI*powf(1.0f + lGSquare - 2.0f * _lG * _lCosAlpha, 1.5f));
}

Vector3 ScatteringModelDefault::getRayleighCrossSectionCoef(const TaskData & _datas, const Vector3 & _lambdas) const
{
	const Float lRayDensityAtGround = 1.0E-18f * _datas.rayDensityAtGround;
	const Float lCoef = 24*o3d::PI*o3d::PI*o3d::PI/(lRayDensityAtGround*lRayDensityAtGround)*powf((_datas.rayAirIndex*_datas.rayAirIndex - 1)/(_datas.rayAirIndex*_datas.rayAirIndex+2), 2) * _datas.rayKingFactor;

	return Vector3(lCoef/powf(_lambdas[R]*1.0E9f, 4), lCoef/powf(_lambdas[G]*1.0E9f, 4), lCoef/powf(_lambdas[B]*1.0E9f, 4));
}

Vector3 ScatteringModelDefault::getMieCrossSectionCoef(const TaskData & _datas, const Vector3 & _lambdas) const
{
	return Vector3(_datas.mieCrossSection, _datas.mieCrossSection, _datas.mieCrossSection);
}

Vector3 ScatteringModelDefault::getSphereIntersection(const Vector3 & _pos, const Vector3 & _direction, Float _radius) const
{
	const Float lSquareAltitude = _pos.squareLength();
    const Float lAltitude = Math::sqrt(lSquareAltitude);
	Vector3 lNormalizedPosition(_pos);
	lNormalizedPosition.normalize();

	const Float lCosAlpha = lNormalizedPosition * _direction;
    const Float lT = Math::sqrt(_radius * _radius - lSquareAltitude*(1.0f - lCosAlpha*lCosAlpha)) - lAltitude * lCosAlpha;

	return (_pos + _direction * lT);
}

void ScatteringModelDefault::getOpticalLength(	const TaskData & _datas,
							const Vector3 & _pos,
							const Vector3 & _direction,
							Float & _rayOpticalLength,
							Float & _mieOpticalLength) const
{
	const Vector3 lSkyVertex(getSphereIntersection(_pos, _direction, 1000.0f*(_datas.planetRadius+_datas.atmThickness)));

	const Float lDistance = (lSkyVertex - _pos).length();

	const Float lEarthRadius = 1000.0f*_datas.planetRadius;
	const Float lBaseAltitude = _pos.length();
	const Float lBaseAltitudeSquare = lBaseAltitude*lBaseAltitude;
	const Float lInitStepSize = o3d::min<Float>(_datas.rayDensityDecFactor, _datas.mieDensityDecFactor) / _datas.stepIndex;

	const Float lIRayDecFactor = 1.0f / _datas.rayDensityDecFactor;
	const Float lIMieDecFactor = 1.0f / _datas.mieDensityDecFactor;
	const Float lTwoDot = 2 * (_pos * _direction);

	Float lRayResult = 0.0;
	Float lMieResult = 0.0;

	Float lLastRayValue = expf(-(lBaseAltitude - lEarthRadius)*lIRayDecFactor);
	Float lLastMieValue = expf(-(lBaseAltitude - lEarthRadius)*lIMieDecFactor);

	Float lS = 0.0;
	Float lStepSize = lInitStepSize;

	Bool quit = False;

	while ((lS < lDistance) && !quit)
	{
		lS += lStepSize;

		if (lS > lDistance)
		{
			lStepSize -= (lS - lDistance);
			lS -= (lS - lDistance);

			quit = True;
		}

        const Float lAltitude = Math::sqrt(lBaseAltitudeSquare + lTwoDot * lS + lS*lS) - lEarthRadius;
		Float lRayValue = expf(-lAltitude * lIRayDecFactor);
		Float lMieValue = expf(-lAltitude * lIMieDecFactor);

		lRayResult += (lLastRayValue + lRayValue)*lStepSize;
		lMieResult += (lLastMieValue + lMieValue)*lStepSize;

		lLastRayValue = lRayValue;
		lLastMieValue = lMieValue;

		lStepSize *= _datas.stepFactor;
	}

	_rayOpticalLength = (lRayResult * 0.5f * _datas.rayDensityAtGround);
	_mieOpticalLength = (lMieResult * 0.5f * _datas.mieDensityAtGround);
}

//! @brief Internal function.
void ScatteringModelDefault::convertToRGB(const TaskData & _datas, Float * _pColor, Float _maxLuminance)
{
	_maxLuminance = o3d::max<Float>(1E-3f, _maxLuminance);

	const Float lLum = getLuminance(_pColor);

	if (lLum > 0.0f)
	{
		const Float l3RgbBright = _pColor[R] + _pColor[G] + _pColor[B];
		const Float lCoef = 3.0f * getPhysicalBrightness(_datas, lLum, _maxLuminance)/l3RgbBright;

		_pColor[R] *= lCoef;
		_pColor[G] *= lCoef;
		_pColor[B] *= lCoef;

		const Float lMed = 1.0f/3.0f * (_pColor[R] + _pColor[G] + _pColor[B]);
		const Float lAlpha = 1.0f - expf(-10.0f * lLum);
		
		_pColor[R] = _pColor[R] * lAlpha + (1.0f - lAlpha) * lMed;
		_pColor[G] = _pColor[G] * lAlpha + (1.0f - lAlpha) * lMed;
		_pColor[B] = _pColor[B] * lAlpha + (1.0f - lAlpha) * lMed;
	}
	else
	{
		_pColor[R] = 0.0f;
		_pColor[G] = 0.0f;
		_pColor[B] = 0.0f;
	}
}

//! @brief Internal function.
void ScatteringModelDefault::convertRGBToLuminance(const TaskData & _datas, Float * _pColor)
{
	const Float lRgbBright = 1.0f/3.0f*(_pColor[R] + _pColor[G] + _pColor[B]);
	const Float lCoef = -_datas.postCoefB / (2.0f * logf((_datas.postCoefA - lRgbBright)/_datas.postCoefC));
	const Float lICoef = 1.0f/lCoef;

	_pColor[R] *= lICoef;
	_pColor[G] *= lICoef;
	_pColor[B] *= lICoef;
}

