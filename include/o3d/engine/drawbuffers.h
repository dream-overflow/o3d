/**
 * @file drawbuffers.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DRAWBUFFERS_H
#define _O3D_DRAWBUFFERS_H

#include <vector>

#include "o3d/core/baseobject.h"
#include "o3d/core/templatearray.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class Context;

/**
 * @brief Multi render target MRT or multi draw buffers helper.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2014-04-18
 */
class O3D_API DrawBuffers : NonCopyable<>
{
public:

    /**
     * @brief The Attachment enum.
     */
    enum Attachment
    {
        COLOR_ATTACHMENT0 = 0x8CE0,
        COLOR_ATTACHMENT1 = 0x8CE1,
        COLOR_ATTACHMENT2 = 0x8CE2,
        COLOR_ATTACHMENT3 = 0x8CE3,
        COLOR_ATTACHMENT4 = 0x8CE4,
        COLOR_ATTACHMENT5 = 0x8CE5,
        COLOR_ATTACHMENT6 = 0x8CE6,
        COLOR_ATTACHMENT7 = 0x8CE7,
        COLOR_ATTACHMENT8 = 0x8CE8,
        COLOR_ATTACHMENT9 = 0x8CE9,
        COLOR_ATTACHMENT10 = 0x8CE10,
        COLOR_ATTACHMENT11 = 0x8CE11,
        COLOR_ATTACHMENT12 = 0x8CE12,
        COLOR_ATTACHMENT13 = 0x8CE13,
        COLOR_ATTACHMENT14 = 0x8CE14,
        COLOR_ATTACHMENT15 = 0x8CE15,
        DEPTH_ATTACHMENT = 0x8D00,
        STENCIL_ATTACHMENT = 0x8D20
    };

    //! Constructor.
    DrawBuffers(Context *context);

    //! Destructor.
    ~DrawBuffers();

    //! Get the gl context (read only).
    inline const Context* getContext() const { return m_context; }
    //! Get the gl context.
    inline Context* getContext() { return m_context; }

    //! Set the number of draw buffers.
    void setCount(UInt32 size) { m_drawBuffers.setSize(size); }

    //! Get the number of draw buffers.
    inline UInt32 getCount() const { return m_drawBuffers.getSize(); }

    //! Get the array of draw buffers.
    inline const ArrayUInt32& getDrawBuffers() const { return m_drawBuffers; }

    //! Set the array of draw buffers.
    inline void setDrawBuffers(const ArrayUInt32& buffers) { m_drawBuffers = buffers; }

    //! Push an attachment at the end of the array.
    inline void push(Attachment attachment) { m_drawBuffers.push(attachment); }

    /**
     * @brief setDrawBuffer Set the color attachment for a specified draw buffer index.
     * @param index Array index.
     * @param attachment Attachement enum.
     */
    inline void setDrawBuffer(UInt32 index, Attachment attachment)
    {
        m_drawBuffers[index] = attachment;
    }

    //! Set the draw buffers.
    void apply() const;

    //! Reset to a single draw buffer set to COLOR_ATTACHMENT0.
    void reset() const;

protected:

    Context *m_context;   //!< Related context.

    ArrayUInt32 m_drawBuffers;
};


} // namespace o3d

#endif // _O3D_DRAWBUFFERS_H
