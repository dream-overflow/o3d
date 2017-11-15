/**
 * @file service.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SERVICE_H
#define _O3D_SERVICE_H

#include "baseobject.h"

namespace o3d {

/**
 * @brief Service base clas.
 * A service is a managed class that is an entry point for a module.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-26
 */
class O3D_API Service : public BaseObject
{
public:

    O3D_DECLARE_ABSTRACT_CLASS(Service)

    enum ServiceState
    {
        STATE_UNLOADED = 0,
        STATE_LOADED,
        STATE_INITIALIZED,
        STATE_STARTED,
        STATE_STOPPED,
        STATE_RESTARTABLE
    };

    Service();

    virtual ~Service() = 0;

    /**
     * @brief load Useful for dynamic library loading.
     */
    virtual void load() = 0;

    /**
     * @brief init Initialise is called before start.
     * Here do config checking, dependencies...
     */
    virtual void init() = 0;

    /**
     * @brief start Run the service here (manage a thread...).
     */
    virtual void start() = 0;

    /**
     * @brief stop All the operation to perform to cleanup correctly the service.
     */
    virtual void stop() = 0;

    /**
     * @brief load Useful for dynamic library unloading.
     */
    virtual void unload() = 0;

    //! Get the current state.
    ServiceState getState() const { return m_state; }

    //! Is the service loaded.
    Bool isLoaded() const { return m_state >= STATE_LOADED; }

    //! Is the service initialized.
    Bool isInit() const { return m_state >= STATE_INITIALIZED; }

    //! Is the service starded.
    Bool isStarted() const { return m_state >= STATE_STARTED; }

    //! Is the service stopped.
    Bool isStopped() const { return m_state >= STATE_STOPPED; }

    //! Is the service previously stopped, and is can be restarted using start() again.
    Bool isRestartable() const { return m_state == STATE_RESTARTABLE; }

protected:

    ServiceState m_state;
};

} // namespace o3d

#endif // _O3D_SERVICE_H

