/**
 * @file map2dtileset.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MAP2DTILESET_H
#define _O3D_MAP2DTILESET_H

#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/shader/shader.h"
#include "o3d/core/memorydbg.h"

#include <set>

namespace o3d {

/**
 * @brief A single sized tiles set with auto-generation.
 * A tile set contain one or many 2d textures and information about how
 * the tiles are defined and generated.
 * The directory contain many files, and the name (without the extension) define
 * the real usage name of the tile. The tile set build one or many atlases textures
 * from these set of files.
 * Only PNG files are supported because they support transparency.
 * For animated tile an xml file must define the animation. The name of the xml file
 * without the extension (.xml) define the name of the animated tile.
 * Note that the file name 'tile.xml' is reserved for the màpping of the tile.
 *
 * <?xml version="1.0" standalone="no" ?>
 * <!-- This file contain animation for myTile -->
 * <Tile name="myTile">
 * 	<Frame id="0" file="filename.png" delay="100" />
 *     [...]
 * </Tile>
 *
 * Defining many frames, with the related tile filename and the delay (in ms) of display.
 * 'id' is just informative. The real frame order is the order of the elements in
 * the xml file.
 *
 * About the "tile.xml" file, each tile folder must contain a such file of the form :
 *
 * <?xml version="1.0" standalone="no" ?>
 * <!-- This file contain animation for myTileSet -->
 * <Tiles name="myTileSet" tileWidth="128" tileHeight="92" textureWidth="2048" textureHeight="1024">
 * 	<Tile id="0" name="filenameWithoutExt" />
 *     [...]
 * </Tiles>
 *
 * Each Tile entry contain the mapping between the name (filename without extension) and
 * a unique id. id must start at 0 and be adgacents. tileWidth/Height represents
 * the size of a tile. Each tile must have the same size. An exception is thrown if a
 * tile image has a different size.
 * textureWidth/Height defines the size of an atlas texture and must be power of two.
 *
 * @todo Use of a TEXTURE_2D_ARRAY in place of multiple-atlases
 *
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-08-03
 */
class O3D_API Map2dTileSet : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(Map2dTileSet)

	Map2dTileSet(BaseObject *parent, const String &tileSetPath = "");

	virtual ~Map2dTileSet();

	/**
	 * Load a tile set path and return the number of loaded tiles.
	 * Simple generation of a unique texture containing any images of the folder.
	 * Each tile must have the same size.
	 * @param tileSetPath Path where to find the tile with a "tile.xml" file.
	 * @return Number of loaded tiles.
	 */
	UInt32 load(const String &tileSetPath);

	//! Check for the existence of a tile name for a specific size.
	Bool isExists(const String &name) const;

	/**
	 * @brief Draw an tile given its size and name.
	 * @param pos Position.
	 * @param name Name of the tile.
	 * @param intensity Color intensity. 1 mean full intensity.
	 */
	void draw(
			const Vector2i &pos,
			const String &name,
			Float intensity = 1.0f);

    /**
     * @brief draw An array of vertices as triangle strips using full intensity,
     *        and a single texture.
     * @param vertices
     * @param texCoords
     * @param texture
     */
    void draw(
            ArrayBufferf &vertices,
            ArrayBufferf &texCoords);

	//! Get the tile set path.
	inline const String& getPath() const { return m_pathName; }

	//! Get the size of the tile.
	inline const Vector2i& getTileSize() const { return m_tileSet.tileSize; }

	//! Get the size of the textures.
	inline const Vector2i& getTextureSize() const { return m_textureSize; }

	//! Get the number of textures.
	inline UInt32 getNumTextures() const { return m_tileSet.textures.size(); }

