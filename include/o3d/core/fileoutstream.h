/**
 * @file fileoutstream.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FILEOUTSTREAM_H
#define _O3D_FILEOUTSTREAM_H

#include "outstream.h"
#include "file.h"

namespace o3d {

/**
 * @brief FileOutStream
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-12-05
 */
class O3D_API FileOutStream : public OutStream
{
public:

    enum Mode
    {
        CREATE = 0,           //!< Create the file if not exists, else truncate the content.
        CREATE_IF_NOT_EXISTS, //!< Create the file if not exists, else fail.
        APPEND                //!< Append to the end of the file if exists, create anyway.
    };

    FileOutStream(
            const String &filename,
            Mode mode = CREATE);

    virtual ~FileOutStream();

    //! False
    virtual Bool isMemory() const;

    virtual UInt32 writer(const void* buf, UInt32 size, UInt32 count);

    virtual void close();

    virtual void flush();

    //! Get the C file descriptor.
    int getFD() const;

    //! Get the stdio C file stream.
    FILE* getFile() const;

    virtual Int32 writeLine(
            const String &str,
            CharacterEncoding encoding = ENCODING_UTF8);

    virtual Int32 writeString(
            const String &str,
            CharacterEncoding encoding = ENCODING_UTF8);

protected:

    FileOutStream();

    FILE *m_file;
    Int32 m_length;
};

} // namespace o3d

#endif // _O3D_FILEOUTSTREAM_H

