/**
 * @file outstream.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_OUTSTREAM_H
#define _O3D_OUTSTREAM_H

#include "textoutstream.h"
#include "closable.h"

#include <type_traits>

namespace o3d {

/**
 * @brief OutStream Binary output stream base class
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-12-04
 */
class O3D_API OutStream : public TextOutStream, public Closable
{
public:

    virtual ~OutStream() = 0;

    //! Is the stream go in memory (True), or to an other location (False).
    virtual Bool isMemory() const = 0;

    //! write buf with size * count
    virtual UInt32 writer(const void* buf, UInt32 size, UInt32 count) = 0;

    //! flush the stream.
    virtual void flush() = 0;

    //! Write buf with size * count.
//    template <typename T>
//    UInt32 write(const T* buf, UInt32 size, UInt32 count)
//    {
//        return writer(buf, size, count);
//    }

    //-----------------------------------------------------------------------------------
    // Template specialization for endian conversions
    //-----------------------------------------------------------------------------------

    // INT8 (1bytes)
    UInt32 write(const Int8* buf, UInt32 count)
    {
        return writer(buf, sizeof(Int8), count);
    }

    // UINT8 (1bytes)
    UInt32 write(const UInt8* buf, UInt32 count)
    {
        return writer(buf, sizeof(UInt8), count);
    }

    // USHORT (2bytes)
    UInt32 write(const UInt16* buf, UInt32 count)
    {
        #ifdef O3D_BIG_ENDIAN
        UInt16* pdata = new UInt16[count];

        for (UInt32 i = 0; i < count; ++i)
        {
            pdata[i] = buf[i];
            System::swapBytes2(&pdata[i]);
        }

        UInt32 ret = writer(pdata, sizeof(UInt16), count);
        deleteArray(pdata);
        return ret;
        #else
        return writer(buf, sizeof(UInt16), count);
        #endif
    }

    // SHORT (2bytes)
    UInt32 write(const Int16* buf, UInt32 count)
    {
        #ifdef O3D_BIG_ENDIAN
        Int16* pdata = new Int16[count];

        for (UInt32 i = 0; i < count; ++i)
        {
            pdata[i] = buf[i];
            System::swapBytes2(&pdata[i]);
        }

        UInt32 ret = writer(pdata, sizeof(Int16), count);
        deleteArray(pdata);
        return ret;
        #else
        return writer(buf, sizeof(Int16), count);
        #endif
    }

    // UINT (4bytes)
    UInt32 write(const UInt32* buf, UInt32 count)
    {
        #ifdef O3D_BIG_ENDIAN
        UInt32* pdata = new UInt32[count];

        for (UInt32 i = 0; i < count; ++i)
        {
            pdata[i] = buf[i];
            System::swapBytes4(&pdata[i]);
        }

        UInt32 ret = writer(pdata, sizeof(UInt32), count);
        deleteArray(pdata);
        return ret;
        #else
        return writer(buf, sizeof(UInt32), count);
        #endif
    }

    // INT (4bytes)
    UInt32 write(const Int32* buf, UInt32 count)
    {
        #ifdef O3D_BIG_ENDIAN
        Int32* pdata = new Int32[count];

        for (UInt32 i = 0; i < count; ++i)
        {
            pdata[i] = buf[i];
            System::swapBytes4(&pdata[i]);
        }

        UInt32 ret = writer(pdata, sizeof(Int32), count);
        deleteArray(pdata);
        return ret;
        #else
        return writer(buf, sizeof(Int32), count);
        #endif
    }

    // ULINT (8bytes)
    UInt32 write(const UInt64* buf, UInt32 count)
    {
        #ifdef O3D_BIG_ENDIAN
        UInt64* pdata = new UInt64[count];

        for (UInt32 i = 0; i < count; ++i)
        {
            pdata[i] = buf[i];
            System::swapBytes8(&pdata[i]);
        }

        UInt32 ret = writer(pdata, sizeof(UInt64), count);
        deleteArray(pdata);
        return ret;
        #else
        return writer(buf, sizeof(UInt64), count);
        #endif
    }

    // LINT (8bytes)
    UInt32 write(const Int64* buf, UInt32 count)
    {
        #ifdef O3D_BIG_ENDIAN
        Int64* pdata = new Int64[count];

        for (UInt32 i = 0; i < count; ++i)
        {
            pdata[i] = buf[i];
            System::swapBytes8(&pdata[i]);
        }

        UInt32 ret = writer(pdata, sizeof(Int64), count);
        deleteArray(pdata);
        return ret;
        #else
        return writer(buf, sizeof(Int64), count);
        #endif
    }

    // FLOAT (4bytes)
    UInt32 write(const Float* buf, UInt32 count)
    {
        #ifdef O3D_BIG_ENDIAN
        Float* pdata = new Float[count];

        for (UInt32 i = 0; i < count; ++i)
        {
            pdata[i] = buf[i];
            System::swapBytes4(&pdata[i]);
        }

        UInt32 ret = writer(pdata, sizeof(Float), count);
        deleteArray(pdata);
        return ret;
        #else
        return writer(buf, sizeof(Float), count);
        #endif
    }


