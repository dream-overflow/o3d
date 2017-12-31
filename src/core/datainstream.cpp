/**
 * @file datainstream.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/datainstream.h"

using namespace o3d;

InStream::~InStream()
{

}

DataInStream::DataInStream(const ArrayUInt8 &array, Bool own) :
    m_own(own),
    m_data(&array),
    m_pos(nullptr)
{
    if (!m_data || m_data->isNull()) {
        O3D_ERROR(E_NullPointer("array must be valid"));
    }

    m_pos = m_data->getData();
}

DataInStream::~DataInStream()
{
    close();
}

Bool DataInStream::isMemory() const
{
    return True;
}

UInt32 DataInStream::reader(void *buf, UInt32 size, UInt32 count)
{
    UInt32 len = size * count;

    if (m_pos + len > m_data->getData() + m_data->getSize()) {
        len = (UInt32)((m_data->getData() + m_data->getSize()) - m_pos);
    }

    memcpy(buf, m_pos, len);

    m_pos += len;

    return len;
}

void DataInStream::close()
{
    if (m_own) {
        deletePtr(m_data);
    } else {
        m_data = nullptr;
    }

    m_pos = nullptr;
}

void DataInStream::reset(UInt64 n)
{
    if (n > (UInt64)m_data->getSize()) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }

    m_pos = m_data->getData() + n;
}

void DataInStream::seek(Int64 n)
{
    if (m_pos + n > m_data->getData() + m_data->getSize()) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }

    if (m_pos + n < m_data->getData()) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }

    m_pos += n;
}

void DataInStream::end(Int64 n)
{
    if (n > 0) {
        O3D_ERROR(E_IndexOutOfRange("value must be negative"));
    }

    if (m_pos + n < m_data->getData()) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }

    m_pos = m_data->getData() + m_data->getSize() - n;
}

Int32 DataInStream::getAvailable() const
{
    return (Int32)(m_data->getData() + m_data->getSize() - m_pos);
}

Int32 DataInStream::getPosition() const
{
    return (Int32)(m_pos - m_data->getData());
}

Bool DataInStream::isEnd() const
{
    return m_pos > m_data->getData() + m_data->getSize() - 1;
}

UInt8 DataInStream::peek()
{
    if (m_pos > m_data->getData() + m_data->getSize() - 1) {
        return (UInt8)EOF;
    } else {
        return *m_pos;
    }
}

void DataInStream::ignore(Int32 limit, UInt8 delim)
{
    const UInt8* plimit = o3d::min<const UInt8*>(m_data->getData() + m_data->getSize(), m_data->getData() + limit);

    while ((m_pos < plimit) && (*m_pos != delim)) {
        ++m_pos;
    }
}

Int32 DataInStream::readLine(String &str, CharacterEncoding encoding)
{
    const UInt8* plimit = m_data->getData() + m_data->getSize();

    ArrayChar read;

    Int32 c;
    while((m_pos < plimit) && ((c = *m_pos) != '\n')) {
        if (c != '\r') {
            read.push((Char)c);
        }

        ++m_pos;
    }

    if (m_pos < plimit && *m_pos == '\n') {
        ++m_pos;
    }

    if ((read.getSize() == 0) && (m_pos == plimit)) {
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

Int32 DataInStream::readLine(String &str, Int32 limit, UInt8 delim, CharacterEncoding encoding)
{
    const UInt8* plimit = o3d::min<const UInt8*>(
                m_data->getData() + m_data->getSize(),
                m_data->getData() + limit);

    ArrayChar read;

    Int32 c;
    while((m_pos < plimit) && ((c = *m_pos) != '\n') && (*m_pos != delim)) {
        if (c != '\r')
            read.push((Char)c);

        ++m_pos;
    }

    if (m_pos < plimit && *m_pos == '\n') {
        ++m_pos;
    }

    if ((str.length() == 0) && (m_pos == plimit)) {
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

Int32 DataInStream::readWord(String &str, CharacterEncoding encoding)
{
    const UInt8* limit = m_data->getData() + m_data->getSize();

    ArrayChar read;

    Int32 c;
    while((m_pos < limit-1) && ((c = *m_pos) != ' ') && (c != '\n') && (c != '\r')) {
        read.push((Char)c);
        ++m_pos;
    }

    if (*m_pos == '\r') {
        ++m_pos;
    }

    if (*m_pos == 10) {
        ++m_pos;
    }

    if ((read.getSize() == 0) && (m_pos == limit)) {
        return EOF;
    }

    read.push(0);
    str.fromUtf8(read.getData());

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

SharedDataInStream::SharedDataInStream(const SmartArrayUInt8 &sharedArray) :
    m_data(sharedArray),
    m_pos(nullptr)
{
    if (m_data.isNull()) {
        O3D_ERROR(E_NullPointer("shared array must be valid"));
    }

    m_pos = m_data.getData();
}

SharedDataInStream::~SharedDataInStream()
{
    close();
}

Bool SharedDataInStream::isMemory() const
{
    return True;
}

UInt32 SharedDataInStream::reader(void *buf, UInt32 size, UInt32 count)
{
    UInt32 len = size * count;

    if (m_pos + len > m_data.getData() + m_data.getSize()) {
        len = (UInt32)((m_data.getData() + m_data.getSize()) - m_pos);
    }

    memcpy(buf, m_pos, len);

    m_pos += len;

    return len;
}

void SharedDataInStream::close()
{
    m_data.releaseCheckAndDelete();
    m_pos = nullptr;
}

void SharedDataInStream::reset(UInt64 n)
{
    if (n > (UInt64)m_data.getSize()) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }

    m_pos = m_data.getData() + n;
}

void SharedDataInStream::seek(Int64 n)
{
    if (m_pos + n > m_data.getData() + m_data.getSize()) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }

    if (m_pos + n < m_data.getData()) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }

    m_pos += n;
}

void SharedDataInStream::end(Int64 n)
{
    if (n > 0) {
        O3D_ERROR(E_IndexOutOfRange("value must be negative"));
    }

    if (m_pos + n < m_data.getData()) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }

    m_pos = m_data.getData() + m_data.getSize() - n;
}

Int32 SharedDataInStream::getAvailable() const
{
    return (Int32)(m_data.getData() + m_data.getSize() - m_pos);
}

Int32 SharedDataInStream::getPosition() const
{
    return (Int32)(m_pos - m_data.getData());
}

Bool SharedDataInStream::isEnd() const
{
    return m_pos > m_data.getData() + m_data.getSize() - 1;
}

UInt8 SharedDataInStream::peek()
{
    if (m_pos > m_data.getData() + m_data.getSize() - 1)
        return (UInt8)EOF;
    else
        return *m_pos;
}

void SharedDataInStream::ignore(Int32 limit, UInt8 delim)
{
    const UInt8* plimit = o3d::min<const UInt8*>(m_data.getData() + m_data.getSize(), m_data.getData() + limit);

    while ((m_pos < plimit) && (*m_pos != delim)) {
        ++m_pos;
    }
}

Int32 SharedDataInStream::readLine(String &str, CharacterEncoding encoding)
{
    UInt8* plimit = m_data.getData() + m_data.getSize();

    ArrayChar read;

    Int32 c;
    while((m_pos < plimit) && ((c = *m_pos) != '\n'))
    {
        if (c != '\r')
            read.push((Char)c);

        ++m_pos;
    }

    if (m_pos < plimit && *m_pos == '\n')
        ++m_pos;

    if ((read.getSize() == 0) && (m_pos == plimit))
    {
        // empty string
        str.destroy();
        return EOF;
    }

    read.push(0);

    // set using the specified encoding
    if (encoding == ENCODING_UTF8)
        str.fromUtf8(read.getData());
    else if (encoding == ENCODING_ANSI)
        str.set(read.getData(),0);
    else
        O3D_ERROR(E_InvalidParameter("Unsupported character encoding"));

    return str.length();
}

Int32 SharedDataInStream::readLine(String &str, Int32 limit, UInt8 delim, CharacterEncoding encoding)
{
    UInt8* plimit = o3d::min<UInt8*>(
                m_data.getData() + m_data.getSize(),
                m_data.getData() + limit);

    ArrayChar read;

    Int32 c;
    while((m_pos < plimit) && ((c = *m_pos) != '\n') && (*m_pos != delim))
    {
        if (c != '\r')
            read.push((Char)c);

        ++m_pos;
    }

    if (m_pos < plimit && *m_pos == '\n')
        ++m_pos;

    if ((str.length() == 0) && (m_pos == plimit))
    {
        // empty string
        str.destroy();
        return EOF;
    }

    read.push(0);

    // set using the specified encoding
    if (encoding == ENCODING_UTF8)
        str.fromUtf8(read.getData());
    else if (encoding == ENCODING_ANSI)
        str.set(read.getData(),0);
    else
        O3D_ERROR(E_InvalidParameter("Unsupported character encoding"));

    return str.length();
}

Int32 SharedDataInStream::readWord(String &str, CharacterEncoding encoding)
{
    UInt8* limit = m_data.getData() + m_data.getSize();

    ArrayChar read;

    Int32 c;
    while((m_pos < limit-1) && ((c = *m_pos) != ' ') && (c != '\n') && (c != '\r'))
    {
        read.push((Char)c);
        ++m_pos;
    }

    if (*m_pos == '\r')
        ++m_pos;

    if (*m_pos == 10)
        ++m_pos;

    if ((read.getSize() == 0) && (m_pos == limit))
        return EOF;

    read.push(0);
    str.fromUtf8(read.getData());

    // set using the specified encoding
    if (encoding == ENCODING_UTF8)
        str.fromUtf8(read.getData());
    else if (encoding == ENCODING_ANSI)
        str.set(read.getData(),0);
    else
        O3D_ERROR(E_InvalidParameter("Unsupported character encoding"));

    return str.length();
}
