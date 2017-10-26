/**
 * @file scatteringmodelbase.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/sky/scatteringmodelbase.h"
#include "o3d/engine/enginetype.h"

#include "o3d/core/smartarray.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(ScatteringModelBase, ENGINE_SCATTERING_MODEL_BASE, BaseObject)

//! Default constructor
ScatteringModelBase::ScatteringModelBase(BaseObject * _pParent):
	BaseObject(_pParent),
	m_mustQuit(False),
	m_thread(this)
{
}

//! The destructor
ScatteringModelBase::~ScatteringModelBase()
{
	if (isStarted())
	{
		stop();
		wait();
	}
}

//! @brief Start the task
ScatteringModelBase::TaskResult ScatteringModelBase::start(TaskData & _datas, Bool _asynch)
{
	// Sera tjs vrai si on appelle deux fois la fonction Start puisque le thread precedent n'aura
	// toujours pas ete nettoye avec un CloseHandle
	if (isStarted())
	{
		stop();
		wait();
	}

	m_mustQuit = False;

	if (_asynch)
	{
		_datas.result = RESULT_STARTED;

		m_thread.start((void*)&_datas);

		O3D_ASSERT(m_thread.isThread());

		return RESULT_STARTED;
	}
	else
		return TaskResult(run((void*)&_datas));
}

//! @brief Stop the task
void ScatteringModelBase::stop()
{
	if (isStarted())
		m_mustQuit = True;
}

//! @brief Waits for the task to end
ScatteringModelBase::TaskResult ScatteringModelBase::wait()
{
	if (isStarted())
		return TaskResult(m_thread.waitFinish());
	else
		return RESULT_UNDEFINED;
}

//! @brief Returns wether or not the task is started
Bool ScatteringModelBase::isStarted() const
{
	return (m_thread.isThread());
}

//! @brief Thread method
Int32 ScatteringModelBase::run(void* _arg)
{
	TaskData * lpDatas = (TaskData*)_arg;
	lpDatas->result = entry(*lpDatas);

    onFinish(*lpDatas);

	return Int32(lpDatas->result);
}

//! @brief Called periodically
//! @return TRUE means that must quit as soon as possible
Bool ScatteringModelBase::mustQuit()
{
	return m_mustQuit;
}

