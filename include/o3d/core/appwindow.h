/**
 * @file appwindow.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_APPWINDOW_H
#define _O3D_APPWINDOW_H

#include "debug.h"
#include "evt.h"
#include "inputmanager.h"
#include "keyboard.h"
#include "mouse.h"
#include "touchscreen.h"
#include "objects.h"
#include "memorydbg.h"
#include "vector2.h"

#include "../image/image.h"

#include <vector>

namespace o3d {

class Image;

/**
 * @brief A native window.
 * @date 2004-12-12
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class O3D_API AppWindow : public EvtHandler, NonCopyable<>
{
public:

    //! Possibles color formats.
    enum ColorFormats
    {
        COLOR_RGBA4,
        COLOR_RGB5A1,
        COLOR_RGB565 = 16,  //!< 5/6/5 bits RGB.
        COLOR_RGB8 = 24,    //!< 8 bits channel RGB.
        COLOR_RGBA8 = 32,   //!< 8 bits channel RGBA (default).
        COLOR_RGB16 = 48,   //!< 16 bits channel RGB.
        COLOR_RGBA16 = 64,  //!< 16 bits channel RGBA.
        COLOR_RGB32 = 96,   //!< 32 bits channel RGB.
        COLOR_RGBA32 = 128  //!< 32 bits channel RGBA.
    };

    //! Possible depth/stencil combinations.
    enum DepthStencilFormats
    {
        DEPTH_16 = 16,           //!< 16 bits depth, no stencil buffer.
        DEPTH_16_STENCIL_8,      //!< 16 bits depth, 8 bits stencil buffer.
        DEPTH_24_STENCIL_8 = 24, //!< 24 bits depth, 8 bits stencil buffer (default).
        DEPTH_32 = 32            //!< 32 bits depth, no stencil buffer.
    };

    //! Possible multi sample values.
    enum MultiSamples
    {
        NO_MSAA = 0,     //!< No anti-aliasing.
        MSAA2X = 2,      //!< Multi-sample anti-aliasing 2x.
        MSAA4X = 4,      //!< Multi-sample anti-aliasing 4x.
        MSAA8X = 8,      //!< Multi-sample anti-aliasing 8x.
        MSAA16X = 16     //!< Multi-sample anti-aliasing 16x.
    };

	//! Default constructor.
	AppWindow();

	//! Virtual destructor.
	virtual ~AppWindow();

	//! Define/change the window title.
	void setTitle(const String &title);
	//! Get window title.
	inline String getTitle() const { return m_title; }

    //! Get the window identifier.
    //! @note HWND on WIN32
    //! @note Window on X11
    //! @note windowID on SDL
	inline _HWND getHWND() const { return m_hWnd; }

    //! Get the related pixel format.
    //! @note PixelFormat on WIN32
    //! @note best GLX frame buffer on X11
    //! @note Uint32 pixel format on SDL
    //! @note EGLConfig on EFL
	inline _PF getPixelFormat() const { return m_PF; }

    //! Get the related HDC.
    //! @note HDC on WIN32
    //! @note Drawable on GLX
    //! @note SDL_Window on SDL
    //! @note EGLSurface on EGL
	inline _HDC getHDC() const { return m_HDC; }

    //! Get the related input context.
    //! @note null on WIN32
    //! @note XIM on X11
    //! @note null on SDL
	inline void* getIC() const { return m_ic; }

	//! Define the window icon filename or empty string to remove the icon.
	void setIcon(const String &icon);

	//! Define the window icon.
	//! @param icon A image to use as icon. If the picture is not valid the icon is removed.
	//! If the image is 24 bits color, it will use the color mask for transparency color.
	void setIcon(const Image &icon);
	//! Get the window icon image.
	inline const Image& getIcon() const { return m_icon; }

	//! Get input manager instance
	inline InputManager& getInput() { return m_inputManager; }
	//! get input manager instance (const version)
	inline const InputManager& getInput() const { return m_inputManager; }

    /**
     * @brief create Create and show a centered window that can be used by an OpenGL renderer.
     * @param clientWidth Width of the window client area in pixels.
     * @param clientHeight Height of the window client area in pixels.
     * @param colorFormat Bits per pixels (mostly 32 bits : COLOR_RGBA8).
     * @param depthFormat Depth buffer bits (mostly 24 bits, with a 8 bit stencil : DEPTH_24_STENCIL_8).
     * @param fullScreen If TRUE create a popup window taking the primary full screen area.
     * @param resizable If TRUE the window can be resized, otherwise it is fixed size.
     */
    void create(Int32 clientWidth,
        Int32 clientHeight,
        ColorFormats colorFormat = COLOR_RGBA8,
        DepthStencilFormats depthFormat = DEPTH_24_STENCIL_8,
        MultiSamples msaa = NO_MSAA,
        Bool fullScreen = False,
        Bool resizable = False);

    //! Is the window parameters are defined.
    Bool isSet() const;

	//! Set the window for full screen style (border less).
	//! @param fullScreen Fullscreen or windowed mode.
	//! @param freq Display frequency in hertz or 0 to use default.
	void setFullScreen(Bool fullScreen, UInt32 freq = 0);
	//! Is the window is border less (full screen style).
	Bool isFullScreen() const;

    //! Destroy the window immediately (prefere @see terminate).
	void destroy();

    //! Is the window running.
	inline Bool isRunning() const { return m_running; }

    //! Send a terminaison message. At next main loop the window will be destroyed.
	void terminate();

	//-------------------------------------------------------------------------------
	// Size and pos
	//-------------------------------------------------------------------------------

	//! Resize the window client area in pixels once the window is created.
	void resize(Int32 clientWidth, Int32 clientHeight);

	//! Get current window width
	inline Int32 getWidth() const { return m_width; }
	//! Get current window height
	inline Int32 getHeight() const { return m_height; }
	//! Get window size with the non client area
	Vector2i getSize() const { return Vector2i(m_width, m_height); }

	//! Get current client window width
	inline Int32 getClientWidth() const { return m_clientWidth; }
	//! Get current client window height
	inline Int32 getClientHeight() const { return m_clientHeight; }
	//! Get window client area size
	Vector2i getClientSize() const { return Vector2i(m_clientWidth, m_clientHeight); }

	//! Get the window position X.
	inline Int32 getPositionX() const { return m_posX; }
	//! Get the window position Y.
	inline Int32 getPositionY() const { return m_posY; }
	//! Get the window position.
	Vector2i getPos() const { return Vector2i(m_posX, m_posY); }

	/**
	 * Define the window minimal size hint (default is 0, 0).
	 * @note A call to resize must follow this setter in case of a minimal size greater
	 * than current size.
	 */
	void setMinSize(const Vector2i &minSize);
	//! Get the window minimal size.
	const Vector2i& getMinSize() const { return m_minSize; }

	/**
	 * Define the window maximal size (default is -1, -1, meaning no limits).
	 * @note A call to resize must follow this setter in case of a maximal size lesser
	 * than current size.
	 */
	void setMaxSize(const Vector2i &maxSize);
	//! Get the window maximal size.
	const Vector2i& getMaxSize() const { return m_maxSize; }

	//-------------------------------------------------------------------------------
	// Buffers parameters
	//-------------------------------------------------------------------------------/

    //! Get the number of bits per pixel (16, 24, 32...).
    UInt32 getBpp() const;

    //! Get the number of bits of the depth buffer (16, 24, 32).
    UInt32 getDepth() const;

    //! Get the number of bits of the stencil buffer (0, 8).
    UInt32 getStencil() const;

    //! Get the number of sample in case of screen MSSA [0..32].
    UInt32 getSamples() const;

    //! Get the color format.
    inline ColorFormats getColorFormat() const { return m_colorFormat; }

    //! Get the depth/stencil format.
    inline DepthStencilFormats getDepthStencilFormat() const { return m_depthStencilFormat; }

    //! Get the screen MSAA format.
    inline MultiSamples getMultiSamples() const { return m_samples; }

	//-------------------------------------------------------------------------------
	// Inputs
	//-------------------------------------------------------------------------------

	//! Grab/ungrab the mouse.
    //! Infinite mouse area using raw input on O3D_WINAPI.
	//! In grab mode you can benefit of the mouse smoother and internal mouse acceleration.
	//! @note The window must be initialized.
	void grabMouse(Bool lock = True);
	//! Is the mouse grabbed.
	Bool isMouseGrabbed() const;

	//! Grab/ungrab the keyboard. Sysapp key are handled by the window,
	//! and performances are improved.
	//! @note The window must be initialized.
	void grabKeyboard(Bool lock = True);
	//! Is the keyboard grabbed.
	Bool isKeyboardGrabbed() const;

    //-------------------------------------------------------------------------------
    // Clipboard
    //-------------------------------------------------------------------------------

    //! Paste a string content into the clipboard.
    //! @param primary Default is False, meaning use of the clipboard selection.
    //! @return True if the paste success.
    Bool pasteToClipboard(const String &text, Bool primary=False);

    //! Get a string content from the clipboard.
    //! @param primary Default is False, meaning use of the clipboard selection.
    //! @return A string containing the text from the clipboard or null string if the
    //! clipboard is empty of text.
    String copyFromClipboard(Bool primary=False);

	//-------------------------------------------------------------------------------
	// Timers and FPS
	//-------------------------------------------------------------------------------

    //! Set the update frequency in ms (default 120 fps).
    inline void setUpdateFreq(UInt32 freq = 8) { m_updateFreq = freq; }
	//! Get the update frequency in ms.
	inline UInt32 getUpdateFreq() const { return m_updateFreq; }
	
	//! Set the paint frequency in ms (default 60 fps).
	inline void setPaintFreq(UInt32 freq = 16) { m_paintFreq = freq; }
	//! Get the paint frequency in ms.
	inline UInt32 getPaintFreq() const { return m_paintFreq; }

	//! Get the last paint event duration in seconds.
	inline Float getPaintDuration() const { return m_lastPaintDuration; }

    //! Set the getLastFps() refresh frequency per second (2 is default).
	inline void setFpsSpeed(Float speed) { m_timeToFps = speed; }
    //! Get the getLastFps() refresh frequency per second (2 is default).
	inline Float getFpsSpeed() const { return m_timeToFps; }

	//! Get the last out FPS.
	Float getLastFps();

	//! Get the average FPS.
	Float getAverageFps() const;

	//! Log information about the last FPS.
	void logFps();

	//! Report FPS statistics.
	String report() const;

	//-------------------------------------------------------------------------------
	// Virtual callback default behaviors.
	//-------------------------------------------------------------------------------

    /**
     * @brief callBackKey Key pressed or released virtual callback.
     * @param key key Virtual key code.
     * @param key key Virtual key code of the compound character.
     * @param pressed pressed TRUE if pressed, FALSE if released.
     * @param repeat repeat TRUE if key repetition. That's mean pressed is FALSE.
     */
    virtual void callBackKey(VKey key, VKey character, Bool pressed, Bool repeat);

    /**
     * @brief callBackCharacter Character compound virtual callback.
     * @param key key Virtual key code.
     * @param key key Virtual key code of the compound character.
     * @param unicode unicode Unicode character.
     * @param repeat repeat TRUE if key repetition.
     */
    virtual void callBackCharacter(VKey key, VKey character, WChar unicode, Bool repeat);

	//! Mouse motion virtual callback.
	virtual void callBackMouseMotion();
	//! Mouse wheel rotation virtual callback.
	virtual void callBackMouseWheel();
	//! Mouse button state change virtual callback.
	virtual void callBackMouseButton(
		Mouse::Buttons button,
		Bool pressed,
		Bool dblClick);

    //! Touch-screen motion virtual callback.
    virtual void callBackTouchScreenMotion();
    //! Touch-screen change virtual callback.
    virtual void callBackTouchScreenChange();

	virtual void callBackCreate();
	virtual Int32 callBackPaint(void*);
	virtual void callBackMove();
	virtual void callBackMaximize();
	virtual void callBackMinimize();
	virtual void callBackRestore();
	virtual void callBackResize();
	virtual void callBackClose();
	virtual void callBackDestroy();
	virtual void callBackClear();
	virtual Int32 callBackUpdate(void*);
	virtual void callBackDraw();
	virtual void callBackDisplay();
	virtual void callBackFocus();
	virtual void callBackLostFocus();
	virtual void callBackMouseGain();
	virtual void callBackMouseLost();

