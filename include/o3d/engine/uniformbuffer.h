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

#include "o3d/core/debug.h"
#include "o3d/core/memorydbg.h"

#include <vector>

#include "enginetype.h"

namespace o3d {

class Context;
class Shadable;

/**
 * @brief Uniform buffer object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2018-01-01
 */
class O3D_API UniformBuffer
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
     * @param size Size in bytes.
     * @param dontUnbint Keep bound is True, else unbind.
     */
    void create(UInt32 size, Bool dontUnbind = False);

    /**
     * @brief Destroy the uniform buffer object.
     */
    void release();

private:

    Context *m_context;
    UInt32 m_id;
};

} // namespace o3d

#endif // _O3D_UNIFORMBUFFER_H
