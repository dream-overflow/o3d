/**
 * @file filemanager.h
 * @brief Global file/resource manager.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-09-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FILEMANAGER_H
#define _O3D_FILEMANAGER_H

#include "asset.h"
#include "instream.h"
#include "fileoutstream.h"

#include "memorydbg.h"

namespace o3d {

class BaseFile;
class BaseDir;

/**
 * @brief BlockData Useful struct for data reading/writing managing
 * @deprecated For now...
 */
struct O3D_API BlockData
{
    UInt32 m_delay;       //!< delay between to block of m_size (in ms)
    UInt32 m_size;        //!< size of a chunk of data to read/write
    UInt32 m_bytesec;     //!< value in byte per second
    Bool m_use;           //!< is enable or disable

    BlockData() :
        m_delay(50),
        m_size(25600),
        m_bytesec(512000),
        m_use(False) {}

    BlockData(const BlockData &dup) :
        m_delay(dup.m_delay),
        m_size(dup.m_size),
        m_bytesec(dup.m_bytesec),
        m_use(dup.m_use) {}

    const BlockData& operator= (const BlockData &dup)
    {
        m_delay = dup.m_delay;
        m_size = dup.m_size;
        m_bytesec = dup.m_bytesec;
        m_use = dup.m_use;

        return (*this);
    }
};

