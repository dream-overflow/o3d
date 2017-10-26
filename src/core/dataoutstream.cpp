/**
 * @file dataoutstream.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/dataoutstream.h"

using namespace o3d;

OutStream::~OutStream()
{

}

DataOutStream::DataOutStream(ArrayUInt8 &array, Bool own) :
    m_own(own),
    m_data(&array)
{
    if (!m_data)
        O3D_ERROR(E_NullPointer("array must be valid"));
}

DataOutStream::~DataOutStream()
{
    close();
}

Bool DataOutStream::isMemory() const
{
    return True;
}

UInt32 DataOutStream::writer(const void *buf, UInt32 size, UInt32 count)
{
    m_data->pushArray((UInt8*)buf, size * count);
    return size * count;
}

void DataOutStream::close()
{
    if (m_own)
        deletePtr(m_data);
    else
        m_data = nullptr;
}

void DataOutStream::flush()
{

}

Int32 DataOutStream::writeLine(const String &str, CharacterEncoding encoding)
{
    if (str.length() == 0)
    {
        writeChar('\n');
        return 0;
    }
    else
    {
        // print using the specified encoding
        if (encoding == ENCODING_UTF8)
        {
            CString utf8 = str.toUtf8().getData();

            UInt32 res = writer(utf8.getData(), 1, utf8.length());

            if (res == 0)
                O3D_ERROR(E_InvalidResult("Cannot write the line into the data stream"));

            writeChar('\n');

            return (Int32)res;
        }
        else if (encoding == ENCODING_ANSI)
        {
            CString utf8 = str.toAscii().getData();

            UInt32 res = writer(utf8.getData(), 1, utf8.length());

            if (res == 0)
                O3D_ERROR(E_InvalidResult("Cannot write the line into the data stream"));

            writeChar('\n');

            return (Int32)res;
        }
        else
            O3D_ERROR(E_InvalidParameter("Unsupported character encoding"));
    }
}

Int32 DataOutStream::writeString(const String &str, CharacterEncoding encoding)
{
    if (str.length() > 0)
    {
        // print using the specified encoding
        if (encoding == ENCODING_UTF8)
        {
            CString utf8 = str.toUtf8().getData();

            UInt32 res = writer(utf8.getData(), 1, utf8.length());

            if (res == 0)
                O3D_ERROR(E_InvalidResult("Cannot write the string into the file stream"));

            return (Int32)res;
        }
        else if (encoding == ENCODING_ANSI)
        {
            CString utf8 = str.toAscii().getData();

            UInt32 res = writer(utf8.getData(), 1, utf8.length());

            if (res == 0)
                O3D_ERROR(E_InvalidResult("Cannot write the string into the file stream"));

            return (Int32)res;
        }
        else
            O3D_ERROR(E_InvalidParameter("Unsupported character encoding"));
    }
    else
        return 0;
}

