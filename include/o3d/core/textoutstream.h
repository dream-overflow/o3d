/**
 * @file textoutstream.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEXTOUTSTREAM_H
#define _O3D_TEXTOUTSTREAM_H

#include "string.h"

namespace o3d {

/**
 * @brief TextOutStream Text output stream base class.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-12-04
 */
class O3D_API TextOutStream
{
public:

    //! write text line and return number of wrote characters.
    virtual Int32 writeLine(
            const String &str,
            CharacterEncoding encoding = ENCODING_UTF8) = 0;

    //! write a string into the current line and return number of wrote characters.
    virtual Int32 writeString(
            const String &str,
            CharacterEncoding encoding = ENCODING_UTF8) = 0;
};

} // namespace o3d

#endif // _O3D_TEXTOUTSTREAM_H

