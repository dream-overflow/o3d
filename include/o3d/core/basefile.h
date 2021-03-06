/**
 * @file basefile.h
 * @brief This is the abstract file class.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-06-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_BASEFILE_H
#define _O3D_BASEFILE_H

#include "memorydbg.h"
#include "basedir.h"
#include "file.h"
#include "datetime.h"

namespace o3d {

/**
 * @brief File abstract model.
 * @details It take informations available in stats, truncates strings path and file name.
 * Its a very useful class to avoid you of some basics repetitive calls.
 */
class O3D_API BaseFile
{
protected:

	struct CachedData;

public:

	//! default constructor
    BaseFile(const String &filename);

	//! constructor with separates pathname and filename
    BaseFile(const String &pathname, const String &filename);

	//! constructor with separates pathname and filename
    BaseFile(const BaseDir &dir, const String &filename);

	//! copy constructor
    BaseFile(const BaseFile& dup);

    virtual ~BaseFile() = 0;

    virtual BaseFile* clone() const = 0;

	//! duplicator (take care it copy the state of the cached data)
    inline BaseFile& operator=(const BaseFile& dup)
	{
		m_type = dup.m_type;
		m_cached = dup.m_cached;
		m_isValid = dup.m_isValid;
		m_filename = dup.m_filename;
		m_fileExt = dup.m_fileExt;
		m_fullFilename = dup.m_fullFilename;

		return (*this);
	}

	//-----------------------------------------------------------------------------------
	// Setters like ctor
	//-----------------------------------------------------------------------------------

	//! set the filename
    virtual void setFile(const String &filename);

	//! set from a separates pathname and filename
    virtual void setFile(const String &pathname, const String &filename);

	//! set from separates pathname and filename
    virtual void setFile(const BaseDir &dir, const String &filename);

	//
	// Cache system
	//

	//! is use the cache informations data
	inline void useCache(Bool is) { m_cached = is; }

	//! is the data cache system enable ?
	inline Bool isCaching() const { return m_cached; }

	//! refresh the cache for file information (if cache is used)
	inline void refreshCache() { cacheDatas(); }

	//! get cache data structure
	inline const CachedData& getCachedData() const { return m_cachedData; }

	//-----------------------------------------------------------------------------------
	// Accessors
	//-----------------------------------------------------------------------------------

	//! return the file information type
    inline FileLocation getFileLocation() const { return m_type; }

	//! return the file name only (without the path)
	inline const String& getFileName() const { return m_filename; }

	//! return the full file name (with complete path)
	inline const String& getFullFileName() const { return m_fullFilename; }

	//! return the file extension only
	inline const String& getFileExt() const { return m_fileExt; }

	//! return the file path only, without trailing slash.
    String getFilePath() const;

	//! Get the file type.
	virtual FileTypes getType() = 0;

	//! Get the file size in bytes.
	virtual UInt64 getFileSize() = 0;

	//! check if the filename exists
	virtual Bool exists() = 0;

	//! is an absolute or relative filename ?
	virtual Bool isAbsolute() const = 0;

	//! is the file readable (check the rights)
	virtual Bool isReadable() = 0;

	//! is the file writable (check the rights)
	virtual Bool isWritable() = 0;

	//! is the file is hidden by the system
	virtual Bool isHidden() = 0;

	//! get the file owner
	virtual Int16 getOwnerId() = 0;

	//! get the file owner
	virtual const String& getOwnerName() = 0;

	//! return the owner group id
	virtual Int16 getGroupId() = 0;

	//! return the group string name
	virtual const String& getGroupName() = 0;

	//! make absolute. make - if relative - this file name absolute depend to the current working directory
	virtual Bool makeAbsolute() = 0;

	//! adapt a filename to this path. Note: the filename path and the Dir must have a common root.
    //! ex:
	//!     'c:/tool/one/two/file.txt' as source
	//!     'c:/tool/one/one/one' as path
	//!	     result in '../../two/file.txt'
	//! Only a string is returned because the file doesn't really exist.
    String makeRelative(const BaseDir &dir) const;

	//! get the creation date/time
    virtual const DateTime& getCreationDate() = 0;

	//! get the last access date/time
    virtual const DateTime& getLastAccessDate() = 0;

	//! get the last modification date/time
    virtual const DateTime& getModifiedDate() = 0;

	//! is the file a symbolic link
	virtual Bool isSymbolicLink() = 0;

	//! is the file is in the root directory
	virtual Bool isInRoot() const = 0;

	//-----------------------------------------------------------------------------------
	// Comparison operators
	//-----------------------------------------------------------------------------------

    //! check if two file are the same
    inline Bool operator== (const BaseFile &cmp) const
	{
		return (m_fullFilename == cmp.m_fullFilename);
	}

    //! check if two file are different
    inline Bool operator!= (const BaseFile &cmp) const
	{
		return (m_fullFilename != cmp.m_fullFilename);
	}

protected:

    FileLocation m_type;     //!< type of the file info

    Bool m_cached;           //!< is information are cached
    Bool m_isValid;          //!< is the file exist
	String m_filename;       //!< only the filename
	String m_fileExt;        //!< only the file extension
	String m_fullFilename;   //!< the full file name and path

	struct CachedData
	{
		FileTypes type;       //!< file type

        DateTime created;     //!< creation date
        DateTime lastAccess;  //!< last access date
        DateTime modified;    //!< last modification date

		Bool readable;      //!< is the file readable
		Bool writable;      //!< is the file writable
		Bool hidden;        //!< is it an hidden file
		Bool symLink;       //!< is it a symbolic link

		Int16 ownerId;      //!< owner id
        String ownerName;   //!< owner name

		Int16 groupId;      //!< owner group id
        String groupName;   //!< owner group name

		UInt64 fileSize;    //!< file size in bytes

		CachedData() :
			type(FILE_UNKNWON),
			readable(False),
			writable(False),
			hidden(False),
			symLink(False),
			ownerId(0),
			groupId(0),
			fileSize(0)
		{
		}

		CachedData(const CachedData &dup) :
			type(dup.type),
			created(dup.created),
			lastAccess(dup.lastAccess),
			modified(dup.modified),
			readable(dup.readable),
			writable(dup.writable),
			hidden(dup.hidden),
			symLink(dup.symLink),
			ownerId(dup.ownerId),
			ownerName(dup.ownerName),
			groupId(dup.groupId),
			groupName(dup.groupName),
			fileSize(dup.fileSize)
		{
		}

		CachedData& operator= (const CachedData &dup)
		{
			type = dup.type;
			created = dup.created;
			lastAccess = dup.lastAccess;
			modified = dup.modified;
			readable = dup.readable;
			writable = dup.writable;
			hidden = dup.hidden;
			symLink = dup.symLink;
			ownerId = dup.ownerId;
			ownerName = dup.ownerName;
			groupId = dup.groupId;
			groupName = dup.groupName;
			fileSize = dup.fileSize;

			return *this;
		}
	};

	CachedData m_cachedData;    //!< caches informations

	//! fill the CachedData structure if cache is enable
	virtual void cacheDatas() = 0;
};

} // namespace o3d

#endif // _O3D_BASEFILE_H
