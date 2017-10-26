/**
 * @file map2dtileset.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/map2d/map2dtileset.h"
#include "o3d/core/architecture.h"
#include "o3d/core/xmldoc.h"
#include "o3d/core/diskfileinfo.h"
#include "o3d/engine/gl.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/virtualfilelisting.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/shader/shadermanager.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Map2dTileSet, ENGIME_MAP_2D_TILE_SET, SceneEntity)

// Write texture results into PNG file
//#define DEBUG_TEXTURE_OUT

Map2dTileSet::Map2dTileSet(BaseObject *parent, const String &tileSetPath) :
	SceneEntity(parent),
    m_vertices(getScene()->getContext()),
    m_texCoords(getScene()->getContext())
{
	// default shader, simple texture rendering with opacity
	Shader *shader = getScene()->getShaderManager()->addShader("gui/icon");
	shader->buildInstance(m_defaultShader.inst);

	m_defaultShader.inst.assign(0, 0, -1, "ALPHA_TEST_REF=0.0;ALPHA_FUNC_GREATER;");
	m_defaultShader.inst.build(Shader::BUILD_COMPILE_AND_LINK);

	m_defaultShader.a_vertex = m_defaultShader.inst.getAttributeLocation("a_vertex");
	m_defaultShader.a_texCoords = m_defaultShader.inst.getAttributeLocation("a_texCoords");

	m_defaultShader.u_modelViewProjectionMatrix = m_defaultShader.inst.getUniformLocation("u_modelViewProjectionMatrix");
	m_defaultShader.u_alpha = m_defaultShader.inst.getUniformLocation("u_alpha");
	m_defaultShader.u_texture = m_defaultShader.inst.getUniformLocation("u_texture");

	// color shader, simple texture rendering with color mask
	shader = getScene()->getShaderManager()->addShader("map2d/colorMask");
	shader->buildInstance(m_colorShader.inst);

	m_colorShader.inst.assign(0, 0, -1, "ALPHA_TEST_REF=0.0;ALPHA_FUNC_GREATER;");
	m_colorShader.inst.build(Shader::BUILD_COMPILE_AND_LINK);

	m_colorShader.a_vertex = m_colorShader.inst.getAttributeLocation("a_vertex");
	m_colorShader.a_texCoords = m_colorShader.inst.getAttributeLocation("a_texCoords");

	m_colorShader.u_modelViewProjectionMatrix = m_colorShader.inst.getUniformLocation("u_modelViewProjectionMatrix");
	m_colorShader.u_color = m_colorShader.inst.getUniformLocation("u_color");
	m_colorShader.u_texture = m_colorShader.inst.getUniformLocation("u_texture");

	// picking shader, simple color rendering (maybe see with opacity map)
	shader = getScene()->getShaderManager()->addShader("picking");
	shader->buildInstance(m_pickingShader.inst);

	m_pickingShader.inst.assign(0, 0, -1, "MESH;");
	m_pickingShader.inst.build(Shader::BUILD_COMPILE_AND_LINK);

	m_pickingShader.a_vertex = m_pickingShader.inst.getAttributeLocation("a_vertex");

	m_pickingShader.u_modelViewProjectionMatrix = m_pickingShader.inst.getUniformLocation("u_modelViewProjectionMatrix");
	m_pickingShader.u_pickingColor = m_pickingShader.inst.getUniformLocation("u_pickingColor");

	if (tileSetPath.isValid())
		load(tileSetPath);
}

Map2dTileSet::~Map2dTileSet()
{
//	// textures can be now deleted
//	for (SmartObject<Texture2D> texture : m_tileSet.textures)
//	{
//		if (texture.isValid())
//			texture->setPersistant(False);
//	}
}

UInt32 Map2dTileSet::load(const String &pathName)
{
	if (m_pathName.isValid())
		O3D_ERROR(E_InvalidOperation("The tile set is already constructed"));

	String lPath(pathName);
	lPath.replace('\\','/');
	lPath.trimRight('/');

	return browseFolder(lPath);
}

Bool Map2dTileSet::isExists(const String &name) const
{
	auto cit = m_nameToId.find(name);

	CIT_TileMap cit2 = m_tileSet.tileMap.find(cit->second);
	if (cit2 != m_tileSet.tileMap.end())
		return True;

	return False;
}

void Map2dTileSet::draw(const Vector2i &pos, const String &name, Float intensity)
{
	auto it = m_nameToId.find(name);

	IT_TileMap it2 = m_tileSet.tileMap.find(it->second);
	if (it2 != m_tileSet.tileMap.end())
	{
		// Internal draw method
		draw(pos, it2->second, 0, intensity);
    }
}

void Map2dTileSet::draw(VertexBufferObjf &vertices,
        VertexBufferObjf &texCoords)
{
    if (m_firstTexture == nullptr)
        return;

    // Draw a quad with the texture
    Context *glContext = getScene()->getContext();

    glContext->blending().setFunc(Blending::SRC_A__ONE_MINUS_SRC_A);
    glContext->disableDepthTest();

    m_defaultShader.inst.bindShader();

    m_defaultShader.inst.setConstMatrix4(
                m_defaultShader.u_modelViewProjectionMatrix,
                False,
                glContext->modelViewProjection());

    m_defaultShader.inst.setConstFloat(m_defaultShader.u_alpha, 1.0f);
    m_defaultShader.inst.setConstTexture(m_defaultShader.u_texture, m_firstTexture, 0);

    texCoords.attribute(m_defaultShader.a_texCoords, 2, 0, 0);
    vertices.attribute(m_defaultShader.a_vertex, 3, 0, 0);

    getScene()->drawArrays(P_TRIANGLE_STRIP, 0, vertices.getCount() / 3);

    glContext->bindTexture(TEXTURE_2D, 0);

    glContext->disableVertexAttribArray(m_defaultShader.a_vertex);
    glContext->disableVertexAttribArray(m_defaultShader.a_texCoords);

    m_defaultShader.inst.unbindShader();
}

const Map2dTileSet::Tile& Map2dTileSet::getTile(UInt32 id) const
{
	auto it = m_tileSet.tileMap.find(id);
	if (it != m_tileSet.tileMap.end())
		return it->second;

	O3D_ERROR(E_InvalidParameter("Invalid tile id"));
}

UInt32 Map2dTileSet::browseFolder(const String &path)
{
	VirtualFileListing fileListing;
	fileListing.setPath(path);

	fileListing.searchFirstFile();
	fileListing.setExt("*.png|*.xml");

	String lPath = path;
	lPath.replace('\\','/');
	lPath.trimRight('/');

	m_pathName = FileManager::instance()->getFullFileName(lPath);

	// load "tile.xml"
	std::map<UInt32, String> entries;
	loadXmlFile(m_pathName + '/' + "tile.xml", entries);

	FLItem *fileItem;

	T_StringList files;
	T_StringList inis;

	while ((fileItem = fileListing.searchNextFile()) != NULL)
	{
		if (fileItem->FileType == FILE_FILE)
		{
			if (fileItem->FileName.endsWith(".xml"))
			{
				if (fileItem->FileName != "tile.xml")
					inis.push_back(fileListing.getFileFullName());
			}
			else
				files.push_back(fileListing.getFileFullName());
		}
	}
/*
	// find the best size for the texture
	UInt32 mw = m_tileSet.tileSize.x() + 1;
	UInt32 mh = m_tileSet.tileSize.y() + 1;

	UInt32 cw = mw;
	UInt32 ch = mh;

	UInt32 texMax = static_cast<UInt32>(getScene()->getContext()->getTextureMaxSize());

	// dynamicaly adapt the width and the height to be close of a square
	while (((cw / mw) * (ch / mh)) < files.size())
	{
		if (cw > ch)
			ch += mh;
		else
			cw += mw;

		cw = nextPow2(cw);
		ch = nextPow2(ch);

		if ((cw > texMax) || (ch > texMax))
			O3D_ERROR(E_InvalidParameter("Unsupported font height. Desired height is to big for hardware"));
	}

	UInt32 x = 0, y = 0;

	UInt32 width = cw;
	UInt32 height = ch;
*/
	TextureElt elt;

	String name;
	UInt32 tileId;
	Frame frame;

	// load any images
	for (std::list<String>::iterator it = files.begin(); it != files.end(); ++it)
	{
		TileDef tileDef = generateTile(*it, name, elt);
		tileId = m_nameToId[name];

		// and insert the static tiles
		m_tileSet.tileMap[tileId] = Tile();
		Tile &tile = m_tileSet.tileMap[tileId];

		frame.delay = 0;
		frame.tileDef = tileDef;
		frame.texture = elt.texture;

		tile.name = name;
		tile.frames.push_back(frame);

		// erase done entries
		auto tileIt = entries.find(tileId);
		if (tileIt != entries.end())
			entries.erase(tileIt);
	}

	// fill others tile id (doubled)
	for (auto it = entries.begin(); it != entries.end(); ++it)
	{
		tileId = it->first;
		name = it->second;

		m_nameToId[name];

		// duplicate the tile element
		m_tileSet.tileMap[tileId] = m_tileSet.tileMap[m_nameToId[name]];
	}

