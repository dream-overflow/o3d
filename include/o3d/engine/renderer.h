/**
 * @file renderer.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_RENDERER_H
#define _O3D_RENDERER_H

#include "o3d/core/string.h"
#include "o3d/core/evthandler.h"
#include "o3d/core/memorydbg.h"
#include "o3d/core/templatebitset.h"

#include "o3d/image/image.h"

namespace o3d {

class Scene;
class Context;
class AppWindow;

/**
 * @brief The Renderer class
 * OpenGL renderer initialization, and managing.
 * A renderer is needed to create a scene for rendering off-screen and on-screen.
 *
 * The renderer need an AppWindow to be created. If you want to manage an external
 * window or frame you have to inherit from Renderer and implement your own create/share
 * methods and override isCurrent, setCurrent, setVerticalRefresh, isVerticalRefresh
 *  methods.
 *
 * Also, this is the renderer which own the Context object. A renderer can only
 * be destroyed when no Scene use it, and no shared renderer reference it.
 * Shared renderer are useful when you need many rendering context but want to have the
 * same textures, VBO, FBO, and shader for all shared renderer.
 * In this case you must use the share(...) method and not the create(...).
 *  Note that a shared renderer cannot be shared, and all shared renderer must be
 * destroyed before the sharing (original/main/parent) renderer.
 * @date 2002-08-01
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class O3D_API Renderer : public EvtHandler
{
public:

    //! OpenGL version.
    enum Version
    {
        OGL_UNDEFINED = 0,
        OGL_330 = 330,
        OGL_400 = 400,
        OGL_410 = 410,
        OGL_420 = 420,
        OGL_430 = 430,
        OGL_440 = 440,
        OGL_450 = 450,
        OGL_460 = 460   //!< or greater
    };

    //! OpenGL debug level
    enum DebugLevel
    {
        DEBUG_TYPE_ERROR               = 0x824C,
        DEBUG_TYPE_DEPRECATED_BEHAVIOR = 0x824D,
        DEBUG_TYPE_UNDEFINED_BEHAVIOR  = 0x824E,
        DEBUG_TYPE_PORTABILITY         = 0x824F,
        DEBUG_TYPE_PERFORMANCE         = 0x8250,
        DEBUG_TYPE_OTHER               = 0x8251
    };

	//! Default constructor.
	Renderer();

	//! Virtual destructor.
	virtual ~Renderer();

    /**
     * @brief Create the OpenGL rendered using a given an application window.
     * Must be defined for each sub-system.
     * @param appWindow Application window. Must be a valid window.
     * @param debug Since OGL 4.x debug mode is allowed.
     * @note It takes the color model of the application window. Also take care of
     * the debug mode, when use it it can impact performances.
     */
    virtual void create(AppWindow *appWindow, Bool debug = False);

    /**
     * @brief Share the OpenGL rendering using a given an application window.
     * Must be defined for each sub-system.
     * Like create(...) but with context sharing.
     * @param sharing Shared context, for sharing texture, VBO, FBO, and shaders.
     * @param appWindow Application window. Must be a valid window.
     * @param debug  Since OGL 4.x debug mode is allowed.
     * @warning Unsupported by SDL.
     * @note It takes the color model of the application window. Also take care of
     * the debug mode, when use it it can impact performances.
     */
    virtual void share(Renderer *sharing, AppWindow *appWindow, Bool debug = False);

	//! Delete the renderer. Must be defined for each sub-system.
	virtual void destroy();

    //
	// Getters
    //

	//! Is the renderer exists.
    inline Bool isValid() const { return m_state.getBit(STATE_DEFINED); }

    //! Is the debuging context defined and available.
    inline Bool hasDebugSupport() const { return m_state.getBit(STATE_DEBUG_AVAILABLE); }

	//! Get the handle device context.
	inline _HDC getHDC() const { return m_HDC; }
	//! Get the OpenGL handle rendering context.
	inline _HGLRC getHGLRC() const { return m_HGLRC; }

    //! Get the attached application window (read-only).
	inline const AppWindow* getAppWindow() const { return m_appWindow; }
    //! Get the attached application window.
    inline AppWindow* getAppWindow() { return m_appWindow; }

    //! Get bit per pixel (16,24,32...).
	inline UInt32 getBpp() const { return m_bpp; }
    //! Get depth size (16,24,32).
	inline UInt32 getDepth() const { return m_depth; }
    //! Get stencil size (0,8).
    inline UInt32 getStencil() const { return m_stencil; }
    //! Get the number of samplers for screen MSAA. 0 or 1 mean single sample.
    inline UInt32 getSamples() const { return m_samples; }

	//! Get information about the renderer.
	String getRendererName() const;
    //! Get OpenGL version of the renderer as string. Context must be current.
    String getStrVersion() const;
    //! Get OpenGL version enum. Valid after initialization of the renderer.
    inline Version getVersion() const { return m_version; }

	//! Is the renderer use of a shared renderer.
    inline Bool isSharing() const { return (m_sharing != nullptr); }

	//! Get the sharing renderer (valid if IsSharing() return true).
	inline Renderer* getSharing() const { return m_sharing; }

	//! Reference a share.
	inline void refShare() { m_shareCount++; }

	//! Dereference a share.
	void unrefShare();

	//! Is the renderer shared with another renderer(s).
	inline Bool isShared() const { return (m_shareCount > 0); }

	//! Count the number of time this renderer is shared.
	inline Int32 countShared() const { return m_shareCount; }

	//! Increment the renderer reference usage.
	void useIt();

	//! Decrement the renderer reference usage.
	void releaseIt();

	//! Check if the renderer is used by one or many Scene.
	inline Bool isUsed() const { return (m_refCount > 0); }

    //
    // Debugging (OGL 4.x+)
    //

    //! Is debugging enabled (only on debug context @see hasDebugSupport).
    inline Bool isDebug() const { return m_state.getBit(STATE_DEBUG_ON); }

    //! Set debugging (only on debug context @see hasDebugSupport).
    //! @param debug True mean active, False disable it but stay with a debug context.
    void setDebug(Bool debug = True);

    //! Defined the minimal captured (and logged) debug level.
    //! @param level Minimal debug level (default is DEBUG_TYPE_PERFORMANCE);
    void setDebugLevel(DebugLevel level);

    //! Get the current minimal captured debug level.
    inline DebugLevel getDebugLevel() const { return m_debugLevel; }

    //
	// Processing
    //

	//! Check for an error from OpenGL.
	//! The error string can be obtain using GetError().
	Bool isError();

	//! Get an error string from OpenGL.
	//! @return An empty string if no error.
	String getError() const;

	//! Is it the current OpenGL context.
	virtual Bool isCurrent() const;

	//! Set as current OpenGL context.
	virtual void setCurrent();

	//! Take a simple screen shot using the renderer color buffer.
	//! @param filename Output picture filename.
	//! @param format Picture file format.
	//! @param quality Picture compression quality (for JPEG).
	Bool screenShot(
		const String &filename,
		Image::FileFormat format,
		UInt32 quality = 255);

	//! Clear the color buffer.
	void clearColorBuffer();

	//! Clear the depth-buffer.
	void clearDepthBuffer();

	//! Clear the stencil-buffer.
	void clearStencilBuffer();

	//! Clear all three buffers (color, depth, stencil).
	void clearAll();

	//! Get the OpenGL context.
	Context* getContext();

	//! Get the OpenGL context (read only).
	const Context* getContext() const;

	//! Vertical refresh.
	virtual void setVerticalRefresh(Bool use);

	//! Is vertical refresh.
	virtual Bool isVerticalRefresh() const;

    //
    // Memory informations
    //

    /**
     * Returns the total amount of free memory for textures.
     * @return Size in kB or 0 if the query cannot be done.
     * @note On NVidia this value is the same for textures, VBO and render buffers.
     */
    UInt32 getTextureFreeMemory() const;

    /**
     * Returns the total amount of free memory for VBO.
     * @return Size in kB or 0 if the query cannot be done.
     * @note On NVidia this value is the same for textures, VBO and render buffers.
     */
    UInt32 getVBOFreeMemory() const;

    /**
     * Returns the total amount of free memory for render buffers.
     * @return Size in kB or 0 if the query cannot be done.
     * @note On NVidia this value is the same for textures, VBO and render buffers.
     */
    UInt32 getRenderBufferFreeMemory() const;

