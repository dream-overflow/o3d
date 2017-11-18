/**
 * @file sceneinfo.h
 * @brief Scene information object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-07-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SCENEINFO_H
#define _O3D_SCENEINFO_H

#include "o3d/core/file.h"
#include "o3d/core/datetime.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class SceneInfo
//-------------------------------------------------------------------------------------
//! Authoring scene information (for IO).
//---------------------------------------------------------------------------------------
class O3D_API SceneInfo
{
public:

	//! Default constructor
	SceneInfo();

	//! Set the title name
	inline void setSceneName(const String &name) { m_sceneName = name; }
	//! Get the title name
	inline const String& getSceneName() const { return m_sceneName; }

	//! Set the title
	inline void setTitle(const String &name) { m_title = name; }
	//! Get the title
	inline const String& getTitle() const { return m_title; }

	//! Set the author name
	inline void setAuthor(const String &name) { m_author = name; }
	//! Get the author name
	inline const String& getAuthor() const { return m_author; }

	//! Set the copyright
	inline void setCopyright(const String &name) { m_copyright = name; }
	//! Get the copyright
	inline const String& getCopyright() const { return m_copyright; }

	//! Set the comment
	inline void setComment(const String &name) { m_comment = name; }
	//! Get the comment
	inline const String& getComment() const { return m_comment; }

	//! Set the subject
	inline void setSubject(const String &name) { m_subject = name; }
	//! Get the subject
	inline const String& getSubject() const { return m_subject; }

	//! Set the revision
	inline void setRevision(UInt32 revision) { m_revision = revision; }
	//! Get the revision
	inline UInt32 getRevision() const { return m_revision; }

	//! Set the created date
    inline void setCreated(const DateTime& date) { m_created = date; }
	//! Get the created date
    inline const DateTime& getCreated() const { return m_created; }

	//! Set the modified date
    inline void setModified(const DateTime& date) { m_modified = date; }
	//! Get the modified date
    inline const DateTime& getModified() const { return m_modified; }

	//! Set the unit
	inline void setUnit(Float unit) { m_unit = unit; }
	//! Get the unit
	inline Float getUnit() const { return m_unit; }

	//! Set the unit name
	inline void setUnitName(const String &name) { m_unitName = name; }
	//! Get the unit name
	inline const String& getUnitName() const { return m_unitName; }

	// Serialisation
	Bool writeToFile(OutStream &os);
	Bool readFromFile(InStream &is);

private:

	String m_sceneName;
	String m_title;
	String m_author;
	String m_copyright;
	String m_comment;
	String m_subject;

	UInt32 m_revision;

    DateTime m_created;
    DateTime m_modified;

	Float m_unit;
	String m_unitName;
};

} // namespace o3d

#endif // _O3D_SCENEINFO_H