#ifdef DEBUG_TEXTURE_OUT
	Image out; // for debug
	out.allocate(elt.texture->getWidth(), elt.texture->getHeight(), 4);
	UInt8 *data = out.getDataWrite();
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	out.save(getName() + String::print("%u.png", m_tileSet.textures.size()-1) ,Image::PNG, 0);
#endif

	// load ini files for animated tile, and insert the animated icons
	for (std::list<String>::iterator it = inis.begin(); it != inis.end(); ++it)
	{
		loadXmlDef(*it);
	}

	getScene()->getContext()->bindTexture(TEXTURE_2D, 0, True);

	return files.size();
}

//! Load an XML tile file of definition.
void Map2dTileSet::loadXmlDef(const String &filename)
{
	XmlDoc xmlDoc("import_tile_set");

    InStream *is = FileManager::instance()->openInStream(filename);
    xmlDoc.read(*is);
    deletePtr(is);

	String tileFile;
	String name;

	// Read XML Content
	// Element TextureTheme
	TiXmlElement *node = xmlDoc.getDoc()->FirstChildElement("Tile");
	if (!node)
		O3D_ERROR(E_InvalidFormat("Missing Tile token in " + filename));

	// Iterate trough frame elements
	TiXmlNode *frameNode = NULL;
	Int32 id;
	Int32 delay;

	// tile name. if empty use the name of the file like for static tiles.
	name = node->Attribute("name");
	if (name.isEmpty())
	{
		DiskFileInfo fileInfo(filename);
		name = fileInfo.getFileName();
		name.trimRight(fileInfo.getFileExt());
	}

	UInt32 tileId = m_nameToId[name];

	m_tileSet.tileMap[tileId] = Tile();
	Tile &tile = m_tileSet.tileMap[tileId];

	tile.name = name;
	tile.id = tileId;

	Frame frame;

	// for each frame
	while ((frameNode = node->IterateChildren("Frame",frameNode)) != NULL)
	{
		frameNode->ToElement()->Attribute("id", &id);
		tileFile = frameNode->ToElement()->Attribute("file");
		frameNode->ToElement()->Attribute("delay", &delay);

		frame.delay = delay;

		tileFile.trimRight(".png");
		tileFile.trimRight(".PNG");

		tileId = m_nameToId[tileFile];

		IT_TileMap it = m_tileSet.tileMap.find(tileId);
		if (it != m_tileSet.tileMap.end())
		{
			// take frame detail from the source static tile frame
			frame.tileDef = it->second.frames[0].tileDef;
			frame.texture = it->second.frames[0].texture;
		}
		else
		{
			frame.tileDef.sU = frame.tileDef.sV = frame.tileDef.eU = frame.tileDef.eV = 0.f;
			frame.texture = nullptr;
		}

		tile.frames.push_back(frame);
	}
}

