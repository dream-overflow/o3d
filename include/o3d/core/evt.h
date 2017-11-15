/**
 * @file evt.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-01-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_EVT_H
#define _O3D_EVT_H

#include "evthandler.h"
#include "evtlink.h"
#include "evtfunction.h"

/*
//! Creation of a new connection.
#define O3D_LINK_EVT(CLASS_, SENDER_, SIGNAL_, RECEIVER_, SLOT_) \
    o3d::Link<decltype(CLASS_::SIGNAL_)>::make(SENDER_, SENDER_->SIGNAL_, RECEIVER_, &SLOT_)

//! Creation of a new connection to a signal.
#define O3D_LINK_EVT_SAFE(CLASS_, SENDER_, SIGNAL_, RECEIVER_, SLOT_) \
    o3d::Link<decltype(CLASS_::SIGNAL_)>::make(SENDER_, ((CLASS_*)SENDER_)->SIGNAL_, RECEIVER_, &SLOT_)
*/

namespace o3d {

///**
// * @brief Used on slot to define a parameter.
// * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
// * @date 2013-01-01
// * Usage :
// * void mySlot(EvtType<String>::R name);
// */
//template <typename T>
//struct EvtType {

//    //! Result type.
//    typedef T const & R;
//};

///**
// * @brief Helper used to connect a signal.
// * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
// * @date 2013-01-01
// * Usage :
// * o3d::Link<className::signalType>::make(
// *         sender,
// *         sender->signal,
// *         receiver,
// *         &receiver::slot)
// */
//template <class SGN>
//struct Link
//{
//    typedef typename SGN::SignalType SignalType;
//    typedef typename SGN::ParamType ParamType;
//    typedef typename ParamType::T_FunctionParamPtr FunctionParamPtrType;

//    template <class FUNC_PTR>
//    static EvtLink<ParamType>* make(
//            EvtHandler *sender,
//            SignalType &signal,
//            EvtHandler *receiver,
//            FUNC_PTR slot)
//    {
//        return new EvtLink<ParamType>(
//                    sender,
//                    receiver,
//                    //reinterpret_cast<void*>(receiver),
//                    &signal.mContainer,
//                    String(""),
//                    new ParamType((FunctionParamPtrType)slot));
//    }
//};

///**
// * Make a signal link, used to connect a signal to a slot between an emiter and a
// * receiver. The slot must be a function pointer to a class method.
// * @param sender Sender event handler object
// * @param signal Signal object on the sender
// * @param receiver Receiver event handler object
// * @param slot Pointer on a slot on the receiver (must be of the type of the signal)
// * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
// * @date 2013-01-01
// */
//template <class SIGNAL_TYPE, class FUNC_PTR>
//static EvtLinkBase* evtLink(
//        EvtHandler *sender,
//        SIGNAL_TYPE &signal,
//        EvtHandler *receiver,
//        FUNC_PTR slot)
//{
//    typedef typename SIGNAL_TYPE::ParamType ParamType;
//    typedef typename ParamType::T_FunctionParamPtr FunctionParamPtrType;

//    return new EvtLink<ParamType>(
//                sender,
//                receiver,
//                //reinterpret_cast<void*>(receiver),
//                &signal.mContainer,
//                String(""),
//                new ParamType((FunctionParamPtrType)slot));
//}

///**
// * @brief Base template struct for signals definition.
// * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
// * @date 2013-01-01
// */
//template <class P0 = NullType, class P1 = NullType, class P2 = NullType, class P3 = NullType>
//struct Signal1;

///**
// * @brief A signal with 0 parameters and void as return.
// * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
// * @date 2013-01-01
// * Declaration usage :
// * Signal1<> onMySignal;
// * Call the signal :
// * onMySignal();
// */
//template <>
//struct Signal1 <NullType, NullType, NullType, NullType>
//{
//    typedef EvtFunction0Param ParamType;

//    typedef Signal1<NullType, NullType, NullType, NullType> SignalType;

//    mutable EvtContainer<ParamType> mContainer;

//    void operator ()() const
//    {
//        EvtContainer<ParamType>::T_FunctionArray lCallBacks;
//        mContainer.getCallBacks(lCallBacks);

//        for (EvtContainer<ParamType>::IT_FunctionArray it = lCallBacks.begin() ; it != lCallBacks.end() ; it++)
//        {
//            (*it)->call();
//        }
//    }

//    typedef EvtFunction0Param::T_FunctionParamPtr Cast;

//    template <class FUNC_PTR>
//    void bind(
//            EvtHandler *sender,
//            EvtHandler *receiver,
//            FUNC_PTR slot,
//            EvtHandler::ConnectionType _type=EvtHandler::CONNECTION_AUTO)
//    {
//        EvtLinkBase *link = new EvtLink<ParamType>(
//                                sender,
//                                receiver,
//                                //reinterpret_cast<void*>(receiver),
//                                &this->mContainer,
//                                String(""),
//                                new ParamType((Cast)slot));

//        receiver->connect(link, _type);
//    }
//};

///**
// * @brief A signal with 1 parameters and void as return.
// * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
// * @date 2013-01-01
// * Declaration usage :
// * Signal1<TYPE> onMySignal;
// * Call the signal :
// * onMySignal(VALUE);
// */
//template <class P0>
//struct Signal1 <P0, NullType, NullType, NullType>
//{
//    typedef EvtFunction1Param<P0> ParamType;
//    typedef typename EvtContainer<ParamType>::T_FunctionArray T_FunctionArray;
//    typedef typename EvtContainer<ParamType>::IT_FunctionArray IT_FunctionArray;

//    typedef Signal1<P0, NullType, NullType, NullType> SignalType;

//    mutable EvtContainer<ParamType> mContainer;

//    void operator ()(P0 const &_p0) const
//    {
//        T_FunctionArray lCallBacks;
//        mContainer.getCallBacks(lCallBacks);

//        for (IT_FunctionArray it = lCallBacks.begin() ; it != lCallBacks.end() ; it++)
//        {
//            (*it)->call(_p0);
//        }
//    }

//    typedef typename EvtFunction1Param<P0>::T_FunctionParamPtr Cast;

//    template <class FUNC_PTR>
//    void bind(
//            EvtHandler *sender,
//            EvtHandler *receiver,
//            FUNC_PTR slot,
//            EvtHandler::ConnectionType _type=EvtHandler::CONNECTION_AUTO)
//    {
//        EvtLinkBase *link = new EvtLink<ParamType>(
//                                sender,
//                                receiver,
//                                //reinterpret_cast<void*>(receiver),
//                                &this->mContainer,
//                                String(""),
//                                new ParamType((Cast)slot));

//        receiver->connect(link, _type);
//    }
//};

///**
// * @brief A signal with 2 parameters and void as return.
// * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
// * @date 2013-01-01
// * Declaration usage :
// * Signal1<TYPE1, TYPE2> onMySignal;
// * Call the signal :
// * onMySignal(VALUE1, VALUE2);
// */
//template <class P0, class P1>
//struct Signal1 <P0, P1, NullType, NullType>
//{
//    typedef EvtFunction2Param<P0, P1> ParamType;
//    typedef typename EvtContainer<ParamType>::T_FunctionArray T_FunctionArray;
//    typedef typename EvtContainer<ParamType>::IT_FunctionArray IT_FunctionArray;

//    typedef Signal1<P0, P1, NullType, NullType> SignalType;

//    mutable EvtContainer<ParamType> mContainer;

//    void operator ()(P0 const &_p0, P1 const &_p1) const
//    {
//        T_FunctionArray lCallBacks;
//        mContainer.getCallBacks(lCallBacks);

//        for (IT_FunctionArray it = lCallBacks.begin() ; it != lCallBacks.end() ; it++)
//        {
//            (*it)->call(_p0, _p1);
//        }
//    }

//    typedef typename EvtFunction2Param<P0, P1>::T_FunctionParamPtr Cast;

//    template <class FUNC_PTR>
//    void bind(
//            EvtHandler *sender,
//            EvtHandler *receiver,
//            FUNC_PTR slot,
//            EvtHandler::ConnectionType _type=EvtHandler::CONNECTION_AUTO)
//    {
//        EvtLinkBase *link = new EvtLink<ParamType>(
//                                sender,
//                                receiver,
//                                //reinterpret_cast<void*>(receiver),
//                                &this->mContainer,
//                                String(""),
//                                new ParamType((Cast)slot));

//        receiver->connect(link, _type);
//    }
//};

///**
// * @brief A signal with 3 parameters and void as return.
// * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
// * @date 2013-01-01
// * Declaration usage :
// * Signal1<TYPE1, TYPE2, TYPE3> onMySignal;
// * Call the signal :
// * onMySignal(VALUE1, VALUE2, VALUE3);
// */
//template <class P0, class P1, class P2>
//struct Signal1 <P0, P1, P2, NullType>
//{
//    typedef EvtFunction3Param<P0, P1, P2> ParamType;
//    typedef typename EvtContainer<ParamType>::T_FunctionArray T_FunctionArray;
//    typedef typename EvtContainer<ParamType>::IT_FunctionArray IT_FunctionArray;

//    typedef Signal1<P0, P1, P2, NullType> SignalType;

//    mutable EvtContainer<ParamType> mContainer;

//    void operator ()(P0 const &_p0, P1 const &_p1, P2 const &_p2) const
//    {
//        T_FunctionArray lCallBacks;
//        mContainer.getCallBacks(lCallBacks);

//        for (IT_FunctionArray it = lCallBacks.begin() ; it != lCallBacks.end() ; it++)
//        {
//            (*it)->call(_p0, _p1, _p2);
//        }
//    }

//    typedef typename EvtFunction3Param<P0, P1, P2>::T_FunctionParamPtr Cast;

//    template <class FUNC_PTR>
//    void bind(
//            EvtHandler *sender,
//            EvtHandler *receiver,
//            FUNC_PTR slot,
//            EvtHandler::ConnectionType _type=EvtHandler::CONNECTION_AUTO)
//    {
//        EvtLinkBase *link = new EvtLink<ParamType>(
//                                sender,
//                                receiver,
//                                //reinterpret_cast<void*>(receiver),
//                                &this->mContainer,
//                                String(""),
//                                new ParamType((Cast)slot));

//        receiver->connect(link, _type);
//    }
//};

///**
// * @brief A signal with 4 parameters and void as return.
// * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
// * @date 2013-01-01
// * Declaration usage :
// * Signal1<TYPE1, TYPE2, TYPE3, TYPE4> onMySignal;
// * Call the signal :
// * onMySignal(VALUE1, VALUE2, VALUE3, VALUE4);
// */
//template <class P0, class P1, class P2, class P3>
//struct Signal1 // <P0, P1, P2, P3>
//{
//    typedef EvtFunction4Param<P0, P1, P2, P3> ParamType;
//    typedef typename EvtContainer<ParamType>::T_FunctionArray T_FunctionArray;
//    typedef typename EvtContainer<ParamType>::IT_FunctionArray IT_FunctionArray;

//    typedef Signal1<P0, P1, P2, P3> SignalType;

//    mutable EvtContainer<ParamType> mContainer;

//    void operator ()(P0 const &_p0, P1 const &_p1, P2 const &_p2, P3 const &_p3) const
//    {
//        T_FunctionArray lCallBacks;
//        mContainer.getCallBacks(lCallBacks);

//        for (IT_FunctionArray it = lCallBacks.begin() ; it != lCallBacks.end() ; it++)
//        {
//            (*it)->call(_p0, _p1, _p2, _p3);
//        }
//    }

//    typedef typename EvtFunction4Param<P0, P1, P2, P3>::T_FunctionParamPtr Cast;

//    template <class FUNC_PTR>
//    void bind(
//            EvtHandler *sender,
//            EvtHandler *receiver,
//            FUNC_PTR slot,
//            EvtHandler::ConnectionType _type=EvtHandler::CONNECTION_AUTO)
//    {
//        EvtLinkBase *link = new EvtLink<ParamType>(
//                                sender,
//                                receiver,
//                                //reinterpret_cast<void*>(receiver),
//                                &this->mContainer,
//                                String(""),
//                                new ParamType((Cast)slot));

//        receiver->connect(link, _type);
//    }
//};

/**
 * @brief A signal with variadic parameters and void as return.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-01-04
 * Declaration usage :
 * Signal<TYPE1, TYPE2, ...> onMySignal{this};
 * Call the signal :
 * onMySignal(VALUE1, VALUE2, ...);
 * @note Uses of the new {} constructor parameters in way to easily initiate
 * a signal to this parent as sender event handler.
 */
template <typename ...Params>
struct Signal
{
    EvtHandler *m_owner;

