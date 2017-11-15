/**
 * @file worldmanager.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_WORLDMANAGER_H
#define _O3D_WORLDMANAGER_H

#include "../hierarchy/node.h"

namespace o3d {

/**
 * @brief World manager is responsible of the full or part of the world.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2016-02-14
 * A world manager update, display, load, unload scene object, according to
 * specific technics, strategy, and have in responsibility the state of the scene
 * objects.
 * A common world manager is WorldDistance, WorldQuadtree and WorldOctree that are
 * respectively an implementation of a distance based view and load/unload,
 * a quadtree based, and finally an octree based management of the scene.
 * It is possible to have many world manager, in way to control different area of
 * a seamless world.
 * The world region is a scene object, directly managed into a viewport
 * The world can be made of multiple regions, each having its own policy, and set
 * of resources.
 * There is a special case of region, the WorldGlobal. It manages omnipresents resources,
 * because somes objects need to be always present, such as an object targeted by
 * the camera.
 */
class O3D_API WorldManager : public SceneObject
{
public:

    O3D_DECLARE_CLASS(WorldManager)

    //! Default constructor.
    //! When the scene object is created it is automatically added to the
    //! SceneObjectManager of the scene.
    WorldManager(BaseObject *parent);

    //! Virtual destructor.
    virtual ~WorldManager();

    //! Delete child
    virtual Bool deleteChild(BaseObject *child);


    virtual void draw(const DrawInfo &drawInfo);
    virtual void update();

protected:

    //! Restricted copy constructor.
    WorldManager(const WorldManager &dup);

    //! Restricted copy operator
    WorldManager& operator=(const WorldManager &dup);

};


} // namespace o3d

#endif // _O3D_WORLDMANAGER_H