void Map2dTileSet::loadXmlFile(const String &filename, std::map<UInt32, String> &entries)
{
	// load the tile definition file
	XmlDoc xmlDoc("import_tile");

    InStream *is = FileManager::instance()->openInStream(filename);
    xmlDoc.read(*is);
    deletePtr(is);

	// Read XML Content
	// Element Tiles
	TiXmlElement *node = xmlDoc.getDoc()->FirstChildElement("Tiles");
	if (!node)
		O3D_ERROR(E_InvalidFormat("Missing Tiles token in " + filename));

	Int32 w, h;

	if (node->ToElement()->Attribute("tileWidth", &w) &&
		node->ToElement()->Attribute("tileHeight", &h))
	{
		m_tileSet.tileSize.set(w, h);
	}
	else
		O3D_ERROR(E_InvalidFormat("Missing tile size in " + filename));

	if (node->ToElement()->Attribute("textureWidth", &w) &&
		node->ToElement()->Attribute("textureHeight", &h))
	{
		m_textureSize.set(w, h);
	}
	else
		O3D_ERROR(E_InvalidFormat("Missing texture size in " + filename));

	TiXmlNode *tileNode = NULL;

	String name;
	Int32 id;

	// for each tile
	while ((tileNode = node->IterateChildren("Tile", tileNode)) != NULL)
	{
		name = tileNode->ToElement()->Attribute("name");

		tileNode->ToElement()->Attribute("id", &id);

		m_nameToId[name] = (UInt32)id;
		entries[id] = name;
	}

	if ((m_textureSize.x() < m_tileSet.tileSize.x()) ||
		(m_textureSize.y() < m_tileSet.tileSize.y()))
		O3D_ERROR(E_InvalidFormat("Texture size must be greater than tile size"));
}

