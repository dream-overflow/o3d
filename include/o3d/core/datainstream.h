/**
 * @file datainstream.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DATAINSTREAM_H
#define _O3D_DATAINSTREAM_H

#include "instream.h"
#include "templatearray.h"

namespace o3d {

/**
 * @brief DataInStream base on a ArrayUInt8
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-12-04
 */
class O3D_API DataInStream : public InStream
{
public:

    /**
     * @brief DataInStream Stream a valid array.
     * @param array Valid array.
     * @param own True mean delete this array at close or destructor.
     */
    DataInStream(const ArrayUInt8 &array, Bool own = False);

    virtual ~DataInStream();

    //! True
    virtual Bool isMemory() const;

    virtual UInt32 reader(void *buf, UInt32 size, UInt32 count);

    virtual void close();

    virtual void reset(UInt64 n = 0);

    virtual void seek(Int64 n);

    virtual void end(Int64 n = 0);

    virtual Int32 getAvailable() const;

    virtual Int32 getPosition() const;

    virtual Bool isEnd() const;

    virtual UInt8 peek();

    virtual void ignore(Int32 limit, UInt8 delim);

    virtual Int32 readLine(
            String &str,
            CharacterEncoding encoding = ENCODING_UTF8);

    virtual Int32 readLine(
            String &str,
            Int32 limit,
            UInt8 delim,
            CharacterEncoding encoding = ENCODING_UTF8);

    virtual Int32 readWord(
            String &str,
            CharacterEncoding encoding = ENCODING_UTF8);

private:

    Bool m_own;

    const ArrayUInt8 *m_data;
    const UInt8 *m_pos;
};

/**
 * @brief SharedDataInStream base on a SmartArrayUInt8
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-12-05
 */
class O3D_API SharedDataInStream : public InStream
{
public:

    /**
     * @brief SharedDataInStream Stream a valid shared array.
     * @param array Valid shared array.
     */
    SharedDataInStream(const SmartArrayUInt8 &sharedArray);

    virtual ~SharedDataInStream();

    //! True
    virtual Bool isMemory() const;

    virtual UInt32 reader(void *buf, UInt32 size, UInt32 count);

    virtual void close();

    virtual void reset(UInt64 n = 0);

    virtual void seek(Int64 n);

    virtual void end(Int64 n = 0);

    virtual Int32 getAvailable() const;

    virtual Int32 getPosition() const;

    virtual Bool isEnd() const;

    virtual UInt8 peek();

    virtual void ignore(Int32 limit, UInt8 delim);

    virtual Int32 readLine(
            String &str,
            CharacterEncoding encoding = ENCODING_UTF8);

    virtual Int32 readLine(
            String &str,
            Int32 limit,
            UInt8 delim,
            CharacterEncoding encoding = ENCODING_UTF8);

    virtual Int32 readWord(
            String &str,
            CharacterEncoding encoding = ENCODING_UTF8);
private:

    SmartArrayUInt8 m_data;
    UInt8 *m_pos;
};

} // namespace o3d

#endif // _O3D_DATAINSTREAM_H

