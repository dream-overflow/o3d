/**
 * @file rendermanager.h
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2006-08-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PCLODRENDERMANAGER_H
#define _O3D_PCLODRENDERMANAGER_H

#include "o3d/core/smartpointer.h"
#include "o3d/core/vector3.h"
#include "o3d/engine/scene/sceneentity.h"

#include "o3d/engine/landscape/pclod/pclodrenderer.h"

#include <map>

namespace o3d {

/*================================================================
					class PCLODRenderManager
================================================================*/
class O3D_API PCLODRenderManager : public SceneEntity
{
private:

	/* Internal types */
	typedef SmartPtr<PCLODRendererBase> SPCLODRendererBase;

	struct PCLODRendererInfo
	{
		SPCLODRendererBase pRenderer;

		UInt32 rendererOrder;		//! Position of the renderer in the ordered array (0 if front to back rendering is not used)
		UInt32 rendererDistance;	//! Last computed distance of the object associated to the renderer.
	};

	typedef std::map<PCLODRendererBase*, PCLODRendererInfo*>	T_RendererMap;
	typedef T_RendererMap::iterator							IT_RendererMap;
	typedef T_RendererMap::const_iterator					CIT_RendererMap;

	typedef std::vector<PCLODRendererInfo*>					T_RendererArray;
	typedef T_RendererArray::iterator						IT_RendererArray;
	typedef T_RendererArray::const_iterator					CIT_RendererArray;

	/* Members */
	T_RendererMap m_rendererTable;

	/* Options */
	T_RendererArray m_rendererOrder;

	Vector3 m_lastViewPosition;		//! Position of the camera of the last refresh (front to back)
	UInt32 m_refreshTimer;			//! An update of the rendererOrder array occurs only when this value reaches 0

	/* Restricted */
	PCLODRenderManager(const PCLODRenderManager &);
	PCLODRenderManager & operator = (const PCLODRenderManager &);

public:

	O3D_DECLARE_DYNAMIC_CLASS(PCLODRenderManager)

	/* Constructors */
	PCLODRenderManager(BaseObject *pParent);
	~PCLODRenderManager();

	void destroy();

	/* Functions */

	const PCLODTerrain* getTerrain() const;
	PCLODTerrain* getTerrain();

	void draw();

	void update();

	/* Add or remove an object from the manager */
	void addObject(PCLODRendererBase *);
	void removeObject(PCLODRendererBase *);
	Bool findObject(PCLODRendererBase *);
};

} // namespace o3d

#endif // _O3D_PCLODRENDERMANAGER_H