public:

	//! Keyboard key state change.
    Signal<Keyboard*, KeyEvent> onKey{this}; // keyboard, key event
	//! Keyboard key state change.
    Signal<Keyboard*, CharacterEvent> onCharacter{this}; // keyboard, character event

	//! Mouse motion.
    Signal<Mouse*> onMouseMotion{this}; // mouse
	//! Mouse wheel rotation.
    Signal<Mouse*> onMouseWheel{this}; // mouse
	//! Mouse button state change.
    Signal<Mouse*, ButtonEvent> onMouseButton{this}; // mouse, button event

    //! Touch-screen motion.
    Signal<TouchScreen*> onTouchScreenMotion{this}; // touch screen
    //! Touch-screen change.
    Signal<TouchScreen*, TouchScreenEvent> onTouchScreenChange{this}; // mouse, touch event

	//! Window create
    Signal<> onCreate{this};
	//! Window close button pressed
    Signal<> onClose{this};
	//! Window destroy
    Signal<> onDestroy{this};

	//! Window move
    Signal<UInt32, UInt32> onMove{this}; // x, y
	//! Window maximize
    Signal<UInt32, UInt32> onMaximize{this}; // width, height
	//! Window minimize
    Signal<UInt32, UInt32> onMinimize{this}; // width, height (0,0)
	//! Window restore
    Signal<UInt32, UInt32> onRestore{this}; // width, height
	//! Window resize
    Signal<UInt32, UInt32> onResize{this}; // width, height

	//! Update content
    Signal<> onUpdate{this};
	//! Clear content
    Signal<> onClear{this};
	//! Draw content
    Signal<> onDraw{this};
	//! Swap video buffers
    Signal<> onDisplay{this};

	//! Keyboard focus gained
    Signal<> onFocus{this};
	//! Keyboard focus lost
    Signal<> onLostFocus{this};

	//! Mouse gain the window
    Signal<> onMouseGain{this};
	//! Mouse lost the window
    Signal<> onMouseLost{this};

