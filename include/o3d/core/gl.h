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

    /**
     * @brief Inherit from this class and specialize call to reference your internal
     * get proc address method.
     */
    class GetProcAddressCallbackMethod
    {
    public:

        virtual void* call(const Char* ext)
        {
            return nullptr;
        }
    };

    //! Initialize, plateform dependency.
    static void init(const Char *library = nullptr);

    //! Terminate (delete an eventual ms_callback).
    static void quit();

    //! Setup the adresse of the get proc address function pointer.
    static void setProcAddress(GetProcAddressCallbackMethod *callback);

    //! Returns a GL extension pointer address.
    static void* getProcAddress(const Char *ext);

    // static void swapBuffers();

private:

    static GetProcAddressCallbackMethod* ms_callback;
};

} // namespace o3d

#endif // _O3D_GL_H
