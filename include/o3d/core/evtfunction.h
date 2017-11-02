/**
 * @file evtfunction.h
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2008-03-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_EVTFUNCTION_H
#define _O3D_EVTFUNCTION_H

#include "evthandler.h"
#include "evtmanager.h"

#include <functional>
#include <tuple>

namespace o3d {

class EvtLinkBase;
class Thread;

/**
 * @brief Event function. Base template class.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-03-06
 */
class O3D_API EvtFunctionBase
{
protected:

	/* Members */
    EvtLinkBase * m_link;

    EvtHandler * m_sender;
    EvtHandler * m_receiver;

    //void * m_baseThis;

    Thread * m_thread;

public:

	/* Constructors */
    EvtFunctionBase();
	EvtFunctionBase(const EvtFunctionBase &);

    virtual ~EvtFunctionBase();

    void setLink(EvtLinkBase * _pLink) { m_link = _pLink; }
    EvtLinkBase * getLink() const { return m_link; }

    void setThread(Thread * _pThread) { m_thread = _pThread; }
    Thread * getThread() const { return m_thread; }

    void setSender(EvtHandler * _pSender) { m_sender = _pSender; }
    EvtHandler * getSender() const { return m_sender; }

    void setReceiver(EvtHandler * _pReceiver) { m_receiver = _pReceiver; }
    EvtHandler * getReceiver() const { return m_receiver; }

    //void setBaseThis(void * _baseThis) { m_baseThis = _baseThis; }
    //void * getBaseThis() const { return m_baseThis; }

    Bool isSync() const;

protected:

    inline void setReceiverSender(EvtHandler * /*_pHandler*/) { m_receiver->setSender(m_sender); }
    inline void resetReceiverSender() { m_receiver->resetSender(); }
};


/**
 * @brief Event function for asynchronous mode. Base class.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2016-01-03
 */
class O3D_API EvtFunctionAsyncBase
{
protected:

    EvtHandler *m_sender;
    EvtHandler *m_receiver;

    //void *m_baseThis;

    Thread *m_thread;

    /* Restricted */
    EvtFunctionAsyncBase(const EvtFunctionAsyncBase &);

public:

    EvtFunctionAsyncBase();
    EvtFunctionAsyncBase(const EvtFunctionBase &dup) :
        m_sender(dup.getSender()),
        m_receiver(dup.getReceiver()),
        //m_baseThis(dup.getBaseThis()),
        m_thread(dup.getThread())
    {
    }

    virtual ~EvtFunctionAsyncBase();

    void setThread(Thread * _pThread) { m_thread = _pThread; }
    Thread * getThread() const { return m_thread; }

    void setSender(EvtHandler * _pSender) { m_sender = _pSender; }
    EvtHandler * getSender() const { return m_sender; }

    void setReceiver(EvtHandler * _pReceiver) { m_receiver = _pReceiver; }
    EvtHandler * getReceiver() const { return m_receiver; }

    //void setBaseThis(void * _baseThis) { m_baseThis = _baseThis; }
    //void * getBaseThis() const { return m_baseThis; }

    virtual void process() = 0;

protected:

    inline void setReceiverSender(EvtHandler * /*_pHandler*/) { m_receiver->setSender(m_sender); }
    inline void resetReceiverSender() { m_receiver->resetSender(); }
};

/**
 * @brief Event function without parameters.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-03-06
 */
class O3D_API EvtFunction0Param : public EvtFunctionBase
{
public:

	typedef void (EvtHandler::*T_FunctionParamPtr)(void);

private:

    T_FunctionParamPtr m_funcPtr;

	/* Restricted */
	EvtFunction0Param(const EvtFunction0Param & _which);

public:

	EvtFunction0Param(T_FunctionParamPtr _func);

	void call();
};

/**
 * @brief Event function without parameters.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-03-06
 */
class O3D_API EvtFunctionAsync0Param : public EvtFunctionAsyncBase
{
public:

    typedef void (EvtHandler::*T_FunctionParamPtr)(void);

private:

    T_FunctionParamPtr m_funcPtr;

    /* Restricted */
    EvtFunctionAsync0Param(const EvtFunctionAsync0Param & _which);

public:

    EvtFunctionAsync0Param(
            const EvtFunctionBase & _which,
            T_FunctionParamPtr _func):
        EvtFunctionAsyncBase(_which),
        m_funcPtr(_func)
    {
    }

    virtual ~EvtFunctionAsync0Param()
    {
    }

    virtual void process();
};


template<std::size_t ...T>
struct seq { };

