/**
 * @file sha1.h
 * @brief SHA1 Hash calculation.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-11-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_SHA1_H
#define _O3D_SHA1_H

#include "memorydbg.h"
#include "file.h"
#include "smartarray.h"

namespace o3d {

/**
 * @brief SHA1 computation.
 */
class O3D_API SHA1Hash
{
public:

    //! Default constructor, init a SHA1 for streaming using update and finalize.
    SHA1Hash();
    //! constructor, use an array, compute and finalize.
    SHA1Hash(UInt8* data, UInt32 len);
    //! constructor, use an uint8 smart array, compute and finalize.
    SHA1Hash(const SmartArrayUInt8 &array);
    //! constructor, use an open stream, compute and finalize.
    SHA1Hash(InStream &is);

    ~SHA1Hash();

    //! true if update can be called.
    inline Bool isStreaming() const { return m_privateData != nullptr; }

    //! update the SHA1 checksum
    void update(UInt8* data, UInt32 len);
    //! update the SHA1 checksum
    void update(const SmartArrayUInt8 &array);
    //! update the SHA1 checksum
    void update(InStream &is);

    //! finalize the SHA1 cheksum before getRaw or getHex
    void finalize();

    //! get the MD5 in 20bytes char
    inline const SmartArrayUInt8& getRaw() const { return m_rawDigest; }

    //! get the MD5 in 40+1bytes hex
    String getHex() const;

protected:

    SmartArrayUInt8 m_rawDigest;    //!< the result in 20bytes array

    void *m_privateData;
};

}

#endif // _O3D_SHA1_H
