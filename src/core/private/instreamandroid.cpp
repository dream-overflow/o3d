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

#include "o3d/core/basefileinfo.h"
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
}

InStreamAndroid::~InStreamAndroid()
{
    close();
}

Bool InStreamAndroid::isMemory() const
{
    return False;
}

UInt32 InStreamAndroid::reader(void *buf, UInt32 size, UInt32 count)
{
    //return (UInt32)fread(buf, size, count, m_file) * size;
}

void InStreamAndroid::close()
{
    if (m_asset) {
        AAsset_close(m_asset);
        m_asset = nullptr;
    }
}

void InStreamAndroid::reset(UInt64 n)
{
  /*  if (n == 0) {
        rewind(m_file);
    } else if (fseek(m_file, n, SEEK_SET) != 0) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }*/
}

void InStreamAndroid::seek(Int64 n)
{
   /* if (fseek(m_file, n, SEEK_CUR) != 0) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }*/
}

void InStreamAndroid::end(Int64 n)
{
  /*  if (fseek(m_file, n, SEEK_END) != 0) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }*/
}

UInt8 InStreamAndroid::peek()
{
    /*UInt8 value = (UInt8)getc(m_file);
    ungetc(value, m_file);

    return value;*/
}

void InStreamAndroid::ignore(Int32 limit, UInt8 delim)
{
   /* Int32 value;
    Int32 counter = 0;

    while ((counter < limit) && ((value = getc(m_file)) != EOF) && ((UInt8)value != delim)) {
        ++counter;
    }*/
}

Int32 InStreamAndroid::getAvailable() const
{
   /* if (m_length == 0) {
        *const_cast<Int32*>(&m_length) = getFileLength(getFD());
    }

    if (m_length != 0) {
        return m_length - ftell(m_file);
    } else {
        return 0;
    }*/
}

Int32 InStreamAndroid::getPosition() const
{
    /*return ftell(m_file);*/
}

Bool InStreamAndroid::isEnd() const
{
   /* return feof(m_file) != 0;*/
}

Int32 InStreamAndroid::readLine(String &str, CharacterEncoding encoding)
{
    ArrayChar read;
/*
    Int32 c;
    while( ((c = getc(m_file)) != '\n') && ( c != EOF)) {
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

    return str.length();*/
}

Int32 InStreamAndroid::readLine(String &str, Int32 limit, UInt8 delim, CharacterEncoding encoding)
{
    ArrayChar read;
/*
    Int32 c = EOF, counter = 0;

    while((counter < limit) && ((c = getc(m_file)) != '\n') && (c != EOF) && (c != delim)) {
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
    }*/

    return str.length();
}

Int32 InStreamAndroid::readWord(String &str, CharacterEncoding encoding)
{
    ArrayChar read;
/*
    Int32 c;
    while( ((c = getc(m_file)) != ' ') && ( c != EOF) && (c != '\n') && (c != '\r')) {
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
    }*/

    return str.length();
}

AAsset* InStreamAndroid::getAsset() const
{
    return m_asset;
}

#endif // O3D_ANDROID