template<std::size_t N, std::size_t ...S>
struct gens : gens<N-1, N-1, S...> { };

template<std::size_t ...S>
struct gens<0, S...>
{
    typedef seq<S...> type;
};

/**
 * @brief Asynchrounous event function with variadics parameters.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2016-01-03
 */
template <typename... Args>
class O3D_API_TEMPLATE EvtStdFunctionAsyncParam : public EvtFunctionAsyncBase
{
public:

    typedef std::function<void(Args...)> T_StdFunction;
    typedef typename std::tuple<Args...> T_StdArguments;

private:

    T_StdFunction m_func;
    T_StdArguments m_values;

    /* Restricted */
    EvtStdFunctionAsyncParam(const EvtStdFunctionAsyncParam & _which);

public:

    EvtStdFunctionAsyncParam(
            const EvtFunctionBase & _which,
            T_StdFunction _func,
            Args... values):
        EvtFunctionAsyncBase(_which),
        m_func(_func),
        m_values(values...)
    {
    }

    virtual ~EvtStdFunctionAsyncParam()
    {
    }

    template<std::size_t ...S>
    inline void callFunc(seq<S...>)
    {
        m_func(std::get<S>(m_values) ...);
    }

    virtual void process()
    {
        O3D_ASSERT(m_receiver != nullptr);

        setReceiverSender(m_sender);
        //callFunc(std::index_sequence_for<Args...>{});
        callFunc(typename gens<sizeof...(Args)>::type());
        resetReceiverSender();
    }
};


/**
 * @brief Event function without parameters.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2016-01-03
 */
template <typename... Args>
class O3D_API_TEMPLATE EvtStdFunction: public EvtFunctionBase
{
public:

    typedef std::function<void(Args...)> T_StdFunction;

private:

    T_StdFunction m_func;

    /* Restricted */
    EvtStdFunction(const EvtStdFunction & _which);

public:

    EvtStdFunction(T_StdFunction _func):
        EvtFunctionBase(),
        m_func(_func)
    {
    }

    void call(Args... values)
    {
        O3D_ASSERT(m_link != nullptr);

        if (isSync())
        {
            setReceiverSender(m_sender);
            m_func(values...);
            resetReceiverSender();
        }
        else
        {
            // keep a standalone copy of the event functor with copy of each parameters
            EvtStdFunctionAsyncParam<Args...> *event =
                    new EvtStdFunctionAsyncParam<Args...>(*this, m_func, values...);

            EvtManager::instance()->postEvent(event);
        }
    }
};


/**
 * @brief Asynchrounous event std::function without parameters.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2016-03-01
 */
class O3D_API EvtStdFunctionAsync0Param: public EvtFunctionAsyncBase
{
public:

    typedef std::function<void()> T_StdFunction;

private:

    T_StdFunction m_func;

    /* Restricted */
    EvtStdFunctionAsync0Param(const EvtStdFunctionAsync0Param & _which);

public:

    EvtStdFunctionAsync0Param(
            const EvtFunctionBase & _which,
            T_StdFunction _func) :
        EvtFunctionAsyncBase(_which),
        m_func(_func)
    {
    }

    virtual void process();
};


/**
 * @brief Event std::function without parameters.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2016-03-01
 */
class O3D_API EvtStdFunction0Param : public EvtFunctionBase
{
public:

    typedef std::function<void()> T_StdFunction;

private:

    T_StdFunction m_func;

    /* Restricted */
    EvtStdFunction0Param(const EvtStdFunction0Param & _which);

public:

    /* Constructors */
    EvtStdFunction0Param(T_StdFunction _func);

    virtual ~EvtStdFunction0Param();

    void call();
};


///**
// * @brief Asynchrounous event function with a single parameters.
// * @author Frederic SCHERMA (frederic.scherma@gmail.com)
// * @date 2016-01-03
// */
//template <class T>
//class O3D_API_TEMPLATE EvtFunctionAsync1Param : public EvtFunctionAsyncBase
//{
//public:

//    typedef void (EvtHandler::*T_FunctionParamPtr)(T const &);

//private:

//    T_FunctionParamPtr m_funcPtr;

//    T* m_value0;			//! Used for posted events

//    /* Restricted */
//    EvtFunctionAsync1Param(const EvtFunctionAsync1Param & _which);

//public:

//    EvtFunctionAsync1Param(
//            const EvtFunctionBase & _which,
//            T_FunctionParamPtr _func,
//            T const *value0):
//        EvtFunctionAsyncBase(_which),
//        m_funcPtr(_func),
//        m_value0(nullptr)
//    {
//        if (value0 != nullptr)
//            m_value0 = new T(*value0);
//    }

