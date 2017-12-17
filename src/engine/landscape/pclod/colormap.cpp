/**
 * @file colormap.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/landscape/pclod/colormap.h"

#include "o3d/core/smartarray.h"

#include "o3d/engine/landscape/pclod/terrain.h"
#include "o3d/engine/landscape/pclod/object.h"
#include "o3d/engine/landscape/pclod/pclodmaterial.h"
#include "o3d/engine/landscape/pclod/pclodtexturemanager.h"
#include "o3d/core/string.h"
#include "o3d/core/memorydbg.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/objects.h"
#include "o3d/core/fileinfo.h"
#include "o3d/image/image.h"
#include "o3d/engine/texture/texture2d.h"

using namespace o3d;

/*================================================================
					class O3DPCLODColormap
================================================================*/
PCLODColormap::PCLODColormap(BaseObject *pParent, PCLODTopZone * _pZone) :
	m_pParent(pParent),
	m_zoneId(0),
	m_filePath(),
	m_filePosition(),
	m_refCounter(0),
	m_pTexture(NULL),
	m_pTopZone(_pZone),
	m_mutex(),
	m_staticNoise(),
	m_staticNoiseSize()
{
    O3D_ASSERT(m_pTopZone != nullptr);

	m_pTopZone->use(PCLODTopZone::ZONE_MATERIAL);
	m_pTopZone->useIt();
}

PCLODColormap::PCLODColormap(BaseObject *pParent, PCLODTopZone * _pZone, UInt32 _zoneId, const String _path, UInt32 _position):
	m_pParent(pParent),	
	m_zoneId(_zoneId),
	m_filePath(_path),
	m_filePosition(_position),
	m_refCounter(0),
	m_pTexture(NULL),
	m_pTopZone(_pZone),
	m_mutex(),
	m_staticNoise(),
	m_staticNoiseSize()
{
    O3D_ASSERT(m_pTopZone != nullptr);

	m_pTopZone->use(PCLODTopZone::ZONE_MATERIAL);
	m_pTopZone->useIt();
}

PCLODColormap::~PCLODColormap()
{
    O3D_ASSERT(m_pTexture == nullptr);
    O3D_ASSERT(m_pTopZone == nullptr);
}

PCLODTerrain* PCLODColormap::getTerrain() const
{
	return ((PCLODTextureManager*)m_pParent)->getTerrain();
}

/* Configs */
void PCLODColormap::useStaticNoise(const SmartArrayInt8 & _noise, UInt32 _width, UInt32 _height)
{
	m_staticNoise = _noise;
	m_staticNoiseSize = Vector2ui(_width, _height);
}

/* IO Functions */

/** Cette fonction charge la colormap a partir du disque dur.
 * - Retourne False si le fichier n'existe pas ou n'est pas lisible
 * - Si la fonction retourne False, la colormap a été initialisé avec une texture noir. **/
Bool PCLODColormap::rtLoad()
{
    O3D_ASSERT(m_pTexture == nullptr);

	Bool loadingError = False;

	FileInfo fileInfo(m_filePath);

	if (!fileInfo.exists())
	{
		// If the file is not found, we load a black texture

		PCLOD_WARNING(String("Colormap : Colormap not found (") << m_filePath << "). Will be generated");

		Image emptyPict;
		emptyPict.loadWithColor(4, 4, 0, 0, 0, 255, False);

		// The ref counter is incremented at the end of this function
        m_pTexture = new Texture2D(m_pParent, emptyPict);
		m_pTexture->setFiltering(Texture::BILINEAR_FILTERING);
        m_pTexture->setName("<emptyPict>");
        m_pTexture->setResourceName("<emptyPict>");
		m_pTexture->setWrap(Texture::CLAMP);

		loadingError = True;
	}
	else
	{
        AutoPtr<InStream> colormapFile;

        colormapFile = FileManager::instance()->openInStream(m_filePath);
        colormapFile->reset(m_filePosition);

		// Try to load the picture file
		Image colormapPic(*colormapFile);
		if (colormapPic.isValid())
		{
			m_pTexture = new Texture2D(m_pParent, colormapPic);
		}
		else
		{
			PCLOD_WARNING(String("Colormap : Colormap file found (") << m_filePath <<
					"), but its format is not supported. Will be generated");

			Image emptyPict;
			emptyPict.loadWithColor(4, 4, 0, 0, 0, 255, False);

			m_pTexture = new Texture2D(m_pParent, emptyPict);
		}

		m_pTexture->setFiltering(Texture::BILINEAR_FILTERING);
		m_pTexture->setWrap(Texture::CLAMP);
	}

	PCLOD_MESSAGE(String("Colormap : Colormap (") << m_filePath << ") loaded (" <<
		   	m_pTexture->getImage().getSize()/1024 << "Ko).");

	return (!loadingError);
}

