/**
 * @file guimanager.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_GUIMANAGER_H
#define _O3D_GUIMANAGER_H

#include "o3d/core/base.h"

namespace o3d {

class ViewPort;

/**
 * @brief Gui manager interface.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2014-01-30
 */
class O3D_API GuiManager
{
public:

    virtual ~GuiManager() = 0;

    /**
     * @brief init Called when the Gui is set to a scene.
     */
    virtual void init() = 0;

    /**
     * @brief release Called when the manager is removed from the scene.
     */
    virtual void release() = 0;

    /**
     * @brief update Called at each scene update.
     */
    virtual void update() = 0;

    /**
     * @brief reshape On reshape.
     * @param width In pixels
     * @param height In pixels
     */
    virtual void reshape(UInt32 width, UInt32 height) = 0;

    /**
     * @brief Return the viewport.
     */
    virtual ViewPort* getViewPort() = 0;

    /**
     * @brief Return the viewport (const version).
     */
    virtual const ViewPort* getViewPort() const = 0;
};

} // namespace o3d

#endif // _O3D_GUIMANAGER_H