// Create a tile.
Map2dTileSet::TileDef Map2dTileSet::generateTile(
		const String &filename,
		String &tileName,
		TextureElt &textureElt)
{
    String fullfilename = FileManager::instance()->getFullFileName(filename);
    String path;
    File::getFileNameAndPath(fullfilename, tileName, path);

    InStream *is = FileManager::instance()->openInStream(fullfilename);
    Image picture(*is, PF_RGBA_U8);

	tileName.trimRight(".png");
	tileName.trimRight(".PNG");

    deletePtr(is);

	if ((picture.getWidth() != (UInt32)m_tileSet.tileSize.x()) ||
		(picture.getHeight() != (UInt32)m_tileSet.tileSize.y()))
	{
        O3D_ERROR(E_InvalidParameter("Image size differs from tile size in " + fullfilename));
	}

	if (textureElt.texture == nullptr)
		textureElt = addTexture();

	UInt32 width = textureElt.texture->getWidth();
	UInt32 height = textureElt.texture->getHeight();

    if ((textureElt.x + m_tileSet.tileSize.x()) > width)
	{
		textureElt.x = 0;
		textureElt.y += m_tileSet.tileSize.y() + 2;

        if ((textureElt.y + m_tileSet.tileSize.y()) > height)
		{
#ifdef DEBUG_TEXTURE_OUT
			Image out; // for debug
			out.allocate(width, height, 4);
			UInt8 *data = out.getDataWrite();
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			out.save(getName() + String::print("%u.png", m_tileSet.textures.size()-1), Image::PNG, 0);
#endif
			// add a new texture
			textureElt = addTexture();
	//		O3D_ERROR(E_InvalidResult("TileSet generation render out of region"));
		}
	}

	TileDef tile;
	tile.sU = (Float)textureElt.x / width;
	tile.sV = (Float)textureElt.y / height;
	tile.eU = (Float)(textureElt.x+m_tileSet.tileSize.x()) / width;
	tile.eV = (Float)(textureElt.y+m_tileSet.tileSize.y()) / height;

	glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			textureElt.x,
			textureElt.y,
			m_tileSet.tileSize.x(),
			m_tileSet.tileSize.y(),
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			picture.getData());

	// bottom border row + resting pixel
	SmartArrayUInt8 row((m_tileSet.tileSize.x()+1)*4);
    UInt32 *dst = nullptr;
	UInt32 *src = nullptr;

    if (textureElt.y+m_tileSet.tileSize.y() < height)
    {
        Int32 r = (m_tileSet.tileSize.y() - 1) * m_tileSet.tileSize.x() * 4;
        dst = (UInt32*)row.getData();

        for (Int32 x = 0; x < m_tileSet.tileSize.x(); ++x)
        {
            src = (UInt32*)(picture.getData() + x*4 + r);
            *dst++ = *src;
        }

        // resting pixel
        *dst = *src;

        glTexSubImage2D(
                    GL_TEXTURE_2D,
                    0,
                    textureElt.x,
                    textureElt.y + m_tileSet.tileSize.y(),
                    o3d::min(m_tileSet.tileSize.x()+1, (Int32)width),
                    1,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    row.getData());
    }

	// top border row
	if (textureElt.y > 0)
	{
		Int32 r = 0;
		dst = (UInt32*)row.getData();
		for (Int32 x = 0; x < m_tileSet.tileSize.x(); ++x)
        {
			src = (UInt32*)(picture.getData() + x*4 + r);
			*dst++ = *src;
		}

		// resting pixel
		*dst = *src;

		glTexSubImage2D(
					GL_TEXTURE_2D,
					0,
					textureElt.x,
					textureElt.y - 1,
                    o3d::min(m_tileSet.tileSize.x()+1, (Int32)width),
					1,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					row.getData());
	}

    // right border columns + resting pixel
	SmartArrayUInt8 column((m_tileSet.tileSize.y()+1)*4);

    if (textureElt.x+m_tileSet.tileSize.x() < width)
    {
        dst = (UInt32*)column.getData();
        Int32 c = (m_tileSet.tileSize.x()-1) * 4;
        for (Int32 y = 0; y < m_tileSet.tileSize.y(); ++y)
        {
            src = (UInt32*)(picture.getData() + y * m_tileSet.tileSize.x() * 4 + c);
            *dst++ = *src;
        }

        // resting pixel
        *dst = *src;

        glTexSubImage2D(
                    GL_TEXTURE_2D,
                    0,
                    textureElt.x + m_tileSet.tileSize.x(),
                    textureElt.y,
                    1,
                    o3d::min(m_tileSet.tileSize.y()+1, (Int32)height),
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    column.getData());
    }

	// left border colums
	if (textureElt.x > 0)
	{
		dst = (UInt32*)column.getData();
		for (Int32 y = 0; y < m_tileSet.tileSize.y(); ++y)
		{
			src = (UInt32*)(picture.getData() + y * m_tileSet.tileSize.x() * 4);
			*dst++ = *src;
		}

		// resting pixel
		*dst = *src;

		glTexSubImage2D(
					GL_TEXTURE_2D,
					0,
					textureElt.x - 1,
					textureElt.y,
					1,
                    o3d::min(m_tileSet.tileSize.y()+1, (Int32)height),
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					column.getData());
	}

	// resting pixel
    if (textureElt.x > 0 && textureElt.y > 0)
	{
		Rgb p = picture.getPixel(0, 0).getRgb();

		glTexSubImage2D(
					GL_TEXTURE_2D,
					0,
					textureElt.x - 1,
					textureElt.y - 1,
					1,
					1,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					&p);
	}

	textureElt.x += m_tileSet.tileSize.x() + 2;

	return tile;
}

