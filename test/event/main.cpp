//---------------------------------------------------------------------------------------
//! @file main.cpp
//! Main entry of the events handler test.
//! @date 2004-01-01
//! @author Frederic SCHERMA
//-------------------------------------------------------------------------------------
//- Objective-3D Copyright (C) 2001-2009 Revolutioning Entertainment
//- mailto:contact@revolutioning.com
//- http://o3d.revolutioning.com
//-------------------------------------------------------------------------------------
// This file is part of Objective-3D.
// Objective-3D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Objective-3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Objective-3D.  If not, see <http://www.gnu.org/licenses/>.
//---------------------------------------------------------------------------------------

//#include <o3d/core/EvtHandler.h>
//#include <o3d/core/Thread.h>

#include <iostream>
#include <o3d/core/Architecture.h>

/*O3DMutex lMutex;

O3DThread * pThread1 = NULL;
O3DThread * pThread2 = NULL;
O3DEvtManager * EvtManager = NULL;

class EventSender : public O3DEvtHandler
{
	O3D_UINT32 m_counter;
public:

	EventSender(): m_counter(0)
	{
		Connect(O3D_LINK_EVT(EventSender, this, MON_SIGNAL, this, EventSender::OnMonSignal), O3DEvtHandler::CONNECTION_SYNCH);
		Connect(O3D_LINK_EVT(EventSender, this, MON_SIGNAL, this, EventSender::OnMonSignal), O3DEvtHandler::CONNECTION_ASYNCH);
		Connect(O3D_LINK_EVT(EventSender, this, MON_SIGNAL, this, EventSender::OnMonSignal), pThread1);
		Connect(O3D_LINK_EVT(EventSender, this, MON_SIGNAL, this, EventSender::OnMonSignal), pThread2);
	}

	~EventSender()
	{
	}

	void OnMonSignal(const int & _value)
	{
		O3D_ASSERT(GetSender() == this);
		std::cout << O3DThreadManager::GetThreadId() << " : value = " << _value << std::endl;
	}

	void Send()
	{
		MON_SIGNAL(++m_counter);
	}

public signals:

	SIGNAL_1(MON_SIGNAL, int);
};

class Obj1 : public O3DEvtHandler
{
public:

	Obj1(EventSender * _pSender)
	{
//		CONNECT_ASYNCH(EventSender, _pSender, MON_SIGNAL, this, Obj1::OnSignal);
//		CONNECT(EventSender, _pSender, MON_SIGNAL, this, Obj1::OnSignal);
	}

	~Obj1() {}

public:

	void OnSignal(EvtType<int>::R _value)
	{
		lMutex.Lock();
		std::cout << "Signal recu < " << ThreadManager::GetThreadId() << "> : " << _value << std::endl;
		lMutex.Unlock();
	}
};

O3D_INT32 MyThread(void * _pData)
{
	O3DThread * lThread = (O3DThread*)_pData;

	O3DEvtPool lPool(lThread);

	for (O3D_UINT32 k = 0 ; k < 100; ++k)
	{
		O3DEvtManager::Instance()->ProcessEvent();
		Sleep(10);
	}

	return 0;
};*/

class ClassEvent
{
public:

	ClassEvent(): valueEvent(0.0) { strcpy(nameEvent, "Event"); }
	virtual ~ClassEvent() {}

	virtual void Fonction()
	{
		std::cout << "valeur : " << this->valueEvent << std::endl;
	}

private:

	double valueEvent;
	char nameEvent[10];
};

class ClassBase
{
public:

	ClassBase(): value(0.0) { strcpy(name, "Base"); }
	virtual ~ClassBase() {}

private:

	double value;
	char name[10];
};

class ClassDerivee : public ClassBase, public ClassEvent
{
public:

	ClassDerivee(): ClassBase(), valueDerivee(1.0) { strcpy(nameDerive, "Derivee"); }
	virtual ~ClassDerivee() {}

	void Event(double _arg)
	{
		std::cout << "Event " << _arg << " : valeur derivee : " << this->valueDerivee << std::endl;

		this->Fonction();
	}

private:

	double valueDerivee;
	char nameDerive[10];
};

typedef void (ClassEvent::*T_FunctionEvent)(void);
typedef void (ClassEvent::*T_FunctionInt)(double);
typedef void (ClassDerivee::*T_FunctionDerivee)(double);

int main(int argc, char * argv[])
{
	ClassDerivee * pClassDerivee = new ClassDerivee();
	ClassBase * pClassBase = (ClassBase*)pClassDerivee;
	ClassEvent * pClassEvent = (ClassEvent*)pClassDerivee;

	T_FunctionDerivee pFuncDerive = &(ClassDerivee::Event);
	T_FunctionEvent pFuncBase = (T_FunctionEvent)(T_FunctionInt)(&ClassDerivee::Event);

	int lOffset = int((char*)pClassEvent - (char*)pClassBase);

	std::cout << sizeof(T_FunctionEvent) << std::endl;
	std::cout << sizeof(T_FunctionInt) << std::endl;
	std::cout << sizeof(T_FunctionDerivee) << std::endl;

	ClassEvent * pBrutalCast = reinterpret_cast<ClassEvent*>(pClassBase);
	(pBrutalCast->*((T_FunctionInt)pFuncBase))(3.14);
	(pClassEvent->*((T_FunctionInt)pFuncBase))(3.14);
	(((ClassEvent*)((char*)pClassEvent - lOffset))->*((T_FunctionInt)pFuncBase))(3.14);

/*	EvtManager = O3DEvtManager::Instance();

	pThread1 = new O3DThread();
	pThread2 = new O3DThread();

	O3DEvtPool lPool(NULL);

	pThread1->Create(new O3DCallbackFunction(&MyThread), pThread1);
	pThread2->Create(new O3DCallbackFunction(&MyThread), pThread2);

	EventSender * lSender = new EventSender();
	lSender->Send();

	pThread1->WaitFinish();
	pThread2->WaitFinish();

	deletePtr(lSender);
	deletePtr(pThread1);
	deletePtr(pThread2);*/

	system("PAUSE");

	return 0;
}
