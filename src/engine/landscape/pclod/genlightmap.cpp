/**
 * @file genlightmap.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/landscape/pclod/genlightmap.h"

#include "o3d/engine/landscape/pclod/lightmap.h"
#include "o3d/engine/landscape/pclod/lightcontainer.h"
#include "o3d/engine/landscape/pclod/pclodtexturemanager.h"
#include "o3d/engine/landscape/pclod/zonemanager.h"
#include "o3d/engine/landscape/pclod/object.h"
#include "o3d/core/callback.h"
#include "o3d/core/vector3.h"
#include "o3d/core/memorydbg.h"

using namespace o3d;

/* Build a lightmap from an 2d array of points. The light source is supposed to be at infinity.
 * You maybe add a border to your heightmap so that zones will have a continus lightmap.
 * Arguments:
 * - the 2d array of points
 * - the direction of the light
 * The function returns a 2d array of float which is the result of the product between the
 * direction of the light and the average normal at each points */
void GenLightMap::buildLinearNormalMapInfiniteLight(
	const Float * _pSource,
	const UInt32 width,
	const UInt32 height,
	Float _units,
	Float * & _pTarget,
	Callback * _pCallback)
{
	Array2DFloat _source(_pSource, width, height);

	// We can compute all points easily, but the border needs specials cares
	if (_pTarget == NULL)
		_pTarget = new Float[3*width*height];

	Float * pNormal = NULL;

	UInt32 widthLength = 3 * width;
	UInt32 maxWidthLength = 3 * (width - 1);

	// On place le pointeur sur la première ligne
	pNormal = _pTarget + widthLength;

	for (UInt32 y = 1 ; y < height-1 ; ++y)
	{
		if (_pCallback != NULL)
		{
			Int32 progress = (100*(y-1))/height;
			if (_pCallback->call((void*)&progress) != 0)
			{
				deleteArray(_pTarget);
				deletePtr(_pCallback);
				return;
			}
		}

		// La première normale n'est pas calculée ici
		pNormal += 3;

		for (UInt32 x = 1 ; x < width-1 ; ++x)
		{
			pNormal[X] = _source(x-1, y+1) - _source(x+1,y-1) + 2*_source(x-1, y)-2*_source(x+1, y);
			pNormal[Y] = _source(x+1, y-1) - _source(x-1,y+1) + 2*_source(x, y-1)-2*_source(x, y+1);
			pNormal[Z] = 6*_units;

			pNormal += 3;
		}

		pNormal += 3;
	}

	UInt32 maxWidth = width - 1;

	Float * pNormalLeft = _pTarget + widthLength;
	Float * pNormalRight = pNormalLeft + maxWidthLength;

	// Left border
	for (UInt32 y = 1 ; y < height-1 ; ++y)
	{
		pNormalLeft[X] = -2*_source(1, y) + 2*_source(0, y) - _source(1, y-1) + _source(0, y-1);
		pNormalLeft[Y] = _source(0, y-1) - _source(0, y+1) - _source(1,y ) + _source(1, y-1);
		pNormalLeft[Z] = 3 * _units;

		pNormalLeft += widthLength;

		// Right border
		pNormalRight[X] = 2*_source(maxWidth-1, y) - 2*_source(maxWidth, y) + _source(maxWidth-1, y+1) - _source(maxWidth, y+1);
		pNormalRight[Y] = _source(maxWidth, y-1) - _source(maxWidth, y+1) + _source(maxWidth-1,y ) - _source(maxWidth-1, y+1);
		pNormalRight[Z] = 3 * _units;

		pNormalRight += widthLength;
	}

	UInt32 maxHeight = height - 1;

	Float * pNormalBot = _pTarget + 3;
	Float * pNormalTop = _pTarget + maxHeight * widthLength + 3;

	// TopBorder
	for (UInt32 x = 1 ; x < width-1 ; ++x)
	{
		pNormalBot[X] = _source(x-1, maxHeight) - _source(x+1, maxHeight) + _source(x, maxHeight-1) - _source(x+1, maxHeight-1);
		pNormalBot[Y] = 2*_source(x, maxHeight-1) - 2*_source(x, maxHeight) + _source(x+1, maxHeight-1) - _source(x+1, maxHeight);
		pNormalBot[Z] = 3.0f*_units;

		pNormalBot += 3;

		// Bottom border
		pNormalTop[X] = -_source(x+1, 0) + _source(x-1, 0) - _source(x, 1) + _source(x-1, 1);
		pNormalTop[Y] = 2*_source(x, 0) - 2*_source(x, 1) + _source(x-1, 0) - _source(x-1, 1);
		pNormalTop[Z] = 3.0f*_units;

		pNormalTop += 3;
	}

	// The four corners
	// Bottom left
	pNormal = _pTarget;

	pNormal[X] = _source(0, 0) - _source(1, 0);
	pNormal[Y] = _source(0, 0) - _source(0, 1);
	pNormal[Z] = _units;

	// Bottom right
	pNormal = _pTarget + maxWidthLength ;

	pNormal[X] = _source(maxWidth-1, 0) - _source(maxWidth, 0) + _source(maxWidth-1, 1) - _source(maxWidth, 1);
	pNormal[Y] = _source(maxWidth-1, 0) - _source(maxWidth-1, 1) + _source(maxWidth, 0) - _source(maxWidth, 1);
	pNormal[Z] = 2*_units;

	// Top left
	pNormal = _pTarget + maxHeight * widthLength;

	pNormal[X] = _source(0, maxHeight-1) - _source(1, maxHeight-1) + _source(0, maxHeight) - _source(1, maxHeight);
	pNormal[Y] = _source(0, maxHeight-1) - _source(0, maxHeight) + _source(1, maxHeight-1) - _source(1, maxHeight);
	pNormal[Z] = 2*_units;

	// Top right
	pNormal = _pTarget + height * widthLength - 3;

	pNormal[X] = _source(maxWidth-1, maxHeight) - _source(maxWidth, maxHeight);
	pNormal[Y] = _source(maxWidth, maxHeight-1) - _source(maxWidth, maxHeight);
	pNormal[Z] = _units;

	if (_pCallback != NULL)
	{
		Int32 progress = 100;
		_pCallback->call((void*)&progress);
		deletePtr(_pCallback);
	}
}