    // DOUBLE (8bytes)
    UInt32 write(const Double* buf, UInt32 count)
    {
        #ifdef O3D_BIG_ENDIAN
        Double* pdata = new Double[count];

        for (UInt32 i = 0; i < count; ++i)
        {
            pdata[i] = buf[i];
            System::swapBytes8(&pdata[i]);
        }

        UInt32 ret = writer(pdata, sizeof(Double), count);
        deleteArray(pdata);
        return ret;
        #else
        return writer(buf, sizeof(Double), count);
        #endif
    }

    struct BoolIsInt
    {
        static inline void write(OutStream &ostream, const Bool t)
        {
            UInt8 tmp = t ? 1 : 0;
            ostream.writer(&tmp,1,1);
        }
    };

    struct BoolIsUChar
    {
        static inline void write(OutStream &ostream, const Bool t)
        {
            ostream.writer(&t, 1, 1);
        }
    };

    typedef StaticCond<sizeof(Bool)>=2, BoolIsInt, BoolIsUChar>::Result BoolWrite;

    // Bool
    inline void writeBool(const Bool &t)
    {
        BoolWrite::write(*this, t);
    }
    inline OutStream& operator<< (Bool t)
    {
        BoolWrite::write(*this, t);
        return *this;
    }

    // Char/Int8
    inline void writeChar(const Char &t)
    {
        write(&t, 1);
    }
    inline void writeInt8(const Int8 &t)
    {
        write(&t, 1);
    }
    inline OutStream& operator<< (Char t)
    {
        write(&t, 1);
        return *this;
    }

    // UInt8
    inline void writeUInt8(const UInt8 &t)
    {
        write(&t, 1);
    }
    inline OutStream& operator<< (UInt8 t)
    {
        write(&t, 1);
        return *this;
    }

    // Int16
    inline void writeInt16(const Int16 &t)
    {
        write(&t, 1);
    }
    inline OutStream& operator<< (Int16 t)
    {
        write(&t, 1);
        return *this;
    }

    // UInt16
    inline void writeUInt16(const UInt16 &t)
    {
        write(&t, 1);
    }
    inline OutStream& operator<< (UInt16 t)
    {
        write(&t, 1);
        return *this;
    }

    // Int32
    inline void writeInt32(const Int32 &t)
    {
        write(&t, 1);
    }
    inline OutStream& operator<< (Int32 t)
    {
        write(&t, 1);
        return *this;
    }

    // UInt32
    inline void writeUInt32(const UInt32 &t)
    {
        write(&t, 1);
    }
    inline OutStream& operator<< (UInt32 t)
    {
        write(&t, 1);
        return *this;
    }

    // Int64
    inline void writeInt64(const Int64 &t)
    {
        write(&t, 1);
    }
    inline OutStream& operator<< (Int64 t)
    {
        write(&t, 1);
        return *this;
    }

    // UInt64
    inline void writeUInt64(const UInt64 &t)
    {
        write(&t, 1);
    }
    inline OutStream& operator<< (UInt64 t)
    {
        write(&t, 1);
        return *this;
    }

    // Float
    inline void writeFloat(const Float &t)
    {
        write(&t, 1);
    }
    inline OutStream& operator<< (Float t)
    {
        write(&t, 1);
        return *this;
    }

    // Double
    inline void writeDouble(const Double &t)
    {
        write(&t, 1);
    }
    inline OutStream& operator<< (Double t)
    {
        write(&t, 1);
        return *this;
    }
};


template <class _T>
struct StreamObjectIsEnumWrite
{
    static inline void write(OutStream &ostream, _T &object)
    {
        ostream << Int32(object);
    }

    static inline void write(OutStream &ostream, const _T &object)
    {
        ostream << Int32(object);
    }
};

template <class _T>
struct StreamObjectIsClassWrite
{
    static inline void write(OutStream &ostream, _T &object)
    {
        object.writeToFile(ostream);
    }

    static inline void write(OutStream &ostream, const _T &object)
    {
        object.writeToFile(ostream);
    }
};

template <class _T>
struct StreamObjectWrite
{
    typedef typename StaticCond<std::is_enum<_T>::value, StreamObjectIsEnumWrite<_T>, StreamObjectIsClassWrite<_T> >::Result Result;
};

// serialize
template <class _T>
inline OutStream& operator<< (OutStream &os, const _T &object)
{
    StreamObjectWrite<_T>::Result::write(os, object);
    return os;
}

// serialize
template <class _T>
inline OutStream& operator<< (OutStream &os, _T &object)
{
    StreamObjectWrite<_T>::Result::write(os, object);
    return os;
}

} // namespace o3d

#endif // _O3D_OUTSTREAM_H

