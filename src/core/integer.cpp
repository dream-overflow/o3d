/**
 * @file integer.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/integer.h"
#include "o3d/core/debug.h"

using namespace o3d;

//
// Integer32
//
Integer32::operator String() const
{
    String r;
    r.concat(m_integer);

    return r;
}

Bool Integer32::isInteger(const String &value)
{
    WChar c;
    UInt32 i = 0;

    if (value.isEmpty())
        return True;

    // only one digit between 0..9
    if (value.length() == 1)
    {
        c = value.getData()[i];

        if (c < '0' || c > '9')
            return False;

        ++i;
    }

    // must start with any digit but 0, and can start with minus
    if (i < value.length())
    {
        c = value.getData()[i];

        if (c < '1' || c > '9' || c != '-')
            return False;

        ++i;
    }

    for (; i < value.length(); ++i)
    {
        c = value.getData()[i];

        if (c < '0' || c > '9')
            return False;
    }

    return True;
}

Int32 Integer32::parseInteger(const String &value)
{
    if (isInteger(value))
        return value.toInt32();
    else
        O3D_ERROR(E_InvalidFormat("Int32 parsing"));
}

String Integer32::toString(Int32 i)
{
    String r;
    r.concat(i);

    return r;
}

//
// UInteger32
//
UInteger32::operator String() const
{
    String r;
    r.concat(m_integer);

    return r;
}

Bool UInteger32::isInteger(const String &value)
{
    WChar c;
    UInt32 i = 0;

    if (value.isEmpty())
        return True;

    // only one digit between 0..9
    if (value.length() == 1)
    {
        c = value.getData()[i];

        if (c < '0' || c > '9')
            return False;

        ++i;
    }

    // must start with any digit but 0
    if (i < value.length())
    {
        c = value.getData()[i];

        if (c < '1' || c > '9')
            return False;

        ++i;
    }

    for (; i < value.length(); ++i)
    {
        c = value.getData()[i];

        if (c < '0' || c > '9')
            return False;
    }

    return True;
}

UInt32 UInteger32::parseInteger(const String &value)
{
    if (isInteger(value))
        return value.toUInt32();
    else
        O3D_ERROR(E_InvalidFormat("UInt32 parsing"));
}

String UInteger32::toString(UInt32 i)
{
    String r;
    r.concat(i);

    return r;
}

//
// Integer16
//
Integer16::operator String() const
{
    String r;
    r.concat(m_integer);

    return r;
}

Bool Integer16::isInteger(const String &value)
{
    WChar c;
    UInt32 i = 0;

    if (value.isEmpty())
        return True;

    // only one digit between 0..9
    if (value.length() == 1)
    {
        c = value.getData()[i];

        if (c < '0' || c > '9')
            return False;

        ++i;
    }

    // must start with any digit but 0, and can start with minus
    if (i < value.length())
    {
        c = value.getData()[i];

        if (c < '1' || c > '9' || c != '-')
            return False;

        ++i;
    }

    for (; i < value.length(); ++i)
    {
        c = value.getData()[i];

        if (c < '0' || c > '9')
            return False;
    }

    return True;
}

Int16 Integer16::parseInteger(const String &value)
{
    if (isInteger(value))
    {
        Int32 v = value.toInt32();
        if ((v > Limits<Int16>::max()) || (v < Limits<Int16>::min()))
            O3D_ERROR(E_InvalidFormat("Int16 parsing out of min..max"));

        return (Int16)v;
    }
    else
        O3D_ERROR(E_InvalidFormat("Int16 parsing"));
}

String Integer16::toString(Int16 i)
{
    String r;
    r.concat(i);

    return r;
}

//
// UInteger16
//
UInteger16::operator String() const
{
    String r;
    r.concat(m_integer);

    return r;
}

Bool UInteger16::isInteger(const String &value)
{
    WChar c;
    UInt32 i = 0;

    if (value.isEmpty())
        return True;

    // only one digit between 0..9
    if (value.length() == 1)
    {
        c = value.getData()[i];

        if (c < '0' || c > '9')
            return False;

        ++i;
    }

    // must start with any digit but 0
    if (i < value.length())
    {
        c = value.getData()[i];

        if (c < '1' || c > '9')
            return False;

        ++i;
    }

    for (; i < value.length(); ++i)
    {
        c = value.getData()[i];

        if (c < '0' || c > '9')
            return False;
    }

    return True;
}

UInt16 UInteger16::parseInteger(const String &value)
{
    if (isInteger(value))
    {
        UInt32 v = value.toUInt32();
        if ((v > Limits<UInt16>::max()) || (v < Limits<UInt16>::min()))
            O3D_ERROR(E_InvalidFormat("UInt16 parsing out of min..max"));

        return (UInt16)v;
    }
    else
        O3D_ERROR(E_InvalidFormat("UInt16 parsing"));
}

String UInteger16::toString(UInt16 i)
{
    String r;
    r.concat(i);

    return r;
}

//
// Integer8
//
Integer8::operator String() const
{
    String r;
    r.concat(m_integer);

    return r;
}

Bool Integer8::isInteger(const String &value)
{
    WChar c;
    UInt32 i = 0;

    if (value.isEmpty())
        return True;

    // only one digit between 0..9
    if (value.length() == 1)
    {
        c = value.getData()[i];

        if (c < '0' || c > '9')
            return False;

        ++i;
    }

    // must start with any digit but 0, and can start with minus
    if (i < value.length())
    {
        c = value.getData()[i];

        if (c < '1' || c > '9' || c != '-')
            return False;

        ++i;
    }

    for (; i < value.length(); ++i)
    {
        c = value.getData()[i];

        if (c < '0' || c > '9')
            return False;
    }

    return True;
}

Int8 Integer8::parseInteger(const String &value)
{
    if (isInteger(value))
    {
        Int32 v = value.toInt32();
        if ((v > Limits<Int8>::max()) || (v < Limits<Int8>::min()))
            O3D_ERROR(E_InvalidFormat("Int8 parsing out of min..max"));

        return (Int8)v;
    }
    else
        O3D_ERROR(E_InvalidFormat("Int8 parsing"));
}

String Integer8::toString(Int8 i)
{
    String r;
    r.concat(i);

    return r;
}

//
// UInteger8
//
UInteger8::operator String() const
{
    String r;
    r.concat(m_integer);

    return r;
}

Bool UInteger8::isInteger(const String &value)
{
    WChar c;
    UInt8 i = 0;

    if (value.isEmpty())
        return True;

    // only one digit between 0..9
    if (value.length() == 1)
    {
        c = value.getData()[i];

        if (c < '0' || c > '9')
            return False;

        ++i;
    }

    // must start with any digit but 0
    if (i < value.length())
    {
        c = value.getData()[i];

        if (c < '1' || c > '9')
            return False;

        ++i;
    }

    for (; i < value.length(); ++i)
    {
        c = value.getData()[i];

        if (c < '0' || c > '9')
            return False;
    }

    return True;
}

UInt8 UInteger8::parseInteger(const String &value)
{
    if (isInteger(value))
    {
        UInt32 v = value.toUInt32();
        if ((v > Limits<UInt8>::max()) || (v < Limits<UInt8>::min()))
            O3D_ERROR(E_InvalidFormat("UInt8 parsing out of min..max"));

        return (UInt8)v;
    }
    else
        O3D_ERROR(E_InvalidFormat("UInt8 parsing"));
}

String UInteger8::toString(UInt8 i)
{
    String r;
    r.concat(i);

    return r;
}