/* This function is just provided for convenience.
 * - It allows to specify borders to compute the lightmaps.
 * - The borders is an array of size 2*(W+H)+4 */
void GenLightMap::buildLinearNormalMapInfiniteLightBorders(const Float * _pSource,
											   const UInt32 _width,
											   const UInt32 _height,
											   Float _units,
											   Float * & _pTarget,
											   const Float * _pBorder,
											   Callback * _callback)
{
	// Pour le moment, on récréer juste une heightmap et on utilise la fonction précédente.
	// Très mauvais en performance.

	Array2DFloat source(_pSource, _width, _height);

	Array2DFloat buffer(_width + 2, _height + 2);
	buffer.insert(source, 1, 1);

	const UInt32 bufferWidth = buffer.width();
	const UInt32 bufferHeight = buffer.height();
	const UInt32 maxBufferWidth = bufferWidth - 1;
	const UInt32 maxBufferHeight = bufferHeight - 1;

	UInt32 rIndex = 0;
	for (UInt32 x = 0 ; x < bufferWidth ; ++x, ++rIndex)
		buffer(x, maxBufferHeight) = _pBorder[rIndex];

	for (Int32 y = maxBufferHeight ; y >= 0; --y, ++rIndex)
		buffer(maxBufferWidth, y) = _pBorder[rIndex];

	for (Int32 x = maxBufferWidth ; x >= 0 ; --x, ++rIndex)
		buffer(x, 0) = _pBorder[rIndex];

	for (UInt32 y = 0 ; y < bufferHeight ; ++y, ++rIndex)
		buffer(0, y) = _pBorder[rIndex];

	Float * normalMap = NULL;

	GenLightMap::buildLinearNormalMapInfiniteLight(
		&buffer[0], bufferWidth, bufferHeight, _units, normalMap, _callback);

	rIndex = 0;

	if (_pTarget == NULL)
		_pTarget = new Float[3*_width * _height];

	for (UInt32 y = 1 ; y < maxBufferHeight - 1; ++y, rIndex += maxBufferWidth - 2)
	{
		memcpy((void*)&_pTarget[rIndex], (const void*)&buffer(1, y), 3*(maxBufferWidth - 2)*sizeof(Float));
	}

	deleteArray(normalMap);
}

void GenLightMap::normalizeNormalMap(Float * _buffer, const UInt32 _width, const UInt32 _height)
{
	Vector3 * ptr = (Vector3*)_buffer;
	UInt32 k = _width * _height;

	while (k > 0)
	{
		ptr->normalize();

		++ptr;
		--k;
	}
}

/* The function return the lightmap associated to a normal map of size W * H.
 * The lightmap this function returns is (W-1)*(H-1). */
void GenLightMap::buildLinearLightmap(const Vector3 & _lightDir, const Float * _pSource, const UInt32 _sWidth, const UInt32 _sHeight, Float * & _pTarget, UInt32 & _tWidth, UInt32 & _tHeight)
{
	// On suppose que la normal map est pas normalisé. Je sais pas si c'est nécessaire.
	// Il me semble pas.
	// Il y a des optimisations a faire au niveau du calcul de base.

	const Vector3 * _pNormalMap = (const Vector3 *)_pSource;

	_tWidth = _sWidth - 1;
	_tHeight = _sHeight - 1;

	if (_pTarget == NULL)
		_pTarget = new Float[_tWidth * _tHeight];

	Vector3 normal;

	for (UInt32 y = 0 ; y < _tHeight ; ++y)
		for (UInt32 x = 0 ; x < _tWidth ; ++x)
		{
			normal =	_pNormalMap[x + y * _sWidth] +
						_pNormalMap[x+1 + y * _sWidth] +
						_pNormalMap[x + (y+1) * _sWidth] +
						_pNormalMap[x+1 + (y+1) * _sWidth];

			normal.normalize();

			_pTarget[x + y * _tWidth] = o3d::max<Float>(_lightDir * normal, 0.0f);
		}
}

