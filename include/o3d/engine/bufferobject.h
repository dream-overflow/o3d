/**
 * @file bufferobject.h
 * @brief Buffer object base class.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2018-01-10
 * @copyright Copyright (c) 2001-2018 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_BUFFEROBJECT_H
#define _O3D_BUFFEROBJECT_H

#include "o3d/core/debug.h"

namespace o3d {

class Context;

/**
 * @brief The BufferObject base class.
 */
class O3D_API BufferObject
{
public:

    static const UInt32 BUFFER_SIZE = 0x8764;

    /**
     * @brief Buffer storage type.
     */
    enum Storage
    {
        STATIC = 0x88E4,    //!< If the data changes rarely.
        DYNAMIC = 0x88E8,   //!< If the data changes up to once per frame.
        STREAMED = 0x88E0   //!< If the data changes up to once per draw.
    };

    /**
     * @brief Lock/map flags.
     */
    enum LockFlags
    {
        MAP_READ = 0x0001,
        MAP_WRITE = 0x0002,
        MAP_INVALIDATE_RANGE = 0x0004,
        MAP_INVALIDATE_BUFFER = 0x0008,
        MAP_FLUSH_EXPLICIT = 0x0010,
        MAP_UNSYNCHRONIZED = 0x0020,
        MAP_PERSISTENT = 0x0040,
        MAP_COHERENT = 0x0080
    };

    friend constexpr LockFlags operator|(LockFlags a, LockFlags b)
    {
        return static_cast<LockFlags>(static_cast<int>(a) | static_cast<int>(b));
    }

    friend inline LockFlags& operator|=(LockFlags &a, LockFlags b) {
        a = a | b;
        return a;
    }

    BufferObject(Context *context) :
        m_context(context),
        m_bufferId(O3D_UNDEFINED)
    {
        O3D_ASSERT(m_context != nullptr);
    }

    //! Get the gl context (read only).
    inline const Context* getContext() const { return m_context; }
    //! Get the gl context.
    inline Context* getContext() { return m_context; }

    //! Return the OpenGL buffer identifier.
    inline UInt32 getBufferId() const { return m_bufferId; }

    //! Return True if the buffer exists.
    inline Bool exists() const { return (m_bufferId != O3D_UNDEFINED); }

protected:

    Context *m_context;
    UInt32 m_bufferId;
};

} // namespace o3d

#endif // _O3D_BUFFEROBJECT_H
