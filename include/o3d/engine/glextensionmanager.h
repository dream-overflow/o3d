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

	static void getExtFunctions();
};

//
// OpenGL extensions
//

//#ifdef __cplusplus
//extern "C" {
//#endif

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifndef O3D_GL_PROTOTYPES
  #ifndef O3D_GLAPI
    #define O3D_GLAPI O3D_API extern
  #endif
#else
  #ifdef O3D_WINDOWS
    #ifndef O3D_GLAPI
        // On Windows we have to import function using function pointers
        #define O3D_GLAPI extern O3D_API
    #endif
  #else
    #ifndef O3D_GLAPI
        #define O3D_GLAPI extern
    #endif
#endif

#endif

/* Base GL types */

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

//
// GL_ARB_sync
//

//#ifndef GL_ARB_sync
//#define GL_ARB_sync 1
#ifndef O3D_GL_ARB_sync
#define O3D_GL_ARB_sync 1
typedef o3d::Int64 GLint64;
typedef o3d::UInt64 GLuint64;
typedef struct __GLsync *GLsync;
#endif

//
// GL_VERSION_1_0
//

//#ifndef GL_VERSION_1_0
//#define GL_VERSION_1_0 1
#ifndef O3D_GL_VERSION_1_0
#define O3D_GL_VERSION_1_0 1

#ifndef O3D_GL_PROTOTYPES
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
O3D_GLAPI PFNGLGETINTEGERVPROC glGetIntegerv;
O3D_GLAPI PFNGLGETSTRINGPROC glGetString;
O3D_GLAPI PFNGLGETTEXIMAGEPROC glGetTexImage;
O3D_GLAPI PFNGLGETTEXPARAMETERFVPROC glGetTexParameterfv;
O3D_GLAPI PFNGLGETTEXPARAMETERIVPROC glGetTexParameteriv;
O3D_GLAPI PFNGLGETTEXLEVELPARAMETERFVPROC glGetTexLevelParameterfv;
O3D_GLAPI PFNGLGETTEXLEVELPARAMETERIVPROC glGetTexLevelParameteriv;
O3D_GLAPI PFNGLISENABLEDPROC glIsEnabled;
O3D_GLAPI PFNGLDEPTHRANGEPROC glDepthRange;
O3D_GLAPI PFNGLVIEWPORTPROC glViewport;
#else
O3D_GLAPI void APIENTRY glCullFace (GLenum mode);
O3D_GLAPI void APIENTRY glFrontFace (GLenum mode);
O3D_GLAPI void APIENTRY glHint (GLenum target, GLenum mode);
O3D_GLAPI void APIENTRY glLineWidth (GLfloat width);
O3D_GLAPI void APIENTRY glPointSize (GLfloat size);
O3D_GLAPI void APIENTRY glPolygonMode (GLenum face, GLenum mode);
O3D_GLAPI void APIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height);
O3D_GLAPI void APIENTRY glTexParameterf (GLenum target, GLenum pname, GLfloat param);
O3D_GLAPI void APIENTRY glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params);
O3D_GLAPI void APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param);
O3D_GLAPI void APIENTRY glTexParameteriv (GLenum target, GLenum pname, const GLint *params);
O3D_GLAPI void APIENTRY glTexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
O3D_GLAPI void APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
O3D_GLAPI void APIENTRY glDrawBuffer (GLenum mode);
O3D_GLAPI void APIENTRY glClear (GLbitfield mask);
O3D_GLAPI void APIENTRY glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
O3D_GLAPI void APIENTRY glClearStencil (GLint s);
O3D_GLAPI void APIENTRY glClearDepth (GLclampd depth);
O3D_GLAPI void APIENTRY glStencilMask (GLuint mask);
O3D_GLAPI void APIENTRY glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
O3D_GLAPI void APIENTRY glDepthMask (GLboolean flag);
O3D_GLAPI void APIENTRY glDisable (GLenum cap);
O3D_GLAPI void APIENTRY glEnable (GLenum cap);
O3D_GLAPI void APIENTRY glFinish (void);
O3D_GLAPI void APIENTRY glFlush (void);
O3D_GLAPI void APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor);
O3D_GLAPI void APIENTRY glLogicOp (GLenum opcode);
O3D_GLAPI void APIENTRY glStencilFunc (GLenum func, GLint ref, GLuint mask);
O3D_GLAPI void APIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);
O3D_GLAPI void APIENTRY glDepthFunc (GLenum func);
O3D_GLAPI void APIENTRY glPixelStoref (GLenum pname, GLfloat param);
O3D_GLAPI void APIENTRY glPixelStorei (GLenum pname, GLint param);
O3D_GLAPI void APIENTRY glReadBuffer (GLenum mode);
O3D_GLAPI void APIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
O3D_GLAPI void APIENTRY glGetBooleanv (GLenum pname, GLboolean *params);
O3D_GLAPI void APIENTRY glGetDoublev (GLenum pname, GLdouble *params);
O3D_GLAPI GLenum APIENTRY glGetError (void);
O3D_GLAPI void APIENTRY glGetFloatv (GLenum pname, GLfloat *params);
O3D_GLAPI void APIENTRY glGetIntegerv (GLenum pname, GLint *params);
O3D_GLAPI const GLubyte * APIENTRY glGetString (GLenum name);
O3D_GLAPI void APIENTRY glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
O3D_GLAPI void APIENTRY glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params);
O3D_GLAPI void APIENTRY glGetTexParameteriv (GLenum target, GLenum pname, GLint *params);
O3D_GLAPI void APIENTRY glGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat *params);
O3D_GLAPI void APIENTRY glGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params);
O3D_GLAPI GLboolean APIENTRY glIsEnabled (GLenum cap);
O3D_GLAPI void APIENTRY glDepthRange (GLclampd near, GLclampd far);
O3D_GLAPI void APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height);
#endif // O3D_GL_PROTOTYPES
#endif // GL_VERSION_1_0

