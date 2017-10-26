/**
 * @file colormap.h
 * @brief PCLOD color map.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2007-06-26
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PCLODCOLORMAP_H
#define _O3D_PCLODCOLORMAP_H

#include "o3d/core/base.h"
#include "o3d/core/thread.h"
#include "o3d/core/smartpointer.h"
#include "o3d/core/evt.h"
#include "o3d/core/vector2.h"

#include <map>

namespace o3d {

class PCLODMaterial;
class PCLODTopZone;
class String;
class BaseObject;
class PCLODTerrain;
class Texture2D;

//---------------------------------------------------------------------------------------
//! @class PCLODColormap
//-------------------------------------------------------------------------------------
//! PCLOD color map.
//---------------------------------------------------------------------------------------
class O3D_API PCLODColormap : public EvtHandler
{
public:

	/* Type */
	typedef std::map<UInt32, PCLODMaterial*>	O3D_T_MatTable;
	typedef O3D_T_MatTable::iterator				O3D_IT_MatTable;
	typedef O3D_T_MatTable::const_iterator			O3D_CIT_MatTable;

private:

	/* Members */
	BaseObject *m_pParent;
	UInt32 m_zoneId;

	String m_filePath;
	UInt32 m_filePosition;

	UInt32 m_refCounter;

	Texture2D * m_pTexture;

	PCLODTopZone * m_pTopZone;

	mutable FastMutex m_mutex;

	/* Configs */
	SmartArrayInt8 m_staticNoise;
	Vector2ui m_staticNoiseSize;

	/* Restricted */
	PCLODColormap(const PCLODColormap &);
	PCLODColormap & operator = (const PCLODColormap &);

public:

	/* Constructors */
	PCLODColormap(BaseObject *pParent, PCLODTopZone *);
	PCLODColormap(BaseObject *pParent, PCLODTopZone *, UInt32 _zoneId, const String _path, UInt32 _position = 0);
	virtual ~PCLODColormap();

	PCLODTerrain* getTerrain() const;

	/* Configs : These functions must be called just after the constructor. */
	void useStaticNoise(const SmartArrayInt8 &, UInt32, UInt32);

	/* Functions */

	/* IO Functions */

	/** Cette fonction charge la colormap a partir du disque dur.
	 * - Retourne False si le fichier n'existe pas ou n'est pas lisible
	 * - Si la fonction retourne False, la colormap a été initialisé avec une texture noir. **/
	Bool rtLoad();

	//! @brief Free all resources
	//! This functions is called in the texture manager in the event callback
	//! MTE_OnColormapDelection.
	void mtUnload();

	/** Génère la colormap à partir de la table des matériaux et de la précision demandée.
	 *  - Pour une heightmap de dimension a,b, la colormap aura pour dimension k*(a-1), k*(b-1)
	 *    avec k la précision demandée. Dans tous les cas, la taille maximale est 4096*4096 */
	void rtGenerate(O3D_T_MatTable &, UInt32);

	//! @brief Return the state of the colormap
    Bool loaded() const { return (m_pTexture != nullptr); }

	UInt32 getZoneId() const { return m_zoneId; }

	Texture2D * getTexture() const { return m_pTexture; }

	/* Load the texture to openGL */
	void loadToGL() const;

	/* Functions which provide a references counter */
	void useIt() const;
	void releaseIt() const;

	Bool noLongerUsed() const { return (m_refCounter == 0); }
	UInt32 getReferenceCounter() const { return m_refCounter; }

public:

    Signal<> onTextureUnused{this};
};

} // namespace o3d

#endif // _O3D_PCLODCOLORMAP_H

