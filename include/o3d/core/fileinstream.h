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

class BaseFile;

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
    FileInStream(const BaseFile &file);

    virtual ~FileInStream();

    //! False
    virtual Bool isMemory() const override;

    virtual UInt32 reader(void* buf, UInt32 size, UInt32 count) override;

    virtual void close() override;

    virtual void reset(UInt64 n) override;

    virtual void seek(Int64 n) override;

    virtual void end(Int64 n) override;

    virtual UInt8 peek() override;

    virtual void ignore(Int32 limit, UInt8 delim) override;

    virtual Int32 getAvailable() const override;

    virtual Int32 getPosition() const override;

    virtual Bool isEnd() const override;

    //! Get the C file descriptor.
    int getFD() const;

    //! Get the stdio C file stream.
    FILE* getFile() const;

    virtual Int32 readLine(
            String &str,
            CharacterEncoding encoding = ENCODING_UTF8) override;

    virtual Int32 readLine(
            String &str,
            Int32 limit,
            UInt8 delim,
            CharacterEncoding encoding = ENCODING_UTF8) override;

    virtual Int32 readWord(
            String &str,
            CharacterEncoding encoding = ENCODING_UTF8) override;

protected:

    FileInStream();

    FILE *m_file;     //!< File stream
    Int32 m_length;   //!< Cached file size
};

} // namespace o3d

#endif // _O3D_FILEINSTREAM_H