void GenLightMap::buildLinearShadowmapInfiniteLightX(const Array2DFloat & _source, Float _units, Float _orientation, Array2DFloat & _target)
{
	UInt32 width = _source.width();
	UInt32 height = _source.height();

	UInt32 maxWidth = width - 1;
//	UInt32 maxHeight = height - 1;

	_target.setSize(width, height);

	Float currentMax = 0.0f;

	Float lightSlope = _orientation / _units;

#ifdef _DEBUG
	for (UInt32 y = 0 ; y < height ; ++y)
	{
		currentMax = _source(maxWidth, y);
		_target(maxWidth, y) = 1.0f;

		for (UInt32 x = maxWidth-1 ; x < maxWidth; --x)
		{
			currentMax -= lightSlope;

			if (_source(x,y) >= currentMax)
			{
				currentMax = _source(x,y);

				_target(x,y) = 1.0f;
			}
			else
				_target(x,y) = 0.0f;
		}
	}
#else

	Float * pSourceRow = &_source[0];
	Float * pSourceRowEnd = &_source[width*height];
	Float * pSourcePoint = NULL;
	Float * pSourcePointEnd = NULL;

	Float * pTargetRow = &_target[maxWidth];
	Float * pTargetPoint = NULL;

	while (pSourceRow != pSourceRowEnd)
	{
		pSourcePoint = pSourceRow + maxWidth;
		pSourcePointEnd = pSourceRow - 1;

		pTargetPoint = pTargetRow;
		*pTargetPoint = 1.0f;
		--pTargetPoint;

		currentMax = *pSourcePoint;
		--pSourcePoint;

		while (pSourcePoint != pSourcePointEnd)
		{
			currentMax -= lightSlope;

			if (*pSourcePoint >= currentMax)
			{
				currentMax = *pSourcePoint;

				*pTargetPoint = 1.0f;
			}
			else
				*pTargetPoint = 0.0f;

			--pSourcePoint;
			--pTargetPoint;
		}

		pSourceRow += width;
		pTargetRow += width;
	}
#endif
}

void GenLightMap::buildShadowmapInfiniteLight(const Array2DFloat & _source, Float _units, const Vector3 & _dir, Array2DFloat & _target, Callback * _pCallback)
{
	Vector3 lightDir(_dir);
	lightDir.normalize();

	Float VlightSlope = lightDir[Z]/sqrtf(lightDir[X]*lightDir[X] + lightDir[Y]*lightDir[Y]) * _units;
	Float HlightSlope = lightDir[Y]/lightDir[X] * _units;

	UInt32 width = _source.width();
	UInt32 height = _source.height();

	UInt32 maxWidth = _source.width() - 1;
	UInt32 maxHeight = _source.height() - 1;

	Int32 * pOffset = new Int32[width];

	Float Hdeltaf = HlightSlope * Float(maxWidth);
	Int32 Hdeltai = Int32(floorf(Hdeltaf + 0.5f));

	Int32 segmentLength = width / (Hdeltai+1);
	Int32 segmentBorder = width % (Hdeltai+1);
	UInt32 leftSegmentBorder = segmentBorder/2 + (segmentBorder & 0x00000001); //(((segmentBorder & 0x00000001) != 0)?1:0);
	UInt32 rightSegmentBorder = segmentBorder/2;

	// Construction du décalage de ligne
	Int32 * pOffsetPos = &pOffset[0];

	for (UInt32 k = leftSegmentBorder ; k > 0 ; --k)
	{
		*pOffsetPos = 0;
		++pOffsetPos;
	}

	UInt32 kMax = width - rightSegmentBorder - leftSegmentBorder;

	for (UInt32 k = 0 ; k < kMax ; ++k)
	{
		*pOffsetPos = k/segmentLength;
		++pOffsetPos;
	}

	for (UInt32 k = rightSegmentBorder ; k >0 ; --k)
	{
		*pOffsetPos = Hdeltai;
		++pOffsetPos;
	}

	Float currentMax = 0.0f;
	Int32 xMax = 0;
	Int32 currentRow = 0;
	Int32 offset = 0;

	_target.setSize(width, height);

	const Float * currentSourcePoint = NULL;

	Float * currentTargetPoint = NULL;

	for (Int32 y = -Hdeltai ; y < Int32(height); ++y)
	{
		if (_pCallback != NULL)
		{
			UInt8 progress = (UInt8)( 100*(y+Hdeltai)/(height + Hdeltai - 1) );

			if (_pCallback->call((void*)&progress) != 0)
			{
				_target.free();
				deleteArray(pOffset);
				deletePtr(_pCallback);
				return;
			}
		}

		xMax = width;

		while ((currentRow = y + pOffset[--xMax]) > Int32(maxHeight)) {}

		currentSourcePoint = &_source(xMax, currentRow);
		currentTargetPoint = &_target(xMax, currentRow);

		//currentMax = _source(xMax, y + pOffset[xMax]);
		currentMax = *currentSourcePoint;

		if ((xMax > 1) && ((offset = pOffset[xMax]-pOffset[xMax-1]) != 0))
		{
			currentSourcePoint -= offset*width;
			currentTargetPoint -= offset*width;
		}

		--currentSourcePoint;
		--currentTargetPoint;

		for (UInt32 x = xMax-1 ; x < width ; --x)
		{
			currentMax -= VlightSlope;

			currentRow = y + pOffset[x];

			//if (y + pOffset[x] < 0)
			if (currentRow < 0)
				break;

//			if (_source(x, y + pOffset[x]) >= currentMax)
			if (*currentSourcePoint >= currentMax)
			{
				currentMax = *currentSourcePoint;

				*currentTargetPoint = 1.0f;
			}
			else
			{
				*currentTargetPoint = 0.0f;
			}

			if ((x > 1) && ((offset = pOffset[x]-pOffset[x-1]) != 0))
			{
				Int32 dec = offset*width;

				currentSourcePoint -= dec;
				currentTargetPoint -= dec;
			}

			--currentSourcePoint;
			--currentTargetPoint;
		}
	}

	deletePtr(_pCallback);
	deleteArray(pOffset);
}

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define SOUTH_WEST 0
#define NORTH_WEST 1
#define NORTH_EAST 2
#define SOUTH_EAST 3

