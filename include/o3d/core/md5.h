/**
 * @file md5.h
 * @brief MD5 Checksum calculation and file checking.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
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

//---------------------------------------------------------------------------------------
//! @class MD5Hash
//-------------------------------------------------------------------------------------
//! MD5 Checksum calculation and file checking
//---------------------------------------------------------------------------------------
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
    Bool isStreaming() const { return m_intMD5 != nullptr; }

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
	inline const String& getHex() const { return m_hexDigest; }

protected:

    class MD5 *m_intMD5;

    SmartArrayUInt8 m_rawDigest; //!< the result in 16bytes array
    String m_hexDigest;          //!< the result in 32+1bytes hex string
};

}

#endif // _O3D_MD5_H