    template<class T>
    Signal(T *_this) :
        m_owner(static_cast<EvtHandler*>(_this))
    {
    }

    typedef EvtStdFunction<Params...> ParamType;
    typedef typename EvtContainer<ParamType>::T_FunctionArray T_FunctionArray;
    typedef typename EvtContainer<ParamType>::IT_FunctionArray IT_FunctionArray;

    typedef Signal<Params...> SignalType;

    mutable EvtContainer<ParamType> mContainer;

    void operator ()(Params... values) const
    {
        T_FunctionArray lCallBacks;
        mContainer.getCallBacks(lCallBacks);

        for (IT_FunctionArray it = lCallBacks.begin() ; it != lCallBacks.end() ; it++)
        {
            (*it)->call(values...);
        }
    }

    typedef void (EvtHandler::*T_FunctionParamPtr)(Params...);
    typedef typename EvtStdFunction<Params...>::T_StdFunction T_StdFunction;

    using placeholders_list = std::tuple<decltype(std::placeholders::_1)
                                   , decltype(std::placeholders::_2)
                                   , decltype(std::placeholders::_3)
                                   , decltype(std::placeholders::_4)
                                   , decltype(std::placeholders::_5)
                                   , decltype(std::placeholders::_6)
                                   , decltype(std::placeholders::_7)
                                   , decltype(std::placeholders::_8)
                                   , decltype(std::placeholders::_9)
                                   , decltype(std::placeholders::_10)
                                   >;

