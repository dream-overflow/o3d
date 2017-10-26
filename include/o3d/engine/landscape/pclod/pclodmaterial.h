/**
 * @file pclodmaterial.h
 * @brief PCLOD material.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2007-06-26
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PCLODMATERIAL_H
#define _O3D_PCLODMATERIAL_H

#include "o3d/core/evt.h"
#include "o3d/engine/texture/texture2d.h"
#include <map>

namespace o3d {

class PCLODTextureEvent;
class String;
class PCLODTerrain;

//---------------------------------------------------------------------------------------
//! @class PCLODMaterial
//-------------------------------------------------------------------------------------
//! PCLOD material.
//---------------------------------------------------------------------------------------
class O3D_API PCLODMaterial : public EvtHandler
{
public:

	/* Type */
	struct PCLODRgba
	{
		UInt8 red;
		UInt8 green;
		UInt8 blue;
		UInt8 alpha;
	};

	typedef std::map<UInt32, PCLODRgba *>	T_MatSampleMap;
	typedef T_MatSampleMap::iterator		IT_MatSampleMap;
	typedef T_MatSampleMap::const_iterator	CIT_MatSampleMap;

public:

	/* Constructors */
	PCLODMaterial(BaseObject *pParent, UInt32 _matId, const String _path, UInt32 _position = 0);
	PCLODMaterial(const PCLODMaterial &);
	virtual ~PCLODMaterial();

	PCLODMaterial & operator = (const PCLODMaterial &);

	/* Functions */

	PCLODTerrain* getTerrain() const;
	Texture2D * getTexture() const { return m_pTexture; }

	/* IO Functions */

	//! @brief (RT) Load the material texture
	//! This function loads the texture without sending it to opengl.
	//! In the same time, samples are generated (precision = 2)
	void rtLoad();

	//! @brief (RT) Release the used texture and some memory allocations
	//! This function is called by the manager after it was detected that this
	//! material was not used anymore.
	void rtUnload();

	Bool loaded() const { return (m_pTexture != NULL); }
	UInt32 getMatId() const { return m_matId; }


	//! Returne le sample avec la précision demandée. Le génère s'il n'existe pas a cette precision.
	//! - L'argument est la precision du sample. Doit etre une puissance de 2
	PCLODRgba * getSample(UInt32);

	//! Load the texture to openGL
	void loadToGL() const;

	/* Functions which provide a references counter */
	void useIt() const;
	void releaseIt() const;

	Bool noLongerUsed() const { return (m_refCounter == 0); }
	UInt32 getReferenceCounter() const { return m_refCounter; }

public:

    Signal<> onTextureUnused{this};

private:

	/* Members */
	BaseObject *m_pParent;
	UInt32 m_matId;

	String m_filePath;
	UInt32 m_filePosition;

	Texture2D * m_pTexture;

	UInt32 m_refCounter;

	mutable FastMutex m_mutex;

	T_MatSampleMap m_sample;

	//! Cette fonction créée l'échantillon à partir de la texture haute qualité.
	//! - La texture doit être chargée de taille 2^k
	void generateSample(UInt32);
};

} // namespace o3d

#endif // _O3D_PCLODMATERIAL_H

