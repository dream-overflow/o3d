/**
 * @file filelisting.h
 * @brief Directory file listing.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-09-05
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FILELISTING_H
#define _O3D_FILELISTING_H

#include "stringlist.h"
#include "datetime.h"
#include "memorydbg.h"

#include <vector>

//! MAX_PATH isn't defined under UNIX
#ifndef MAX_PATH
	#ifdef PATH_MAX
		#define MAX_PATH PATH_MAX
	#else
		#ifdef WIN32
			#define MAX_PATH 260
		#else
			#define MAX_PATH 1024
		#endif
	#endif
#endif

namespace o3d {

/**
 * @brief Contain data about a file or a logical drive (windows only).
 * FileSize and time informations are valid only if the type is FILE_FILE or FILE_DIR.
 */
struct FLItem
{
    String FileName;                 //!< full filename only
    FileTypes FileType;              //!< file type
	UInt64 FileSize;                 //!< file size if bytes

    DateTime CreateTime;             //!< creation date
    DateTime AccessTime;             //!< last access date
    DateTime WriteTime;              //!< last modification date
};

typedef std::vector<FLItem*> T_FLItemVector;
typedef T_FLItemVector::iterator O3D_IT_FLItemVector;
typedef T_FLItemVector::const_iterator O3D_CIT_FLItemVector;

/**
 * @brief Folder file listing on file system only.
 */
class O3D_API FileListing
{
public:

	//! default constructor
	FileListing();

	//! virtual destructor
	virtual ~FileListing();

	//! return the path of the listed directory, without trailing slash.
	String getPath() const;

	//! define working path (absolute or relative).
	virtual void setPath(const String &path);

	//! define the extensions filter (like "*.ini|*.zip|*.dat|*.pak|").
	void setExt(const String &extFilter);

	//! define the file type than we want find (FILE_DIR, FILE_FILE, FILE_BOTH)
	inline void setType(FileTypes type) { m_type = type; }

	//! get number of file
	virtual Int32 getNumFile() const;
	//! get the file
	virtual FLItem *getFile() const;

	//! get only the filename, without trailing slash.
	virtual String getFileName() const;
	//! get the absolute directory of the file, without trailing slash.
	virtual String getFilePath() const;
	//! get the complete file name, without trailing slash.
	virtual String getFileFullName() const;

	//! get the file type
	virtual FileTypes getFileType() const;
	//! get the file size
	virtual UInt64 getFileSize() const;
	//! get the creation file time
    virtual const DateTime& getCreationTime() const;
	//! get the last access file time
    virtual const DateTime& getAccesTime() const;
	//! get the last write file time
    virtual const DateTime& getWriteTime() const;

	//! initialize the file search
	virtual void searchFirstFile();
    //! return the next file (if null its finished)
	virtual FLItem *searchNextFile();

protected:

    Int32 m_curPos;              //!< current position used by FindNextFile
    String m_path;               //!< current search path
    FileTypes m_type;            //!< file or directory type we want to return

    T_StringList m_extFilter;    //!< extension filter
    T_FLItemVector m_fileList;   //!< file list

	// internal methods
	Bool isValidExt(const String &ext) const; //!< return true if this extension is in the extension list
	Bool isDriveDir() const;                  //!< return true if the relative or absolute path is a device directory
	Bool isAbsolutePath() const;              //!< return true if it's an absolute path
};

} // namespace o3d

#endif // _O3D_FILELISTING_H