    static constexpr std::size_t value = sizeof...(Params);

    template <typename C, typename T, std::size_t... Ints>
    static auto binder(void (C::*funcPtr)(Params...), T t, std::index_sequence<Ints...>)
    {
        return std::bind(funcPtr, t, typename std::tuple_element<Ints, placeholders_list>::type{}...);
    }

    //! Connect with a class::method pointer.
    template <class C>
    void connect(
            EvtHandler *receiver,
            void (C::*funcPtr)(Params...),
            EvtHandler::ConnectionType _type=EvtHandler::CONNECTION_AUTO)
    {
        //T_StdFunction func = std::bind((T_FunctionParamPtr)slot, std::ref(receiver), std::placeholders::_1);
        auto func = binder((T_FunctionParamPtr)funcPtr, receiver, std::make_index_sequence<value>{});

        EvtLinkBase *link = new EvtLink<ParamType>(
                                m_owner,
                                receiver,
                                //reinterpret_cast<void*>(receiver),
                                &this->mContainer,
                                String(""),
                                new ParamType(func));
                                //new ParamType((T_FunctionParamPtr)slot));

        receiver->connect(link, _type);
    }

    //! Connect with a class::method pointer and a specific thread.
    template <class C>
    void connect(
            EvtHandler *receiver,
            void (C::*funcPtr)(Params...),
            Thread *thread)
    {
        auto func = binder((T_FunctionParamPtr)funcPtr, receiver, std::make_index_sequence<value>{});

        EvtLinkBase *link = new EvtLink<ParamType>(
                                m_owner,
                                receiver,
                                //reinterpret_cast<void*>(receiver),
                                &this->mContainer,
                                String(""),
                                new ParamType(func));

        receiver->connect(link, thread);
    }

