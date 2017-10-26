/**
 * @file lightmap.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/landscape/pclod/lightmap.h"

#include "o3d/engine/landscape/pclod/pclodtexturemanager.h"

#include "o3d/engine/landscape/pclod/object.h"
#include "o3d/engine/landscape/pclod/terrain.h"
#include "o3d/engine/landscape/pclod/configs.h"
#include "o3d/engine/landscape/pclod/zonemanager.h"

#include "o3d/engine/texture/texture2d.h"
#include "o3d/core/objects.h"


using namespace o3d;

/*================================================================
					class O3DPCLODLightmap
================================================================*/
PCLODLightmap::PCLODLightmap(BaseObject *pParent, PCLODTopZone * _pTopZone):
	BaseObject(pParent),
	m_pTopZone(_pTopZone),
	m_normalMap(),
	m_staticLightmapBuffer(),
	m_staticLightmapChanged(False),
	m_dynamicLightmapBuffer(),
	m_lightMap(),
	m_offsetLod(0),
    m_pTexture(nullptr),
	m_refCounter(0),
	m_counterMutex(),
	m_semaphore()
{
    O3D_ASSERT(m_pTopZone != nullptr);

	m_pTopZone->use(PCLODTopZone::ZONE_HEIGHTMAP);
	m_pTopZone->useIt();

	m_pTexture = new Texture2D(this);
	m_pTexture->setWrap(Texture::CLAMP);
	m_pTexture->setFiltering(Texture::LINEAR_FILTERING);

	buildNormalMap();
}

PCLODLightmap::~PCLODLightmap()
{
	// Le destructeur ne doit jamais etre appelé avant les fonctions Unload
	O3D_ASSERT(!generated());
    O3D_ASSERT(m_pTopZone == nullptr);
}

PCLODTerrain * PCLODLightmap::getTerrain()
{
	return getTextureManager()->getTerrain();
}

const PCLODTerrain * PCLODLightmap::getTerrain() const
{
	return getTextureManager()->getTerrain();
}

PCLODTextureManager * PCLODLightmap::getTextureManager()
{
	return (PCLODTextureManager*)getParent();
}

const PCLODTextureManager * PCLODLightmap::getTextureManager() const
{
	return (PCLODTextureManager*)getParent();
}

/* Functions */

UInt32 PCLODLightmap::getZoneId() const
{
    O3D_ASSERT(m_pTopZone != nullptr);

	return m_pTopZone->getId();
}

Bool PCLODLightmap::generated() const
{
//	O3DReadWriteLocker lLocker(m_semaphore, O3DReadWriteLocker::READ_LOCKER);

/*	for (CIT_LightMap it = m_lightMap.begin() ; it != m_lightMap.end() ; it++)
		if (it->first->needUpdate(it->second))
			return False;*/

    return ((m_pTexture != nullptr) && (m_pTexture->getTextureId() != 0));
}

/* Release the texture */
void PCLODLightmap::mtUnload()
{
    O3D_ASSERT(m_pTexture != nullptr);
	O3D_ASSERT(m_refCounter == 0);
    O3D_ASSERT(m_pTopZone != nullptr);

	UInt32 zoneId = getZoneId();
	UInt32 texSize = m_pTexture->getImage().getSize();

	deletePtr(m_pTexture);

	m_pTopZone->release(PCLODTopZone::ZONE_HEIGHTMAP);
	m_pTopZone->releaseIt();
    m_pTopZone = nullptr;

	clearNeighbors();

	m_staticLightmapBuffer.destroy();
	m_staticLightmapChanged = False;

	m_dynamicLightmapBuffer.destroy();
	m_normalMap.destroy();

	// C'est le light container qui enlevera sa référence de la map
	while (!m_lightMap.empty())
		m_lightMap.begin()->first->mtRemoveLightmap(this);

	O3D_ASSERT(m_lightMap.empty());

	PCLOD_MESSAGE(String("Lightmap : <") << zoneId << "> destroyed (" << texSize/1024 << "Ko" << ").");
}