public:

	//! Define a tile element
	struct TileDef
	{
		Float sU, sV;
		Float eU, eV;

		TileDef() :
			sU(0.f),
			sV(0.f),
			eU(0.f),
			eV(0.f)
		{
		}

		TileDef(const TileDef &dup) :
			sU(dup.sU),
			sV(dup.sV),
			eU(dup.eU),
			eV(dup.eV)
		{
		}

		inline TileDef& operator= (const TileDef &dup)
		{
			sU = dup.sU;
			sV = dup.sV;
			eU = dup.eU;
			eV = dup.eV;

			return *this;
		}
	};

	//! A frame element for animated tile.
	struct Frame
	{
		UInt32 delay;
		TileDef tileDef;
		Texture2D* texture;     //!< Texture object.
	};

	typedef std::vector<Frame> T_FrameVector;
	typedef T_FrameVector::iterator IT_FrameVector;
	typedef T_FrameVector::const_iterator CIT_FrameVector;

	//! Definition of a tile.
	struct Tile
	{
		T_FrameVector frames;   //!< Vector of frame. Only one if not animated.
		String name;            //!< Tile name.
		UInt32 id;          //!< Tile id.
	};

	//! Get information about a tile.
	const Tile &getTile(UInt32 id) const;

	/**
	 * @brief Draw a tile given its coordinates and texture pointer.
	 * @param pos Position.
	 * @param tile Tile reference.
	 * @param frame Frame number (0 only for static tiles).
	 * @param intensity Color intensity. 1 mean full intensity.
	 */
	void draw(
			const Vector2i &pos,
			const Tile &tile,
			UInt32 frame,
			Float intensity = 1.0f);

	/**
	 * @brief Draw a tile given its coordinates and texture pointer using a color mask.
	 * @param pos Position.
	 * @param tile Tile reference.
	 * @param frame Frame number (0 only for static tiles).
	 * @param color Color mask.
	 */
	void draw(
			const Vector2i &pos,
			const Tile &tile,
			UInt32 frame,
			const Color& color);

	/**
	 * @brief Draw a tile in picking mode.
	 * @param pos Position.
	 * @param tile Tile reference.
	 * @param frame Frame number (0 only for static tiles).
	 */
	void drawPicking(
			const Vector2i &pos,
			const Tile &tile,
			UInt32 frame,
            UInt32 pickableId/*const Color &pickColor*/);

private:

	typedef std::map<UInt32, Tile> T_TileMap;
	typedef T_TileMap::iterator IT_TileMap;
	typedef T_TileMap::const_iterator CIT_TileMap;

	std::map<String, UInt32> m_nameToId;

	struct TileSet
	{
		//! A list of atlases textures containing the tiles.
		std::list<SmartObject<Texture2D> > textures;

		T_TileMap tileMap;   //!< Definition of tiles.
		Vector2i tileSize;   //!< Size of the tiles.
	};

	Vector2i m_textureSize;

	TileSet m_tileSet;

	String m_pathName;

	ArrayBufferf m_vertices;
	ArrayBufferf m_texCoords;

	struct DefaultShader
	{
		ShaderInstance inst;

		Int32 a_vertex;
		Int32 a_texCoords;

		Int32 u_modelViewProjectionMatrix;
		Int32 u_texture;
		Int32 u_alpha;
	};

	DefaultShader m_defaultShader;

	struct ColorShader
	{
		ShaderInstance inst;

		Int32 a_vertex;
		Int32 a_texCoords;

		Int32 u_modelViewProjectionMatrix;
		Int32 u_texture;
		Int32 u_color;
	};

	ColorShader m_colorShader;

	struct PickingShader
	{
		ShaderInstance inst;

		Int32 a_vertex;

		Int32 u_modelViewProjectionMatrix;
        Int32 u_picking;
	};

	PickingShader m_pickingShader;

    Texture2D *m_firstTexture;

	//! Browse a path to search any tiles.
	//! @return The number of found tiles.
	UInt32 browseFolder(const String &path);

	//! Load an XML tiles file of definition.
	void loadXmlDef(const String &filename);

	//! Load the XML tile definition.
	void loadXmlFile(const String &filename, std::map<UInt32, String> &entries);

	struct TextureElt
	{
		Texture2D *texture;
		UInt32 x, y;
		
		TextureElt() :
			texture(nullptr),
			x(0),
			y(0)
		{}
	};

	//! Create a tile.
	TileDef generateTile(
			const String &filename,
			String &tileName,
			TextureElt &texture);

	//! Add a new texture with the predefined size.
	TextureElt addTexture();
};

} // namespace o3d

#endif // _O3D_MAP2DTILESET_H
