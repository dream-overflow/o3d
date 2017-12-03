/**
 * @file gl.h
 * @brief GL configuration entry point.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_GL_H
#define _O3D_GL_H

#include "o3d/core/base.h"

namespace o3d {

/**
 * @brief OpenGL initializer, with somes helpers.
 */
class O3D_API GL
{
public:

    //! Initialize, plateform dependency.
    static void init(const Char *library = nullptr);

    //! Returns a GL extension pointer address.
    static void* getProcAddress(const Char *ext);

    // static void swapBuffers();
};

} // namespace o3d

#endif // _O3D_GL_H
