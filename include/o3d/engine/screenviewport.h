/**
 * @file screenviewport.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SCREENVIEWPORT_H
#define _O3D_SCREENVIEWPORT_H

#include "viewport.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

/**
 * @brief Viewport managing.
 * @date 2002-08-01
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * Screen out viewport. A portion (or the full) of the rendering device context.
 */
class O3D_API ScreenViewPort : public ViewPort
{
public:

	O3D_DECLARE_CLASS(ScreenViewPort)

	//! Default constructor
    ScreenViewPort(
            BaseObject *parent,
            Camera* camera = nullptr,
            SceneDrawer *drawer = nullptr);

	//! Display the view-port.
    virtual void display(UInt32 w, UInt32 h) override;
};

} // namespace o3d

#endif // _O3D_SCREENVIEWPORT_H
