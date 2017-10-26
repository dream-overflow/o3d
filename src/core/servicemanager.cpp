/**
 * @file servicemanager.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/servicemanager.h"
#include "o3d/core/coretype.h"
#include "o3d/core/virtualfilelisting.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/stringtokenizer.h"
#include "o3d/core/dynamiclibrary.h"

#include <functional>

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(Service, CORE_SERVICE, BaseObject)
O3D_IMPLEMENT_DYNAMIC_CLASS1(ServiceManager, CORE_SERVICE_MANAGER, BaseObject)

Service::Service() :
    m_state(STATE_UNLOADED)
{

}

Service::~Service()
{

}

ServiceManager::ServiceManager(const String &ldPath, const String &configPath) :
    BaseObject(nullptr),
    m_ldPath(ldPath),
    m_configPath(configPath)
{
}

ServiceManager::ServiceManager(BaseObject *parent) :
    BaseObject(parent)
{
}

ServiceManager::~ServiceManager()
{
    unload();
}

void ServiceManager::addService(Service *service)
{
    if (service)
        m_services.insert(std::make_pair(service->getName(), service));
}

Service *ServiceManager::getService(const String &name) const
{
    auto it = m_services.find(name);
    if (it != m_services.end())
        return it->second;
    else
        return nullptr;
}

void ServiceManager::load()
{
    // Load libraries
    VirtualFileListing fileListing;
    fileListing.setPath(m_ldPath);
    fileListing.setExt("*.dll|*.so");

    fileListing.searchFirstFile();

    FLItem *fileItem;
    String serviceName;
    Service *service;

    while ((fileItem = fileListing.searchNextFile()) != nullptr)
    {
        // a sub directory
        if (fileItem->FileType == FILE_FILE)
        {
            DynamicLibrary *dl = DynamicLibrary::load(fileListing.getFileFullName());

            if (dl->getFunctionPtr(CString("getServiceName")) && dl->getFunctionPtr(CString("getService")))
            {
                auto getName = dl->getFunction<String>(CString("getServiceName"));
                serviceName = getName();

                auto getService = dl->getFunction<Service*>(CString("getService"));
                service = getService();

                if (service)
                {
                    // service load
                    if (!service->isLoaded())
                        service->load();

                    m_dls.insert(std::make_pair(service, dl));
                    m_services.insert(std::make_pair(serviceName, service));

                    O3D_MESSAGE(String("Found and add service ") + serviceName);
                }
                else
                {
                    DynamicLibrary::unload(dl);
                    O3D_WARNING(String("Found but unable to load the service ") + serviceName);
                }
            }
        }
    }

    // Load config file
    String filePath = FileManager::instance()->getFullFileName(m_configPath + "/services.conf");
    InStream *configFile = FileManager::instance()->openInStream(filePath);

    String key;

    T_StringList *list = nullptr;

    String line;
    while (configFile->readLine(line) != EOF)
    {
        line.trimLeftChars(" \t");
        line.trimRightChars(" \t");

        if (line.startsWith("#"))
        {
            // comment
        }
        else
        {
            StringTokenizer tk(line, " =,");
            key = tk.nextToken();

            if (!tk.hasMoreTokens())
            {
                deletePtr(configFile);
                O3D_ERROR(E_InvalidFormat(String("Missing value for key ") + key + "in " + m_ldPath + "/services"));
            }

            if (key == "initOrder")
                list = &m_initOrder;
            else if (key == "startOrder")
                list = &m_startOrder;
            else if (key == "stopOrder")
                list = &m_stopOrder;
            else
            {
                deletePtr(configFile);
                O3D_ERROR(E_InvalidFormat(String("Unsupported key ") + key + "in " + m_ldPath + "/services"));
            }

            while (tk.hasMoreTokens())
            {
                serviceName = tk.nextToken();

                // comment, stop here
                if (serviceName.startsWith("#"))
                    break;

                auto it = m_services.find(serviceName);
                if (it != m_services.end())
                {
                    list->push_back(serviceName);
                }
                else
                {
                    O3D_WARNING(String("Try to use the service ") + serviceName + " but it is not loaded");
                }
            }
        }
    }

    deletePtr(configFile);
}

void ServiceManager::init()
{
    Service *service;

    for (String &serviceName : m_initOrder)
    {
        auto it = m_services.find(serviceName);
        if (it != m_services.end())
        {
            service = it->second;
            O3D_MESSAGE(String("Initializing service " + serviceName + "..."));

            if (service->getState() != Service::STATE_LOADED)
            {
                O3D_WARNING(String("Service " + serviceName + " is not loaded"));
                continue;
            }

            if (!service->isInit())
            {
                O3D_MESSAGE(String("Service " + serviceName + " initialized"));
                service->init();
            }
            else
                O3D_MESSAGE(String("Service " + serviceName + " already initialized"));
        }
    }
}

void ServiceManager::start()
{
    Service *service;

    for (String &serviceName : m_startOrder)
    {
        auto it = m_services.find(serviceName);
        if (it != m_services.end())
        {
            service = it->second;
            O3D_MESSAGE(String("Starting service " + serviceName + "..."));

            if (service->getState() != Service::STATE_INITIALIZED)
            {
                O3D_WARNING(String("Service " + serviceName + " is not initialized"));
                continue;
            }

            if (!service->isStarted())
            {
                O3D_MESSAGE(String("Service " + serviceName + " started"));
                service->start();
            }
            else
                O3D_MESSAGE(String("Service " + serviceName + " already started"));
        }
    }
}

void ServiceManager::stop()
{
    Service *service;

    for (String &serviceName : m_stopOrder)
    {
        auto it = m_services.find(serviceName);
        if (it != m_services.end())
        {
            service = it->second;
            O3D_MESSAGE(String("Stopping service " + serviceName + "..."));

            if (service->getState() != Service::STATE_STARTED)
            {
                O3D_WARNING(String("Service " + serviceName + " is not started"));
                continue;
            }

            if (!service->isStopped())
            {
                O3D_MESSAGE(String("Service " + serviceName + " stopped"));
                service->stop();
            }
            else
                O3D_MESSAGE(String("Service " + serviceName + " already stopped"));
        }
    }
}

void ServiceManager::unload()
{
    for (std::pair<String, Service*> entry : m_services)
    {
        Service *service = entry.second;

        O3D_MESSAGE(String("Unloading service ") + entry.first);

        if (service->isLoaded())
            service->unload();

        deletePtr(service);

        auto it = m_dls.find(entry.second);
        if (it != m_dls.end())
        {
            DynamicLibrary::unload(it->second);
            m_dls.erase(it);
        }
    }

    m_services.clear();
}