//    virtual ~EvtFunctionAsync1Param()
//    {
//        deletePtr(m_value0);
//    }

//    virtual void process()
//    {
//        O3D_ASSERT(m_value0 != nullptr);
//        O3D_ASSERT(m_receiver != nullptr);

//        setReceiverSender(m_sender);
//        (static_cast<EvtHandler*>(getReceiver())->*(m_funcPtr))(*m_value0);
//        //(((EvtHandler*)getBaseThis())->*(m_funcPtr))(*m_value0);
//        resetReceiverSender();

//        deletePtr(m_value0);
//    }
//};


///**
// * @brief Event function with a single parameters.
// * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
// * @date 2008-03-06
// */
//template <class T>
//class O3D_API_TEMPLATE EvtFunction1Param : public EvtFunctionBase
//{
//public:

//	typedef void (EvtHandler::*T_FunctionParamPtr)(T const &);

//private:

//    T_FunctionParamPtr m_funcPtr;

//	/* Restricted */
//    EvtFunction1Param(const EvtFunction1Param & _which);

//public:

//	/* Constructors */
//	EvtFunction1Param(T_FunctionParamPtr _func):
//        EvtFunctionBase(),
//        m_funcPtr(_func)
//	{
//	}

//	void call(T const & _value)
//	{
//        O3D_ASSERT(m_link != nullptr);

//        if (isSync())
//		{
//            setReceiverSender(m_sender);
//            (static_cast<EvtHandler*>(getReceiver())->*(m_funcPtr))(_value);
//            //(((EvtHandler*)getBaseThis())->*(m_funcPtr))(_value);
//			resetReceiverSender();
//		}
//		else
//		{
//            // keep a standalone copy of the event functor with copy of each parameters
//            EvtFunctionAsync1Param<T> *event =
//                    new EvtFunctionAsync1Param<T>(*this, m_funcPtr, &_value);

//            EvtManager::instance()->postEvent(event);
//		}
//	}
//};


///**
// * @brief Asynchronous event function with two parameters.
// * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
// * @date 2008-03-06
// */
//template <class T0, class T1>
//class O3D_API_TEMPLATE EvtFunctionAsync2Param : public EvtFunctionAsyncBase
//{
//public:

//    typedef void (EvtHandler::*T_FunctionParamPtr)(T0 const &, T1 const &);

//private:

//    T_FunctionParamPtr m_funcPtr;

//    T0 * m_value0;			//! Used for posted events
//    T1 * m_value1;

//    /* Restricted */
//    EvtFunctionAsync2Param(const EvtFunctionAsync2Param & _which);

//public:

//    EvtFunctionAsync2Param(
//            const EvtFunctionBase & _which,
//            T_FunctionParamPtr _func,
//            T0 const *value0,
//            T1 const *value1) :
//        EvtFunctionAsyncBase(_which),
//        m_funcPtr(_func),
//        m_value0(nullptr),
//        m_value1(nullptr)
//    {
//        if (value0 != nullptr)
//            m_value0 = new T0(*value0);
//        if (value1 != nullptr)
//            m_value1 = new T1(*value1);
//    }

//    virtual ~EvtFunctionAsync2Param()
//    {
//        deletePtr(m_value0);
//        deletePtr(m_value1);
//    }

//    virtual void process()
//    {
//        O3D_ASSERT(m_value0 != nullptr);
//        O3D_ASSERT(m_value1 != nullptr);
//        O3D_ASSERT(m_receiver != nullptr);

//        setReceiverSender(m_sender);
//        (static_cast<EvtHandler*>(getReceiver())->*(m_funcPtr))(*m_value0, *m_value1);
//        //(((EvtHandler*)getBaseThis())->*(m_funcPtr))(*m_value0, *m_value1);
//        resetReceiverSender();

//        deletePtr(m_value0);
//        deletePtr(m_value1);
//    }
//};


///**
// * @brief Event function with two parameters.
// * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
// * @date 2008-03-06
// */
//template <class T0, class T1>
//class O3D_API_TEMPLATE EvtFunction2Param : public EvtFunctionBase
//{
//public:

//	typedef void (EvtHandler::*T_FunctionParamPtr)(T0 const &, T1 const &);

//private:

//    T_FunctionParamPtr m_funcPtr;

//	/* Restricted */
//    EvtFunction2Param(const EvtFunction2Param & _which);

