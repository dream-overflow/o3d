/**
 * @file map2dtilelayer.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/map2d/map2dtilelayer.h"
#include "o3d/core/baseobject.h"
#include "o3d/engine/enginetype.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/object/camera.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Map2dTileLayer, ENGINE_MAP_2D_TILE_LAYER, Map2dLayer)

Map2dTileLayer::Map2dTileLayer(BaseObject *parent) :
	Map2dLayer(parent),
	m_rows(0),
	m_columns(0),
	m_tileSet(this),
	m_backgroundTile(0),
    m_background(False),
    m_vertices(getScene()->getContext()),
    m_texCoords(getScene()->getContext())
{
}

UInt32 Map2dTileLayer::getNumElt() const
{
	return 1;
}

UInt32 Map2dTileLayer::getNumSon() const
{
	return 1;
}

const SceneObject *Map2dTileLayer::findSon(const String &name) const
{
	return nullptr;
}

SceneObject *Map2dTileLayer::findSon(const String &name)
{
	return nullptr;
}

Bool Map2dTileLayer::findSon(SceneObject *object) const
{
    return False;
}

const Transform *Map2dTileLayer::getTransform() const
{
	return nullptr;
}

Transform *Map2dTileLayer::getTransform()
{
	return nullptr;
}

Bool Map2dTileLayer::isNodeObject() const
{
	return False;
}

const Vector2i& Map2dTileLayer::getSize() const
{
	return m_size;
}

void Map2dTileLayer::setTileSet(Map2dTileSet *tileSet)
{
    if (m_tileSet.get() != tileSet)
    {
        m_tileSet = tileSet;

        m_size.set(
                    m_tileSet->getTileSize().x() * m_columns,
                    m_tileSet->getTileSize().y() * m_rows);

        setDrawUpdate();
    }
}

void Map2dTileLayer::setBackgroundTile(UInt32 tileId)
{
    if (m_backgroundTile != tileId)
    {
        m_backgroundTile = tileId;
        setDrawUpdate();
    }
}

void Map2dTileLayer::enableBackground()
{
    if (!m_background)
    {
        m_background = True;
        setDrawUpdate();
    }
}

void Map2dTileLayer::disableBackground()
{
    if (m_background)
    {
        m_background = False;
        setDrawUpdate();
    }
}

void Map2dTileLayer::setMapData(const SmartArrayUInt8 &data, UInt32 rows, UInt32 columns)
{
	if (m_tileMap.getNumElt() != m_rows * m_columns)
		O3D_ERROR(E_InvalidParameter("data must have rows*columns elements"));

	if (!m_tileSet.isValid())
		O3D_ERROR(E_InvalidOperation("Tile set must be defined before"));

	m_tileMap = data;
	m_rows = rows;
	m_columns = columns;

	m_size.set(
				m_tileSet->getTileSize().x() * columns,
                m_tileSet->getTileSize().y() * rows);

    setDrawUpdate();
}

void Map2dTileLayer::fillBuffers()
{
    Camera *camera = getScene()->getActiveCamera();

    if (camera == nullptr)
        return;

    // update only
    if (!camera->isCameraChanged() && !isNeedDraw())
        return;

    clearDrawUpdate();

    // compute the visible portion of the map
    Vector3 camPos = camera->getAbsoluteMatrix().getTranslation();

    // centered view position in tile unit
    Float centerCol = camPos.x() / m_tileSet->getTileSize().x();
    Float centerRow = camPos.y() / m_tileSet->getTileSize().y();

    // size of the area to display in tile unit
    Float w = (camera->getRight()-camera->getLeft()-1) / m_tileSet->getTileSize().x();
    Float h = (camera->getBottom()-camera->getTop()-1) / m_tileSet->getTileSize().y();

    Vector2i tileSize = m_tileSet->getTileSize();
    Vector2i pos;

    // render with a background
    if (m_background)
    {
        // start position in tile unit, centered
        Int32 sx = (Int32)floor(centerCol-w/2.f);
        Int32 sy = (Int32)floor(centerRow-h/2.f);

        // end position in tile unit, centered
        Int32 ex = (Int32)ceil(centerCol + w/2.f);
        Int32 ey = (Int32)ceil(centerRow + h/2.f);

        // start y position in screen unit, centered
        pos.y() = Int32((Int32)sy * tileSize.y());

        // 4 vertices per tiles because of independants textures coordinates
        Int32 len = (ey-sy) * 4 * (ex-sx);

        SmartArrayFloat vertices(len*3);
        SmartArrayFloat texCoords(len*2);

        Float *uvs = texCoords.getData();
        Float *vs = vertices.getData();

        for (Int32 y = sy; y < ey; ++y)
        {
            // start x position in screen unit, centered
            pos.x() = Int32((Int32)sx * tileSize.x());

            for (Int32 x = sx; x < ex; ++x)
            {
                if (x < 0 || y < 0 || x >= Int32(m_columns) || y >= Int32(m_rows))
                {
                    try {
                        const Map2dTileSet::Tile &tile = m_tileSet->getTile(m_backgroundTile);
                        const Map2dTileSet::TileDef &tileDef = tile.frames[0].tileDef;

                        *uvs++ = tileDef.sU; *uvs++ = tileDef.eV;
                        *uvs++ = tileDef.eU; *uvs++ = tileDef.eV;
                        *uvs++ = tileDef.sU; *uvs++ = tileDef.sV;
                        *uvs++ = tileDef.eU; *uvs++ = tileDef.sV;

                        *vs++ = pos.x();                *vs++ = pos.y() + tileSize.y(); *vs++ = 0.0f;
                        *vs++ = pos.x() + tileSize.x(); *vs++ = pos.y() + tileSize.y(); *vs++ = 0.0f;
                        *vs++ = pos.x();                *vs++ = pos.y();                *vs++ = 0.0f;
                        *vs++ = pos.x() + tileSize.x(); *vs++ = pos.y();                *vs++ = 0.0f;
                    } catch (E_BaseException &e)
                    {
                        *uvs++ = 0.f; *uvs++ = 0.f;
                        *uvs++ = 0.f; *uvs++ = 0.f;
                        *uvs++ = 0.f; *uvs++ = 0.f;
                        *uvs++ = 0.f; *uvs++ = 0.f;

                        *vs++ = pos.x();                *vs++ = pos.y() + tileSize.y(); *vs++ = 0.0f;
                        *vs++ = pos.x() + tileSize.x(); *vs++ = pos.y() + tileSize.y(); *vs++ = 0.0f;
                        *vs++ = pos.x();                *vs++ = pos.y();                *vs++ = 0.0f;
                        *vs++ = pos.x() + tileSize.x(); *vs++ = pos.y();                *vs++ = 0.0f;
                    }
                }
                else
                {
                    try {
                        const Map2dTileSet::Tile &tile = m_tileSet->getTile(m_tileMap[y*m_columns + x]);
                        const Map2dTileSet::TileDef &tileDef = tile.frames[0].tileDef;

                        *uvs++ = tileDef.sU; *uvs++ = tileDef.eV;
                        *uvs++ = tileDef.eU; *uvs++ = tileDef.eV;
                        *uvs++ = tileDef.sU; *uvs++ = tileDef.sV;
                        *uvs++ = tileDef.eU; *uvs++ = tileDef.sV;

                        *vs++ = pos.x();                *vs++ = pos.y() + tileSize.y(); *vs++ = 0.0f;
                        *vs++ = pos.x() + tileSize.x(); *vs++ = pos.y() + tileSize.y(); *vs++ = 0.0f;
                        *vs++ = pos.x();                *vs++ = pos.y();                *vs++ = 0.0f;
                        *vs++ = pos.x() + tileSize.x(); *vs++ = pos.y();                *vs++ = 0.0f;
                    } catch (E_BaseException &e)
                    {
                        *uvs++ = 0.f; *uvs++ = 0.f;
                        *uvs++ = 0.f; *uvs++ = 0.f;
                        *uvs++ = 0.f; *uvs++ = 0.f;
                        *uvs++ = 0.f; *uvs++ = 0.f;

                        *vs++ = pos.x();                *vs++ = pos.y() + tileSize.y(); *vs++ = 0.0f;
                        *vs++ = pos.x() + tileSize.x(); *vs++ = pos.y() + tileSize.y(); *vs++ = 0.0f;
                        *vs++ = pos.x();                *vs++ = pos.y();                *vs++ = 0.0f;
                        *vs++ = pos.x() + tileSize.x(); *vs++ = pos.y();                *vs++ = 0.0f;
                    }
                }

                pos.x() += tileSize.x();
            }

            pos.y() += tileSize.y();
        }

        m_vertices.create(len*3, VertexBuffer::DYNAMIC, vertices.getData());
        m_texCoords.create(len*2, VertexBuffer::DYNAMIC, texCoords.getData());
    }
    else // wihthout background
    {
        // start position in tile unit, centered
        Int32 sx = (Int32)(centerCol-w/2.f);
        Int32 sy = (Int32)(centerRow-h/2.f);

        if (sx < 0)
        {
            centerCol += -sx;
            sx = 0;
        }

        if (sy < 0)
        {
            centerRow += -sy;
            sy = 0;
        }

        // end position in tile unit, centered
        Int32 ex = o3d::clamp((Int32)ceil(centerCol + w/2.f), 0, (Int32)m_columns);
        Int32 ey = o3d::clamp((Int32)ceil(centerRow + h/2.f), 0, (Int32)m_rows);

        // 4 vertices per tiles because of independants textures coordinates
        Int32 len = (ey-sy) * 4 * (ex-sx);

        SmartArrayFloat vertices(len*3);
        SmartArrayFloat texCoords(len*2);

        Float *uvs = texCoords.getData();
        Float *vs = vertices.getData();

        // start y position in screen unit, centered
        pos.y() = Int32((Int32)sy * tileSize.y());

        for (Int32 y = sy; y < ey; ++y)
        {
            // start x position in screen unit, centered
            pos.x() = Int32((Int32)sx * tileSize.x());

            for (Int32 x = sx; x < ex; ++x)
            {
                try {
                    const Map2dTileSet::Tile &tile = m_tileSet->getTile(m_tileMap[y*m_columns + x]);
                    const Map2dTileSet::TileDef &tileDef = tile.frames[0].tileDef;

                    *uvs++ = tileDef.sU; *uvs++ = tileDef.eV;
                    *uvs++ = tileDef.eU; *uvs++ = tileDef.eV;
                    *uvs++ = tileDef.sU; *uvs++ = tileDef.sV;
                    *uvs++ = tileDef.eU; *uvs++ = tileDef.sV;

                    *vs++ = pos.x();                *vs++ = pos.y() + tileSize.y(); *vs++ = 0.0f;
                    *vs++ = pos.x() + tileSize.x(); *vs++ = pos.y() + tileSize.y(); *vs++ = 0.0f;
                    *vs++ = pos.x();                *vs++ = pos.y();                *vs++ = 0.0f;
                    *vs++ = pos.x() + tileSize.x(); *vs++ = pos.y();                *vs++ = 0.0f;
                } catch (E_BaseException &e)
                {
                    *uvs++ = 0.f; *uvs++ = 0.f;
                    *uvs++ = 0.f; *uvs++ = 0.f;
                    *uvs++ = 0.f; *uvs++ = 0.f;
                    *uvs++ = 0.f; *uvs++ = 0.f;

                    *vs++ = pos.x();                *vs++ = pos.y() + tileSize.y(); *vs++ = 0.0f;
                    *vs++ = pos.x() + tileSize.x(); *vs++ = pos.y() + tileSize.y(); *vs++ = 0.0f;
                    *vs++ = pos.x();                *vs++ = pos.y();                *vs++ = 0.0f;
                    *vs++ = pos.x() + tileSize.x(); *vs++ = pos.y();                *vs++ = 0.0f;
                }

                pos.x() += tileSize.x();
            }

            pos.y() += tileSize.y();
        }

        m_vertices.create(len*3, VertexBuffer::DYNAMIC, vertices.getData());
        m_texCoords.create(len*2, VertexBuffer::DYNAMIC, texCoords.getData());
    }
}

void Map2dTileLayer::draw(const DrawInfo &drawInfo)
{
	if (!getActivity() || !getVisibility())
		return;

    // buffered tiling only if the tile set have a unique texture
    if (m_tileSet->getNumTextures() == 1)
    {
        fillBuffers();

        setUpModelView();

        m_tileSet->draw(m_vertices, m_texCoords);
    }
    else
    {
        clearDrawUpdate();

        // tile per tile draw
        setUpModelView();

        Camera *camera = getScene()->getActiveCamera();

        // compute the visible portion of the map
        Vector3 camPos = camera->getAbsoluteMatrix().getTranslation();

        // centered view position in tile unit
        Float centerCol = camPos.x() / m_tileSet->getTileSize().x();
        Float centerRow = camPos.y() / m_tileSet->getTileSize().y();

        // size of the area to display in tile unit
        Float w = (camera->getRight()-camera->getLeft()-1) / m_tileSet->getTileSize().x();
        Float h = (camera->getBottom()-camera->getTop()-1) / m_tileSet->getTileSize().y();

        //printf("center: x(%f %f) y(%f %f)\n", centerCol, w, centerRow, h);

        Vector2i tileSize = m_tileSet->getTileSize();
        Vector2i pos;

        // render with a background
        if (m_background)
        {
            // start position in tile unit, centered
            Int32 sx = (Int32)floor(centerCol-w/2.f);
            Int32 sy = (Int32)floor(centerRow-h/2.f);

            // end position in tile unit, centered
            Int32 ex = (Int32)ceil(centerCol + w/2.f);
            Int32 ey = (Int32)ceil(centerRow + h/2.f);

            // start y position in screen unit, centered
            pos.y() = Int32((Int32)sy * tileSize.y());

            // picking pass
            if (isPicking() && drawInfo.pass == DrawInfo::PICKING_PASS)
            {
                // normaly nothing to do
            }
            else if (drawInfo.pass == DrawInfo::AMBIENT_PASS)
            {
                for (Int32 y = sy; y < ey; ++y)
                {
                    // start x position in screen unit, centered
                    pos.x() = Int32((Int32)sx * tileSize.x());

                    for (Int32 x = sx; x < ex; ++x)
                    {
                        if (x < 0 || y < 0 || x >= Int32(m_columns) || y >= Int32(m_rows))
                        {
                            try {
                                const Map2dTileSet::Tile &tile = m_tileSet->getTile(m_backgroundTile);
                                m_tileSet->draw(pos, tile, 0, 1.f);
                            } catch (E_BaseException &e)
                            {
                                //printf("missing tile id:%i\n",m_backgroundTile);
                            }
                        }
                        else
                        {
                            try {
                                const Map2dTileSet::Tile &tile = m_tileSet->getTile(m_tileMap[y*m_columns + x]);
                                m_tileSet->draw(pos, tile, 0, 1.f);
                            } catch (E_BaseException &e)
                            {
                                //printf("missing tile id:%i\n",m_tileMap[y*m_columns + x]);
                            }
                        }

                        pos.x() += tileSize.x();
                    }

                    pos.y() += tileSize.y();
                }
            }
        }
        else // wihthout background
        {
            // start position in tile unit, centered
            Int32 sx = (Int32)(centerCol-w/2.f);
            Int32 sy = (Int32)(centerRow-h/2.f);

            if (sx < 0)
            {
                centerCol += -sx;
                sx = 0;
            }

            if (sy < 0)
            {
                centerRow += -sy;
                sy = 0;
            }

            // end position in tile unit, centered
            Int32 ex = o3d::clamp((Int32)ceil(centerCol + w/2.f), 0, (Int32)m_columns);
            Int32 ey = o3d::clamp((Int32)ceil(centerRow + h/2.f), 0, (Int32)m_rows);

            // start y position in screen unit, centered
            pos.y() = Int32((Int32)sy * tileSize.y());

            // picking pass
            if (isPicking() && drawInfo.pass == DrawInfo::PICKING_PASS)
            {
                // normaly nothing to do
            }
            else if (drawInfo.pass == DrawInfo::AMBIENT_PASS)
            {
                for (Int32 y = sy; y < ey; ++y)
                {
                    // start x position in screen unit, centered
                    pos.x() = Int32((Int32)sx * tileSize.x());

                    for (Int32 x = sx; x < ex; ++x)
                    {
                        try {
                            const Map2dTileSet::Tile &tile = m_tileSet->getTile(m_tileMap[y*m_columns + x]);
                            m_tileSet->draw(pos, tile, 0, 1.f);
                        } catch (E_BaseException &e)
                        {
                            //printf("missing tile id:%i\n",m_tileMap[y*m_columns + x]);
                        }

                        pos.x() += tileSize.x();
                    }

                    pos.y() += tileSize.y();
                }
            }
        }
    }
}

void Map2dTileLayer::update()
{
    if (hasUpdated())
        setDrawUpdate();

    clearUpdated();
}

