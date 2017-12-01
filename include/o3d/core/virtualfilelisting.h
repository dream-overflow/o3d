/**
 * @file virtualfilelisting.h
 * @brief Virtual file and disk file listing.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date $2005-09-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_VIRTUALFILELISTING_H
#define _O3D_VIRTUALFILELISTING_H

#include "memorydbg.h"

#include "filelisting.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class VirtualFileListing
//-------------------------------------------------------------------------------------
//! Virtual file and disk file listing.
//---------------------------------------------------------------------------------------
class O3D_API VirtualFileListing : public FileListing
{
public:

	//! default constructor
	VirtualFileListing();
	//! destructor
	virtual ~VirtualFileListing();

	//! get num file
	virtual Int32 getNumFile() const;
	//! get the file
	virtual FLItem *getFile() const;

	//! get only the filename, without trailing slash.
	virtual String getFileName() const;
	//! get the absolute path of the file, without trailing slash.
	virtual String getFilePath() const;
	//! get the complete file name, without trailing slash.
	virtual String getFileFullName() const;

	//! get the file type
	virtual FileTypes getFileType() const;
	/*//! get the file size
	virtual UInt64 getFileSize() const;
	//! get the creation file time
    virtual const DateTime& getCreationTime() const;
	//! get the last access file time
    virtual const DateTime& getAccesTime() const;
	//! get the last write file time
    virtual const DateTime& getWriteTime() const;*/

	//! Initialize the file search
	virtual void searchFirstFile();
    //! return the next file (if null its finished)
	virtual FLItem *searchNextFile();

	//! define working path (absolute or relative)
	virtual void setPath(const String &path);

protected:

	T_FLItemVector m_virtualFileList;     //!< list of virtual files
	Int32 m_virtualCurPos;            //!< position in virtual files list

	//! internal method
	//! Initialize the virtual file search
	void virtualFindFirstFile();
};

} // namespace o3d

#endif // _O3D_VIRTUALFILELISTING_H
