/**
 * @file appwindow.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/appwindow.h"

#include "o3d/core/architecture.h"
#include "o3d/core/wintools.h"
#include "o3d/core/application.h"
#include "o3d/core/display.h"

using namespace o3d;

// Constructor
AppWindow::AppWindow() :
	m_title("Objective-3D"),
	m_icon(),
	m_minSize(0, 0),
	m_maxSize(-1, -1),
	m_width(-1),
	m_height(-1),
	m_freq(0),
    m_colorFormat((ColorFormats)-1),
    m_depthStencilFormat((DepthStencilFormats)-1),
    m_samples(NO_MSAA),
	m_clientWidth(-1),
	m_clientHeight(-1),
	m_posX(-1),
	m_posY(-1),
	m_resizable(False),
	m_hWnd(NULL_HWND),
	m_HDC(NULL_HDC),
	m_PF(NULL_PF),
	m_hasMouse(False),
	m_hasFocus(False),
	m_captureMouse(0),
	m_running(False),
	m_lastUpdate(0),
	m_lastPaint(0),
    m_updateFreq(8),
	m_paintFreq(16),
	// related to frame rate
	m_lastPaintDuration(0.0f),
	m_interval(0),
	m_frame(0),
	m_lastTime(0),
	m_totalTime(0),
	m_totalFrame(0),
	m_timeToFps(1.0f / FPS_GETFPS_SPEED),
	m_lastFpsOut(0),
	m_lastFps(0.f)
{
}

// Destructor
AppWindow::~AppWindow()
{
	// delete the window
	destroy();
}

// Terminate the window.
void AppWindow::terminate()
{
    EventData event;
    processEvent(EVT_DESTROY, event);
}

// Create and show a centered window that can be used by an OpenGL renderer.
void AppWindow::create(Int32 width,
    Int32 height,
    ColorFormats colorFormat,
    DepthStencilFormats depthFormat,
    MultiSamples msaa,
    Bool fullScreen,
    Bool resizable)
{
    if (m_hWnd) {
		O3D_ERROR(E_InvalidOperation("The window already exists"));
    }

	m_width  = m_clientWidth = width;
	m_height = m_clientHeight = height;

	m_resizable = resizable;
	m_running = False;

    m_colorFormat = colorFormat;
    m_depthStencilFormat = depthFormat;
    m_samples = msaa;

	m_hasMouse = m_hasFocus = False;

	applySettings(fullScreen);
}

Bool AppWindow::isSet() const
{
    return ((m_clientWidth != -1) &&
            (m_clientHeight != -1) &&
            (m_colorFormat != (ColorFormats)-1)  &&
            (m_depthStencilFormat != (DepthStencilFormats)-1));
}

UInt32 AppWindow::getBpp() const
{
    switch (m_colorFormat) {
        case COLOR_RGBA4:
        case COLOR_RGB5A1:
        case COLOR_RGB565:
            return 16;
        case COLOR_RGB8:
            return 3*8;
        case COLOR_RGBA8:
            return 4*8;
        case COLOR_RGB16:
            return 3*16;
        case COLOR_RGBA16:
            return 4*16;
        case COLOR_RGB32:
            return 3*32;
        case COLOR_RGBA32:
            return 4*32;
        default:
            return 0;
    }
}

UInt32 AppWindow::getDepth() const
{
    switch (m_depthStencilFormat) {
        case DEPTH_16:
        case DEPTH_16_STENCIL_8:
            return 16;
        case DEPTH_24_STENCIL_8:
            return 24;
        case DEPTH_32:
            return 32;
        default:
            return 0;
    }
}

UInt32 AppWindow::getStencil() const
{
    switch (m_depthStencilFormat) {
        case DEPTH_16:
        case DEPTH_32:
            return 0;
        case DEPTH_16_STENCIL_8:
        case DEPTH_24_STENCIL_8:
            return 8;
        default:
            return 0;
    }
}

UInt32 AppWindow::getSamples() const
{
    return (UInt32)m_samples;
}

// init inputs
void AppWindow::initInputs()
{
	m_inputManager.initInput(this);
	m_inputManager.initKeyboard();
	m_inputManager.initMouse(m_width, m_height);
}

// Is the mouse locked.
Bool AppWindow::isMouseGrabbed() const
{
	return m_inputManager.isMouse() && m_inputManager.getMouse()->isGrabbed();
}

// Is the mouse locked.
Bool AppWindow::isKeyboardGrabbed() const
{
	return m_inputManager.isKeyboard() && m_inputManager.getKeyboard()->isGrabbed();
}

// Lock/unlock the mouse. Infinite mouse area using raw input.
void AppWindow::grabMouse(Bool lock)
{
    if (!m_hWnd) {
		O3D_ERROR(E_InvalidOperation("The window is not valid"));
    }

    if (!m_inputManager.isMouse()) {
		O3D_ERROR(E_InvalidOperation("The mouse is not valid"));
    }

	m_inputManager.getMouse()->setGrab(lock);
}

// Lock/unlock the keyboard.
void AppWindow::grabKeyboard(Bool lock)
{
    if (!m_hWnd) {
		O3D_ERROR(E_InvalidOperation("The window is not valid"));
    }

    if (!m_inputManager.isKeyboard()) {
		O3D_ERROR(E_InvalidOperation("The keyboard is not valid"));
    }

	m_inputManager.getKeyboard()->setGrab(lock);
}

// Is the window is border less (full screen style).
Bool AppWindow::isFullScreen() const
{
    return Display::instance()->getAppWindow() == this;
}

// Set the window icon.
void AppWindow::setIcon(const String &icon)
{
    if (icon.isValid()) {
		// set the icon
		Image newIcon(icon);
		setIcon(newIcon);
    } else {
		// remove the icon
		setIcon(Image());
	}
}

// Callback method
void AppWindow::callBackCreate()
{
	initInputs();

	onCreate();
}

void AppWindow::callBackKey(VKey key, VKey character, Bool pressed, Bool repeat)
{
	Keyboard* pKeyboard = m_inputManager.getKeyboard();
	// DEFAULT
    if (pKeyboard) {
        KeyEvent event(key, character, pressed, repeat);
		onKey(pKeyboard, event);
	}
}

void AppWindow::callBackCharacter(VKey key, VKey character, WChar unicode, Bool repeat)
{
	Keyboard* pKeyboard = m_inputManager.getKeyboard();
	// DEFAULT
    if (pKeyboard) {
        CharacterEvent event(key, character, unicode, repeat);
		onCharacter(pKeyboard, event);
	}
}

void AppWindow::callBackMouseMotion()
{
	Mouse* pMouse = m_inputManager.getMouse();
	// DEFAULT
	if (pMouse)	{
		onMouseMotion(pMouse);
	}
}

void AppWindow::callBackMouseWheel()
{
	Mouse* pMouse = m_inputManager.getMouse();
	// DEFAULT
	if (pMouse)	{
		onMouseWheel(pMouse);
	}
}

void AppWindow::callBackMouseButton(
	Mouse::Buttons button,
	Bool pressed,
	Bool dblClick)
{
	Mouse* pMouse = m_inputManager.getMouse();
	// DEFAULT
    if (pMouse) {
		ButtonEvent event(button, pressed, dblClick);
		onMouseButton(pMouse, event);
	}
}

void AppWindow::callBackClose()
{
	// DEFAULT
	onClose();
}

void AppWindow::callBackDestroy()
{
	// DEFAULT

	// delete inputs
    //m_inputManager.destroy();

	onDestroy();

	m_running = False;
}

void AppWindow::callBackMove()
{
	// DEFAULT
	onMove(UInt32(m_posX), UInt32(m_posY));
}

void AppWindow::callBackMaximize()
{
	// DEFAULT
	Mouse* mouse = m_inputManager.getMouse();
    if (mouse) {
		mouse->release();
		mouse->setMouseRegion(Box2i(0, 0, m_clientWidth, m_clientHeight));
		mouse->acquire();
		onMouseMotion(mouse);
	}

	onMaximize(UInt32(m_clientWidth), UInt32(m_clientHeight));
}

void AppWindow::callBackMinimize()
{
	// DEFAULT
	Mouse* mouse = m_inputManager.getMouse();
    if (mouse) {
		mouse->release();
		mouse->setMouseRegion(Box2i(0, 0, 0, 0));
		onMouseMotion(mouse);
	}

	onMinimize(0,0);
}

void AppWindow::callBackRestore()
{
	// DEFAULT
	Mouse* mouse = m_inputManager.getMouse();
    if (mouse) {
		mouse->release();
		mouse->setMouseRegion(Box2i(0, 0, m_clientWidth, m_clientHeight));
		mouse->acquire();
		onMouseMotion(mouse);
	}

	onRestore(UInt32(m_clientWidth), UInt32(m_clientHeight));
}

void AppWindow::callBackResize()
{
	// DEFAULT
	Mouse* mouse = m_inputManager.getMouse();
    if (mouse) {
		mouse->release();
        mouse->setMouseRegion(Box2i(0, 0, m_clientWidth, m_clientHeight));
		mouse->acquire();
		onMouseMotion(mouse);
	}

	onResize(UInt32(m_clientWidth), UInt32(m_clientHeight));
}

Int32 AppWindow::callBackPaint(void*)
{
	// DEFAULT
	TimeMesure mesure(m_lastPaintDuration);

    if (m_clientWidth && m_clientHeight) {
		callBackClear();
		callBackDraw();
		callBackDisplay();
    }

	computeFPS();

	return 0;
}

void AppWindow::callBackClear()
{
	// DEFAULT
	onClear();
}

Int32 AppWindow::callBackUpdate(void*)
{
	// DEFAULT
	m_inputManager.update();

	onUpdate();

	return 0;
}

void AppWindow::callBackDraw()
{
	// DEFAULT
	onDraw();
}

void AppWindow::callBackDisplay()
{
	// DEFAULT
	swapBuffers();
	onDisplay();
}

void AppWindow::callBackFocus()
{
	// DEFAULT acquire the keyboard and mouse
	m_inputManager.getKeyboard()->acquire();

	onFocus();
}

void AppWindow::callBackLostFocus()
{
	// DEFAULT release the keyboard and mouse
	m_inputManager.getKeyboard()->release();

	onLostFocus();
}

void AppWindow::callBackMouseGain()
{
	// DEFAULT acquire the mouse
	Mouse* pMouse = m_inputManager.getMouse();
    if (pMouse) {
		pMouse->acquire();
		pMouse->disableCursor();

		m_hasMouse = True;
	}
	onMouseGain();
}

void AppWindow::callBackMouseLost()
{
	// DEFAULT acquire the mouse
	Mouse* pMouse = m_inputManager.getMouse();
    if (pMouse) {
		pMouse->release();
		pMouse->enableCursor();

		m_hasMouse = False;
	}
    onMouseLost();
}

// Get the last FPS.
Float AppWindow::getLastFps()
{
	// first time: init
    if (m_lastFpsOut == 0) {
		m_lastFpsOut = System::getTime();
        if (m_interval != 0) {
			m_lastFps = m_framesList[m_interval-1];
        }
    } else {
		Int64 curTime = System::getTime();

		// update the FPS if time is elapsed
        if ( ((Float)(curTime - m_lastFpsOut) / (Float)System::getTimeFrequency()) >= m_timeToFps) {
			m_lastFpsOut = curTime;

            if (m_interval != 0) {
				m_lastFps = m_framesList[m_interval-1];
            }
		}
	}

	// return a recent FPS (FPS of the m_timeToFps(second) last frames)
	return m_lastFps;
}

// Compute FPS.
void AppWindow::computeFPS()
{
    if (m_frame == 0 && m_interval == 0) {
		// initialization
		m_totalTime = 0;
		m_totalFrame = 0;
		m_lastTime = System::getTime();
	}

	++m_frame;
	++m_totalFrame;

    if (m_frame == FPS_INTERVAL_SIZE) {
		Int64 time = System::getTime();
		m_totalTime += time - m_lastTime;

		m_framesList[m_interval] = (Float)System::getTimeFrequency() *
			((Float)FPS_INTERVAL_SIZE)/((Float)(time - m_lastTime));

		m_lastTime = time;
		++m_interval;
		m_frame = 0;

        if (m_interval == FPS_MAX_INTERVAL) {
			m_interval = 1;
			m_framesList[0] = m_framesList[FPS_MAX_INTERVAL-1];
		}
	}
}

// Log information about the last FPS.
void AppWindow::logFps()
{
    String str;

	str = "Last ";
	str.concat(m_interval);
	str += " frames :\n";

    for (UInt32 i = 0; i < m_interval; ++i) {
        str += String::print("    |- %i -> %.4f fps\n", i, m_framesList[i]);
	}

	O3D_MESSAGE(str);
}

// Report FPS statistics.
String AppWindow::report() const
{
	Float sum = 0;
	Float moy = 0;
	Float equart = 0;
	UInt32 i;

	String msg;

	msg = "Parameters : Interval size=";
	msg.concat(m_interval);
	msg += "\n\n";

    for (i = 0; i < m_interval; ++i) {
		msg += "Interval ";
		msg.concat(i);
		msg += " : FPS=";
		msg.concat(m_framesList[i]);
		msg += "\n";

		sum += m_framesList[i];
	}

    if (m_interval !=0) {
		moy = sum / m_interval;
    }

    for (i = 0; i < m_interval; ++i) {
		equart += (m_framesList[i] - moy) * (m_framesList[i] - moy);
	}

    if (m_interval != 0) {
        equart = Math::sqrt(equart/m_interval);
    }

	msg += "\n- Esperance=";
	msg.concat(moy);
	msg += "\n- Standard Deviation=";
	msg.concat(equart);

	return msg;
}

// Get the average FPS.
Float AppWindow::getAverageFps() const
{
    return ((Float)System::getTimeFrequency() * ((Float)m_totalFrame/(Float)m_totalTime));
}

// Is it need to paint.
Bool AppWindow::isPaintNeeded()
{
	Int64 time = System::getTime();

    if (m_lastPaint == 0) {
		m_lastPaint = time;
		return True;
	}

    if ((time - m_lastPaint) >= (m_paintFreq * System::getTimeFrequency()) / 1000) {
		m_lastPaint = time;
		return True;
	}
	
	return False;
}

// Is it need to update.
Bool AppWindow::isUpdateNeeded()
{
	Int64 time = System::getTime();

    if (m_lastUpdate == 0) {
		m_lastUpdate = time;
		return True;
	}

    if ((time - m_lastUpdate) >= (m_updateFreq * System::getTimeFrequency()) / 1000) {
		m_lastUpdate = time;
		return True;
	}
	
	return False;
}
