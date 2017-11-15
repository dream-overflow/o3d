/**
 * @file icon.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgets/icon.h"
#include "o3d/gui/guitype.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Icon, GUI_ICON, Widget)

// Default initialization constructor from a parent base object.
Icon::Icon(BaseObject *parent) :
	Widget(parent),
        m_iconSet(nullptr),
        m_icon(nullptr),
		m_lastTime(0),
		m_lastFrame(0)
{
}

// Default initialization constructor from a parent.
Icon::Icon(
	Widget *parent,
	IconSet *iconSet,
	const String &iconName,
	const Vector2i &iconSize) :
		Widget(parent),
		m_iconSet(iconSet),
        m_icon(nullptr),
		m_lastTime(0),
		m_lastFrame(0)
{
	O3D_ASSERT(m_iconSet);

	if (!m_iconSet)
		O3D_ERROR(E_InvalidParameter("Icon set is not valid"));

	m_size = m_minSize = m_maxSize = iconSize;

	m_icon = iconSet->getIconInfo(iconName, iconSize);

    m_capacities.disable(CAPS_FOCUSABLE);
}

// Virtual destructor.
Icon::~Icon()
{

}

// Draw
void Icon::draw()
{
	if (m_iconSet && isShown())
	{
		UInt32 frame = computeFrame();
		m_iconSet->draw(m_pos, *m_icon, frame, m_alpha);
	}
}

void Icon::updateCache()
{
	if (m_iconSet != nullptr)
	{
		UInt32 frame = computeFrame();
		m_iconSet->draw(m_pos, *m_icon, frame, m_alpha);
	}
}

// Get the icon name.
const String& Icon::getIconName() const
{
	if (m_icon != nullptr)
		return m_icon->name;
	else
		return String::getNull();
}

// Get the icon size.
const Vector2i& Icon::getIconSize() const
{
	if (m_icon != nullptr)
		return m_icon->size;
	else
		return Vector::nullVector2i;
}

Vector2i Icon::getDefaultSize()
{
	return getIconSize();
}

// Compute the frame index.
UInt32 Icon::computeFrame()
{
	if (m_icon->frames.size() <= 1)
		return 0;
	else
	{
		UInt32 time = System::getMsTime();
		if (m_lastTime == 0)
		{
			m_lastTime = time;
			m_lastFrame = 0;

			return 0;
		}

		UInt32 elapsed = time - m_lastTime;
		while (elapsed >= m_icon->frames[m_lastFrame].delay)
		{
			elapsed -= m_icon->frames[m_lastFrame].delay;

			++m_lastFrame;

			// loop
			if (m_lastFrame >= m_icon->frames.size())
				m_lastFrame = 0;
		}

		m_lastTime = time - elapsed;

		return m_lastFrame;
	}
}

