/**
 * @file pclodmaterial.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/landscape/pclod/pclodmaterial.h"

#include "o3d/engine/landscape/pclod/terrain.h"
#include "o3d/engine/landscape/pclod/pclodtexturemanager.h"
#include "o3d/core/string.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/fileinfo.h"
#include "o3d/core/memorydbg.h"
#include "o3d/engine/texture/texture2d.h"

#include <memory>

using namespace o3d;

/*================================================================
                    class O3DPCLODMaterial
================================================================*/
PCLODMaterial::PCLODMaterial(
		BaseObject *pParent,
		UInt32 _matId,
		const String _path,
		UInt32 _position) :
	m_pParent(pParent),	
	m_matId(_matId),
	m_filePath(_path),
	m_filePosition(_position),
    m_pTexture(nullptr),
	m_refCounter(0),
	m_mutex(),
	m_sample()
{
}

PCLODMaterial::~PCLODMaterial()
{
    O3D_ASSERT(m_pTexture == nullptr);
	O3D_ASSERT(m_sample.empty());
}

/* Functions */

PCLODTerrain* PCLODMaterial::getTerrain() const
{
	return ((PCLODTextureManager*)m_pParent)->getTerrain();
}

/* IO Functions */
void PCLODMaterial::rtLoad()
{
	O3D_ASSERT(!loaded());

	FileInfo lFileInfo(m_filePath);

	if (!lFileInfo.exists())
	{
		// If the file is not found, we load a black texture

		// If the file was not defined on purpose, we dont show any message
		if (m_filePath.length() > 0)
		{
			PCLOD_WARNING(String("Material : Material not found (") << m_filePath <<
					". Will be replaced by a black texture");
		}

		Image emptyPict;
		emptyPict.loadWithColor(4, 4, 0, 0, 0, 255, False);

        m_pTexture = new Texture2D(m_pParent, emptyPict);
		m_pTexture->setFiltering(Texture::BILINEAR_FILTERING);
		m_pTexture->setWrap(Texture::REPEAT);
        m_pTexture->setName("<emptyPict>");
        m_pTexture->setResourceName("<emptyPict>");
	}
	else
	{
		// Manage exceptions
        AutoPtr<InStream> materialFile(FileManager::instance()->openInStream(m_filePath));

        materialFile->reset(m_filePosition);

		// Try to load the picture file
		Image materialPic(*materialFile);
		if (materialPic.isValid())
		{
			m_pTexture = new Texture2D(m_pParent, materialPic);
		}
		else
		{
			PCLOD_WARNING(String("Material : Material file found (") << m_filePath <<
					"), but its format is not supported. Will be replaced by a black texture");

			Image emptyPict;
			emptyPict.loadWithColor(4, 4, 0, 0, 0, 255, False);

			m_pTexture = new Texture2D(m_pParent, emptyPict);
		}

		m_pTexture->setFiltering(Texture::BILINEAR_FILTERING);
		m_pTexture->setWrap(Texture::REPEAT);
	}

	// Il s'agit ici que d'une précision standard
	generateSample(2);

	PCLOD_MESSAGE(String("Material : Material (") << m_matId << ") loaded (" <<
			m_pTexture->getImage().getSize()/1024 << "Ko).");
}

void PCLODMaterial::rtUnload()
{
	// Cette fonction est appelée avant d'envoyer dans le thread principal
	// la requete de destruction de la texture de matériau. Il faut donc la
	// libérer.
	O3D_ASSERT(loaded());
    O3D_ASSERT(m_pTexture != nullptr);

	UInt32 texSize = m_pTexture->getImage().getSize();

	//m_pTexture->setParent(NULL);
	m_pTexture = NULL;

	for (IT_MatSampleMap it = m_sample.begin() ; it != m_sample.end() ; it++)
		deleteArray(it->second);

	m_sample.clear();

	PCLOD_MESSAGE(String("Material : Material (") << m_filePath <<
			") released (" << texSize/1024 << "Ko).");
}

// Returne le sample avec la précision demandée. Le génère s'il n'existe pas a cette precision */
PCLODMaterial::PCLODRgba * PCLODMaterial::getSample(UInt32 _precision)
{
	IT_MatSampleMap it = m_sample.find(_precision);

	if (it != m_sample.end())
		return it->second;
	else
	{
		PCLOD_WARNING(String("Texture Material <") << m_matId << "> : Sample size " <<
				(1 << _precision) << " not available, must be generated");

		generateSample(_precision);

		it = m_sample.find(_precision);
		if (it != m_sample.end())
			return it->second;
		else
            return nullptr;
	}
}