    //! Connect with a std::function (bind or lambda expression) and a specified receiver.
    template<class T>
    void connect(
            T *receiver,
            T_StdFunction func,
            EvtHandler::ConnectionType _type=EvtHandler::CONNECTION_AUTO)
    {
        EvtLinkBase *link = new EvtLink<ParamType>(
                                m_owner,
                                static_cast<EvtHandler*>(receiver),
                                //reinterpret_cast<void*>(receiver),
                                &this->mContainer,
                                String(""),
                                new ParamType(func));

        receiver->connect(link, _type);
    }

    //! Connect with a std::function (bind or lambda expression), a specified receiver,
    //! on a specified thread.
    template<class T>
    void connect(
            T *receiver,
            T_StdFunction func,
            Thread *thread)
    {
        EvtLinkBase *link = new EvtLink<ParamType>(
                                m_owner,
                                static_cast<EvtHandler*>(receiver),
                                //reinterpret_cast<void*>(receiver),
                                &this->mContainer,
                                String(""),
                                new ParamType(func));

        receiver->connect(link, thread);
    }

    //! Connect with a std::function (bind or lambda expression)
    //! No need of receiver, it assume it is himself for conveniance.
    void connect(
            T_StdFunction func,
            EvtHandler::ConnectionType _type=EvtHandler::CONNECTION_AUTO)
    {
        EvtLinkBase *link = new EvtLink<ParamType>(
                                m_owner,
                                m_owner,
                                //reinterpret_cast<void*>(m_owner),
                                &this->mContainer,
                                String(""),
                                new ParamType(func));

        m_owner->connect(link, _type);
    }

    //! Connect with a std::function (bind or lambda expression)
    //! No need of receiver, it assume it is himself for conveniance.
    void connect(
            T_StdFunction func,
            Thread *thread)
    {
        EvtLinkBase *link = new EvtLink<ParamType>(
                                m_owner,
                                m_owner,
                                //reinterpret_cast<void*>(m_owner),
                                &this->mContainer,
                                String(""),
                                new ParamType(func));

        m_owner->connect(link, thread);
    }
};


/**
 * @brief Specialization of the previous signe for 0 zero parameters and void as return.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-01-04
 * Declaration usage :
 * Signal<> onMySignal{this};
 * Call the signal :
 * onMySignal();
 * @note Uses of the new {} constructor parameters in way to easily initiate
 * a signal to this parent as sender event handler.
 */
template <>
struct Signal <>
{
    EvtHandler *m_owner;

