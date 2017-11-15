/**
 * @file cursormanager.h
 * @brief Mouse cursor manager.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-02-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CURSORMANAGER_H
#define _O3D_CURSORMANAGER_H

#include "iconset.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class CursorManager
//-------------------------------------------------------------------------------------
//! Mouse cursor manager. It use the cursor structure, with a primary forlder,
//! and sub-folder named by the size of the cursors contained in.
//! Eachsub-folder must have a file named 'cursor.xml'. It is defined as following :
//! 
//! <?xml version="1.0" standalone="no" ?>
//! <!-- This file contain hot spot definition for the cursors -->
//! <Cursors>
//!		<Cursor name="Default" xhot="0" yhot="0" />
//! [...]
//! </Cursors>
//! 
//! For each cursor it define an hot spot.
//! 
//! The icon folder must contain icon of PNG and XML format with a specific 
//! naming convention. 'cursor' as a prefix, followed by the name of the cursor,
//! and the extension (png for a static, xml for an animated icon).
//! For animated cursor see animated icon.
//!  - cursorDefault.png => default cursor state
//!  - cursorRight.png => cursor 'inverted' default state
//!  - cursorWait.png => wait cursor state
//!  - cursorUnavailable => unavailable cursor state
//!  - cursorMove.png => moving cursor state
//!  - cursorBorderUp.png => up border cursor state
//!  - cursorBorderDown.png => down border cursor state
//!  - cursorBorderLeft.png => left border cursor state
//!  - cursorBorderRight.png => right border cursor state
//!  - cursorBorderTopLeft.png => top-left border cursor state
//!  - cursorBorderTopRight.png => top-right border cursor state
//!  - cursorBorderBottomLeft.png => bottom-left border cursor state
//!  - cursorBorderBottomRight.png => bottom-right border cursor state
//!  - cursorText.png => show text input cursor state
//!  - cursorDrag.png => cursor dragging
//!  - cursorCanDrop.png => cursor drop possible
//!  - cursorArrow.png => cursor arrow
//!  - cursorPrecision.png => cursor precision
//!  - cursorPencil.png => cursor pencil
//!  - cursorQuestion.png => cursor question
//!  - cursorXXX.png where XXX represent a user defined name => any user defined states
//---------------------------------------------------------------------------------------
class O3D_API CursorManager : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(CursorManager)

	//! Standard cursor type.
	enum CursorType
	{
		NONE = 0,             //!< No cursor (disabled).
		DEFAULT = 1,          //!< Default cursor.
		RIGHT,                //!< Like default cursor but vertically inverted.
		WAIT,                 //!< Wait operation.
		UNAVAILABLE,          //!< Unavailable feature.
		BACKGROUND,           //!< Background operation.
		MOVE,                 //!< Move widget.
		BORDER_TOP,           //!< Border up.
		BORDER_BOTTOM,        //!< Border down.
		BORDER_LEFT,          //!< Border left.
		BORDER_RIGHT,         //!< Border right.
		BORDER_TOP_LEFT,      //!< Border top-left.
		BORDER_TOP_RIGHT,     //!< Border top-right.
		BORDER_BOTTOM_LEFT,   //!< Border bottom-left.
		BORDER_BOTTOM_RIGHT,  //!< Border bottom-right.
		TEXT,                 //!< Show text area.
		DRAG,                 //!< Dragging.
		CAN_DROP,             //!< Can drop the currently dragged object.
		ARROW,                //!< Straight arrow.
		PRECISION,            //!< Precision pointer.
		PENCIL,               //!< Pencil.
		QUESTION              //!< Question arrow.
	};

	//! Number of standard cursors types.
	static const Int32 NUM_CURSOR_TYPE = QUESTION + 1;

	//! Default constructor.
	CursorManager(BaseObject *parent);

	//! Virtual destructor.
	virtual ~CursorManager();

	//! Load the cursor theme. It must be defined like a @see IconSet.
	Bool load(const String &path);

	//! Set the current cursor according to a cursor type as enum.
	//! @param type Any of CursorType excepted USER_DEFINED value.
	void setCursor(CursorType type, UInt32 size = 32);

	//! Set the current cursor according to a cursor type as string.
	//! @param name The same name as the file name without the prefix 'mouse' and the file extension.
	//! For example with 'mouseDefault.png' use 'Default'.
	void setCursor(const String &name, UInt32 size = 32);

	//! Get the current cursor type. If the cursor is a user defined it returns USER_DEFINED.
	inline CursorType getCursorType() const { return m_cursorType; }

	//! Get the current cursor size.
	inline UInt32 getCursorSize() const { return m_size; }

	//! Get the current cursor name.
	inline const String& getCursorName() const { return m_cursorName; }

	//! Draw the current cursor at the specified location.
	void draw(const Vector2i &pos, Float intensity = 1.0f);

	//! convert a cursor type name string to its enum CursorType value
	CursorType string2CursorType(const String &str) const;

protected:

	struct Cursor
	{
		String name;
		Vector2i hotSpot;
	};

	typedef std::vector<Cursor> T_CursorVector;
	typedef T_CursorVector::iterator IT_CursorVector;
	typedef T_CursorVector::const_iterator CIT_CursorVector;

	typedef std::map<UInt32, T_CursorVector> T_CursorSubSetMap;
	typedef T_CursorSubSetMap::iterator IT_CursorSubSetMap;
	typedef T_CursorSubSetMap::const_iterator CIT_CursorSubSetMap;

	String m_path;        //!< Cursors path.

	T_CursorSubSetMap m_subSetMap;  //!< Hot spot for each cursor.

	IconSet m_iconSet;    //!< Icon set defining the cursors set.
	CursorType m_cursorType; //!< Current cursor type.
	String m_cursorName;  //!< Current cursor name.

	Vector2i m_offset;    //!< Current cursor offset.
	UInt32 m_size;    //!< Current cursor size.

	IconSet::Icon *m_icon; //!< Icon reference.

	UInt32 m_lastTime;    //!< Last draw time for animated icon.
	UInt32 m_lastFrame;   //!< Last played frame.
	
	std::map<String, CursorType> m_cursorNameToType;

	//! Browse a path to search any size folders.
	void browseFolder(const String &path);

	//! Load XML file 'cursor.xml'.
	void loadXmlFile(const String &filename, UInt32 size);

	//! Create the map of string cursor name to integer cursor type.
	void createStringToCursorTypeMap();

	//! Compute the frame index.
	UInt32 computeFrame();
};

} // namespace o3d

#endif // _O3D_CURSORMANAGER_H

