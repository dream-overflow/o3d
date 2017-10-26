/**
 * @file gui.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/fontmanager.h"
#include "o3d/gui/gui.h"

#include "o3d/gui/widgetmanager.h"
#include "o3d/gui/thememanager.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/appwindow.h"
#include "o3d/engine/texture/texture.h"
#include "o3d/engine/renderer.h"
#include "o3d/engine/screenviewport.h"
#include "o3d/engine/viewportmanager.h"
#include "o3d/core/string.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/context.h"
#include "o3d/engine/framebuffer.h"
#include "o3d/gui/i18n.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Gui, GUI_GUI, SceneEntity)

class WinManagerViewPort : public ScreenViewPort
{
public:

	WinManagerViewPort(BaseObject *parent, Camera *camera, WidgetManager *zOrder) :
		ScreenViewPort(parent, camera),
		m_widgetManager(zOrder)
	{
	}

	virtual void draw()
	{
		// draw recursively and Z orderer all widgets
		m_widgetManager->draw();
	}

	virtual void drawgetPicking()
	{

	}

private:

	WidgetManager *m_widgetManager;
};

// Default constructor
Gui::Gui(BaseObject *parent) :
	SceneEntity(parent),
	m_i18n(nullptr),
    m_fontManager(nullptr),
    m_camera(nullptr),
    m_viewPort(nullptr),
    m_widgetManager(nullptr),
    m_themeManager(nullptr),
    m_FBOPooler(nullptr),
    m_oldMouseButton(),
	m_oldMousePos(-1, -1)
{
	m_name          = "Scene::Gui";
	m_i18n          = new I18n();

	m_fontManager   = new FontManager(this);
	m_camera        = new Camera(this);

	m_camera->setName("<o3d::winmanager::camera>");
	m_camera->setPersistant(True);

	m_widgetManager = new WidgetManager(this);
	m_themeManager  = new ThemeManager(this);
	m_FBOPooler     = new T_FBOPooler;

	m_viewPort = new WinManagerViewPort(this, m_camera, m_widgetManager);
	m_viewPort->setName("<o3d::winmanager::viewport>");
	m_viewPort->setPersistant(True);

    getScene()->getViewPortManager()->addViewPort(m_viewPort, Limits<Int32>::max());

	reshape(
		getScene()->getViewPortManager()->getReshapeWidth(),
		getScene()->getViewPortManager()->getReshapeHeight());
}

//---------------------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------------------
Gui::~Gui()
{
	// delete all FBO
	for (IT_FBOPooler it = m_FBOPooler->begin(); it != m_FBOPooler->end(); ++it)
		deletePtr((*it));

	if (m_viewPort)
	{
		m_viewPort->setPersistant(False);
		getScene()->getViewPortManager()->deleteViewPort(m_viewPort);
	}

	deletePtr(m_camera);

	deletePtr(m_FBOPooler);
	deletePtr(m_widgetManager);
	deletePtr(m_themeManager);
	deletePtr(m_fontManager);
    deletePtr(m_i18n);
}

void Gui::init()
{
    m_widgetManager->init();
}

void Gui::release()
{
    // nothing
}

std::list<FrameBuffer*>& Gui::getFBOPooler()
{
	return *m_FBOPooler;
}

// return an fbo for the given size and pixel format. Create a new one if none existing
FrameBuffer* Gui::findFBO(
	UInt32 width,
	UInt32 height,
	PixelFormat pixelFormat)
{
	if (!m_FBOPooler)
        return nullptr;

	for (IT_FBOPooler it = m_FBOPooler->begin(); it != m_FBOPooler->end(); ++it)
	{
        if (((UInt32)(*it)->getDimension().x() == width) &&
            ((UInt32)(*it)->getDimension().y() == height) &&
			((*it)->getColorFormat() == pixelFormat))
		{
			return *it;
		}
	}

	// create a new one
    FrameBuffer *pFBO = new FrameBuffer(getScene()->getContext());
	pFBO->create(width, height, pixelFormat);

	m_FBOPooler->push_back(pFBO);

	return pFBO;
}

String Gui::tr(const String &key) const
{
	return getI18n()->get(key);
}

String Gui::tr(const String &key, const String &_1) const
{
	return getI18n()->get(key, _1);
}

String Gui::tr(const String &key, const String &_1, const String &_2) const
{
	return getI18n()->get(key, _1, _2);
}

String Gui::tr(const String &key, const String &_1, const String &_2, const String &_3) const
{
	return getI18n()->get(key, _1, _2, _3);
}

ViewPort* Gui::getViewPort()
{
    return m_viewPort;
}

const ViewPort* Gui::getViewPort() const
{
	return m_viewPort;
}

// Change the winmanager viewport priority (default is O3D_MAX_INT32)
void Gui::changeViewPortPriority(Int32 newPriority)
{
	if (m_viewPort)
		getScene()->getViewPortManager()->changePriority(m_viewPort, newPriority);
}

// reset the winmanager viewport to its default parameters
void Gui::resetViewPort()
{
	if (m_viewPort && m_camera)
	{
		// default priority
		changeViewPortPriority(o3d::Limits<Int32>::max());

		// parameters
		m_viewPort->enable();
		m_viewPort->enablePercent();
		m_viewPort->setSize(0.f,0.f,1.f,1.f);
		m_viewPort->setName("<o3d::winmanager::viewport>");

		if (m_viewPort->getCamera() != m_camera)
			m_viewPort->setCamera(m_camera);
	}
}

// default drawing method for render a GUI
void Gui::draw()
{
	// call the current viewport callback
	if (m_viewPort && m_camera)
	{
		getScene()->getContext()->modelView().identity();

		m_viewPort->display(
			getScene()->getViewPortManager()->getReshapeWidth(),
			getScene()->getViewPortManager()->getReshapeHeight());
	}
}

// update some objects on ZOrder
void Gui::update()
{
	m_widgetManager->update();
	m_themeManager->update();
	m_fontManager->update();
}

// when a keyboard event or mouse event occurred send it with this methods
void Gui::postKeyEvent(Keyboard *keyboard, KeyEvent event)
{
	if (m_widgetManager)
        m_widgetManager->keyboardToggled(keyboard, event);
}

void Gui::postCharacterEvent(Keyboard* keyboard, CharacterEvent event)
{
	if (m_widgetManager)
        m_widgetManager->character(keyboard, event);
}

void Gui::postMouseMotionEvent(Mouse *mouse)
{
	if (!m_widgetManager)
		return;

	Int32 x = mouse->getMappedPosition().x();
	Int32 y = mouse->getMappedPosition().y();

	if ((x != m_oldMousePos.x()) || (y != m_oldMousePos.y()))
	{
		m_widgetManager->mouseMove(x, y);
		m_oldMousePos.set(x, y);
	}
}

void Gui::postMouseButtonEvent(Mouse *mouse, ButtonEvent event)
{
	if (!m_widgetManager)
		return;

	Int32 x = mouse->getMappedPosition().x();
	Int32 y = mouse->getMappedPosition().y();

    if (event.isDown() && !m_oldMouseButton[event.button()])
    {
        m_oldMouseButton.enable(event.button());
        m_widgetManager->mouseButtonDown(event.button(), x, y);
    }
    else if (event.isUp() && m_oldMouseButton[event.button()])
	{
        m_oldMouseButton.disable(event.button());
        m_widgetManager->mouseButtonUp(event.button(), x, y);
	}
}

void Gui::postMouseWheelEvent(Mouse *mouse)
{
	if (!m_widgetManager)
		return;

	Int32 x = mouse->getMappedPosition().x();
	Int32 y = mouse->getMappedPosition().y();

	if (mouse->getWheelDelta() != 0)
		m_widgetManager->mouseWheel(x, y, mouse->getWheelDelta());
}

// set/get the screen resolution for the GUI and font
void Gui::reshape(UInt32 width, UInt32 height)
{
	m_camera->setOrtho(0,(Float)(width),(Float)(height),0);
	m_camera->setZnear(-1.f);
	m_camera->setZfar(1.f);
	m_camera->computeOrtho();

    m_widgetManager->reshape(width, height);
}

Int32 Gui::getWidth() const
{
	return (Int32)(m_camera->getRight());
}

Int32 Gui::getHeight() const
{
	return (Int32)(m_camera->getBottom());
}

void Gui::defaultAttachment(AppWindow *appWindow)
{
    if (!appWindow)
        return;

    // Inputs
    appWindow->onKey.connect(this, &Gui::postKeyEvent);
    appWindow->onCharacter.connect(this, &Gui::postCharacterEvent);
    appWindow->onMouseMotion.connect(this, &Gui::postMouseMotionEvent);
    appWindow->onMouseButton.connect(this, &Gui::postMouseButtonEvent);
    appWindow->onMouseWheel.connect(this, &Gui::postMouseWheelEvent);

    // synchronize reshape
    reshape(appWindow->getClientWidth(), appWindow->getClientHeight());
}

/*---------------------------------------------------------------------------------------
  Import/export
---------------------------------------------------------------------------------------*/

Bool Gui::importGui(const String &filename)
{
    InStream *is = FileManager::instance()->openInStream(filename);

	Bool ret;

	try
	{
        ret = importGui(*is);
	}
	catch(E_BaseException &)
	{
        deletePtr(is);
		throw;
	}

    deletePtr(is);
	return ret;
}

Bool Gui::importGui(InStream &is)
{
    return True;
}