    template<class T>
    Signal(T *_this) :
        m_owner(static_cast<EvtHandler*>(_this))
    {
    }

    typedef EvtStdFunction0Param ParamType;
    typedef typename EvtContainer<ParamType>::T_FunctionArray T_FunctionArray;
    typedef typename EvtContainer<ParamType>::IT_FunctionArray IT_FunctionArray;

    typedef Signal<> SignalType;

    mutable EvtContainer<ParamType> mContainer;

    void operator ()() const
    {
        T_FunctionArray lCallBacks;
        mContainer.getCallBacks(lCallBacks);

        for (IT_FunctionArray it = lCallBacks.begin() ; it != lCallBacks.end() ; it++)
        {
            (*it)->call();
        }
    }

    typedef void (EvtHandler::*T_FunctionParamPtr)();
    typedef typename EvtStdFunction0Param::T_StdFunction T_StdFunction;

    //! Connect with a class::method pointer.
    template <class C>
    void connect(
            EvtHandler *receiver,
            void (C::*funcPtr)(),
            EvtHandler::ConnectionType _type=EvtHandler::CONNECTION_AUTO)
    {
        T_StdFunction func = std::bind((T_FunctionParamPtr)funcPtr, receiver);

        EvtLinkBase *link = new EvtLink<ParamType>(
                                m_owner,
                                receiver,
                                //reinterpret_cast<void*>(receiver),
                                &this->mContainer,
                                String(""),
                                new ParamType(func));
                                //new ParamType((T_FunctionParamPtr)slot));

        receiver->connect(link, _type);
    }

    //! Connect with a class::method pointer.
    template <class C>
    void connect(
            EvtHandler *receiver,
            void (C::*funcPtr)(),
            Thread *thread)
    {
        T_StdFunction func = std::bind((T_FunctionParamPtr)funcPtr, receiver);

        EvtLinkBase *link = new EvtLink<ParamType>(
                                m_owner,
                                receiver,
                                //reinterpret_cast<void*>(receiver),
                                &this->mContainer,
                                String(""),
                                new ParamType(func));

        receiver->connect(link, thread);
    }

    //! Connect with a std::function (bind or lambda expression) and a specified receiver.
    template<class T>
    void connect(
            T *receiver,
            T_StdFunction func,
            EvtHandler::ConnectionType _type=EvtHandler::CONNECTION_AUTO)
    {
        EvtLinkBase *link = new EvtLink<ParamType>(
                                m_owner,
                                static_cast<EvtHandler*>(receiver),
                                //reinterpret_cast<void*>(receiver),
                                &this->mContainer,
                                String(""),
                                new ParamType(func));

        receiver->connect(link, _type);
    }

    //! Connect with a std::function (bind or lambda expression) and a specified receiver.
    template<class T>
    void connect(
            T *receiver,
            T_StdFunction func,
            Thread *thread)
    {
        EvtLinkBase *link = new EvtLink<ParamType>(
                                m_owner,
                                static_cast<EvtHandler*>(receiver),
                                //reinterpret_cast<void*>(receiver),
                                &this->mContainer,
                                String(""),
                                new ParamType(func));

        receiver->connect(link, thread);
    }

    //! Connect with a std::function (bind or lambda expression)
    //! No need of receiver, it assume it is himself for conveniance.
    void connect(
            T_StdFunction func,
            EvtHandler::ConnectionType _type=EvtHandler::CONNECTION_AUTO)
    {
        EvtLinkBase *link = new EvtLink<ParamType>(
                                m_owner,
                                m_owner,
                                //reinterpret_cast<void*>(m_owner),
                                &this->mContainer,
                                String(""),
                                new ParamType(func));

        m_owner->connect(link, _type);
    }

    //! Connect with a std::function (bind or lambda expression)
    //! No need of receiver, it assume it is himself for conveniance.
    void connect(
            T_StdFunction func,
            Thread *thread)
    {
        EvtLinkBase *link = new EvtLink<ParamType>(
                                m_owner,
                                m_owner,
                                //reinterpret_cast<void*>(m_owner),
                                &this->mContainer,
                                String(""),
                                new ParamType(func));

        m_owner->connect(link, thread);
    }
};

} // end namespace o3d

#endif // _O3D_EVT_H

