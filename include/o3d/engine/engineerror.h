/**
 * @file engineerror.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ENGINEERROR_H
#define _O3D_ENGINEERROR_H

#include "o3d/core/error.h"

namespace o3d {

/**
 * @brief E_OpenGLDebug throw when an OpenGL error is detected by an OpenGL debug context.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2014-04-27
 */
class O3D_API E_OpenGLDebug : public E_BaseException
{
    O3D_E_DEF_CLASS(E_OpenGLDebug)

    //! Ctor
    E_OpenGLDebug(const String& msg) throw() : E_BaseException(msg)
        O3D_E_DEF(E_OpenGLDebug, "OpenGL error")
};


} // namespace o3d

#endif // _O3D_ENGINEERROR_H

