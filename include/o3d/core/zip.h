/**
 * @file zip.h
 * @brief OpenFile.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-09-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ZIP_H
#define _O3D_ZIP_H

#include "memorydbg.h"

#include "string.h"
#include "hashmap.h"
#include "asset.h"

#include <vector>
#include <list>

namespace o3d {

#define O3D_ZIP_SIG				0x04034b50		//!< ZIP Signature
#define O3D_ZIP_DATA_DESCRIPTOR	0x00000008		//!< Bit 3 on => structure MTZipDataDescriptor

// byte-align structures
#ifdef _MSC_VER
#	pragma pack(push,packing)
#	pragma pack(1)
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error you must byte-align these structures with the appropriate compiler directives
#endif

struct ZipDataDescriptor
{
	UInt32 Crc32;
	UInt32 CompressedSize;
	UInt32 UnCompressedSize;
} PACK_STRUCT;

struct ZipHeader
{
	UInt32 Sig;                //!< Signature
	Int16 VersionToExtract;
	Int16 BitFlag;
	Int16 CompressionMethod;
	Int16 LastModFileTime;
	Int16 LastModeFileData;
	ZipDataDescriptor DataDescriptor;
	Int16 FileNameLength;
	Int16 ExtraFieldLength;
} PACK_STRUCT;

// Default alignment
#ifdef _MSC_VER
#	pragma pack(pop, packing)
#endif

#undef PACK_STRUCT

// file name (variable size)
// extra field (variable size)

//! ZIP file token information
struct ZipToken
{
	ZipHeader FileHeader;
    UInt64 FilePos;		    //!< File token position since data chunk start
	String FileName;		//!< File token filename relative to ZIP archive
	FileTypes FileType;     //!< File type (file or directory)
};

typedef std::vector<ZipToken*> T_ZipTokenVector;
typedef T_ZipTokenVector::iterator IT_ZipTokenVector;
typedef T_ZipTokenVector::const_iterator CIT_ZipTokenVector;

/**
 * @brief ZIP file management for decompression. You must attach a File opened
 * in reading and binary mode to Zip. Then you can get file from this archive with
 * OpenFile.
 */
class O3D_API Zip : public Asset
{
public:

    /**
     * @brief Zip
     * @param is An opened input stream on the zip file.
     * @param zipName Zip filename
     * @param zipPath Zip path.
     */
    Zip(InStream &is, const String &zipName, const String &zipPath);

    //! Destructor.
	virtual ~Zip();

    /**
     * @brief release all datas and close the file
     */
    virtual void destroy() override;

    /**
     * @brief return a file index in the list (-1 if not found).
     * @param fileName the path of the filename is relative to the Zip file.
     * @return index of the file, or -1 if not found.
     */
    virtual Int32 findFile(const String& fileName) override;

    virtual Bool isPath(const String& path) const override;

    virtual InStream* openInStream(const String& filename) override;
    virtual InStream* openInStream(Int32 index) override;

    /**
     * @return Returns the location relative to the file system.
     */
    virtual String location() const override;

    /**
     * @return Returns the name of the zip file.
     */
    virtual String name() const override;

    virtual String fullPathName() const override;

    /**
     * @brief get the number of files in this archive.
     */
    virtual Int32 getNumFiles() const override;

    /**
     * @return Returns the protocol (zip://)
     */
    virtual String protocol() const override;

    /**
     * @brief get the file name at a specified index, prefixed by the Zip path.
     */
    virtual String getFileName(Int32 index) const override;

    virtual FileTypes getFileType(Int32 index) const override;

    virtual UInt64 getFileSize(Int32 index) const override;

    virtual void searchFirstFile(const String &path) override;

protected:

    InStream *m_is;          //!< the zip stream object

	String m_zipPathName;	 //!< absolute path of the zip file
	String m_zipFileName;	 //!< zip file name

	T_ZipTokenVector m_filelist; //!< file list

	typedef stdext::hash_map<String, Int32, std::hash<String> > T_ZipFileMap;
	typedef T_ZipFileMap::iterator IT_ZipFileMap;
	typedef T_ZipFileMap::const_iterator CIT_ZipFileMap;

	T_ZipFileMap m_fileMap;  //!< File map (relative path, index)

	// internal methods
	// read the header and return true it's not the last
	Bool readHeader();
};

typedef ::std::list<Zip*> T_ZipList;
typedef T_ZipList::iterator IT_ZipList;
typedef T_ZipList::const_iterator CIT_ZipList;

} // namespace o3d

#endif // _O3D_ZIP_H
