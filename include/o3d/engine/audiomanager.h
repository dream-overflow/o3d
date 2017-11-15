/**
 * @file audiomanager.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_AUDIOMANAGER_H
#define _O3D_AUDIOMANAGER_H

#include "o3d/core/base.h"

namespace o3d {

/**
 * @brief Audio manager interface.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2014-01-30
 */
class O3D_API AudioManager
{
public:

    virtual ~AudioManager() = 0;

    /**
     * @brief init Called when the manager is set to the scene.
     */
    virtual void init() = 0;

    /**
     * @brief release Called when the manager is removed from the scene.
     */
    virtual void release() = 0;

    /**
     * @brief update Called on scene update.
     */
    virtual void update() = 0;

    /**
     * @brief focus Called when get the focus.
     */
    virtual void focus() = 0;

    /**
     * @brief lostFocus Call when the focus is lost.
     */
    virtual void lostFocus() = 0;
};

} // namespace o3d

#endif // _O3D_AUDIOMANAGER_H