//public:

//	/* Constructors */
//	EvtFunction2Param(T_FunctionParamPtr _func):
//        EvtFunctionBase(),
//        m_funcPtr(_func)
//	{
//	}

//	void call(T0 const & _value0, T1 const & _value1)
//	{
//        O3D_ASSERT(m_link != nullptr);

//        if (isSync())
//		{
//            setReceiverSender(m_sender);
//            (static_cast<EvtHandler*>(getReceiver())->*(m_funcPtr))(_value0, _value1);
//            //(((EvtHandler*)getBaseThis())->*(m_funcPtr))(_value0, _value1);
//			resetReceiverSender();
//		}
//		else
//		{
//            EvtFunctionAsync2Param<T0, T1> *event =
//                    new EvtFunctionAsync2Param<T0, T1>(*this, m_funcPtr, &_value0, &_value1);

//            EvtManager::instance()->postEvent(event);
//		}
//	}
//};


///**
// * @brief Asynchronous event function with three parameters.
// * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
// * @date 2008-03-06
// */
//template <class T0, class T1, class T2>
//class O3D_API_TEMPLATE EvtFunctionAsync3Param : public EvtFunctionAsyncBase
//{
//public:

//    typedef void (EvtHandler::*T_FunctionParamPtr)(T0 const &, T1 const &, T2 const &);

//private:

//    T_FunctionParamPtr m_funcPtr;

//    T0 * m_value0;			//! Used for posted events
//    T1 * m_value1;
//    T2 * m_value2;

//    /* Restricted */
//    EvtFunctionAsync3Param(const EvtFunctionAsync3Param & _which);

//public:

//    EvtFunctionAsync3Param(
//            const EvtFunctionBase & _which,
//            T_FunctionParamPtr _func,
//            T0 const *value0,
//            T1 const *value1,
//            T2 const *value2) :
//        EvtFunctionAsyncBase(_which),
//        m_funcPtr(_func),
//        m_value0(nullptr),
//        m_value1(nullptr),
//        m_value2(nullptr)
//    {
//        if (value0 != nullptr)
//            m_value0 = new T0(*value0);
//        if (value1 != nullptr)
//            m_value1 = new T1(*value1);
//        if (value2 != nullptr)
//            m_value2 = new T2(*value2);
//    }

//    virtual ~EvtFunctionAsync3Param()
//    {
//        deletePtr(m_value0);
//        deletePtr(m_value1);
//        deletePtr(m_value2);
//    }

//    virtual void process()
//    {
//        O3D_ASSERT(m_value0 != nullptr);
//        O3D_ASSERT(m_value1 != nullptr);
//        O3D_ASSERT(m_value2 != nullptr);
//        O3D_ASSERT(m_receiver != nullptr);

//        setReceiverSender(m_sender);
//        (static_cast<EvtHandler*>(getReceiver())->*(m_funcPtr))(*m_value0, *m_value1, *m_value2);
//        //(((EvtHandler*)getBaseThis())->*(m_funcPtr))(*m_pValue0, *m_pValue1, *m_pValue2);
//        resetReceiverSender();

//        deletePtr(m_value0);
//        deletePtr(m_value1);
//        deletePtr(m_value2);
//    }
//};


///**
// * @brief Event function with three parameters.
// * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
// * @date 2008-03-06
// */
//template <class T0, class T1, class T2>
//class O3D_API_TEMPLATE EvtFunction3Param : public EvtFunctionBase
//{
//public:

//	typedef void (EvtHandler::*T_FunctionParamPtr)(T0 const &, T1 const &, T2 const &);

//private:

//    T_FunctionParamPtr m_funcPtr;

//	/* Restricted */
//    EvtFunction3Param(const EvtFunction3Param & _which);

//public:

//	EvtFunction3Param(T_FunctionParamPtr _func):
//        EvtFunctionBase(),
//        m_funcPtr(_func)
//	{
//	}

//	void call(T0 const & _value0, T1 const & _value1, T2 const & _value2)
//	{
//        O3D_ASSERT(m_link != nullptr);

//        if (isSync())
//		{
//            setReceiverSender(m_sender);
//            (static_cast<EvtHandler*>(getReceiver())->*(m_funcPtr))(_value0, _value1, _value2);
//            //(((EvtHandler*)getBaseThis())->*(m_funcPtr))(_value0, _value1, _value2);
//            resetReceiverSender();
//		}
//		else
//		{
//            EvtFunctionAsync3Param<T0, T1, T2> *event =
//                    new EvtFunctionAsync3Param<T0, T1, T2>(*this, m_funcPtr, &_value0, &_value1, &_value2);

