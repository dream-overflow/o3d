/**
 * @file map2ddrawer.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MAP2DDRAWER_H
#define _O3D_MAP2DDRAWER_H

#include "../scene/scenedrawer.h"
#include "map2d.h"

namespace o3d {

/**
 * @brief A simple scene drawer that manage a Map2d without any 3d part of the engine
 * and without post effects.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-08-02
 */
class O3D_API Map2dDrawer : public SceneDrawer
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(Map2dDrawer)

	Map2dDrawer(BaseObject *parent);

	void setMap2d(Map2d *map2d) { m_map2d.set(this, map2d); }
	Map2d* getMap2d() { return m_map2d.get(); }
	const Map2d* getMap2d() const { return m_map2d.get(); }

    virtual void draw(ViewPort *viewPort) override;
    virtual void drawPicking(ViewPort *viewPort) override;

private:

	SmartObject<Map2d> m_map2d;   //!< Related 2d map to draw in the scene
};

} // namespace o3d

#endif // _O3D_MAP2DDRAWER_H
