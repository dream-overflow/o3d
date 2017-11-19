/**
 * @file stringutils.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_STRING_UTILS_H
#define _O3D_STRING_UTILS_H

#include "o3d/core/string.h"
#include "o3d/core/smartarray.h"
#include "o3d/core/templatearray.h"

namespace o3d {

/**
 * @brief String and CString useful functions.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2015-12-22
 */
class O3D_API StringUtils
{
public:

    //! Convert a byte ptr array with its length to its hexadecimal cstring.
    //! @param data Input byte array
    //! @param length Length of the data array in bytes
    //! @param upperCase True means upper-case letters (default True).
    static CString toHex(const UInt8* data, UInt32 length, Bool upperCase=True);

    //! Convert a byte array to its hexadecimal string.
    //! @param arr Input byte array
    //! @param upperCase True means upper-case letters (default True).
    static String toHex(const SmartArrayUInt8 &arr, Bool upperCase=True);

    //! Convert a byte array to its hexadecimal string.
    //! @param arr Input byte array
    //! @param upperCase True means upper-case letters (default True).
    static String toHex(const ArrayUInt8 &arr, Bool upperCase=True);

private:

    static const UInt8 HEX_DATA_LC[];
    static const UInt8 HEX_DATA_UC[];
};

} // namespace o3d

#endif // _O3D_STRING_UTILS_H