//! (RT) Update of lights
void PCLODLightmap::rtUpdate()
{
	TimeCounter timeCounter(0.0f);
	timeCounter.update();

	// Le semaphore protège ici : m_normalMap, m_lightMap, m_staticLightmapBuffer
	ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::WRITE_LOCKER);

	Vector2i lComputeOrigin;
	Vector2i lComputeSize;
	Bool lNeedRefresh = False;

	for (IT_LightMap it = m_lightMap.begin() ; (it != m_lightMap.end()) && (!lNeedRefresh) ; it++)
	{
		if (it->first->isStaticLight() && it->first->needUpdate(it->second))
			lNeedRefresh = True;
	}

	if (lNeedRefresh)
	{
		if (m_staticLightmapBuffer.isEmpty())
			m_staticLightmapBuffer.setSize(m_normalMap.width(), m_normalMap.height());

		m_staticLightmapBuffer.fastFill(Vector3(0.0f, 0.0f, 0.0f));

		for (IT_LightMap it = m_lightMap.begin() ; it != m_lightMap.end() ; it++)
		{
			if (it->first->isStaticLight() && !it->first->generateLightmap(
						it->second,
						m_pTopZone,
						m_normalMap,
						m_staticLightmapBuffer,
						lComputeOrigin,
						lComputeSize,
						0))
				return;
		}

		m_staticLightmapChanged = True;

		timeCounter.update();

		PCLOD_MESSAGE(String("Static lightmap : <") << getZoneId() << "> updated in " <<
				1000.0f*timeCounter.elapsed() << "ms (" << m_staticLightmapBuffer.elt()/256 << "Ko)");
	}
}

/* Reload the texture if there is waiting data (MAIN THREAD)*/
void PCLODLightmap::mtUpdate()
{
    O3D_ASSERT(m_pTexture != nullptr);

	Vector2i lComputeOrigin;
	Vector2i lComputeSize;

	TimeCounter timeCounter(0.0f);

	// Le semaphore protege m_lightMap, m_staticLightmapBuffer
	ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::READ_LOCKER);

	Bool lNeedRefresh = (m_lightMap.empty() && (m_pTexture->getTextureId() == 0));

	for (IT_LightMap it = m_lightMap.begin() ; (it != m_lightMap.end()) && !lNeedRefresh ; it++)
	{
		if (it->first->isDynamicLight() && it->first->needUpdate(it->second))
			lNeedRefresh = True;
	}

	if (lNeedRefresh)
	{
		timeCounter.update();

		if (m_dynamicLightmapBuffer.isEmpty())
			m_dynamicLightmapBuffer.setSize(m_normalMap.width(), m_normalMap.height());

		m_dynamicLightmapBuffer.fastFill(Vector3(0.0f, 0.0f, 0.0f));

		for (IT_LightMap it = m_lightMap.begin() ; it != m_lightMap.end() ; it++)
		{
			if (it->first->isDynamicLight())
				it->first->generateLightmap(
						it->second,
						m_pTopZone,
						m_normalMap,
						m_dynamicLightmapBuffer,
						lComputeOrigin,
						lComputeSize,
						0);
		}
	}

	if (lNeedRefresh || m_staticLightmapChanged)
	{
		if (!lNeedRefresh)
			timeCounter.update();

		// La derniere étape est de fusionner les deux buffers
		T_LightMapBuffer lBufferSum;

		if (lNeedRefresh)
		{
			lBufferSum = m_dynamicLightmapBuffer;
			lBufferSum += m_staticLightmapBuffer;
		}
		else
		{
			lBufferSum = m_staticLightmapBuffer;
			lBufferSum += m_dynamicLightmapBuffer;
		}

		m_pTexture->create(
			False, 
			lBufferSum.width(),
			lBufferSum.height(),
			PF_RGB_F32,
			(Float*)&lBufferSum.getData()[0],
			PF_RGB_F32,
			False);

		m_staticLightmapChanged = False;
		timeCounter.update();

		PCLOD_MESSAGE(String("Dynamic lightmap : <") << getZoneId() << "> updated in " <<
				1000.0f*timeCounter.elapsed() << "ms (" << m_dynamicLightmapBuffer.elt()/256 << "Ko)");
	}
}

void PCLODLightmap::mtAddLight(PCLODLight * _pLight)
{
	ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::WRITE_LOCKER);

	O3D_ASSERT(m_lightMap.find(_pLight) == m_lightMap.end());

	// The lightInfo object is not initialized so that the light container
	// will know that this lightmap must be inconditionaly refreshed.
	m_lightMap[_pLight] = PCLODLightInfo();
}

