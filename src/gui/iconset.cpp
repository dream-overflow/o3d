/**
 * @file iconset.cpp
 * @brief Implementation of IconSet.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2009-11-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/engine/glextdefines.h"
#include "o3d/gui/iconset.h"

#include "o3d/core/architecture.h"
#include "o3d/core/xmldoc.h"
#include "o3d/core/fileinfo.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/virtualfilelisting.h"
#include "o3d/gui/gui.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/shader/shadermanager.h"

using namespace o3d;

// Construct given a icon set XML file.
IconSet::IconSet(BaseObject *parent, const String &iconSetPath) :
	SceneEntity(parent),
    m_vertices(getScene()->getContext()),
    m_texCoords(getScene()->getContext())
{
	Shader *shader = getScene()->getShaderManager()->addShader("gui/icon");
	shader->buildInstance(m_shaderInstance);

	m_shaderInstance.assign(0, 0, -1, "ALPHA_TEST_REF=0.0;ALPHA_FUNC_GREATER;");
	m_shaderInstance.build(Shader::BUILD_COMPILE_AND_LINK);

	a_vertex = m_shaderInstance.getAttributeLocation("a_vertex");
	a_texCoords = m_shaderInstance.getAttributeLocation("a_texCoords");

	u_modelViewProjectionMatrix = m_shaderInstance.getUniformLocation("u_modelViewProjectionMatrix");
	u_alpha = m_shaderInstance.getUniformLocation("u_alpha");
	u_texture = m_shaderInstance.getUniformLocation("u_texture");

	if (iconSetPath.isValid())
		load(iconSetPath);
}

// Virtual destructor.
IconSet::~IconSet()
{
	for (IT_IconSubSetMap it = m_subSetMap.begin(); it != m_subSetMap.end(); ++it)
	{
		deletePtr(it->second.texture);
	}
}

// Load an icon set path.
UInt32 IconSet::load(const String &pathName)
{
	if (m_pathName.isValid())
		O3D_ERROR(E_InvalidOperation(String("The icon set is already constructed for ") + m_pathName));

	String lPath(pathName);
	lPath.replace('\\','/');
	lPath.trimRight('/');

	return browseFolder(lPath);
}

// Check for the existence of a icon name for a specific size.
Bool IconSet::isExists(const Vector2i &size, const String &name) const
{
	CIT_IconSubSetMap cit = m_subSetMap.find(size);
	if (cit != m_subSetMap.end())
	{
		CIT_IconMap cit2 = cit->second.iconMap.find(name);
		if (cit2 != cit->second.iconMap.end())
			return True;
	}

	return False;
}

// Draw an icon given its size and name.
void IconSet::draw(const Vector2i &pos, const Vector2i &size, const String &name, Float intensity)
{
	IT_IconSubSetMap it = m_subSetMap.find(size);
	if (it != m_subSetMap.end())
	{
		IT_IconMap it2 = it->second.iconMap.find(name);
		if (it2 != it->second.iconMap.end())
		{
			// Internal draw method
			draw(pos, it2->second, 0, intensity);
		}
	}
}

// Browse a path to search any size folders.
UInt32 IconSet::browseFolder(const String &path)
{
	VirtualFileListing fileListing;
	fileListing.setPath(path);

	fileListing.searchFirstFile();

	FLItem *fileItem;
	UInt32 numIcons = 0;

	String lPath = path;
	lPath.replace('\\','/');
	lPath.trimRight('/');

	Int32 xPos;
	Vector2i size;
	m_pathName = FileManager::instance()->getFullFileName(lPath);

	while ((fileItem = fileListing.searchNextFile()) != NULL)
	{
		// a sub directory
		if (fileItem->FileType == FILE_DIR)
		{
			// detect the size of the folder
			xPos = fileItem->FileName.find('x');

			if ((xPos > 0) && xPos < Int32(fileItem->FileName.length()-1))
			{
				size.x() = fileItem->FileName.toInt32();
				size.y() = fileItem->FileName.toInt32((UInt32)xPos+1);

				if (size.x() > 0 && size.y() > 0)
				{
					numIcons += browseSizeFolder(fileListing.getFileFullName(), size);
				}
			}
		}
	}

	return numIcons;
}

// Browse a size folder.
UInt32 IconSet::browseSizeFolder(const String &path, const Vector2i &size)
{
	VirtualFileListing fileListing;
	fileListing.setPath(path);

	fileListing.searchFirstFile();
	fileListing.setExt("*.png|*.xml");

	FLItem *fileItem;

	Texture2D *texture = new Texture2D(this);
	texture->setFiltering(Texture::NO_FILTERING);
	texture->setWrap(Texture::CLAMP);
	texture->setPersistant(True);

	m_subSetMap.insert(std::make_pair(size, IconSubSet()));
	IconSubSet &subSet = m_subSetMap[size];

	subSet.texture = texture;
	subSet.size = size;

	std::list<String> files;
	std::list<String> inis;

	while ((fileItem = fileListing.searchNextFile()) != NULL)
	{
		if (fileItem->FileType == FILE_FILE)
		{
			if (fileItem->FileName.endsWith(".xml"))
			{
				if (fileItem->FileName != "cursor.xml")
					inis.push_back(fileListing.getFileFullName());
			}
			else
				files.push_back(fileListing.getFileFullName());
		}
	}

	// find the best size for the texture atlas
	UInt32 mw = size.x() + 1;
	UInt32 mh = size.y() + 1;

	UInt32 cw = mw;
	UInt32 ch = mh;

	UInt32 texMax = static_cast<UInt32>(getScene()->getContext()->getTextureMaxSize());

	// dynamically adapt the width and the height to be close of a square
	while (((cw / mw) * (ch / mh)) < files.size())
	{
		if (cw > ch)
			ch += mh;
		else
			cw += mw;

		cw = nextPow2(cw);
		ch = nextPow2(ch);

		if ((cw > texMax) || (ch > texMax))
			O3D_ERROR(E_InvalidParameter("Unsupported texture size. Desired size is to big for hardware"));
	}

	UInt32 x = 0, y = 0;

	UInt32 width = cw;
	UInt32 height = ch;

	// create the texture with a black background
	ArrayUInt8 black((UInt8)0, width*height*4, 0);
	texture->create(False, width, height, PF_RGBA_U8, black.getData(), PF_RGBA_U8, True);
	black.destroy();

	T_IconDefMap iconDefMap;
	String name;
	Frame frame;

	// load any pictures
	for (std::list<String>::iterator it = files.begin(); it != files.end(); ++it)
	{
		iconDefMap[name] = generateIcon(subSet, *it, width, height, size.x(), size.y(), x, y, name);

		// and insert the static icons
		subSet.iconMap[name] = Icon();
		Icon &icon = subSet.iconMap[name];

		frame.delay = 0;
		frame.iconDef = iconDefMap[name];
	
		icon.name = name;
		icon.size = size;
		icon.frames.push_back(frame);
		icon.texture = texture;
	}

	// load ini files for animated icons, and insert the animated icons
	for (std::list<String>::iterator it = inis.begin(); it != inis.end(); ++it)
	{
		loadXmlDef(*it, subSet);
	}

	getScene()->getContext()->bindTexture(TEXTURE_2D, 0, True);

	return files.size();
}

//! Load an XML icon file of definition.
void IconSet::loadXmlDef(const String &filename, IconSubSet &subSet)
{
	XmlDoc xmlDoc("import_icon_set");

    InStream *is = FileManager::instance()->openInStream(filename);
    xmlDoc.read(*is);
    deletePtr(is);

	String iconFile;
	String name;

	// Read XML Content
	// Element TextureTheme
	TiXmlElement *node = xmlDoc.getDoc()->FirstChildElement("Icon");
	if (!node)
		O3D_ERROR(E_InvalidFormat("Missing Icon token in " + filename));
	
	// Iterate trough frame elements
	TiXmlNode *frameNode = NULL;
	Int32 id;
	Int32 delay;

	// icon name. if empty use the name of the file like for static icons.
	name = node->Attribute("name");
	if (name.isEmpty())
	{
		FileInfo fileInfo(filename);
		name = fileInfo.getFileName();
		name.trimRight(fileInfo.getFileExt());
	}

	subSet.iconMap[name] = Icon();
	Icon &icon = subSet.iconMap[name];

	icon.name = name;
	icon.size = subSet.size;
	icon.texture = subSet.texture;

	Frame frame;

	// for each frame
	while ((frameNode = node->IterateChildren("Frame",frameNode)) != NULL)
	{
		frameNode->ToElement()->Attribute("id", &id);
		iconFile = frameNode->ToElement()->Attribute("file");
		frameNode->ToElement()->Attribute("delay", &delay);

		frame.delay = delay;

		iconFile.trimRight(".png");
		iconFile.trimRight(".PNG");

		IT_IconMap it = subSet.iconMap.find(iconFile);
		if (it != subSet.iconMap.end())
			frame.iconDef = it->second.frames[0].iconDef;
		else
			frame.iconDef.sU = frame.iconDef.sV = frame.iconDef.eU = frame.iconDef.eV = 0.f;

		icon.frames.push_back(frame);
	}
}

// Create an icons.
IconSet::IconDef IconSet::generateIcon(IconSubSet &subSet,
		const String &filename,
		UInt32 textureWidth,
		UInt32 textureHeight,
		UInt32 iconWidth,
		UInt32 iconHeight,
		UInt32 &x,
		UInt32 &y,
		String &iconName)
{
    InStream *is = FileManager::instance()->openInStream(filename);
    Image picture(*is, PF_RGBA_U8);

    String path;
    File::getFileNameAndPath(filename, iconName, path);

	iconName.trimRight(".png");
	iconName.trimRight(".PNG");

    deletePtr(is);

    if ((x + iconWidth) > textureWidth)
	{
		x = 0;
		y += iconHeight + 1;

        if ((y + iconHeight) > textureHeight)
			O3D_ERROR(E_InvalidResult(String("IconSet generation render out of region when adding ") + iconName));
	}

	IconDef icon;
	icon.sU = (Float)x / textureWidth;
	icon.sV = (Float)y / textureHeight;
	icon.eU = (Float)(x+iconWidth) / textureWidth;
	icon.eV = (Float)(y+iconHeight) / textureHeight;

	glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			x,
			y,
			iconWidth,
			iconHeight,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			picture.getData());

	x += iconWidth + 1;

	return icon;
}

// Get information about an icon.
IconSet::Icon* IconSet::getIconInfo(const String &name, const Vector2i &size)
{
	IT_IconSubSetMap it = m_subSetMap.find(size);
	if (it != m_subSetMap.end())
	{
		IT_IconMap it2 = it->second.iconMap.find(name);
		if (it2 != it->second.iconMap.end())
		{
			return &it2->second;
		}
	}

	// not found
	O3D_ERROR(E_InvalidParameter(String("Icon name/size doesn't exists into the icon set: ") + name + " " + size));

	return NULL;
}

void IconSet::draw(
		const Vector2i &pos,
		Icon &icon,
		UInt32 frame,
		Float intensity)
{
	if (icon.texture)
	{
		IconDef &iconDef = icon.frames[frame].iconDef;

		// Draw a quad with the texture
		Context *glContext = getScene()->getContext();

        glContext->blending().setFunc(Blending::SRC_A__ONE_MINUS_SRC_A);
		glContext->disableDepthTest();

		m_shaderInstance.bindShader();

		m_shaderInstance.setConstMatrix4(
				u_modelViewProjectionMatrix,
				False,
				glContext->modelViewProjection());

		m_shaderInstance.setConstFloat(u_alpha, intensity);
		m_shaderInstance.setConstTexture(u_texture, icon.texture, 0);

		const Float texCoords[8] = {
				iconDef.sU, iconDef.eV,
				iconDef.eU, iconDef.eV,
				iconDef.sU, iconDef.sV,
				iconDef.eU, iconDef.sV };

		m_texCoords.create(8, VertexBuffer::STREAMED, texCoords, True);
		m_texCoords.attribute(a_texCoords, 2, 0, 0);

		const Float vertices[12] = {
				static_cast<Float>(pos.x()),                 static_cast<Float>(pos.y() + icon.size.y()), 0.f,
				static_cast<Float>(pos.x() + icon.size.x()), static_cast<Float>(pos.y() + icon.size.y()), 0.f,
				static_cast<Float>(pos.x()),                 static_cast<Float>(pos.y() + 0),             0.f,
				static_cast<Float>(pos.x() + icon.size.x()), static_cast<Float>(pos.y() + 0),             0.f };

		m_vertices.create(12, VertexBuffer::STREAMED, vertices, True);
		m_vertices.attribute(a_vertex, 3, 0, 0);

		getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);

		glContext->bindTexture(TEXTURE_2D, 0);

		glContext->disableVertexAttribArray(a_vertex);
		glContext->disableVertexAttribArray(a_texCoords);

		m_shaderInstance.unbindShader();
	}
}