//
// GL_VERSION_1_1
//

//#ifndef GL_VERSION_1_1
//#define GL_VERSION_1_1 1
#ifndef O3D_GL_VERSION_1_1
#define O3D_GL_VERSION_1_1 1

#ifndef O3D_GL_PROTOTYPES
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
#else
O3D_GLAPI void APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count);
O3D_GLAPI void APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
O3D_GLAPI void APIENTRY glGetPointerv (GLenum pname, GLvoid* *params);
O3D_GLAPI void APIENTRY glPolygonOffset (GLfloat factor, GLfloat units);
O3D_GLAPI void APIENTRY glCopyTexImage1D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
O3D_GLAPI void APIENTRY glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
O3D_GLAPI void APIENTRY glCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
O3D_GLAPI void APIENTRY glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
O3D_GLAPI void APIENTRY glTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
O3D_GLAPI void APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
O3D_GLAPI void APIENTRY glBindTexture (GLenum target, GLuint texture);
O3D_GLAPI void APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures);
O3D_GLAPI void APIENTRY glGenTextures (GLsizei n, GLuint *textures);
O3D_GLAPI GLboolean APIENTRY glIsTexture (GLuint texture);
#endif // GL_VERSION_1_1
#endif // O3D_GL_PROTOTYPES

//
// GL_VERSION_1_2
//

//#ifndef GL_VERSION_1_2
//#define GL_VERSION_1_2 1
#ifndef O3D_GL_VERSION_1_2
#define O3D_GL_VERSION_1_2 1

// draw range element
#ifndef O3D_GL_PROTOTYPES
typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSPROC) (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);

O3D_GLAPI PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;
#else
O3D_GLAPI void APIENTRY glDrawRangeElements (GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *);
#endif // O3D_GL_PROTOTYPES

// texture_3d
#ifndef O3D_GL_PROTOTYPES
typedef void (APIENTRYP PFNGLTEXIMAGE3DPROC) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels);

O3D_GLAPI PFNGLTEXIMAGE3DPROC    glTexImage3D;
O3D_GLAPI PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
#else
O3D_GLAPI void APIENTRY glTexImage3D (GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *);
O3D_GLAPI void APIENTRY glTexSubImage3D (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
#endif // O3D_GL_PROTOTYPES
#endif // GL_VERSION_1_2

//
// GL_VERSION_1_3
//

//#ifndef GL_VERSION_1_3
//#define GL_VERSION_1_3 1
#ifndef O3D_GL_VERSION_1_3
#define O3D_GL_VERSION_1_3 1

// multi-texture extension
#ifndef O3D_GL_PROTOTYPES
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC) (GLenum texture);

O3D_GLAPI PFNGLACTIVETEXTUREPROC glActiveTexture;
#else
O3D_GLAPI void APIENTRY glActiveTexture (GLenum);
#endif // O3D_GL_PROTOTYPES

// glCompressedTexImage
#ifndef O3D_GL_PROTOTYPES
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE3DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE2DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE1DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXIMAGEPROC) (GLenum target, GLint level, GLvoid *img);

O3D_GLAPI PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D;
O3D_GLAPI PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
O3D_GLAPI PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D;
O3D_GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;
O3D_GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;
O3D_GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D;
O3D_GLAPI PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage;
#else
O3D_GLAPI void APIENTRY glCompressedTexImage1D (GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid *);
O3D_GLAPI void APIENTRY glCompressedTexImage2D (GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);
O3D_GLAPI void APIENTRY glCompressedTexImage3D (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);
O3D_GLAPI void APIENTRY glCompressedTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
O3D_GLAPI void APIENTRY glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
O3D_GLAPI void APIENTRY glCompressedTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data);
O3D_GLAPI void APIENTRY glGetCompressedTexImage (GLenum target, GLint level, GLvoid *img);
#endif // O3D_GL_PROTOTYPES
#endif // GL_VERSION_1_3

//
// GL_VERSION_1_4
//

//#ifndef GL_VERSION_1_4
//#define GL_VERSION_1_4 1
#ifndef O3D_GL_VERSION_1_4
#define O3D_GL_VERSION_1_4 1

// point_parameters can be used for particles
#ifndef O3D_GL_PROTOTYPES
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
#else
O3D_GLAPI void APIENTRY glPointParameterf (GLenum, GLfloat);
O3D_GLAPI void APIENTRY glPointParameterfv (GLenum, const GLfloat *);
O3D_GLAPI void APIENTRY glPointParameteri (GLenum pname, GLint param);
O3D_GLAPI void APIENTRY glPointParameteriv (GLenum pname, const GLint *params);

O3D_GLAPI void APIENTRY glBlendFuncSeparate (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);

O3D_GLAPI void APIENTRY glMultiDrawArrays (GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount);
O3D_GLAPI void APIENTRY glMultiDrawElements (GLenum mode, const GLsizei *count, GLenum type, const GLvoid* const *indices, GLsizei drawcount);

O3D_GLAPI void APIENTRY glBlendEquation(GLenum mode);
#endif // O3D_GL_PROTOTYPES
#endif // GL_VERSION_1_4

//
// GL_VERSION_1_5
//

//#ifndef GL_VERSION_1_5
//#define GL_VERSION_1_5 1
#ifndef O3D_GL_VERSION_1_5
#define O3D_GL_VERSION_1_5 1
/* GL types for handling large vertex buffer objects */
typedef std::ptrdiff_t GLintptr;
typedef std::ptrdiff_t GLsizeiptr;

