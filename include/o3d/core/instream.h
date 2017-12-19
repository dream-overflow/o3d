/**
 * @file instream.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_INSTREAM_H
#define _O3D_INSTREAM_H

#include "textinstream.h"
#include "closable.h"

#include <type_traits>

namespace o3d {

/**
 * @brief InStream binary stream base class
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-12-04
 * @todo Must be fully 64 bits
 */
class O3D_API InStream : public TextInStream, public Closable
{
public:

    virtual ~InStream() = 0;

    //! Is the stream come from memory (True), or from an other location (False).
    virtual Bool isMemory() const = 0;

    //! read buf with size * count.
    virtual UInt32 reader(void* buf, UInt32 size, UInt32 count) = 0;

    /**
     * @brief reset the stream to its first position.
     * @param n Initially 0, but can be any other positive position from start.
     */
    virtual void reset(UInt64 n = 0) = 0;

    /**
     * @brief seek of n bytes.
     * @param n Can be negative to seek before current position.
     */
    virtual void seek(Int64 n) = 0;

    /**
     * @brief end position.
     * @param n Initially 0, but can be any other negative position from end.
     */
    virtual void end(Int64 n = 0) = 0;

    //! peek the next char without changing the current stream position.
    virtual UInt8 peek() = 0;

    /**
     * @brief ignore Read until delim is found or EOF is reached.
     * @param limit Limit in number of characters to ignore
     * @param delim Delimiter byte that stop the ignore if found before limit
     */
    virtual void ignore(Int32 limit, UInt8 delim) = 0;

    //! get an estimation of the number of bytes availables for read.
    virtual Int32 getAvailable() const = 0;

    //! get the stream position.
    virtual Int32 getPosition() const = 0;

    //! is the end of the stream reached
    virtual Bool isEnd() const = 0;

    //! Read buf with size * count.
//    template <typename T>
//    UInt32 read(T* buf, UInt32 size, UInt32 count)
//    {
//        return reader(buf, size, count);
//    }

    //-----------------------------------------------------------------------------------
    // Template specialization for endian conversions
    //-----------------------------------------------------------------------------------

    // INT8 (1bytes)
    UInt32 read(Int8* buf, UInt32 count)
    {
        return reader(buf, sizeof(Int8), count);
    }

    // UINT8 (1bytes)
    UInt32 read(UInt8* buf, UInt32 count)
    {
        return reader(buf, sizeof(UInt8), count);
    }

    // USHORT (2bytes)
    UInt32 read(UInt16* buf, UInt32 count)
    {
        #ifdef O3D_BIG_ENDIAN
        UInt32 ret = reader(buf, sizeof(UInt16), count);
        UInt32 nbrElt = ret / sizeof(UInt16);

        for (UInt32 i = 0; i < nbrElt; ++i)
            System::swapBytes2(&buf[i]);

        return ret;
        #else
        return reader(buf, sizeof(UInt16), count);
        #endif
    }

    // SHORT (2bytes)
    UInt32 read(Int16* buf, UInt32 count)
    {
        #ifdef O3D_BIG_ENDIAN
        UInt32 ret = reader(buf, sizeof(Int16), count);
        UInt32 nbrElt = ret / sizeof(Int16);

        for (UInt32 i = 0 ; i < nbrElt ; ++i)
            System::swapBytes2(&buf[i]);

        return ret;
        #else
        return reader(buf, sizeof(Int16), count);
        #endif
    }

    // UINT (4bytes)
    UInt32 read(UInt32* buf, UInt32 count)
    {
        #ifdef O3D_BIG_ENDIAN
        UInt32 ret = reader(buf, sizeof(UInt32), count);
        UInt32 nbrElt = ret / sizeof(UInt32);

        for (UInt32 i = 0 ; i < nbrElt ; ++i)
            System::swapBytes4(&buf[i]);

        return ret;
        #else
        return reader(buf, sizeof(UInt32), count);
        #endif
    }

    // INT (4bytes)
    UInt32 read(Int32* buf, UInt32 count)
    {
        #ifdef O3D_BIG_ENDIAN
        UInt32 ret = reader(buf, sizeof(Int32), count);
        UInt32 nbrElt = ret / sizeof(Int32);

        for (UInt32 i = 0 ; i < nbrElt ; ++i)
           System::swapBytes4(&buf[i]);

        return ret;
        #else
        return reader(buf, sizeof(Int32), count);
        #endif
    }

    // ULINT (8bytes)
    UInt32 read(UInt64* buf, UInt32 count)
    {
        #ifdef O3D_BIG_ENDIAN
        UInt32 ret = reader(buf, sizeof(UInt64), count);
        UInt32 nbrElt = ret / sizeof(UInt64);

        for (UInt32 i = 0 ; i < nbrElt ; ++i)
            System::swapBytes8(&buf[i]);

        return ret;
        #else
        return reader(buf, sizeof(UInt64), count);
        #endif
    }

    // LINT (8bytes)
    UInt32 read(Int64* buf, UInt32 count)
    {
        #ifdef O3D_BIG_ENDIAN
        UInt32 ret = reader(buf, sizeof(Int64), count);
        UInt32 nbrElt = ret / sizeof(Int64);

        for (UInt32 i = 0 ; i < nbrElt ; ++i)
            System::swapBytes8(&buf[i]);

        return ret;
        #else
        return reader(buf, sizeof(Int64), count);
        #endif
    }

