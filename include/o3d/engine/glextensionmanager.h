/**
 * @file glextensionmanager.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_GLEXTENSIONMANAGER_H
#define _O3D_GLEXTENSIONMANAGER_H

#include "o3d/core/string.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class DynamicLibrary;

/**
 * @brief OpenGL extension manager.
 * It load many extensions entry point and check for theirs existence. The manager can
 * thrown an exception or warning if some entries points are missing.
 * @date 2003-01-30
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class O3D_API GLExtensionManager
{
public:

    /**
     * Initialize the extension manager. Depending of the build options and target
     * get OpenGL function pointers.
     */
    static void init();

    /**
     * @brief isExtensionSupported Check for an extension.
     * @param ext Extension name.
     * @return True if the extension exists.
     */
	static Bool isExtensionSupported(const String &ext);

    /**
     * @brief getFunction Get a function pointer.
     * @param foo Function name.
     * @return Pointer onto the queried function or nullptr if not found.
     */
    static void* getFunctionPtr(const Char *foo);

    /**
     * @brief getFunction Get a function pointer.
     * @param foo Function name.
     * @return Pointer onto the queried function or nullptr if not found.
     */
    static void* getFunctionPtr(const CString &foo);

    /**
     * @brief getFunction Get a typed function pointer.
     * @param foo Function name.
     * @return Pointer onto the queried function or nullptr if not found.
     */
    template<class T>
    static inline T getFunction(const CString &foo)
    {
        return reinterpret_cast<T>(getFunctionPtr(foo));
    }

private:

    static Bool ms_valid;
    static DynamicLibrary *ms_openGL;

    static void getGLCommonFunctions();
    static void getGLFunctions();
    static void getGLESFunctions();
};

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifndef O3D_GLAPI
#define O3D_GLAPI O3D_API extern
#endif

// Base GL types
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;

#ifndef O3D_GL_ARB_sync
#define O3D_GL_ARB_sync 1
typedef o3d::Int64 GLint64;
typedef o3d::UInt64 GLuint64;
typedef struct __GLsync *GLsync;
#endif

#ifndef O3D_GL_VERSION_1_0
#define O3D_GL_VERSION_1_0 1
typedef void (APIENTRYP PFNGLCULLFACEPROC) (GLenum mode);
typedef void (APIENTRYP PFNGLFRONTFACEPROC) (GLenum mode);
typedef void (APIENTRYP PFNGLHINTPROC) (GLenum target, GLenum mode);
typedef void (APIENTRYP PFNGLLINEWIDTHPROC) (GLfloat width);
typedef void (APIENTRYP PFNGLPOINTSIZEPROC) (GLfloat size);
typedef void (APIENTRYP PFNGLPOLYGONMODEPROC) (GLenum face, GLenum mode);
typedef void (APIENTRYP PFNGLSCISSORPROC) (GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLTEXPARAMETERFPROC) (GLenum target, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLTEXPARAMETERFVPROC) (GLenum target, GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLTEXPARAMETERIPROC) (GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLTEXPARAMETERIVPROC) (GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLTEXIMAGE1DPROC) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (APIENTRYP PFNGLTEXIMAGE2DPROC) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (APIENTRYP PFNGLDRAWBUFFERPROC) (GLenum mode);
typedef void (APIENTRYP PFNGLCLEARPROC) (GLbitfield mask);
typedef void (APIENTRYP PFNGLCLEARCOLORPROC) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
typedef void (APIENTRYP PFNGLCLEARSTENCILPROC) (GLint s);
typedef void (APIENTRYP PFNGLCLEARDEPTHPROC) (GLclampd depth);
typedef void (APIENTRYP PFNGLSTENCILMASKPROC) (GLuint mask);
typedef void (APIENTRYP PFNGLCOLORMASKPROC) (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
typedef void (APIENTRYP PFNGLDEPTHMASKPROC) (GLboolean flag);
typedef void (APIENTRYP PFNGLDISABLEPROC) (GLenum cap);
typedef void (APIENTRYP PFNGLENABLEPROC) (GLenum cap);
typedef void (APIENTRYP PFNGLFINISHPROC) (void);
typedef void (APIENTRYP PFNGLFLUSHPROC) (void);
typedef void (APIENTRYP PFNGLBLENDFUNCPROC) (GLenum sfactor, GLenum dfactor);
typedef void (APIENTRYP PFNGLLOGICOPPROC) (GLenum opcode);
typedef void (APIENTRYP PFNGLSTENCILFUNCPROC) (GLenum func, GLint ref, GLuint mask);
typedef void (APIENTRYP PFNGLSTENCILOPPROC) (GLenum fail, GLenum zfail, GLenum zpass);
typedef void (APIENTRYP PFNGLDEPTHFUNCPROC) (GLenum func);
typedef void (APIENTRYP PFNGLPIXELSTOREFPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLPIXELSTOREIPROC) (GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLREADBUFFERPROC) (GLenum mode);
typedef void (APIENTRYP PFNGLREADPIXELSPROC) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
typedef void (APIENTRYP PFNGLGETBOOLEANVPROC) (GLenum pname, GLboolean *params);
typedef void (APIENTRYP PFNGLGETDOUBLEVPROC) (GLenum pname, GLdouble *params);
typedef GLenum (APIENTRYP PFNGLGETERRORPROC) (void);
typedef void (APIENTRYP PFNGLGETFLOATVPROC) (GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETINTEGERVPROC) (GLenum pname, GLint *params);
typedef const GLubyte * (APIENTRYP PFNGLGETSTRINGPROC) (GLenum name);
typedef void (APIENTRYP PFNGLGETTEXIMAGEPROC) (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
typedef void (APIENTRYP PFNGLGETTEXPARAMETERFVPROC) (GLenum target, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETTEXLEVELPARAMETERFVPROC) (GLenum target, GLint level, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETTEXLEVELPARAMETERIVPROC) (GLenum target, GLint level, GLenum pname, GLint *params);
typedef GLboolean (APIENTRYP PFNGLISENABLEDPROC) (GLenum cap);
typedef void (APIENTRYP PFNGLDEPTHRANGEPROC) (GLclampd near, GLclampd far);
typedef void (APIENTRYP PFNGLVIEWPORTPROC) (GLint x, GLint y, GLsizei width, GLsizei height);
O3D_GLAPI PFNGLCULLFACEPROC glCullFace;
O3D_GLAPI PFNGLFRONTFACEPROC glFrontFace;
O3D_GLAPI PFNGLHINTPROC glHint;
O3D_GLAPI PFNGLLINEWIDTHPROC glLineWidth;
O3D_GLAPI PFNGLPOINTSIZEPROC glPointSize;
O3D_GLAPI PFNGLPOLYGONMODEPROC glPolygonMode;
O3D_GLAPI PFNGLSCISSORPROC glScissor;
O3D_GLAPI PFNGLTEXPARAMETERFPROC glTexParameterf;
O3D_GLAPI PFNGLTEXPARAMETERFVPROC glTexParameterfv;
O3D_GLAPI PFNGLTEXPARAMETERIPROC glTexParameteri;
O3D_GLAPI PFNGLTEXPARAMETERIVPROC glTexParameteriv;
O3D_GLAPI PFNGLTEXIMAGE1DPROC glTexImage1D;
O3D_GLAPI PFNGLTEXIMAGE2DPROC glTexImage2D;
O3D_GLAPI PFNGLDRAWBUFFERPROC glDrawBuffer;
O3D_GLAPI PFNGLCLEARPROC glClear;
O3D_GLAPI PFNGLCLEARCOLORPROC glClearColor;
O3D_GLAPI PFNGLCLEARSTENCILPROC glClearStencil;
O3D_GLAPI PFNGLCLEARDEPTHPROC glClearDepth;
O3D_GLAPI PFNGLSTENCILMASKPROC glStencilMask;
O3D_GLAPI PFNGLCOLORMASKPROC glColorMask;
O3D_GLAPI PFNGLDEPTHMASKPROC glDepthMask;
O3D_GLAPI PFNGLDISABLEPROC glDisable;
O3D_GLAPI PFNGLENABLEPROC glEnable;
O3D_GLAPI PFNGLFINISHPROC glFinish;
O3D_GLAPI PFNGLFLUSHPROC glFlush;
O3D_GLAPI PFNGLBLENDFUNCPROC glBlendFunc;
O3D_GLAPI PFNGLLOGICOPPROC glLogicOp;
O3D_GLAPI PFNGLSTENCILFUNCPROC glStencilFunc;
O3D_GLAPI PFNGLSTENCILOPPROC glStencilOp;
O3D_GLAPI PFNGLDEPTHFUNCPROC glDepthFunc;
O3D_GLAPI PFNGLPIXELSTOREFPROC glPixelStoref;
O3D_GLAPI PFNGLPIXELSTOREIPROC glPixelStorei;
O3D_GLAPI PFNGLREADBUFFERPROC glReadBuffer;
O3D_GLAPI PFNGLREADPIXELSPROC glReadPixels;
O3D_GLAPI PFNGLGETBOOLEANVPROC glGetBooleanv;
O3D_GLAPI PFNGLGETDOUBLEVPROC glGetDoublev;
O3D_GLAPI PFNGLGETERRORPROC glGetError;
O3D_GLAPI PFNGLGETFLOATVPROC glGetFloatv;
O3D_GLAPI PFNGLGETINTEGERVPROC _glGetIntegerv;
O3D_GLAPI PFNGLGETSTRINGPROC _glGetString;
O3D_GLAPI PFNGLGETTEXIMAGEPROC glGetTexImage;
O3D_GLAPI PFNGLGETTEXPARAMETERFVPROC glGetTexParameterfv;
O3D_GLAPI PFNGLGETTEXPARAMETERIVPROC glGetTexParameteriv;
O3D_GLAPI PFNGLGETTEXLEVELPARAMETERFVPROC glGetTexLevelParameterfv;
O3D_GLAPI PFNGLGETTEXLEVELPARAMETERIVPROC glGetTexLevelParameteriv;
O3D_GLAPI PFNGLISENABLEDPROC glIsEnabled;
O3D_GLAPI PFNGLDEPTHRANGEPROC glDepthRange;
O3D_GLAPI PFNGLVIEWPORTPROC glViewport;
#endif // O3D_GL_VERSION_1_0

#ifndef O3D_GL_VERSION_1_1
#define O3D_GL_VERSION_1_1 1
typedef void (APIENTRYP PFNGLDRAWARRAYSPROC) (GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC) (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
typedef void (APIENTRYP PFNGLGETPOINTERVPROC) (GLenum pname, GLvoid* *params);
typedef void (APIENTRYP PFNGLPOLYGONOFFSETPROC) (GLfloat factor, GLfloat units);
typedef void (APIENTRYP PFNGLCOPYTEXIMAGE1DPROC) (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
typedef void (APIENTRYP PFNGLCOPYTEXIMAGE2DPROC) (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE1DPROC) (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE2DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE1DPROC) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE2DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (APIENTRYP PFNGLBINDTEXTUREPROC) (GLenum target, GLuint texture);
typedef void (APIENTRYP PFNGLDELETETEXTURESPROC) (GLsizei n, const GLuint *textures);
typedef void (APIENTRYP PFNGLGENTEXTURESPROC) (GLsizei n, GLuint *textures);
typedef GLboolean (APIENTRYP PFNGLISTEXTUREPROC) (GLuint texture);
O3D_GLAPI PFNGLDRAWARRAYSPROC glDrawArrays;
O3D_GLAPI PFNGLDRAWELEMENTSPROC glDrawElements;
O3D_GLAPI PFNGLGETPOINTERVPROC glGetPointerv;
O3D_GLAPI PFNGLPOLYGONOFFSETPROC glPolygonOffset;
O3D_GLAPI PFNGLCOPYTEXIMAGE1DPROC glCopyTexImage1D;
O3D_GLAPI PFNGLCOPYTEXIMAGE2DPROC glCopyTexImage2D;
O3D_GLAPI PFNGLCOPYTEXSUBIMAGE1DPROC glCopyTexSubImage1D;
O3D_GLAPI PFNGLCOPYTEXSUBIMAGE2DPROC glCopyTexSubImage2D;
O3D_GLAPI PFNGLTEXSUBIMAGE1DPROC glTexSubImage1D;
O3D_GLAPI PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D;
O3D_GLAPI PFNGLBINDTEXTUREPROC glBindTexture;
O3D_GLAPI PFNGLDELETETEXTURESPROC glDeleteTextures;
O3D_GLAPI PFNGLGENTEXTURESPROC glGenTextures;
O3D_GLAPI PFNGLISTEXTUREPROC glIsTexture;
#endif // O3D_GL_VERSION_1_1

#ifndef O3D_GL_VERSION_1_2
#define O3D_GL_VERSION_1_2 1
typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSPROC) (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);
typedef void (APIENTRYP PFNGLTEXIMAGE3DPROC) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels);
O3D_GLAPI PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;
O3D_GLAPI PFNGLTEXIMAGE3DPROC    glTexImage3D;
O3D_GLAPI PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
#endif // O3D_GL_VERSION_1_2

#ifndef O3D_GL_VERSION_1_3
#define O3D_GL_VERSION_1_3 1
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC) (GLenum texture);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE3DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE2DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE1DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXIMAGEPROC) (GLenum target, GLint level, GLvoid *img);
O3D_GLAPI PFNGLACTIVETEXTUREPROC glActiveTexture;
O3D_GLAPI PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D;
O3D_GLAPI PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
O3D_GLAPI PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D;
O3D_GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;
O3D_GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;
O3D_GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D;
O3D_GLAPI PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage;
#endif // O3D_GL_VERSION_1_3

