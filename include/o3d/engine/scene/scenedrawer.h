/**
 * @file scenedrawer.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SCENEDRAWER_H
#define _O3D_SCENEDRAWER_H

#include "sceneentity.h"

namespace o3d {

class WorldManager;
class ViewPort;

/**
 * @brief Draw pipeline abstract class for a scene.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-01-13
 */
class O3D_API SceneDrawer :
        public SceneEntity,
        NonCopyable<>,
        NonAssignable<>
{
public:

   O3D_DECLARE_ABSTRACT_CLASS(SceneDrawer)

   //! Constructor.
   SceneDrawer(BaseObject *parent);

   /**
    * @brief draw Draw into the given viewport.
    * @details Process a draw. This method is called by a viewport draw.
    * @param viewPort Valid current viewport.
    */
   virtual void draw(ViewPort *viewPort) = 0;

   /**
    * @brief Process a simpler draw for the picking pass.
    * @details Process a draw. This method is called by a viewport drawPicking.
    * @param viewPort Valid current viewport.
    */
   virtual void drawPicking(ViewPort *viewPort) = 0;

   //! Add a world manager to display with this drawer instance.
   //! @param wm Valid world manager.
//   virtual void addWorldManager(WorldManager *wm) = 0;

   //! Remove a world manager from this drawer.
   //! @return True if found and successfully removed.
//   virtual Bool removeWorldManager(WorldManager *wm) = 0;
};

} // o3d

#endif // _O3D_SCENEDRAWER_H
