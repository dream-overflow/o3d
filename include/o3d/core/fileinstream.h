/**
 * @file fileinstream.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FILEINSTREAM_H
#define _O3D_FILEINSTREAM_H

#include "instream.h"

namespace o3d {

class FileInfo;

/**
 * @brief FileInStream Reading file stream.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-12-04
 */
class O3D_API FileInStream : public InStream
{
public:

    /**
     * @brief FileInStream Open a file stream in reading from the file system.
     * @param filename File name from file system.
     * @throw E_FileNotFoundOrInvalidRights
     */
    FileInStream(const String &filename);

    /**
     * @brief FileInStream Open a file stream in reading from the file system.
     * @param file File info.
     * @throw E_FileNotFoundOrInvalidRights
     */
    FileInStream(const FileInfo &file);

    virtual ~FileInStream();

    //! False
    virtual Bool isMemory() const;

    virtual UInt32 reader(void* buf, UInt32 size, UInt32 count);

    virtual void close();

    virtual void reset(UInt64 n);

    virtual void seek(Int64 n);

    virtual void end(Int64 n);

    virtual UInt8 peek();

    virtual void ignore(Int32 limit, UInt8 delim);

    virtual Int32 getAvailable() const;

    virtual Int32 getPosition() const;

    virtual Bool isEnd() const;

    //! Get the C file descriptor.
    int getFD() const;

    //! Get the stdio C file stream.
    FILE* getFile() const;

    virtual Int32 readLine(
            String &str,
            CharacterEncoding encoding = ENCODING_UTF8);

    virtual Int32 readLine(
            String &str,
            Int32 limit,
            UInt8 delim,
            CharacterEncoding encoding = ENCODING_UTF8);

    virtual Int32 readWord(
            String &str,
            CharacterEncoding encoding = ENCODING_UTF8);

protected:

    FileInStream();

    FILE *m_file;     //!< File stream
    Int32 m_length;   //!< Cached file size
};

} // namespace o3d

#endif // _O3D_FILEINSTREAM_H
