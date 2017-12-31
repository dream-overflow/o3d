/**
 * @file widget.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/widget.h"
#include "o3d/gui/widgets/tooltip.h"
#include "o3d/gui/widgets/statictext.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/gui/widgetmanager.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"
#include "o3d/gui/i18n.h"
#include "o3d/engine/context.h"
#include "o3d/engine/framebuffer.h"
#include "o3d/engine/shader/shadermanager.h"
#include "o3d/gui/widgets/layout.h"

using namespace o3d;

const Vector2i Widget::DEFAULT_POS = Vector2i(-1,-1);
const Vector2i Widget::DEFAULT_SIZE = Vector2i(-1,-1);

O3D_IMPLEMENT_ABSTRACT_CLASS1(Widget, GUI_WIDGET, WidgetProperties)

// initialization constructor
Widget::Widget(BaseObject *parent) :
		WidgetProperties(parent),
		m_pos(DEFAULT_POS),
		m_size(DEFAULT_SIZE),
		m_minSize(DEFAULT_SIZE),
		m_maxSize(DEFAULT_SIZE),
		m_virtualSize(DEFAULT_SIZE),
		m_minVirtualSize(DEFAULT_SIZE),
		m_maxVirtualSize(DEFAULT_SIZE),
        m_theme(nullptr),
        m_layoutItem(nullptr),
        m_tabIndex(-1),
        m_texture(nullptr),
        m_shader(getScene()->getContext())
{
}

Widget::Widget(
	Widget *parent,
	const Vector2i &pos,
	const Vector2i &size,
	const String &name) :
		WidgetProperties(parent),
		m_pos(pos),
		m_size(size),
		m_minSize(size),
		m_maxSize(size),
		m_virtualSize(DEFAULT_SIZE),
		m_minVirtualSize(DEFAULT_SIZE),
		m_maxVirtualSize(DEFAULT_SIZE),
        m_theme(nullptr),
        m_layoutItem(nullptr),
        m_tabIndex(-1),
        m_texture(nullptr),
        m_shader(getScene()->getContext())
{
	m_name = name;
	init();
}

Widget::Widget(
	WidgetManager *parent,
	const Vector2i &pos,
	const Vector2i &size,
	const String &name) :
		WidgetProperties(parent),
		m_pos(pos),
		m_size(size),
		m_minSize(size),
		m_maxSize(size),
		m_virtualSize(DEFAULT_SIZE),
		m_minVirtualSize(DEFAULT_SIZE),
		m_maxVirtualSize(DEFAULT_SIZE),
        m_theme(nullptr),
        m_layoutItem(nullptr),
        m_tabIndex(-1),
        m_texture(nullptr),
        m_shader(getScene()->getContext())
{
	m_name = name;
	init();
}

Widget::WigdetShader::WigdetShader(Context *context) :
    vertices(context),
    texCoords(context),
    blendFunc(Blending::ONE__ONE_MINUS_SRC_A___ONE__ONE_MINUS_SRC_A)
{
}

void Widget::init()
{
    // default any widget have focus capacity
    m_capacities.enable(CAPS_FOCUSABLE);
    //m_capacities.disable(CAPS_LAYOUT_TYPE);
    //m_capacities.disable(CAPS_CONTAINER);

	m_texture.setParent(this);
	m_texture.setWrap(Texture::CLAMP);
	m_texture.setFiltering(Texture::NO_FILTERING);
	m_texture.setPersistant(True);

    Shader *shader = getScene()->getShaderManager()->addShader("gui/widgetSimple");
	shader->buildInstance(m_shader.shaderInstance);

    m_shader.shaderInstance.assign(0, 0, "ALPHA_TEST_REF=0.0;ALPHA_FUNC_GREATER;");
	m_shader.shaderInstance.build(Shader::BUILD_COMPILE_AND_LINK);

	m_shader.a_vertex = m_shader.shaderInstance.getAttributeLocation("a_vertex");
	m_shader.a_texCoords = m_shader.shaderInstance.getAttributeLocation("a_texCoords");

	m_shader.u_modelViewProjectionMatrix = m_shader.shaderInstance.getUniformLocation("u_modelViewProjectionMatrix");
	m_shader.u_alpha = m_shader.shaderInstance.getUniformLocation("u_alpha");
	m_shader.u_texture = m_shader.shaderInstance.getUniformLocation("u_texture");
}

// destructor
Widget::~Widget()
{
	deletePtr(m_layoutItem);

    // safe remove
    getGui()->getWidgetManager()->checkPrevTargetedWidget(this);
}

void Widget::updateTabIndex(Widget *widget)
{
    // nothing
}

Gui *Widget::getGui()
{
    return (Gui*)getScene()->getGui();
}

const Gui *Widget::getGui() const
{
    return (const Gui*)getScene()->getGui();
}

// Hierarchy management
const Widget* Widget::getTopLevelWidget() const
{
	// get the toplevel widget
	const Widget *topLevelWidget = this;
    while (topLevelWidget->m_parent != getGui()->getWidgetManager())
	{
		const Widget *parentWidget = o3d::dynamicCast<const Widget*>(topLevelWidget->m_parent);
		if (!parentWidget)
			break;
		else
			topLevelWidget = parentWidget;
	}

	return topLevelWidget;
}

Widget* Widget::getTopLevelWidget()
{
	// get the toplevel widget
	Widget *topLevelWidget = this;
    while (topLevelWidget->m_parent != getGui()->getWidgetManager())
	{
		Widget *parentWidget = o3d::dynamicCast<Widget*>(topLevelWidget->m_parent);
		if (!parentWidget)
			break;
		else
			topLevelWidget = parentWidget;
	}

	return topLevelWidget;
}

// retrieve the parent widget or WidgetManager has parent for TopLevelWidget
Widget* Widget::getParentWidget()
{
    if (m_parent == getGui()->getWidgetManager()) {
        return nullptr;
    }

	return o3d::dynamicCast<Widget*>(m_parent);
}

const Widget* Widget::getParentWidget() const
{
    if (m_parent == getGui()->getWidgetManager()) {
        return nullptr;
    }

	return o3d::dynamicCast<const Widget*>(m_parent);
}

// Size property
Vector2i Widget::getClientSize() const
{
	return m_size;
}

void Widget::setSize(Int32 width, Int32 height)
{
    setSize(-1, -1, width, height, SIZE_USE_EXISTING);
}

void Widget::setSize(Int32 x, Int32 y, Int32 width, Int32 height, Int32 mode)
{
	// get the current size and position
	Vector2i curSize = size();
    Vector2i curPos = pos();

	// nothing to do ?
	if ((x == curPos.x()) && (y == curPos.y()) &&
		(width == curSize.x()) && (height == curSize.y()) &&
		!(mode & SIZE_FORCE))
	{
		return;
	}

	if ((x == -1) && !(mode & SIZE_ALLOW_MINUS_ONE))
		x = curPos.x();

	if ((y == -1) && !(mode & SIZE_ALLOW_MINUS_ONE))
		y = curPos.y();

	if (!isTopLevelWidget() && !(mode & SIZE_NO_ADJUSTEMENT))
	{
//		Vector2i org = getParentWidget()->getClientAreaOrigin();
//
//		x += org.x();
//		y += org.y();
	}

	Vector2i size = DEFAULT_SIZE;
	if (width == -1)
	{
		if (mode & SIZE_AUTO_WIDTH)
		{
			size = getBestSize();
			width = size.x();
		}
		else
		{
			// just take the current one
			width = curSize.x();
		}
	}

	if (height == -1)
	{
		if (mode & SIZE_AUTO_HEIGHT)
		{
			if (size.x() == -1)
			{
				size = getBestSize();
			}
			//else { already called getBestSize() above }

			height = size.y();
		}
		else
		{
			// just take the current one
			height = curSize.y();
		}
	}

	// store new results
	m_pos.set(x,y);
	m_size.set(width,height);

	positionChanged();
	sizeChanged();
}

// Virtual widget size
void Widget::virtualSizeChanged(Int32 x, Int32 y)
{
	if (m_minVirtualSize.x() != -1 && m_minVirtualSize.x() > x)
		x = m_minVirtualSize.x();
	if (m_maxVirtualSize.x() != -1 && m_maxVirtualSize.x() < x)
		x = m_maxVirtualSize.x();
	if (m_minVirtualSize.y() != -1 && m_minVirtualSize.y() > y)
		y = m_minVirtualSize.y();
	if (m_maxVirtualSize.y() != -1 && m_maxVirtualSize.y() < y)
		y = m_maxVirtualSize.y();

	m_virtualSize.set(x,y);
}

Vector2i Widget::getVirtualSize() const
{
	// we should use the entire client area so if it is greater than our virtual size,
	// expand it to fit (otherwise if the window is big enough we wouldn't be using
	// parts of it)
	Vector2i size = getClientSize();
	if (m_virtualSize.x() > size.x())
		size.x() = m_virtualSize.x();

	if (m_virtualSize.y() > size.y())
		size.y() = m_virtualSize.y();

	return size;
}

// Set size hints
void Widget::setSizeHints(
	Int32 minW,
	Int32 minH,
	Int32 maxW,
	Int32 maxH,
	Int32 incW,
	Int32 incH)
{
	O3D_ASSERT(
		(minW == -1 || maxW == -1 || minW <= maxW) &&
		(minH == -1 || maxH == -1 || minH <= maxH));

	m_minSize.x() = minW;
	m_minSize.y() = minH;
	m_maxSize.x() = maxW;
	m_maxSize.y() = maxH;
}

// Set virtual size hints
void Widget::setVirtualSizeHints(Int32 minW, Int32 minH, Int32 maxW, Int32 maxH)
{
	m_minVirtualSize.x() = minW;
	m_minVirtualSize.y() = minH;
	m_maxVirtualSize.x() = maxW;
	m_maxVirtualSize.y() = maxH;
}

// Layout
void Widget::layout()
{
	if (getLayout())
	{
		Vector2i virtualSize(getVirtualSize());
		getLayout()->setDimension(Vector2i(0,0),virtualSize);
	}
}

Vector2i Widget::fit()
{
	if (getLayout() && getLayout()->getNumChildren())
        setSize(getBestSize());

	return m_size;
}

void Widget::fitInside()
{
	if (getLayout() && getLayout()->getNumChildren())
        setSize(getBestVirtualSize());
}

// Compute the best size
Vector2i Widget::getBestSize()
{
	Vector2i best;

	if (getLayout())
	{
        best = getWindowSizeForVirtualSize(getLayout()->getMinSize());
	}
	else
	{
		O3D_WARNING("A layout is needed to compute the best size");
		return best;
	}

	// Add any difference between size and client size
    Vector2i diff = size() - getClientSize();
	best.x() += o3d::max(0,diff.x());
	best.y() += o3d::max(0,diff.y());

	return best;
}

// Compute the best virtual size
Vector2i Widget::getBestVirtualSize()
 {
	Vector2i client(getClientSize());
	Vector2i best(getBestSize());

	return Vector2i(o3d::max(client.x(),best.x()), o3d::max(client.y(),best.y()));
}

// Merge the widget's best size into the widget's minimum size
Vector2i Widget::getEffectiveMinSize()
{
    // merge the best size with the min size, giving priority to the min size
    Vector2i min = getMinSize();
    if (min.x() == -1 || min.y() == -1)
    {
        Vector2i best = getBestSize();

        if (min.x() == -1) min.x() = best.x();
        if (min.y() == -1) min.y() = best.y();
    }
    return min;
}

// Position property
void Widget::setPos(const Vector2i& pos)
{
    m_pos = pos;
	positionChanged();
}

void Widget::setPos(Int32 x, Int32 y)
{
	m_pos.set(x,y);
	positionChanged();
}

void Widget::translate(const Vector2i& pos)
{
	m_pos += pos;
	positionChanged();
}

void Widget::translate(Int32 x, Int32 y)
{
	m_pos += Vector2i(x,y);
	positionChanged();
}

Vector2i Widget::getAbsPos() const
{
	Vector2i pos(m_pos);

	//pos += getOrigin();
	//pos -= getScrollPos();

	if (getParentWidget())
		pos += getParentWidget()->getAbsPosOrigin();

	return pos;
}

Vector2i Widget::getOrigin() const
{
	return Vector2i(0,0);
}

Vector2i Widget::getAbsPosOrigin() const
{
	//return getAbsPos() + getOrigin(); OR...
	Vector2i pos = m_pos + getOrigin();

	pos -= getScrollPos();

	if (getParentWidget())
		pos += getParentWidget()->getAbsPosOrigin();

	return pos;
}

void Widget::setFocus()
{
    if (isFocusable())
        getGui()->getWidgetManager()->setFocus(this);
    else
        getGui()->getWidgetManager()->setFocus(getTopLevelWidget());
}

// get the parent layout. By default an HLayout is setup. (const version)
const Layout* Widget::getLayout() const
{
    return nullptr;
}

// get the parent layout. By default an HLayout is setup.
Layout* Widget::getLayout()
{
    return nullptr;
}

// Events and updates
Bool Widget::isTargeted(Int32 x,Int32 y,Widget *&child)
{
	return False;
}

Bool Widget::cut(Bool primary)
{
    return False;
}

Bool Widget::copy(Bool primary)
{
    return False;
}

Bool Widget::paste(Bool primary)
{
    return False;
}

Bool Widget::mouseLeftPressed(Int32 x,Int32 y)
{
	return False;
}

Bool Widget::mouseLeftReleased(Int32 x,Int32 y)
{
	return False;
}

Bool Widget::mouseMiddlePressed(Int32 x,Int32 y)
{
    return False;
}

Bool Widget::mouseMiddleReleased(Int32 x,Int32 y)
{
    return False;
}

Bool Widget::mouseRightPressed(Int32 x,Int32 y)
{
	return False;
}

Bool Widget::mouseRightReleased(Int32 x,Int32 y)
{
	return False;
}

Bool Widget::mouseX1Pressed(Int32 x,Int32 y)
{
    return False;
}

Bool Widget::mouseX1Released(Int32 x,Int32 y)
{
    return False;
}

Bool Widget::mouseX2Pressed(Int32 x,Int32 y)
{
    return False;
}

Bool Widget::mouseX2Released(Int32 x,Int32 y)
{
    return False;
}

Bool Widget::mouseWheel(Int32 x, Int32 y, Int32 z)
{
	return False;
}

Bool Widget::mouseMove(Int32 x,Int32 y)
{
	return True;
}

void Widget::mouseMoveIn()
{
	onEnterWidget();
}

void Widget::mouseMoveOut()
{
	onLeaveWidget();
}

Bool Widget::mouseDrag(Int32 x, Int32 y, Widget *&draged)
{
    return False;
}

Bool Widget::mouseDrop(Int32 x, Int32 y, Widget *droped)
{
    return False;
}

Bool Widget::keyboardToggled(Keyboard *keyboard, KeyEvent event)
{
	return False;
}

Bool Widget::character(Keyboard *keyboard, CharacterEvent event)
{
	return False;
}

// Other events management
void Widget::focused()
{
	m_capacities.enable(STATE_FOCUSED);

	onFocus();
}

void Widget::lostFocus()
{
	m_capacities.disable(STATE_FOCUSED);

	onLostFocus();
}

void Widget::positionChanged()
{
	setDirty();
}

void Widget::sizeChanged()
{
	setDirty();
}

void Widget::valueChanged()
{
	setDirty();
}

// Get the theme to use
Theme* Widget::getUsedTheme()
{
	if (m_theme)
		return m_theme;
	else
	{
        if (m_parent != getGui()->getWidgetManager())
		{
			Widget *parentWidget = getParentWidget();
			if (parentWidget)
				return parentWidget->getUsedTheme();
		}

        return getGui()->getWidgetManager()->getDefaultTheme();
	}
}

const Theme* Widget::getUsedTheme() const
{
	if (m_theme)
		return m_theme;
	else
	{
        if (m_parent != getGui()->getWidgetManager())
		{
			const Widget *parentWidget = getParentWidget();
			if (parentWidget)
				return parentWidget->getUsedTheme();
		}

        return getGui()->getWidgetManager()->getDefaultTheme();
	}
}

// Set the widget always on top.
void Widget::setAlwaysOnTop(Bool onTop)
{
}

// Is the widget always on top.
Bool Widget::isAlwaysOnTop() const
{
	return False;
}

// Set the widget as modal.
void Widget::setModal(Bool modal)
{
}

// Is the widget is modal.
Bool Widget::isModal() const
{
	return False;
}

// Get the current cursor type name for this widget if targeted.
String Widget::getCursorTypeName() const
{
	return "Default";
}

String Widget::tr(const String &key) const
{
    return getGui()->getI18n()->get(key);
}

String Widget::tr(const String &key, const String &_1) const
{
    return getGui()->getI18n()->get(key, _1);
}

String Widget::tr(const String &key, Int32 _1) const
{
    return getGui()->getI18n()->get(key, _1);
}

String Widget::tr(const String &key, const String &_1, const String &_2) const
{
    return getGui()->getI18n()->get(key, _1, _2);
}

String Widget::tr(const String &key, const String &_1, const String &_2, const String &_3) const
{
    return getGui()->getI18n()->get(key, _1, _2, _3);
}

Bool Widget::isFocusable() const
{
    return m_capacities[CAPS_FOCUSABLE];
}

void Widget::setTabIndex(Int32 index)
{
    if (isFocusable())
    {
        m_tabIndex = index;
        getTopLevelWidget()->updateTabIndex(this);
    }
}

Widget *Widget::findNextTabIndex(Widget *widget, Int32 direction)
{
    if (direction == 0)
        return widget;

    if (isFocusable())
        return this;
    else
        return nullptr;
}

Widget *Widget::getPreviouslyFocusedWidget()
{
    return nullptr;
}

void Widget::setPreviouslyFocusedWidget(Widget *widget)
{
    // nothing
}

Vector2i Widget::getScrollPos() const
{
    return Vector2i(0, 0);
}

void Widget::setToolTip(Widget *toolTip)
{

}

Widget *Widget::getToolTip()
{
    return nullptr;
}

void Widget::setToolTip(const String &label)
{
    ToolTip *toolTip = new ToolTip(this, DEFAULT_POS, DEFAULT_SIZE);
    toolTip->getLayout()->addWidget(new StaticText(toolTip->getLayout(), label));

    toolTip->fit();

    setToolTip(toolTip);
}

// Simply draw the cache (default a quad with the texture)
void Widget::drawCache()
{
	// empty surface
	if (m_size.x() <= 0 || m_size.y() <= 0)
		return;

	Context *glContext = getScene()->getContext();

    glContext->blending().setEquation(Blending::FUNC_ADD);
    glContext->blending().setFunc(m_shader.blendFunc);
	glContext->disableDepthTest();

	m_shader.shaderInstance.bindShader();

	m_shader.shaderInstance.setConstMatrix4(
			m_shader.u_modelViewProjectionMatrix,
			False,
			glContext->modelViewProjection());

    m_shader.shaderInstance.setConstFloat(m_shader.u_alpha, m_alpha);
	m_shader.shaderInstance.setConstTexture(m_shader.u_texture, &m_texture, 0);

	const Float vertices[] = {
			(Float)m_pos.x()+m_size.x(), (Float)m_pos.y(), 0.f,
			(Float)m_pos.x(), (Float)m_pos.y(), 0.f,
			(Float)m_pos.x()+m_size.x(), (Float)m_pos.y()+m_size.y(), 0.f,
			(Float)m_pos.x(), (Float)m_pos.y()+m_size.y(), 0.f };

    m_shader.vertices.create(12, VertexBuffer::DYNAMIC, vertices, True);
    m_shader.vertices.attribute(m_shader.a_vertex, 3, 0, 0);

	const Float texCoords[] = {
			m_textureTopRightCoord.x(), m_textureTopRightCoord.y(),
			0.f, m_textureTopRightCoord.y(),
			m_textureTopRightCoord.x(), 0.f,
			0.f,0.f };

    m_shader.texCoords.create(8, VertexBuffer::DYNAMIC, texCoords, True);
    m_shader.texCoords.attribute(m_shader.a_texCoords, 2, 0, 0);

	getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);

	glContext->disableVertexAttribArray(m_shader.a_vertex);
	glContext->disableVertexAttribArray(m_shader.a_texCoords);

	m_shader.shaderInstance.unbindShader();
}

// Cache update if necessary, and followed by a draw of him
void Widget::processCache(Bool clear)
{
	// only if a redraw is necessary
    if (isDirty()) {
		Vector2i size = m_size;

		// empty surface
        if (m_size.x() <= 0 || m_size.y() <= 0) {
			return;
        }

		// setup
		Int32 oldViewPort[4];
		Matrix4 oldProjection;
		oldProjection = getScene()->getContext()->projection().get();

		// compute the needed texture size in power of two
		Vector2i tmp(size), textureSize(2,2);

        while ((tmp.x() = tmp.x() >> 1) != 0) {
			textureSize.x() <<= 1;
        }

        while ((tmp.y() = tmp.y() >> 1) != 0) {
			textureSize.y() <<= 1;
        }

		// limits to max texture size
		if ((textureSize.x() > getScene()->getContext()->getTextureMaxSize()) ||
            (textureSize.y() > getScene()->getContext()->getTextureMaxSize())) {
			O3D_ERROR(E_InvalidParameter("Unsupported texture width"));
		}

		// compute the floating point top-right texture coordinate
		m_textureTopRightCoord.x() = (Float)size.x() / textureSize.x();
		m_textureTopRightCoord.y() = (Float)size.y() / textureSize.y();

		// fill the texture cache
        FrameBuffer *pFBO = getGui()->findFBO(textureSize.x(), textureSize.y(), PF_RGBA_U8);
        O3D_ASSERT(pFBO != nullptr);

		pFBO->bindBuffer();

		// create/resize the texture
        if ((m_texture.getWidth() != (UInt32)textureSize.x()) || (m_texture.getHeight() != (UInt32)textureSize.y())) {
            if (!m_texture.create(False, textureSize.x(), textureSize.y(), PF_RGBA_U8, True)) {
				return;
            }
		}

        pFBO->attachTexture2D(&m_texture,FrameBuffer::COLOR_ATTACHMENT0);

        // not efficient but necessary because fbo is defined at this time
        if (!pFBO->isCompleteness()) {
            return;
        }

        // here we dont care of an active scissor test
        Bool scissor = getScene()->getContext()->getScissorTest();
        Box2i oldScissor = getScene()->getContext()->getScissor();

        if (scissor) {
            getScene()->getContext()->disableScissorTest();
        }

		getScene()->getContext()->modelView().push();
		getScene()->getContext()->modelView().identity();

		Matrix4 ortho2d;
        ortho2d.buildOrtho(0.f, (Float)size.x(), (Float)size.y(), 0.f, -1.f, 1.f);

		getScene()->getContext()->projection().set(ortho2d);

		getScene()->getContext()->bindTexture(TEXTURE_2D,0,True);

        // TODO manage the multi viewport of GL 4 and a fallback with a stack for GL < 4
		getScene()->getContext()->getViewPort(oldViewPort);
		getScene()->getContext()->setViewPort(0, 0, size.x(), size.y());

        if (clear) {
            pFBO->clearColor(FrameBuffer::COLOR_ATTACHMENT0, Color(0.f, 0.f, 0.f, 0.f));
        }

		// call the draw
		updateCache();

		// detach the texture before unbound
		pFBO->detach(FrameBuffer::COLOR_ATTACHMENT0);

		// clean
        pFBO->unbindBuffer();

        // restore previous active scissor test
        if (scissor) {
            getScene()->getContext()->setScissor(oldScissor);
            getScene()->getContext()->enableScissorTest();
        }

		getScene()->getContext()->setViewPort(oldViewPort);

		// cache is now updated
		setClean();

		getScene()->getContext()->modelView().pop();
		getScene()->getContext()->projection().set(oldProjection);
	}

	drawCache();
}

// Serialization
Bool Widget::writeToFile(OutStream &os)
{
	return True;
}

Bool Widget::readFromFile(InStream &is)
{
	return True;
}

// INTERNAL : define the layout item
void Widget::defineLayoutItem()
{
	deletePtr(m_layoutItem);

    if (isLayoutObject()) {
		m_layoutItem = new LayoutItem(reinterpret_cast<Layout*>(this));
    } else if (typeOf<Spacer>(this)) {
		m_layoutItem = new LayoutItem(reinterpret_cast<Spacer*>(this));
    } else {
		m_layoutItem = new LayoutItem(reinterpret_cast<Widget*>(this));
    }
}

// INTERNAL : get the layout item
LayoutItem* Widget::getLayoutItem()
{
	return m_layoutItem;
}

void Widget::themeChanged()
{
    // nothing
}

void Widget::setBlendFunc(Blending::FuncProfile func)
{
    m_shader.blendFunc = func;
}

void Widget::update()
{
    // nothing
}

void Widget::postImportPass()
{
    // nothing
}
