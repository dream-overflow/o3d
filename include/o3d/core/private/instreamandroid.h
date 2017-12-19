/**
 * @file instreamandroid.h
 * @brief Specialization for android asset.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-17
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_INSTREAMANDROID_H
#define _O3D_INSTREAMANDROID_H

#include "../instream.h"

#ifdef O3D_ANDROID

class AAsset;

namespace o3d {

/**
 * @brief InStreamAndroid specialization for reading into android assets.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-17
 */
class O3D_API InStreamAndroid : public InStream
{
public:

    /**
     * @brief InStreamAndroid Open a file stream in reading from the android asset.
     * @param filename File name from the asset.
     * @param asset Valid to own Android asset.
     * @throw E_FileNotFoundOrInvalidRights
     */
    InStreamAndroid(const String &filename, AAsset *asset);

    virtual ~InStreamAndroid();

    //! False
    virtual Bool isMemory() const;

    virtual UInt32 reader(void* buf, UInt32 size, UInt32 count);

    virtual void close();

    virtual void reset(UInt64 n);

    virtual void seek(Int64 n);

    virtual void end(Int64 n);

    virtual UInt8 peek();

    virtual void ignore(Int32 limit, UInt8 delim);

    virtual Int32 getAvailable() const;

    virtual Int32 getPosition() const;

    virtual Bool isEnd() const;

    //! Get the AAsset handler.
    AAsset* getAsset() const;

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

protected:

    InStreamAndroid();

    AAsset *m_asset;    //!< Android asset
    UInt64 m_pos;       //!< Seek position
    UInt64 m_length;    //!< Cached file size
};

} // namespace o3d

#endif // O3D_ANDROID

#endif // _O3D_INSTREAMANDROID_H
