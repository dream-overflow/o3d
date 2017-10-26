/**
 * @file filemanager.h
 * @brief Global file/resource manager.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-09-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FILEMANAGER_H
#define _O3D_FILEMANAGER_H

#include "zip.h"

#include "instream.h"
#include "fileoutstream.h"

#include "memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class FileManager
//-------------------------------------------------------------------------------------
//! File/resource manager.
//! Using file manager to open a file on the system, or any others virtual files
//! mounted into this manager.
//! Zip file can be mounted virtualy for a reading access.
//! For some purpose the openFile and closeFile methods are
//! thread safe. But take care because the others methods are not.
//---------------------------------------------------------------------------------------
class O3D_API FileManager
{
public:

	//! Get the singleton instance.
    static FileManager* instance();

	//! Delete the singleton instance.
    static void destroy();

	//! Type of the target for flow control.
	enum FileSpeedManager
	{
		DISK_FILE_WRITE,     //!< Control write flow.
		DISK_FILE_READ       //!< Control read flow.
	};

	//! Location of the mounted file system.
	enum Location
	{
		LOCATION_SYSTEM = 0, //!< Local file system (rw).
		LOCATION_ZIP = 1     //!< Zip mounted file system (read only).
	};

	static const Int32 NUM_FILE_SPEED_MANAGER = 2;

	//! Return true if Path is relative otherwise return false.
	static Bool isRelativePath(const String &path);

	//! Define the packs files extension (default is "*.pak").
	inline void setPackExt(const String &packExt) { m_packExt = packExt; }

	//! Return the packs files extension.
	inline String getPackExt()const { return m_packExt; }

	//! Define the current working path (absolute).
	Bool setWorkingDirectory();

	//! Define the current working path (absolute).
	Bool setWorkingDirectory(const String &workingDir);

	//! Define the default path (root absolute path).
	Bool setDefaultWorkingDirectory();

	//! Set the current path with a relative one.
	Bool changeWorkingDirectory(const String &relativDir);

	//! Reset to the last working directory.
	Bool restoreWorkingDirectory();

	//! Get the current working directory, without trailing slash.
	String getWorkingDirectory();

	//! Return a full path filename, such as OpenFile but it don't open it.
	String getFullFileName(const String &filename);

    /**
     * @brief openInputStream Open an input stream.
     *        It can be a FileInStream or a SharedDataInStream.
     * @param filename
     * @return nullptr or a new input stream.
     */
    InStream* openInStream(const String &filename);

    /**
      * @brief openOutputStream Open a file output stream.
      * @param filename
      * @param mode Create if necessary, truncate if exists, write only.
      * @return nullptr or a new output stream.
      */
    FileOutStream* openOutStream(
                const String &filename,
                FileOutStream::Mode mode);

	//-----------------------------------------------------------------------------------
	// Virtual file from (ZIP)
	//-----------------------------------------------------------------------------------

	//! Add an archive file. Zip archive are supported.
	//! @return true if it was not already added.
	Bool mountArchive(const String &archiveName);

	//! Add all the archives files found in the working directory that are not
	//! already present.
	//! @return the number of mounted archives.
	Int32 addAllArchives();

	//! Remove a file pack.
	//! @true if it have already deleted.
	Bool removeArchive(const String &archiveName);

	//! Remove all the mounted archives.
	//! @return the number of removed archives.
	Int32 removeAllArchives();

	//! Get a mounted archive file (only Zip).
	Zip* getArchive(const String &archiveName);

	//! Initialize file search for file contained in pack file (virtual files).
	void searchFirstVirtualFile();

	//! Return the next file name (empty string if finished).
	//! @param fileType If non NULL will be contain the file type.
    String searchNextVirtualFile(FileTypes *fileType = nullptr);


	//-----------------------------------------------------------------------------------
	// File transfer flow control
	//-----------------------------------------------------------------------------------

	//! Enable transfer flow control (not thread safe methods).
	inline void enableFileSpeedManager(FileSpeedManager type)
	{
		O3D_ASSERT(type < NUM_FILE_SPEED_MANAGER);
		m_FileSpeedManager[type].m_use = True;
	}

	//! Disable transfer flow control (not thread safe methods).
	inline void disableFileSpeedManager(FileSpeedManager type)
	{
		O3D_ASSERT(type < NUM_FILE_SPEED_MANAGER);
		m_FileSpeedManager[type].m_use = False;
	}

	//! Is the transfer flow control is enable or disable for a type.
	inline Bool isFileSpeedManagerFor(FileSpeedManager type) const
	{
		O3D_ASSERT(type < NUM_FILE_SPEED_MANAGER);
		return m_FileSpeedManager[type].m_use;
	}

	//! Set the transfer flow control in delay and chunk-size.
	void setSpeedManager(FileSpeedManager type, UInt32 delay, UInt32 blocksize);

	//! Get the transfer flow control data for a type.
	inline const BlockData& getSpeedManagerData(FileSpeedManager type) const
	{
		O3D_ASSERT(type < NUM_FILE_SPEED_MANAGER);
		return m_FileSpeedManager[type];
	}

	//! Get the transfer flow control rate in bytes per second.
	inline UInt32 getSpeedManagerBytePerSec(FileSpeedManager type) const
	{
		O3D_ASSERT(type < NUM_FILE_SPEED_MANAGER);
		return m_FileSpeedManager[type].m_bytesec;
	}

	//! Get the transfer flow control delay per chunk.
	inline UInt32 getSpeedManagerDelay(FileSpeedManager type) const
	{
		O3D_ASSERT(type < NUM_FILE_SPEED_MANAGER);
		return m_FileSpeedManager[type].m_delay;
	}

	//! Get the transfer flow control chunk-size per second.
	inline UInt32 getSpeedManagerBlockSize(FileSpeedManager type) const
	{
		O3D_ASSERT(type < NUM_FILE_SPEED_MANAGER);
		return m_FileSpeedManager[type].m_size;
	}

protected:

	String m_packExt;               //!< files packs extension
	String m_workingDir;            //!< working absolute path
	String m_defaultWorkingDir;     //!< root absolute path
	String m_oldWorkingDir;         //!< old working absolute path
	T_ZipList m_packList;	        //!< file list of packs

	Int32 m_curFilePos;         //!< current position of used file by findNextFile
	IT_ZipList m_curPackPos;        //!< current position of used pack by findNextFile

	//! return the index of file pack end if not found
	IT_ZipList findPackFile(const String &packName);

	// each Opened file take the state of theses flags, and use it until it is opened
	// modify theses flags in running mean that only new opened file take the new states

	//! is the disk/mem file reading/writing speed is managed
	BlockData m_FileSpeedManager[NUM_FILE_SPEED_MANAGER];


	//-----------------------------------------------------------------------------------
	// Singleton
	//-----------------------------------------------------------------------------------

    static FileManager* m_instance;

	//! Restricted default contructor
    FileManager();

	//! Restricted destructor
    virtual ~FileManager();

	//! Non copyable
    FileManager(const FileManager& win);

	//! Non assignable
    void operator=(const FileManager& win);
};

} // namespace o3d

#endif // _O3D_FILEMANAGER_H

