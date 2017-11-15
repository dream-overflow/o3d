/**
 * @file char.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/char.h"

using namespace o3d;

String WideChar::ms_punctuation = "'!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~'";
String WideChar::ms_whiteSpaces = " \t\b";
String WideChar::ms_wsp = "'!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~' \t\b";

WideChar::operator String() const
{
    String r;
    r.concat(m_char);

    return r;
}

Bool WideChar::isStringAlpha(const String &value)
{
    WChar c;
    UInt32 i = 0;

    if (value.isEmpty())
        return True;

    for (; i < value.length(); ++i)
    {
        c = value.getData()[i];

        if (!(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z'))
            return False;

        //if (!iswalpha(c))
        //    return False;
    }

    return True;
}

Bool WideChar::isStringDigit(const String &value)
{
    WChar c;
    UInt32 i = 0;

    if (value.isEmpty())
        return True;

    for (; i < value.length(); ++i)
    {
        c = value.getData()[i];

        if (!iswdigit(c))
            return False;
    }

    return True;
}

String WideChar::toString(WChar c)
{
    String r;
    r.concat(c);

    return r;
}

//WChar WideChar::toUpper(WChar c)
//{
//    return towupper(c);
////    if (c >= 'a' && c <= 'z')
////        return c - ('a' - 'A');

////    return c;
//}

//WChar WideChar::toLower(WChar c)
//{
//    return towlower(c);
////    if (c >= 'A' && c <= 'Z')
////        return c + ('a' - 'A');

////    return c;
//}

