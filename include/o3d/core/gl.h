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

    enum GLAPIType
    {
        GLAPI_UNDEFINED = 0x0,
        GLAPI_CUSTOM = 0x1,
        GLAPI_GL = 0x10,
        GLAPI_GLES_3 = 0x20
    };

    enum Impl
    {
        IMPL_NONE = 0,
        IMPL_CUSTOM = 1,
        IMPL_GLX_14 = 2,
        IMPL_WGL = 3,
        IMPL_EGL_15 = 4,
        IMPL_SDL_2 = 5
    };

    /**
     * @brief Inherit from this class and specialize call to reference your internal
     * get proc address method.
     */
    class GetProcAddressCallbackMethod
    {
    public:

        virtual ~GetProcAddressCallbackMethod() = 0;
        virtual void* call(const Char *ext) = 0;
        virtual void swapBuffers(void *surface) = 0;
    };

    //! Initialize, plateform dependency.
    static void init(const Char *library = nullptr);

    //! Terminate (delete an eventual ms_callback).
    static void quit();

    //! Setup the adresse of the get proc address function pointer.
    static void setProcAddress(GetProcAddressCallbackMethod *callback);

    //! Returns a GL extension pointer address.
    static void* getProcAddress(const Char *ext);

    //! Which used implementation : EGL, GLX, SDL2 or WGL, CUSTOM or null.
    //! Custom means than a callback was provided.
    static const Char *getImplementationName();

    //! Which used implementation.
    static Impl getImplementation();

    //! Get instancied GL API type.
    static GLAPIType getType();

    //! Swap buffer according to defined native display and window handler,
    //! using the defined implementation.
    static void swapBuffers(_DISP display, _HWND hWnd, _HDC hdc);

private:

    static GetProcAddressCallbackMethod* ms_callback;
    static Impl ms_nativeImpl;
    static Impl ms_usedImpl;
};

} // namespace o3d

#endif // _O3D_GL_H
