/**
 * @file evtcontainer.h
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2007-11-13
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_EVTCONTAINER_H
#define _O3D_EVTCONTAINER_H

#include "base.h"
#include "mutex.h"

#include <vector>

namespace o3d {

/**
 * @brief Event container that manages the event function for each signal in an
 * EventHandler object.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2007-11-13
 */
template <class T>
class O3D_API_TEMPLATE EvtContainer
{
public:

    typedef T *T_FunctionType;

    typedef std::vector<T_FunctionType>               T_FunctionArray;
	typedef typename T_FunctionArray::iterator        IT_FunctionArray;
	typedef typename T_FunctionArray::const_iterator CIT_FunctionArray;

private:

	/* Members */
	T_FunctionArray m_callBacks;

	mutable RecursiveMutex m_mutex;

public:

	/* Construtors */
	EvtContainer();
	~EvtContainer();

    void addHandler(T_FunctionType  _pFunc);
    void removeHandler(T_FunctionType _pFunc);

	/* Accessors */
	void getCallBacks(T_FunctionArray & _array) const;
};


template <class T>
EvtContainer<T>::EvtContainer():
	m_callBacks(),
	m_mutex()
{
}

template <class T>
EvtContainer<T>::~EvtContainer()
{
	RecurMutexLocker lLocker(m_mutex);

	T_FunctionArray lCpy(m_callBacks);

	for (IT_FunctionArray it = lCpy.begin() ; it != lCpy.end() ; it++)
		(*it)->getLink()->getReceiver()->disconnect((*it)->getLink());
}

template <class T>
void EvtContainer<T>::addHandler(T_FunctionType _pFunc)
{
	RecurMutexLocker lLocker(m_mutex);

	IT_FunctionArray it = m_callBacks.begin();

	while (it != m_callBacks.end())
		if ((*it) == _pFunc)
			break;
		else
			it++;

	O3D_ASSERT(it == m_callBacks.end());

	if (it == m_callBacks.end())
		m_callBacks.push_back(_pFunc);
}

template <class T>
void EvtContainer<T>::removeHandler(T_FunctionType _pFunc)
{
	RecurMutexLocker lLocker(m_mutex);

	for (IT_FunctionArray it = m_callBacks.begin() ; it != m_callBacks.end() ; it++)
		if ((*it) == _pFunc)
		{
			m_callBacks.erase(it);
			return;
		}

	O3D_ASSERT(0);
}

template <class T>
void EvtContainer<T>::getCallBacks(T_FunctionArray & _array) const
{
	RecurMutexLocker lLocker(m_mutex);
	_array = m_callBacks;
}

} // namespace o3d

#endif // _O3D_EVTCONTAINER_H

