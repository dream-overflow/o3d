/**
 * @file uuid.cpp
 * @brief Uuid object
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/uuid.h"
#include "o3d/core/md5.h"
#include "o3d/core/sha1.h"
#include "o3d/core/debug.h"
#include "o3d/core/stringutils.h"
#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"
#include "o3d/core/application.h"

using namespace o3d;

static UInt64 s_lastTime = 0;
static UInt16 s_clockSeq = 0;
static SmartArrayUInt8 s_nodeId = SmartArrayUInt8();

struct uuid_t {
    UInt32  time_low;
    UInt16  time_mid;
    UInt16  time_hi_and_version;
    UInt8   clock_seq_hi_and_reserved;
    UInt8   clock_seq_low;
    UInt8   node[6];
};

/* uuid_create -- generate a UUID */
int uuid_create(uuid_t * uuid);

/* uuid_create_md5_from_name -- create a version 3 (MD5) UUID using a "name" from a "name space" */
void uuid_create_md5_from_name(uuid_t *uuid,         /* resulting UUID */
    uuid_t nsid,          /* UUID of the namespace */
    Char *name,           /* the name from which to generate a UUID */
    int namelen           /* the length of the name */
);

/* uuid_create_sha1_from_name -- create a version 5 (SHA-1) UUID using a "name" from a "name space" */
void uuid_create_sha1_from_name(
    uuid_t *uuid,         /* resulting UUID */
    uuid_t nsid,          /* UUID of the namespace */
    Char *name,           /* the name from which to generate a UUID */
    int namelen           /* the length of the name */
);

/* puid -- print a UUID */
void puid(uuid_t u);

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

