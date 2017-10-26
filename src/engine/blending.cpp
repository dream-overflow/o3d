/**
 * @file blending.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/blending.h"
#include "o3d/core/debug.h"

#include "o3d/engine/glextensionmanager.h"

using namespace o3d;

//#define GL_MAX_DUAL_SOURCE_DRAW_BUFFERS   0x88FC

Blending::Blending()
{
}

Blending::~Blending()
{
}

Blending::BufferIndex::BufferIndex() :
    globalFunc(True),
    globalEquation(True),
    functions(DISABLED),
    equations(FUNC_ADD)
{
}

void Blending::forceDefaultFunc()
{
    glBlendFunc(ONE, ZERO);
    glDisable(GL_BLEND);

    m_global.globalFunc = True;
    m_global.functions = DISABLED;

    for (UInt32 i = 0; i < MAX_DRAW_BUFFERS; ++i)
    {
        m_locals[i].globalFunc = True;
    }
}

void Blending::forceDefaultEquation()
{
    glBlendEquation(FUNC_ADD);

    m_global.globalEquation = True;
    m_global.equations = FUNC_ADD;

    for (UInt32 i = 0; i < MAX_DRAW_BUFFERS; ++i)
    {
        m_locals[i].globalEquation = True;
    }
}

void Blending::setDefaultFunc()
{
    if (m_global.globalFunc && m_global.functions == DISABLED)
        return;

    glBlendFunc(ONE, ZERO);
    glDisable(GL_BLEND);

    m_global.globalFunc = True;
    m_global.functions = DISABLED;

    for (UInt32 i = 0; i < MAX_DRAW_BUFFERS; ++i)
    {
        m_locals[i].globalFunc = True;
    }
}

void Blending::setDefaultFunc(UInt32 drawBuffer)
{
    if (drawBuffer >= MAX_DRAW_BUFFERS)
        O3D_ERROR(E_IndexOutOfRange("Draw buffer index"));

    BufferIndex *buffer = &m_locals[drawBuffer];

    // global overrides
    if (buffer->globalFunc)
    {
        // inherit from global
        buffer->globalFunc = False;
        buffer->functions = m_global.functions;
    }

    // was global, set local
    if (m_global.globalFunc)
        m_global.globalFunc = False;

    // if local uses defaults
    if (buffer->functions == DISABLED)
        return;

    glBlendFunci(drawBuffer, ONE, ZERO);
    glDisablei(GL_BLEND, drawBuffer);

    buffer->functions = DISABLED;
}

void Blending::setDefaultEquation()
{
    if (m_global.globalEquation && m_global.equations == FUNC_ADD)
        return;

    glBlendEquation(FUNC_ADD);

    m_global.globalEquation = True;
    m_global.equations = FUNC_ADD;

    for (UInt32 i = 0; i < MAX_DRAW_BUFFERS; ++i)
    {
        m_locals[i].globalEquation = True;
    }
}

void Blending::setDefaultEquation(UInt32 drawBuffer)
{
    if (drawBuffer >= MAX_DRAW_BUFFERS)
        O3D_ERROR(E_IndexOutOfRange("Draw buffer index"));

    BufferIndex *buffer = &m_locals[drawBuffer];

    // global overrides
    if (buffer->globalEquation)
    {
        // inherit from global
        buffer->globalEquation = False;
        buffer->equations = m_global.equations;
    }

    // was global, set local
    if (m_global.globalEquation)
        m_global.globalEquation = False;

    // if local uses defaults
    if (buffer->equations == FUNC_ADD)
        return;

    glBlendEquationi(drawBuffer, FUNC_ADD);

    buffer->equations = FUNC_ADD;
}

Blending::FuncProfile Blending::getProfile() const
{
    if (!m_global.globalFunc)
        O3D_WARNING("Try to get the global blending configuration but it is defined per draw buffer.");

    return m_global.functions;
}

Blending::FuncProfile Blending::getProfile(UInt32 drawBuffer) const
{
    if (drawBuffer >= MAX_DRAW_BUFFERS)
        O3D_ERROR(E_IndexOutOfRange("Draw buffer index"));

    const BufferIndex *buffer = &m_locals[drawBuffer];

    // if global overrides
    if (m_global.globalFunc || buffer->globalFunc)
        return m_global.functions;
    else
        return buffer->functions;
}

Bool Blending::getState() const
{
    if (!m_global.globalFunc)
        O3D_WARNING("Try to get the global blending configuration but it is defined per draw buffer.");

    return m_global.functions != DISABLED;
}

Bool Blending::getState(UInt32 drawBuffer) const
{
    if (drawBuffer >= MAX_DRAW_BUFFERS)
        O3D_ERROR(E_IndexOutOfRange("Draw buffer index"));

    const BufferIndex *buffer = &m_locals[drawBuffer];

    // if global overrides
    if (m_global.globalFunc || buffer->globalFunc)
        return m_global.functions != DISABLED;
    else
        return buffer->functions != DISABLED;
}

void Blending::setFunc(Blending::Func src, Blending::Func dst)
{
    Bool enabled = m_global.functions != DISABLED;

    if (!m_global.globalFunc)
    {
        m_global.globalFunc = True;
        enabled = False;

        for (UInt32 i = 0; i < MAX_DRAW_BUFFERS; ++i)
        {
            m_locals[i].globalFunc = True;
        }
    }

    m_global.functions = CUSTOM;

    if (!enabled)
        glEnable(GL_BLEND);

    glBlendFunc(src, dst);
}

void Blending::setFunc(
        Blending::Func srcRgb, Blending::Func dstRgb,
        Blending::Func srcAlpha, Blending::Func dstAlpha)
{
    Bool enabled = m_global.functions != DISABLED;

    if (!m_global.globalFunc)
    {
        m_global.globalFunc = True;
        enabled = False;

        for (UInt32 i = 0; i < MAX_DRAW_BUFFERS; ++i)
        {
            m_locals[i].globalFunc = True;
        }
    }

    m_global.functions = CUSTOM;

    if (!enabled)
        glEnable(GL_BLEND);

    glBlendFuncSeparate(srcRgb, dstRgb, srcAlpha, dstAlpha);
}

void Blending::setFunc(UInt32 drawBuffer, Blending::Func src, Blending::Func dst)
{
    if (drawBuffer >= MAX_DRAW_BUFFERS)
        O3D_ERROR(E_IndexOutOfRange("Draw buffer index"));

    BufferIndex *buffer = &m_locals[drawBuffer];

    // global overrides
    if (buffer->globalFunc)
    {
        // inherit from global
        buffer->globalFunc = False;
        buffer->functions = m_global.functions;
    }

    Bool enabled = buffer->functions != DISABLED;

    // was global, set local
    if (m_global.globalFunc)
        m_global.globalFunc = False;

    if (!enabled)
        glEnablei(drawBuffer, GL_BLEND);

    glBlendFunci(drawBuffer, src, dst);
}

void Blending::setFunc(
        UInt32 drawBuffer,
        Blending::Func srcRgb,
        Blending::Func dstRgb,
        Blending::Func srcAlpha,
        Blending::Func dstAlpha)
{
    if (drawBuffer >= MAX_DRAW_BUFFERS)
        O3D_ERROR(E_IndexOutOfRange("Draw buffer index"));

    BufferIndex *buffer = &m_locals[drawBuffer];

    // global overrides
    if (buffer->globalFunc)
    {
        // inherit from global
        buffer->globalFunc = False;
        buffer->functions = m_global.functions;
        buffer->equations = m_global.equations;
    }

    Bool enabled = buffer->functions != DISABLED;

    // global does not longer overrides any buffers
    if (m_global.globalFunc)
        m_global.globalFunc = False;

    if (!enabled)
        glEnablei(drawBuffer, GL_BLEND);

    glBlendFuncSeparatei(drawBuffer, srcRgb, dstRgb, srcAlpha, dstAlpha);
}

void Blending::setFunc(Blending::FuncProfile profile)
{
    // was global, stay global, same profile => finished
    if (m_global.globalFunc && m_global.functions == profile)
        return;

    Bool enabled = m_global.functions != DISABLED;

    if (!m_global.globalFunc)
    {
        m_global.globalFunc = True;

        // to global for any
        for (UInt32 i = 0; i < MAX_DRAW_BUFFERS; ++i)
        {
            m_locals[i].globalFunc = True;
        }

        // force because of an indeterminate state
        if (profile == DISABLED)
        {
            glDisable(GL_BLEND);
            enabled = False;
        }
        else
        {
            glEnable(GL_BLEND);
            enabled = True;
        }
    }

    if (!enabled && profile != DISABLED)
        glEnable(GL_BLEND);

    switch(profile)
    {
        case DISABLED:
            if (enabled)
                glDisable(GL_BLEND);
            break;

        case CUSTOM:
            break;

        case SRC_A__ONE_MINUS_SRC_A:
            glBlendFunc(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
            break;

        case SRC_A__ONE:
            glBlendFunc(SRC_ALPHA, ONE);
            break;

        case SRC_A__ZERO:
            glBlendFunc(SRC_ALPHA, ZERO);
            break;

        case ONE__ONE:
            glBlendFunc(ONE, ONE);
            break;

        case DST_COL__SRC_COL:
            glBlendFunc(DST_COLOR, SRC_COLOR);
            break;

        case ONE__ONE_MINUS_SRC_A___ONE__ZERO:
            glBlendFuncSeparate(ONE, ONE_MINUS_SRC_ALPHA, ONE, ZERO);
            break;

        case ONE__ONE_MINUS_SRC_A___SRC_A__ZERO:
            glBlendFuncSeparate(ONE, ONE_MINUS_SRC_ALPHA, SRC_ALPHA, ZERO);
            break;

        case ONE__ONE_MINUS_SRC_A___ONE__ONE_MINUS_SRC_A:
            glBlendFuncSeparate(ONE, ONE_MINUS_SRC_ALPHA, ONE, ONE_MINUS_SRC_ALPHA);
            break;
    }

    m_global.functions = profile;
}

void Blending::setFunc(UInt32 drawBuffer, Blending::FuncProfile profile)
{
    if (drawBuffer >= MAX_DRAW_BUFFERS)
        O3D_ERROR(E_IndexOutOfRange("Draw buffer index"));

    // global overrides the same queried profile, shortcut
    if (m_global.globalFunc && m_global.functions == profile)
        return;

    BufferIndex *buffer = &m_locals[drawBuffer];

    if (buffer->globalFunc)
    {
        // inherit from global
        buffer->globalFunc = False;
        buffer->functions = m_global.functions;
    }

    Bool enabled = buffer->functions != DISABLED;

    // global does not longer overrides any buffers
    if (m_global.globalFunc)
        m_global.globalFunc = False;

    // already set
    if (buffer->functions == profile)
        return;

    if (!enabled && profile != DISABLED)
        glEnablei(drawBuffer, GL_BLEND);

    switch(profile)
    {
        case DISABLED:
            if (enabled)
                glDisablei(GL_BLEND, drawBuffer);
            break;

        case CUSTOM:
            break;

        case SRC_A__ONE_MINUS_SRC_A:
            glBlendFunci(drawBuffer, SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
            break;

        case SRC_A__ONE:
            glBlendFunc(SRC_ALPHA, ONE);
            break;

        case SRC_A__ZERO:
            glBlendFunc(SRC_ALPHA, ZERO);
            break;

        case ONE__ONE:
            glBlendFunc(ONE, ONE);
            break;

        case DST_COL__SRC_COL:
            glBlendFunc(DST_COLOR, SRC_COLOR);
            break;

        case ONE__ONE_MINUS_SRC_A___ONE__ZERO:
            glBlendFuncSeparate(ONE, ONE_MINUS_SRC_ALPHA, ONE, ZERO);
            break;

        case ONE__ONE_MINUS_SRC_A___SRC_A__ZERO:
            glBlendFuncSeparate(ONE, ONE_MINUS_SRC_ALPHA, SRC_ALPHA, ZERO);
            break;

        case ONE__ONE_MINUS_SRC_A___ONE__ONE_MINUS_SRC_A:
            glBlendFuncSeparate(ONE, ONE_MINUS_SRC_ALPHA, ONE, ONE_MINUS_SRC_ALPHA);
            break;
    }

    buffer->functions = profile;
}

void Blending::setEquation(Blending::Equation eq)
{
    Equation equations = m_global.equations;

    if (!m_global.globalEquation)
    {
        m_global.globalEquation = True;
        equations = (Equation)0;

        for (UInt32 i = 0; i < MAX_DRAW_BUFFERS; ++i)
        {
            m_locals[i].globalEquation = True;
        }
    }

    if (equations == eq)
        return;

    m_global.equations = eq;

    glBlendEquation(eq);
}

void Blending::setEquation(Blending::Equation rgb, Blending::Equation a)
{
    Equation equations = m_global.equations;

    if (!m_global.globalEquation)
    {
        m_global.globalEquation = True;
        equations = (Equation)0;

        for (UInt32 i = 0; i < MAX_DRAW_BUFFERS; ++i)
        {
            m_locals[i].globalEquation = True;
        }
    }

    if (equations == rgb && equations == a)
        return;

    m_global.equations = (Equation)0;

    glBlendEquationSeparate(rgb, a);
}

void Blending::setEquation(UInt32 drawBuffer, Blending::Equation eq)
{
    if (drawBuffer >= MAX_DRAW_BUFFERS)
        O3D_ERROR(E_IndexOutOfRange("Draw buffer index"));

    // global overrides the same queried equation, shortcut
    if (m_global.globalEquation && m_global.equations == eq)
        return;

    BufferIndex *buffer = &m_locals[drawBuffer];

    if (buffer->globalEquation)
    {
        // inherit from global
        buffer->globalEquation = False;
        buffer->equations = m_global.equations;
    }

    // global does not longer overrides any buffers
    if (m_global.globalEquation)
        m_global.globalEquation = False;

    // already set
    if (buffer->equations == eq)
        return;

    // setup blend equation if necessary
    glBlendEquationi(drawBuffer, eq);
    buffer->equations = eq;
}

void Blending::setEquation(UInt32 drawBuffer, Blending::Equation rgb, Blending::Equation a)
{
    if (drawBuffer >= MAX_DRAW_BUFFERS)
        O3D_ERROR(E_IndexOutOfRange("Draw buffer index"));

    // global overrides the same queried equation, shortcut
    if (m_global.globalEquation && (m_global.equations == rgb) && (m_global.equations == a))
        return;

    BufferIndex *buffer = &m_locals[drawBuffer];

    if (buffer->globalEquation)
    {
        // inherit from global
        buffer->globalEquation = False;
        buffer->equations = m_global.equations;
    }

    // global does not longer overrides any buffers
    if (m_global.globalEquation)
        m_global.globalEquation = False;

    // already set
    if ((buffer->equations == rgb) && (buffer->equations == a))
        return;

    // setup blend equation if necessary
    glBlendEquationSeparatei(drawBuffer, rgb, a);
    buffer->equations = (Equation)0;
}

