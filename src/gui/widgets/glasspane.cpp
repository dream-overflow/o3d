/**
 * @file glasspane.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/glasspane.h"
#include "o3d/gui/widgetdrawmode.h"

#include "o3d/engine/context.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/gui/widgetmanager.h"

#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"
#include "o3d/gui/widgets/layout.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(GlassPane, GUI_WIDGET_GLASS_PANE, Window)

GlassPane::GlassPane(BaseObject *parent) :
	Window(parent)
{
}

GlassPane::GlassPane(
	Widget *parent,
	const Vector2i &pos,
	const Vector2i &size,
	const String &name) :
		Window(parent, "", pos, size, EMPTY_STYLE, name)
{
    onDeleteGlassPane.connect(this, &GlassPane::deleteGlassPane, CONNECTION_ASYNCH);
}

GlassPane::GlassPane(
		WidgetManager *parent,
		const Vector2i &pos,
		const Vector2i &size,
		const String &name) :
		Window(parent, "", pos, size, EMPTY_STYLE, name)
{
    onDeleteGlassPane.connect(this, &GlassPane::deleteGlassPane, CONNECTION_ASYNCH);
}

GlassPane::~GlassPane()
{
}

void GlassPane::deleteGlassPane()
{
    ((Gui*)getScene()->getGui())->getWidgetManager()->deleteWindow(this);
}

Bool GlassPane::mouseLeftPressed(Int32 x, Int32 y)
{
	onDeleteGlassPane();
	return True;
}

Bool GlassPane::mouseLeftReleased(Int32 x, Int32 y)
{
	return True;
}

Bool GlassPane::mouseRightPressed(Int32 x, Int32 y)
{
	onDeleteGlassPane();
	return True;
}

Bool GlassPane::mouseRightReleased(Int32 x, Int32 y)
{
	return True;
}

Bool GlassPane::mouseWheel(Int32 x, Int32 y, Int32 z)
{
	onDeleteGlassPane();
	return True;
}

Bool GlassPane::mouseMove(Int32 x, Int32 y)
{
	return True;
}

Bool GlassPane::keyboardToggled(Keyboard *keyboard, KeyEvent event)
{
	onDeleteGlassPane();
	return True;
}

Bool GlassPane::character(Keyboard *keyboard, CharacterEvent event)
{
	return True;
}

Bool GlassPane::isTargeted(Int32 x, Int32 y, Widget *&pWidget)
{
	// no negative part
	if (x < 0 || y < 0)
		return False;

    if ((x < m_size.x()) && (y < m_size.y()))
	{
		// others children widgets
		Vector2i org = getOrigin();
		pWidget = this;

		// delta by the client origin for children widgets
		// title bar buttons are in children space, using
		// negatives coordinates.
        x -= org.x();
        y -= org.y();

		// recursive targeted
		if (getLayout() && getLayout()->isTargeted(
            x - getLayout()->pos().x(),
            y - getLayout()->pos().y(),
			pWidget))
		{
			return True;
		}

		return True;
	}

	return False;
}

//---------------------------------------------------------------------------------------
// draw
//---------------------------------------------------------------------------------------
void GlassPane::draw()
{
	if (isShown())
	{
		// is need to recompute the layout
		if (getLayout() && getLayout()->isDirty())
			getLayout()->layout();

		Vector2i org = getOrigin();

		// translate in window coordinates
		getScene()->getContext()->modelView().translate(
                Vector3((Float)m_pos.x(), (Float)m_pos.y(), 0.f));

		// translate in client area window coordinates
		getScene()->getContext()->modelView().translate(
                Vector3((Float)org.x(), (Float)org.y(),0.f));

		// finally draw its content into the client area
		if (getLayout())
		{
			getScene()->getContext()->modelView().push();
			getLayout()->draw();
			getScene()->getContext()->modelView().pop();
		}
	}
}

void GlassPane::updateCache()
{
	// nothing
}