public:

    //! Internal event type.
	enum EventType
	{
		EVT_CREATE,
		EVT_CLOSE,
		EVT_DESTROY,
		EVT_MAXIMIZE,
		EVT_MINIMIZE,
		EVT_RESTORE,
		EVT_RESIZE,
		EVT_MOVE,
		EVT_MOUSE_GAIN,
		EVT_MOUSE_LOST,
		EVT_INPUT_FOCUS_GAIN,
		EVT_INPUT_FOCUS_LOST,
		EVT_UPDATE,
		EVT_PAINT,
		EVT_KEYDOWN,
		EVT_KEYUP,
		EVT_CHARDOWN,
		EVT_CHARUP,
		EVT_MOUSE_BUTTON_DOWN,
		EVT_MOUSE_BUTTON_UP,
		EVT_MOUSE_MOTION,
        EVT_MOUSE_WHEEL,
        EVT_TOUCH_DOWN,
        EVT_TOUCH_POINTER_DOWN,
        EVT_TOUCH_MOVE,
        EVT_TOUCH_POINTER_UP,
        EVT_TOUCH_UP
	};

    //! Internal event struct.
	struct EventData
	{
        Float fx, fy;
		Int32 x, y;
		Int32 w, h;
		UInt32 button;
		UInt32 unicode;
		UInt32 key;
        UInt32 character;
        Int64 time;
	};

    //! Internally process a window event.
	void processEvent(EventType eventType, EventData &eventData);

