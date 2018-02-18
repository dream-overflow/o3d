/**
 * @file spacer.h
 * @brief Gizmo widget used to insert space into layouts.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-03-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SPACER_H
#define _O3D_SPACER_H

#include "widget.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Spacer
//-------------------------------------------------------------------------------------
//! Gizmo widget used to insert space into layouts.
//---------------------------------------------------------------------------------------
class O3D_API Spacer : public Widget
{
public:

	O3D_DECLARE_CLASS(Spacer)

	//! default constructor
	Spacer(Widget *parent);

	//-----------------------------------------------------------------------------------
	// Properties
	//-----------------------------------------------------------------------------------

	//! get the recommended widget default size
    virtual Vector2i getDefaultSize() override;

	//------------------------------------------------------------------------------------
	// Widget
	//-----------------------------------------------------------------------------------

    virtual void sizeChanged() override;

    virtual void draw() override {}
    virtual void updateCache() override {}

protected:

	//! default constructor
	Spacer(BaseObject *parent);
};

} // namespace o3d

#endif // _O3D_SPACER_H