//static Vector2i getCorner(PCLODTopZone* _pZone, Int32 _corner)
//{
//	const Vector2i & lOrigin = _pZone->getZoneOrigin();
//	const Vector2ui lZoneSize(_pZone->getZoneSize());

//	switch(_corner)
//	{
//	case SOUTH_WEST:
//		return lOrigin;
//		break;
//	case NORTH_WEST:
//		return Vector2i(lOrigin[X], lOrigin[Y] + lZoneSize[Y]);
//		break;
//	case NORTH_EAST:
//		return Vector2i(lOrigin[X] + lZoneSize[X], lOrigin[Y] + lZoneSize[Y]);
//		break;
//	case SOUTH_EAST:
//		return Vector2i(lOrigin[X] + lZoneSize[X], lOrigin[Y]);
//		break;
//	default:
//		O3D_ASSERT(0);
//		break;
//	}

//	return Vector2i(0,0);
//}

static void getZoneIntersection(Int32 _dir, PCLODTopZone * _pZone, PCLODTopZone * _pNeighbor, Vector2i & _p1, Vector2i & _p2)
{
    O3D_ASSERT(_pZone != nullptr);
    O3D_ASSERT(_pNeighbor != nullptr);

	Vector2i lRangeZone, lRangeNeighbor;

	switch (_dir)
	{
	case NORTH:
		lRangeZone.set(_pZone->getZoneOrigin()[X], _pZone->getZoneOrigin()[X] + _pZone->getZoneSize()[X]-1);
		lRangeNeighbor.set(_pNeighbor->getZoneOrigin()[X], _pNeighbor->getZoneOrigin()[X] + _pNeighbor->getZoneSize()[X]-1);

		_p2[Y] = _p1[Y] = _pZone->getZoneOrigin()[Y] + _pZone->getZoneSize()[Y]-1;
		_p1[X] = o3d::max<Int32>(lRangeZone[0], lRangeNeighbor[0]);
		_p2[X] = o3d::min<Int32>(lRangeZone[1], lRangeNeighbor[1]);
		break;
	case EAST:
		lRangeZone.set(_pZone->getZoneOrigin()[Y], _pZone->getZoneOrigin()[Y] + _pZone->getZoneSize()[Y]-1);
		lRangeNeighbor.set(_pNeighbor->getZoneOrigin()[Y], _pNeighbor->getZoneOrigin()[Y] + _pNeighbor->getZoneSize()[Y]-1);

		_p2[X] = _p1[X] = _pZone->getZoneOrigin()[X] + _pZone->getZoneSize()[X]-1;
		_p1[Y] = o3d::max<Int32>(lRangeZone[0], lRangeNeighbor[0]);
		_p2[Y] = o3d::min<Int32>(lRangeZone[1], lRangeNeighbor[1]);
		break;
	case SOUTH:
		lRangeZone.set(_pZone->getZoneOrigin()[X], _pZone->getZoneOrigin()[X] + _pZone->getZoneSize()[X]-1);
		lRangeNeighbor.set(_pNeighbor->getZoneOrigin()[X], _pNeighbor->getZoneOrigin()[X] + _pNeighbor->getZoneSize()[X]-1);

		_p2[Y] = _p1[Y] = _pZone->getZoneOrigin()[Y];
		_p1[X] = o3d::max<Int32>(lRangeZone[0], lRangeNeighbor[0]);
		_p2[X] = o3d::min<Int32>(lRangeZone[1], lRangeNeighbor[1]);
		break;
	case WEST:
		lRangeZone.set(_pZone->getZoneOrigin()[Y], _pZone->getZoneOrigin()[Y] + _pZone->getZoneSize()[Y]-1);
		lRangeNeighbor.set(_pNeighbor->getZoneOrigin()[Y], _pNeighbor->getZoneOrigin()[Y] + _pNeighbor->getZoneSize()[Y]-1);

		_p2[X] = _p1[X] = _pZone->getZoneOrigin()[X];
		_p1[Y] = o3d::max<Int32>(lRangeZone[0], lRangeNeighbor[0]);
		_p2[Y] = o3d::min<Int32>(lRangeZone[1], lRangeNeighbor[1]);
		break;
	default:
		O3D_ASSERT(0);
		break;
	}
}

