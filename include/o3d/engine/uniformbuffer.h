/**
 * @file uniformbuffer.h
 * @brief Uniform buffer object management.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2018-01-01
 * @copyright Copyright (c) 2001-2018 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_UNIFORMBUFFER_H
#define _O3D_UNIFORMBUFFER_H

#include "bufferobject.h"
#include "enginetype.h"

namespace o3d {

class Shadable;

/**
 * @brief Uniform buffer object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2018-01-01
 */
class O3D_API UniformBuffer : public BufferObject
{
public:

    /**
     * @brief Uniform buffer object.
     * @param context
     */
    UniformBuffer(Context *context);

    ~UniformBuffer();

    /**
     * @brief Create with the specified size in bytes.
     * @param data Data pointer to the structure of uniform values.
     * @param size Size in bytes.
     * @param dontUnbint Keep bound is True, else unbind.
     */
    void create(UInt8 *data, UInt32 size, Bool dontUnbind = False);

    void bind();
    void unbind();

    /**
     * @brief Destroy the uniform buffer object.
     */
    void release();

    /**
     * @brief Update the contant of the UBO with data.
     * @param data Data of the same size and format than at creation.
     * @note Must be bound, or then it is automatically bound.
     */
    void update(UInt8* data);

    //! Return the OpenGL buffer identifier.
    inline UInt32 getBufferId() const { return m_bufferId; }

private:

    Context *m_context;
    UInt32 m_bufferId;

    UInt32 m_size;
};

} // namespace o3d

#endif // _O3D_UNIFORMBUFFER_H
