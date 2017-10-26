/**
 * @file pclodrenderer.cpp
 * @brief Implementation of PCLOD terrain Renderer.h
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2006-08-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/landscape/pclod/pclodrenderer.h"

#include "o3d/engine/landscape/pclod/rendermanager.h"
#include "o3d/engine/landscape/pclod/terrain.h"

using namespace o3d;

//---------------------------------------------------------------------------------------
// O3DPCLODRendererBase
//---------------------------------------------------------------------------------------

UInt32 PCLODRendererBase::m_instanceCounter = 0;

PCLODRendererBase::PCLODRendererBase():
	m_refCounter(0),
	m_refMutex(),
	m_isInit(False),
	m_pRenderManager(NULL)
{
	m_instanceCounter++;
}

PCLODRendererBase::~PCLODRendererBase()
{
	m_instanceCounter--;
}

void PCLODRendererBase::init()
{
	m_isInit = True;
}

Bool PCLODRendererBase::isVisible()
{
	return True;
}

/* Used with Front to back rendering */
Vector3 PCLODRendererBase::getPosition()
{
	return Vector3(0.0f, 0.0f, 0.0f);
}

const PCLODTerrain* PCLODRendererBase::getTerrain() const
{
	return m_pRenderManager->getTerrain();
}

PCLODTerrain* PCLODRendererBase::getTerrain()
{
	return m_pRenderManager->getTerrain();
}

Scene* PCLODRendererBase::getScene()
{
	return getTerrain()->getScene();
}

const Scene* PCLODRendererBase::getScene() const
{
	return getTerrain()->getScene();
}

/* Functions which provide a references counter */
void PCLODRendererBase::useIt()
{
	FastMutexLocker locker(m_refMutex);
	m_refCounter++;
}

void PCLODRendererBase::releaseIt()
{
	FastMutexLocker locker(m_refMutex);
	O3D_ASSERT(m_refCounter > 0);
	m_refCounter--;
}

Bool PCLODRendererBase::noLongerUsed() const
{
	FastMutexLocker locker(m_refMutex);
	return (m_refCounter == 0);
}

UInt32 PCLODRendererBase::getReferenceCounter() const
{
	FastMutexLocker locker(m_refMutex);
	return m_refCounter;
}