// Libère les ressources occupées
// Cette fonction est appelée par le texture manager dans l'événement
// MTE_OnColormapDelection.
void PCLODColormap::mtUnload()
{
	O3D_ASSERT(m_pTexture != NULL);

	UInt32 texSize = m_pTexture->getImage().getSize();

	deletePtr(m_pTexture);

	m_zoneId = 0;
	m_filePath.destroy();
	m_filePosition = 0;

	m_pTopZone->release(PCLODTopZone::ZONE_MATERIAL);
	m_pTopZone->releaseIt();
	m_pTopZone = NULL;

	m_staticNoise.releaseCheckAndDelete();
	m_staticNoiseSize = Vector2ui(0, 0);

	PCLOD_MESSAGE(String("Colormap : Colormap (") << m_filePath << ") released (" << texSize/1024 << "Ko).");

	m_pParent = NULL;
}

/** Génère la colormap à partir de la table des matériaux et de la précision demandée.
 *  - Pour une heightmap de dimension a,b, la colormap aura pour dimension k*(a-1), k*(b-1)
 *    avec k la précision demandée. Dans tous les cas, la taille maximale est 4096*4096 */
void PCLODColormap::rtGenerate(O3D_T_MatTable & _matTable, UInt32 _samplePrecision)
{
	O3D_ASSERT(m_pTexture != NULL);

	// Si _sampleSize n'est pas disponible pour certain matériau, ils génèreront le sample.
	TimeCounter timeCounter(0.0f);
	timeCounter.update();

	const UInt32 _sampleSize = 1 << _samplePrecision;

	SmartArrayUInt32 materialBuffer;
	Vector2ui materialSize;

	m_pTopZone->getMaterial(materialBuffer, materialSize);

	const UInt32 matWidth = materialSize[X];
	//const UInt32 matHeight = materialSize[Y];

	const UInt32 width = (materialSize[X]-1) * _sampleSize;
	const UInt32 height = (materialSize[Y]-1) * _sampleSize;

	m_pTexture->getImage().loadWithColor(width, height, 0, 0, 0, 255, True);

	PCLODMaterial::PCLODRgba * const pData = (PCLODMaterial::PCLODRgba*)m_pTexture->getImage().getData();

	const PCLODMaterial::PCLODRgba * pSample = NULL;

	// Création du sample null.
	PCLODMaterial::PCLODRgba * pNullSample = new PCLODMaterial::PCLODRgba[_sampleSize*_sampleSize];
	memset((void*)pNullSample, 0, _sampleSize*_sampleSize * sizeof(PCLODMaterial::PCLODRgba));

	// Création des 4 samples de fondus
	// Leur taille est fixé par la précision demandée
	const UInt32 pSamplePixel = _sampleSize*_sampleSize;
	//UInt8 decPower = (UInt8)_samplePrecision;

	UInt8 * pTransitionSample = new UInt8[4*pSamplePixel];

	UInt8 * const pFirstSample	= pTransitionSample;
	UInt8 * const pSecondSample = pFirstSample + pSamplePixel;
	UInt8 * const pThirdSample	= pSecondSample + pSamplePixel;
	UInt8 * const pFourthSample = pThirdSample + pSamplePixel;

	if (_sampleSize > 1)
	{
		for (UInt8 y = 0 ; y < _sampleSize ; ++y)
			for(UInt8 x = 0 ; x < _sampleSize; ++x)
			{
				pFirstSample[x + y * _sampleSize]	= (UInt8)( _sampleSize - 1 - o3d::max<UInt8>(x,y));
				pSecondSample[x + y * _sampleSize]	= (UInt8)( (_sampleSize - 1 - x + y < _sampleSize-1 ?_sampleSize - 1 - (y + _sampleSize - 1 - x):0) );
				pThirdSample[x + y * _sampleSize]	= o3d::min<UInt8>(x,y);
				pFourthSample[x + y * _sampleSize]	= (UInt8)( (x + _sampleSize - 1 - y < _sampleSize-1 ?_sampleSize - 1 - (_sampleSize - 1 - y + x):0) );
/*				pFirstSample[x + y * _sampleSize]	= _sampleSize - 1 - o3d::max<UInt8>(x, y);
				pSecondSample[x + y * _sampleSize]	= _sampleSize - 1 - o3d::max<UInt8>(_sampleSize - 1 -x, y);
				pThirdSample[x + y * _sampleSize]	= _sampleSize - 1 - o3d::max<UInt8>(x, _sampleSize - 1 -y);
				pFourthSample[x + y * _sampleSize]	= _sampleSize - 1 - o3d::max<UInt8>(_sampleSize - 1 -x, _sampleSize - 1 -y);*/
			}

/*		UInt8 first[] = { 3, 2, 1, 0, 2, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0 };
		UInt8 second[] = { 0, 1, 2, 3, 0, 1, 1, 2, 0, 0, 1, 1, 0, 0, 0, 0 };
		UInt8 third[] = { 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 2, 0, 1, 2, 3 };
		UInt8 fourth[] = { 0, 0, 0, 0, 1, 1, 0, 0, 2, 1, 1, 0, 3, 2, 1, 0 };

		memcpy((void*)pFirstSample, first, 16*sizeof(UInt8));
		memcpy((void*)pSecondSample, second, 16*sizeof(UInt8));
		memcpy((void*)pThirdSample, third, 16*sizeof(UInt8));
		memcpy((void*)pFourthSample, fourth, 16*sizeof(UInt8));*/
	}
	else
	{
		//decPower = 2;
		memset((void*)pTransitionSample, 1, 4*sizeof(UInt8));
	}

	UInt32 lastMaterialId = 0;

	UInt32 * row0Id = materialBuffer.getData();
	UInt32 * row1Id = row0Id + matWidth;

	for (UInt32 y = 0 ; y < materialSize[Y]-1 ; ++y)
	{
		for (UInt32 x = 0 ; x < materialSize[X]-1 ; ++x, ++row0Id, ++row1Id)
		{
			if ((_samplePrecision == 0) || ((*row0Id == *row1Id) && (*(row0Id+1) == *row0Id) && (*(row1Id+1) == *row1Id)))
			{
				if ((*row0Id != lastMaterialId) || (pSample == NULL))
				{
					O3D_IT_MatTable it = _matTable.find(*row0Id);

					if ((it == _matTable.end() || (pSample = it->second->getSample(_samplePrecision)) == NULL))
						pSample = pNullSample;

					lastMaterialId = *row0Id;
				}

				for (UInt32 k = 0; k < _sampleSize; ++k)
					memcpy((void*)&pData[x * _sampleSize + (y * _sampleSize + k) * width], (const void*)&pSample[k*_sampleSize], _sampleSize*sizeof(PCLODMaterial::PCLODRgba));
			}
			else
			{
				O3D_IT_MatTable it0 = _matTable.find(*row0Id);
				O3D_IT_MatTable it1 = _matTable.find(*(row0Id+1));
				O3D_IT_MatTable it2 = _matTable.find(*(row1Id+1));
				O3D_IT_MatTable it3 = _matTable.find(*row1Id);

				const PCLODMaterial::PCLODRgba * s0 = (it0 != _matTable.end() ? it0->second->getSample(_samplePrecision) : pNullSample);
				const PCLODMaterial::PCLODRgba * s1 = (it1 != _matTable.end() ? it1->second->getSample(_samplePrecision) : pNullSample);
				const PCLODMaterial::PCLODRgba * s2 = (it2 != _matTable.end() ? it2->second->getSample(_samplePrecision) : pNullSample);
				const PCLODMaterial::PCLODRgba * s3 = (it3 != _matTable.end() ? it3->second->getSample(_samplePrecision) : pNullSample);

				UInt8 k = 0;
				UInt8 div = (UInt8) (_sampleSize - 1);

				for (UInt8 j = 0 ; j < _sampleSize ; ++j)
				{
					PCLODMaterial::PCLODRgba * ptr = &pData[x * _sampleSize + (y * _sampleSize + j) * width];

					for(UInt8 i = 0 ; i < _sampleSize; ++i, ++ptr, ++s0, ++s1, ++s2, ++s3, ++k)
					{
						ptr->red	= *(pFirstSample + k)	* (s0->red / div);
						ptr->green	= *(pFirstSample + k)	* (s0->green / div);
						ptr->blue	= *(pFirstSample + k)	* (s0->blue / div);
						ptr->alpha	= 255;

						ptr->red	+= *(pSecondSample + k)	* (s1->red / div);
						ptr->green	+= *(pSecondSample + k)	* (s1->green / div);
						ptr->blue	+= *(pSecondSample + k)	* (s1->blue / div);

						ptr->red	+= *(pThirdSample + k)	* (s2->red / div);
						ptr->green	+= *(pThirdSample + k)	* (s2->green / div);
						ptr->blue	+= *(pThirdSample + k)	* (s2->blue / div);

						ptr->red	+= *(pFourthSample + k)	* (s3->red / div);
						ptr->green	+= *(pFourthSample + k)	* (s3->green / div);
						ptr->blue	+= *(pFourthSample + k)	* (s3->blue / div);

/*						UInt32 color[4];
						color[0] = *(pFirstSample + k) * s0->red + *(pSecondSample + k) * s1->red + *(pThirdSample + k)	* s2->red + *(pFourthSample + k) * s3->red;
						color[0] /= div;
						color[1] = *(pFirstSample + k) * s0->green + *(pSecondSample + k) * s1->green + *(pThirdSample + k)	* s2->green + *(pFourthSample + k) * s3->green;
						color[1] /= div;
						color[2] = *(pFirstSample + k) * s0->blue + *(pSecondSample + k) * s1->blue + *(pThirdSample + k)	* s2->blue + *(pFourthSample + k) * s3->blue;
						color[2] /= div;

						ptr->red = color[0];
						ptr->green = color[1];
						ptr->blue = color[2];
						ptr->alpha = 255;*/
					}
				}
			}
		}

		++row0Id;
		++row1Id;
	}

	deleteArray(pNullSample);
	deleteArray(pTransitionSample);

	// Application d'un bruit statique si l'option est activé
//	const Image * pPicture = NULL;

	if ((m_staticNoise.getNumElt() > 0) && (m_staticNoiseSize != Vector2ui(0, 0)))
	{
		UInt8 * pTarget = (UInt8*)m_pTexture->getImage().getDataWrite();
		const UInt32 bpp = m_pTexture->getImage().getBpp() >> 3;

		for (UInt32 y = 0 ; y < height ; ++y)
		{
			const Int8 * pNoise = &m_staticNoise.getData()[(y % m_staticNoiseSize[Y]) * m_staticNoiseSize[X]];

			UInt32 counter = 0;
			for (UInt32 x = width ; x > 0 ; --x, ++pNoise, ++counter, pTarget += bpp)
			{
				pTarget[0] = (UInt8)o3d::max<Int16>(o3d::min<Int16>(Int16(pTarget[0]) + Int16(*pNoise), 255), 0);
				pTarget[1] = (UInt8)o3d::max<Int16>(o3d::min<Int16>(Int16(pTarget[1]) + Int16(*pNoise), 255), 0);
				pTarget[2] = (UInt8)o3d::max<Int16>(o3d::min<Int16>(Int16(pTarget[2]) + Int16(*pNoise), 255), 0);

				if (counter >= m_staticNoiseSize[X])
				{
					counter -= m_staticNoiseSize[X];
					pNoise -= m_staticNoiseSize[X];
				}
			}
		}
	}

	timeCounter.update();

	PCLOD_MESSAGE(String("Colormap : <") << getZoneId() << "> ("
		<< width << "x" << height << ") generated in "
		<< (Float)(1000.0f*timeCounter.elapsed()) << "ms ("
		<< (UInt32)(width*height*sizeof(PCLODMaterial::PCLODRgba)/1024) << "Ko)");
}

/* Load the texture to openGL */
void PCLODColormap::loadToGL() const
{
	if (!loaded())
	{
		PCLOD_ERROR(String("Colormap : Attempt to load the texture <") << m_filePath <<
				"> into video memory, but you must load it from disk before");
	}
	else if (!m_pTexture->isValid())
	{
		Texture2D * pTexture = getTexture();
		pTexture->create(True);                    // create using mipmaps
		pTexture->setWrap(Texture::CLAMP);
		pTexture->setFiltering(Texture::BILINEAR_FILTERING);

		if (!m_pTexture->isValid())
		{
			PCLOD_ERROR(String("Colormap : An error occured while trying to send the texture <") <<
					m_filePath << "> to openGL");
		}
	}
}

/* Functions which provide a references counter */
void PCLODColormap::useIt() const
{
	const_cast<PCLODColormap*>(this)->m_refCounter++;
}

void PCLODColormap::releaseIt() const
{
	O3D_ASSERT(m_refCounter > 0);
	const_cast<PCLODColormap*>(this)->m_refCounter--;

	if (m_refCounter == 0)
        onTextureUnused();
}

