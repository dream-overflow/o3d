/**
 * @file fileoutstream.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/fileoutstream.h"
#include "o3d/core/debug.h"

using namespace o3d;

FileOutStream::FileOutStream() :
    m_file(nullptr),
    m_length(0)
{

}

FileOutStream::FileOutStream(const String &filename, Mode mode) :
    m_file(nullptr),
    m_length(0)
{
    String name(filename);
    name.replace('\\', '/');

    // file open
#ifdef _MSC_VER
    if (mode == CREATE)
        _wfopen_s(&m_file, name.getData(), L"wb");
    else if (mode == CREATE_IF_NOT_EXISTS)
        _wfopen_s(&m_file, name.getData(), L"wbx");
    else if (mode == APPEND)
        _wfopen_s(&m_file, name.getData(), L"ab");
    else
        O3D_ERROR(E_InvalidParameter("Mode must by specifed"));
#else
    if (mode == CREATE) {
        m_file = fopen(name.toUtf8().getData(), "wb");
    } else if (mode == CREATE_IF_NOT_EXISTS) {
        m_file = fopen(name.toUtf8().getData(), "wbx");
    } else if (mode == APPEND) {
        m_file = fopen(name.toUtf8().getData(), "ab");
    } else {
        O3D_ERROR(E_InvalidParameter("Mode must by specifed"));
    }
#endif

    if (!m_file) {
        O3D_ERROR(E_FileNotFoundOrInvalidRights("", name));
    }
}

FileOutStream::~FileOutStream()
{
    close();
}

Bool FileOutStream::isMemory() const
{
    return False;
}

UInt32 FileOutStream::writer(const void *buf, UInt32 size, UInt32 count)
{
    return (UInt32)fwrite(buf, size, count, m_file) * size;
}

void FileOutStream::close()
{
    if (m_file) {
        fclose(m_file);
        m_file = nullptr;
    }
}

void FileOutStream::flush()
{
    if (m_file) {
        fflush(m_file);
    }
}

int FileOutStream::getFD() const
{
    if (m_file) {
#ifdef _MSC_VER
        return _fileno(m_file);
#else
        return fileno(m_file);
#endif
    } else {
        return -1;
    }
}

FILE *FileOutStream::getFile() const
{
    return m_file;
}


Int32 FileOutStream::writeLine(const String &str, CharacterEncoding encoding)
{
    if (str.length() == 0) {
        if (fputc('\n', m_file) == EOF) {
            O3D_ERROR(E_InvalidResult("Cannot write the line into the file stream"));
        }

        return 0;
    } else {
        // print using the specified encoding
        if (encoding == ENCODING_UTF8) {
            CString utf8 = str.toUtf8().getData();

            UInt32 res = fwrite(utf8.getData(), 1, utf8.length(), m_file);
            if (res == 0 || fputc('\n', m_file) == EOF) {
                O3D_ERROR(E_InvalidResult("Cannot write the line into the file stream"));
            }

            return (Int32)res;
        } else if (encoding == ENCODING_ANSI) {
            CString utf8 = str.toAscii().getData();

            Int32 res = fwrite(utf8.getData(), 1, utf8.length(), m_file);
            if (res == 0 || fputc('\n', m_file) == EOF) {
                O3D_ERROR(E_InvalidResult("Cannot write the line into the file stream"));
            }

            return (Int32)res;
        } else {
            O3D_ERROR(E_InvalidParameter("Unsupported character encoding"));
        }
    }
}

Int32 FileOutStream::writeString(const String &str, CharacterEncoding encoding)
{
    if (str.length() > 0) {
        // print using the specified encoding
        if (encoding == ENCODING_UTF8) {
            CString utf8 = str.toUtf8().getData();

            UInt32 res = fwrite(utf8.getData(), 1, utf8.length(), m_file);
            if (res == 0) {
                O3D_ERROR(E_InvalidResult("Cannot write the string into the file stream"));
            }

            return (Int32)res;
        } else if (encoding == ENCODING_ANSI) {
            CString utf8 = str.toAscii().getData();

            UInt32 res = fwrite(utf8.getData(), 1, utf8.length(), m_file);
            if (res == 0) {
                O3D_ERROR(E_InvalidResult("Cannot write the string into the file stream"));
            }

            return (Int32)res;
        } else {
            O3D_ERROR(E_InvalidParameter("Unsupported character encoding"));
        }
    } else {
        return 0;
    }
}

/*
    // managed writing
    if (m_blockDataW.m_use)
    {
        UInt32 totalsize = size * count;   // total size to write in bytes

        // only for small data
        if (totalsize < 128)
        {
            return (UInt32)fwrite(buf, size, count, m_file) * size;
        }

        UInt8 *data = (UInt8*)buf;  // current data ptr
        UInt32 curpos = 0;               // nbr bytes currently wrote
        UInt32 load,loaded;

        while (curpos != totalsize)
        {
            load = m_blockDataW.m_size;
            if ((totalsize - curpos) < m_blockDataW.m_size)
                load = totalsize - curpos;

            // load data
            loaded = (UInt32)fwrite(data, 1, load, m_file);
            if (loaded <= load)
                curpos += load;

            data += loaded;

            // delay
            if (totalsize < m_blockDataW.m_size) // one time load with totalsize < blocksize
                System::waitMs((UInt32)((Float)m_blockDataW.m_delay*(Float)totalsize/(Float)m_blockDataW.m_size));
            else if (load < m_blockDataW.m_size) // last time load with load < blocksize
                System::waitMs((UInt32)((Float)m_blockDataW.m_delay*(Float)load/(Float)m_blockDataW.m_size));
            else								// each time delay
                System::waitMs(m_blockDataW.m_delay);
        }

        return curpos;
    }
*/