void PCLODLightmap::mtRemoveLight(PCLODLight * _pLight)
{
	ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::WRITE_LOCKER);

	O3D_ASSERT(m_lightMap.find(_pLight) != m_lightMap.end());

	m_lightMap.erase(_pLight);
}

/* Define a neighbors of this zone */
void PCLODLightmap::setNeighbor(ZoneDirection _dir, PCLODLightmap * _pLightmap)
{
    O3D_ASSERT(_pLightmap != nullptr);

	addNeighbor(_dir, _pLightmap);
	_pLightmap->addNeighbor(static_cast<ZoneDirection>((_dir+2)%4), this);
}

PCLODLightmap * PCLODLightmap::getNeighbor(Int32 _direction, Int32 _index) const
{
	if (m_pNeighbors[_direction].empty())
        return nullptr;

	if (_index == O3D_MAX_INT32)
		return m_pNeighbors[_direction].back();
	else if (_index >= Int32(m_pNeighbors[_direction].size()))
        return nullptr;
	else
		return m_pNeighbors[_direction][_index];
}

/* Functions which provide a references counter */
void PCLODLightmap::useIt() const
{
	FastMutexLocker locker(m_counterMutex);
	m_refCounter++;
}

void PCLODLightmap::releaseIt() const
{
	FastMutexLocker locker(m_counterMutex);

	O3D_ASSERT(m_refCounter > 0);

	if (--m_refCounter == 0)
        onTextureUnused();
}

/* Restricted functions */
void PCLODLightmap::addNeighbor(ZoneDirection _dir, PCLODLightmap * _pLightmap)
{
	O3D_ASSERT(_dir < DIR_SIZE);
    O3D_ASSERT(_pLightmap != nullptr);

	T_LightmapArray & dirArray = m_pNeighbors[_dir];

	O3D_ASSERT(std::find(dirArray.begin(), dirArray.end(), _pLightmap) == dirArray.end());

	dirArray.push_back(_pLightmap);

	// La normalMap doit etre récalculée, donc on la supprime pour forcer sa génération
	// @todo Faire un événement en spécifiant la zone à recalculer. Inutile de tout refaire...
	// On désactive ca pour le moment
	//O3DFastMutexLocker lLocker(m_mutex);
	//m_normalMap.destroy();

#ifdef _DEBUG

	// Dans tous les cas, la taille de la liste des voisins est inférieure ou égale à
	// l'extension de la zone

	Vector2ui extension;
	m_pTopZone->getZoneManager()->getBaseZoneSize(extension);

	extension[X] = (m_pTopZone->getZoneSize()[X]-1) / (extension[X]-1);
	extension[Y] = (m_pTopZone->getZoneSize()[Y]-1) / (extension[Y]-1);

	O3D_ASSERT(m_pNeighbors[DIR_N].size() <= extension[Y]);
	O3D_ASSERT(m_pNeighbors[DIR_E].size() <= extension[X]);
	O3D_ASSERT(m_pNeighbors[DIR_S].size() <= extension[Y]);
	O3D_ASSERT(m_pNeighbors[DIR_W].size() <= extension[X]);

#endif
}

void PCLODLightmap::removeNeighbor(ZoneDirection _dir, PCLODLightmap * _pLightmap)
{
	O3D_ASSERT(_dir < DIR_SIZE);
    O3D_ASSERT(_pLightmap != nullptr);

	T_LightmapArray & dirArray = m_pNeighbors[_dir];

	IT_LightmapArray it = std::find(dirArray.begin(), dirArray.end(), _pLightmap);
	O3D_ASSERT(it != dirArray.end());

	dirArray.erase(it);
}

void PCLODLightmap::clearNeighbors()
{
	for (UInt32 k = 0 ; k < UInt32(DIR_SIZE) ; ++k)
	{
		T_LightmapArray & dirArray = m_pNeighbors[k];

		for (IT_LightmapArray it = dirArray.begin() ; it != dirArray.end() ; it++)
			(*it)->removeNeighbor(static_cast<ZoneDirection>((k + 2)%4), this);

		dirArray.clear();
	}
}

