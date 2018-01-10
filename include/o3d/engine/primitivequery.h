/**
 * @file primitivequery.h
 * @brief This class is an interface for a primitive query object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2018-01-10
 * @copyright Copyright (c) 2001-2018 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_PRIMITIVEQUERY_H
#define _O3D_PRIMITIVEQUERY_H

#include "queryobject.h"

namespace o3d {

class Context;

/**
 * @brief This class is an interface for a primitive query object.
 * @todo
 */
class O3D_API PrimitiveQuery : public QueryObject
{
public:

    enum PrimitiveType
    {
        PRIMITIVES_GENERATED = 0x8C87,
        TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN = 0x8C88
    };

    //! Constructor.
    PrimitiveQuery(Context *context, PrimitiveType type = PRIMITIVES_GENERATED);
    //! Destructor.
    ~PrimitiveQuery();

    PrimitiveType getPrimitiveType() const { return m_primitiveType; }

    //! Start the occlusion test
    void begin();
    //! end the occlusion test
    void end();

    //! Force to get the results
    void forceResults();

protected:

    PrimitiveType m_primitiveType;  //!< Type of primitive

    UInt32 m_primitiveCount;  //!< Generated primitives
};

} // namespace o3d

#endif // _O3D_PRIMITIVEQUERY_H