/* Load the texture to openGL */
void PCLODMaterial::loadToGL() const
{
	if (!loaded())
	{
		PCLOD_ERROR(String("Material : Attempt to load the texture <") << m_filePath <<
				"> into video memory, but you must load it from disk before");
		return;
	}

	if (m_pTexture->isValid())
		return;
	else
	{
		Texture2D * pTexture = getTexture();
		pTexture->create(True);

		if (!m_pTexture->isValid())
		{
			PCLOD_ERROR(String("Material : An error occured while trying to send the texture <") <<
					m_filePath << "> to openGL");
			return;
		}
	}
}

/* Functions which provide a references counter */
void PCLODMaterial::useIt() const
{
	m_mutex.lock();
	const_cast<PCLODMaterial*>(this)->m_refCounter++;
	m_mutex.unlock();
}

void PCLODMaterial::releaseIt() const
{
	FastMutexLocker locker(m_mutex);
	O3D_ASSERT(m_refCounter > 0);
	const_cast<PCLODMaterial*>(this)->m_refCounter--;

	if (m_refCounter == 0)
	{
        onTextureUnused();
	}
}

/** Cette fonction créée l'échantillon à partir de la texture haute qualité.
  * - La texture doit être chargée. **/
void PCLODMaterial::generateSample(UInt32 _samplePrecision)
{
	UInt32 _sampleSize = 1 << _samplePrecision;

    O3D_ASSERT(m_pTexture != nullptr);
	O3D_ASSERT(!m_pTexture->getImage().isEmpty());
	O3D_ASSERT(m_pTexture->getImage().getWidth() == m_pTexture->getImage().getHeight());

	if (m_sample.find(_samplePrecision) != m_sample.end())
		return;

	if ((m_sample.size() == 0) || (_samplePrecision > m_sample.rbegin()->first))
	{
		// Quand le niveau est supérieur à tous ceux générés auparavent,
		// on a pas le choix. Il faut le recalculer.
		PCLODRgba * pNewSample = new PCLODRgba[_sampleSize * _sampleSize];

		Image & picture = m_pTexture->getImage();
		UInt8 * pData = picture.getDataWrite();

		const UInt32 width = picture.getWidth();
		const UInt32 bpp = picture.getBpp() >> 3;

		UInt32 blockSize = picture.getWidth() / _sampleSize;
		UInt32 blockPixel = blockSize*blockSize;

		for (UInt32 j = 0 ; j < _sampleSize ; ++j)
			for (UInt32 i = 0 ; i < _sampleSize ; ++i)
			{
				UInt32 color[4] = {0, 0, 0, 0};

				for (UInt32 y = blockSize ; y > 0 ; --y)
				{
					PCLODRgba * ptr = (PCLODRgba*)&pData[bpp*(i*blockSize + (j*blockSize+y-1)*width)];

					for (UInt32 x = blockSize; x > 0 ; --x)
					{
						color[0] += ptr->red;
						color[1] += ptr->green;
						color[2] += ptr->blue;
						color[3] += ((bpp==4)?ptr->alpha:255);

						ptr = (PCLODRgba*)((UInt8*)ptr + (bpp==4?4:3));
					}
				}

				color[0] /= blockPixel;
				color[1] /= blockPixel;
				color[2] /= blockPixel;
				color[3] /= blockPixel;

				PCLODRgba medColor = { UInt8(color[0]), UInt8(color[1]), UInt8(color[2]), UInt8(color[3]) };
				pNewSample[i + j * _sampleSize] = medColor;
			}

		m_sample[_samplePrecision] = pNewSample;
	}
	else
	{
		// Sinon c'est plus simple, on prend le sample du niveau supérieur
		IT_MatSampleMap upperBound = m_sample.upper_bound(_samplePrecision);

		const UInt32 prevSize = upperBound->first;
		const PCLODRgba * pPrevSample = upperBound->second;

		const UInt32 blockSize = prevSize / _sampleSize;
		const UInt32 blockPixel = blockSize*blockSize;

		PCLODRgba * pNewSample = new PCLODRgba[_sampleSize * _sampleSize];
		const PCLODRgba * ptr = pPrevSample;

		for (UInt32 j = 0 ; j < _sampleSize ; ++j)
			for (UInt32 i = 0 ; i < _sampleSize ; ++i)
			{
				UInt32 color[4] = { 0, 0, 0, 0 };

				for (UInt32 y = blockSize ; y > 0 ; --y)
					for (UInt32 x = blockSize; x > 0 ; --x, ++ptr)
					{
						color[0] += ptr->red;
						color[1] += ptr->green;
						color[2] += ptr->blue;
						color[3] += ptr->alpha;
					}

				color[0] /= blockPixel;
				color[1] /= blockPixel;
				color[2] /= blockPixel;
				color[3] /= blockPixel;

				PCLODRgba medColor = { UInt8(color[0]), UInt8(color[1]), UInt8(color[2]), UInt8(color[3]) };
				pNewSample[i + j * _sampleSize] = medColor;
			}

		m_sample[_samplePrecision] = pNewSample;
	}
}