#ifndef O3D_GL_VERSION_1_4
#define O3D_GL_VERSION_1_4 1
typedef void (APIENTRYP PFNGLPOINTPARAMETERFPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFVPROC) (GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLPOINTPARAMETERIPROC) (GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLPOINTPARAMETERIVPROC) (GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEPROC) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSPROC) (GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSPROC) (GLenum mode, const GLsizei *count, GLenum type, const GLvoid* const *indices, GLsizei drawcount);
typedef void (APIENTRYP PFNGLBLENDEQUATIONPROC) (GLenum mode);
O3D_GLAPI PFNGLPOINTPARAMETERFPROC	glPointParameterf;
O3D_GLAPI PFNGLPOINTPARAMETERFVPROC	glPointParameterfv;
O3D_GLAPI PFNGLPOINTPARAMETERIPROC	glPointParameteri;
O3D_GLAPI PFNGLPOINTPARAMETERIVPROC	glPointParameteriv;
O3D_GLAPI PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
O3D_GLAPI PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays;
O3D_GLAPI PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements;
O3D_GLAPI PFNGLBLENDEQUATIONPROC glBlendEquation;
#endif // O3D_GL_VERSION_1_4

#ifndef O3D_GL_VERSION_1_5
#define O3D_GL_VERSION_1_5 1
// GL types for handling large vertex buffer objects
typedef std::ptrdiff_t GLintptr;
typedef std::ptrdiff_t GLsizeiptr;

typedef void (APIENTRYP PFNGLGENQUERIESPROC) (GLsizei n, GLuint *ids);
typedef void (APIENTRYP PFNGLDELETEQUERIESPROC) (GLsizei n, const GLuint *ids);
typedef GLboolean (APIENTRYP PFNGLISQUERYPROC) (GLuint id);
typedef void (APIENTRYP PFNGLBEGINQUERYPROC) (GLenum target, GLuint id);
typedef void (APIENTRYP PFNGLENDQUERYPROC) (GLenum target);
typedef void (APIENTRYP PFNGLGETQUERYIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTIVPROC) (GLuint id, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTUIVPROC) (GLuint id, GLenum pname, GLuint *params);
O3D_GLAPI PFNGLGENQUERIESPROC	glGenQueries;
O3D_GLAPI PFNGLISQUERYPROC		glIsQuery;
O3D_GLAPI PFNGLBEGINQUERYPROC	glBeginQuery;
O3D_GLAPI PFNGLENDQUERYPROC		glEndQuery;
O3D_GLAPI PFNGLGETQUERYOBJECTIVPROC	glGetQueryObjectiv;
O3D_GLAPI PFNGLGETQUERYIVPROC	glGetQueryiv;
O3D_GLAPI PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv;
O3D_GLAPI PFNGLDELETEQUERIESPROC	glDeleteQueries;

typedef void (APIENTRYP PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRYP PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRYP PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
typedef void (APIENTRYP PFNGLBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
typedef void (APIENTRYP PFNGLGETBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data);
typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef GLvoid* (APIENTRYP PFNGLMAPBUFFERPROC) (GLenum target, GLenum access);
typedef GLboolean (APIENTRYP PFNGLUNMAPBUFFERPROC) (GLenum target);
O3D_GLAPI PFNGLBINDBUFFERPROC glBindBuffer;
O3D_GLAPI PFNGLDELETEBUFFERSPROC glDeleteBuffers;
O3D_GLAPI PFNGLGENBUFFERSPROC glGenBuffers;
O3D_GLAPI PFNGLBUFFERDATAPROC glBufferData;
O3D_GLAPI PFNGLBUFFERSUBDATAPROC glBufferSubData;
O3D_GLAPI PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
O3D_GLAPI PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
O3D_GLAPI PFNGLMAPBUFFERPROC glMapBuffer;
O3D_GLAPI PFNGLUNMAPBUFFERPROC glUnmapBuffer;
#endif // O3D_GL_VERSION_1_5

#ifndef O3D_GL_VERSION_2_0
#define O3D_GL_VERSION_2_0 1
/* GL type for program/shader text */
typedef char GLchar;  /* native character */

// shading_language and misc
typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEPROC) (GLenum modeRGB, GLenum modeAlpha);

typedef void (APIENTRYP PFNGLDRAWBUFFERSPROC) (GLsizei n, const GLenum *bufs);

typedef void (APIENTRYP PFNGLSTENCILOPSEPARATEPROC) (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
typedef void (APIENTRYP PFNGLSTENCILFUNCSEPARATEPROC) (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask);
typedef void (APIENTRYP PFNGLSTENCILMASKSEPARATEPROC) (GLenum face, GLuint mask);

typedef void (APIENTRYP PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRYP PFNGLBINDATTRIBLOCATIONPROC) (GLuint program, GLuint index, const GLchar *name);
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC) (void);
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC) (GLenum type);
typedef void (APIENTRYP PFNGLDELETEPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLDELETESHADERPROC) (GLuint shader);
typedef void (APIENTRYP PFNGLDETACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRYP PFNGLGETACTIVEATTRIBPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
typedef void (APIENTRYP PFNGLGETATTACHEDSHADERSPROC) (GLuint program, GLsizei maxCount, GLsizei *count, GLuint *obj);
typedef GLint (APIENTRYP PFNGLGETATTRIBLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRYP PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRYP PFNGLGETSHADERSOURCEPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLGETUNIFORMFVPROC) (GLuint program, GLint location, GLfloat *params);
typedef void (APIENTRYP PFNGLGETUNIFORMIVPROC) (GLuint program, GLint location, GLint *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBDVPROC) (GLuint index, GLenum pname, GLdouble *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBFVPROC) (GLuint index, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIVPROC) (GLuint index, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBPOINTERVPROC) (GLuint index, GLenum pname, GLvoid* *pointer);
typedef GLboolean (APIENTRYP PFNGLISPROGRAMPROC) (GLuint program);
typedef GLboolean (APIENTRYP PFNGLISSHADERPROC) (GLuint shader);
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar* *string, const GLint *length);
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);
typedef void (APIENTRYP PFNGLUNIFORM2FPROC) (GLint location, GLfloat v0, GLfloat v1);
typedef void (APIENTRYP PFNGLUNIFORM3FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRYP PFNGLUNIFORM4FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRYP PFNGLUNIFORM1IPROC) (GLint location, GLint v0);
typedef void (APIENTRYP PFNGLUNIFORM2IPROC) (GLint location, GLint v0, GLint v1);
typedef void (APIENTRYP PFNGLUNIFORM3IPROC) (GLint location, GLint v0, GLint v1, GLint v2);
typedef void (APIENTRYP PFNGLUNIFORM4IPROC) (GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void (APIENTRYP PFNGLUNIFORM1FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM2FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM3FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM4FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM1IVPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLUNIFORM2IVPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLUNIFORM3IVPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLUNIFORM4IVPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1DPROC) (GLuint index, GLdouble x);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FPROC) (GLuint index, GLfloat x);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1SPROC) (GLuint index, GLshort x);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1SVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2DPROC) (GLuint index, GLdouble x, GLdouble y);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FPROC) (GLuint index, GLfloat x, GLfloat y);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2SPROC) (GLuint index, GLshort x, GLshort y);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2SVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3DPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3SPROC) (GLuint index, GLshort x, GLshort y, GLshort z);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3SVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NBVPROC) (GLuint index, const GLbyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NIVPROC) (GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NSVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUBPROC) (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUBVPROC) (GLuint index, const GLubyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUIVPROC) (GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUSVPROC) (GLuint index, const GLushort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4BVPROC) (GLuint index, const GLbyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4DPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4IVPROC) (GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4SPROC) (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4SVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4UBVPROC) (GLuint index, const GLubyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4UIVPROC) (GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4USVPROC) (GLuint index, const GLushort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);

O3D_GLAPI PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;

O3D_GLAPI PFNGLDRAWBUFFERSPROC glDrawBuffers;

O3D_GLAPI PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate;
O3D_GLAPI PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;
O3D_GLAPI PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate;

O3D_GLAPI PFNGLCREATESHADERPROC		glCreateShader;
O3D_GLAPI PFNGLSHADERSOURCEPROC		glShaderSource;
O3D_GLAPI PFNGLCOMPILESHADERPROC	glCompileShader;
O3D_GLAPI PFNGLCREATEPROGRAMPROC	glCreateProgram;
O3D_GLAPI PFNGLATTACHSHADERPROC		glAttachShader;
O3D_GLAPI PFNGLLINKPROGRAMPROC		glLinkProgram;
O3D_GLAPI PFNGLVALIDATEPROGRAMPROC	glValidateProgram;
O3D_GLAPI PFNGLUSEPROGRAMPROC		glUseProgram;

O3D_GLAPI PFNGLDETACHSHADERPROC		glDetachShader;
O3D_GLAPI PFNGLDELETESHADERPROC		glDeleteShader;
O3D_GLAPI PFNGLDELETEPROGRAMPROC	glDeleteProgram;

