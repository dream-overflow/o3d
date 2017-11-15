/**
 * @file integer.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_INTEGER_H
#define _O3D_INTEGER_H

#include "string.h"

namespace o3d {

/**
 * @brief Signed integer 32 container and tools.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-22
 */
class O3D_API Integer32
{
public:

    Integer32() :
        m_integer(0)
    {
    }

    Integer32(Int32 _v) : m_integer(_v)
    {
    }

    //! To string operator().
    operator String() const;

    //! To int32 operator().
    inline operator Int32() const { return m_integer; }

    //! From int32 operator=
    Int32& operator= (Int32 i)
    {
        m_integer = i;
        return m_integer;
    }

    //! Return TRUE if the string in parameter is a signed 32 bits integer.
    static Bool isInteger(const String &value);

    //! Return the int32 if the string can be converted.
    static Int32 parseInteger(const String &value);

    //! Return the string of the given value.
    static String toString(Int32 i);

private:

    Int32 m_integer;
};

/**
 * @brief Unsigned integer 32 container and tools.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-22
 */
class O3D_API UInteger32
{
public:

    UInteger32() :
        m_integer(0)
    {
    }

    UInteger32(UInt32 _v) : m_integer(_v)
    {
    }

    //! To string operator().
    operator String() const;

    //! To uint32 operator().
    inline operator UInt32() const { return m_integer; }

    //! From uint32 operator=
    UInt32& operator= (UInt32 i)
    {
        m_integer = i;
        return m_integer;
    }

    //! Return TRUE if the string in parameter is an unsigned 32 bits integer.
    static Bool isInteger(const String &value);

    //! Return the uint32 if the string can be converted.
    static UInt32 parseInteger(const String &value);

    //! Return the string of the given value.
    static String toString(UInt32 i);

private:

    UInt32 m_integer;
};

/**
 * @brief Signed integer 16 container and tools.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-29
 */
class O3D_API Integer16
{
public:

    Integer16() :
        m_integer(0)
    {
    }

    Integer16(Int16 _v) : m_integer(_v)
    {
    }

    //! To string operator().
    operator String() const;

    //! To int16 operator().
    inline operator Int16() const { return m_integer; }

    //! From int16 operator=
    Int16& operator= (Int16 i)
    {
        m_integer = i;
        return m_integer;
    }

    //! Return TRUE if the string in parameter is a signed 16 bits integer.
    static Bool isInteger(const String &value);

    //! Return the int16 if the string can be converted.
    static Int16 parseInteger(const String &value);

    //! Return the string of the given value.
    static String toString(Int16 i);

private:

    Int16 m_integer;
};

/**
 * @brief Unsigned integer 16 container and tools.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-29
 */
class O3D_API UInteger16
{
public:

    UInteger16() :
        m_integer(0)
    {
    }

    UInteger16(UInt16 _v) : m_integer(_v)
    {
    }

    //! To string operator().
    operator String() const;

    //! To uint16 operator().
    inline operator UInt16() const { return m_integer; }

    //! From uint16 operator=
    UInt16& operator= (UInt16 i)
    {
        m_integer = i;
        return m_integer;
    }

    //! Return TRUE if the string in parameter is an unsigned 16 bits integer.
    static Bool isInteger(const String &value);

    //! Return the uint16 if the string can be converted.
    static UInt16 parseInteger(const String &value);

    //! Return the string of the given value.
    static String toString(UInt16 i);

private:

    UInt16 m_integer;
};

/**
 * @brief Signed integer 8 container and tools.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-29
 */
class O3D_API Integer8
{
public:

    Integer8() :
        m_integer(0)
    {
    }

    Integer8(Int8 _v) : m_integer(_v)
    {
    }

    //! To string operator().
    operator String() const;

    //! To int8 operator().
    inline operator Int8() const { return m_integer; }

    //! From int8 operator=
    Int8& operator= (Int8 i)
    {
        m_integer = i;
        return m_integer;
    }

    //! Return TRUE if the string in parameter is a signed 8 bits integer.
    static Bool isInteger(const String &value);

    //! Return the int8 if the string can be converted.
    static Int8 parseInteger(const String &value);

    //! Return the string of the given value.
    static String toString(Int8 i);

private:

    Int8 m_integer;
};

/**
 * @brief Unsigned integer 8 container and tools.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-29
 */
class O3D_API UInteger8
{
public:

    UInteger8() :
        m_integer(0)
    {
    }

    UInteger8(UInt8 _v) : m_integer(_v)
    {
    }

    //! To string operator().
    operator String() const;

    //! To uint8 operator().
    inline operator UInt8() const { return m_integer; }

    //! From uint8 operator=
    UInt8& operator= (UInt8 i)
    {
        m_integer = i;
        return m_integer;
    }

    //! Return TRUE if the string in parameter is an unsigned 8 bits integer.
    static Bool isInteger(const String &value);

    //! Return the uint8 if the string can be converted.
    static UInt8 parseInteger(const String &value);

    //! Return the string of the given value.
    static String toString(UInt8 i);

private:

    UInt8 m_integer;
};

} // namespace o3d

#endif // _O3D_INTEGER_H

