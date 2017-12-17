/**
 * @file char.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CHAR_H
#define _O3D_CHAR_H

#include "string.h"
#include <wctype.h>

namespace o3d {

/**
 * @brief Wide char.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-22
 */
class O3D_API WideChar
{
public:

    WideChar() :
        m_char(0)
    {
    }

    WideChar(WChar _v) : m_char(_v)
    {
    }

    //! To string operator().
    operator String() const;

    //! To wchar operator().
    inline operator WChar() const { return m_char; }

    //! From wchar operator=
    WChar& operator= (WChar c)
    {
        m_char = c;
        return m_char;
    }

    //! Return True if the string in parameter contains only latin letters ([a..z] or [A..Z])
    static Bool isStringAlpha(const String &value);

    //! Return True if the string in parameter contains only digits ([0..9])
    static Bool isStringDigit(const String &value);

    //! Return the string of the given value.
    static String toString(WChar c);

    //! Return True if the char in parameter is an alphabet char or digit.
    static Bool isAlphaNum(WChar c)
    {
        return iswalnum(c);
    }

    //! Return True if the char in parameter is an alphabet char.
    static Bool isAlpha(WChar c)
    {
        return iswalpha(c);
    }

    //! Return True if the string in parameter is a digit char ([0..9])
    static Bool isDigit(WChar c)
    {
        return iswdigit(c);
    }

    //! Upper case.
    inline static WChar toUpper(WChar c)
    {
        return towupper(c);
    }

    //! Lower case.
    inline static WChar toLower(WChar c)
    {
        return towlower(c);
    }

    //! Get punctuations characters as a string.
    inline static const String& punctuation() { return ms_punctuation; }

    //! Get white spaces characters as a string.
    inline static const String& whiteSpaces() { return ms_whiteSpaces; }

    //! Get white spaces plus punctuations characters as a string.
    inline static const String& ponctuationsPlusWs() { return ms_wsp; }

private:

    WChar m_char;

    static String ms_punctuation;
    static String ms_whiteSpaces;
    static String ms_wsp;
};

} // namespace o3d

#endif // _O3D_CHAR_H