protected:

    enum States
    {
        STATE_DEFINED = 0,
        STATE_DEBUG_AVAILABLE = 1,
        STATE_DEBUG_ON = 2,
    };

    Int32 m_glErrno;          //!< OpenGL last error code.
	Version m_version;        //!< OpenGL renderer version.

	Renderer *m_sharing;      //!< The sharing renderer.
    Int32 m_shareCount;       //!< Greater than 0 mean the context is shared into another renderer.

	AppWindow *m_appWindow;   //!< Attached application window.

	_HDC   m_HDC;             //!< Handle device context (Drawable object).
	_HGLRC m_HGLRC;           //!< OpenGL handle on HDC.

    BitSet32 m_state;         //!< Current states
    DebugLevel m_debugLevel;  //!< Minimal captured debug level (default is DEBUG_TYPE_PERFORMANCE)

    UInt32 m_bpp;             //!< Color buffer size in bits (16,24,32...).
    UInt32 m_depth;           //!< Depth buffer size in bits (16,24,32).
    UInt32 m_stencil;         //!< Stencil buffer size in bits (0,8).
    UInt32 m_samples;         //!< Multi-sample number of samples (0,32).

    Int32 m_refCount;         //!< Reference counter.
	Context *m_glContext;     //!< OpenGL non-sharable variable states.

	//! Attach clear signal to the application window.
	void doAttachment(AppWindow *appWindow);

    void initDebug();
    void enableDebug();
    void disableDebug();
};

} // namespace o3d

#endif // _O3D_RENDERER_H
