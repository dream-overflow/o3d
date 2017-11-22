/**
 * @file sha1.cpp
 * @brief SHA1 Hash calculation.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-11-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"

#include "o3d/core/sha1.h"
#include "o3d/core/error.h"
#include "o3d/core/debug.h"
#include "o3d/core/stringutils.h"
#include "o3d/core/filemanager.h"

using namespace o3d;

#define SHA1_HASH_SIZE 20

/* This structure will hold context information for the SHA-1 hashing operation */
typedef struct SHA1Context {
    UInt32 Intermediate_Hash[SHA1_HASH_SIZE/4]; /* Message Digest */
    UInt32 Length_Low;                        /* Message length in bits */
    UInt32 Length_High;                       /* Message length in bits */
    UInt32 Message_Block_Index;               /* Index into message block array */
    UInt8 Message_Block[64];                  /* 512-bit message blocks */
    int Computed;                             /* Is the digest computed? */
    int Corrupted;                            /* Is the message digest corrupted? */
} SHA1Context;

/* Function Prototypes */
static int SHA1Reset  (SHA1Context *);
static int SHA1Input  (SHA1Context *, const UInt8 *, unsigned int);
static int SHA1Result (SHA1Context *, UInt8 Message_Digest[]);

///* Local Function Prototyptes */
static void SHA1PadMessage         (SHA1Context *);
static void SHA1ProcessMessageBlock(SHA1Context *);

SHA1Hash::SHA1Hash() :
    m_rawDigest(20),
    m_privateData(new SHA1Context)
{
    SHA1Context *ctx = reinterpret_cast<SHA1Context*>(m_privateData);
    SHA1Reset(ctx);
}

SHA1Hash::SHA1Hash(UInt8 *data, UInt32 len) :
    m_rawDigest(20),
    m_privateData(new SHA1Context)
{
    SHA1Context *ctx = reinterpret_cast<SHA1Context*>(m_privateData);
    SHA1Reset(ctx);

    if (data && len) {
        SHA1Input(ctx, data, len);
    }

    SHA1Result(ctx, m_rawDigest.getData());

    deletePtr(ctx);
    m_privateData = nullptr;
}

SHA1Hash::SHA1Hash(const SmartArrayUInt8 &array) :
    m_rawDigest(20),
    m_privateData(new SHA1Context)
{
    SHA1Context *ctx = reinterpret_cast<SHA1Context*>(m_privateData);
    SHA1Reset(ctx);

    if (array.isValid()) {
        SHA1Input(ctx, array.getData(), array.getSizeInBytes());
    }

    SHA1Result(ctx, m_rawDigest.getData());

    deletePtr(ctx);
    m_privateData = nullptr;
}

SHA1Hash::SHA1Hash(InStream &is) :
    m_rawDigest(20),
    m_privateData(new SHA1Context)
{
    SHA1Context *ctx = reinterpret_cast<SHA1Context*>(m_privateData);
    SHA1Reset(ctx);

    // stream
    UInt8 *buf = new UInt8[4096];
    Int32 size;

    while (is.getAvailable() > 0)
    {
        size = o3d::min(is.getAvailable(), 4096);

        is.read(buf, size);
        SHA1Input(ctx, buf, size);
    }

    deleteArray(buf);

    SHA1Result(ctx, m_rawDigest.getData());

    deletePtr(ctx);
    m_privateData = nullptr;
}

SHA1Hash::~SHA1Hash()
{
    if (m_privateData != nullptr) {
        delete reinterpret_cast<SHA1Context*>(m_privateData);
    }
}

void SHA1Hash::update(UInt8 *data, UInt32 len)
{
    if (m_privateData && data && len) {
        SHA1Context *ctx = reinterpret_cast<SHA1Context*>(m_privateData);
        SHA1Input(ctx, data, len);
    }
}

void SHA1Hash::update(const SmartArrayUInt8 &array)
{
    if (m_privateData && array.isValid()) {
        SHA1Context *ctx = reinterpret_cast<SHA1Context*>(m_privateData);
        SHA1Input(ctx, array.getData(), array.getSizeInBytes());
    }
}

void SHA1Hash::update(InStream &is)
{
    if (!m_privateData) {
        return;
    }

    SHA1Context *ctx = reinterpret_cast<SHA1Context*>(m_privateData);

    // stream
    UInt8 *buf = new UInt8[4096];
    Int32 size;

    while (is.getAvailable() > 0) {
        size = o3d::min(is.getAvailable(), 4096);

        is.read(buf, size);
        SHA1Input(ctx, buf, size);
    }

    deleteArray(buf);
}