O3D_GLAPI PFNGLGETPROGRAMIVPROC		glGetProgramiv;
O3D_GLAPI PFNGLGETPROGRAMINFOLOGPROC	glGetProgramInfoLog;
O3D_GLAPI PFNGLGETSHADERIVPROC		glGetShaderiv;
O3D_GLAPI PFNGLGETSHADERINFOLOGPROC	glGetShaderInfoLog;
O3D_GLAPI PFNGLISPROGRAMPROC		glIsProgram;
O3D_GLAPI PFNGLISSHADERPROC			glIsShader;

O3D_GLAPI PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
O3D_GLAPI PFNGLGETACTIVEUNIFORMPROC   glGetActiveUniform;
O3D_GLAPI PFNGLUNIFORM1FPROC	glUniform1f;
O3D_GLAPI PFNGLUNIFORM2FPROC	glUniform2f;
O3D_GLAPI PFNGLUNIFORM3FPROC	glUniform3f;
O3D_GLAPI PFNGLUNIFORM4FPROC	glUniform4f;
O3D_GLAPI PFNGLUNIFORM1FVPROC	glUniform1fv;
O3D_GLAPI PFNGLUNIFORM2FVPROC	glUniform2fv;
O3D_GLAPI PFNGLUNIFORM3FVPROC	glUniform3fv;
O3D_GLAPI PFNGLUNIFORM4FVPROC	glUniform4fv;
O3D_GLAPI PFNGLUNIFORM1IPROC	glUniform1i;
O3D_GLAPI PFNGLUNIFORM2IPROC	glUniform2i;
O3D_GLAPI PFNGLUNIFORM3IPROC	glUniform3i;
O3D_GLAPI PFNGLUNIFORM4IPROC	glUniform4i;
O3D_GLAPI PFNGLUNIFORMMATRIX2FVPROC	glUniformMatrix2fv;
O3D_GLAPI PFNGLUNIFORMMATRIX3FVPROC	glUniformMatrix3fv;
O3D_GLAPI PFNGLUNIFORMMATRIX4FVPROC	glUniformMatrix4fv;

O3D_GLAPI PFNGLDISABLEVERTEXATTRIBARRAYPROC	glDisableVertexAttribArray;
O3D_GLAPI PFNGLENABLEVERTEXATTRIBARRAYPROC	glEnableVertexAttribArray;
O3D_GLAPI PFNGLGETACTIVEATTRIBPROC		glGetActiveAttrib;
O3D_GLAPI PFNGLBINDATTRIBLOCATIONPROC	glBindAttribLocation;
O3D_GLAPI PFNGLGETATTRIBLOCATIONPROC	glGetAttribLocation;
O3D_GLAPI PFNGLGETVERTEXATTRIBDVPROC	glGetVertexAttribdv;
O3D_GLAPI PFNGLGETVERTEXATTRIBFVPROC	glGetVertexAttribfv;
O3D_GLAPI PFNGLGETVERTEXATTRIBIVPROC	glGetVertexAttribiv;
O3D_GLAPI PFNGLGETVERTEXATTRIBPOINTERVPROC	glGetVertexAttribPointerv;

O3D_GLAPI PFNGLVERTEXATTRIB1DPROC		glVertexAttrib1d;
O3D_GLAPI PFNGLVERTEXATTRIB1DVPROC		glVertexAttrib1dv;
O3D_GLAPI PFNGLVERTEXATTRIB1FPROC		glVertexAttrib1f;
O3D_GLAPI PFNGLVERTEXATTRIB1FVPROC		glVertexAttrib1fv;
O3D_GLAPI PFNGLVERTEXATTRIB1SPROC		glVertexAttrib1s;
O3D_GLAPI PFNGLVERTEXATTRIB1SVPROC		glVertexAttrib1sv;
O3D_GLAPI PFNGLVERTEXATTRIB2DPROC		glVertexAttrib2d;
O3D_GLAPI PFNGLVERTEXATTRIB2DVPROC		glVertexAttrib2dv;
O3D_GLAPI PFNGLVERTEXATTRIB2FPROC		glVertexAttrib2f;
O3D_GLAPI PFNGLVERTEXATTRIB2FVPROC		glVertexAttrib2fv;
O3D_GLAPI PFNGLVERTEXATTRIB2SPROC		glVertexAttrib2s;
O3D_GLAPI PFNGLVERTEXATTRIB2SVPROC		glVertexAttrib2sv;
O3D_GLAPI PFNGLVERTEXATTRIB3DPROC		glVertexAttrib3d;
O3D_GLAPI PFNGLVERTEXATTRIB3DVPROC		glVertexAttrib3dv;
O3D_GLAPI PFNGLVERTEXATTRIB3FPROC		glVertexAttrib3f;
O3D_GLAPI PFNGLVERTEXATTRIB3FVPROC		glVertexAttrib3fv;
O3D_GLAPI PFNGLVERTEXATTRIB3SPROC		glVertexAttrib3s;
O3D_GLAPI PFNGLVERTEXATTRIB3SVPROC		glVertexAttrib3sv;
O3D_GLAPI PFNGLVERTEXATTRIB4NBVPROC		glVertexAttrib4Nbv;
O3D_GLAPI PFNGLVERTEXATTRIB4NIVPROC		glVertexAttrib4Niv;
O3D_GLAPI PFNGLVERTEXATTRIB4NSVPROC		glVertexAttrib4Nsv;
O3D_GLAPI PFNGLVERTEXATTRIB4NUBPROC		glVertexAttrib4Nub;
O3D_GLAPI PFNGLVERTEXATTRIB4NUBVPROC	glVertexAttrib4Nubv;
O3D_GLAPI PFNGLVERTEXATTRIB4NUIVPROC	glVertexAttrib4Nuiv;
O3D_GLAPI PFNGLVERTEXATTRIB4NUSVPROC	glVertexAttrib4Nusv;
O3D_GLAPI PFNGLVERTEXATTRIB4BVPROC		glVertexAttrib4bv;
O3D_GLAPI PFNGLVERTEXATTRIB4DPROC		glVertexAttrib4d;
O3D_GLAPI PFNGLVERTEXATTRIB4DVPROC		glVertexAttrib4dv;
O3D_GLAPI PFNGLVERTEXATTRIB4FPROC		glVertexAttrib4f;
O3D_GLAPI PFNGLVERTEXATTRIB4FVPROC		glVertexAttrib4fv;
O3D_GLAPI PFNGLVERTEXATTRIB4IVPROC		glVertexAttrib4iv;
O3D_GLAPI PFNGLVERTEXATTRIB4SPROC		glVertexAttrib4s;
O3D_GLAPI PFNGLVERTEXATTRIB4SVPROC		glVertexAttrib4sv;
O3D_GLAPI PFNGLVERTEXATTRIB4UBVPROC		glVertexAttrib4ubv;
O3D_GLAPI PFNGLVERTEXATTRIB4UIVPROC		glVertexAttrib4uiv;
O3D_GLAPI PFNGLVERTEXATTRIB4USVPROC		glVertexAttrib4usv;
O3D_GLAPI PFNGLVERTEXATTRIBPOINTERPROC	glVertexAttribPointer;
#endif // O3D_GL_VERSION_2_0