// occlusion_query
#ifndef O3D_GL_PROTOTYPES
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
#else
O3D_GLAPI void APIENTRY glGenQueries (GLsizei, GLuint *);
O3D_GLAPI void APIENTRY glDeleteQueries (GLsizei, const GLuint *);
O3D_GLAPI GLboolean APIENTRY glIsQuery (GLuint);
O3D_GLAPI void APIENTRY glBeginQuery (GLenum, GLuint);
O3D_GLAPI void APIENTRY glEndQuery (GLenum);
O3D_GLAPI void APIENTRY glGetQueryiv (GLenum, GLenum, GLint *);
O3D_GLAPI void APIENTRY glGetQueryObjectiv (GLuint, GLenum, GLint *);
O3D_GLAPI void APIENTRY glGetQueryObjectuiv (GLuint, GLenum, GLuint *);
#endif // O3D_GL_PROTOTYPES

// vertex_buffer_object
#ifndef O3D_GL_PROTOTYPES
typedef void (APIENTRYP PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRYP PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRYP PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
typedef void (APIENTRYP PFNGLBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
typedef void (APIENTRYP PFNGLGETBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data);
typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef GLvoid* (APIENTRYP PFNGLMAPBUFFERPROC) (GLenum target, GLenum access);
typedef GLboolean (APIENTRYP PFNGLUNMAPBUFFERPROC) (GLenum target);

O3D_GLAPI PFNGLBINDBUFFERPROC	glBindBuffer;
O3D_GLAPI PFNGLDELETEBUFFERSPROC	glDeleteBuffers;
O3D_GLAPI PFNGLGENBUFFERSPROC	glGenBuffers;
O3D_GLAPI PFNGLBUFFERDATAPROC	glBufferData;
O3D_GLAPI PFNGLBUFFERSUBDATAPROC    glBufferSubData;
O3D_GLAPI PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
O3D_GLAPI PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
O3D_GLAPI PFNGLMAPBUFFERPROC	glMapBuffer;
O3D_GLAPI PFNGLUNMAPBUFFERPROC	glUnmapBuffer;
#else
O3D_GLAPI void APIENTRY glBindBuffer (GLenum, GLuint);
O3D_GLAPI void APIENTRY glDeleteBuffers (GLsizei, const GLuint *);
O3D_GLAPI void APIENTRY glGenBuffers (GLsizei, GLuint *);
O3D_GLAPI void APIENTRY glBufferData (GLenum, GLsizeiptr, const GLvoid *, GLenum);
O3D_GLAPI void APIENTRY glBufferSubData (GLenum, GLintptr, GLsizeiptr, const GLvoid *);
O3D_GLAPI void APIENTRY glGetBufferSubData (GLenum, GLintptr, GLsizeiptr, GLvoid *);
O3D_GLAPI GLvoid* APIENTRY glMapBuffer (GLenum, GLenum);
O3D_GLAPI GLboolean APIENTRY glUnmapBuffer (GLenum);
O3D_GLAPI void APIENTRY glGetBufferParameteriv (GLenum, GLenum, GLint *);
#endif // O3D_GL_PROTOTYPES
#endif // GL_VERSION_1_5

//
// GL_VERSION_2_0
//

//#ifndef GL_VERSION_2_0
//#define GL_VERSION_2_0 1
#ifndef O3D_GL_VERSION_2_0
#define O3D_GL_VERSION_2_0 1
/* GL type for program/shader text */
typedef char GLchar;			/* native character */

// shading_language and misc
#ifndef O3D_GL_PROTOTYPES
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
#else
O3D_GLAPI void APIENTRY glBlendEquationSeparate (GLenum, GLenum);

O3D_GLAPI void APIENTRY glDrawBuffers (GLsizei, const GLenum *);

O3D_GLAPI void APIENTRY glStencilOpSeparate (GLenum, GLenum, GLenum, GLenum);
O3D_GLAPI void APIENTRY glStencilFuncSeparate (GLenum, GLenum, GLint, GLuint);
O3D_GLAPI void APIENTRY glStencilMaskSeparate (GLenum, GLuint);

O3D_GLAPI void APIENTRY glAttachShader (GLuint, GLuint);
O3D_GLAPI void APIENTRY glBindAttribLocation (GLuint, GLuint, const GLchar *);
O3D_GLAPI void APIENTRY glCompileShader (GLuint);
O3D_GLAPI GLuint APIENTRY glCreateProgram (void);
O3D_GLAPI GLuint APIENTRY glCreateShader (GLenum);
O3D_GLAPI void APIENTRY glDeleteProgram (GLuint);
O3D_GLAPI void APIENTRY glDeleteShader (GLuint);
O3D_GLAPI void APIENTRY glDetachShader (GLuint, GLuint);
O3D_GLAPI void APIENTRY glDisableVertexAttribArray (GLuint);
O3D_GLAPI void APIENTRY glEnableVertexAttribArray (GLuint);
O3D_GLAPI void APIENTRY glGetActiveAttrib (GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *);
O3D_GLAPI void APIENTRY glGetActiveUniform (GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *);
O3D_GLAPI void APIENTRY glGetAttachedShaders (GLuint, GLsizei, GLsizei *, GLuint *);
O3D_GLAPI GLint APIENTRY glGetAttribLocation (GLuint, const GLchar *);
O3D_GLAPI void APIENTRY glGetProgramiv (GLuint, GLenum, GLint *);
O3D_GLAPI void APIENTRY glGetProgramInfoLog (GLuint, GLsizei, GLsizei *, GLchar *);
O3D_GLAPI void APIENTRY glGetShaderiv (GLuint, GLenum, GLint *);
O3D_GLAPI void APIENTRY glGetShaderInfoLog (GLuint, GLsizei, GLsizei *, GLchar *);
O3D_GLAPI void APIENTRY glGetShaderSource (GLuint, GLsizei, GLsizei *, GLchar *);
O3D_GLAPI GLint APIENTRY glGetUniformLocation (GLuint, const GLchar *);
O3D_GLAPI void APIENTRY glGetUniformfv (GLuint, GLint, GLfloat *);
O3D_GLAPI void APIENTRY glGetUniformiv (GLuint, GLint, GLint *);
O3D_GLAPI void APIENTRY glGetVertexAttribdv (GLuint, GLenum, GLdouble *);
O3D_GLAPI void APIENTRY glGetVertexAttribfv (GLuint, GLenum, GLfloat *);
O3D_GLAPI void APIENTRY glGetVertexAttribiv (GLuint, GLenum, GLint *);
O3D_GLAPI void APIENTRY glGetVertexAttribPointerv (GLuint, GLenum, GLvoid* *);
O3D_GLAPI GLboolean APIENTRY glIsProgram (GLuint);
O3D_GLAPI GLboolean APIENTRY glIsShader (GLuint);
O3D_GLAPI void APIENTRY glLinkProgram (GLuint);
O3D_GLAPI void APIENTRY glShaderSource (GLuint, GLsizei, const GLchar* *, const GLint *);
O3D_GLAPI void APIENTRY glUseProgram (GLuint);
O3D_GLAPI void APIENTRY glUniform1f (GLint, GLfloat);
O3D_GLAPI void APIENTRY glUniform2f (GLint, GLfloat, GLfloat);
O3D_GLAPI void APIENTRY glUniform3f (GLint, GLfloat, GLfloat, GLfloat);
O3D_GLAPI void APIENTRY glUniform4f (GLint, GLfloat, GLfloat, GLfloat, GLfloat);
O3D_GLAPI void APIENTRY glUniform1i (GLint, GLint);
O3D_GLAPI void APIENTRY glUniform2i (GLint, GLint, GLint);
O3D_GLAPI void APIENTRY glUniform3i (GLint, GLint, GLint, GLint);
O3D_GLAPI void APIENTRY glUniform4i (GLint, GLint, GLint, GLint, GLint);
O3D_GLAPI void APIENTRY glUniform1fv (GLint, GLsizei, const GLfloat *);
O3D_GLAPI void APIENTRY glUniform2fv (GLint, GLsizei, const GLfloat *);
O3D_GLAPI void APIENTRY glUniform3fv (GLint, GLsizei, const GLfloat *);
O3D_GLAPI void APIENTRY glUniform4fv (GLint, GLsizei, const GLfloat *);
O3D_GLAPI void APIENTRY glUniform1iv (GLint, GLsizei, const GLint *);
O3D_GLAPI void APIENTRY glUniform2iv (GLint, GLsizei, const GLint *);
O3D_GLAPI void APIENTRY glUniform3iv (GLint, GLsizei, const GLint *);
O3D_GLAPI void APIENTRY glUniform4iv (GLint, GLsizei, const GLint *);
O3D_GLAPI void APIENTRY glUniformMatrix2fv (GLint, GLsizei, GLboolean, const GLfloat *);
O3D_GLAPI void APIENTRY glUniformMatrix3fv (GLint, GLsizei, GLboolean, const GLfloat *);
O3D_GLAPI void APIENTRY glUniformMatrix4fv (GLint, GLsizei, GLboolean, const GLfloat *);
O3D_GLAPI void APIENTRY glValidateProgram (GLuint);
O3D_GLAPI void APIENTRY glVertexAttrib1d (GLuint, GLdouble);
O3D_GLAPI void APIENTRY glVertexAttrib1dv (GLuint, const GLdouble *);
O3D_GLAPI void APIENTRY glVertexAttrib1f (GLuint, GLfloat);
O3D_GLAPI void APIENTRY glVertexAttrib1fv (GLuint, const GLfloat *);
O3D_GLAPI void APIENTRY glVertexAttrib1s (GLuint, GLshort);
O3D_GLAPI void APIENTRY glVertexAttrib1sv (GLuint, const GLshort *);
O3D_GLAPI void APIENTRY glVertexAttrib2d (GLuint, GLdouble, GLdouble);
O3D_GLAPI void APIENTRY glVertexAttrib2dv (GLuint, const GLdouble *);
O3D_GLAPI void APIENTRY glVertexAttrib2f (GLuint, GLfloat, GLfloat);
O3D_GLAPI void APIENTRY glVertexAttrib2fv (GLuint, const GLfloat *);
O3D_GLAPI void APIENTRY glVertexAttrib2s (GLuint, GLshort, GLshort);
O3D_GLAPI void APIENTRY glVertexAttrib2sv (GLuint, const GLshort *);
O3D_GLAPI void APIENTRY glVertexAttrib3d (GLuint, GLdouble, GLdouble, GLdouble);
O3D_GLAPI void APIENTRY glVertexAttrib3dv (GLuint, const GLdouble *);
O3D_GLAPI void APIENTRY glVertexAttrib3f (GLuint, GLfloat, GLfloat, GLfloat);
O3D_GLAPI void APIENTRY glVertexAttrib3fv (GLuint, const GLfloat *);
O3D_GLAPI void APIENTRY glVertexAttrib3s (GLuint, GLshort, GLshort, GLshort);
O3D_GLAPI void APIENTRY glVertexAttrib3sv (GLuint, const GLshort *);
O3D_GLAPI void APIENTRY glVertexAttrib4Nbv (GLuint, const GLbyte *);
O3D_GLAPI void APIENTRY glVertexAttrib4Niv (GLuint, const GLint *);
O3D_GLAPI void APIENTRY glVertexAttrib4Nsv (GLuint, const GLshort *);
O3D_GLAPI void APIENTRY glVertexAttrib4Nub (GLuint, GLubyte, GLubyte, GLubyte, GLubyte);
O3D_GLAPI void APIENTRY glVertexAttrib4Nubv (GLuint, const GLubyte *);
O3D_GLAPI void APIENTRY glVertexAttrib4Nuiv (GLuint, const GLuint *);
O3D_GLAPI void APIENTRY glVertexAttrib4Nusv (GLuint, const GLushort *);
O3D_GLAPI void APIENTRY glVertexAttrib4bv (GLuint, const GLbyte *);
O3D_GLAPI void APIENTRY glVertexAttrib4d (GLuint, GLdouble, GLdouble, GLdouble, GLdouble);
O3D_GLAPI void APIENTRY glVertexAttrib4dv (GLuint, const GLdouble *);
O3D_GLAPI void APIENTRY glVertexAttrib4f (GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
O3D_GLAPI void APIENTRY glVertexAttrib4fv (GLuint, const GLfloat *);
O3D_GLAPI void APIENTRY glVertexAttrib4iv (GLuint, const GLint *);
O3D_GLAPI void APIENTRY glVertexAttrib4s (GLuint, GLshort, GLshort, GLshort, GLshort);
O3D_GLAPI void APIENTRY glVertexAttrib4sv (GLuint, const GLshort *);
O3D_GLAPI void APIENTRY glVertexAttrib4ubv (GLuint, const GLubyte *);
O3D_GLAPI void APIENTRY glVertexAttrib4uiv (GLuint, const GLuint *);
O3D_GLAPI void APIENTRY glVertexAttrib4usv (GLuint, const GLushort *);
O3D_GLAPI void APIENTRY glVertexAttribPointer (GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *);
#endif // O3D_GL_PROTOTYPES
#endif // GL_VERSION_2_0

//
// GL_VERSION_3_0
//

//#ifndef GL_VERSION_3_0
//#define GL_VERSION_3_0 1
#ifndef O3D_GL_VERSION_3_0
#define O3D_GL_VERSION_3_0 1

#ifndef O3D_GL_PROTOTYPES
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
#else
O3D_GLAPI void APIENTRY glColorMaski (GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
O3D_GLAPI void APIENTRY glGetBooleani_v (GLenum target, GLuint index, GLboolean *data);
O3D_GLAPI void APIENTRY glGetIntegeri_v (GLenum target, GLuint index, GLint *data);
O3D_GLAPI void APIENTRY glEnablei (GLenum target, GLuint index);
O3D_GLAPI void APIENTRY glDisablei (GLenum target, GLuint index);
O3D_GLAPI GLboolean APIENTRY glIsEnabledi (GLenum target, GLuint index);
O3D_GLAPI void APIENTRY glBeginTransformFeedback (GLenum primitiveMode);
O3D_GLAPI void APIENTRY glEndTransformFeedback (void);
O3D_GLAPI void APIENTRY glBindBufferRange (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
O3D_GLAPI void APIENTRY glBindBufferBase (GLenum target, GLuint index, GLuint buffer);
O3D_GLAPI void APIENTRY glTransformFeedbackVaryings (GLuint program, GLsizei count, const GLchar* *varyings, GLenum bufferMode);
O3D_GLAPI void APIENTRY glGetTransformFeedbackVarying (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
O3D_GLAPI void APIENTRY glClampColor (GLenum target, GLenum clamp);
O3D_GLAPI void APIENTRY glBeginConditionalRender (GLuint id, GLenum mode);
O3D_GLAPI void APIENTRY glEndConditionalRender (void);
O3D_GLAPI void APIENTRY glVertexAttribIPointer (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
O3D_GLAPI void APIENTRY glGetVertexAttribIiv (GLuint index, GLenum pname, GLint *params);
O3D_GLAPI void APIENTRY glGetVertexAttribIuiv (GLuint index, GLenum pname, GLuint *params);
O3D_GLAPI void APIENTRY glVertexAttribI1i (GLuint index, GLint x);
O3D_GLAPI void APIENTRY glVertexAttribI2i (GLuint index, GLint x, GLint y);
O3D_GLAPI void APIENTRY glVertexAttribI3i (GLuint index, GLint x, GLint y, GLint z);
O3D_GLAPI void APIENTRY glVertexAttribI4i (GLuint index, GLint x, GLint y, GLint z, GLint w);
O3D_GLAPI void APIENTRY glVertexAttribI1ui (GLuint index, GLuint x);
O3D_GLAPI void APIENTRY glVertexAttribI2ui (GLuint index, GLuint x, GLuint y);
O3D_GLAPI void APIENTRY glVertexAttribI3ui (GLuint index, GLuint x, GLuint y, GLuint z);
O3D_GLAPI void APIENTRY glVertexAttribI4ui (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
O3D_GLAPI void APIENTRY glVertexAttribI1iv (GLuint index, const GLint *v);
O3D_GLAPI void APIENTRY glVertexAttribI2iv (GLuint index, const GLint *v);
O3D_GLAPI void APIENTRY glVertexAttribI3iv (GLuint index, const GLint *v);
O3D_GLAPI void APIENTRY glVertexAttribI4iv (GLuint index, const GLint *v);
O3D_GLAPI void APIENTRY glVertexAttribI1uiv (GLuint index, const GLuint *v);
O3D_GLAPI void APIENTRY glVertexAttribI2uiv (GLuint index, const GLuint *v);
O3D_GLAPI void APIENTRY glVertexAttribI3uiv (GLuint index, const GLuint *v);
O3D_GLAPI void APIENTRY glVertexAttribI4uiv (GLuint index, const GLuint *v);
O3D_GLAPI void APIENTRY glVertexAttribI4bv (GLuint index, const GLbyte *v);
O3D_GLAPI void APIENTRY glVertexAttribI4sv (GLuint index, const GLshort *v);
O3D_GLAPI void APIENTRY glVertexAttribI4ubv (GLuint index, const GLubyte *v);
O3D_GLAPI void APIENTRY glVertexAttribI4usv (GLuint index, const GLushort *v);
O3D_GLAPI void APIENTRY glGetUniformuiv (GLuint program, GLint location, GLuint *params);
O3D_GLAPI void APIENTRY glBindFragDataLocation (GLuint program, GLuint color, const GLchar *name);
O3D_GLAPI GLint APIENTRY glGetFragDataLocation (GLuint program, const GLchar *name);
O3D_GLAPI void APIENTRY glUniform1ui (GLint location, GLuint v0);
O3D_GLAPI void APIENTRY glUniform2ui (GLint location, GLuint v0, GLuint v1);
O3D_GLAPI void APIENTRY glUniform3ui (GLint location, GLuint v0, GLuint v1, GLuint v2);
O3D_GLAPI void APIENTRY glUniform4ui (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
O3D_GLAPI void APIENTRY glUniform1uiv (GLint location, GLsizei count, const GLuint *value);
O3D_GLAPI void APIENTRY glUniform2uiv (GLint location, GLsizei count, const GLuint *value);
O3D_GLAPI void APIENTRY glUniform3uiv (GLint location, GLsizei count, const GLuint *value);
O3D_GLAPI void APIENTRY glUniform4uiv (GLint location, GLsizei count, const GLuint *value);
O3D_GLAPI void APIENTRY glTexParameterIiv (GLenum target, GLenum pname, const GLint *params);
O3D_GLAPI void APIENTRY glTexParameterIuiv (GLenum target, GLenum pname, const GLuint *params);
O3D_GLAPI void APIENTRY glGetTexParameterIiv (GLenum target, GLenum pname, GLint *params);
O3D_GLAPI void APIENTRY glGetTexParameterIuiv (GLenum target, GLenum pname, GLuint *params);
O3D_GLAPI void APIENTRY glClearBufferiv (GLenum buffer, GLint drawbuffer, const GLint *value);
O3D_GLAPI void APIENTRY glClearBufferuiv (GLenum buffer, GLint drawbuffer, const GLuint *value);
O3D_GLAPI void APIENTRY glClearBufferfv (GLenum buffer, GLint drawbuffer, const GLfloat *value);
O3D_GLAPI void APIENTRY glClearBufferfi (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
O3D_GLAPI const GLubyte * APIENTRY glGetStringi (GLenum name, GLuint index);
#endif // O3D_GL_PROTOTYPES
#endif // GL_VERSION_3_0

//
// GL_VERSION_3_2
//

//#ifndef GL_VERSION_3_2
//#define GL_VERSION_3_2 1
#ifndef O3D_GL_VERSION_3_2
#define O3D_GL_VERSION_3_2 1

// Geometry Shader
#ifndef O3D_GL_PROTOTYPES
//typedef void (APIENTRYP PFNGLGETINTEGER64I_VPROC) (GLenum target, GLuint index, GLint64 *data);
//typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERI64VPROC) (GLenum target, GLenum pname, GLint64 *params);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETERIPROC) (GLuint program, GLenum pname, GLint value);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREFACEPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level, GLenum face);

//O3D_GLAPI PFNGLGETINTEGER64I_VPROC glGetInteger64i_v;
//O3D_GLAPI PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v;
O3D_GLAPI PFNGLPROGRAMPARAMETERIPROC glProgramParameteri;
O3D_GLAPI PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
O3D_GLAPI PFNGLFRAMEBUFFERTEXTUREFACEPROC glFramebufferTextureFace;
#else
//O3D_GLAPI void APIENTRY glGetInteger64i_v (GLenum, GLuint, GLint64 *);
//O3D_GLAPI void APIENTRY glGetBufferParameteri64v (GLenum, GLenum, GLint64 *);
O3D_GLAPI void APIENTRY glProgramParameteri (GLuint, GLenum, GLint);
O3D_GLAPI void APIENTRY glFramebufferTexture (GLenum, GLenum, GLuint, GLint);
O3D_GLAPI void APIENTRY glFramebufferTextureFace (GLenum, GLenum, GLuint, GLint, GLenum);
#endif // O3D_GL_PROTOTYPES
#endif // GL_VERSION_3_2

//
// GL_VERSION_3_3
//

//#ifndef GL_VERSION_3_3
//#define GL_VERSION_3_3 1
#ifndef O3D_GL_VERSION_3_3
#define O3D_GL_VERSION_3_3 1

// Sampler object
#ifndef O3D_GL_PROTOTYPES
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
#else
O3D_GLAPI void APIENTRY glGenSamplers (GLsizei count, GLuint *samplers);
O3D_GLAPI void APIENTRY glDeleteSamplers (GLsizei count, const GLuint *samplers);
O3D_GLAPI GLboolean APIENTRY glIsSampler (GLuint sampler);
O3D_GLAPI void APIENTRY glBindSampler (GLuint unit, GLuint sampler);
O3D_GLAPI void APIENTRY glSamplerParameteri (GLuint sampler, GLenum pname, GLint param);
O3D_GLAPI void APIENTRY glSamplerParameteriv (GLuint sampler, GLenum pname, const GLint *param);
O3D_GLAPI void APIENTRY glSamplerParameterf (GLuint sampler, GLenum pname, GLfloat param);
O3D_GLAPI void APIENTRY glSamplerParameterfv (GLuint sampler, GLenum pname, const GLfloat *param);
O3D_GLAPI void APIENTRY glSamplerParameterIiv (GLuint sampler, GLenum pname, const GLint *param);
O3D_GLAPI void APIENTRY glSamplerParameterIuiv (GLuint sampler, GLenum pname, const GLuint *param);
O3D_GLAPI void APIENTRY glGetSamplerParameteriv (GLuint sampler, GLenum pname, GLint *params);
O3D_GLAPI void APIENTRY glGetSamplerParameterIiv (GLuint sampler, GLenum pname, GLint *params);
O3D_GLAPI void APIENTRY glGetSamplerParameterfv (GLuint sampler, GLenum pname, GLfloat *params);
O3D_GLAPI void APIENTRY glGetSamplerParameterIuiv (GLuint sampler, GLenum pname, GLuint *params);
#endif // O3D_GL_PROTOTYPES
#endif // GL_VERSION_3_3

//
// GL_VERSION_4_0
//

//#ifndef GL_VERSION_4_0
//#define GL_VERSION_4_0 1
#ifndef O3D_GL_VERSION_4_0
#define O3D_GL_VERSION_4_0 1

#ifndef O3D_GL_PROTOTYPES
typedef void (APIENTRYP PFNGLBLENDEQUATIONIPROC) (GLuint buf, GLenum mode);
typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEIPROC) (GLuint buf, GLenum modeRGB, GLenum modeAlpha);
typedef void (APIENTRYP PFNGLBLENDFUNCIPROC) (GLuint buf, GLenum src, GLenum dst);
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEIPROC) (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);

O3D_GLAPI PFNGLBLENDEQUATIONIPROC glBlendEquationi;
O3D_GLAPI PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei;
O3D_GLAPI PFNGLBLENDFUNCIPROC glBlendFunci;
O3D_GLAPI PFNGLBLENDFUNCSEPARATEIPROC glBlendFuncSeparatei;
#else
O3D_GLAPI void APIENTRY glBlendEquationi (GLuint buf, GLenum mode);
O3D_GLAPI void APIENTRY glBlendEquationSeparatei (GLuint buf, GLenum modeRGB, GLenum modeAlpha);
O3D_GLAPI void APIENTRY glBlendFunci (GLuint buf, GLenum src, GLenum dst);
O3D_GLAPI void APIENTRY glBlendFuncSeparatei (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
#endif // O3D_GL_PROTOTYPES

#endif // GL_VERSION_4_0

//
// GL_VERSION_4_1
//

//#ifndef GL_VERSION_4_1
//#define GL_VERSION_4_1 1
#ifndef O3D_GL_VERSION_4_1
#define O3D_GL_VERSION_4_1 1

#endif // GL_VERSION_4_1

//
// GL_VERSION_4_2
//

//#ifndef GL_VERSION_4_2
//#define GL_VERSION_4_2 1
#ifndef O3D_GL_VERSION_4_2
#define O3D_GL_VERSION_4_2 1

#endif // GL_VERSION_4_2

//
// GL_VERSION_4_3
//

//#ifndef GL_VERSION_4_3
//#define GL_VERSION_4_3 1
#ifndef O3D_GL_VERSION_4_3
#define O3D_GL_VERSION_4_3 1

#endif // GL_VERSION_4_3

//
// GL_VERSION_4_4
//

//#ifndef GL_VERSION_4_4
//#define GL_VERSION_4_4 1
#ifndef O3D_GL_VERSION_4_4
#define O3D_GL_VERSION_4_4 1

#endif // GL_VERSION_4_4

//
// GL_VERSION_4_5
//

//#ifndef GL_VERSION_4_5
//#define GL_VERSION_4_5 1
#ifndef O3D_GL_VERSION_4_5
#define O3D_GL_VERSION_4_5 1

#endif // GL_VERSION_4_5

//
// GL_ARB_framebuffer_object
//

//#ifndef GL_ARB_framebuffer_object
//#define GL_ARB_framebuffer_object 1
#ifndef O3D_GL_ARB_framebuffer_object
#define O3D_GL_ARB_framebuffer_object 1

// framebuffer_object (render-to-texture)
#ifndef O3D_GL_PROTOTYPES
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

O3D_GLAPI PFNGLISRENDERBUFFERPROC 		glIsRenderbuffer;
O3D_GLAPI PFNGLBINDRENDERBUFFERPROC 	glBindRenderbuffer;
O3D_GLAPI PFNGLDELETERENDERBUFFERSPROC 	glDeleteRenderbuffers;
O3D_GLAPI PFNGLGENRENDERBUFFERSPROC 	glGenRenderbuffers;
O3D_GLAPI PFNGLRENDERBUFFERSTORAGEPROC 	glRenderbufferStorage;
O3D_GLAPI PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv;
O3D_GLAPI PFNGLISFRAMEBUFFERPROC 		glIsFramebuffer;
O3D_GLAPI PFNGLBINDFRAMEBUFFERPROC 		glBindFramebuffer;
O3D_GLAPI PFNGLDELETEFRAMEBUFFERSPROC 	glDeleteFramebuffers;
O3D_GLAPI PFNGLGENFRAMEBUFFERSPROC 		glGenFramebuffers;
O3D_GLAPI PFNGLCHECKFRAMEBUFFERSTATUSPROC 	glCheckFramebufferStatus;
O3D_GLAPI PFNGLFRAMEBUFFERTEXTURE1DPROC 	glFramebufferTexture1D;
O3D_GLAPI PFNGLFRAMEBUFFERTEXTURE2DPROC 	glFramebufferTexture2D;
O3D_GLAPI PFNGLFRAMEBUFFERTEXTURE3DPROC 	glFramebufferTexture3D;
O3D_GLAPI PFNGLFRAMEBUFFERRENDERBUFFERPROC 	glFramebufferRenderbuffer;
O3D_GLAPI PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
O3D_GLAPI PFNGLBLITFRAMEBUFFERPROC          glBlitFramebuffer;
O3D_GLAPI PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
O3D_GLAPI PFNGLGENERATEMIPMAPPROC 		glGenerateMipmap;
O3D_GLAPI PFNGLFRAMEBUFFERTEXTURELAYERPROC  glFramebufferTextureLayer;

#else
O3D_GLAPI GLboolean APIENTRY glIsRenderbuffer (GLuint);
O3D_GLAPI void APIENTRY glBindRenderbuffer (GLenum, GLuint);
O3D_GLAPI void APIENTRY glDeleteRenderbuffers (GLsizei, const GLuint *);
O3D_GLAPI void APIENTRY glGenRenderbuffers (GLsizei, GLuint *);
O3D_GLAPI void APIENTRY glRenderbufferStorage (GLenum, GLenum, GLsizei, GLsizei);
O3D_GLAPI void APIENTRY glGetRenderbufferParameteriv (GLenum, GLenum, GLint *);
O3D_GLAPI GLboolean APIENTRY glIsFramebuffer (GLuint);
O3D_GLAPI void APIENTRY glBindFramebuffer (GLenum, GLuint);
O3D_GLAPI void APIENTRY glDeleteFramebuffers (GLsizei, const GLuint *);
O3D_GLAPI void APIENTRY glGenFramebuffers (GLsizei, GLuint *);
O3D_GLAPI GLenum APIENTRY glCheckFramebufferStatus (GLenum);
O3D_GLAPI void APIENTRY glFramebufferTexture1D (GLenum, GLenum, GLenum, GLuint, GLint);
O3D_GLAPI void APIENTRY glFramebufferTexture2D (GLenum, GLenum, GLenum, GLuint, GLint);
O3D_GLAPI void APIENTRY glFramebufferTexture3D (GLenum, GLenum, GLenum, GLuint, GLint, GLint);
O3D_GLAPI void APIENTRY glFramebufferRenderbuffer (GLenum, GLenum, GLenum, GLuint);
O3D_GLAPI void APIENTRY glGetFramebufferAttachmentParameteriv (GLenum, GLenum, GLenum, GLint *);
O3D_GLAPI void APIENTRY glGenerateMipmap (GLenum);
O3D_GLAPI void APIENTRY glBlitFramebuffer (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
#endif // O3D_GL_PROTOTYPES
#endif // GL_ARB_framebuffer_objectf

//
// GL_ARB_vertex_array_object
//

//#ifndef GL_ARB_vertex_array_object
//#define GL_ARB_vertex_array_object 1
#ifndef O3D_GL_ARB_vertex_array_object
#define O3D_GL_ARB_vertex_array_object 1

#ifndef O3D_GL_PROTOTYPES
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint *arrays);
typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint *arrays);
typedef GLboolean (APIENTRYP PFNGLISVERTEXARRAYPROC) (GLuint array);

O3D_GLAPI PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
O3D_GLAPI PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
O3D_GLAPI PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
O3D_GLAPI PFNGLISVERTEXARRAYPROC glIsVertexArray;
#else
O3D_GLAPI void APIENTRY glBindVertexArray (GLuint array);
O3D_GLAPI void APIENTRY glDeleteVertexArrays (GLsizei n, const GLuint *arrays);
O3D_GLAPI void APIENTRY glGenVertexArrays (GLsizei n, GLuint *arrays);
O3D_GLAPI GLboolean APIENTRY glIsVertexArray (GLuint array);
#endif // O3D_GL_PROTOTYPES
#endif // GL_ARB_vertex_array_object

//
// GL_ARB_map_buffer_range
//

//#ifndef GL_ARB_map_buffer_range
//#define GL_ARB_map_buffer_range 1
#ifndef O3D_GL_ARB_map_buffer_range
#define O3D_GL_ARB_map_buffer_range 1

#ifndef O3D_GL_PROTOTYPES
typedef GLvoid* (APIENTRYP PFNGLMAPBUFFERRANGEPROC) (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef void (APIENTRYP PFNGLFLUSHMAPPEDBUFFERRANGEPROC) (GLenum target, GLintptr offset, GLsizeiptr length);

O3D_GLAPI PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
O3D_GLAPI PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange;
#else
O3D_GLAPI GLvoid* APIENTRY glMapBufferRange (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
O3D_GLAPI void APIENTRY glFlushMappedBufferRange (GLenum target, GLintptr offset, GLsizeiptr length);
#endif // O3D_GL_PROTOTYPES
#endif // GL_ARB_map_buffer_range

//
// GL_ARB_texture_multisample
//

//#ifndef GL_ARB_texture_multisample
//#define GL_ARB_texture_multisample 1
#ifndef O3D_GL_ARB_texture_multisample
#define O3D_GL_ARB_texture_multisample 1

#ifndef O3D_GL_PROTOTYPES
typedef void (APIENTRYP PFNGLTEXIMAGE2DMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLTEXIMAGE3DMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLGETMULTISAMPLEFVPROC) (GLenum pname, GLuint index, GLfloat *val);
typedef void (APIENTRYP PFNGLSAMPLEMASKIPROC) (GLuint index, GLbitfield mask);

O3D_GLAPI PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
O3D_GLAPI PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample;
O3D_GLAPI PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv;
O3D_GLAPI PFNGLSAMPLEMASKIPROC glSampleMaski;
#else
O3D_GLAPI void APIENTRY glTexImage2DMultisample (GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
O3D_GLAPI void APIENTRY glTexImage3DMultisample (GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
O3D_GLAPI void APIENTRY glGetMultisamplefv (GLenum pname, GLuint index, GLfloat *val);
O3D_GLAPI void APIENTRY glSampleMaski (GLuint index, GLbitfield mask);
#endif // O3D_GL_PROTOTYPES
#endif

//#ifdef __cplusplus
//}
//#endif

} // namespace o3d

#endif // _O3D_GLEXTENSIONMANAGER_H
