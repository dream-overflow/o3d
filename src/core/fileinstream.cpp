/**
 * @file fileinstream.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/fileinstream.h"
#include "o3d/core/basefileinfo.h"
#include "o3d/core/templatearray.h"

#ifdef _MSC_VER
    #include <io.h>
#else
    #include <sys/types.h>
    #include <stdio.h>
    #include <fcntl.h>
#endif

#include <sys/stat.h>

using namespace o3d;

FileInStream::FileInStream() :
    m_file(nullptr),
    m_length(0)
{

}

FileInStream::FileInStream(const String &filename) :
    m_file(nullptr),
    m_length(0)
{
    String name(filename);
    name.replace('\\', '/');

    // file open
#ifdef _MSC_VER
    _wfopen_s(&m_file, name.getData(), "rb");
#else
    m_file = fopen(name.toUtf8().getData(), "rb");
#endif

    if (!m_file) {
        O3D_ERROR(E_FileNotFoundOrInvalidRights("", name));
    }
}

FileInStream::FileInStream(const BaseFileInfo &file) :
    m_file(nullptr),
    m_length(0)
{
    String filename(file.getFullFileName());

    // file open
#ifdef _MSC_VER
    _wfopen_s(&m_file, filename.getData(), "rb");
#else
    m_file = fopen(filename.toUtf8().getData(), "rb");
#endif

    if (!m_file) {
        O3D_ERROR(E_FileNotFoundOrInvalidRights("", filename));
    }
}

FileInStream::~FileInStream()
{
    close();
}

Bool FileInStream::isMemory() const
{
    return False;
}

UInt32 FileInStream::reader(void *buf, UInt32 size, UInt32 count)
{
    return (UInt32)fread(buf, size, count, m_file) * size;
}

void FileInStream::close()
{
    if (m_file) {
        fclose(m_file);
        m_file = nullptr;
    }
}

void FileInStream::reset(UInt64 n)
{
    if (n == 0) {
        rewind(m_file);
    } else if (fseek(m_file, n, SEEK_SET) != 0) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }
}

void FileInStream::seek(Int64 n)
{
    if (fseek(m_file, n, SEEK_CUR) != 0) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }
}

void FileInStream::end(Int64 n)
{
    if (fseek(m_file, n, SEEK_END) != 0) {
        O3D_ERROR(E_IndexOutOfRange(""));
    }
}

UInt8 FileInStream::peek()
{
    UInt8 value = (UInt8)getc(m_file);
    ungetc(value, m_file);

    return value;
}

void FileInStream::ignore(Int32 limit, UInt8 delim)
{
    Int32 value;
    Int32 counter = 0;

    while ((counter < limit) && ((value = getc(m_file)) != EOF) && ((UInt8)value != delim)) {
        ++counter;
    }
}

static inline Int64 getFileLength(int fd)
{
#ifdef _MSC_VER
    struct _stat64 st_file;
    _fstat64(fd, &st_file);
    return (Int64)st_file.st_size;
#else
    struct stat st_file;
    fstat(fd, &st_file);
    return (Int64)st_file.st_size;
#endif
}

Int32 FileInStream::getAvailable() const
{
    if (m_length == 0) {
        *const_cast<Int32*>(&m_length) = getFileLength(getFD());
    }

    if (m_length != 0) {
/*        fpos_t pos;
        fgetpos(m_file, &pos);

#ifdef O3D_WINDOWS
        return m_length - pos;
#else
        return m_length - pos.__pos;
#endif*/
        return m_length - ftell(m_file);
    } else {
        return 0;
    }
}

Int32 FileInStream::getPosition() const
{
    return ftell(m_file);
}

Bool FileInStream::isEnd() const
{
    return feof(m_file) != 0;
}

int FileInStream::getFD() const
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

FILE *FileInStream::getFile() const
{
    return m_file;
}

Int32 FileInStream::readLine(String &str, CharacterEncoding encoding)
{
    ArrayChar read;

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

    return str.length();
}

Int32 FileInStream::readLine(String &str, Int32 limit, UInt8 delim, CharacterEncoding encoding)
{
    ArrayChar read;

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
    }

    return str.length();
}

Int32 FileInStream::readWord(String &str, CharacterEncoding encoding)
{
    ArrayChar read;

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
    }

    return str.length();
}

/*
    // managed reading
    if (m_blockDataR.m_use)
    {
        UInt32 totalsize = size * count;   // total size to read in bytes

        // only for small data
        if (totalsize < 128)
        {
            return (UInt32)fread(buf, size, count, m_file) * size;
        }

        UInt8 *data = (UInt8*)buf;  // current data ptr
        UInt32 curpos = 0;              // nbr bytes currently read
        UInt32 load,loaded;

        while (curpos != totalsize)
        {
            load = m_blockDataR.m_size;
            if ((totalsize - curpos) < m_blockDataR.m_size)
            {
                load = totalsize - curpos;
            }

            // load data
            loaded = (UInt32)fread(data, 1, load, m_file);
            if (loaded <= load)
                curpos += load;

            data += loaded;

            // delay
            if (totalsize < m_blockDataR.m_size) // one time load with totalsize < blocksize
                System::waitMs((UInt32)((Float)m_blockDataR.m_delay*(Float)totalsize/(Float)m_blockDataR.m_size));
            else if (load < m_blockDataR.m_size) // last time load with load < blocksize
                System::waitMs((UInt32)((Float)m_blockDataR.m_delay*(Float)load/(Float)m_blockDataR.m_size));
            else								// each time delay
                System::waitMs(m_blockDataR.m_delay);
        }

        return curpos;
    }
*/
