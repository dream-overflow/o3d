/**
 * @file instreamandroid.cpp
 * @brief Android asset file streaming.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-17
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/private/instreamandroid.h"

#ifdef O3D_ANDROID

#include "o3d/core/templatearray.h"
#include "o3d/core/debug.h"

#include "o3d/core/private/assetandroid.h"

#include <android/asset_manager.h>
#include "android/android_native_app_glue.h"

using namespace o3d;

InStreamAndroid::InStreamAndroid() :
    m_asset(nullptr),
    m_pos(0),
    m_length(0)
{

}

InStreamAndroid::InStreamAndroid(const String &filename, AAsset *asset) :
    m_asset(asset),
    m_pos(0),
    m_length(0)
{
    if (!m_asset) {
        O3D_ERROR(E_NullPointer("Asset must be valid for file " + filename));
    }

    m_length = AAsset_getLength64(m_asset);
}

InStreamAndroid::~InStreamAndroid()
{
    this->close();
}

Bool InStreamAndroid::isMemory() const
{
    return False;
}

UInt32 InStreamAndroid::reader(void *buf, UInt32 size, UInt32 count)
{
    if (!m_asset) {
        return 0;
    }

    Int32 len = AAsset_read(m_asset, buf, size*count);
    if (len > 0) {
        m_pos += len;
        return (UInt32)len;
    } else {
        return 0;
    }
}

void InStreamAndroid::close()
{
    if (m_asset) {
        AAsset_close(m_asset);
        m_asset = nullptr;
        m_pos = 0;
    }
}

void InStreamAndroid::reset(UInt64 n)
{
    Int64 pos;

    if ((pos = AAsset_seek64(m_asset, n, SEEK_SET)) < 0) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }

    m_pos = (UInt64)pos;
}

void InStreamAndroid::seek(Int64 n)
{
    Int64 pos;

    if ((pos = AAsset_seek64(m_asset, n, SEEK_CUR)) < 0) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }

    m_pos = (UInt64)pos;
}

void InStreamAndroid::end(Int64 n)
{
    Int64 pos;

    if ((pos = AAsset_seek64(m_asset, n, SEEK_END)) < 0) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }

    m_pos = (UInt64)pos;
}

inline Int32 AAgetc(AAsset *asset, UInt64 &pos)
{
    UInt8 value;
    if (AAsset_read(asset, &value, 1) == 1) {
        ++pos;
        return (Int32)value;
    } else {
        return EOF;
    }
}

UInt8 InStreamAndroid::peek()
{
    if (m_pos < m_length) {
        Int32 value = AAgetc(m_asset, m_pos);
        if (value != EOF) {
            AAsset_seek(m_asset, -1, SEEK_CUR);
            return value;
        }
    }

    return 0;
}

void InStreamAndroid::ignore(Int32 limit, UInt8 delim)
{
    Int32 value;
    Int32 counter = 0;

    while ((counter < limit) && ((value = AAgetc(m_asset, m_pos)) != EOF) && ((UInt8)value != delim)) {
        ++counter;
    }

    m_pos += counter;
}

Int32 InStreamAndroid::getAvailable() const
{
    // return (Int32)AAsset_getRemainingLength64(m_asset);
    return AAsset_getRemainingLength(m_asset);
}

Int32 InStreamAndroid::getPosition() const
{
    return m_pos;
}

Bool InStreamAndroid::isEnd() const
{
   return m_pos == m_length;
}

Int32 InStreamAndroid::readLine(String &str, CharacterEncoding encoding)
{
    ArrayChar read;

    Int32 c;
    while (((c = AAgetc(m_asset, m_pos)) != '\n') && ( c != EOF)) {
        if (c != '\r') {
            read.push((Char)c);
        }
    }

    if ((read.getSize() == 0) && (c == EOF)) {
        // empty string
        str.destroy();
        return EOF;
    }

    read.push(0);

    // set using the specified encoding
    if (encoding == ENCODING_UTF8) {
        str.fromUtf8(read.getData());
    } else if (encoding == ENCODING_ANSI) {
        str.set(read.getData(),0);
    } else {
        O3D_ERROR(E_InvalidParameter("Unsupported character encoding"));
    }

    return str.length();
}

Int32 InStreamAndroid::readLine(String &str, Int32 limit, UInt8 delim, CharacterEncoding encoding)
{
    ArrayChar read;

    Int32 c = EOF, counter = 0;

    while ((counter < limit) && ((c = AAgetc(m_asset, m_pos)) != '\n') && (c != EOF) && (c != delim)) {
        if (c != '\r') {
            read.push((Char)c);
        }
    }

    if ((str.length() == 0) && (c == EOF)) {
        // empty string
        str.destroy();
        return EOF;
    }

    read.push(0);

    // set using the specified encoding
    if (encoding == ENCODING_UTF8) {
        str.fromUtf8(read.getData());
    } else if (encoding == ENCODING_ANSI) {
        str.set(read.getData(),0);
    } else {
        O3D_ERROR(E_InvalidParameter("Unsupported character encoding"));
    }

    return str.length();
}

Int32 InStreamAndroid::readWord(String &str, CharacterEncoding encoding)
{
    ArrayChar read;

    Int32 c;
    while (((c = AAgetc(m_asset, m_pos)) != ' ') && ( c != EOF) && (c != '\n') && (c != '\r')) {
        read.push((Char)c);
    }

    if ((read.getSize() == 0) && (c == EOF)) {
        return EOF;
    }

    read.push(0);

    // set using the specified encoding
    if (encoding == ENCODING_UTF8) {
        str.fromUtf8(read.getData());
    } else if (encoding == ENCODING_ANSI) {
        str.set(read.getData(),0);
    } else {
        O3D_ERROR(E_InvalidParameter("Unsupported character encoding"));
    }

    return str.length();
}

AAsset* InStreamAndroid::getAsset() const
{
    return m_asset;
}

#endif // O3D_ANDROID