static void fillEdge(Int32 _dir, PCLODLightInfo & lInfos, Float _value)
{	
	switch (_dir)
	{
	case NORTH:
		for (UInt32 k = 0 ; k < lInfos.shadowMap.width() ; ++k)
			lInfos.shadowMap(k, lInfos.shadowMap.height()-1) = _value;
		break;
	case EAST:
		for (UInt32 k = 0 ; k < lInfos.shadowMap.height() ; ++k)
			lInfos.shadowMap(lInfos.shadowMap.width()-1, k) = _value;
		break;
	case SOUTH:
		for (UInt32 k = 0 ; k < lInfos.shadowMap.width() ; ++k)
			lInfos.shadowMap(k, 0) = _value;
		break;
	case WEST:
		for (UInt32 k = 0 ; k < lInfos.shadowMap.height() ; ++k)
			lInfos.shadowMap(0, k) = _value;
		break;
	default:
		O3D_ASSERT(0);
		break;
	}
}

//! Extrait le bord d'un buffer d'une zone
//! Si les points demandés sont hors de la zone, la fonction ne fera que son possible.
//! @param _pZone la zone en question
//! @param _pSourceBuffer le buffer duquel on souhaite récupérer les valeurs
//! @param _start le point du départ sur le bord de la zone (repère absolu)
//! @param _end le point final du bord (repère absolu)
//! @param _pBuffer le buffer dans lequel stocker les résultats
//! @param _stride l'écart en élément entre chaque point à insérer dans le buffer.
/*static void getZoneEdge(O3DPCLODTopZone * _pZone, Float * _pSourceBuffer, const O3DVector2i & _start, const O3DVector2i & _end, Float * _pBuffer, Int32 _stride)
{
	const O3DVector2i lSize(Int32(_pZone->getZoneSize()[X]), Int32(_pZone->getZoneSize()[X]));
	const O3DVector2i & lOrigin = _pZone->getZoneOrigin();
	O3DVector2i lRelativeStart(_start - lOrigin);
	O3DVector2i lRelativeEnd(_end - lOrigin);

	lRelativeStart[X] = o3d::min<Int32>(lRelativeStart[X], lSize[X] - 1);
	lRelativeStart[Y] = o3d::min<Int32>(lRelativeStart[Y], lSize[Y] - 1);

	O3D_ASSERT((lRelativeStart[X] >= 0) && (lRelativeStart[X] < lSize[X]));
	O3D_ASSERT((lRelativeStart[Y] >= 0) && (lRelativeStart[Y] < lSize[Y]));

	const Float * lPtrSource = NULL;
	Float * lPtrTarget = _pBuffer;

	if (lRelativeStart[X] == lRelativeEnd[X])
	{
		const Int32 lFirst = o3d::min<Int32>(lRelativeStart[X], lRelativeEnd[X]);
		const Int32 lLast = o3d::max<Int32>(lRelativeStart[X], lRelativeEnd[X]);

		const Int32 lSourceStride = 1;
		lPtrSource = _pSourceBuffer + lSourceStride * lFirst;

		for (Int32 k = lFirst ; k <= lLast ; ++k, lPtrTarget += _stride, lPtrSource += lSourceStride)
			(*lPtrTarget) = (*_pSourceBuffer);

	}
	else if (_start[Y] == _end[Y])
	{
		const Int32 lFirst = o3d::min<Int32>(lRelativeStart[Y], lRelativeEnd[Y]);
		const Int32 lLast = o3d::max<Int32>(lRelativeStart[Y], lRelativeEnd[Y]);

		const Int32 lSourceStride = lSize[X];
		lPtrSource = _pSourceBuffer + lSourceStride * lFirst;

		for (Int32 k = lFirst ; k <= lLast ; ++k, lPtrTarget += _stride, lPtrSource += lSourceStride)
			(*lPtrTarget) = (*_pSourceBuffer);
	}
	else
		O3D_ASSERT(0);
}*/

