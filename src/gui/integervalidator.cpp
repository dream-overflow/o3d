/**
 * @file integervalidator.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/integervalidator.h"

using namespace o3d;

Validator::~Validator()
{

}

IntegerValidator::IntegerValidator(Flags flag) :
    m_min(0),
    m_max(o3d::Limits<Int64>::max()),
    m_flags(flag)
{
    if ((m_flags & FLAG_NEGATIVE) != 0) {
        m_min = o3d::Limits<Int64>::min();
    }
}

IntegerValidator::IntegerValidator(Int64 min, Int64 max) :
    m_min(min),
    m_max(max),
    m_flags(FLAG_EMPTY)
{
    if (m_min < 0) {
        m_flags = FLAG_NEGATIVE;
    }
}

void IntegerValidator::setMin(Int64 min)
{
    if (((m_flags & FLAG_NEGATIVE) == 0) && min < 0) {
        m_min = 0;
        return;
    }

    m_min = min;
}

void IntegerValidator::setMax(Int64 max)
{
    m_max = max;
}

IntegerValidator::~IntegerValidator()
{

}

Bool IntegerValidator::validate(const String &value) const
{
    WChar c;

    Int32 i = 0;

    // only one digit between 0..9
    if (value.length() == 1) {
        c = value.getData()[i];

        if (c < '0' || c > '9') {
            return False;
        }

        ++i;
    }

    // must start with any digit but 0, and can start with minus
    if ((i < value.length()) && ((m_flags & FLAG_NEGATIVE) != 0)) {
        c = value.getData()[i];

        if (c < '1' || c > '9' || c != '-') {
            return False;
        }

        ++i;
    } else if (i < value.length()) {
        // must start with any digit but 0
        c = value.getData()[i];

        if (c < '1' || c > '9') {
            return False;
        }

        ++i;
    }

    for (; i < value.length(); ++i) {
        c = value.getData()[i];

        if (c < '0' || c > '9') {
            return False;
        }
    }

    if (value.isEmpty()) {
        return True;
    }

    if ((m_flags & FLAG_NEGATIVE) != 0) {
        Int64 res = value.toInt64();
        if (res < m_min || res > m_max) {
            return False;
        }
    } else {
        UInt64 res = value.toUInt64();
        if ((Int64)res < m_min || (Int64)res > m_max) {
            return False;
        }
    }

    return True;
}

Int32 IntegerValidator::getType() const
{
    return TYPE_INTEGER;
}
