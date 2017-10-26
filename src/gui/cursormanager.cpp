/**
 * @file cursormanager.cpp
 * @brief Implementation of CursorManager.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2010-02-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/cursormanager.h"
#include "o3d/gui/guitype.h"

#include "o3d/core/xmldoc.h"
#include "o3d/core/diskfileinfo.h"
#include "o3d/core/filemanager.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(CursorManager, GUI_CURSOR_MANAGER, SceneEntity)

// Default constructor.
CursorManager::CursorManager(BaseObject *parent) :
	SceneEntity(parent),
	m_iconSet(this),
	m_cursorType(DEFAULT),
	m_cursorName(),
	m_size(0),
	m_icon(NULL),
	m_lastTime(0),
	m_lastFrame(0)
{
	createStringToCursorTypeMap();
}

// Virtual destructor.
CursorManager::~CursorManager()
{

}

// Create the map of string cursor name to integer cursor type.
void CursorManager::createStringToCursorTypeMap()
{
	m_cursorNameToType.insert(std::make_pair(String("None"), NONE));
	m_cursorNameToType.insert(std::make_pair(String("Default"), DEFAULT));
	m_cursorNameToType.insert(std::make_pair(String("Right"), RIGHT));
	m_cursorNameToType.insert(std::make_pair(String("Wait"), WAIT));
	m_cursorNameToType.insert(std::make_pair(String("Unavailable"), UNAVAILABLE));
	m_cursorNameToType.insert(std::make_pair(String("Background"), BACKGROUND));
	m_cursorNameToType.insert(std::make_pair(String("Move"), MOVE));
	m_cursorNameToType.insert(std::make_pair(String("BorderTop"), BORDER_TOP));
	m_cursorNameToType.insert(std::make_pair(String("BorderBottom"), BORDER_BOTTOM));
	m_cursorNameToType.insert(std::make_pair(String("BorderLeft"), BORDER_LEFT));
	m_cursorNameToType.insert(std::make_pair(String("BorderRight"), BORDER_RIGHT));
	m_cursorNameToType.insert(std::make_pair(String("BorderTopLeft"), BORDER_TOP_LEFT));
	m_cursorNameToType.insert(std::make_pair(String("BorderTopRight"), BORDER_TOP_RIGHT));
	m_cursorNameToType.insert(std::make_pair(String("BorderBottomLeft"), BORDER_BOTTOM_LEFT));
	m_cursorNameToType.insert(std::make_pair(String("BorderBottomRight"), BORDER_BOTTOM_RIGHT));
	m_cursorNameToType.insert(std::make_pair(String("Text"), TEXT));
	m_cursorNameToType.insert(std::make_pair(String("Drag"), DRAG));
	m_cursorNameToType.insert(std::make_pair(String("CanDrop"), CAN_DROP));
	m_cursorNameToType.insert(std::make_pair(String("Arrow"), ARROW));
	m_cursorNameToType.insert(std::make_pair(String("Precision"), PRECISION));
	m_cursorNameToType.insert(std::make_pair(String("Pencil"), PENCIL));
	m_cursorNameToType.insert(std::make_pair(String("Question"), QUESTION));
}

// Browse a path to search cursor.xml file from any size folders.
void CursorManager::browseFolder(const String &path)
{
	VirtualFileListing fileListing;
	fileListing.setPath(path);

	fileListing.searchFirstFile();

	FLItem *fileItem;

	String lPath = path;
	lPath.replace('\\','/');
	lPath.trimRight('/');

	while ((fileItem = fileListing.searchNextFile()) != NULL)
	{
		// a sub directory
		if (fileItem->FileType == FILE_DIR)
		{
			// we cannot directly found a version directory, or a sub directory
			if ((fileItem->FileName == String("8x8"))   ||
				(fileItem->FileName == String("16x16")) ||
				(fileItem->FileName == String("24x24")) ||
				(fileItem->FileName == String("32x32")) ||
				(fileItem->FileName == String("48x48")) ||
				(fileItem->FileName == String("64x64")) ||
				(fileItem->FileName == String("72x72")) ||
				(fileItem->FileName == String("96x96")) ||
				(fileItem->FileName == String("128x128")))
			{
				UInt32 size = fileItem->FileName.toUInt32(0);
				loadXmlFile(fileListing.getFileFullName() + '/' + "cursor.xml", size);
			}
		}
	}
}

// Load the cursor theme. It must be defined like a @see IconSet.
Bool CursorManager::load(const String &path)
{
	if (m_path.isValid())
		O3D_ERROR(E_InvalidOperation("The cursor manager is already constructed"));

	Bool result = m_iconSet.load(path);

	if (!m_iconSet.isExists(Vector2i(32, 32), "cursorDefault"))
		O3D_ERROR(E_InvalidParameter("At least, Default 32x32 cursor must be defined"));

	browseFolder(path);

	// set to default cursor
	setCursor(DEFAULT, 32);

	m_path = FileManager::instance()->getFullFileName(path);

	return result;
}

// Load XML file 'cursor.xml'.
void CursorManager::loadXmlFile(const String &filename, UInt32 size)
{
	// load the cursor definition file
	XmlDoc xmlDoc("import_cursor");

    InStream *is = FileManager::instance()->openInStream(filename);
    xmlDoc.read(*is);
    deletePtr(is);

	// create the sub-set
	m_subSetMap.insert(std::make_pair(size, T_CursorVector()));
	T_CursorVector &cursors = m_subSetMap[size];

	// Read XML Content
	// Element Cursors
	TiXmlElement *node = xmlDoc.getDoc()->FirstChildElement("Cursors");
	if (!node)
		O3D_ERROR(E_InvalidFormat("Missing Cursors token in " + filename));

	TiXmlNode *cursorNode = NULL;

	String name;
	Int32 xhot;
	Int32 yhot;

	// undefined hot spot
	Cursor cursor;
	cursors.resize(NUM_CURSOR_TYPE);

	CursorType cursorType;

	for (std::map<String, CursorType>::iterator it = m_cursorNameToType.begin();
		it != m_cursorNameToType.end(); ++it)
	{
		cursorType = string2CursorType(it->first);

		cursors[cursorType].name = it->first;
		cursors[cursorType].hotSpot.set(-1, -1);
	}

	// for each cursor
	while ((cursorNode = node->IterateChildren("Cursor",cursorNode)) != NULL)
	{
		name = cursorNode->ToElement()->Attribute("name");

		cursorNode->ToElement()->Attribute("xhot", &xhot);
		cursorNode->ToElement()->Attribute("yhot", &yhot);

		cursorType = string2CursorType(name);

		cursors[cursorType].hotSpot.set(xhot, yhot);
		cursors[cursorType].name = name;
	}
}

// Set the current cursor according to a cursor type as enum.
void CursorManager::setCursor(CursorType type, UInt32 size)
{
	if ((type != m_cursorType) || (size != m_size))
	{
		// hidden
		if (type == NONE)
		{
			m_icon = nullptr;
			m_cursorType = type;
			m_size = 0;
			m_offset.set(0,0);

			return;
		}

		CIT_CursorSubSetMap cit = m_subSetMap.find(size);

		if (cit == m_subSetMap.end())
			O3D_ERROR(E_InvalidParameter("Invalid cursor size"));

		const T_CursorVector &cursors = cit->second;

		const String iconName = String("cursor") + cursors[type].name;
		if (m_iconSet.isExists(Vector2i(size, size), iconName))
		{
			m_cursorType = type;
			m_cursorName = cursors[type].name;
			m_offset = cursors[type].hotSpot;
			m_size = size;
		}
		else
		{
			m_cursorType = DEFAULT;
			m_cursorName = "Default";
			m_offset = m_subSetMap[32][DEFAULT].hotSpot;
			m_size = 32;
		}

		// undefined hot spot, take a default
		if ((m_offset.x() == -1) || (m_offset.y() == -1))
		{
			switch (type)
			{
				case DEFAULT:
				case RIGHT:
				case UNAVAILABLE:
				case BACKGROUND:
				case DRAG:
				case CAN_DROP:
				case ARROW:
					m_offset.set(0,0);
					break;			
				case WAIT:
				case MOVE:
				case BORDER_TOP:
				case BORDER_BOTTOM:
				case BORDER_LEFT:
				case BORDER_RIGHT:
				case BORDER_TOP_LEFT:
				case BORDER_TOP_RIGHT:
				case BORDER_BOTTOM_LEFT:
				case BORDER_BOTTOM_RIGHT:
				case TEXT:
				case PRECISION:
				case PENCIL:
					m_offset.set(Int32(size>>1),Int32(size>>1));
					break;
				default:
					m_offset.set(0,0);
					break;
			}
		}

		m_icon = m_iconSet.getIconInfo(iconName, Vector2i(size, size));
	}
}

// Set the current cursor according to a cursor type as string.
void CursorManager::setCursor(const String &name, UInt32 size)
{
	if ((name != m_cursorName) || (size != m_size))
	{
		CursorType type = string2CursorType(name);
		setCursor(type, size);
	}
}

// Draw the current cursor at the specified location.
void CursorManager::draw(const Vector2i &pos, Float intensity)
{
	if (m_icon)
	{
		UInt32 frame = computeFrame();
		m_iconSet.draw(pos-m_offset, *m_icon, frame, intensity);
	}
}

// Compute the frame index.
UInt32 CursorManager::computeFrame()
{
	O3D_ASSERT(m_icon != NULL);

	if (m_icon->frames.size() <= 1)
		return 0;
	else
	{
		UInt32 time = System::getMsTime();
		if (m_lastTime == 0)
		{
			m_lastTime = time;
			m_lastFrame = 0;

			return 0;
		}

		UInt32 elapsed = time - m_lastTime;
		while (elapsed >= m_icon->frames[m_lastFrame].delay)
		{
			elapsed -= m_icon->frames[m_lastFrame].delay;

			++m_lastFrame;

			// loop
			if (m_lastFrame >= m_icon->frames.size())
				m_lastFrame = 0;
		}

		m_lastTime = time - elapsed;

		return m_lastFrame;
	}
}

// convert a cursor type name string to its enum CursorType value
CursorManager::CursorType CursorManager::string2CursorType(const String &str) const
{
	std::map<String, CursorType>::const_iterator cit = m_cursorNameToType.find(str);
	if (cit == m_cursorNameToType.end())
		O3D_ERROR(E_InvalidParameter("Unsupported cursor type"));

	return cit->second;
}

