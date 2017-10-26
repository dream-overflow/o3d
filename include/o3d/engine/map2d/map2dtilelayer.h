/**
 * @file map2dtilelayer.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MAP2DTILELAYER_H
#define _O3D_MAP2DTILELAYER_H

#include "map2dlayer.h"
#include "map2dtileset.h"

namespace o3d {

/**
 * @brief Map 2d layer dedicated to tile rendering.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-08-04
 */
class O3D_API Map2dTileLayer : public Map2dLayer
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(Map2dTileLayer)

	Map2dTileLayer(BaseObject *parent);

	//! Compute recursively the number of element of this branch
	virtual UInt32 getNumElt() const;

	virtual UInt32 getNumSon() const;

	//! Find an object/node given its name
	virtual const SceneObject* findSon(const String &name) const;
	//! Find an object/node given its name
	virtual SceneObject* findSon(const String &name);

	//! Find a scene object and return true if found.
	virtual Bool findSon(SceneObject *object) const;

	//
	// Transforms
	//

	//! Get the front transform or null if none (read only)
	virtual const Transform* getTransform() const;

	//! Get the front transform or null if none
	virtual Transform *getTransform();

	//
	//
	//

	//! Return false because it is a leaf.
	virtual Bool isNodeObject() const;

	//! Get the map size in pixels.
	const Vector2i& getSize() const;

	//! Get the number of rows.
	inline UInt32 getRows() const { return m_rows; }
	//! Get the number of columns.
	inline UInt32 getColumns() const { return m_columns; }

	virtual void draw(const DrawInfo &drawInfo);

    virtual void update();

	//! Define the tile set.
	void setTileSet(Map2dTileSet *tileSet);
	//! Get the current tile set.
	Map2dTileSet* getTileSet() { return m_tileSet.get(); }
	//! Get the current tile set (const version).
	const Map2dTileSet* getTileSet() const { return m_tileSet.get(); }

	//! Define a background tile in the case the map doesn't cover entirely the screen.
    void setBackgroundTile(UInt32 tileId);
	//! Get the background tile id.
	UInt32 getBackgroundTile() const { return m_backgroundTile; }

	//! Enable background tiling.
    void enableBackground();
	//! Disble background tiling.
    void disableBackground();
	//! Is background tiling.
	Bool isBackground() const { return m_background; }

	/**
	 * Define the tile map data and the size of the map is computed
	 * according to the size of the tile and the number of rows and columns
	 * @param data An array of byte with rows * columns elements
	 * @param rows Number of rows of tiles in the data
	 * @param columns Number or columns of tiles in the data
	 */
	 void setMapData(const SmartArrayUInt8 &data, UInt32 rows, UInt32 columns);

private:

	 UInt32 m_rows;
	 UInt32 m_columns;

	 Vector2i m_size;               //!< map size

	 SmartArrayUInt8 m_tileMap;     //!< Tile indexes for the entire map

	 SmartObject<Map2dTileSet> m_tileSet;  //!< Related tileset.

	 UInt32 m_backgroundTile;
	 Bool m_background;

     VertexBufferObjf m_vertices;
     VertexBufferObjf m_texCoords;

     void fillBuffers();
};

} // namespace o3d

#endif // _O3D_MAP2DTILELAYER_H

