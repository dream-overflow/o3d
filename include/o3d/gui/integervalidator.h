/**
 * @file integervalidator.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_INTEGERVALIDATOR_H
#define _O3D_INTEGERVALIDATOR_H

#include "validator.h"

namespace o3d {

/**
 * @brief IntegerValidator only for integer value within a range and negative
 * value or not.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-10-31
 */
class O3D_API IntegerValidator : public Validator
{
public:

    enum Flags
    {
        FLAG_EMPTY = 0,
        FLAG_NEGATIVE = 1
    };

    IntegerValidator(Flags flag = FLAG_EMPTY);

    IntegerValidator(Int64 min, Int64 max);

    virtual ~IntegerValidator();

    //! Set the minimal value (default is min int 64 if negative flag is set, else 0).
    void setMin(Int64 min);
    //! Get the minimal value.
    Int64 getMin() const { return m_min; }

    //! Set the minimal value (default is max int 64).
    void setMax(Int64 max);
    //! Get the maximal value.
    Int64 getMax() const { return m_max; }

    //! Validate the current value, and return TRUE if success.
    virtual Bool validate(const String &value) const;

    //! Get the type of the validator.
    virtual Int32 getType() const;

private:

    Int64 m_min;
    Int64 m_max;

    Flags m_flags;
};

} // namespace o3d

#endif // _O3D_INTEGERVALIDATOR_H