/**
 * @brief File and asset manager.
 * Using file manager to open a file on the system, or any others virtual files
 * mounted into this manager (zip, asset manager...).
 * Zip files can be mounted virtually for a reading access only.
 * This manager is thread safe.
 * By default on Android it support for the asset manager.
 */
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

	static const Int32 NUM_FILE_SPEED_MANAGER = 2;

    //-----------------------------------------------------------------------------------
    // Helpers
    //-----------------------------------------------------------------------------------

	//! Return true if Path is relative otherwise return false.
    static Bool isRelativePath(const String &path);

    /**
     * @brief When a path contain some '..' this method reform the path without '..' and './'
     * @param path To cleanup
     */
    static void adaptPath(String &path);

    /**
     * @brief Adapt an absolute path of a file to another absolute path. ex:
     * - 'c:/tool/one/two/file.txt' as source
     * - 'c:/tool/one/one/one' as path
     * result in '../../two/file.txt'
     * @param filename
     * @param pathname
     * @return
     */
    static String convertPath(const String &filename, const String &pathname);

    /**
     * @brief Get the file path only from a full path filename.
     * @param fullFileName Input fullFileName to parse.
     */
    static String getFilePath(const String &fullFileName);

    /**
     * @brief Get the file name and the path name from a complete (full) file name.
     * @param fullFileName Input fullFileName to parse.
     * @param filename The filename only (output).
     * @param pathname The path only (output).
     */
    static void getFileNameAndPath(
        const String &fullFileName,
        String &filename,
        String &pathname);

    /**
     * @brief Get the file name only from a full path.
     * @param fullFileName Input fullFileName to parse.
     */
    static String getFileName(const String &fullFileName);

    /**
     * @brief Get the file name only without path neither extension, from a full path.
     * @param fullFileName Input fullFileName to parse.
     */
    static String getStrippedFileName(const String &fullFileName);

    /**
     * @brief Get the file extension only from a full path.
     * @param fullFileName Input fullFileName to parse.
     */
    static String getFileExt(const String &fullFileName);

    /**
     * @brief Get the file name and its extension from a full path.
     * @param fullFileName Input fullFileName to parse.
     * @param filename The filename only (output).
     * @param ext The extension only (output).
     */
    static void getFileNameAndExt(
            const String &fullFileName,
            String &filename,
            String &ext);

    //-----------------------------------------------------------------------------------
    // Assets support
    //-----------------------------------------------------------------------------------

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

    //! Return a full path filename if it is in a relative path, from current working directory.
    String getFullFileName(const String &filename) const;

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
    FileOutStream* openOutStream(const String &filename, FileOutStream::Mode mode);

	//-----------------------------------------------------------------------------------
    // Assets support
	//-----------------------------------------------------------------------------------

    //! Define the packs files extension (default is "*.opk").
    inline void setPackExt(const String &packExt) { m_packExt = packExt; }

    //! Return the packs files extension.
    inline String getPackExt()const { return m_packExt; }

    //! Add an asset handler. Can be a Zip or any other supported protocol.
	//! @return true if it was not already added.
    Bool mountAsset(const String &protocol, const String &assetName);

	//! Add all the archives files found in the working directory that are not
	//! already present.
	//! @return the number of mounted archives.
    Int32 mountAllArchives();

    //! Remove an asset.
    Bool umountAsset(const String &assetName);

	//! Remove all the mounted archives.
	//! @return the number of removed archives.
    Int32 umountAllAssets();

    //! Get a mounted asset.
    Asset* getAsset(const String &assetName);

    //! Initialize file search for file contained in asset.
    void searchFirstVirtualFile(const String &path);

	//! Return the next file name (empty string if finished).
    //! @param fileType If non null will be contain the file type.
    String searchNextVirtualFile(const String &path, FileTypes *fileType = nullptr);

    //-----------------------------------------------------------------------------------
    // File and directory support
    //-----------------------------------------------------------------------------------

    //! Find if a directory exists on file system or on assets.
    Bool isPath(const String &path) const;

    //! Find if a file exists on file system or on assets.
    Bool isFile(const String &fileName) const;

    //! Get the length of a file on file system or on assets.
    UInt64 fileSize(const String &fileName) const;

    //! Get the type of a file on file system or on assets.
    FileTypes fileType(const String &fileName) const;

    //! Get a file info object. It refers a valid file on system or asset.
    BaseFile* file(const String &fileName) const;

    //! Get a dir info object. It refers a valid path on system or asset.
    BaseDir* dir(const String &path) const;

	//-----------------------------------------------------------------------------------
    // File transfer flow control (@deprecated no longer used)
	//-----------------------------------------------------------------------------------

	//! Enable transfer flow control (not thread safe methods).
	inline void enableFileSpeedManager(FileSpeedManager type)
	{
		O3D_ASSERT(type < NUM_FILE_SPEED_MANAGER);
        m_flowCtrl[type].m_use = True;
	}

	//! Disable transfer flow control (not thread safe methods).
	inline void disableFileSpeedManager(FileSpeedManager type)
	{
		O3D_ASSERT(type < NUM_FILE_SPEED_MANAGER);
        m_flowCtrl[type].m_use = False;
	}

	//! Is the transfer flow control is enable or disable for a type.
	inline Bool isFileSpeedManagerFor(FileSpeedManager type) const
	{
		O3D_ASSERT(type < NUM_FILE_SPEED_MANAGER);
        return m_flowCtrl[type].m_use;
	}

	//! Set the transfer flow control in delay and chunk-size.
	void setSpeedManager(FileSpeedManager type, UInt32 delay, UInt32 blocksize);

	//! Get the transfer flow control data for a type.
	inline const BlockData& getSpeedManagerData(FileSpeedManager type) const
	{
		O3D_ASSERT(type < NUM_FILE_SPEED_MANAGER);
        return m_flowCtrl[type];
	}

	//! Get the transfer flow control rate in bytes per second.
	inline UInt32 getSpeedManagerBytePerSec(FileSpeedManager type) const
	{
		O3D_ASSERT(type < NUM_FILE_SPEED_MANAGER);
        return m_flowCtrl[type].m_bytesec;
	}

	//! Get the transfer flow control delay per chunk.
	inline UInt32 getSpeedManagerDelay(FileSpeedManager type) const
	{
		O3D_ASSERT(type < NUM_FILE_SPEED_MANAGER);
        return m_flowCtrl[type].m_delay;
	}

	//! Get the transfer flow control chunk-size per second.
	inline UInt32 getSpeedManagerBlockSize(FileSpeedManager type) const
	{
		O3D_ASSERT(type < NUM_FILE_SPEED_MANAGER);
        return m_flowCtrl[type].m_size;
	}

protected:

    String m_packExt;             //!< files packs extension
    String m_workingDir;          //!< working absolute path
    String m_defaultWorkingDir;   //!< root absolute path
    String m_oldWorkingDir;       //!< old working absolute path

    T_AssetList m_assets;	      //!< List of mounted assets

    Int32 m_curFilePos;           //!< current position of used file by findNextFile
    IT_AssetList m_curAssetIt;    //!< current position of used pack by findNextFile

    //! return a iterator on an asset name
    IT_AssetList findAsset(const String &assetName);

    //! Flow control for file reading and writing (independantly).
    BlockData m_flowCtrl[NUM_FILE_SPEED_MANAGER];

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