/*static void buildShadowBorder(O3DPCLODLight * _pLight,
					   O3DPCLODLightmap * _pLightmap,
					   O3DPCLODLightInfo & _lightInfos,
					   const std::vector<O3DPCLODLightmap*> & _array,
					   Int32 _direction)
{
	const O3DVector2ui lSize = _pLightmap->getZone()->getZoneSize();
	O3DVector2i lStartPoint;
	O3DVector2i lEndPoint;

	switch (_direction)
	{
	case NORTH:
		lStartPoint = _pLightmap->getZone()->getZoneOrigin();
		lStartPoint[Y] += lSize[Y]-1;
		lEndPoint.set(lStartPoint[X] + lSize[X]-1, lStartPoint[Y]);
		break;
	case SOUTH:
		lStartPoint = _pLightmap->getZone()->getZoneOrigin();
		lEndPoint.set(lStartPoint[X] + lSize[X]-1, lStartPoint[Y]);
		break;
	case EAST:
		lStartPoint = _pLightmap->getZone()->getZoneOrigin();
		lStartPoint[X] += lSize[X]-1;
		lEndPoint.set(lStartPoint[X], lStartPoint[Y] + lSize[Y]-1);
		break;
	case WEST:
		lStartPoint = _pLightmap->getZone()->getZoneOrigin();
		lEndPoint.set(lStartPoint[X], lStartPoint[Y] + lSize[Y]-1);
		break;
	};

	for (std::vector<O3DPCLODLightmap*>::const_iterator it = _array.begin() ; it != _array.end() ; it++)
	{
		O3DPCLODTopZone * lpTopZone = (*it)->getZone();

		GetZoneEdge(lpTopZone, (*it)->getLightInfo(_pLight).shadowMap, lStartPoint, lEndPoint, _lightInfos.shadowMap, (_direction == NORTH) || (_direction == SOUTH) ? 1 : lpTopZone->getZoneSize()[X]);
	}
}*/