Uuid Uuid::makeUuid(Version version)
{
    Uuid uuid;
version=VERSION_1;
    if (version == VERSION_1) {
        uuid_t u;
        uuid_create(&u);

//        *reinterpret_cast<UInt32*>(&uuid.m_raw.getData()[0]) = u.time_low;
//        *reinterpret_cast<UInt16*>(&uuid.m_raw.getData()[4]) = u.time_mid;
//        *reinterpret_cast<UInt16*>(&uuid.m_raw.getData()[6]) = u.time_hi_and_version;
//        uuid.m_raw.getData()[8] = u.clock_seq_hi_and_reserved;
//        uuid.m_raw.getData()[9] = u.clock_seq_low;
//        memcpy(&uuid.m_raw.getData()[10], &u.node, 6);

        memcpy(uuid.m_raw.getData(), &u, 16);
    } else if (version == VERSION_3) {
        uuid_t nsid;
        uuid_create(&nsid);

        uuid_t u;
        puid(nsid);

        CString ns = Application::getAppName().toUtf8();
        uuid_create_md5_from_name(&u, nsid, ns.getData(), ns.length());

        memcpy(uuid.m_raw.getData(), &u, 16);
    } else if (version == VERSION_5) {
        uuid_t nsid;
        uuid_create(&nsid);

        uuid_t u;

        CString ns = Application::getAppName().toUtf8();
        uuid_create_sha1_from_name(&u, nsid, ns.getData(), ns.length());

        memcpy(uuid.m_raw.getData(), &u, 16);
    }

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

    // a simple memcpy does the same a
//    if (timeLow() < _which.timeLow()) {
//        return True;
//    }

//    if (timeMid() < _which.timeMid()) {
//        return True;
//    }

//    if (timeHiVersion() < _which.timeHiVersion()) {
//        return True;
//    }

//    if (clockSeqHiVariant() < _which.clockSeqHiVariant()) {
//        return True;
//    }

//    if (clockSeqLow() < _which.clockSeqLow()) {
//        return True;
//    }

//    memcmp(node(), _which.node(), 6) < 0;

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

//
// Internal generation
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef O3D_WINDOWS
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#endif

/* set the following to the number of 100ns ticks of the actual resolution of your system's clock */
#define UUIDS_PER_TICK 1024

typedef struct UuidNode {
    UInt8 nodeID[6];
} uuid_node_t;

void get_ieee_node_identifier(uuid_node_t *node);
void get_system_time(UInt64 *uuid_time);
SmartArrayUInt8 get_random_info();

/* system dependent call to get IEEE node ID. This sample implementation generates a random node ID. */
void get_ieee_node_identifier(uuid_node_t *node)
{
    SmartArrayUInt8 seed;

    if (s_nodeId.isEmpty()) {
        seed = get_random_info();
        seed.getData()[0] |= 0x01;

        s_nodeId = seed;
    }

    memcpy(node, s_nodeId.getData(), 6);
}

/* system dependent call to get the current system time.
 * Returned as 100ns ticks since UUID epoch, but resolution may be less than 100ns. */
void get_system_time(UInt64 *uuid_time)
{
#ifdef O3D_WINDOWS
    ULARGE_INTEGER time;

    // NT keeps time in FILETIME format which is 100ns ticks since Jan 1, 1601. UUIDs use time in 100ns ticks since Oct 15, 1582.
    // The difference is 17 Days in Oct + 30 (Nov) + 31 (Dec) + 18 years and 5 leap days.
    GetSystemTimeAsFileTime((FILETIME *)&time);
    time.QuadPart +=
          (UInt64) (1000*1000*10)       // seconds
        * (UInt64) (60 * 60 * 24)       // days
        * (UInt64) (17+30+31+365*18+5); // # of days
    *uuid_time = time.QuadPart;
#else
    struct timeval tp;

    gettimeofday(&tp, (struct timezone *)0);

    // Offset between UUID formatted times and Unix formatted times.
    // - UUID UTC base time is October 15, 1582
    // - Unix base time is January 1, 1970
    *uuid_time = ((UInt64)tp.tv_sec * 10000000) + ((UInt64)tp.tv_usec * 10) + UInt64(0x01B21DD213814000);
#endif
}

/* @todo Improve @see RFC 1750 */
SmartArrayUInt8 get_random_info()
{
    MD5Hash md5;

#ifdef O3D_WINDOWS
    struct R {
        MEMORYSTATUS m;
        SYSTEM_INFO s;
        FILETIME t;
        LARGE_INTEGER pc;
        DWORD tc;
        DWORD l;
        Char hostname[MAX_COMPUTERNAME_LENGTH + 1];
    } r;

    GlobalMemoryStatus(&r.m);
    GetSystemInfo(&r.s);
    GetSystemTimeAsFileTime(&r.t);
    QueryPerformanceCounter(&r.pc);
    r.tc = GetTickCount();

    r.l = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerName(r.hostname, &r.l);

    md5.update((const UInt8*)&r, sizeof(R));
#else
    struct R {
        struct sysinfo s;
        struct timeval t;
        char hostname[257];
    } r;

    sysinfo(&r.s);
    gettimeofday(&r.t, (struct timezone *)0);
    gethostname(r.hostname, 256);

    md5.update((const UInt8*)&r, sizeof(R));
#endif

    md5.finalize();

    return md5.getRaw();
}

/* various forward declarations */
static UInt16 true_random(void);
static void get_current_time(UInt64 *timestamp);

/* read_state -- read UUID generator state from non-volatile store */
static int read_state(UInt16 *clockseq, UInt64 *timestamp, uuid_node_t *node);
static void write_state(UInt16 clockseq, UInt64 timestamp, uuid_node_t node);

static void format_uuid_v1(uuid_t *uuid, UInt16 clockseq, UInt64 timestamp, uuid_node_t node);
static void format_uuid_v3or5(uuid_t *uuid, const UInt8 hash[], int v);

/* uuid_create -- generator a UUID */
int uuid_create(uuid_t *uuid)
{
     UInt64 timestamp, last_time;
     UInt16 clockseq;
     uuid_node_t node;
     uuid_node_t last_node;
     int f;

     /* get time, node ID, saved state from non-volatile storage */
     get_current_time(&timestamp);
     get_ieee_node_identifier(&node);
     //f = read_state(&clockseq, &last_time, &last_node);

     /* if no NV state, or if clock went backwards, or node ID changed (e.g., new network card) change clockseq */
     if (!f || memcmp(&node, &last_node, sizeof node)) {
         clockseq = true_random();
     } else if (timestamp < last_time) {
         clockseq++;
     }

     /* save the state for next time */
     write_state(clockseq, timestamp, node);

     /* stuff fields into the UUID */
     format_uuid_v1(uuid, clockseq, timestamp, node);
     return 1;
}

/* format_uuid_v1 -- make a UUID from the timestamp, clockseq, and node ID */
void format_uuid_v1(uuid_t* uuid, UInt16 clock_seq, UInt64 timestamp, uuid_node_t node)
{
    printf("%u %u %u\n", clock_seq, timestamp, clock_seq);

    /* Construct a version 1 uuid with the information we've gathered plus a few constants. */
    uuid->time_low = (unsigned long)(timestamp & 0xFFFFFFFF);
    uuid->time_mid = (unsigned short)((timestamp >> 32) & 0xFFFF);
    uuid->time_hi_and_version = (unsigned short)((timestamp >> 48) & 0x0FFF);
    uuid->time_hi_and_version |= (1 << 12);
    uuid->clock_seq_low = clock_seq & 0xFF;
    uuid->clock_seq_hi_and_reserved = (clock_seq & 0x3F00) >> 8;
    uuid->clock_seq_hi_and_reserved |= 0x80;
    memcpy(&uuid->node, &node, sizeof uuid->node);
}

/* get-current_time -- get time as 60-bit 100ns ticks since UUID epoch.
 * Compensate for the fact that real clock resolution is less than 100ns. */
void get_current_time(UInt64 *timestamp)
{
    static int inited = 0;
    static UInt64 time_last = 0;
    static UInt16 uuids_this_tick;
    UInt64 time_now;

    if (!inited) {
        get_system_time(&time_now);
        uuids_this_tick = UUIDS_PER_TICK;
        inited = 1;
        // time_last = time_now;
    }

    for ( ; ; ) {
        get_system_time(&time_now);

        /* if clock reading changed since last UUID generated, */
        if (time_last != time_now) {
            /* reset count of uuids gen'd with this clock reading */
            uuids_this_tick = 0;
            time_last = time_now;
            break;
        }
        if (uuids_this_tick < UUIDS_PER_TICK) {
            uuids_this_tick++;
            break;
        }

        /* going too fast for our clock; spin */
    }
    /* add the count of uuids to low order bits of the clock reading */
    *timestamp = time_now + uuids_this_tick;
}

/* true_random -- generate a crypto-quality random number. */
static UInt16 true_random(void)
{
    // @todo better random generator (@see stl)
    static int inited = 0;
    UInt64 time_now;

    if (!inited) {
        get_system_time(&time_now);
        time_now = time_now / UUIDS_PER_TICK;
        srand((unsigned int)(((time_now >> 32) ^ time_now) & 0xffffffff));
        inited = 1;
    }

    return rand();
}

/* read_state -- read UUID generator state from non-volatile store */
int read_state(UInt16 *clockseq, UInt64 *timestamp, uuid_node_t *node)
{
//    static int inited = 0;

//    if (!inited) {
//        inited = 1;
//        return 0;
//    }

    *clockseq = s_clockSeq;
    *timestamp = s_lastTime;
    memcpy(node, s_nodeId.getData(), sizeof(UuidNode));

    return 1;
}

/* write_state -- save UUID generator state back to non-volatile storage */
void write_state(UInt16 clockseq, UInt64 timestamp, uuid_node_t node)
{
    s_clockSeq = clockseq;
    s_lastTime = timestamp;
    memcpy(s_nodeId.getData(), &node, sizeof(UuidNode));
}

/* uuid_create_md5_from_name -- create a version 3 (MD5) UUID using a "name" from a "name space" */
void uuid_create_md5_from_name(uuid_t *uuid, uuid_t nsid, Char *name, int namelen)
{
    MD5Hash md5;
    uuid_t net_nsid;

    /* put name space ID in network byte order so it hashes the same no matter what endian machine we're on */
    net_nsid = nsid;

    #ifdef O3D_BIG_ENDIAN
    // network endianess is big endian, so nothing to do
    #else
    System::swapBytes4(&net_nsid.time_low);
    System::swapBytes2(&net_nsid.time_mid);
    System::swapBytes2(&net_nsid.time_hi_and_version);
    #endif

    md5.update((UInt8*)&net_nsid, sizeof net_nsid);
    md5.update((UInt8*)name, namelen);
    md5.finalize();

    /* the hash is in network byte order at this point */
    format_uuid_v3or5(uuid, md5.getRaw().getData(), 3);
}

void uuid_create_sha1_from_name(uuid_t *uuid, uuid_t nsid, Char *name, int namelen)
{
    SHA1Hash sha1;
    uuid_t net_nsid;

    /* put name space ID in network byte order so it hashes the same no matter what endian machine we're on */
    net_nsid = nsid;

    #ifdef O3D_BIG_ENDIAN
    // network endianess is big endian, so nothing to do
    #else
    System::swapBytes4(&net_nsid.time_low);
    System::swapBytes2(&net_nsid.time_mid);
    System::swapBytes2(&net_nsid.time_hi_and_version);
    #endif

    sha1.update((UInt8*)&net_nsid, sizeof net_nsid);
    sha1.update((UInt8*)name, namelen);
    sha1.finalize();

    /* the hash is in network byte order at this point */
    format_uuid_v3or5(uuid, sha1.getRaw().getData(), 5);
}

/* puid -- print a UUID */
void puid(uuid_t u)
{
    int i;

    printf("%8.8x-%4.4x-%4.4x-%2.2x%2.2x-", u.time_low, u.time_mid,
    u.time_hi_and_version, u.clock_seq_hi_and_reserved,
    u.clock_seq_low);
    for (i = 0; i < 6; i++) {
        printf("%2.2x", u.node[i]);
    }

    printf("\n");
}

/* format_uuid_v3or5 -- make a UUID from a (pseudo)random 128-bit number */
void format_uuid_v3or5(uuid_t *uuid, const UInt8 hash[16], int v)
{
    memcpy(uuid, hash, sizeof *uuid);

    /* convert UUID to local byte order */
    #ifdef O3D_BIG_ENDIAN
    // network endianess is big endian, so nothing to do
    #else
    System::swapBytes4(&uuid->time_low);
    System::swapBytes2(&uuid->time_mid);
    System::swapBytes2(&uuid->time_hi_and_version);
    #endif

    /* put in the variant and version bits */
    uuid->time_hi_and_version &= 0x0FFF;
    uuid->time_hi_and_version |= (v << 12);
    uuid->clock_seq_hi_and_reserved &= 0x3F;
    uuid->clock_seq_hi_and_reserved |= 0x80;
}

void Uuid::init()
{
    s_nodeId = get_random_info();
    s_nodeId.getData()[0] |= 0x01;
    s_clockSeq = true_random();

    get_current_time(&s_lastTime);
}

void Uuid::quit()
{
    // release
    s_nodeId = SmartArrayUInt8();
}


void Uuid::setIEEENodeId(const SmartArrayUInt8 &lnodeId)
{
    MD5Hash md5;

    md5.update(lnodeId);
    md5.finalize();

    s_nodeId = md5.getRaw();
    s_nodeId.getData()[0] |= 0x01;
}

void Uuid::setTime(UInt64 time)
{
    s_lastTime = time;
}