//            EvtManager::instance()->postEvent(event);
//		}
//	}
//};

///**
// * @brief Asynchronous event function with four parameters.
// * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
// * @date 2008-03-06
// */
//template <class T0, class T1, class T2, class T3>
//class O3D_API_TEMPLATE EvtFunctionAsync4Param : public EvtFunctionAsyncBase
//{
//public:

//	typedef void (EvtHandler::*T_FunctionParamPtr)(T0 const &, T1 const &, T2 const &, T3 const &);

//private:

//    T_FunctionParamPtr m_funcPtr;

//    T0 * m_value0;			//! Used for posted events
//    T1 * m_value1;
//    T2 * m_value2;
//    T3 * m_value3;

//	/* Restricted */
//    EvtFunctionAsync4Param(const EvtFunctionAsync4Param & _which);

//public:

//    EvtFunctionAsync4Param(
//            const EvtFunctionBase & _which,
//            T_FunctionParamPtr _func,
//            T0 const *value0,
//            T1 const *value1,
//            T2 const *value2,
//            T3 const *value3) :
//        EvtFunctionAsyncBase(_which),
//        m_funcPtr(_func),
//        m_value0(nullptr),
//        m_value1(nullptr),
//        m_value2(nullptr),
//        m_value3(nullptr)
//    {
//        if (value0 != nullptr)
//            m_value0 = new T0(*value0);
//        if (value1 != nullptr)
//            m_value1 = new T1(*value1);
//        if (value2 != nullptr)
//            m_value2 = new T2(*value2);
//        if (value3 != nullptr)
//            m_value3 = new T3(*value3);
//    }

//    virtual ~EvtFunctionAsync4Param()
//	{
//        deletePtr(m_value0);
//        deletePtr(m_value1);
//        deletePtr(m_value2);
//        deletePtr(m_value3);
//	}

//	virtual void process()
//	{
//        O3D_ASSERT(m_value0 != nullptr);
//        O3D_ASSERT(m_value1 != nullptr);
//        O3D_ASSERT(m_value2 != nullptr);
//        O3D_ASSERT(m_value3 != nullptr);
//        O3D_ASSERT(m_receiver != nullptr);

//        setReceiverSender(m_sender);
//        (static_cast<EvtHandler*>(getReceiver())->*(m_funcPtr))(*m_value0, *m_value1, *m_value2, *m_value3);
//        //(((EvtHandler*)getBaseThis())->*(m_funcPtr))(*m_pValue0, *m_pValue1, *m_pValue2, *m_pValue3);
//		resetReceiverSender();

//        deletePtr(m_value0);
//        deletePtr(m_value1);
//        deletePtr(m_value2);
//        deletePtr(m_value3);
//	}
//};


///**
// * @brief Event function with four parameters.
// * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
// * @date 2008-03-06
// */
//template <class T0, class T1, class T2, class T3>
//class O3D_API_TEMPLATE EvtFunction4Param : public EvtFunctionBase
//{
//public:

//    typedef void (EvtHandler::*T_FunctionParamPtr)(T0 const &, T1 const &, T2 const &, T3 const &);

//private:

//    T_FunctionParamPtr m_funcPtr;

//    /* Restricted */
//    EvtFunction4Param(const EvtFunction4Param & _which);

//public:

//    EvtFunction4Param(T_FunctionParamPtr _func):
//        EvtFunctionBase(),
//        m_funcPtr(_func)
//    {
//    }

//    void call(T0 const & _value0, T1 const & _value1, T2 const & _value2, T3 const & _value3)
//    {
//        O3D_ASSERT(m_link != nullptr);

//        if (isSync())
//        {
//            setReceiverSender(m_sender);
//            (static_cast<EvtHandler*>(getReceiver())->*(m_funcPtr))(_value0, _value1, _value2, _value3);
//            //(((EvtHandler*)getBaseThis())->*(m_funcPtr))(_value0, _value1, _value2, _value3);
//            resetReceiverSender();
//        }
//        else
//        {
//            EvtFunctionAsync4Param<T0, T1, T2, T3> *event =
//                    new EvtFunctionAsync4Param<T0, T1, T2, T3>(*this, m_funcPtr, &_value0, &_value1, &_value2, &_value3);

//            EvtManager::instance()->postEvent(event);
//        }
//    }
//};

} // namespace o3d

#endif // _O3D_EVTFUNCTION_H