// Suppose toutes les zones de meme extension
// Pour gérer n'importe quelles extensions sans s'emmerder, on peut modifier le filtrage
// pour le calcul de la normalMap 2^k+1 sur les coins en ne prenant pas en compte les
// vertex des diagonales
void PCLODLightmap::buildNormalMap()
{
    O3D_ASSERT(m_pTopZone != nullptr);
	O3D_ASSERT(m_offsetLod <= 0);

//	m_offsetLod = 0;

	const Vector2ui lLightmapSize(m_pTopZone->getZoneSize());

	SmartArrayFloat realHeightmapBuffer;
	Vector2ui realHeightmapSize;

	// On récupère la heightmap de la zone
	m_pTopZone->getHeightmap(realHeightmapBuffer, realHeightmapSize);

	// On applique le lod
	Vector2ui heightmapSize((realHeightmapSize[X] >> (-m_offsetLod)) + (m_offsetLod != 0 ? 1 : 0),
							   (realHeightmapSize[Y] >> (-m_offsetLod)) + (m_offsetLod != 0 ? 1 : 0));
	SmartArrayFloat heightmapBuffer(heightmapSize[X] * heightmapSize[Y]);
	UInt8 lodFactor = 1 << (-m_offsetLod);

	for (UInt32 y = 0 ; y < heightmapSize[Y] ; ++y)
		for (UInt32 x = 0 ; x < heightmapSize[X] ; ++x)
		{
			heightmapBuffer[x + y * heightmapSize[X]] = realHeightmapBuffer[x*lodFactor + y*lodFactor*realHeightmapSize[X]];
		}

/*	O3DFloatSmartArray heightmapBuffer;
	O3DVector2ui heightmapSize;
	UInt32 lodFactor = 1;

	// On récupère la heightmap de la zone
	m_pTopZone->getHeightmap(heightmapBuffer, heightmapSize);*/

	// On créé une nouvelle heightmap de taille 35*35, 67*67, 131*131 prenant la compte les zones voisines
	// La normalMap temporaire générée sera donc de taille 35*35, 67*67, 131*131
	// La normalMap finale générée sera donc de taille 32*32, 64*64, 128*128
	// Taille de la heightmap en float
	const UInt32 realWidth = heightmapSize[X]+2;
	const UInt32 realHeight = heightmapSize[Y]+2;

	const UInt32 realMaxWidth = realWidth-1;
	const UInt32 realMaxHeight = realHeight-1;

	const UInt32 finalWidth = heightmapSize[X]-1;
	const UInt32 finalHeight = heightmapSize[Y]-1;

	// Allocation de la normal Map
	if (m_normalMap.isEmpty())
		m_normalMap.setSize(finalWidth, finalHeight);

	Array2DFloat heightmap(realWidth, realHeight);

	for (UInt32 y = 0 ; y < heightmapSize[Y] ; ++y)
		memcpy((void*)&heightmap(1, 1+y), (const void*)&heightmapBuffer[y*heightmapSize[X]], heightmapSize[X]*sizeof(Float));

	// Initialisation des bords de la heightmap en fonction des voisins

	// On récupère certaines infos à partir du ZoneManager
	Vector2f zoneUnits;
	m_pTopZone->getZoneManager()->getZoneUnits(zoneUnits);

	SmartArrayFloat neighborHeightmap;
	Vector2ui neighborSize;

	// Construction du bord gauche
	if (m_pNeighbors[DIR_W].size() > 0)
	{
		UInt32 row = 1;

		for (IT_LightmapArray it = m_pNeighbors[DIR_W].begin() ; it != m_pNeighbors[DIR_W].end() ; it++)
		{
			(*it)->m_pTopZone->getHeightmap(neighborHeightmap, neighborSize);

			for (UInt32 k = 0 ; k < neighborSize[Y] ; k += lodFactor, ++row)
				heightmap(0,row) = neighborHeightmap[neighborSize[X]-2 + k * neighborSize[X]];
		}
	}
	else // Si il n'y a pas de voisins, on copie juste les valeurs
		for (UInt32 k = 0 ; k < realHeight ; ++k)
			heightmap(0,k) = heightmap(1, k);

	// Construction du bord droit
	if (m_pNeighbors[DIR_E].size() > 0)
	{
		UInt32 row = 1;

		for (IT_LightmapArray it = m_pNeighbors[DIR_E].begin() ; it != m_pNeighbors[DIR_E].end() ; it++)
		{
			(*it)->m_pTopZone->getHeightmap(neighborHeightmap, neighborSize);

/*			Int8 deltaLod = -(*it)->getLod() + getLod();
			UInt32 deltaIndex = 1 << deltaLod;*/

			for (UInt32 k = 0 ; k < neighborSize[Y] ; k += lodFactor, ++row)
			{
				heightmap(realMaxWidth,row)		= neighborHeightmap[1 + k * neighborSize[X]];

/*				if ((deltaLod > 0) && (k % deltaIndex == 0) && (k > 0))
				{
					UInt32 factor = 1;

					for (Int32 m = row - deltaIndex + 1; m < Int32(row) ; ++m, ++factor)
					{
						heightmap(realMaxWidth, m) = heightmap(realMaxWidth, row - deltaIndex) + (Float(factor)/Float(deltaIndex)) * (heightmap(realMaxWidth, row) - heightmap(realMaxWidth, row - deltaIndex));
						heightmap(realMaxWidth-1, m) = heightmap(realMaxWidth-1, row - deltaIndex) + (Float(factor)/Float(deltaIndex)) * (heightmap(realMaxWidth-1, row) - heightmap(realMaxWidth-1, row - deltaIndex));
					}
				}*/
			}
		}
	}
	else // Si il n'y a pas de voisins
		for (UInt32 k = 0 ; k < realHeight ; ++k)
			heightmap(realMaxWidth,k) = heightmap(realMaxWidth-1, k);

	// Construction du bord haut
	if (m_pNeighbors[DIR_N].size() > 0)
	{
		UInt32 row = 1;

		for (IT_LightmapArray it = m_pNeighbors[DIR_N].begin() ; it != m_pNeighbors[DIR_N].end() ; it++)
		{
			(*it)->m_pTopZone->getHeightmap(neighborHeightmap, neighborSize);

			for (UInt32 k = 0 ; k < neighborSize[X] ; k += lodFactor, ++row)
				heightmap(row, realMaxHeight)	= neighborHeightmap[k + neighborSize[X]];
		}
	}
	else // Si il n'y a pas de voisins
		for (UInt32 k = 0 ; k < realWidth ; ++k)
			heightmap(k, realMaxHeight) = heightmap(k, realMaxHeight-1);

	// Construction du bord bas
	if (m_pNeighbors[DIR_S].size() > 0)
	{
		UInt32 row = 1;

		for (IT_LightmapArray it = m_pNeighbors[DIR_S].begin() ; it != m_pNeighbors[DIR_S].end() ; it++)
		{
			(*it)->m_pTopZone->getHeightmap(neighborHeightmap, neighborSize);

			for (UInt32 k = 0 ; k < neighborSize[X] ; k += lodFactor, ++row)
				heightmap(row,0) = neighborHeightmap[k + (neighborSize[Y]-2)*neighborSize[X]];
		}
	}
	else // Si il n'y a pas de voisins
		for (UInt32 k = 0 ; k < realWidth ; ++k)
			heightmap(k,0) = heightmap(k, 1);

	// On cherche les valeurs des coins N-E, S-E, S-W, N-W ont été définie

	// Coin N-E
	if ((m_pNeighbors[DIR_N].size() > 0) && (m_pNeighbors[DIR_N].front()->m_pNeighbors[DIR_E].size() > 0))
	{
		PCLODLightmap * pNELightmap = m_pNeighbors[DIR_N].front()->m_pNeighbors[DIR_E].front();

		pNELightmap->m_pTopZone->getHeightmap(neighborHeightmap, neighborSize);

		heightmap(realMaxWidth, realMaxHeight) = neighborHeightmap[1 + neighborSize[X]];
	}

	// Coin S-E
	if ((m_pNeighbors[DIR_S].size() > 0) && (m_pNeighbors[DIR_S].front()->m_pNeighbors[DIR_E].size() > 0))
	{
		PCLODLightmap * pSELightmap = m_pNeighbors[DIR_S].front()->m_pNeighbors[DIR_E].front();

		pSELightmap->m_pTopZone->getHeightmap(neighborHeightmap, neighborSize);

		heightmap(realMaxWidth, 0) = neighborHeightmap[1 + (neighborSize[Y]-2)*neighborSize[X]];
	}

	// Coin N-W
	if ((m_pNeighbors[DIR_N].size() > 0) && (m_pNeighbors[DIR_N].front()->m_pNeighbors[DIR_W].size() > 0))
	{
		PCLODLightmap * pNWLightmap = m_pNeighbors[DIR_N].front()->m_pNeighbors[DIR_W].front();

		pNWLightmap->m_pTopZone->getHeightmap(neighborHeightmap, neighborSize);

		heightmap(0, realMaxHeight) = neighborHeightmap[neighborSize[X]-2 + neighborSize[X]];
	}

	// Coin S-W
	if ((m_pNeighbors[DIR_S].size() > 0) && (m_pNeighbors[DIR_S].front()->m_pNeighbors[DIR_W].size() > 0))
	{
		PCLODLightmap * pSWLightmap = m_pNeighbors[DIR_S].front()->m_pNeighbors[DIR_W].front();

		pSWLightmap->m_pTopZone->getHeightmap(neighborHeightmap, neighborSize);

		heightmap(0, 0) = neighborHeightmap[neighborSize[X]-2 + (neighborSize[Y]-2)*neighborSize[X]];
	}


	// Allocation de la normalMap temporaire
	// On calcule les normales moyennes en chaque sommet.
	const UInt32 mapWidth = realWidth-2;
	const UInt32 mapHeight = realHeight-2;

	Float * pTempMap = new Float[3*mapWidth*mapHeight];
	Float * pNormal = pTempMap;

	// We can compute all points easily, but the border needs specials cares
	for (UInt32 y = 1 ; y < realHeight-1 ; ++y)
	{
		for (UInt32 x = 1 ; x < realWidth-1 ; ++x)
		{
			pNormal[X] = heightmap(x-1, y+1) - heightmap(x+1,y-1) + 2*heightmap(x-1, y)-2*heightmap(x+1, y);
			pNormal[Y] = heightmap(x+1, y-1) - heightmap(x-1,y+1) + 2*heightmap(x, y-1)-2*heightmap(x, y+1);
			pNormal[Z] = 6*zoneUnits[X];

			pNormal += 3;
		}
	}

	// A partir de toutes ces normales, on peut calculer la normalMap finale
	const Vector3 * pN0 = (Vector3*)pTempMap;
	const Vector3 * pN1 = pN0 + 2*mapWidth;

	Vector3 * ptr = m_normalMap.getData();

	// Première ligne
	*ptr = *pN0;
	ptr->normalize();
	++ptr;
	++pN0;

	for (UInt32 x = finalWidth-2 ; x > 0 ; --x)
	{
		*ptr = *pN0 + *(pN0 + 1);
		ptr->normalize();
		++ptr;
		++pN0;
	}

	*ptr = *(pN0+1);
	ptr->normalize();
	++ptr;
	pN0 += 2;

	// Cas général
	for (UInt32 y = finalHeight-2 ; y > 0 ; --y)
	{
		*ptr = *pN0 + *pN1;
		ptr->normalize();
		++ptr;
		++pN0;
		++pN1;

		for (UInt32 x = finalWidth-2 ; x > 0 ; --x)
		{
			*ptr = *pN0 + *(pN0 + 1) + *pN1 + *(pN1 + 1);

			ptr->normalize();

			++pN0;
			++pN1;
			++ptr;
		}

		*ptr = *(pN0+1) + *(pN1+1);
		ptr->normalize();
		++ptr;
		++pN0;
		++pN1;

		++pN0;
		++pN1;
	}

	// Dernière ligne
	*ptr = *pN1;
	ptr->normalize();
	++ptr;
	++pN1;

	for (UInt32 x = finalWidth-2 ; x > 0 ; --x)
	{
		*ptr = *pN1 + *(pN1 + 1);
		ptr->normalize();
		++ptr;
		++pN1;
	}

	*ptr = *(pN1+1);
	ptr->normalize();

	deleteArray(pTempMap);
}

