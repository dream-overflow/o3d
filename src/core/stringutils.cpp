/**
 * @file stringutils.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/stringutils.h"

using namespace o3d;

const UInt8 StringUtils::HEX_DATA_LC[] = "0123456789abcdef";
const UInt8 StringUtils::HEX_DATA_UC[] = "0123456789ABCDEF";

CString StringUtils::toHex(const UInt8* data, UInt32 length, Bool upperCase)
{
    const UInt8 *hex_array = HEX_DATA_UC;
    if (!upperCase) {
        hex_array = HEX_DATA_LC;
    }

    CString hex(length<<1);

    for (UInt32 i = 0; i < length; ++i) {
        UInt32 v = data[i] & 0xff;
        hex[i<<1] = hex_array[v >> 4];
        hex[(i<<1)+1] = hex_array[v & 0x0f];
    }

    return hex;
}

String StringUtils::toHex(const ArrayUInt8 &arr, Bool upperCase)
{
    return String(toHex(arr.getData(), arr.getSize(), upperCase));
}

String StringUtils::toHex(const SmartArrayUInt8 &arr, Bool upperCase)
{
    return String(toHex(arr.getData(), arr.getSize(), upperCase));
}
