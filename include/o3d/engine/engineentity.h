/**
 * @file engineentity.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ENGINEENTITY_H
#define _O3D_ENGINEENTITY_H

#include "o3d/core/smartobject.h"
#include "o3d/core/resourcemanager.h"
#include "o3d/core/memorydbg.h"

#include "enginetype.h"

namespace o3d {

class Engine;

/**
 * @brief A base object of the engine.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2016-02-17
 */
class O3D_API EngineEntity : public BaseObject
{
public:

    O3D_DECLARE_ABSTRACT_CLASS(EngineEntity)

    EngineEntity(BaseObject *parent) :
        BaseObject(parent)
    {
    }

    //! Get the parent engine.
    inline Engine* getEngine() { return reinterpret_cast<Engine*>(m_topLevelParent); }

    //! Get the parent engine (read only).
    inline const Engine* getEngine() const { return reinterpret_cast<Engine*>(m_topLevelParent); }

    //! The top level parent must be an engine.
    virtual Bool hasTopLevelParentTypeOf() const override;
};


/**
 * @brief A base resource of the engine.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2016-02-17
 */
class O3D_API EngineResource : public Resource
{
public:

    O3D_DECLARE_ABSTRACT_CLASS(EngineResource)

    EngineResource(BaseObject *parent) :
        Resource(parent)
    {
    }

    //! Get the parent engine.
    inline Engine* getEngine() { return reinterpret_cast<Engine*>(m_topLevelParent); }

    //! Get the parent engine (read only).
    inline const Engine* getEngine() const { return reinterpret_cast<Engine*>(m_topLevelParent); }

    //! The top level parent must be an engine.
    virtual Bool hasTopLevelParentTypeOf() const override;
};


/**
 * @brief A base resource manager of the engine.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2016-02-17
 */
class O3D_API EngineResourceManager : public ResourceManager
{
public:

    O3D_DECLARE_ABSTRACT_CLASS(EngineResourceManager)

    EngineResourceManager(
            BaseObject *parent,
            const String &path,
            const String &extList) :
        ResourceManager(parent, path, extList)
    {
    }

    //! Get the parent engine.
    inline Engine* getEngine() { return reinterpret_cast<Engine*>(m_topLevelParent); }

    //! Get the parent engine (read only).
    inline const Engine* getEngine() const { return reinterpret_cast<Engine*>(m_topLevelParent); }

    //! The top level parent must be an engine.
    virtual Bool hasTopLevelParentTypeOf() const override;
};

} // namespace o3d

#endif // _O3D_ENGINEENTITY_H