#ifndef O3D_GL_VERSION_3_0
#define O3D_GL_VERSION_3_0 1
typedef void (APIENTRYP PFNGLCOLORMASKIPROC) (GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
typedef void (APIENTRYP PFNGLGETBOOLEANI_VPROC) (GLenum target, GLuint index, GLboolean *data);
typedef void (APIENTRYP PFNGLGETINTEGERI_VPROC) (GLenum target, GLuint index, GLint *data);
typedef void (APIENTRYP PFNGLENABLEIPROC) (GLenum target, GLuint index);
typedef void (APIENTRYP PFNGLDISABLEIPROC) (GLenum target, GLuint index);
typedef GLboolean (APIENTRYP PFNGLISENABLEDIPROC) (GLenum target, GLuint index);
typedef void (APIENTRYP PFNGLBEGINTRANSFORMFEEDBACKPROC) (GLenum primitiveMode);
typedef void (APIENTRYP PFNGLENDTRANSFORMFEEDBACKPROC) (void);
typedef void (APIENTRYP PFNGLBINDBUFFERRANGEPROC) (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLBINDBUFFERBASEPROC) (GLenum target, GLuint index, GLuint buffer);
typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKVARYINGSPROC) (GLuint program, GLsizei count, const GLchar* *varyings, GLenum bufferMode);
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKVARYINGPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
typedef void (APIENTRYP PFNGLCLAMPCOLORPROC) (GLenum target, GLenum clamp);
typedef void (APIENTRYP PFNGLBEGINCONDITIONALRENDERPROC) (GLuint id, GLenum mode);
typedef void (APIENTRYP PFNGLENDCONDITIONALRENDERPROC) (void);
typedef void (APIENTRYP PFNGLVERTEXATTRIBIPOINTERPROC) (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIIVPROC) (GLuint index, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIUIVPROC) (GLuint index, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1IPROC) (GLuint index, GLint x);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2IPROC) (GLuint index, GLint x, GLint y);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3IPROC) (GLuint index, GLint x, GLint y, GLint z);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4IPROC) (GLuint index, GLint x, GLint y, GLint z, GLint w);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1UIPROC) (GLuint index, GLuint x);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2UIPROC) (GLuint index, GLuint x, GLuint y);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3UIPROC) (GLuint index, GLuint x, GLuint y, GLuint z);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UIPROC) (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1IVPROC) (GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2IVPROC) (GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3IVPROC) (GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4IVPROC) (GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1UIVPROC) (GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2UIVPROC) (GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3UIVPROC) (GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UIVPROC) (GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4BVPROC) (GLuint index, const GLbyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4SVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UBVPROC) (GLuint index, const GLubyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4USVPROC) (GLuint index, const GLushort *v);
typedef void (APIENTRYP PFNGLGETUNIFORMUIVPROC) (GLuint program, GLint location, GLuint *params);
typedef void (APIENTRYP PFNGLBINDFRAGDATALOCATIONPROC) (GLuint program, GLuint color, const GLchar *name);
typedef GLint (APIENTRYP PFNGLGETFRAGDATALOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLUNIFORM1UIPROC) (GLint location, GLuint v0);
typedef void (APIENTRYP PFNGLUNIFORM2UIPROC) (GLint location, GLuint v0, GLuint v1);
typedef void (APIENTRYP PFNGLUNIFORM3UIPROC) (GLint location, GLuint v0, GLuint v1, GLuint v2);
typedef void (APIENTRYP PFNGLUNIFORM4UIPROC) (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
typedef void (APIENTRYP PFNGLUNIFORM1UIVPROC) (GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLUNIFORM2UIVPROC) (GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLUNIFORM3UIVPROC) (GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLUNIFORM4UIVPROC) (GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLTEXPARAMETERIIVPROC) (GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLTEXPARAMETERIUIVPROC) (GLenum target, GLenum pname, const GLuint *params);
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIUIVPROC) (GLenum target, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLCLEARBUFFERIVPROC) (GLenum buffer, GLint drawbuffer, const GLint *value);
typedef void (APIENTRYP PFNGLCLEARBUFFERUIVPROC) (GLenum buffer, GLint drawbuffer, const GLuint *value);
typedef void (APIENTRYP PFNGLCLEARBUFFERFVPROC) (GLenum buffer, GLint drawbuffer, const GLfloat *value);
typedef void (APIENTRYP PFNGLCLEARBUFFERFIPROC) (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
typedef const GLubyte * (APIENTRYP PFNGLGETSTRINGIPROC) (GLenum name, GLuint index);
typedef GLvoid* (APIENTRYP PFNGLMAPBUFFERRANGEPROC) (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef void (APIENTRYP PFNGLFLUSHMAPPEDBUFFERRANGEPROC) (GLenum target, GLintptr offset, GLsizeiptr length);
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint *arrays);
typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint *arrays);
typedef GLboolean (APIENTRYP PFNGLISVERTEXARRAYPROC) (GLuint array);
typedef GLboolean (APIENTRYP PFNGLISRENDERBUFFERPROC) (GLuint renderbuffer);
typedef void (APIENTRYP PFNGLBINDRENDERBUFFERPROC) (GLenum target, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLDELETERENDERBUFFERSPROC) (GLsizei n, const GLuint *renderbuffers);
typedef void (APIENTRYP PFNGLGENRENDERBUFFERSPROC) (GLsizei n, GLuint *renderbuffers);
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLGETRENDERBUFFERPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef GLboolean (APIENTRYP PFNGLISFRAMEBUFFERPROC) (GLuint framebuffer);
typedef void (APIENTRYP PFNGLBINDFRAMEBUFFERPROC) (GLenum target, GLuint framebuffer);
typedef void (APIENTRYP PFNGLDELETEFRAMEBUFFERSPROC) (GLsizei n, const GLuint *framebuffers);
typedef void (APIENTRYP PFNGLGENFRAMEBUFFERSPROC) (GLsizei n, GLuint *framebuffers);
typedef GLenum (APIENTRYP PFNGLCHECKFRAMEBUFFERSTATUSPROC) (GLenum target);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE1DPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE3DPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
typedef void (APIENTRYP PFNGLFRAMEBUFFERRENDERBUFFERPROC) (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) (GLenum target, GLenum attachment, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGENERATEMIPMAPPROC) (GLenum target);
typedef void (APIENTRYP PFNGLBLITFRAMEBUFFERPROC) (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURELAYERPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
O3D_GLAPI PFNGLCOLORMASKIPROC glColorMaski;
O3D_GLAPI PFNGLGETBOOLEANI_VPROC glGetBooleani_v;
O3D_GLAPI PFNGLGETINTEGERI_VPROC glGetIntegeri_v;
O3D_GLAPI PFNGLENABLEIPROC glEnablei;
O3D_GLAPI PFNGLDISABLEIPROC glDisablei;
O3D_GLAPI PFNGLISENABLEDIPROC glIsEnabledi;
O3D_GLAPI PFNGLBEGINTRANSFORMFEEDBACKPROC glBeginTransformFeedback;
O3D_GLAPI PFNGLENDTRANSFORMFEEDBACKPROC glEndTransformFeedback;
O3D_GLAPI PFNGLBINDBUFFERRANGEPROC glBindBufferRange;
O3D_GLAPI PFNGLBINDBUFFERBASEPROC glBindBufferBase;
O3D_GLAPI PFNGLTRANSFORMFEEDBACKVARYINGSPROC glTransformFeedbackVaryings;
O3D_GLAPI PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying;
O3D_GLAPI PFNGLCLAMPCOLORPROC glClampColor;
O3D_GLAPI PFNGLBEGINCONDITIONALRENDERPROC glBeginConditionalRender;
O3D_GLAPI PFNGLENDCONDITIONALRENDERPROC glEndConditionalRender;
O3D_GLAPI PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
O3D_GLAPI PFNGLGETVERTEXATTRIBIIVPROC glGetVertexAttribIiv;
O3D_GLAPI PFNGLGETVERTEXATTRIBIUIVPROC glGetVertexAttribIuiv;
O3D_GLAPI PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i;
O3D_GLAPI PFNGLVERTEXATTRIBI2IPROC glVertexAttribI2i;
O3D_GLAPI PFNGLVERTEXATTRIBI3IPROC glVertexAttribI3i;
O3D_GLAPI PFNGLVERTEXATTRIBI4IPROC glVertexAttribI4i;
O3D_GLAPI PFNGLVERTEXATTRIBI1UIPROC glVertexAttribI1ui;
O3D_GLAPI PFNGLVERTEXATTRIBI2UIPROC glVertexAttribI2ui;
O3D_GLAPI PFNGLVERTEXATTRIBI3UIPROC glVertexAttribI3ui;
O3D_GLAPI PFNGLVERTEXATTRIBI4UIPROC glVertexAttribI4ui;
O3D_GLAPI PFNGLVERTEXATTRIBI1IVPROC glVertexAttribI1iv;
O3D_GLAPI PFNGLVERTEXATTRIBI2IVPROC glVertexAttribI2iv;
O3D_GLAPI PFNGLVERTEXATTRIBI3IVPROC glVertexAttribI3iv;
O3D_GLAPI PFNGLVERTEXATTRIBI4IVPROC glVertexAttribI4iv;
O3D_GLAPI PFNGLVERTEXATTRIBI1UIVPROC glVertexAttribI1uiv;
O3D_GLAPI PFNGLVERTEXATTRIBI2UIVPROC glVertexAttribI2uiv;
O3D_GLAPI PFNGLVERTEXATTRIBI3UIVPROC glVertexAttribI3uiv;
O3D_GLAPI PFNGLVERTEXATTRIBI4UIVPROC glVertexAttribI4uiv;
O3D_GLAPI PFNGLVERTEXATTRIBI4BVPROC glVertexAttribI4bv;
O3D_GLAPI PFNGLVERTEXATTRIBI4SVPROC glVertexAttribI4sv;
O3D_GLAPI PFNGLVERTEXATTRIBI4UBVPROC glVertexAttribI4ubv;
O3D_GLAPI PFNGLVERTEXATTRIBI4USVPROC glVertexAttribI4usv;
O3D_GLAPI PFNGLGETUNIFORMUIVPROC glGetUniformuiv;
O3D_GLAPI PFNGLBINDFRAGDATALOCATIONPROC glBindFragDataLocation;
O3D_GLAPI PFNGLGETFRAGDATALOCATIONPROC glGetFragDataLocation;
O3D_GLAPI PFNGLUNIFORM1UIPROC glUniform1ui;
O3D_GLAPI PFNGLUNIFORM2UIPROC glUniform2ui;
O3D_GLAPI PFNGLUNIFORM3UIPROC glUniform3ui;
O3D_GLAPI PFNGLUNIFORM4UIPROC glUniform4ui;
O3D_GLAPI PFNGLUNIFORM1UIVPROC glUniform1uiv;
O3D_GLAPI PFNGLUNIFORM2UIVPROC glUniform2uiv;
O3D_GLAPI PFNGLUNIFORM3UIVPROC glUniform3uiv;
O3D_GLAPI PFNGLUNIFORM4UIPROC glUniform4uiv;
O3D_GLAPI PFNGLTEXPARAMETERIIVPROC glTexParameterIiv;
O3D_GLAPI PFNGLTEXPARAMETERIUIVPROC glTexParameterIuiv;
O3D_GLAPI PFNGLGETTEXPARAMETERIIVPROC glGetTexParameterIiv;
O3D_GLAPI PFNGLGETTEXPARAMETERIUIVPROC glGetTexParameterIuiv;
O3D_GLAPI PFNGLCLEARBUFFERIVPROC glClearBufferiv;
O3D_GLAPI PFNGLCLEARBUFFERUIVPROC glClearBufferuiv;
O3D_GLAPI PFNGLCLEARBUFFERFVPROC glClearBufferfv;
O3D_GLAPI PFNGLCLEARBUFFERFIPROC glClearBufferfi;
O3D_GLAPI PFNGLGETSTRINGIPROC glGetStringi;
O3D_GLAPI PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
O3D_GLAPI PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange;
O3D_GLAPI PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
O3D_GLAPI PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
O3D_GLAPI PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
O3D_GLAPI PFNGLISVERTEXARRAYPROC glIsVertexArray;
O3D_GLAPI PFNGLISRENDERBUFFERPROC glIsRenderbuffer;
O3D_GLAPI PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
O3D_GLAPI PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
O3D_GLAPI PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
O3D_GLAPI PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
O3D_GLAPI PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv;
O3D_GLAPI PFNGLISFRAMEBUFFERPROC glIsFramebuffer;
O3D_GLAPI PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
O3D_GLAPI PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
O3D_GLAPI PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
O3D_GLAPI PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
O3D_GLAPI PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D;
O3D_GLAPI PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
O3D_GLAPI PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D;
O3D_GLAPI PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
O3D_GLAPI PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
O3D_GLAPI PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
O3D_GLAPI PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
O3D_GLAPI PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
O3D_GLAPI PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer;
#endif // O3D_GL_VERSION_3_0

#ifndef O3D_GL_VERSION_3_1
#define O3D_GL_VERSION_3_1 1
typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDPROC) (GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDPROC) (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);
typedef void (APIENTRYP PFNGLTEXBUFFERPROC) (GLenum target, GLenum internalformat, GLuint buffer);
typedef void (APIENTRYP PFNGLPRIMITIVERESTARTINDEXPROC) (GLuint index);
typedef void (APIENTRYP PFNGLCOPYBUFFERSUBDATAPROC) (GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLGETUNIFORMINDICESPROC) (GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMSIVPROC) (GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMNAMEPROC) (GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformName);
typedef GLuint (APIENTRYP PFNGLGETUNIFORMBLOCKINDEXPROC) (GLuint program, const GLchar *uniformBlockName);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKIVPROC) (GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC) (GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName);
typedef void (APIENTRYP PFNGLUNIFORMBLOCKBINDINGPROC) (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
O3D_GLAPI PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
O3D_GLAPI PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
O3D_GLAPI PFNGLTEXBUFFERPROC glTexBuffer;
O3D_GLAPI PFNGLPRIMITIVERESTARTINDEXPROC glPrimitiveRestartIndex;
O3D_GLAPI PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData;
O3D_GLAPI PFNGLGETUNIFORMINDICESPROC glGetUniformIndices;
O3D_GLAPI PFNGLGETACTIVEUNIFORMSIVPROC glGetActiveUniformsiv;
O3D_GLAPI PFNGLGETACTIVEUNIFORMNAMEPROC glGetActiveUniformName;
O3D_GLAPI PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;
O3D_GLAPI PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv;
O3D_GLAPI PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName;
O3D_GLAPI PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;
#endif // O3D_GL_VERSION_3_1

#ifndef O3D_GL_VERSION_3_2
#define O3D_GL_VERSION_3_2 1
typedef void (APIENTRYP PFNGLDRAWELEMENTSBASEVERTEXPROC) (GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex);
typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC) (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC) (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC) (GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount, const GLint *basevertex);
typedef void (APIENTRYP PFNGLPROVOKINGVERTEXPROC) (GLenum mode);
typedef GLsync (APIENTRYP PFNGLFENCESYNCPROC) (GLenum condition, GLbitfield flags);
typedef GLboolean (APIENTRYP PFNGLISSYNCPROC) (GLsync sync);
typedef void (APIENTRYP PFNGLDELETESYNCPROC) (GLsync sync);
typedef GLenum (APIENTRYP PFNGLCLIENTWAITSYNCPROC) (GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef void (APIENTRYP PFNGLWAITSYNCPROC) (GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef void (APIENTRYP PFNGLGETINTEGER64VPROC) (GLenum pname, GLint64 *data);
typedef void (APIENTRYP PFNGLGETSYNCIVPROC) (GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values);
typedef void (APIENTRYP PFNGLGETINTEGER64I_VPROC) (GLenum target, GLuint index, GLint64 *data);
typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERI64VPROC) (GLenum target, GLenum pname, GLint64 *params);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLTEXIMAGE2DMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLTEXIMAGE3DMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLGETMULTISAMPLEFVPROC) (GLenum pname, GLuint index, GLfloat *val);
typedef void (APIENTRYP PFNGLSAMPLEMASKIPROC) (GLuint index, GLbitfield mask);
O3D_GLAPI PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex;
O3D_GLAPI PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex;
O3D_GLAPI PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex;
O3D_GLAPI PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glMultiDrawElementsBaseVertex;
O3D_GLAPI PFNGLPROVOKINGVERTEXPROC glProvokingVertex;
O3D_GLAPI PFNGLFENCESYNCPROC glFenceSync;
O3D_GLAPI PFNGLISSYNCPROC glIsSync;
O3D_GLAPI PFNGLDELETESYNCPROC glDeleteSync;
O3D_GLAPI PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
O3D_GLAPI PFNGLWAITSYNCPROC glWaitSync;
O3D_GLAPI PFNGLGETINTEGER64VPROC glGetInteger64v;
O3D_GLAPI PFNGLGETSYNCIVPROC glGetSynciv;
O3D_GLAPI PFNGLGETINTEGER64I_VPROC glGetInteger64i_v;
O3D_GLAPI PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v;
O3D_GLAPI PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
O3D_GLAPI PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
O3D_GLAPI PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample;
O3D_GLAPI PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv;
O3D_GLAPI PFNGLSAMPLEMASKIPROC glSampleMaski;
#endif // O3D_GL_VERSION_3_2

