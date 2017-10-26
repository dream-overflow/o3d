/**
 * @file map2dobject.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MAP2DOBJECT_H
#define _O3D_MAP2DOBJECT_H

#include "o3d/core/classinfo.h"
#include "../scene/sceneobject.h"
#include "o3d/core/box.h"
#include "o3d/core/rect2.h"
#include "map2dtileset.h"

#include <list>

namespace o3d {

//! helper to convert from 2d ortho to 2d isometric coordinates
template<class T>
inline Vector2<T> orthoToIso2d(const Vector2<T> &v)
{
    return Vector2<T>(
                v.x() - v.y(),
                (v.x() + v.y()) / 2);
}

//! helper to convert from 2d iso to 2d ortho coordinates
template<class T>
inline Vector2<T> isoToOrtho2d(const Vector2<T> &v)
{
    Vector2<T> res;
    res.x() = v.y() + v.x() / 2;
    res.y() = 2 * v.y() - res.x();

    return res;
}

class Map2dQuad;

/**
 * @brief Map 2d object base class.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-08-03
 */
class O3D_API Map2dObject : public SceneObject
{
public:

	O3D_DECLARE_CLASS(Map2dObject)

	Map2dObject(BaseObject *parent);

	virtual ~Map2dObject();

	//
	// Drawable
	//

	virtual void draw(const DrawInfo &drawInfo);
	virtual UInt32 getDrawType() const;

	//! Always CLIP_INSIDE
	virtual Geometry::Clipping checkBounding(const AABBox &bbox) const;
	//! Always CLIP_INSIDE
	virtual Geometry::Clipping checkBounding(const Plane &plane) const;
	//! Always CLIP_INSIDE
	virtual Geometry::Clipping checkFrustum(const Frustum &frustum) const;

	//
	// Updatable
	//

	virtual void update();

	//
	// Object
	//

	//! Set the object position.
	void setPos(const Vector2i &pos);

	//! Get the object position in 2d ortho coordinates.
	const Vector2i& getPos() const { return m_pos; }

	//! Get the object size in 2d ortho coordinates.
	const Vector2i& getSize() const;

	//! Get the absolute bounding box in 2d ortho coordinates.
	const Box2i& getAbsBox() const { return m_absBox; }
    //! Get the absolute bounding box in 2d iso coordinates.
    const Box2i& getIsoAbsBox() const { return m_isoAbsBox; }

	//! Get the position in 2d isometric coordinates.
	const Vector2i& getIsoPos() const { return m_isoPos; }
    //! Get the absolute 2d rect of the base of the object in iso 2d coordinates.
    const Rect2i& getIsoAbsBaseRect() const { return m_isoAbsBaseRect; }

    //! Get the absolute 2d rect of the base of the object in ortho 2d coordinates.
    const Rect2i& getAbsBaseRect() const { return m_absBaseRect; }

    //! Set the relative 2d rect of the base of the object in ortho 2d coordinates.
    void setBaseRect(const Rect2i &rect) { m_baseRect = rect; }
    //! Get the relative 2d rect of the base of the object in ortho 2d coordinates.
    const Rect2i& getBaseRect() const { return m_baseRect; }

	//! Set the related texture asset.
	void setTile(Map2dTileSet *tileSet, UInt32 tileId);

	//! Get the related tileset.
	const Map2dTileSet* getTileSet() const { return m_tileSet.get(); }

	//! Get the texture tile id.
	UInt32 getTileId() const { return m_tileId; }

	//! Change the color mask.
	void setColorMask(const Color &color);

	//! Get the color mask.
	const Color& getColorMask() const { return m_colorMask; }

	//! Get the quad where the object belong to.
	Map2dQuad* getQuad() const { return m_quad; }

	//! Set the quad where the object belong to.
	void setQuad(Map2dQuad *quad) { m_quad = quad; }

    //! Comparison operator.
    static bool compare(const Map2dObject *a, const Map2dObject *b)
    {
        if ((a->getIsoAbsBaseRect().a().y() == b->getIsoAbsBaseRect().a().y()))
        {
            return (a->getIsoAbsBaseRect().b().x() > b->getIsoAbsBaseRect().b().x()) ? false :
                   (a->getIsoAbsBaseRect().b().x() == b->getIsoAbsBaseRect().b().x()) ? false : true;
        }
        else
        {
            return (a->getIsoAbsBaseRect().a().y() > b->getIsoAbsBaseRect().a().y()) ? false : true;
        }
    }

protected:

	Vector2i m_pos;       //!< relative position to the layer in ortho 2d coordinates
	SmartObject<Map2dTileSet> m_tileSet;   //!< related tile-set
	UInt32 m_tileId;  //!< tile identifier into the related tile-set

	Bool m_moved;
	Bool m_useColor;
	Color m_colorMask;

    Rect2i m_baseRect;

	Box2i m_absBox;
    Box2i m_isoAbsBox;

    Rect2i m_absBaseRect;
    Rect2i m_isoAbsBaseRect;
	Vector2i m_isoPos;    //!< relative position to the layer in iso 2d coordinates

	const Map2dTileSet::Tile *m_tile;

	Map2dQuad *m_quad;
};

typedef std::list<Map2dObject*> T_Map2dObjectList;
typedef T_Map2dObjectList::iterator IT_Map2dObjectList;
typedef T_Map2dObjectList::const_iterator CIT_Map2dObjectList;

} // namespace o3d

#endif // _O3D_MAP2DOBJECT_H

