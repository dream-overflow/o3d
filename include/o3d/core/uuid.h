/**
 * @file uuid.h
 * @brief Uuid object
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_UUID_H
#define _O3D_UUID_H

#include "string.h"
#include "smartarray.h"

namespace o3d {

class InStream;
class OutStream;

/**
 * @brief The Uuid class
 */
class O3D_API Uuid
{
    friend class Application;

public:

    enum Version
    {
        VERSION_1 = 1,
        VERSION_3 = 3,
        VERSION_5 = 5
    };

    static const Uuid& nullUuid();
    static Uuid makeUuid(Version version = VERSION_5);

    /**
     * @brief By defaut each a node id is generated staticaly for the current application instance.
     * @param nodeId Valid node identifier.
     */
    static void setIEEENodeId(const SmartArrayUInt8 &nodeId);

    /**
     * @brief Set the initial time at a precision of 100ns since Oct 15, 1582.
     * @param time
     */
    static void setTime(UInt64 time);

    static void init();
    static void quit();

    Uuid();
    Uuid(const Char *uuid);
    Uuid(const CString &uuid);
    Uuid(const String &uuid);
    Uuid(const Uuid &dup);

    Uuid& operator= (const Uuid &dup);

    const SmartArrayUInt8& raw() const;

    //! As cstring with { and }
    CString toCString() const;

    //! As string with { and }
    String toString() const;

    //! As string without { }
    String toRfc4122() const;

    Bool isNull() const;
    Bool isValid() const;

    //! @return True if s < *this otherwise false.
    Bool operator< (const Uuid& _which) const;

    //! @return True if s <= *this otherwise false.
    Bool operator<= (const Uuid& _which) const;

    //! @return True if s > *this otherwise false.
    Bool operator> (const Uuid& _which) const;

    //! @return True if s >= *this otherwise false.
    Bool operator>= (const Uuid& _which) const;

    //! @return True if s == *this otherwise false.
    Bool operator== (const Uuid& _which) const;

    //! @return True if s != *this otherwise false.
    Bool operator!= (const Uuid& _which) const;

    //! Time lower part (first 32 bits)
    inline UInt32 timeLow() const { return *reinterpret_cast<const UInt32*>(&m_raw.getData()[0]); }

    //! Time middle part (next 16 bits)
    inline UInt16 timeMid() const { return *reinterpret_cast<const UInt16*>(&m_raw.getData()[4]); }

    //! Time/version part (next 16 bits)
    inline UInt16 timeHiVersion() const { return *reinterpret_cast<const UInt16*>(&m_raw.getData()[6]); }

    //! Clock sequence high part (next 8 bits)
    inline UInt8 clockSeqHiVariant() const { return m_raw.getData()[8]; }

    //! Clock low part (next 8 bits)
    inline UInt8 clockSeqLow() const { return m_raw.getData()[9]; }

    //! Node part (last 48 bits)
    inline const UInt8* node() const { return &m_raw.getData()[10]; }

    //! Version (1 to 5)
    UInt8 version() const;

    //
    // Serialization
    //

    Bool writeToFile(OutStream &os) const;
    Bool readFromFile(InStream &is);

private:

    SmartArrayUInt8 m_raw;

    void *m_privateData;
};

} // namespace o3d

namespace std {

template<>
struct hash<o3d::Uuid> {
    enum
    {
        bucket_size = 8,
        min_buckets = 16
    };

    bool operator()(const o3d::Uuid &s1, const o3d::Uuid &s2) const
    {
        return memcmp(
                    s1.raw().getData(),
                    s2.raw().getData(),
                    s1.raw().getSizeInBytes() < s2.raw().getSizeInBytes() ? s1.raw().getSizeInBytes() : s2.raw().getSizeInBytes()) < 0;
    }

    size_t operator()(const o3d::Uuid &s1) const
    {
        size_t  h = 0;

        o3d::Char *p = reinterpret_cast<o3d::Char*>(const_cast<o3d::UInt8*>(s1.raw().getData()));
        o3d::Char zero = '\0';

        while ( *p != zero ) h = 31 * h + (*p++);

        return h;
    }

    /*std::size_t operator()(o3d::Uuid &c) const
            {
                std::hash<o3d::Char*> hash;
                if (c.isValid())
                    return hash(c.raw().getData());
                else
                    return 0;
            }*/
};

}

#endif // _O3D_UUID_H