void SHA1Hash::finalize()
{
    if (m_privateData) {
        SHA1Context *ctx = reinterpret_cast<SHA1Context*>(m_privateData);
        SHA1Result(ctx, m_rawDigest.getData());

        deletePtr(ctx);
        m_privateData = nullptr;
    }
}

String SHA1Hash::getHex() const
{
    return StringUtils::toHex(m_rawDigest, False);
}

/*
**  OSSP uuid - Universally Unique Identifier
**  Copyright (c) 2004-2008 Ralf S. Engelschall <rse@engelschall.com>
**  Copyright (c) 2004-2008 The OSSP Project <http://www.ossp.org/>
**
**  This file is part of OSSP uuid, a library for the generation
**  of UUIDs which can found at http://www.ossp.org/pkg/lib/uuid/
**
**  Permission to use, copy, modify, and distribute this software for
**  any purpose with or without fee is hereby granted, provided that
**  the above copyright notice and this permission notice appear in all
**  copies.
**
**  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
**  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
**  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
**  IN NO EVENT SHALL THE AUTHORS AND COPYRIGHT HOLDERS AND THEIR
**  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
**  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
**  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
**  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
**  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
**  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
**  SUCH DAMAGE.
**
**  uuid_sha1.c: SHA-1 API implementation
*/

#define SHA1_LEN_BIN 20
#define SHA1_LEN_STR 40

enum {
    shaSuccess = 0,
    shaNull,            /* null pointer parameter */
    shaStateError       /* called Input after Result */
};

#include <stdlib.h>
#include <string.h>

/*
 *  This is a RFC 3174 compliant Secure Hash Function (SHA-1) algorithm
 *  implementation. It is directly derived from the SHA-1 reference
 *  code published in RFC 3174 with just the following functionality
 *  preserving changes:
 *  - reformatted C style to conform with OSSP C style
 *  - added own OSSP style frontend API
 *  - added Autoconf based determination of sha1_uintX_t types
 */

/*
 *  This implements the Secure Hashing Algorithm 1 as defined in
 *  FIPS PUB 180-1 published April 17, 1995.
 *
 *  The SHA-1, produces a 160-bit message digest for a given data
 *  stream. It should take about 2**n steps to find a message with the
 *  same digest as a given message and 2**(n/2) to find any two messages
 *  with the same digest, when n is the digest size in bits. Therefore,
 *  this algorithm can serve as a means of providing a "fingerprint" for
 *  a message.
 *
 *  Caveats: SHA-1 is designed to work with messages less than 2^64 bits
 *  long. Although SHA-1 allows a message digest to be generated for
 *  messages of any number of bits less than 2^64, this implementation
 *  only works with messages with a length that is a multiple of the
 *  size of an 8-bit character.
 */

/* Define the SHA1 circular left shift macro */
#define SHA1CircularShift(bits,word) (((word) << (bits)) | ((word) >> (32-(bits))))

/* This function will initialize the SHA1Context in preparation for
 *  computing a new SHA1 message digest.
 */
static int SHA1Reset(SHA1Context *context)
{
    if (context == nullptr) {
        return shaNull;
    }

    context->Length_Low = 0;
    context->Length_High = 0;
    context->Message_Block_Index = 0;

    context->Intermediate_Hash[0] = 0x67452301;
    context->Intermediate_Hash[1] = 0xEFCDAB89;
    context->Intermediate_Hash[2] = 0x98BADCFE;
    context->Intermediate_Hash[3] = 0x10325476;
    context->Intermediate_Hash[4] = 0xC3D2E1F0;

    context->Computed = 0;
    context->Corrupted = 0;

    return shaSuccess;
}

/* This function will return the 160-bit message digest into the Message_Digest array provided by the caller.
 * NOTE: The first octet of hash is stored in the 0th element, the last octet of hash in the 19th element. */
static int SHA1Result(SHA1Context *context, UInt8 Message_Digest[])
{
    int i;

    if (context == nullptr || Message_Digest == nullptr) {
        return shaNull;
    }
    if (context->Corrupted) {
        return context->Corrupted;
    }

    if (!context->Computed) {
        SHA1PadMessage(context);
        for (i = 0; i < 64; i++) {
            /* message may be sensitive, clear it out */
            context->Message_Block[i] = (UInt8)0;
        }
        context->Length_Low  = 0; /* and clear length */
        context->Length_High = 0;
        context->Computed    = 1;
    }
    for (i = 0; i < SHA1_HASH_SIZE; i++) {
        Message_Digest[i] = (UInt8)(context->Intermediate_Hash[i>>2] >> (8 * (3 - (i & 0x03))));
    }

    return shaSuccess;
}

