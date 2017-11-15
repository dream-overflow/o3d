/**
 * @file servicemanager.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SERVICEMANAGER_H
#define _O3D_SERVICEMANAGER_H

#include "service.h"
#include "stringmap.h"
#include "stringlist.h"

namespace o3d {

class DynamicLibrary;

/**
 * @brief Service manager.
 * Description of the 'services' configuration file :
 * A line is key = value.
 * # for a comment line
 * initOrder = Sv1, Sv2 <,...>
 * startOrder = Sv1, Sv2 <,...>
 * stopOrder = Sv1, Sv2 <,...>
 * with Sv1, Sv2, services name.
 * More about service from dynamic library :
 * Dynamic libraries returns a service name and instance :
 * - String getServiceName() return the service name.
 * - Service* getService() return a new instance of the service class.
 * Thoose two functions must be declared in an extern "C" block, of a source file.
 *
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-26
 */
class O3D_API ServiceManager : public BaseObject
{
public:

    O3D_DECLARE_DYNAMIC_CLASS(ServiceManager)

    /**
     * @brief ServiceManager
     * A 'services.conf' file must be found in the configPath.
     * @param ldPath Path where to find dynamic library (can be empty if no dynamic libraries).
     * @pamar configPath Path where to find service manager configuration.
     */
    ServiceManager(const String &ldPath, const String &configPath);

    virtual ~ServiceManager();

    //! Get the dynamic library path.
    const String& getLdPath() const { return m_ldPath; }

    //! Get the configuration path.
    const String& getConfigPath() const { return m_configPath; }

    /**
     * @brief addService Add manualy a service.
     */
    void addService(Service *service);

    /**
     * @brief getService Get a service by name
     * @param name Valid service name
     * @return Valide service instance or nullptr if not found.
     */
    Service* getService(const String &name) const;

    /**
     * @brief load Load every *.dll and *.so found in the ldPath, and next load the service
     * config file.
     */
    void load();

    /**
     * @brief init Initialise is called before start.
     * Here do config checking, dependencies...
     */
    void init();

    /**
     * @brief start Run the service here (manage a thread...).
     */
    void start();

    /**
     * @brief stop All the operations to perform to cleanup correctly the service before
     *             another start on before to unload.
     */
    void stop();

    /**
     * @brief unload All the previously loaded service are unloaded. The libraries are
     *        unloaded too.
     */
    void unload();

private:

    ServiceManager(BaseObject *parent);

    String m_ldPath;
    String m_configPath;

    T_StringList m_initOrder;
    T_StringList m_startOrder;
    T_StringList m_stopOrder;

    StringMap<Service*> m_services;
    std::map<Service*, DynamicLibrary*> m_dls;
};

} // namespace o3d

#endif // _O3D_SERVICEMANAGER_H

