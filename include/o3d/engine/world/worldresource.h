/**
 * @file worldresource.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_WORLDRESOURCE_H
#define _O3D_WORLDRESOURCE_H

#include "o3d/core/vector3.h"

namespace o3d {

class WorldManager;

/**
 * @brief World resource interface.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2016-02-14
 * A world resource is an high level object defining a way to load, unload its content
 * dynamically, and even to reaload its content dynamicaly for example during the world
 * processing when the set of object is no longer visible, and comes back again.
 * It is managed by a specialised world region.
 * The resource can comes from an external file, an o3dscene, a hard coded resource,
 * or even a scripted resource.
 * You can organise your resource as you need, for example a resource can be only a
 * simply mesh or light, but can also be a bunch of light with somes meshes. You
 * defines freely your granylarity.
 * If you need a more complexe preparation and organisation you should probably need
 * to write your own world region controller.
 * By default there are some commons world region controllers.
 */
class O3D_API WorldResource
{
public:

    enum WorldObjectState
    {
        WR_DESTROYED = 0,  //!< Unavaible, not preloaded or previously destroyed.
        WM_PREPARING = 1,  //!< Prepare the world resource in progess.
        WR_PREPARED = 2,   //!< Prepared, in way to a futur loading.
        WM_LOADING = 3,    //!< Loading the resource in progress.
        WR_LOADED = 4,     //!< Loaded into the scene and the world manager.
    };

    enum ProcessingState
    {
        PS_DONE = 0,
        PS_FAILED = 1,
        PS_IN_PROGRESS = 2
    };

    /**
     * @brief prepareWorldResource
     * @param wm World manager responsible of this object.
     * @return PS_DONE if the operation success, PS_FAILED if error, or PS_IN_PROGRESS
     * if the processing need more time to prepare (for big resource, of waiting for
     * incoming data over Internet...)
     * @details Prepare, preload the data of the resource from its origin. For example
     * to load a model and somes related data from a file.
     * The state goes from WR_DESTROYED to WR_PRELOADED when success.
     * Shared world resource can be performed at this level. For example you could have
     * a resource manager, that is responsible of the resources loading, and into the
     * preload method you can manage a reference counter.
     * At this time the resource is only queried for a later usage.
     * If the preparation required asynchronous processing, this method must returns
     * PS_IN_PROGRESS. The world manager will wait for a further call of its
     * preparedWorldResource method.
     * @note After this call the state is WM_PREPARING or WM_PREPARED.
     */
    virtual ProcessingState prepareWorldResource(WorldManager *wm) = 0;

    /**
     * @brief loadWorldResource
     * @param wm World manager responsible of this object.
     * @return True if the operation success.
     * Load the previously prepared resource into the owner world manager and the scene.
     * Small resources or very fastly generated resources should use only this method,
     * and simply returns PS_DONE for the prepare method.
     * The load method is responsible to add its entities into the scene and into
     * the world manager. This call is necessarily synchronous.
     * If False is returns, the manager ignore this resource, you have to manage this
     * behavior. A manual call to the world manager using the loadWorldResource method
     * can be done later.
     * @note After this call the state is WM_LOADED.
     */
    virtual Bool loadWorldResource(WorldManager *wm) = 0;

    /**
     * @brief unloadWorldResource
     * @param wm World manager responsible of this object.
     * When an objet is no longer usefull for the world manager (non longer visible after
     * a certain policy...) this method is called in way to clean the previously added
     * scene entities. This operation must be the inverse of the load method.
     * @note After this call the state is WS_PREPARED.
     */
    virtual void unloadWorldResource(WorldManager *wm) = 0;

    /**
     * @brief destroyWorldResource
     * @param wm World manager responsible of this object.
     * After the resource was unloaded, the world manager will call for this method,
     * in way to destroy more deeply the related resource. It is the inverse of the
     * prepare method. This method is called according a certain policy.
     * Maybe you have a resource manager, and at this level you can minus the reference
     * counter, and finally unloading resource from memory.
     * @note After this call the state is WS_DESTROYED.
     */
    virtual void destroyWorldResource(WorldManager *wm) = 0;

    /**
     * @brief getWorldResourcePosition
     * @return The world position of the resource or bunch of resource. It is necessary
     * for the world manager in way to know when to prepare, load, unload, destroy the
     * resource correctly.
     */
    virtual Vector3 getWorldResourcePosition() const = 0;

    /**
     * @brief getWorldResourceBoundingVolume
     * @return The resource magnitude with a generic bounding volume object. As the
     * position this information is mandatory. If the objet is relatively very small
     * the bounding volume can be a simple sphere of a small radius.
     * This bounding volume has not necessarely be exactly the same than the Drawable
     * or Audible ones, but keep in mind it is used to prepare, load, unload and destroy
     * the resource according to a policy.
     */
    //virtual BoundingVolume getWorldResourceBoundingVolume() const = 0;
};


} // namespace o3d

#endif // _O3D_WORLDRESOURCE_H

