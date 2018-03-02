/**
 * @file map2dobject.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/geom/aabbox.h"
#include "o3d/geom/plane.h"
#include "o3d/geom/frustum.h"
#include "o3d/engine/map2d/map2dobject.h"
#include "o3d/engine/map2d/map2dvisibility.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Map2dObject, ENGINE_MAP_2D_OBJECT, SceneObject)

Map2dObject::Map2dObject(BaseObject *parent) :
	SceneObject(parent),
	m_tileSet(this),
	m_moved(True),
	m_useColor(False),
	m_colorMask(1.f, 1.f, 1.f, 1.f),
	m_tile(nullptr),
	m_quad(nullptr)
{
	setDrawable(True);
	setUpdatable(True);
	setAnimatable(True);
	setShadable(True);
	setShadowable(False);
	setUpdatable(True);
	setMovable(True);
}

Map2dObject::~Map2dObject()
{
}

#include "o3d/engine/primitive/primitivemanager.h"
#include "o3d/engine/context.h"

// X, Y
//inline Vector2i orthoToIso(const Vector2i &v)
//{
//    // transform ortho to iso
//    static const Float matrix[] = {   // column major matrix
//          0.707106f, -0.353553f,
//          0.707106f,  0.353553f };

//    return Vector2i(
//                v.x() * matrix[0] + v.y() * matrix[2],
//                v.x() * matrix[1] + v.y() * matrix[3]);
//}

void Map2dObject::draw(const DrawInfo &drawInfo)
{
	// Draw the object
	if (getActivity() && getVisibility() && (m_tile != nullptr))
	{
        // offset to center the tile according to its base rect
        Vector2i ofs(-m_baseRect.b().x(), -m_baseRect.a().y());

        if (drawInfo.pass == DrawInfo::AMBIENT_PASS) {
            if (m_useColor) {
                m_tileSet.get()->draw(m_isoPos + ofs, *m_tile, 0, m_colorMask);
            } else {
                m_tileSet.get()->draw(m_isoPos + ofs, *m_tile, 0, 1.0f);
            }

            if (getScene()->getDrawObject(Scene::DRAW_LOCAL_AXIS)) {
                PrimitiveAccess acces = getScene()->getPrimitiveManager()->access(drawInfo);

				acces->setColor(Color(0,1,0,1));
				acces->setModelviewProjection();

                acces->beginDraw(P_LINE_LOOP);
                    acces->addVertex(m_isoAbsBaseRect.a().x(), m_isoAbsBaseRect.a().y(), 0.f);
                    acces->addVertex(m_isoAbsBaseRect.b().x(), m_isoAbsBaseRect.b().y(), 0.f);
                    acces->addVertex(m_isoAbsBaseRect.c().x(), m_isoAbsBaseRect.c().y(), 0.f);
                    acces->addVertex(m_isoAbsBaseRect.d().x(), m_isoAbsBaseRect.d().y(), 0.f);
                    acces->addVertex(m_isoAbsBaseRect.a().x(), m_isoAbsBaseRect.a().y(), 0.f);
                acces->endDraw();

                // local axis
                Int32 scale = 10;
                Vector2i v1 = m_isoPos + orthoToIso2d(Vector2i(0 , 0));
                Vector2i v2 = m_isoPos + orthoToIso2d(Vector2i(scale, 0));
                Vector2i v3 = m_isoPos + orthoToIso2d(Vector2i(0, scale));

                // Y in green
                acces->beginDraw(P_LINES);
                acces->addVertex(v1.x(), v1.y(), 0.f);
                acces->addVertex(v3.x(), v3.y(), 0.f);
                acces->endDraw();

                // X in red
                acces->setColor(Color(1,0,0,1));
                acces->beginDraw(P_LINES);
                acces->addVertex(v1.x(), v1.y(), 0.f);
                acces->addVertex(v2.x(), v2.y(), 0.f);
                acces->endDraw();
			}
        } else if (drawInfo.pass == DrawInfo::PICKING_PASS) {
            m_tileSet.get()->drawPicking(m_isoPos + ofs, *m_tile, 0, getPickableId());
		}
	}
}

UInt32 Map2dObject::getDrawType() const
{
	return Scene::DRAW_MAP_2D_OBJECT;
}

Geometry::Clipping Map2dObject::checkBounding(const AABBox &bbox) const
{
	return Geometry::CLIP_INSIDE;
}

Geometry::Clipping Map2dObject::checkBounding(const Plane &plane) const
{
	return Geometry::CLIP_INSIDE;
}

Geometry::Clipping Map2dObject::checkFrustum(const Frustum &frustum) const
{
	return Geometry::CLIP_INSIDE;
}

void Map2dObject::update()
{
	clearUpdated();

	if (m_moved)
	{
		setUpdated();
		m_moved = False;
	}
}

void Map2dObject::setPos(const Vector2i &pos)
{
	m_pos = pos;
	m_moved = True;

    // offset to center the tile according to its base rect
    Vector2i ofs(-m_baseRect.b().x(), -m_baseRect.a().y());

	// absolutes iso 2d coordinates
    m_isoPos = orthoToIso2d(m_pos);

	// CCW rect
    m_absBaseRect.a() = m_pos + m_baseRect.a() + ofs;
    m_absBaseRect.d() = m_pos + m_baseRect.b() + ofs;
    m_absBaseRect.c() = m_pos + m_baseRect.c() + ofs;
    m_absBaseRect.b() = m_pos + m_baseRect.d() + ofs;

	// absolute ortho 2d box
    m_absBox.set(m_pos + ofs, m_tileSet->getTileSize());
    m_isoAbsBox.set(m_isoPos + ofs, m_tileSet->getTileSize());

    m_isoAbsBaseRect.a() = m_isoPos + m_baseRect.a() + ofs;
    m_isoAbsBaseRect.d() = m_isoPos + m_baseRect.b() + ofs;
    m_isoAbsBaseRect.c() = m_isoPos + m_baseRect.c() + ofs;
    m_isoAbsBaseRect.b() = m_isoPos + m_baseRect.d() + ofs;

	// update the quadtree
	if (m_quad != nullptr)
		m_quad->getVisibility()->updateObject(this);
}

const Vector2i& Map2dObject::getSize() const
{
    return m_tileSet->getTileSize();
}

void Map2dObject::setTile(Map2dTileSet *tileSet, UInt32 tileId)
{
	m_tileSet = tileSet;
	m_tileId = tileId;

	m_tile = &m_tileSet.get()->getTile(m_tileId);
}

void Map2dObject::setColorMask(const Color &color)
{
	m_colorMask = color;
	if (color.red() == 1.f && color.green() == 1.f && color.blue() == 1.f)
		m_useColor = False;
	else
		m_useColor = True;
}

