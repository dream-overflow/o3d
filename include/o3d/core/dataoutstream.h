/**
 * @file dataoutstream.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DATAOUTSTREAM_H
#define _O3D_DATAOUTSTREAM_H

#include "outstream.h"
#include "templatearray.h"

namespace o3d {

/**
 * @brief DataOutStream base on a ArrayUInt8
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-12-04
 */
class O3D_API DataOutStream : public OutStream
{
public:

    /**
     * @brief DataOutStream
     * @param array An array that will growth (use a good threshold for it)
     * @param own If True the array is deleted a close or destructor
     */
    DataOutStream(ArrayUInt8 &array, Bool own = False);

    virtual ~DataOutStream() override;

    //! True
    virtual Bool isMemory() const override;

    //! write buf with size * count. reallocate the array as necessary
    virtual UInt32 writer(const void* buf, UInt32 size, UInt32 count) override;

    //! close the stream.
    virtual void close() override;

    //! flush the stream.
    virtual void flush() override;

    virtual Int32 writeLine(
            const String &str,
            CharacterEncoding encoding = ENCODING_UTF8) override;

    virtual Int32 writeString(
            const String &str,
            CharacterEncoding encoding = ENCODING_UTF8) override;

private:

    Bool m_own;
    ArrayUInt8 *m_data;
};

} // namespace o3d

#endif // _O3D_DATAOUTSTREAM_H