static void computeShadowMap(PCLODLight * _pLight, const Vector3 & _dirVec, Int32 * _dirLight, Int32 * _neighborFind, PCLODLightmap * _pLightmap)
{
	PCLODLightInfo & lInfos = _pLightmap->getLightInfo(_pLight);
	PCLODTopZone * lpTopZone = _pLightmap->getZone();

	if (lInfos.shadowUpToDate)
		return;

	Vector2ui lZoneHeightmapSize;

	SmartArrayFloat lZoneHeightmap;
	lpTopZone->getHeightmap(lZoneHeightmap, lZoneHeightmapSize);

	const Vector2i lMapSize(	(lZoneHeightmapSize[X] >> lInfos.lightmapLevel) + (lInfos.lightmapLevel > 0 ? 1 : 0),
								(lZoneHeightmapSize[Y] >> lInfos.lightmapLevel) + (lInfos.lightmapLevel > 0 ? 1 : 0));
	const Vector2i lZoneSize(lZoneHeightmapSize[X], lZoneHeightmapSize[Y]);
	const Int32 lStep = 1 << lInfos.lightmapLevel;

	if (lInfos.shadowMap.isEmpty())
		lInfos.shadowMap.setSize(lMapSize[X], lMapSize[Y]);

	// Si la lumière est à la verticale
	if (o3d::max<Float>(o3d::abs<Float>(_dirVec[X]), o3d::abs<Float>(_dirVec[Z])) < 0.001f)
	{
		for (Int32 k = 0 ; k < lMapSize[X] * lMapSize[Y] ; ++k)
			lInfos.shadowMap[k] = 0.0f;

		return;
	}

	// Si la lumière est "sous la terre"
	if (_dirVec[Y] > 0.0f)
	{
		for (Int32 k = 0 ; k < lMapSize[X] * lMapSize[Y] ; ++k)
			lInfos.shadowMap[k] = 1000.0f; // Epaisseur de l'ombre

		return;
	}

	fillEdge(_dirLight[0], lInfos, -1.0f);
	fillEdge(_dirLight[1], lInfos, -1.0f);

	if ((_pLightmap->getNeighbor(_dirLight[0], 0) != NULL) || (_pLightmap->getNeighbor(_dirLight[1], 0) != NULL))
	{
		Int32 k = 0;
		PCLODLightmap * lPtr = NULL;

		Vector2i lP1, lP2;

		while ((lPtr = _pLightmap->getNeighbor(_dirLight[0], k)) != NULL)
		{
			if (!lPtr->getLightInfo(_pLight).shadowUpToDate)
				computeShadowMap(_pLight, _dirVec, _dirLight, _neighborFind, lPtr);

			getZoneIntersection(_dirLight[0], _pLightmap->getZone(), lPtr->getZone(), lP1, lP2);

			const Int32 lLocalX = lP1[X] - _pLightmap->getZone()->getZoneOrigin()[X];
			const Int32 lDistX = lP1[X] - lPtr->getZone()->getZoneOrigin()[X];

			const Int32 lLocalYOrigin = _pLightmap->getZone()->getZoneOrigin()[Y];
			const Int32 lDistYOrigin = lPtr->getZone()->getZoneOrigin()[Y];

			const PCLODLightInfo & lDistInfos = lPtr->getLightInfo(_pLight);
			const Vector2i lDistMapSize(lDistInfos.shadowMap.width(), lDistInfos.shadowMap.height());

			for (Int32 p = lP1[Y] ; p < lP2[Y]+1 ; ++p)
			{
				lInfos.shadowMap(lLocalX, p - lLocalYOrigin) = 
					lDistInfos.shadowMap(lDistX, p - lDistYOrigin);
			}

			++k;
		}

		k = 0;
		while ((lPtr = _pLightmap->getNeighbor(_dirLight[1], k)) != NULL)
		{
			if (!lPtr->getLightInfo(_pLight).shadowUpToDate)
				computeShadowMap(_pLight, _dirVec, _dirLight, _neighborFind, lPtr);

			getZoneIntersection(_dirLight[1], _pLightmap->getZone(), lPtr->getZone(), lP1, lP2);

			const Int32 lLocalY = lP1[Y] - _pLightmap->getZone()->getZoneOrigin()[Y];
			const Int32 lDistY = lP1[Y] - lPtr->getZone()->getZoneOrigin()[Y];

			const Int32 lLocalXOrigin = _pLightmap->getZone()->getZoneOrigin()[X];
			const Int32 lDistXOrigin = lPtr->getZone()->getZoneOrigin()[X];

			const PCLODLightInfo & lDistInfos = lPtr->getLightInfo(_pLight);
			const Vector2i lDistMapSize(lDistInfos.shadowMap.width(), lDistInfos.shadowMap.height());

			for (Int32 p = lP1[X] ; p < lP2[X]+1 ; ++p)
			{
				lInfos.shadowMap(p - lLocalXOrigin, lLocalY) = 
					lDistInfos.shadowMap(p - lDistXOrigin, lDistY);
			}

			++k;
		}
	}

	{
		Int32 lOffsetMap = ((_dirLight[0] == WEST) ? 0 : lMapSize[X] - 1);
		Int32 lOffsetHeightmap = ((_dirLight[0] == WEST) ? 0 : lZoneSize[X] - 1);

		for (Int32 k = 0 ; k < lMapSize[Y] ; ++k)
			if (lInfos.shadowMap[lOffsetMap + k*lMapSize[X]] == -1.0f)
				lInfos.shadowMap[lOffsetMap + k*lMapSize[X]] = lZoneHeightmap[lOffsetHeightmap + k*lStep*lZoneSize[X]];

		lOffsetMap = ((_dirLight[1] == SOUTH) ? 0 : lMapSize[Y] - 1) * lMapSize[X];
		lOffsetHeightmap = ((_dirLight[1] == SOUTH) ? 0 : lZoneSize[Y] - 1) * lZoneSize[X];

		for (Int32 k = 0 ; k < lMapSize[X] ; ++k)
			if (lInfos.shadowMap[lOffsetMap + k] == -1.0f)
				lInfos.shadowMap[lOffsetMap + k] = lZoneHeightmap[lOffsetHeightmap + k * lStep];
	}

	// Incrément vertical lorsqu'on se déplace dans le sens de la lumière
	Vector2f lProjDir(o3d::abs<Float>(_dirVec[Z]), o3d::abs<Float>(_dirVec[X]));
	Vector2f lUnits;
	_pLight->getTextureManager()->getZoneManager()->getZoneUnits(lUnits);
	Vector3 lLightDir(_dirVec);
	lLightDir /= lProjDir.normInf();

	// On construit le tableau contenant le trajet des algos
	Int32 * lPathX = new Int32[lMapSize[X]];
	Int32 * lPathY = new Int32[lMapSize[X]];
	const Float lPathZ = lLightDir[Y];

	if (lProjDir[X] > lProjDir[Y])
	{
		for (Int32 k = 0 ; k < lMapSize[X] ; ++k)
		{
			lPathX[k] = Int32(floorf(k*lStep*lLightDir[Z] + 0.5f));
			lPathY[k] = Int32(floorf(k*lStep*lLightDir[X] + 0.5f));
		}

		const Int32 lStartX = (_dirLight[0] == EAST) ? lMapSize[X]-1 : 0;
		const Bool lHoriz = (lPathY[lMapSize[X]-1] == 0);

		// 
		for (Int32 k = 1 ; k < lMapSize[X] ; ++k)
		{
			const Int32 lDeltaY = lPathY[k-1] - lPathY[k];

			const Int32 lStartY = ((_dirLight[1] == NORTH) || lHoriz) ? 0 : 1;
			const Int32 lEndY = ((_dirLight[1] == SOUTH) || lHoriz) ? lMapSize[X] : lMapSize[X]-1;

			const Int32 lX = lStartX + lPathX[k];
			const Int32 lPX = lStartX + lPathX[k-1];

			for (Int32 y = lStartY ; y < lEndY ; ++y)
			{
				const Int32 lPY = y + lDeltaY;

				Float & lCurrentShadowPoint = lInfos.shadowMap[y * lMapSize[X] + lX];
				Float & lCurrentHeightPoint = lZoneHeightmap[y * lStep * lZoneSize[X] + lX * lStep];

				lCurrentShadowPoint = lInfos.shadowMap[lPY * lMapSize[X] + lPX] + lPathZ;

				if (lCurrentShadowPoint < lCurrentHeightPoint)
					lCurrentShadowPoint = lCurrentHeightPoint; // Eclairé
			}
		}
	}
	else
	{
		for (Int32 k = 0 ; k < lMapSize[X] ; ++k)
		{
			lPathX[k] = Int32(floorf(k*lStep*lLightDir[Z] + 0.5f));
			lPathY[k] = Int32(floorf(k*lStep*lLightDir[X] + 0.5f));
		}

		const Int32 lStartY = (_dirLight[1] == SOUTH) ? 0 : lMapSize[X]-1;
		const Bool lVert = (lPathX[lMapSize[X]-1] == 0);

		// 
		for (Int32 k = 1 ; k < lMapSize[X] ; ++k)
		{
			const Int32 lDeltaX = lPathX[k-1] - lPathX[k];

			const Int32 lStartX = ((_dirLight[0] == EAST) || lVert) ? 0 : 1;
			const Int32 lEndX = ((_dirLight[0] == WEST) || lVert) ? lMapSize[X] : lMapSize[X]-1;

			const Int32 lY = lStartY + lPathY[k];
			const Int32 lPY = lStartY + lPathY[k-1];

			for (Int32 x = lStartX ; x < lEndX ; ++x)
			{
				const Int32 lPX = x + lDeltaX;

				Float & lCurrentShadowPoint = lInfos.shadowMap[lY * lMapSize[X] + x];
				Float & lCurrentHeightPoint = lZoneHeightmap[lY * lStep * lZoneSize[X] + x * lStep];

				lCurrentShadowPoint = lInfos.shadowMap[lPY * lMapSize[X] + lPX] + lPathZ;

				if (lCurrentShadowPoint < lCurrentHeightPoint)
					lCurrentShadowPoint = lCurrentHeightPoint; // Eclairé
			}
		}
	}

	for (Int32 k = 0 ; k < lMapSize[X] * lMapSize[X] ; ++k)
		lInfos.shadowMap[k] -= lZoneHeightmap[k*lStep];

	lInfos.shadowUpToDate = True;

	// Désallocation
	deleteArray(lPathX);
	deleteArray(lPathY);
}

