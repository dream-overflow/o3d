/**
 * @file iconset.h
 * @brief A set of icons.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2009-11-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ICONSET_H
#define _O3D_ICONSET_H

#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/shader/shader.h"
#include "o3d/core/memorydbg.h"

#include <set>

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class IconSet
//-------------------------------------------------------------------------------------
//! An icon set contain one texture and many icons definitions. Icon size are
//! normalized per folder (ie: 24x24, 32x32...).
//! It is build from an icons directory, that contain sub directory like :
//!  - 8x8
//!  - 16x16
//!  - 24x24
//!  - 26x25
//!  - ...
//! Each sub-directory contain many files, and the name (without the extension) define
//! the real usage name of the icon. The icon set build textures atlas from these set of
//! files for each sub directory.
//!
//! The limit is due to the size of the atlas.
//! Each icon must have the exact size describes by the parent folder.
//!
//! Only PNG files are supported because they support transparency.
//! For animated icon an XML file must define the animation. The name of the XML file
//! without the extension (.xml) define the name of the animated icon.
//! Note that the file name 'cursor.xml' is reserved.
//! 
//! <?xml version="1.0" standalone="no" ?>
//! <!-- This file contain animation for cursorBackground -->
//! <Icon name="myIcon">
//! 	<Frame id="0" file="filename.png" delay="100" />
//!     [...]
//! </Icon>
//! 
//! Defining many frames, with the related icon filename and the delay of display.
//! 'id' is just informative. The real frame order is the order of the elements in
//! the XML file.
//---------------------------------------------------------------------------------------
class O3D_API IconSet : public SceneEntity
{
	friend class Icon;
	friend class CursorManager;

public:

	//! Construct given a icon set directory.
	//! @param parent Parent object.
	//! @param iconSet The icon set file name or empty.
	IconSet(BaseObject *parent, const String &iconSetPath = "");

	//! Virtual destructor.
	virtual ~IconSet();

	//! Load an icon set path and return the number of loaded icons.
	UInt32 load(const String &iconSetPath);

	//! Check for the existence of a icon name for a specific size.
	Bool isExists(const Vector2i &size, const String &name) const;

	//! Draw an icon given its size and name.
	//! @param pos Position.
	//! @param size Size of the icon.
	//! @param name Name of the icon to draw.
	//! @param intensity Color intensity. 1 mean full intensity.
	void draw(
			const Vector2i &pos,
			const Vector2i &size,
			const String &name,
			Float intensity = 1.0f);

	//! Get the icon set path.
	inline const String& getPath() const { return m_pathName; }

protected:

	//! Define an icon element
	struct IconDef
	{
		Float sU, sV;
		Float eU, eV;

		IconDef() :
			sU(0.f),
			sV(0.f),
			eU(0.f),
			eV(0.f)
		{
		}

		IconDef(const IconDef &dup) :
			sU(dup.sU),
			sV(dup.sV),
			eU(dup.eU),
			eV(dup.eV)
		{
		}

		inline IconDef& operator= (const IconDef &dup)
		{
			sU = dup.sU;
			sV = dup.sV;
			eU = dup.eU;
			eV = dup.eV;

			return *this;
		}
	};

	//! A frame element for animated icon.
	struct Frame
	{
		UInt32 delay;
		IconDef iconDef;
	};

	typedef std::vector<Frame> T_FrameVector;
	typedef T_FrameVector::iterator IT_FrameVector;
	typedef T_FrameVector::const_iterator CIT_FrameVector;

	typedef std::map<String, IconDef> T_IconDefMap;
	typedef T_IconDefMap::iterator IT_IconDefMap;
	typedef T_IconDefMap::const_iterator CIT_IconDefMap;

	//! Definition of an icon.
	struct Icon
	{
		T_FrameVector frames;   //!< Vector of frame. Only one if not animated.
		String name;            //!< Icon name.
		Vector2i size;          //!< Icon size.
		Texture2D* texture;     //!< Texture object.
	};

	typedef std::map<String, Icon> T_IconMap;
	typedef T_IconMap::iterator IT_IconMap;
	typedef T_IconMap::const_iterator CIT_IconMap;

	struct IconSubSet
	{
		Texture2D* texture;  //!< Texture that contain a set of icons.
		T_IconMap iconMap;   //!< Definition of icons.
		Vector2i size;       //!< Size of the icon sub-set.

		IconSubSet() :
            texture(nullptr)
		{
		}
	};

	typedef std::map<Vector2i, IconSubSet> T_IconSubSetMap;
	typedef T_IconSubSetMap::iterator IT_IconSubSetMap;
	typedef T_IconSubSetMap::const_iterator CIT_IconSubSetMap;

	T_IconSubSetMap m_subSetMap;

	String m_pathName;

	VertexBufferObjf m_vertices;
	VertexBufferObjf m_texCoords;

	ShaderInstance m_shaderInstance;

	Int32 a_vertex;
	Int32 a_texCoords;

	Int32 u_modelViewProjectionMatrix;
	Int32 u_texture;
	Int32 u_alpha;

	//! Browse a path to search any size folders.
	//! @return The number of found icons.
	UInt32 browseFolder(const String &path);

	//! Browse a size folder.
	UInt32 browseSizeFolder(const String &path, const Vector2i &size);

	//! Load an XML icon file of definition.
	void loadXmlDef(const String &filename, IconSubSet &subSet);

	//! Create an icons.
	IconDef generateIcon(IconSubSet &subSet,
			const String &filename,
			UInt32 textureWidth,
			UInt32 textureHeight,
			UInt32 iconWidth,
			UInt32 iconHeight,
			UInt32 &x,
			UInt32 &y,
			String &iconName);

	//! Get information about an icon.
	Icon* getIconInfo(const String &name, const Vector2i &size);

	//! Draw an icon given its coordinates and texture pointer.
	//! @param icon Icon reference.
	//! @param frame Frame number (0 only for static icons).
	//! @param intensity Color intensity. 1 mean full intensity.
	void draw(const Vector2i &pos,
			Icon &icon,
			UInt32 frame,
			Float intensity = 1.0f);
};

} // namespace o3d

#endif // _O3D_ICONSET_H

