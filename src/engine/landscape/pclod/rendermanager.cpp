/**
 * @file rendermanager.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2006-08-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/landscape/pclod/rendermanager.h"

#include "o3d/core/smartarray.h"
#include "o3d/geom/aabbox.h"

#include "o3d/engine/landscape/pclod/object.h"
#include "o3d/engine/landscape/pclod/terrain.h"
#include "o3d/engine/landscape/pclod/configs.h"
#include "o3d/engine/landscape/pclod/zonemanager.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/context.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/geom/frustum.h"

using namespace o3d;

//#include <utility>

O3D_IMPLEMENT_DYNAMIC_CLASS1(PCLODRenderManager, ENGINE_PCLOD_RENDER_MANAGER, SceneEntity)

/*================================================================
					class O3DPCLODRenderManager
================================================================*/
PCLODRenderManager::PCLODRenderManager(BaseObject *pParent) :
	SceneEntity(pParent),
	m_rendererTable(),
	m_rendererOrder(),
	m_refreshTimer(0)
{
}

PCLODRenderManager::~PCLODRenderManager()
{
	destroy();

	if (PCLODRendererBase::getRendererCount() > 0)
		PCLOD_WARNING(String("RenderManager : Cleaned but ") << PCLODRendererBase::getRendererCount() << " renderers still in memory");
	else
		PCLOD_MESSAGE(String("RenderManager : All renderer was successful released"));
}

const PCLODTerrain* PCLODRenderManager::getTerrain() const
{
	return ((PCLODZoneManager*)m_parent)->getTerrain();
}

PCLODTerrain* PCLODRenderManager::getTerrain()
{
	return ((PCLODZoneManager*)m_parent)->getTerrain();
}

void PCLODRenderManager::destroy()
{
	for (IT_RendererMap it = m_rendererTable.begin() ; it != m_rendererTable.end() ; it++)
	{
		it->second->pRenderer->clean();
		deletePtr(it->second);
	}

	m_rendererTable.clear();
	m_rendererOrder.clear();
}

/* Functions */
void PCLODRenderManager::draw(const DrawInfo &drawInfo)
{
	Float color = 0.2f;
	Float pas = 0.8f/Float(m_rendererOrder.size());

    for (IT_RendererArray it = m_rendererOrder.begin() ; it != m_rendererOrder.end() ; it++, color += pas) {
		//getScene()->getContext()->setColor(color, color, color); TODO quelle utilitee ?

        if (!(*it)->pRenderer->isInit()) {
            if ((*it)->pRenderer->isReady()) {
				(*it)->pRenderer->init();
            } else {
				continue;
            }
		}

        (*it)->pRenderer->draw(drawInfo);
	}
}

void PCLODRenderManager::update()
{
	// Mise à jour du front to back
    if (getTerrain()->getCurrentConfigs().frontToBack()) {
		Vector3 viewPosition(getTerrain()->getCamera()->getAbsoluteMatrix().getTranslation());

        if ((viewPosition - m_lastViewPosition).normInf() > getTerrain()->getCurrentConfigs().frontToBackMinViewMove()) {
            for (IT_RendererArray it = m_rendererOrder.begin() ; it != m_rendererOrder.end() ; it++) {
				#ifdef _DEBUG
				(*it)->rendererDistance = UInt32(((*it)->pRenderer->getPosition()-viewPosition).length());
				#else
				(*it)->rendererDistance = UInt32(((*it)->pRenderer->getPosition()-viewPosition).normInf());
				#endif
			}

			m_lastViewPosition = viewPosition;
		}

        if (m_refreshTimer == 0) {
            if (m_rendererOrder.size() > 1) {
				PCLODRendererInfo ** pInfo1 = &m_rendererOrder[0];
				PCLODRendererInfo ** pInfo2 = &m_rendererOrder[1];
                PCLODRendererInfo * pPtr = nullptr;

                while (pInfo1 != m_rendererOrder.rbegin().operator ->()) {
                    if ((*pInfo1)->rendererDistance > (*pInfo2)->rendererDistance) {
						pPtr = *pInfo1;
						*pInfo1 = *pInfo2;
						*pInfo2 = pPtr;
						--(*pInfo1)->rendererOrder;
						++(*pInfo2)->rendererOrder;
					}

					++pInfo1;
					++pInfo2;
				}
			}

			m_refreshTimer = getTerrain()->getCurrentConfigs().frontToBackRefreshPeriodicity();
        } else {
			--m_refreshTimer;
        }
	}
}

/* Add or remove an object from the manager */
void PCLODRenderManager::addObject(PCLODRendererBase * _pRenderer)
{
    O3D_ASSERT(_pRenderer != nullptr);
	O3D_ASSERT(!findObject(_pRenderer));

	PCLODRendererInfo * pNewRendererInfo = new PCLODRendererInfo();
	pNewRendererInfo->pRenderer = SPCLODRendererBase(_pRenderer);
	pNewRendererInfo->pRenderer->setRenderManager(this);
	pNewRendererInfo->rendererOrder = UInt32(m_rendererOrder.size());
	pNewRendererInfo->rendererDistance = UInt32((m_lastViewPosition - pNewRendererInfo->pRenderer->getPosition()).length());

	std::pair<IT_RendererMap, Bool> ret = m_rendererTable.insert(std::pair<PCLODRendererBase*, PCLODRendererInfo*>(_pRenderer, pNewRendererInfo));
	m_rendererOrder.push_back(ret.first->second);
}

void PCLODRenderManager::removeObject(PCLODRendererBase * _pRenderer)
{
    O3D_ASSERT(_pRenderer != nullptr);
	O3D_ASSERT(findObject(_pRenderer));

	_pRenderer->clean();

	IT_RendererMap it = m_rendererTable.find(_pRenderer);

	IT_RendererArray itOrder = m_rendererOrder.begin();
	itOrder += it->second->rendererOrder;

	for (IT_RendererArray itInfo = itOrder+1 ; itInfo != m_rendererOrder.end() ; itInfo++)
		--((*itInfo)->rendererOrder);

	m_rendererOrder.erase(itOrder);
	deletePtr(it->second);
	m_rendererTable.erase(it);
}

Bool PCLODRenderManager::findObject(PCLODRendererBase * _pRenderer)
{
    O3D_ASSERT(_pRenderer != nullptr);
	return (m_rendererTable.find(_pRenderer) != m_rendererTable.end());
}

