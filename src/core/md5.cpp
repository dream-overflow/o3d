/**
 * @file md5.cpp
 * @brief MD5 Checksum calculation and file checking.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-01-31
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include "../../third/MD5/_MD5.h"
#include "o3d/core/md5.h"
#include "o3d/core/filemanager.h"

using namespace o3d;

MD5Hash::MD5Hash() :
    m_intMD5(new MD5),
    m_rawDigest(16)
{
}

MD5Hash::MD5Hash(UInt8* data, UInt32 len) :
    m_intMD5(new MD5),
	m_rawDigest(16)
{
    if (data && len)
        m_intMD5->update(data, len);

    m_intMD5->finalize();

    UInt8 *rawDigest = m_intMD5->raw_digest();
    memcpy(m_rawDigest.getData(), rawDigest, 16);

    deleteArray(rawDigest);

    const Char* hexDigest = m_intMD5->hex_digest();
    m_hexDigest = hexDigest;

    deleteArray(hexDigest);
    deletePtr(m_intMD5);
}

MD5Hash::MD5Hash(const SmartArrayUInt8 &array) :
    m_intMD5(new MD5),
    m_rawDigest(16)
{
    if (array.isValid())
        m_intMD5->update(array.getData(), array.getSizeInBytes());

    m_intMD5->finalize();

    UInt8 *rawDigest = m_intMD5->raw_digest();
    memcpy(m_rawDigest.getData(), rawDigest, 16);

    deleteArray(rawDigest);

    const Char* hexDigest = m_intMD5->hex_digest();
    m_hexDigest = hexDigest;

    deleteArray(hexDigest);
    deletePtr(m_intMD5);
}

MD5Hash::MD5Hash(InStream &is) :
    m_intMD5(new MD5),
	m_rawDigest(16)
{
    // stream
    UInt8 *buf = new UInt8[4096];
    Int32 size;

    while (is.getAvailable() > 0)
    {
        size = o3d::min(is.getAvailable(), 4096);

        is.read(buf, size);
        m_intMD5->update(buf, size);
    }

    deleteArray(buf);

    m_intMD5->finalize();

    UInt8 *rawDigest = m_intMD5->raw_digest();
    memcpy(m_rawDigest.getData(), rawDigest, 16);

    deleteArray(rawDigest);

    const Char* hexDigest = m_intMD5->hex_digest();
    m_hexDigest = hexDigest;

    deleteArray(hexDigest);
    deletePtr(m_intMD5);
}

MD5Hash::~MD5Hash()
{
    deletePtr(m_intMD5);
}

void MD5Hash::update(UInt8* data, UInt32 len)
{
    if (m_intMD5 && data && len)
        m_intMD5->update(data, len);
}

void MD5Hash::update(const SmartArrayUInt8 &array)
{
    if (m_intMD5 && array.isValid())
        m_intMD5->update(array.getData(), array.getSizeInBytes());
}

void MD5Hash::update(InStream &is)
{
    if (!m_intMD5)
        return;

    // stream
    UInt8 *buf = new UInt8[4096];
    Int32 size;

    while (is.getAvailable() > 0)
    {
        size = o3d::min(is.getAvailable(), 4096);

        is.read(buf, size);
        m_intMD5->update(buf, size);
    }

    deleteArray(buf);
}

void MD5Hash::finalize()
{
    if (m_intMD5)
    {
        m_intMD5->finalize();

        UInt8 *rawDigest = m_intMD5->raw_digest();
        memcpy(m_rawDigest.getData(), rawDigest, 16);

        deleteArray(rawDigest);

        const Char* hexDigest = m_intMD5->hex_digest();
        m_hexDigest = hexDigest;

        deleteArray(hexDigest);
        deletePtr(m_intMD5);
    }
}