Map2dTileSet::TextureElt Map2dTileSet::addTexture()
{
	UInt32 width = m_textureSize.x();
	UInt32 height = m_textureSize.y();

	Texture2D *texture = new Texture2D(this);
	texture->setFiltering(Texture::LINEAR_FILTERING);
	texture->setWrap(Texture::CLAMP);
	//texture->setPersistant(True);

	// create the texture with a black background
	ArrayUInt8 black((UInt8)0, width*height*4, 0);
	texture->create(False, width, height, PF_RGBA_U8, black.getData(), PF_RGBA_U8, True);
	black.destroy();

	m_tileSet.textures.push_back(SmartObject<Texture2D>(this, texture));

	TextureElt elt;
	elt.texture = texture;
	elt.x = 0;
	elt.y = 0;

    m_firstTexture = texture;

	return elt;
}

void Map2dTileSet::draw(
		const Vector2i &pos,
		const Tile &tile,
		UInt32 frame,
		Float intensity)
{
	const Frame &frameElt = tile.frames[frame];
	const TileDef &tileDef = frameElt.tileDef;
	Texture2D *texture = frameElt.texture;

	if (texture == nullptr)
		return;

	// Draw a quad with the texture
	Context *glContext = getScene()->getContext();

    glContext->blending().setFunc(Blending::SRC_A__ONE_MINUS_SRC_A);
	glContext->disableDepthTest();

	m_defaultShader.inst.bindShader();

	m_defaultShader.inst.setConstMatrix4(
				m_defaultShader.u_modelViewProjectionMatrix,
				False,
				glContext->modelViewProjection());

	m_defaultShader.inst.setConstFloat(m_defaultShader.u_alpha, intensity);
	m_defaultShader.inst.setConstTexture(m_defaultShader.u_texture, texture, 0);

	const Float texCoords[8] = {
		tileDef.sU, tileDef.eV,
		tileDef.eU, tileDef.eV,
		tileDef.sU, tileDef.sV,
		tileDef.eU, tileDef.sV };

	m_texCoords.create(8, VertexBuffer::STREAMED, texCoords, True);
	m_texCoords.attribute(m_defaultShader.a_texCoords, 2, 0, 0);

	const Float vertices[12] = {
        static_cast<Float>(pos.x()),                          static_cast<Float>(pos.y() + m_tileSet.tileSize.y()), 0.f,
        static_cast<Float>(pos.x() + m_tileSet.tileSize.x()), static_cast<Float>(pos.y() + m_tileSet.tileSize.y()), 0.f,
		static_cast<Float>(pos.x()),                          static_cast<Float>(pos.y() + 0),                      0.f,
        static_cast<Float>(pos.x() + m_tileSet.tileSize.x()), static_cast<Float>(pos.y() + 0),                      0.f };

	m_vertices.create(12, VertexBuffer::STREAMED, vertices, True);
	m_vertices.attribute(m_defaultShader.a_vertex, 3, 0, 0);

	getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);

	glContext->bindTexture(TEXTURE_2D, 0);

	glContext->disableVertexAttribArray(m_defaultShader.a_vertex);
	glContext->disableVertexAttribArray(m_defaultShader.a_texCoords);

	m_defaultShader.inst.unbindShader();
}

