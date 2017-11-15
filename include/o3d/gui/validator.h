/**
 * @file validator.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_VALIDATOR_H
#define _O3D_VALIDATOR_H

#include "o3d/core/string.h"

namespace o3d {

/**
 * @brief Validator for a raw usage or with a widget.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-10-31
 */
class O3D_API Validator
{
public:

    enum Type
    {
        TYPE_UNDEFINED = 0,
        TYPE_INTEGER,
        TYPE_DECIMAL,
        TYPE_DOUBLE
    };

    virtual ~Validator() = 0;

    //! Validate the current value, and return TRUE if success.
    virtual Bool validate(const String &value) const = 0;

    //! Get the type of the validator.
    virtual Int32 getType() const = 0;
};

} // namespace o3d

#endif // _O3D_VALIDATOR_H