#ifndef O3D_GL_VERSION_3_3
#define O3D_GL_VERSION_3_3 1
typedef void (APIENTRYP PFNGLGENSAMPLERSPROC) (GLsizei count, GLuint *samplers);
typedef void (APIENTRYP PFNGLDELETESAMPLERSPROC) (GLsizei count, const GLuint *samplers);
typedef GLboolean (APIENTRYP PFNGLISSAMPLERPROC) (GLuint sampler);
typedef void (APIENTRYP PFNGLBINDSAMPLERPROC) (GLuint unit, GLuint sampler);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIPROC) (GLuint sampler, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIVPROC) (GLuint sampler, GLenum pname, const GLint *param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFPROC) (GLuint sampler, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFVPROC) (GLuint sampler, GLenum pname, const GLfloat *param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIIVPROC) (GLuint sampler, GLenum pname, const GLint *param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIUIVPROC) (GLuint sampler, GLenum pname, const GLuint *param);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIVPROC) (GLuint sampler, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIIVPROC) (GLuint sampler, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERFVPROC) (GLuint sampler, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIUIVPROC) (GLuint sampler, GLenum pname, GLuint *params);
O3D_GLAPI PFNGLGENSAMPLERSPROC glGenSamplers;
O3D_GLAPI PFNGLDELETESAMPLERSPROC glDeleteSamplers;
O3D_GLAPI PFNGLISSAMPLERPROC glIsSampler;
O3D_GLAPI PFNGLBINDSAMPLERPROC glBindSampler;
O3D_GLAPI PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri;
O3D_GLAPI PFNGLSAMPLERPARAMETERIVPROC glSamplerParameteriv;
O3D_GLAPI PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf;
O3D_GLAPI PFNGLSAMPLERPARAMETERFVPROC glSamplerParameterfv;
O3D_GLAPI PFNGLSAMPLERPARAMETERIIVPROC glSamplerParameterIiv;
O3D_GLAPI PFNGLSAMPLERPARAMETERIUIVPROC glSamplerParameterIuiv;
O3D_GLAPI PFNGLGETSAMPLERPARAMETERIVPROC glGetSamplerParameteriv;
O3D_GLAPI PFNGLGETSAMPLERPARAMETERIIVPROC glGetSamplerParameterIiv;
O3D_GLAPI PFNGLGETSAMPLERPARAMETERFVPROC glGetSamplerParameterfv;
O3D_GLAPI PFNGLGETSAMPLERPARAMETERIUIVPROC glGetSamplerParameterIuiv;
#endif // O3D_GL_VERSION_3_3

#ifndef O3D_GL_VERSION_4_0
#define O3D_GL_VERSION_4_0 1
typedef void (APIENTRYP PFNGLBLENDEQUATIONIPROC) (GLuint buf, GLenum mode);
typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEIPROC) (GLuint buf, GLenum modeRGB, GLenum modeAlpha);
typedef void (APIENTRYP PFNGLBLENDFUNCIPROC) (GLuint buf, GLenum src, GLenum dst);
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEIPROC) (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
O3D_GLAPI PFNGLBLENDEQUATIONIPROC glBlendEquationi;
O3D_GLAPI PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei;
O3D_GLAPI PFNGLBLENDFUNCIPROC glBlendFunci;
O3D_GLAPI PFNGLBLENDFUNCSEPARATEIPROC glBlendFuncSeparatei;
#endif // O3D_GL_VERSION_4_0

#ifndef O3D_GL_VERSION_4_1
#define O3D_GL_VERSION_4_1 1
typedef void (APIENTRYP PFNGLRELEASESHADERCOMPILERPROC) (void);
typedef void (APIENTRYP PFNGLSHADERBINARYPROC) (GLsizei count, const GLuint *shaders, GLenum binaryformat, const void *binary, GLsizei length);
typedef void (APIENTRYP PFNGLGETSHADERPRECISIONFORMATPROC) (GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision);
typedef void (APIENTRYP PFNGLDEPTHRANGEFPROC) (GLfloat n, GLfloat f);
typedef void (APIENTRYP PFNGLCLEARDEPTHFPROC) (GLfloat d);
typedef void (APIENTRYP PFNGLGETPROGRAMBINARYPROC) (GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary);
typedef void (APIENTRYP PFNGLPROGRAMBINARYPROC) (GLuint program, GLenum binaryFormat, const void *binary, GLsizei length);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETERIPROC) (GLuint program, GLenum pname, GLint value);
typedef void (APIENTRYP PFNGLUSEPROGRAMSTAGESPROC) (GLuint pipeline, GLbitfield stages, GLuint program);
typedef void (APIENTRYP PFNGLACTIVESHADERPROGRAMPROC) (GLuint pipeline, GLuint program);
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROGRAMVPROC) (GLenum type, GLsizei count, const GLchar *const*strings);
typedef void (APIENTRYP PFNGLBINDPROGRAMPIPELINEPROC) (GLuint pipeline);
typedef void (APIENTRYP PFNGLDELETEPROGRAMPIPELINESPROC) (GLsizei n, const GLuint *pipelines);
typedef void (APIENTRYP PFNGLGENPROGRAMPIPELINESPROC) (GLsizei n, GLuint *pipelines);
typedef GLboolean (APIENTRYP PFNGLISPROGRAMPIPELINEPROC) (GLuint pipeline);
typedef void (APIENTRYP PFNGLGETPROGRAMPIPELINEIVPROC) (GLuint pipeline, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1IPROC) (GLuint program, GLint location, GLint v0);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1IVPROC) (GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1FPROC) (GLuint program, GLint location, GLfloat v0);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1FVPROC) (GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1DPROC) (GLuint program, GLint location, GLdouble v0);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1DVPROC) (GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UIPROC) (GLuint program, GLint location, GLuint v0);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UIVPROC) (GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2IPROC) (GLuint program, GLint location, GLint v0, GLint v1);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2IVPROC) (GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2FPROC) (GLuint program, GLint location, GLfloat v0, GLfloat v1);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2FVPROC) (GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2DPROC) (GLuint program, GLint location, GLdouble v0, GLdouble v1);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2DVPROC) (GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UIPROC) (GLuint program, GLint location, GLuint v0, GLuint v1);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UIVPROC) (GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3IPROC) (GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3IVPROC) (GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3FPROC) (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3FVPROC) (GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3DPROC) (GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3DVPROC) (GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UIPROC) (GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UIVPROC) (GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4IPROC) (GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4IVPROC) (GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4FPROC) (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4FVPROC) (GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4DPROC) (GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4DVPROC) (GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UIPROC) (GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UIVPROC) (GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPIPELINEPROC) (GLuint pipeline);
typedef void (APIENTRYP PFNGLGETPROGRAMPIPELINEINFOLOGPROC) (GLuint pipeline, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL1DPROC) (GLuint index, GLdouble x);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL2DPROC) (GLuint index, GLdouble x, GLdouble y);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL3DPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL4DPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL1DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL2DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL3DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL4DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBLPOINTERPROC) (GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBLDVPROC) (GLuint index, GLenum pname, GLdouble *params);
typedef void (APIENTRYP PFNGLVIEWPORTARRAYVPROC) (GLuint first, GLsizei count, const GLfloat *v);
typedef void (APIENTRYP PFNGLVIEWPORTINDEXEDFPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h);
typedef void (APIENTRYP PFNGLVIEWPORTINDEXEDFVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLSCISSORARRAYVPROC) (GLuint first, GLsizei count, const GLint *v);
typedef void (APIENTRYP PFNGLSCISSORINDEXEDPROC) (GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLSCISSORINDEXEDVPROC) (GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLDEPTHRANGEARRAYVPROC) (GLuint first, GLsizei count, const GLdouble *v);
typedef void (APIENTRYP PFNGLDEPTHRANGEINDEXEDPROC) (GLuint index, GLdouble n, GLdouble f);
typedef void (APIENTRYP PFNGLGETFLOATI_VPROC) (GLenum target, GLuint index, GLfloat *data);
typedef void (APIENTRYP PFNGLGETDOUBLEI_VPROC) (GLenum target, GLuint index, GLdouble *data);

O3D_GLAPI PFNGLDEPTHRANGEFPROC glDepthRangef;
O3D_GLAPI PFNGLCLEARDEPTHFPROC glClearDepthf;
O3D_GLAPI PFNGLPROGRAMPARAMETERIPROC glProgramParameteri;
#endif // O3D_GL_VERSION_4_1

