/**
 * @file worldmanager.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/world/worldmanager.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(WorldManager, 100000000/*ENGINE_WORLD_MANAGER*/, SceneObject)

WorldManager::WorldManager(BaseObject *parent) :
    SceneObject(parent)
{
    setMovable(False);
    setUpdatable(True);
    setAnimatable(False);
    setDrawable(True);
    setPickable(False);
    setShadable(False);
    setShadowable(False);
}

WorldManager::~WorldManager()
{

}

// Delete child
Bool WorldManager::deleteChild(BaseObject *child)
{
 /*  if (child)
    {
        if (child->getParent() != this)
            O3D_ERROR(E_InvalidParameter("The parent child differ from this"));
        else
        {
            // object should be type of SceneObject
            SceneObject *object = dynamicCast<SceneObject*>(child);
            if (object)
            {
                IT_SonList it = m_objectList.begin();
                for (; it != m_objectList.end(); ++it)
                {
                    if ((*it) == object)
                        break;
                }

                // remove the object of the son list
                if (it != m_objectList.end())
                {
                    m_objectList.erase(it);

                    // # TODO erase from draw list or visibility manager
                    //if (object->hasDrawable())
                        //m_drawList

                    object->setNode(nullptr);
                }

                deletePtr(object);
            }
            else
            {
                // otherwise simply delete it
                deletePtr(child);
            }

            return True;
        }
    }*/
    return False;
}

void WorldManager::draw(const DrawInfo &drawInfo)
{

}

void WorldManager::update()
{

}