protected:

	//! Number of frame time to record to compute an FPS.
	static const UInt32 FPS_INTERVAL_SIZE = 20;
	//! Number of FPS to record to done the ReportLog.
	static const UInt32 FPS_MAX_INTERVAL = 60;
	//! Default GetFPS frequency refresh (2x by second).
	static const Int32 FPS_GETFPS_SPEED = 2;

	//! Set width and height without changing real window size, and recompute client size.
	//! @see Resize to perform a window resize.
	void setSize(Int32 width, Int32 height);

	//! Apply window settings.
	//! @param fullScreen Create in full screen mode if TRUE.
	void applySettings(Bool fullScreen);

	//! Initialize inputs.
	void initInputs();

	//! Swap buffers.
	void swapBuffers();

	//! Compute FPS.
	void computeFPS();

	//! Is it need to paint.
	Bool isPaintNeeded();

	//! Is it need to update.
	Bool isUpdateNeeded();

	String m_title;        //!< Title of the window.
	Image m_icon;          //!< Icon image.

	Vector2i m_minSize;    //!< Window hint min size
	Vector2i m_maxSize;    //!< Window hint max size

	Int32 m_width;        //!< Width in pixels.
	Int32 m_height;       //!< Height in pixels.
	Int32 m_freq;         //!< Frequency for the displayer.

    ColorFormats m_colorFormat;
    DepthStencilFormats m_depthStencilFormat;

    MultiSamples m_samples; //!< For multisampling (MSAA) more than 1. 0 or 1 mean SS no AA.

	Int32 m_clientWidth;   //!< Client display area width in pixels.
	Int32 m_clientHeight;  //!< Client display area height in pixels.

	Int32 m_posX;         //!< Absolute X position.
	Int32 m_posY;         //!< Absolute Y position.

	Bool m_resizable;     //!< Is the window re-sizable.

	InputManager m_inputManager; //!< input manager instance

    _HWND m_hWnd;         //!< window handle
    _HDC m_HDC;           //!< drawable handle
    _PF m_PF;             //!< pixel format
    void *m_ic;           //!< input controller

	Bool m_hasMouse;      //!< Mouse inside the client area.
	Bool m_hasFocus;      //!< Has keyboard focus.

	Int32 m_captureMouse; //!< Windows only, count for SetCapture/ReleaseCapture.

	Bool m_running;

	Int64 m_lastUpdate;
	Int64 m_lastPaint;
	
	UInt32 m_updateFreq;
	UInt32 m_paintFreq;
	
	//
	// related to frame rate
	//

	Float m_lastPaintDuration;  //!< Last paint event duration.

	Float m_framesList[FPS_MAX_INTERVAL];  //!< Last computed frames duration.

	UInt32 m_interval;		//!< Number of frame elapsed in the interval.

	UInt32 m_frame;			//!< Number of elapsed frame.
	Int64 m_lastTime;		//!< Time since the last frame.

	Int64 m_totalTime;		//!< Time since initialization.
	UInt64 m_totalFrame;    //!< Number of rendered frame.

	Float m_timeToFps;      //!< delay to wait before send the last FPS
	Int64 m_lastFpsOut;     //!< time of the last FPS out
	Float m_lastFps;        //!< the last FPS to return (update at m_timeToFps speed)

#ifdef O3D_WINAPI
	static LRESULT CALLBACK wndProc(HWND hWnd, UInt32 Msg, WPARAM wParam, LPARAM lParam);
#endif
};

} // namespace o3d

#endif // _O3D_APPWINDOW_H