#ifndef O3D_GL_VERSION_4_2
#define O3D_GL_VERSION_4_2 1
typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC) (GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC) (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLuint baseinstance);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC) (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance);
typedef void (APIENTRYP PFNGLGETINTERNALFORMATIVPROC) (GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint *params);
typedef void (APIENTRYP PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC) (GLuint program, GLuint bufferIndex, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLBINDIMAGETEXTUREPROC) (GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
typedef void (APIENTRYP PFNGLMEMORYBARRIERPROC) (GLbitfield barriers);
typedef void (APIENTRYP PFNGLTEXSTORAGE1DPROC) (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
typedef void (APIENTRYP PFNGLTEXSTORAGE2DPROC) (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLTEXSTORAGE3DPROC) (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
typedef void (APIENTRYP PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC) (GLenum mode, GLuint id, GLsizei instancecount);
typedef void (APIENTRYP PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC) (GLenum mode, GLuint id, GLuint stream, GLsizei instancecount);
O3D_GLAPI PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC glDrawArraysInstancedBaseInstance;
O3D_GLAPI PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC glDrawElementsInstancedBaseInstance;
O3D_GLAPI PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glDrawElementsInstancedBaseVertexBaseInstance;
O3D_GLAPI PFNGLGETINTERNALFORMATIVPROC glGetInternalformativ;
O3D_GLAPI PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC glGetActiveAtomicCounterBufferiv;
O3D_GLAPI PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;
O3D_GLAPI PFNGLMEMORYBARRIERPROC glMemoryBarrier;
O3D_GLAPI PFNGLTEXSTORAGE1DPROC glTexStorage1D;
O3D_GLAPI PFNGLTEXSTORAGE2DPROC glTexStorage2D;
O3D_GLAPI PFNGLTEXSTORAGE3DPROC glTexStorage3D;
O3D_GLAPI PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC glDrawTransformFeedbackInstanced;
O3D_GLAPI PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC glDrawTransformFeedbackStreamInstanced;
#endif // GL_VERSION_4_2

#ifndef O3D_GL_VERSION_4_3
#define O3D_GL_VERSION_4_3 1
typedef void (APIENTRY  *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (APIENTRYP PFNGLCLEARBUFFERDATAPROC) (GLenum target, GLenum internalformat, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLCLEARBUFFERSUBDATAPROC) (GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLDISPATCHCOMPUTEPROC) (GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
typedef void (APIENTRYP PFNGLDISPATCHCOMPUTEINDIRECTPROC) (GLintptr indirect);
typedef void (APIENTRYP PFNGLCOPYIMAGESUBDATAPROC) (GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth);
typedef void (APIENTRYP PFNGLFRAMEBUFFERPARAMETERIPROC) (GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLGETFRAMEBUFFERPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETINTERNALFORMATI64VPROC) (GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint64 *params);
typedef void (APIENTRYP PFNGLINVALIDATETEXSUBIMAGEPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth);
typedef void (APIENTRYP PFNGLINVALIDATETEXIMAGEPROC) (GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLINVALIDATEBUFFERSUBDATAPROC) (GLuint buffer, GLintptr offset, GLsizeiptr length);
typedef void (APIENTRYP PFNGLINVALIDATEBUFFERDATAPROC) (GLuint buffer);
typedef void (APIENTRYP PFNGLINVALIDATEFRAMEBUFFERPROC) (GLenum target, GLsizei numAttachments, const GLenum *attachments);
typedef void (APIENTRYP PFNGLINVALIDATESUBFRAMEBUFFERPROC) (GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSINDIRECTPROC) (GLenum mode, const void *indirect, GLsizei drawcount, GLsizei stride);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSINDIRECTPROC) (GLenum mode, GLenum type, const void *indirect, GLsizei drawcount, GLsizei stride);
typedef void (APIENTRYP PFNGLGETPROGRAMINTERFACEIVPROC) (GLuint program, GLenum programInterface, GLenum pname, GLint *params);
typedef GLuint (APIENTRYP PFNGLGETPROGRAMRESOURCEINDEXPROC) (GLuint program, GLenum programInterface, const GLchar *name);
typedef void (APIENTRYP PFNGLGETPROGRAMRESOURCENAMEPROC) (GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name);
typedef void (APIENTRYP PFNGLGETPROGRAMRESOURCEIVPROC) (GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params);
typedef GLint (APIENTRYP PFNGLGETPROGRAMRESOURCELOCATIONPROC) (GLuint program, GLenum programInterface, const GLchar *name);
typedef GLint (APIENTRYP PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC) (GLuint program, GLenum programInterface, const GLchar *name);
typedef void (APIENTRYP PFNGLSHADERSTORAGEBLOCKBINDINGPROC) (GLuint program, GLuint storageBlockIndex, GLuint storageBlockBinding);
typedef void (APIENTRYP PFNGLTEXBUFFERRANGEPROC) (GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLTEXSTORAGE2DMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLTEXSTORAGE3DMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLTEXTUREVIEWPROC) (GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers);
typedef void (APIENTRYP PFNGLBINDVERTEXBUFFERPROC) (GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
typedef void (APIENTRYP PFNGLVERTEXATTRIBFORMATPROC) (GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLVERTEXATTRIBIFORMATPROC) (GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLVERTEXATTRIBLFORMATPROC) (GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLVERTEXATTRIBBINDINGPROC) (GLuint attribindex, GLuint bindingindex);
typedef void (APIENTRYP PFNGLVERTEXBINDINGDIVISORPROC) (GLuint bindingindex, GLuint divisor);
typedef void (APIENTRYP PFNGLDEBUGMESSAGECONTROLPROC) (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
typedef void (APIENTRYP PFNGLDEBUGMESSAGEINSERTPROC) (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf);
typedef void (APIENTRYP PFNGLDEBUGMESSAGECALLBACKPROC) (GLDEBUGPROC callback, const void *userParam);
typedef GLuint (APIENTRYP PFNGLGETDEBUGMESSAGELOGPROC) (GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);
typedef void (APIENTRYP PFNGLPUSHDEBUGGROUPPROC) (GLenum source, GLuint id, GLsizei length, const GLchar *message);
typedef void (APIENTRYP PFNGLPOPDEBUGGROUPPROC) (void);
typedef void (APIENTRYP PFNGLOBJECTLABELPROC) (GLenum identifier, GLuint name, GLsizei length, const GLchar *label);
typedef void (APIENTRYP PFNGLGETOBJECTLABELPROC) (GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label);
typedef void (APIENTRYP PFNGLOBJECTPTRLABELPROC) (const void *ptr, GLsizei length, const GLchar *label);
typedef void (APIENTRYP PFNGLGETOBJECTPTRLABELPROC) (const void *ptr, GLsizei bufSize, GLsizei *length, GLchar *label);
O3D_GLAPI PFNGLCLEARBUFFERDATAPROC glClearBufferData;
O3D_GLAPI PFNGLCLEARBUFFERSUBDATAPROC glClearBufferSubData;
O3D_GLAPI PFNGLDISPATCHCOMPUTEPROC glDispatchCompute;
O3D_GLAPI PFNGLDISPATCHCOMPUTEINDIRECTPROC glDispatchComputeIndirect;
O3D_GLAPI PFNGLCOPYIMAGESUBDATAPROC glCopyImageSubData;
O3D_GLAPI PFNGLFRAMEBUFFERPARAMETERIPROC glFramebufferParameteri;
O3D_GLAPI PFNGLGETFRAMEBUFFERPARAMETERIVPROC glGetFramebufferParameteriv;
O3D_GLAPI PFNGLGETINTERNALFORMATI64VPROC glGetInternalformati64v;
O3D_GLAPI PFNGLINVALIDATETEXSUBIMAGEPROC glInvalidateTexSubImage;
O3D_GLAPI PFNGLINVALIDATETEXIMAGEPROC glInvalidateTexImage;
O3D_GLAPI PFNGLINVALIDATEBUFFERSUBDATAPROC glInvalidateBufferSubData;
O3D_GLAPI PFNGLINVALIDATEBUFFERDATAPROC glInvalidateBufferData;
O3D_GLAPI PFNGLINVALIDATEFRAMEBUFFERPROC glInvalidateFramebuffer;
O3D_GLAPI PFNGLINVALIDATESUBFRAMEBUFFERPROC glInvalidateSubFramebuffer;
O3D_GLAPI PFNGLMULTIDRAWARRAYSINDIRECTPROC glMultiDrawArraysIndirect;
O3D_GLAPI PFNGLMULTIDRAWELEMENTSINDIRECTPROC glMultiDrawElementsIndirect;
O3D_GLAPI PFNGLGETPROGRAMINTERFACEIVPROC glGetProgramInterfaceiv;
O3D_GLAPI PFNGLGETPROGRAMRESOURCEINDEXPROC glGetProgramResourceIndex;
O3D_GLAPI PFNGLGETPROGRAMRESOURCENAMEPROC glGetProgramResourceName;
O3D_GLAPI PFNGLGETPROGRAMRESOURCEIVPROC glGetProgramResourceiv;
O3D_GLAPI PFNGLGETPROGRAMRESOURCELOCATIONPROC glGetProgramResourceLocation;
O3D_GLAPI PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC glGetProgramResourceLocationIndex;
O3D_GLAPI PFNGLSHADERSTORAGEBLOCKBINDINGPROC glShaderStorageBlockBinding;
O3D_GLAPI PFNGLTEXBUFFERRANGEPROC glTexBufferRange;
O3D_GLAPI PFNGLTEXSTORAGE2DMULTISAMPLEPROC glTexStorage2DMultisample;
O3D_GLAPI PFNGLTEXSTORAGE3DMULTISAMPLEPROC glTexStorage3DMultisample;
O3D_GLAPI PFNGLTEXTUREVIEWPROC glTextureView;
O3D_GLAPI PFNGLBINDVERTEXBUFFERPROC glBindVertexBuffer;
O3D_GLAPI PFNGLVERTEXATTRIBFORMATPROC glVertexAttribFormat;
O3D_GLAPI PFNGLVERTEXATTRIBIFORMATPROC glVertexAttribIFormat;
O3D_GLAPI PFNGLVERTEXATTRIBLFORMATPROC glVertexAttribLFormat;
O3D_GLAPI PFNGLVERTEXATTRIBBINDINGPROC glVertexAttribBinding;
O3D_GLAPI PFNGLVERTEXBINDINGDIVISORPROC glVertexBindingDivisor;
O3D_GLAPI PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl;
O3D_GLAPI PFNGLDEBUGMESSAGEINSERTPROC glDebugMessageInsert;
O3D_GLAPI PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
O3D_GLAPI PFNGLGETDEBUGMESSAGELOGPROC glGetDebugMessageLog;
O3D_GLAPI PFNGLPUSHDEBUGGROUPPROC glPushDebugGroup;
O3D_GLAPI PFNGLPOPDEBUGGROUPPROC glPopDebugGroup;
O3D_GLAPI PFNGLOBJECTLABELPROC glObjectLabel;
O3D_GLAPI PFNGLGETOBJECTLABELPROC glGetObjectLabel;
O3D_GLAPI PFNGLOBJECTPTRLABELPROC glObjectPtrLabel;
O3D_GLAPI PFNGLGETOBJECTPTRLABELPROC glGetObjectPtrLabel;
#endif // O3D_GL_VERSION_4_3

#ifndef O3D_GL_VERSION_4_4
#define O3D_GL_VERSION_4_4 1
typedef void (APIENTRYP PFNGLBUFFERSTORAGEPROC) (GLenum target, GLsizeiptr size, const void *data, GLbitfield flags);
typedef void (APIENTRYP PFNGLCLEARTEXIMAGEPROC) (GLuint texture, GLint level, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLCLEARTEXSUBIMAGEPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLBINDBUFFERSBASEPROC) (GLenum target, GLuint first, GLsizei count, const GLuint *buffers);
typedef void (APIENTRYP PFNGLBINDBUFFERSRANGEPROC) (GLenum target, GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizeiptr *sizes);
typedef void (APIENTRYP PFNGLBINDTEXTURESPROC) (GLuint first, GLsizei count, const GLuint *textures);
typedef void (APIENTRYP PFNGLBINDSAMPLERSPROC) (GLuint first, GLsizei count, const GLuint *samplers);
typedef void (APIENTRYP PFNGLBINDIMAGETEXTURESPROC) (GLuint first, GLsizei count, const GLuint *textures);
typedef void (APIENTRYP PFNGLBINDVERTEXBUFFERSPROC) (GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizei *strides);
O3D_GLAPI PFNGLBUFFERSTORAGEPROC glBufferStorage;
O3D_GLAPI PFNGLCLEARTEXIMAGEPROC glClearTexImage;
O3D_GLAPI PFNGLCLEARTEXSUBIMAGEPROC glClearTexSubImage;
O3D_GLAPI PFNGLBINDBUFFERSBASEPROC glBindBuffersBase;
O3D_GLAPI PFNGLBINDBUFFERSRANGEPROC glBindBuffersRange;
O3D_GLAPI PFNGLBINDTEXTURESPROC glBindTextures;
O3D_GLAPI PFNGLBINDSAMPLERSPROC glBindSamplers;
O3D_GLAPI PFNGLBINDIMAGETEXTURESPROC glBindImageTextures;
O3D_GLAPI PFNGLBINDVERTEXBUFFERSPROC glBindVertexBuffers;
#endif // O3D_GL_VERSION_4_4

#ifndef O3D_GL_VERSION_4_5
#define O3D_GL_VERSION_4_5 1
typedef void (APIENTRYP PFNGLCLIPCONTROLPROC) (GLenum origin, GLenum depth);
typedef void (APIENTRYP PFNGLCREATETRANSFORMFEEDBACKSPROC) (GLsizei n, GLuint *ids);
typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC) (GLuint xfb, GLuint index, GLuint buffer);
typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC) (GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKIVPROC) (GLuint xfb, GLenum pname, GLint *param);
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKI_VPROC) (GLuint xfb, GLenum pname, GLuint index, GLint *param);
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKI64_VPROC) (GLuint xfb, GLenum pname, GLuint index, GLint64 *param);

typedef void (APIENTRYP PFNGLCREATEBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRYP PFNGLNAMEDBUFFERSTORAGEPROC) (GLuint buffer, GLsizeiptr size, const void *data, GLbitfield flags);
typedef void (APIENTRYP PFNGLNAMEDBUFFERDATAPROC) (GLuint buffer, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRYP PFNGLNAMEDBUFFERSUBDATAPROC) (GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data);
typedef void (APIENTRYP PFNGLCOPYNAMEDBUFFERSUBDATAPROC) (GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLCLEARNAMEDBUFFERDATAPROC) (GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLCLEARNAMEDBUFFERSUBDATAPROC) (GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data);
typedef void *(APIENTRYP PFNGLMAPNAMEDBUFFERPROC) (GLuint buffer, GLenum access);
typedef void *(APIENTRYP PFNGLMAPNAMEDBUFFERRANGEPROC) (GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef GLboolean (APIENTRYP PFNGLUNMAPNAMEDBUFFERPROC) (GLuint buffer);
typedef void (APIENTRYP PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC) (GLuint buffer, GLintptr offset, GLsizeiptr length);
typedef void (APIENTRYP PFNGLGETNAMEDBUFFERPARAMETERIVPROC) (GLuint buffer, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETNAMEDBUFFERPARAMETERI64VPROC) (GLuint buffer, GLenum pname, GLint64 *params);
typedef void (APIENTRYP PFNGLGETNAMEDBUFFERPOINTERVPROC) (GLuint buffer, GLenum pname, void **params);
typedef void (APIENTRYP PFNGLGETNAMEDBUFFERSUBDATAPROC) (GLuint buffer, GLintptr offset, GLsizeiptr size, void *data);
typedef void (APIENTRYP PFNGLCREATEFRAMEBUFFERSPROC) (GLsizei n, GLuint *framebuffers);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC) (GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC) (GLuint framebuffer, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERTEXTUREPROC) (GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC) (GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC) (GLuint framebuffer, GLenum buf);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC) (GLuint framebuffer, GLsizei n, const GLenum *bufs);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC) (GLuint framebuffer, GLenum src);
typedef void (APIENTRYP PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC) (GLuint framebuffer, GLsizei numAttachments, const GLenum *attachments);
typedef void (APIENTRYP PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC) (GLuint framebuffer, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLCLEARNAMEDFRAMEBUFFERIVPROC) (GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint *value);
typedef void (APIENTRYP PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC) (GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint *value);
typedef void (APIENTRYP PFNGLCLEARNAMEDFRAMEBUFFERFVPROC) (GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat *value);
typedef void (APIENTRYP PFNGLCLEARNAMEDFRAMEBUFFERFIPROC) (GLuint framebuffer, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
typedef void (APIENTRYP PFNGLBLITNAMEDFRAMEBUFFERPROC) (GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
typedef GLenum (APIENTRYP PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC) (GLuint framebuffer, GLenum target);
typedef void (APIENTRYP PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC) (GLuint framebuffer, GLenum pname, GLint *param);
typedef void (APIENTRYP PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC) (GLuint framebuffer, GLenum attachment, GLenum pname, GLint *params);

typedef void (APIENTRYP PFNGLCREATERENDERBUFFERSPROC) (GLsizei n, GLuint *renderbuffers);
typedef void (APIENTRYP PFNGLNAMEDRENDERBUFFERSTORAGEPROC) (GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC) (GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC) (GLuint renderbuffer, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLCREATETEXTURESPROC) (GLenum target, GLsizei n, GLuint *textures);
typedef void (APIENTRYP PFNGLTEXTUREBUFFERPROC) (GLuint texture, GLenum internalformat, GLuint buffer);
typedef void (APIENTRYP PFNGLTEXTUREBUFFERRANGEPROC) (GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE1DPROC) (GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE2DPROC) (GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE3DPROC) (GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC) (GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC) (GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLTEXTURESUBIMAGE1DPROC) (GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLTEXTURESUBIMAGE2DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLTEXTURESUBIMAGE3DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC) (GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLCOPYTEXTURESUBIMAGE1DPROC) (GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
typedef void (APIENTRYP PFNGLCOPYTEXTURESUBIMAGE2DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLCOPYTEXTURESUBIMAGE3DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERFPROC) (GLuint texture, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERFVPROC) (GLuint texture, GLenum pname, const GLfloat *param);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERIPROC) (GLuint texture, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERIIVPROC) (GLuint texture, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERIUIVPROC) (GLuint texture, GLenum pname, const GLuint *params);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERIVPROC) (GLuint texture, GLenum pname, const GLint *param);
typedef void (APIENTRYP PFNGLGENERATETEXTUREMIPMAPPROC) (GLuint texture);
typedef void (APIENTRYP PFNGLBINDTEXTUREUNITPROC) (GLuint unit, GLuint texture);
typedef void (APIENTRYP PFNGLGETTEXTUREIMAGEPROC) (GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC) (GLuint texture, GLint level, GLsizei bufSize, void *pixels);
typedef void (APIENTRYP PFNGLGETTEXTURELEVELPARAMETERFVPROC) (GLuint texture, GLint level, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETTEXTURELEVELPARAMETERIVPROC) (GLuint texture, GLint level, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETTEXTUREPARAMETERFVPROC) (GLuint texture, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETTEXTUREPARAMETERIIVPROC) (GLuint texture, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETTEXTUREPARAMETERIUIVPROC) (GLuint texture, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLGETTEXTUREPARAMETERIVPROC) (GLuint texture, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLCREATEVERTEXARRAYSPROC) (GLsizei n, GLuint *arrays);
typedef void (APIENTRYP PFNGLDISABLEVERTEXARRAYATTRIBPROC) (GLuint vaobj, GLuint index);
typedef void (APIENTRYP PFNGLENABLEVERTEXARRAYATTRIBPROC) (GLuint vaobj, GLuint index);
typedef void (APIENTRYP PFNGLVERTEXARRAYELEMENTBUFFERPROC) (GLuint vaobj, GLuint buffer);
typedef void (APIENTRYP PFNGLVERTEXARRAYVERTEXBUFFERPROC) (GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
typedef void (APIENTRYP PFNGLVERTEXARRAYVERTEXBUFFERSPROC) (GLuint vaobj, GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizei *strides);
typedef void (APIENTRYP PFNGLVERTEXARRAYATTRIBBINDINGPROC) (GLuint vaobj, GLuint attribindex, GLuint bindingindex);
typedef void (APIENTRYP PFNGLVERTEXARRAYATTRIBFORMATPROC) (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLVERTEXARRAYATTRIBIFORMATPROC) (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLVERTEXARRAYATTRIBLFORMATPROC) (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLVERTEXARRAYBINDINGDIVISORPROC) (GLuint vaobj, GLuint bindingindex, GLuint divisor);
typedef void (APIENTRYP PFNGLGETVERTEXARRAYIVPROC) (GLuint vaobj, GLenum pname, GLint *param);
typedef void (APIENTRYP PFNGLGETVERTEXARRAYINDEXEDIVPROC) (GLuint vaobj, GLuint index, GLenum pname, GLint *param);
typedef void (APIENTRYP PFNGLGETVERTEXARRAYINDEXED64IVPROC) (GLuint vaobj, GLuint index, GLenum pname, GLint64 *param);
typedef void (APIENTRYP PFNGLCREATESAMPLERSPROC) (GLsizei n, GLuint *samplers);
typedef void (APIENTRYP PFNGLCREATEPROGRAMPIPELINESPROC) (GLsizei n, GLuint *pipelines);
typedef void (APIENTRYP PFNGLCREATEQUERIESPROC) (GLenum target, GLsizei n, GLuint *ids);
typedef void (APIENTRYP PFNGLGETQUERYBUFFEROBJECTI64VPROC) (GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
typedef void (APIENTRYP PFNGLGETQUERYBUFFEROBJECTIVPROC) (GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
typedef void (APIENTRYP PFNGLGETQUERYBUFFEROBJECTUI64VPROC) (GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
typedef void (APIENTRYP PFNGLGETQUERYBUFFEROBJECTUIVPROC) (GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
typedef void (APIENTRYP PFNGLMEMORYBARRIERBYREGIONPROC) (GLbitfield barriers);
typedef void (APIENTRYP PFNGLGETTEXTURESUBIMAGEPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei bufSize, void *pixels);
typedef GLenum (APIENTRYP PFNGLGETGRAPHICSRESETSTATUSPROC) (void);
typedef void (APIENTRYP PFNGLGETNCOMPRESSEDTEXIMAGEPROC) (GLenum target, GLint lod, GLsizei bufSize, void *pixels);
typedef void (APIENTRYP PFNGLGETNTEXIMAGEPROC) (GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
typedef void (APIENTRYP PFNGLGETNUNIFORMDVPROC) (GLuint program, GLint location, GLsizei bufSize, GLdouble *params);
typedef void (APIENTRYP PFNGLGETNUNIFORMFVPROC) (GLuint program, GLint location, GLsizei bufSize, GLfloat *params);
typedef void (APIENTRYP PFNGLGETNUNIFORMIVPROC) (GLuint program, GLint location, GLsizei bufSize, GLint *params);
typedef void (APIENTRYP PFNGLGETNUNIFORMUIVPROC) (GLuint program, GLint location, GLsizei bufSize, GLuint *params);
typedef void (APIENTRYP PFNGLREADNPIXELSPROC) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void *data);
typedef void (APIENTRYP PFNGLGETNMAPDVPROC) (GLenum target, GLenum query, GLsizei bufSize, GLdouble *v);
typedef void (APIENTRYP PFNGLGETNMAPFVPROC) (GLenum target, GLenum query, GLsizei bufSize, GLfloat *v);
typedef void (APIENTRYP PFNGLGETNMAPIVPROC) (GLenum target, GLenum query, GLsizei bufSize, GLint *v);
typedef void (APIENTRYP PFNGLGETNPIXELMAPFVPROC) (GLenum map, GLsizei bufSize, GLfloat *values);
typedef void (APIENTRYP PFNGLGETNPIXELMAPUIVPROC) (GLenum map, GLsizei bufSize, GLuint *values);
typedef void (APIENTRYP PFNGLGETNPIXELMAPUSVPROC) (GLenum map, GLsizei bufSize, GLushort *values);
typedef void (APIENTRYP PFNGLGETNPOLYGONSTIPPLEPROC) (GLsizei bufSize, GLubyte *pattern);
typedef void (APIENTRYP PFNGLGETNCOLORTABLEPROC) (GLenum target, GLenum format, GLenum type, GLsizei bufSize, void *table);
typedef void (APIENTRYP PFNGLGETNCONVOLUTIONFILTERPROC) (GLenum target, GLenum format, GLenum type, GLsizei bufSize, void *image);
typedef void (APIENTRYP PFNGLGETNSEPARABLEFILTERPROC) (GLenum target, GLenum format, GLenum type, GLsizei rowBufSize, void *row, GLsizei columnBufSize, void *column, void *span);
typedef void (APIENTRYP PFNGLGETNHISTOGRAMPROC) (GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, void *values);
typedef void (APIENTRYP PFNGLGETNMINMAXPROC) (GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, void *values);
typedef void (APIENTRYP PFNGLTEXTUREBARRIERPROC) (void);
// @todo
//O3D_GLAPI glClipControl;
//O3D_GLAPI glCreateTransformFeedbacks;
//O3D_GLAPI glTransformFeedbackBufferBase;
//O3D_GLAPI glTransformFeedbackBufferRange;
//O3D_GLAPI glGetTransformFeedbackiv;
//O3D_GLAPI glGetTransformFeedbacki_v;
//O3D_GLAPI glGetTransformFeedbacki64_v;
O3D_GLAPI PFNGLCREATEBUFFERSPROC glCreateBuffers;
O3D_GLAPI PFNGLNAMEDBUFFERSTORAGEPROC glNamedBufferStorage;
O3D_GLAPI PFNGLNAMEDBUFFERDATAPROC glNamedBufferData;
O3D_GLAPI PFNGLNAMEDBUFFERSUBDATAPROC glNamedBufferSubData;
O3D_GLAPI PFNGLCOPYNAMEDBUFFERSUBDATAPROC glCopyNamedBufferSubData;
O3D_GLAPI PFNGLCLEARNAMEDBUFFERDATAPROC glClearNamedBufferData;
O3D_GLAPI PFNGLCLEARNAMEDBUFFERSUBDATAPROC glClearNamedBufferSubData;
O3D_GLAPI PFNGLMAPNAMEDBUFFERPROC glMapNamedBuffer;
O3D_GLAPI PFNGLMAPNAMEDBUFFERRANGEPROC glMapNamedBufferRange;
O3D_GLAPI PFNGLUNMAPNAMEDBUFFERPROC glUnmapNamedBuffer;
O3D_GLAPI PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC glFlushMappedNamedBufferRange;
O3D_GLAPI PFNGLGETNAMEDBUFFERPARAMETERIVPROC glGetNamedBufferParameteriv;
O3D_GLAPI PFNGLGETNAMEDBUFFERPARAMETERI64VPROC glGetNamedBufferParameteri64v;
O3D_GLAPI PFNGLGETNAMEDBUFFERPOINTERVPROC glGetNamedBufferPointerv;
O3D_GLAPI PFNGLGETNAMEDBUFFERSUBDATAPROC glGetNamedBufferSubData;
O3D_GLAPI PFNGLCREATEFRAMEBUFFERSPROC glCreateFramebuffers;
O3D_GLAPI PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC glNamedFramebufferRenderbuffer;
O3D_GLAPI PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC glNamedFramebufferParameteri;
O3D_GLAPI PFNGLNAMEDFRAMEBUFFERTEXTUREPROC glNamedFramebufferTexture;
O3D_GLAPI PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC glNamedFramebufferTextureLayer;
O3D_GLAPI PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC glNamedFramebufferDrawBuffer;
O3D_GLAPI PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC glNamedFramebufferDrawBuffers;
O3D_GLAPI PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC glNamedFramebufferReadBuffer;
O3D_GLAPI PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC glInvalidateNamedFramebufferData;
O3D_GLAPI PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC glInvalidateNamedFramebufferSubData;
O3D_GLAPI PFNGLCLEARNAMEDFRAMEBUFFERIVPROC glClearNamedFramebufferiv;
O3D_GLAPI PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC glClearNamedFramebufferuiv;
O3D_GLAPI PFNGLCLEARNAMEDFRAMEBUFFERFVPROC glClearNamedFramebufferfv;
O3D_GLAPI PFNGLCLEARNAMEDFRAMEBUFFERFIPROC glClearNamedFramebufferfi;
O3D_GLAPI PFNGLBLITNAMEDFRAMEBUFFERPROC glBlitNamedFramebuffer;
O3D_GLAPI PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC glCheckNamedFramebufferStatus;
O3D_GLAPI PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC glGetNamedFramebufferParameteriv;
O3D_GLAPI PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetNamedFramebufferAttachmentParameteriv;
// @todo
//O3D_GLAPI glCreateRenderbuffers;
//O3D_GLAPI glNamedRenderbufferStorage;
//O3D_GLAPI glNamedRenderbufferStorageMultisample;
//O3D_GLAPI glGetNamedRenderbufferParameteriv;
//O3D_GLAPI glCreateTextures;
//O3D_GLAPI glTextureBuffer;
//O3D_GLAPI glTextureBufferRange;
//O3D_GLAPI glTextureStorage1D;
//O3D_GLAPI glTextureStorage2D;
//O3D_GLAPI glTextureStorage3D;
//O3D_GLAPI glTextureStorage2DMultisample;
//O3D_GLAPI glTextureStorage3DMultisample;
//O3D_GLAPI glTextureSubImage1D;
//O3D_GLAPI glTextureSubImage2D;
//O3D_GLAPI glTextureSubImage3D;
//O3D_GLAPI glCompressedTextureSubImage1D;
//O3D_GLAPI glCompressedTextureSubImage2D;
//O3D_GLAPI glCompressedTextureSubImage3D;
//O3D_GLAPI glCopyTextureSubImage1D;
//O3D_GLAPI glCopyTextureSubImage2D;
//O3D_GLAPI glCopyTextureSubImage3D;
//O3D_GLAPI glTextureParameterf;
//O3D_GLAPI glTextureParameterfv;
//O3D_GLAPI glTextureParameteri;
//O3D_GLAPI glTextureParameterIiv;
//O3D_GLAPI glTextureParameterIuiv;
//O3D_GLAPI glTextureParameteriv;
//O3D_GLAPI glGenerateTextureMipmap;
//O3D_GLAPI glBindTextureUnit;
//O3D_GLAPI glGetTextureImage;
//O3D_GLAPI glGetCompressedTextureImage;
//O3D_GLAPI glGetTextureLevelParameterfv;
//O3D_GLAPI glGetTextureLevelParameteriv;
//O3D_GLAPI glGetTextureParameterfv;
//O3D_GLAPI glGetTextureParameterIiv;
//O3D_GLAPI glGetTextureParameterIuiv;
//O3D_GLAPI glGetTextureParameteriv;
//O3D_GLAPI glCreateVertexArrays;
//O3D_GLAPI glDisableVertexArrayAttrib;
//O3D_GLAPI glEnableVertexArrayAttrib;
//O3D_GLAPI glVertexArrayElementBuffer;
//O3D_GLAPI glVertexArrayVertexBuffer;
//O3D_GLAPI glVertexArrayVertexBuffers;
//O3D_GLAPI glVertexArrayAttribBinding;
//O3D_GLAPI glVertexArrayAttribFormat;
//O3D_GLAPI glVertexArrayAttribIFormat;
//O3D_GLAPI glVertexArrayAttribLFormat;
//O3D_GLAPI glVertexArrayBindingDivisor;
//O3D_GLAPI glGetVertexArrayiv;
//O3D_GLAPI glGetVertexArrayIndexediv;
//O3D_GLAPI glGetVertexArrayIndexed64iv;
//O3D_GLAPI glCreateSamplers;
//O3D_GLAPI glCreateProgramPipelines;
//O3D_GLAPI glCreateQueries;
//O3D_GLAPI glGetQueryBufferObjecti64v;
//O3D_GLAPI glGetQueryBufferObjectiv;
//O3D_GLAPI glGetQueryBufferObjectui64v;
//O3D_GLAPI glGetQueryBufferObjectuiv;
//O3D_GLAPI glMemoryBarrierByRegion;
O3D_GLAPI PFNGLGETTEXTURESUBIMAGEPROC glGetTextureSubImage;
//O3D_GLAPI glGetCompressedTextureSubImage;
//O3D_GLAPI GLenum APIENTRY glGetGraphicsResetStatus;
//O3D_GLAPI glGetnCompressedTexImage;
//O3D_GLAPI glGetnTexImage;
//O3D_GLAPI glGetnUniformdv;
//O3D_GLAPI glGetnUniformfv;
//O3D_GLAPI glGetnUniformiv;
//O3D_GLAPI glGetnUniformuiv;
//O3D_GLAPI glReadnPixels;
//O3D_GLAPI glGetnMapdv;
//O3D_GLAPI glGetnMapfv;
//O3D_GLAPI glGetnMapiv;
//O3D_GLAPI glGetnPixelMapfv;
//O3D_GLAPI glGetnPixelMapuiv;
//O3D_GLAPI glGetnPixelMapusv;
//O3D_GLAPI glGetnPolygonStipple;
//O3D_GLAPI glGetnColorTable;
//O3D_GLAPI glGetnConvolutionFilter;
//O3D_GLAPI glGetnSeparableFilter;
//O3D_GLAPI glGetnHistogram;
//O3D_GLAPI glGetnMinmax;
//O3D_GLAPI glTextureBarrier;
#endif // O3D_GL_VERSION_4_5

#ifndef O3D_GL_VERSION_4_6
#define O3D_GL_VERSION_4_6 1
typedef void (APIENTRYP PFNGLSPECIALIZESHADERPROC) (GLuint shader, const GLchar *pEntryPoint, GLuint numSpecializationConstants, const GLuint *pConstantIndex, const GLuint *pConstantValue);
typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSINDIRECTCOUNTPROC) (GLenum mode, const void *indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSINDIRECTCOUNTPROC) (GLenum mode, GLenum type, const void *indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride);
typedef void (APIENTRYP PFNGLPOLYGONOFFSETCLAMPPROC) (GLfloat factor, GLfloat units, GLfloat clamp);
O3D_GLAPI PFNGLSPECIALIZESHADERPROC glSpecializeShader;
O3D_GLAPI PFNGLMULTIDRAWARRAYSINDIRECTCOUNTPROC glMultiDrawArraysIndirectCount;
O3D_GLAPI PFNGLMULTIDRAWELEMENTSINDIRECTCOUNTPROC glMultiDrawElementsIndirectCount;
O3D_GLAPI PFNGLPOLYGONOFFSETCLAMPPROC glPolygonOffsetClamp;
#endif // O3D_GL_VERSION_4_6

#ifndef O3D_GL_ARB_ES3_2_compatibility
#define O3D_GL_ARB_ES3_2_compatibility 1
typedef void (APIENTRYP PFNGLPRIMITIVEBOUNDINGBOXARBPROC) (GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat minW, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLfloat maxW);
O3D_GLAPI PFNGLPRIMITIVEBOUNDINGBOXARBPROC glPrimitiveBoundingBox;
#endif // O3D_GL_ARB_ES3_2_compatibility

} // namespace o3d

#endif // _O3D_GLEXTENSIONMANAGER_H