    // FLOAT (4bytes)
    UInt32 read(Float* buf, UInt32 count)
    {
        #ifdef O3D_BIG_ENDIAN
        UInt32 ret = reader(buf, sizeof(Float), count);
        UInt32 nbrElt = ret / sizeof(Float);

        for (UInt32 i = 0 ; i < nbrElt ; ++i)
            System::swapBytes4(&buf[i]);

        return ret;
        #else
        return reader(buf, sizeof(Float), count);
        #endif
    }

    // DOUBLE (8bytes)
    UInt32 read(Double* buf, UInt32 count)
    {
        #ifdef O3D_BIG_ENDIAN
        UInt32 ret = reader(buf, sizeof(Double), count);
        UInt32 nbrElt = ret / sizeof(Double);

        for (UInt32 i = 0 ; i < nbrElt ; ++i)
            System::swapBytes8(&buf[i]);

        return ret;
        #else
        return reader(buf, sizeof(Double), count);
        #endif
    }

    struct BoolIsInt
    {
        static inline void read(InStream &istream, Bool &t)
        {
            UInt8 tmp;
            istream.reader(&tmp, 1, 1);
            t = tmp != 0;
        }
    };

    struct BoolIsUChar
    {
        static inline void read(InStream &istream, Bool &t)
        {
            istream.reader(&t, 1, 1);
        }
    };

    typedef StaticCond<sizeof(Bool)>=2, BoolIsInt, BoolIsUChar>::Result BoolRead;

    // Bool
    inline Bool readBool()
    {
        Bool t;
        BoolRead::read(*this, t);
        return t;
    }
    inline InStream& operator>> (Bool &t)
    {
        BoolRead::read(*this, t);
        return *this;
    }

    // Char/Int8
    inline Char readChar()
    {
        Char t;
        read(&t, 1);
        return t;
    }
    inline Int8 readInt8()
    {
        Int8 t;
        read(&t, 1);
        return t;
    }
    inline InStream& operator>> (Char &t)
    {
        read(&t, 1);
        return *this;
    }

    // UInt8
    inline UInt8 readUInt8()
    {
        UInt8 t;
        read(&t, 1);
        return t;
    }
    inline InStream& operator>> (UInt8 &t)
    {
        read(&t, 1);
        return *this;
    }

    // Int16
    inline Int16 readInt16()
    {
        Int16 t;
        read(&t, 1);
        return t;
    }
    inline InStream& operator>> (Int16 &t)
    {
        read(&t, 1);
        return *this;
    }

    // UInt16
    inline UInt16 readUInt16()
    {
        UInt16 t;
        read(&t, 1);
        return t;
    }
    inline InStream& operator>> (UInt16 &t)
    {
        read(&t, 1);
        return *this;
    }

    // Int32
    inline Int32 readInt32()
    {
        Int32 t;
        read(&t, 1);
        return t;
    }
    inline InStream& operator>> (Int32 &t)
    {
        read(&t, 1);
        return *this;
    }

    // UInt32
    inline UInt32 readUInt32()
    {
        UInt32 t;
        read(&t, 1);
        return t;
    }
    inline InStream& operator>> (UInt32 &t)
    {
        read(&t, 1);
        return *this;
    }

    // Int64
    inline Int64 readInt64()
    {
        Int64 t;
        read(&t,1);
        return t;
    }
    inline InStream& operator>> (Int64 &t)
    {
        read(&t, 1);
        return *this;
    }

    // UInt64
    inline UInt64 readUInt64()
    {
        UInt64 t;
        read(&t, 1);
        return t;
    }
    inline InStream& operator>> (UInt64 &t)
    {
        read(&t, 1);
        return *this;
    }

    // Float
    inline Float readFloat()
    {
        Float t;
        read(&t, 1);
        return t;
    }
    inline InStream& operator>> (Float &t)
    {
        read(&t, 1);
        return *this;
    }

    // Double
    inline Double readDouble()
    {
        Double t;
        read(&t, 1);
        return t;
    }
    inline InStream& operator>> (Double &t)
    {
        read(&t, 1);
        return *this;
    }
};


template <class _T>
struct StreamObjectIsEnumRead
{
    static inline void read(InStream &istream, _T &object)
    {
        Int32 int32;
        istream >> int32;
        object = _T(int32);
    }
};

template <class _T>
struct StreamObjectIsClassRead
{
    static inline void read(InStream &istream, _T &object)
    {
        object.readFromFile(istream);
    }
};

template <class _T>
struct StreamObjectRead
{
    typedef typename StaticCond<std::is_enum<_T>::value, StreamObjectIsEnumRead<_T>, StreamObjectIsClassRead<_T> >::Result Result;
};

// de-serialize
template <class _T>
inline InStream& operator>> (InStream &is, _T &object)
{
    StreamObjectRead<_T>::Result::read(is, object);
    return is;
}

} // namespace o3d

#endif // _O3D_INSTREAM_H
