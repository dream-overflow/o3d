/**
 * @file md5.h
 * @brief MD5 Checksum calculation and file checking.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-01-31
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MD5_H
#define _O3D_MD5_H

#include "memorydbg.h"
#include "file.h"
#include "smartarray.h"

namespace o3d {

/**
 * @brief MD5 Checksum calculation and file checking
 */
class O3D_API MD5Hash
{
public:

    //! Default constructor, init a MD5 for streaming using update and finalize.
    MD5Hash();
    //! constructor, use an array, compute and finalize.
    MD5Hash(UInt8* data, UInt32 len);
    //! constructor, use an uint8 smart array, compute and finalize.
    MD5Hash(const SmartArrayUInt8 &array);
    //! constructor, use an open stream, compute and finalize.
    MD5Hash(InStream &is);

    ~MD5Hash();

    //! true if update can be called.
    inline Bool isStreaming() const { return m_privateData != nullptr; }

    //! update the MD5 checksum
    void update(UInt8* data, UInt32 len);
    //! update the MD5 checksum
    void update(const SmartArrayUInt8 &array);
    //! update the MD5 checksum
    void update(InStream &is);

    //! finalize the MD5 cheksum before getRaw or getHex
    void finalize();

	//! get the MD5 in 16bytes char
	inline const SmartArrayUInt8& getRaw() const { return m_rawDigest; }

	//! get the MD5 in 32+1bytes hex
    String getHex() const;

protected:

    SmartArrayUInt8 m_rawDigest; //!< the result in 16bytes array

    struct PrivateMD5
    {
        UInt32 state[4];
        UInt32 count[2];         //!< number of *bits*, mod 2^64
        UInt8 buffer[64];         //!< input buffer
        UInt8 digest[16];
        UInt8 finalized;

        PrivateMD5();

        void update(const UInt8 *input, UInt32 input_length);
        void finalize();

        void transform(const UInt8 block[64]);
    };

    PrivateMD5 *m_privateData;
};

}

#endif // _O3D_MD5_H
