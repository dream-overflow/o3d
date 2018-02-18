/**
 * @file icon.h
 * @brief An icon container.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2009-11-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ICON_H
#define _O3D_ICON_H

#include "../iconset.h"
#include "widget.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Icon
//-------------------------------------------------------------------------------------
//! A widget that define an icon. An icon is created from an icon set IconSet and
//! an identifier into this icon set.
//---------------------------------------------------------------------------------------
class O3D_API Icon : public Widget
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(Icon)

	//! Default initialization constructor from a parent.
	//! @param parent Parent widget.
	//! @param iconSet Icon set.
	//! @param name Icon name into the icon set.
	//! @param iconSize Icon size into the icon set.
	Icon(Widget *parent,
		IconSet *iconSet,
		const String &iconName,
		const Vector2i &iconSize);

	//! Virtual destructor.
	virtual ~Icon();

	// draw
    virtual void draw() override;
    virtual void updateCache() override;

	//! Get the icon set.
	inline const IconSet* getIconSet() const { return m_iconSet; }

	//! Get the icon name.
	const String& getIconName() const;

	//! Get the icon size.
	const Vector2i& getIconSize() const;

    virtual Vector2i getDefaultSize() override;

protected:

	//! Default initialization constructor from a parent base object.
	Icon(BaseObject *parent);

	IconSet *m_iconSet;
	IconSet::Icon *m_icon;

	UInt32 m_lastTime;    //!< Last draw time for animated icon.
	UInt32 m_lastFrame;   //!< Last played frame.
	
	//! Compute the frame index.
	UInt32 computeFrame();
};

} // namespace o3d

#endif // _O3DICON_H
