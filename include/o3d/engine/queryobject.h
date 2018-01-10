/**
 * @file queryobject.h
 * @brief Base class for query object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2018-01-10
 * @copyright Copyright (c) 2001-2018 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_QUERYOBJECT_H
#define _O3D_QUERYOBJECT_H

#include "o3d/core/base.h"

namespace o3d {

class Context;

/**
 * @brief Base class for query object.
 */
class O3D_API QueryObject
{
public:

    //! Constructor.
    QueryObject(Context *context) :
        m_context(context),
        m_queryId(O3D_UNDEFINED)
    {
        O3D_ASSERT(m_context != nullptr);
    }

    /**
     * @brief Get the query object identifier.
     */
    inline UInt32 getQueryId() const { return m_queryId; }

protected:

    Context *m_context;       //!< Related context
    UInt32 m_queryId;         //!< Id of this occlusion query object
};

} // namespace o3d

#endif // _O3D_QUERYOBJECT_H
