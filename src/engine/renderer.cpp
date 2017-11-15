/**
 * @file renderer.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/renderer.h"
#include "o3d/engine/context.h"

#include "o3d/core/debug.h"
#include "o3d/core/appwindow.h"

#include "o3d/core/architecture.h"
#include "o3d/engine/gl.h"
#include "o3d/engine/engineerror.h"

using namespace o3d;

// Default constructor
Renderer::Renderer() :
	m_glErrno(GL_NO_ERROR),
	m_version(OGL_UNDEFINED),
    m_sharing(nullptr),
	m_shareCount(0),
    m_appWindow(nullptr),
	m_HDC(NULL_HDC),
	m_HGLRC(NULL_HGLRC),
    m_state(),
    m_debugLevel(DEBUG_TYPE_PERFORMANCE),
	m_bpp(0),
	m_depth(0),
    m_stencil(0),
    m_samples(0),
	m_refCount(0),
    m_glContext(nullptr)
{
}

Renderer::~Renderer()
{
	destroy();
}

// Get the OpenGL context
Context* Renderer::getContext()
{
	return m_glContext;
}

// Get the OpenGL context (read only)
const Context* Renderer::getContext() const
{
    return m_glContext;
}

void Renderer::unrefShare()
{
	m_shareCount--;

	if (m_shareCount < 0)
		O3D_ERROR(E_IndexOutOfRange("Renderer share reference counter reach a negative value"));
}

void Renderer::useIt()
{
	m_refCount++;
}

void Renderer::releaseIt()
{
	m_refCount--;

	if (m_refCount < 0)
        O3D_ERROR(E_IndexOutOfRange("Renderer usage reference counter reach a negative value"));
}

void Renderer::setDebug(Bool debug)
{
    if (m_state.getBit(STATE_DEBUG_AVAILABLE))
    {
        if (debug && !m_state.getBit(STATE_DEBUG_ON))
        {
            enableDebug();
            m_state.enable(STATE_DEBUG_ON);
        }
        else if (!debug && m_state.getBit(STATE_DEBUG_ON))
        {
            disableDebug();
            m_state.disable(STATE_DEBUG_ON);
        }
    }
}

void Renderer::setDebugLevel(Renderer::DebugLevel level)
{
    m_debugLevel = level;
}

void Renderer::clearAll()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::clearColorBuffer()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::clearDepthBuffer()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void Renderer::clearStencilBuffer()
{
	glClear(GL_STENCIL_BUFFER_BIT);
}

Bool Renderer::screenShot(
	const String& filename,
	Image::FileFormat format,
	UInt32 quality)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("The OpenGL context must be defined before"));

	Image picture;

	Box2i viewPort = m_glContext->getViewPort();

	picture.allocate(viewPort.width(), viewPort.height(), 3);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glReadPixels(
			viewPort.x(),
			viewPort.y(),
			viewPort.width(),
			viewPort.height(),
			GL_RGB,
			GL_UNSIGNED_BYTE,
			picture.getDataWrite());

	picture.vFlip();

	Bool ret = picture.saveRgb(filename, format, quality);

	return ret;
}

String Renderer::getRendererName() const
{
	if (isCurrent())
	{
		String vendor(reinterpret_cast<const Char*>(glGetString(GL_VENDOR)));
		vendor += " ";
		String renderer(reinterpret_cast<const Char*>(glGetString(GL_RENDERER)));

		return vendor + renderer;
	}
	else
		return String("ERROR: Undefined OpenGL context");
}

String Renderer::getVersion() const
{
	if (isCurrent())
		return String(reinterpret_cast<const Char*>(glGetString(GL_VERSION)));
	else
		return String("ERROR: Undefined OpenGL context");
}

// Check for an error from OpenGL.
Bool Renderer::isError()
{
	if (isCurrent())
	{
		m_glErrno = glGetError();

		if (m_glErrno != GL_NO_ERROR)
			O3D_WARNING(getError());

		return (m_glErrno != GL_NO_ERROR);
	}
	else
		return False;
}

// Get an error string from OpenGL.
String Renderer::getError() const
{
	switch (m_glErrno)
	{
		case GL_NO_ERROR:
			return String("OpenGL: No error.");

		case GL_INVALID_ENUM:
			return String("OpenGL: An unacceptable value is specified for an enumerated argument.");

		case GL_INVALID_VALUE:
			return String("OpenGL: A numeric argument is out of range.");

		case GL_INVALID_OPERATION:
			return String("OpenGL: The specified operation is not allowed in the current state.");

		case GL_OUT_OF_MEMORY:
			return String("OpenGL: There is not enough memory left to	execute	the command.");

		default:
			return String("OpenGL: Undefined error.");
	}
}

// Attach clear signal to the application window.
void Renderer::doAttachment(AppWindow *appWindow)
{
    if (appWindow) {
        appWindow->onClear.connect(this, &Renderer::clearAll);
    }
}

#include "o3d/engine/glextensionmanager.h"

#define GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX          0x9047
#define GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX    0x9048
#define GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049
#define GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX            0x904A
#define GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX            0x904B

#define GL_VBO_FREE_MEMORY_ATI                     0x87FB
#define GL_TEXTURE_FREE_MEMORY_ATI                 0x87FC
#define GL_RENDERBUFFER_FREE_MEMORY_ATI            0x87FD

UInt32 Renderer::getTextureFreeMemory() const
{
    if (GLExtensionManager::isExtensionSupported("GL_NVX_gpu_memory_info"))
    {
        GLint free;
        glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &free);

        return (UInt32)free;
    }
    else if (GLExtensionManager::isExtensionSupported("GL_ATI_meminfo"))
    {
        GLint mem[4];
        glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, mem);

        return (UInt32)mem[0];
    }

    return 0;
}

UInt32 Renderer::getVBOFreeMemory() const
{
    if (GLExtensionManager::isExtensionSupported("GL_NVX_gpu_memory_info"))
    {
        GLint free;
        glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &free);

        return (UInt32)free;
    }
    else if (GLExtensionManager::isExtensionSupported("GL_ATI_meminfo"))
    {
        GLint mem[4];
        glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, mem);

        return (UInt32)mem[0];
    }

    return 0;
}

UInt32 Renderer::getRenderBufferFreeMemory() const
{
    if (GLExtensionManager::isExtensionSupported("GL_NVX_gpu_memory_info"))
    {
        GLint free;
        glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &free);

        return (UInt32)free;
    }
    else if (GLExtensionManager::isExtensionSupported("GL_ATI_meminfo"))
    {
        GLint mem[4];
        glGetIntegerv(GL_RENDERBUFFER_FREE_MEMORY_ATI, mem);

        return (UInt32)mem[0];
    }

    return 0;
}

#include "o3d/engine/glextdefines.h"

#ifndef CALLBACK
#define CALLBACK
#endif

static void formatDebugOutputARB(
    String &output,
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    const char *msg)
{
    // source
    String lsource;
    switch (source)
    {
        case GL_DEBUG_SOURCE_API_ARB:
            lsource = "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
            lsource = "WINDOW_SYSTEM";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
            lsource = "SHADER_COMPILER";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
            lsource = "THIRD_PARTY";
            break;
        case GL_DEBUG_SOURCE_APPLICATION_ARB:
            lsource = "APPLICATION";
            break;
        case GL_DEBUG_SOURCE_OTHER_ARB:
            lsource = "OTHER";
            break;
        default:
            lsource = String::print("UNDEFINED(0x%x)", source);
            break;
    }

    // type
    String ltype;
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR_ARB:
            ltype = "ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
            ltype = "DEPRECATED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
            ltype = "UNDEFINED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_PORTABILITY_ARB:
            ltype = "PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE_ARB:
            ltype = "PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_OTHER_ARB:
            ltype = "OTHER";
            break;
        default:
            ltype = String::print("UNDEFINED(0x%x)", type);
            break;
    }

    // severity
    String lseverity;
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH_ARB:
            lseverity = "HIGH";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB:
            lseverity = "MEDIUM";
            break;
        case GL_DEBUG_SEVERITY_LOW_ARB:
            lseverity = "LOW";
            break;
        default:
            lseverity = "UNDEFINED";
            break;
    }

    output = String::print(
        "OpenGL: %s [source=%ls type=%ls severity=%ls id=%u]",
        msg,
        lsource.getData(),
        ltype.getData(),
        lseverity.getData(),
        id);
}

static void CALLBACK debugCallbackARB(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const char *message,
    GLvoid *userParam)
{
    String lmessage;

    // minimal debug level
    if (type > static_cast<Renderer*>(userParam)->getDebugLevel())
        return;

    formatDebugOutputARB(lmessage, source, type, id, severity, message);

    if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)
        O3D_ERROR(E_OpenGLDebug(lmessage));
    else
        O3D_WARNING(lmessage);
}

//
// setDebug
//

typedef GLvoid (APIENTRY *PFNGLDEBUGPROCARB)(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        GLvoid* userParam);

typedef GLvoid (APIENTRYP PFNGLDEBUGMESSAGECALLBACKARBPROC)(PFNGLDEBUGPROCARB callback, GLvoid* userParam);

void Renderer::initDebug()
{
    if (GLExtensionManager::isExtensionSupported("GL_KHR_debug"))
    {
        PFNGLDEBUGMESSAGECALLBACKARBPROC glDebugMessageCallbackARB = nullptr;
        glDebugMessageCallbackARB = GLExtensionManager::getFunction<PFNGLDEBUGMESSAGECALLBACKARBPROC>("glDebugMessageCallback");

        glDebugMessageCallbackARB(debugCallbackARB, this);

        m_state.enable(STATE_DEBUG_AVAILABLE);
    }
    else if (GLExtensionManager::isExtensionSupported("GL_ARB_debug_output"))
    {
        PFNGLDEBUGMESSAGECALLBACKARBPROC glDebugMessageCallbackARB = nullptr;
        glDebugMessageCallbackARB = GLExtensionManager::getFunction<PFNGLDEBUGMESSAGECALLBACKARBPROC>("glDebugMessageCallbackARB");

        glDebugMessageCallbackARB(debugCallbackARB, this);

        m_state.enable(STATE_DEBUG_AVAILABLE);
    }
    else
    {
        m_state.disable(STATE_DEBUG_AVAILABLE);
    }
}

void Renderer::enableDebug()
{
    if (m_state.getBit(STATE_DEBUG_AVAILABLE))
    {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        O3D_MESSAGE("GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB is now active");
    }
}

void Renderer::disableDebug()
{
    if (m_state.getBit(STATE_DEBUG_AVAILABLE))
    {
        glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        O3D_MESSAGE("GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB is now disabled");
    }
}