void Map2dTileSet::draw(
			const Vector2i &pos,
			const Tile &tile,
			UInt32 frame,
			const Color& color)
{
	const Frame &frameElt = tile.frames[frame];
	const TileDef &tileDef = frameElt.tileDef;
	Texture2D *texture = frameElt.texture;

	if (texture == nullptr)
		return;

	// Draw a quad with the texture
	Context *glContext = getScene()->getContext();

    glContext->blending().setFunc(Blending::SRC_A__ONE_MINUS_SRC_A);
	glContext->disableDepthTest();

	m_colorShader.inst.bindShader();

	m_colorShader.inst.setConstMatrix4(
				m_colorShader.u_modelViewProjectionMatrix,
				False,
				glContext->modelViewProjection());

	m_colorShader.inst.setConstColor(m_colorShader.u_color, color);
	m_colorShader.inst.setConstTexture(m_colorShader.u_texture, texture, 0);

	const Float texCoords[8] = {
		tileDef.sU, tileDef.eV,
		tileDef.eU, tileDef.eV,
		tileDef.sU, tileDef.sV,
		tileDef.eU, tileDef.sV };

	m_texCoords.create(8, VertexBuffer::STREAMED, texCoords, True);
	m_texCoords.attribute(m_colorShader.a_texCoords, 2, 0, 0);

	const Float vertices[12] = {
		static_cast<Float>(pos.x()),                          static_cast<Float>(pos.y() + m_tileSet.tileSize.y()), 0.f,
		static_cast<Float>(pos.x() + m_tileSet.tileSize.x()), static_cast<Float>(pos.y() + m_tileSet.tileSize.y()), 0.f,
		static_cast<Float>(pos.x()),                          static_cast<Float>(pos.y() + 0),                      0.f,
		static_cast<Float>(pos.x() + m_tileSet.tileSize.x()), static_cast<Float>(pos.y() + 0),                      0.f };

	m_vertices.create(12, VertexBuffer::STREAMED, vertices, True);
	m_vertices.attribute(m_colorShader.a_vertex, 3, 0, 0);

	getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);

	glContext->bindTexture(TEXTURE_2D, 0);

	glContext->disableVertexAttribArray(m_colorShader.a_vertex);
	glContext->disableVertexAttribArray(m_colorShader.a_texCoords);

	m_colorShader.inst.unbindShader();
}

void Map2dTileSet::drawPicking(
		const Vector2i &pos,
		const Map2dTileSet::Tile &tile,
		UInt32 frame,
		const Color &pickColor)
{
    //const Frame &frameElt = tile.frames[frame];
    //const TileDef &tileDef = frameElt.tileDef;

	// Draw a quad with the texture
	Context *glContext = getScene()->getContext();

    glContext->blending().setFunc(Blending::DISABLED);
	glContext->disableDepthTest();

	m_pickingShader.inst.bindShader();

	m_pickingShader.inst.setConstMatrix4(
				m_pickingShader.u_modelViewProjectionMatrix,
				False,
				glContext->modelViewProjection());

	m_pickingShader.inst.setConstColor(m_pickingShader.u_pickingColor, pickColor);

	const Float vertices[12] = {
		static_cast<Float>(pos.x()),                          static_cast<Float>(pos.y() + m_tileSet.tileSize.y()), 0.f,
		static_cast<Float>(pos.x() + m_tileSet.tileSize.x()), static_cast<Float>(pos.y() + m_tileSet.tileSize.y()), 0.f,
		static_cast<Float>(pos.x()),                          static_cast<Float>(pos.y() + 0),                      0.f,
		static_cast<Float>(pos.x() + m_tileSet.tileSize.x()), static_cast<Float>(pos.y() + 0),                      0.f };

	m_vertices.create(12, VertexBuffer::STREAMED, vertices, True);
	m_vertices.attribute(m_pickingShader.a_vertex, 3, 0, 0);

	getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);

	glContext->disableVertexAttribArray(m_pickingShader.a_vertex);

	m_pickingShader.inst.unbindShader();
}

