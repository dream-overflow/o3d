/**
 * @file uuid.cpp
 * @brief Uuid object
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/uuid.h"
#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

#ifdef __cplusplus
extern "C" {
#endif

// #include "../../third/sha/sha1.h"
#include "../../third/uuid/sysdep.h"
#include "../../third/uuid/uuid.h"

#ifdef __cplusplus
}
#endif

using namespace o3d;

static inline UInt8 fromHexAB(WChar a, WChar b)
{
    UInt8 r;

    if (towlower(a) >= 'a') {
        r = (a - 'a' + 10) << 4;
    } else {
        r = (a - '0') << 4;
    }

    if (towlower(b) >= 'a') {
        r += b - 'a' + 10;
    } else {
        r += b - '0';
    }

    return r;
}

static inline UInt8 fromHexAB(Char a, Char b)
{
    UInt8 r;

    if (tolower(a) >= 'a') {
        r = (a - 'a' + 10) << 4;
    } else {
        r = (a - '0') << 4;
    }

    if (tolower(b) >= 'a') {
        r += b - 'a' + 10;
    } else {
        r += b - '0';
    }

    return r;
}

const Uuid &Uuid::nullUuid()
{
    static Uuid nullUuid;
    return nullUuid;
}

Uuid Uuid::makeUuid()
{
    Uuid uuid;

    uuid_t nsid;
    uuid_create(&nsid);

    uuid_t u;
    uuid_create_md5_from_name(&u, nsid, (void*)"objective3d", 11);

    *reinterpret_cast<UInt32*>(&uuid.m_raw.getData()[0]) = u.time_low;
    *reinterpret_cast<UInt16*>(&uuid.m_raw.getData()[4]) = u.time_mid;
    *reinterpret_cast<UInt16*>(&uuid.m_raw.getData()[6]) = u.time_hi_and_version;
    uuid.m_raw.getData()[8] = u.clock_seq_hi_and_reserved;
    uuid.m_raw.getData()[9] = u.clock_seq_low;
    memcpy(&uuid.m_raw.getData()[10], &u.node, 6);

    return uuid;
}

Uuid::Uuid() :
    m_raw(16)
{
    memset(m_raw.getData(), 0, 16);
}

template<class T>
void fromHex(T* data, SmartArrayUInt8 &raw)
{
    if (data[8] == '-' || data[13] == '-' || data[18] == '-' || data[23] == '-') {
        UInt8 *dst = raw.getData();

        dst[0] = fromHexAB(data[0], data[1]);
        dst[1] = fromHexAB(data[2], data[3]);
        dst[2] = fromHexAB(data[4], data[5]);
        dst[3] = fromHexAB(data[6], data[7]);

        dst[4] = fromHexAB(data[9], data[10]);
        dst[5] = fromHexAB(data[11], data[12]);

        dst[6] = fromHexAB(data[14], data[15]);
        dst[7] = fromHexAB(data[16], data[17]);

        dst[8] = fromHexAB(data[19], data[20]);
        dst[9] = fromHexAB(data[21], data[22]);

        dst[10] = fromHexAB(data[24], data[25]);
        dst[11] = fromHexAB(data[26], data[27]);
        dst[12] = fromHexAB(data[28], data[29]);
        dst[13] = fromHexAB(data[30], data[31]);
        dst[14] = fromHexAB(data[32], data[33]);
        dst[15] = fromHexAB(data[34], data[35]);
    } else {
        memset(raw.getData(), 0, 16);
    }
}

template<class T>
static inline void toHexAB(const UInt8 &d, T &a, T &b)
{
    UInt8 p = (d & 0xf0) >> 4;

    if (p >= 10) {
        a = p - 10 + 'a';
    } else {
        a = p + '0';
    }

    p = (d & 0x0f);

    if (p >= 10) {
        b = p - 10 + 'a';
    } else {
        b = p + '0';
    }
}

template<class T>
void toHex(const UInt8* raw, T* dst)
{
    dst[8] = dst[13] = dst[18] = dst[23] = '-';

    toHexAB(raw[0], dst[0], dst[1]);
    toHexAB(raw[1], dst[2], dst[3]);
    toHexAB(raw[2], dst[4], dst[5]);
    toHexAB(raw[3], dst[6], dst[7]);

    toHexAB(raw[4], dst[9], dst[10]);
    toHexAB(raw[5], dst[11], dst[12]);

    toHexAB(raw[6], dst[14], dst[15]);
    toHexAB(raw[7], dst[16], dst[17]);

    toHexAB(raw[8], dst[19], dst[20]);
    toHexAB(raw[9], dst[21], dst[22]);

    toHexAB(raw[10], dst[24], dst[25]);
    toHexAB(raw[11], dst[26], dst[27]);
    toHexAB(raw[12], dst[28], dst[29]);
    toHexAB(raw[13], dst[30], dst[31]);
    toHexAB(raw[14], dst[32], dst[33]);
    toHexAB(raw[15], dst[34], dst[35]);
}

Uuid::Uuid(const Char *uuid) :
    m_raw(16)
{
    size_t len = 0;

    if (uuid) {
        len = strlen(uuid);
    }

    if (len == 38 && uuid[0] == '{' && uuid[len-1] == '}') {
        // from string format
        const Char *data = uuid + 1;
        fromHex(data, m_raw);
    } else if (len == 36) {
        // from rfc4122
        const Char *data = uuid;
        fromHex(data, m_raw);
    } else {
        memset(m_raw.getData(), 0, 16);
    }
}

Uuid::Uuid(const CString &uuid) :
    m_raw(16)
{
    if (uuid.length() == 38 && uuid[0] == '{' && uuid[uuid.length()-1] == '}') {
        // from string format
        const Char *data = uuid.getData() + 1;
        fromHex(data, m_raw);
    } else if (uuid.length() == 36) {
        // from rfc4122
        const Char *data = uuid.getData();
        fromHex(data, m_raw);
    } else {
        memset(m_raw.getData(), 0, 16);
    }
}

Uuid::Uuid(const String &uuid) :
    m_raw(16)
{
    if (uuid.length() == 38 && uuid[0] == '{' && uuid[uuid.length()-1] == '}') {
        // from string format
        const WChar *data = uuid.getData() + 1;
        fromHex(data, m_raw);
    } else if (uuid.length() == 36) {
        // from rfc4122
        const WChar *data = uuid.getData();
        fromHex(data, m_raw);
    } else {
        memset(m_raw.getData(), 0, 16);
    }
}

Uuid::Uuid(const Uuid &dup) :
    m_raw(dup.m_raw)
{

}

Uuid &Uuid::operator=(const Uuid &dup)
{
    m_raw = dup.m_raw;
    return *this;
}

const SmartArrayUInt8 &Uuid::raw() const
{
    return m_raw;
}

CString Uuid::toCString() const
{
    Char dst[38+1];
    dst[0] = '{';
    dst[37] = '}';
    dst[38] = '\0';

    toHex(m_raw.getData(), dst+1);

    return dst;
}

String Uuid::toString() const
{
    WChar dst[38+1];
    dst[0] = '{';
    dst[37] = '}';
    dst[38] = '\0';

    toHex(m_raw.getData(), dst+1);

    return dst;
}

String Uuid::toRfc4122() const
{
    WChar dst[36+1];
    dst[36] = '\0';

    toHex(m_raw.getData(), dst);

    return dst;
}

Bool Uuid::isNull() const
{
    return memcmp(m_raw.getData(), nullUuid().m_raw.getData(), 16) == 0;
}

Bool Uuid::isValid() const
{
    return !isNull();
}

Bool Uuid::operator<(const Uuid &_which) const
{
    if (m_raw.getData() == _which.m_raw.getData()) {
        return False;
    }

    return memcmp(m_raw.getData(), _which.m_raw.getData(), 16) < 0;
}

Bool Uuid::operator<=(const Uuid &_which) const
{
    if (m_raw.getData() == _which.m_raw.getData()) {
        return True;
    }

    return memcmp(m_raw.getData(), _which.m_raw.getData(), 16) <= 0;
}

Bool Uuid::operator>(const Uuid &_which) const
{
    if (m_raw.getData() == _which.m_raw.getData()) {
        return False;
    }

    return memcmp(m_raw.getData(), _which.m_raw.getData(), 16) > 0;
}

Bool Uuid::operator>=(const Uuid &_which) const
{
    if (m_raw.getData() == _which.m_raw.getData()) {
        return True;
    }

    return memcmp(m_raw.getData(), _which.m_raw.getData(), 16) >= 0;
}

Bool Uuid::operator==(const Uuid &_which) const
{
    if (m_raw.getData() == _which.m_raw.getData()) {
        return True;
    }

    return memcmp(m_raw.getData(), _which.m_raw.getData(), 16) == 0;
}

Bool Uuid::operator!=(const Uuid &_which) const
{
    if (m_raw.getData() == _which.m_raw.getData()) {
        return False;
    }

    return memcmp(m_raw.getData(), _which.m_raw.getData(), 16) != 0;
}

UInt8 Uuid::version() const
{
    return (timeHiVersion() & 0xf000) >> 12;
}

Bool Uuid::writeToFile(OutStream &os) const
{
    return m_raw.writeToFile(os);
}

Bool Uuid::readFromFile(InStream &is)
{
    return m_raw.readFromFile(is);
}