/* This function accepts an array of octets as the next portion of the message. */
static int SHA1Input(SHA1Context *context, const UInt8 *message_array, unsigned int length)
{
    if (length == 0) {
        return shaSuccess;
    }
    if (context == nullptr || message_array == nullptr) {
        return shaNull;
    }

    if (context->Computed) {
        context->Corrupted = shaStateError;
        return shaStateError;
    }
    if (context->Corrupted) {
        return context->Corrupted;
    }

    while (length-- && !context->Corrupted) {
        context->Message_Block[context->Message_Block_Index++] = (*message_array & 0xFF);
        context->Length_Low += 8;
        if (context->Length_Low == 0) {
            context->Length_High++;
            if (context->Length_High == 0) {
                context->Corrupted = 1; /* Message is too long */
            }
        }
        if (context->Message_Block_Index == 64) {
            SHA1ProcessMessageBlock(context);
        }
        message_array++;
    }

    return shaSuccess;
}

/* This function will process the next 512 bits of the message stored in the Message_Block array.
 * NOTICE: Many of the variable names in this code, especially the single character names, were used because
 * those were the names used in the publication. */
static void SHA1ProcessMessageBlock(SHA1Context *context)
{
    const UInt32 K[] = {   /* Constants defined in SHA-1   */
        0x5A827999,
        0x6ED9EBA1,
        0x8F1BBCDC,
        0xCA62C1D6
    };

    int     t;             /* Loop counter                */
    UInt32  temp;          /* Temporary word value        */
    UInt32  W[80];         /* Word sequence               */
    UInt32  A, B, C, D, E; /* Word buffers                */

    /* Initialize the first 16 words in the array W */
    for (t = 0; t < 16; t++) {
        W[t]  = (UInt32)(context->Message_Block[t * 4    ] << 24);
        W[t] |= (UInt32)(context->Message_Block[t * 4 + 1] << 16);
        W[t] |= (UInt32)(context->Message_Block[t * 4 + 2] <<  8);
        W[t] |= (UInt32)(context->Message_Block[t * 4 + 3]      );
    }

    for (t = 16; t < 80; t++) {
       W[t] = SHA1CircularShift(1, W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
    }

    A = context->Intermediate_Hash[0];
    B = context->Intermediate_Hash[1];
    C = context->Intermediate_Hash[2];
    D = context->Intermediate_Hash[3];
    E = context->Intermediate_Hash[4];

    for (t = 0; t < 20; t++) {
        temp =  SHA1CircularShift(5, A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 20; t < 40; t++) {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[1];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 40; t < 60; t++) {
        temp = SHA1CircularShift(5, A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 60; t < 80; t++) {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[3];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    context->Intermediate_Hash[0] += A;
    context->Intermediate_Hash[1] += B;
    context->Intermediate_Hash[2] += C;
    context->Intermediate_Hash[3] += D;
    context->Intermediate_Hash[4] += E;

    context->Message_Block_Index = 0;

    return;
}

/* According to the standard, the message must be padded to an even  512 bits. The first padding bit must be a '1'.
 * The last 64 bits represent the length of the original message. All bits in between should be 0.
 * This function will pad the message according to those rules by filling the Message_Block array accordingly.
 * It will also call the ProcessMessageBlock function provided appropriately. When it returns, it can be assumed that
 * the message digest has been computed. */
static void SHA1PadMessage(SHA1Context *context)
{
    /* Check to see if the current message block is too small to hold the initial padding bits and length.
     * If so, we will pad the block, process it, and then continue padding into a second block. */
    if (context->Message_Block_Index > 55) {
        context->Message_Block[context->Message_Block_Index++] = (UInt8)0x80;
        while (context->Message_Block_Index < 64) {
            context->Message_Block[context->Message_Block_Index++] = (UInt8)0;
        }

        SHA1ProcessMessageBlock(context);
        while(context->Message_Block_Index < 56) {
            context->Message_Block[context->Message_Block_Index++] = (UInt8)0;
        }
    } else {
        context->Message_Block[context->Message_Block_Index++] = (UInt8)0x80;
        while(context->Message_Block_Index < 56) {
            context->Message_Block[context->Message_Block_Index++] = (UInt8)0;
        }
    }

    /* Store the message length as the last 8 octets */
    context->Message_Block[56] = (UInt8)(context->Length_High >> 24);
    context->Message_Block[57] = (UInt8)(context->Length_High >> 16);
    context->Message_Block[58] = (UInt8)(context->Length_High >>  8);
    context->Message_Block[59] = (UInt8)(context->Length_High      );
    context->Message_Block[60] = (UInt8)(context->Length_Low  >> 24);
    context->Message_Block[61] = (UInt8)(context->Length_Low  >> 16);
    context->Message_Block[62] = (UInt8)(context->Length_Low  >>  8);
    context->Message_Block[63] = (UInt8)(context->Length_Low       );

    SHA1ProcessMessageBlock(context);
    return;
}