void GenLightMap::buildShadowMapFromDirectionalLight(PCLODLight * _pLight, const Vector3 & _lightDirection, const std::vector<PCLODLightmap*> & _lightmaps)
{
	if (_lightmaps.empty())
		return;

	Int32 lLightDir[2] = { ((_lightDirection[Z] < 0.0f) ? EAST : WEST), ((_lightDirection[X] < 0.0f) ? NORTH : SOUTH) };
	Int32 lNeighborFind[2];

	if (lLightDir[0] == EAST)
	{
		if (lLightDir[1] == NORTH)
		{
			lNeighborFind[0] = O3D_MAX_INT32;	// Sur le coté gauche, on prend la zone d'en haut
			lNeighborFind[1] = 0;
		}
		else
		{
			lNeighborFind[0] = 0;
			lNeighborFind[1] = O3D_MAX_INT32;
		}
	}
	else if (lLightDir[1] == NORTH)
	{
		lNeighborFind[0] = 0;
		lNeighborFind[1] = O3D_MAX_INT32;
	}
	else if (lLightDir[1] == SOUTH)
	{
		lNeighborFind[0] = O3D_MAX_INT32;
		lNeighborFind[1] = 0;
	}

	PCLODLightmap * lBorderLightmap = _lightmaps.front();
	PCLODLightmap * lNeighborLightmap = NULL;

	while (((lNeighborLightmap = lBorderLightmap->getNeighbor(lLightDir[0], lNeighborFind[0])) != NULL) ||
		  ((lNeighborLightmap = lBorderLightmap->getNeighbor(lLightDir[1], lNeighborFind[1])) != NULL))
		  lBorderLightmap = lNeighborLightmap;

	for (std::vector<PCLODLightmap*>::const_iterator it = _lightmaps.begin() ; it != _lightmaps.end() ; it++)
	{
		PCLODLightInfo & lInfos = (*it)->getLightInfo(_pLight);
		
		lInfos.shadowUpToDate = False;
	}

	for (std::vector<PCLODLightmap*>::const_iterator it = _lightmaps.begin() ; it != _lightmaps.end() ; it++)
		computeShadowMap(_pLight, _lightDirection, lLightDir, lNeighborFind, *it);//lBorderLightmap);
}

