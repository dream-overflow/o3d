/**
 * @file textinstream.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEXTINSTREAM_H
#define _O3D_TEXTINSTREAM_H

#include "string.h"

namespace o3d {

/**
 * @brief TextInStream Text input stream base class
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-12-04
 */
class O3D_API TextInStream
{
public:

    //! read text line and return number of read characters.
    virtual Int32 readLine(
            String &str,
            CharacterEncoding encoding = ENCODING_UTF8) = 0;

    //! read text line and return number of read characters and stop at limit size.
    //! or delimiter character
    virtual Int32 readLine(
            String &str,
            Int32 limit,
            UInt8 delim,
            CharacterEncoding encoding = ENCODING_UTF8) = 0;

    //! read word, stop if space or EOF or \n and return number of read characters.
    virtual Int32 readWord(
            String &str,
            CharacterEncoding encoding = ENCODING_UTF8) = 0;
};

} // namespace o3d

#endif // _O3D_TEXTINSTREAM_H

