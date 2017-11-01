/**
 * @file glextensionmanager.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/glx11.h"

#include "o3d/core/debug.h"
#include "o3d/core/stringtokenizer.h"

using namespace o3d;

#define GL_NUM_EXTENSIONS                 0x821D

#ifndef O3D_GL_PROTOTYPES

#ifdef O3D_GL_VERSION_1_2
// draw range element
PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements = nullptr;

// texture_3d
PFNGLTEXIMAGE3DPROC	glTexImage3D = nullptr;
PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D  = nullptr;
#endif // O3D_GL_VERSION_1_2

#ifdef O3D_GL_VERSION_1_3
// multi-texture extension
PFNGLACTIVETEXTUREPROC glActiveTexture  = nullptr;

// glCompressedTexImage
PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D  = nullptr;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D  = nullptr;
PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D  = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D  = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D  = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D  = nullptr;
PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage  = nullptr;
#endif // O3D_GL_VERSION_1_3

#ifdef O3D_GL_VERSION_1_4
// point_parameters can be used for particles
PFNGLPOINTPARAMETERFPROC	glPointParameterf	 = nullptr;
PFNGLPOINTPARAMETERFVPROC	glPointParameterfv	 = nullptr;
PFNGLPOINTPARAMETERIPROC    glPointParameteri    = nullptr;
PFNGLPOINTPARAMETERIVPROC   glPointParameteriv   = nullptr;
PFNGLBLENDFUNCSEPARATEPROC  glBlendFuncSeparate  = nullptr;
PFNGLMULTIDRAWARRAYSPROC    glMultiDrawArrays    = nullptr;
PFNGLMULTIDRAWELEMENTSPROC  glMultiDrawElements  = nullptr;
PFNGLBLENDEQUATIONPROC      glBlendEquation      = nullptr;
#endif // O3D_GL_VERSION_1_4

#ifdef O3D_GL_VERSION_1_5
// occlusion_query
PFNGLGENQUERIESPROC	    glGenQueries	 = nullptr;
PFNGLISQUERYPROC	    glIsQuery		 = nullptr;
PFNGLBEGINQUERYPROC	    glBeginQuery	 = nullptr;
PFNGLENDQUERYPROC	    glEndQuery		 = nullptr;
PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv	 = nullptr;
PFNGLGETQUERYIVPROC	    glGetQueryiv     	     = nullptr;
PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv	 = nullptr;
PFNGLDELETEQUERIESPROC  glDeleteQueries	 = nullptr;

// vertex_buffer_object
PFNGLBINDBUFFERPROC		glBindBuffer		 = nullptr;
PFNGLDELETEBUFFERSPROC	glDeleteBuffers		 = nullptr;
PFNGLGENBUFFERSPROC		glGenBuffers		 = nullptr;
PFNGLBUFFERDATAPROC		glBufferData		 = nullptr;
PFNGLBUFFERSUBDATAPROC        glBufferSubData         = nullptr;
PFNGLGETBUFFERSUBDATAPROC     glGetBufferSubData      = nullptr;
PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv  = nullptr;
PFNGLMAPBUFFERPROC		glMapBuffer    = nullptr;
PFNGLUNMAPBUFFERPROC	glUnmapBuffer  = nullptr;
#endif // O3D_GL_VERSION_1_5

#ifdef O3D_GL_VERSION_2_0

PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate = nullptr;

PFNGLDRAWBUFFERSPROC glDrawBuffers = nullptr;

PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate = nullptr;
PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate = nullptr;
PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate = nullptr;

// shading_language
PFNGLCREATESHADERPROC    glCreateShader		 = nullptr;
PFNGLSHADERSOURCEPROC	 glShaderSource		 = nullptr;
PFNGLCOMPILESHADERPROC	 glCompileShader	 = nullptr;
PFNGLCREATEPROGRAMPROC	 glCreateProgram	 = nullptr;
PFNGLATTACHSHADERPROC	 glAttachShader		 = nullptr;
PFNGLLINKPROGRAMPROC	 glLinkProgram		 = nullptr;
PFNGLVALIDATEPROGRAMPROC glValidateProgram	 = nullptr;
PFNGLUSEPROGRAMPROC	 	 glUseProgram		 = nullptr;

PFNGLDETACHSHADERPROC  glDetachShader  = nullptr;
PFNGLDELETESHADERPROC  glDeleteShader  = nullptr;
PFNGLDELETEPROGRAMPROC glDeleteProgram = nullptr;

PFNGLGETPROGRAMIVPROC		glGetProgramiv		 = nullptr;
PFNGLGETPROGRAMINFOLOGPROC	glGetProgramInfoLog	 = nullptr;
PFNGLGETSHADERIVPROC		glGetShaderiv		 = nullptr;
PFNGLGETSHADERINFOLOGPROC	glGetShaderInfoLog	 = nullptr;
PFNGLISPROGRAMPROC			glIsProgram		 = nullptr;
PFNGLISSHADERPROC			glIsShader		 = nullptr;

// uniform
PFNGLGETUNIFORMLOCATIONPROC	glGetUniformLocation = nullptr;
PFNGLGETACTIVEUNIFORMPROC   glGetActiveUniform   = nullptr;
PFNGLUNIFORM1FPROC		glUniform1f		 = nullptr;
PFNGLUNIFORM2FPROC		glUniform2f		 = nullptr;
PFNGLUNIFORM3FPROC		glUniform3f		 = nullptr;
PFNGLUNIFORM4FPROC		glUniform4f		 = nullptr;
PFNGLUNIFORM1FVPROC		glUniform1fv	 = nullptr;
PFNGLUNIFORM2FVPROC		glUniform2fv	 = nullptr;
PFNGLUNIFORM3FVPROC		glUniform3fv	 = nullptr;
PFNGLUNIFORM4FVPROC		glUniform4fv	 = nullptr;
PFNGLUNIFORM1IPROC		glUniform1i		 = nullptr;
PFNGLUNIFORM2IPROC		glUniform2i		 = nullptr;
PFNGLUNIFORM3IPROC		glUniform3i		 = nullptr;
PFNGLUNIFORM4IPROC		glUniform4i		 = nullptr;
PFNGLUNIFORMMATRIX2FVPROC	glUniformMatrix2fv	 = nullptr;
PFNGLUNIFORMMATRIX3FVPROC	glUniformMatrix3fv	 = nullptr;
PFNGLUNIFORMMATRIX4FVPROC	glUniformMatrix4fv	 = nullptr;

// vertex attributes array
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray  = nullptr;
PFNGLGETACTIVEATTRIBPROC	glGetActiveAttrib		 = nullptr;
PFNGLBINDATTRIBLOCATIONPROC	glBindAttribLocation	 = nullptr;
PFNGLGETATTRIBLOCATIONPROC	glGetAttribLocation		 = nullptr;
PFNGLGETVERTEXATTRIBDVPROC	glGetVertexAttribdv		 = nullptr;
PFNGLGETVERTEXATTRIBFVPROC	glGetVertexAttribfv		 = nullptr;
PFNGLGETVERTEXATTRIBIVPROC	glGetVertexAttribiv		 = nullptr;
PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv = nullptr;

// vertex attributes
PFNGLVERTEXATTRIB1DPROC		glVertexAttrib1d	 = nullptr;
PFNGLVERTEXATTRIB1DVPROC	glVertexAttrib1dv	 = nullptr;
PFNGLVERTEXATTRIB1FPROC		glVertexAttrib1f	 = nullptr;
PFNGLVERTEXATTRIB1FVPROC	glVertexAttrib1fv	 = nullptr;
PFNGLVERTEXATTRIB1SPROC		glVertexAttrib1s	 = nullptr;
PFNGLVERTEXATTRIB1SVPROC	glVertexAttrib1sv	 = nullptr;
PFNGLVERTEXATTRIB2DPROC		glVertexAttrib2d	 = nullptr;
PFNGLVERTEXATTRIB2DVPROC	glVertexAttrib2dv	 = nullptr;
PFNGLVERTEXATTRIB2FPROC		glVertexAttrib2f	 = nullptr;
PFNGLVERTEXATTRIB2FVPROC	glVertexAttrib2fv	 = nullptr;
PFNGLVERTEXATTRIB2SPROC		glVertexAttrib2s	 = nullptr;
PFNGLVERTEXATTRIB2SVPROC	glVertexAttrib2sv	 = nullptr;
PFNGLVERTEXATTRIB3DPROC		glVertexAttrib3d	 = nullptr;
PFNGLVERTEXATTRIB3DVPROC	glVertexAttrib3dv	 = nullptr;
PFNGLVERTEXATTRIB3FPROC		glVertexAttrib3f	 = nullptr;
PFNGLVERTEXATTRIB3FVPROC	glVertexAttrib3fv	 = nullptr;
PFNGLVERTEXATTRIB3SPROC		glVertexAttrib3s	 = nullptr;
PFNGLVERTEXATTRIB3SVPROC	glVertexAttrib3sv	 = nullptr;
PFNGLVERTEXATTRIB4NBVPROC	glVertexAttrib4Nbv	 = nullptr;
PFNGLVERTEXATTRIB4NIVPROC	glVertexAttrib4Niv	 = nullptr;
PFNGLVERTEXATTRIB4NSVPROC	glVertexAttrib4Nsv	 = nullptr;
PFNGLVERTEXATTRIB4NUBPROC	glVertexAttrib4Nub	 = nullptr;
PFNGLVERTEXATTRIB4NUBVPROC	glVertexAttrib4Nubv	 = nullptr;
PFNGLVERTEXATTRIB4NUIVPROC	glVertexAttrib4Nuiv	 = nullptr;
PFNGLVERTEXATTRIB4NUSVPROC	glVertexAttrib4Nusv	 = nullptr;
PFNGLVERTEXATTRIB4BVPROC	glVertexAttrib4bv	 = nullptr;
PFNGLVERTEXATTRIB4DPROC		glVertexAttrib4d	 = nullptr;
PFNGLVERTEXATTRIB4DVPROC	glVertexAttrib4dv	 = nullptr;
PFNGLVERTEXATTRIB4FPROC		glVertexAttrib4f	 = nullptr;
PFNGLVERTEXATTRIB4FVPROC	glVertexAttrib4fv	 = nullptr;
PFNGLVERTEXATTRIB4IVPROC	glVertexAttrib4iv	 = nullptr;
PFNGLVERTEXATTRIB4SPROC		glVertexAttrib4s	 = nullptr;
PFNGLVERTEXATTRIB4SVPROC	glVertexAttrib4sv	 = nullptr;
PFNGLVERTEXATTRIB4UBVPROC	glVertexAttrib4ubv	 = nullptr;
PFNGLVERTEXATTRIB4UIVPROC	glVertexAttrib4uiv	 = nullptr;
PFNGLVERTEXATTRIB4USVPROC	glVertexAttrib4usv	 = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC	glVertexAttribPointer	 = nullptr;
#endif // O3D_GL_VERSION_2_0

#ifdef O3D_GL_VERSION_3_0
PFNGLCOLORMASKIPROC glColorMaski  = nullptr;
PFNGLGETBOOLEANI_VPROC glGetBooleani_v  = nullptr;
PFNGLGETINTEGERI_VPROC glGetIntegeri_v  = nullptr;
PFNGLENABLEIPROC glEnablei  = nullptr;
PFNGLDISABLEIPROC glDisablei  = nullptr;
PFNGLISENABLEDIPROC glIsEnabledi  = nullptr;
PFNGLBEGINTRANSFORMFEEDBACKPROC glBeginTransformFeedback  = nullptr;
PFNGLENDTRANSFORMFEEDBACKPROC glEndTransformFeedback  = nullptr;
PFNGLBINDBUFFERRANGEPROC glBindBufferRange  = nullptr;
PFNGLBINDBUFFERBASEPROC glBindBufferBase  = nullptr;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC glTransformFeedbackVaryings  = nullptr;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying  = nullptr;
PFNGLCLAMPCOLORPROC glClampColor  = nullptr;
PFNGLBEGINCONDITIONALRENDERPROC glBeginConditionalRender  = nullptr;
PFNGLENDCONDITIONALRENDERPROC glEndConditionalRender  = nullptr;
PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer  = nullptr;
PFNGLGETVERTEXATTRIBIIVPROC glGetVertexAttribIiv  = nullptr;
PFNGLGETVERTEXATTRIBIUIVPROC glGetVertexAttribIuiv  = nullptr;
PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i  = nullptr;
PFNGLVERTEXATTRIBI2IPROC glVertexAttribI2i  = nullptr;
PFNGLVERTEXATTRIBI3IPROC glVertexAttribI3i  = nullptr;
PFNGLVERTEXATTRIBI4IPROC glVertexAttribI4i  = nullptr;
PFNGLVERTEXATTRIBI1UIPROC glVertexAttribI1ui  = nullptr;
PFNGLVERTEXATTRIBI2UIPROC glVertexAttribI2ui  = nullptr;
PFNGLVERTEXATTRIBI3UIPROC glVertexAttribI3ui  = nullptr;
PFNGLVERTEXATTRIBI4UIPROC glVertexAttribI4ui  = nullptr;
PFNGLVERTEXATTRIBI1IVPROC glVertexAttribI1iv  = nullptr;
PFNGLVERTEXATTRIBI2IVPROC glVertexAttribI2iv  = nullptr;
PFNGLVERTEXATTRIBI3IVPROC glVertexAttribI3iv  = nullptr;
PFNGLVERTEXATTRIBI4IVPROC glVertexAttribI4iv  = nullptr;
PFNGLVERTEXATTRIBI1UIVPROC glVertexAttribI1uiv  = nullptr;
PFNGLVERTEXATTRIBI2UIVPROC glVertexAttribI2uiv  = nullptr;
PFNGLVERTEXATTRIBI3UIVPROC glVertexAttribI3uiv  = nullptr;
PFNGLVERTEXATTRIBI4UIVPROC glVertexAttribI4uiv  = nullptr;
PFNGLVERTEXATTRIBI4BVPROC glVertexAttribI4bv  = nullptr;
PFNGLVERTEXATTRIBI4SVPROC glVertexAttribI4sv  = nullptr;
PFNGLVERTEXATTRIBI4UBVPROC glVertexAttribI4ubv  = nullptr;
PFNGLVERTEXATTRIBI4USVPROC glVertexAttribI4usv  = nullptr;
PFNGLGETUNIFORMUIVPROC glGetUniformuiv  = nullptr;
PFNGLBINDFRAGDATALOCATIONPROC glBindFragDataLocation  = nullptr;
PFNGLGETFRAGDATALOCATIONPROC glGetFragDataLocation  = nullptr;
PFNGLUNIFORM1UIPROC glUniform1ui  = nullptr;
PFNGLUNIFORM2UIPROC glUniform2ui  = nullptr;
PFNGLUNIFORM3UIPROC glUniform3ui  = nullptr;
PFNGLUNIFORM4UIPROC glUniform4ui  = nullptr;
PFNGLUNIFORM1UIVPROC glUniform1uiv  = nullptr;
PFNGLUNIFORM2UIVPROC glUniform2uiv  = nullptr;
PFNGLUNIFORM3UIVPROC glUniform3uiv  = nullptr;
PFNGLUNIFORM4UIPROC glUniform4uiv  = nullptr;
PFNGLTEXPARAMETERIIVPROC glTexParameterIiv  = nullptr;
PFNGLTEXPARAMETERIUIVPROC glTexParameterIuiv  = nullptr;
PFNGLGETTEXPARAMETERIIVPROC glGetTexParameterIiv  = nullptr;
PFNGLGETTEXPARAMETERIUIVPROC glGetTexParameterIuiv  = nullptr;
PFNGLCLEARBUFFERIVPROC glClearBufferiv  = nullptr;
PFNGLCLEARBUFFERUIVPROC glClearBufferuiv  = nullptr;
PFNGLCLEARBUFFERFVPROC glClearBufferfv  = nullptr;
PFNGLCLEARBUFFERFIPROC glClearBufferfi  = nullptr;
PFNGLGETSTRINGIPROC glGetStringi  = nullptr;
#endif // O3D_GL_VERSION_3_0

#ifdef O3D_GL_VERSION_3_2
//PFNGLGETINTEGER64I_VPROC        glGetInteger64i_v        = nullptr;
//PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v = nullptr;
PFNGLPROGRAMPARAMETERIPROC        glProgramParameteri      = nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC       glFramebufferTexture     = nullptr;
PFNGLFRAMEBUFFERTEXTUREFACEPROC   glFramebufferTextureFace = nullptr;
#endif // O3D_GL_VERSION_3_2

#ifdef O3D_GL_VERSION_3_3
PFNGLGENSAMPLERSPROC glGenSamplers = nullptr;
PFNGLDELETESAMPLERSPROC glDeleteSamplers = nullptr;
PFNGLISSAMPLERPROC glIsSampler = nullptr;
PFNGLBINDSAMPLERPROC glBindSampler = nullptr;
PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri = nullptr;
PFNGLSAMPLERPARAMETERIVPROC glSamplerParameteriv = nullptr;
PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf = nullptr;
PFNGLSAMPLERPARAMETERFVPROC glSamplerParameterfv = nullptr;
PFNGLSAMPLERPARAMETERIIVPROC glSamplerParameterIiv = nullptr;
PFNGLSAMPLERPARAMETERIUIVPROC glSamplerParameterIuiv = nullptr;
PFNGLGETSAMPLERPARAMETERIVPROC glGetSamplerParameteriv = nullptr;
PFNGLGETSAMPLERPARAMETERIIVPROC glGetSamplerParameterIiv = nullptr;
PFNGLGETSAMPLERPARAMETERFVPROC glGetSamplerParameterfv = nullptr;
PFNGLGETSAMPLERPARAMETERIUIVPROC glGetSamplerParameterIuiv = nullptr;
#endif // O3D_GL_VERSION_3_3

#ifdef O3D_GL_VERSION_4_0
PFNGLBLENDEQUATIONIPROC glBlendEquationi = nullptr;
PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei = nullptr;
PFNGLBLENDFUNCIPROC glBlendFunci = nullptr;
PFNGLBLENDFUNCSEPARATEIPROC glBlendFuncSeparatei = nullptr;
#endif // O3D_GL_VERSION_4_0

#ifdef O3D_GL_VERSION_4_1
#endif // O3D_GL_VERSION_4_1

#ifdef O3D_GL_VERSION_4_2
#endif // O3D_GL_VERSION_4_2

#ifdef O3D_GL_VERSION_4_3
#endif // O3D_GL_VERSION_4_3

#ifdef O3D_GL_VERSION_4_4
#endif // O3D_GL_VERSION_4_4

#ifdef O3D_GL_VERSION_4_5
#endif // O3D_GL_VERSION_4_5

#ifdef O3D_GL_ARB_framebuffer_object
// framebuffer_object (render-to-texture)
PFNGLISRENDERBUFFERPROC     glIsRenderbuffer       = nullptr;
PFNGLBINDRENDERBUFFERPROC     glBindRenderbuffer       = nullptr;
PFNGLDELETERENDERBUFFERSPROC     glDeleteRenderbuffers  = nullptr;
PFNGLGENRENDERBUFFERSPROC     glGenRenderbuffers       = nullptr;
PFNGLRENDERBUFFERSTORAGEPROC     glRenderbufferStorage  = nullptr;
PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv  = nullptr;
PFNGLISFRAMEBUFFERPROC         glIsFramebuffer           = nullptr;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer       = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers  = nullptr;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = nullptr;
PFNGLFRAMEBUFFERTEXTURE1DPROC     glFramebufferTexture1D = nullptr;
PFNGLFRAMEBUFFERTEXTURE2DPROC     glFramebufferTexture2D = nullptr;
PFNGLFRAMEBUFFERTEXTURE3DPROC     glFramebufferTexture3D = nullptr;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = nullptr;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC  glGetFramebufferAttachmentParameteriv = nullptr;
PFNGLBLITFRAMEBUFFERPROC        glBlitFramebuffer  = nullptr;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample = nullptr;
PFNGLGENERATEMIPMAPPROC     glGenerateMipmap = nullptr;
PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer = nullptr;
#endif // O3D_GL_ARB_framebuffer_object

// O3D_GL_ARB_vertex_array_object
#ifdef O3D_GL_ARB_vertex_array_object
PFNGLBINDVERTEXARRAYPROC glBindVertexArray       = nullptr;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = nullptr;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays       = nullptr;
PFNGLISVERTEXARRAYPROC glIsVertexArray           = nullptr;
#endif // O3D_GL_ARB_vertex_array_object

#ifdef O3D_GL_ARB_map_buffer_range
PFNGLMAPBUFFERRANGEPROC glMapBufferRange                 = nullptr;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange = nullptr;
#endif // O3D_GL_ARB_map_buffer_range

#ifdef O3D_GL_ARB_texture_multisample
PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample = nullptr;
PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample = nullptr;
PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv = nullptr;
PFNGLSAMPLEMASKIPROC glSampleMaski = nullptr;
#endif // GL_ARB_texture_multisample

#endif // O3D_GL_PROTOTYPES

// statics members
Bool GLExtensionManager::m_valid = False;

// Get OpenGL extension procedure address
#ifdef O3D_WIN32
	#define O3D_GET_OPENGL_PROC(ext) wglGetProcAddress(ext)
#endif
#ifdef O3D_X11
    #define O3D_GET_OPENGL_PROC(ext) o3d::glxGetProcAddress(ext)
#endif
#ifdef O3D_SDL
	#include <SDL2/SDL_video.h>
	#define O3D_GET_OPENGL_PROC(ext) SDL_GL_GetProcAddress(ext)
#endif

// Get extensions pointers
void GLExtensionManager::getExtFunctions()
{
#ifndef O3D_GL_PROTOTYPES

	//
	// draw range element*
	//

#ifdef O3D_GL_VERSION_1_2
	glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC) O3D_GET_OPENGL_PROC("glDrawRangeElements");
	if (!glDrawRangeElements)
		O3D_ERROR(E_NullPointer("OpenGL glDrawRangeElements function"));

	//
	// Texture3d
	//

	glTexImage3D = (PFNGLTEXIMAGE3DPROC) O3D_GET_OPENGL_PROC("glTexImage3D");
	glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC) O3D_GET_OPENGL_PROC("glTexSubImage3D");
	if ((!glTexImage3D)|| (!glTexSubImage3D))
		O3D_ERROR(E_NullPointer("OpenGL glTexImage3D function"));
#endif // O3D_GL_VERSION_1_2

#ifdef O3D_GL_VERSION_1_3

	//
	// multi-texture extension
	//

	glActiveTexture = (PFNGLACTIVETEXTUREPROC) O3D_GET_OPENGL_PROC("glActiveTexture");

	if (!glActiveTexture)
	{
		if (isExtensionSupported("GL_ARB_multitexture"))
			glActiveTexture = (PFNGLACTIVETEXTUREPROC) O3D_GET_OPENGL_PROC("glActiveTextureARB");

		if (!glActiveTexture)
			O3D_ERROR(E_NullPointer("OpenGL glActiveTexture function"));
	}

	//
	// Compressed texture image
	//

	glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC) O3D_GET_OPENGL_PROC("glCompressedTexImage1D");
	glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) O3D_GET_OPENGL_PROC("glCompressedTexImage2D");
	glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC) O3D_GET_OPENGL_PROC("glCompressedTexImage3D");
	glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) O3D_GET_OPENGL_PROC("glCompressedTexSubImage3D");
	glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) O3D_GET_OPENGL_PROC("glCompressedTexSubImage2D");
	glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) O3D_GET_OPENGL_PROC("glCompressedTexSubImage1D");
	glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC) O3D_GET_OPENGL_PROC("glGetCompressedTexImage");

	if ((!glCompressedTexImage1D) || (!glCompressedTexImage2D) || (!glCompressedTexImage3D) ||
		(!glCompressedTexSubImage3D) || (!glCompressedTexSubImage2D) || (!glCompressedTexSubImage1D) ||
		(!glGetCompressedTexImage))
	{
		if (isExtensionSupported("GL_ARB_texture_compression"))
		{
			glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC) O3D_GET_OPENGL_PROC("glCompressedTexImage1DARB");
			glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) O3D_GET_OPENGL_PROC("glCompressedTexImage2DARB");
			glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC) O3D_GET_OPENGL_PROC("glCompressedTexImage3DARB");
			glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) O3D_GET_OPENGL_PROC("glCompressedTexSubImage3DARB");
			glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) O3D_GET_OPENGL_PROC("glCompressedTexSubImage2DARB");
			glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) O3D_GET_OPENGL_PROC("glCompressedTexSubImage1DARB");
			glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC) O3D_GET_OPENGL_PROC("glGetCompressedTexImageARB");
		}

		if ((!glCompressedTexImage1D) || (!glCompressedTexImage2D) || (!glCompressedTexImage3D) ||
			(!glCompressedTexSubImage3D) || (!glCompressedTexSubImage2D) || (!glCompressedTexSubImage1D) ||
			(!glGetCompressedTexImage))
			O3D_ERROR(E_NullPointer("OpenGL compressed texture related functions"));
	}
#endif // O3D_GL_VERSION_1_3

#ifdef O3D_GL_VERSION_1_4

	//
	// PointParametersSupported
	//

	glPointParameterf  = (PFNGLPOINTPARAMETERFPROC)  O3D_GET_OPENGL_PROC("glPointParameterf");
	glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC) O3D_GET_OPENGL_PROC("glPointParameterfv");
    glPointParameteri  = (PFNGLPOINTPARAMETERIPROC)  O3D_GET_OPENGL_PROC("glPointParameteri");
    glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC) O3D_GET_OPENGL_PROC("glPointParameteriv");

    if (!glPointParameterf || !glPointParameterfv || !glPointParameteri || !glPointParameteriv)
    {
        if (isExtensionSupported("GL_ARB_point_parameters"))
        {
            glPointParameterf  = (PFNGLPOINTPARAMETERFPROC)  O3D_GET_OPENGL_PROC("glPointParameterfARB");
            glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC) O3D_GET_OPENGL_PROC("glPointParameterfvARB");
            glPointParameteri  = (PFNGLPOINTPARAMETERIPROC)  O3D_GET_OPENGL_PROC("glPointParameteriARB");
            glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC) O3D_GET_OPENGL_PROC("glPointParameterivARB");

            if (!glPointParameterf || !glPointParameterfv || !glPointParameteri || !glPointParameteriv)
                O3D_ERROR(E_NullPointer("OpenGL point parameter related functions"));
        }
    }

    glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) O3D_GET_OPENGL_PROC("glBlendFuncSeparate");

    if (!glBlendFuncSeparate)
        O3D_ERROR(E_NullPointer("OpenGL blend func separate function"));

    glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC) O3D_GET_OPENGL_PROC("glMultiDrawArrays");
    glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC) O3D_GET_OPENGL_PROC("glMultiDrawElements");

    if (!glMultiDrawArrays || !glMultiDrawElements)
        O3D_ERROR(E_NullPointer("OpenGL multi draw function"));

    glBlendEquation = (PFNGLBLENDEQUATIONPROC) O3D_GET_OPENGL_PROC("glBlendEquation");

    if (!glBlendEquation)
        O3D_ERROR(E_NullPointer("OpenGL blend equation function"));

#endif // O3D_GL_VERSION_1_4

#ifdef O3D_GL_VERSION_1_5
	//
	// Vertex Buffer Object
	//

	glBindBuffer		= (PFNGLBINDBUFFERPROC)	    O3D_GET_OPENGL_PROC("glBindBuffer");
	glGenBuffers		= (PFNGLGENBUFFERSPROC)	    O3D_GET_OPENGL_PROC("glGenBuffers");
	glBufferData		= (PFNGLBUFFERDATAPROC)	    O3D_GET_OPENGL_PROC("glBufferData");
	glBufferSubData		= (PFNGLBUFFERSUBDATAPROC)  O3D_GET_OPENGL_PROC("glBufferSubData");
	glDeleteBuffers		= (PFNGLDELETEBUFFERSPROC)  O3D_GET_OPENGL_PROC("glDeleteBuffers");
    glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC) O3D_GET_OPENGL_PROC("glGetBufferParameteriv");
	glMapBuffer			= (PFNGLMAPBUFFERPROC)      O3D_GET_OPENGL_PROC("glMapBuffer");
	glUnmapBuffer		= (PFNGLUNMAPBUFFERPROC)    O3D_GET_OPENGL_PROC("glUnmapBuffer");

	if ((!glBindBuffer) || (!glDeleteBuffers) || (!glGenBuffers) || (!glBufferData) ||
	    (!glGetBufferParameteriv) || (!glMapBuffer) || (!glUnmapBuffer))
	{
		if (isExtensionSupported("GL_ARB_vertex_buffer_object"))
		{
			glBindBuffer			= (PFNGLBINDBUFFERPROC)	     O3D_GET_OPENGL_PROC("glBindBufferARB");
			glGenBuffers			= (PFNGLGENBUFFERSPROC)	     O3D_GET_OPENGL_PROC("glGenBuffersARB");
			glBufferData			= (PFNGLBUFFERDATAPROC)	     O3D_GET_OPENGL_PROC("glBufferDataARB");
			glBufferSubData			= (PFNGLBUFFERSUBDATAPROC)   O3D_GET_OPENGL_PROC("glBufferSubDataARB");
			glDeleteBuffers			= (PFNGLDELETEBUFFERSPROC)   O3D_GET_OPENGL_PROC("glDeleteBuffersARB");
            glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC) O3D_GET_OPENGL_PROC("glGetBufferParameterivARB");
			glMapBuffer		        = (PFNGLMAPBUFFERPROC)       O3D_GET_OPENGL_PROC("glMapBufferARB");
			glUnmapBuffer			= (PFNGLUNMAPBUFFERPROC)     O3D_GET_OPENGL_PROC("glUnmapBufferARB");
		}

		if ((!glBindBuffer) || (!glDeleteBuffers) || (!glGenBuffers) || (!glBufferData) ||
			(!glGetBufferParameteriv) || (!glMapBuffer) || (!glUnmapBuffer))
				O3D_ERROR(E_NullPointer("OpenGL VBO related functions"));
	}

	// OcclusionQuerySupported
	glGenQueries	= (PFNGLGENQUERIESPROC)O3D_GET_OPENGL_PROC("glGenQueries");
	glIsQuery		= (PFNGLISQUERYPROC)O3D_GET_OPENGL_PROC("glIsQuery");
	glBeginQuery	= (PFNGLBEGINQUERYPROC)O3D_GET_OPENGL_PROC("glBeginQuery");
	glEndQuery		= (PFNGLENDQUERYPROC)O3D_GET_OPENGL_PROC("glEndQuery");
	glGetQueryiv		= (PFNGLGETQUERYIVPROC)O3D_GET_OPENGL_PROC("glGetQueryiv");
	glGetQueryObjectiv	= (PFNGLGETQUERYOBJECTIVPROC)O3D_GET_OPENGL_PROC("glGetQueryObjectiv");
	glGetQueryObjectuiv	= (PFNGLGETQUERYOBJECTUIVPROC)O3D_GET_OPENGL_PROC("glGetQueryObjectuiv");
	glDeleteQueries		= (PFNGLDELETEQUERIESPROC)O3D_GET_OPENGL_PROC("glDeleteQueries");

	if ((!glGenQueries) || (!glIsQuery) || (!glBeginQuery) || (!glEndQuery) || (!glGetQueryiv) ||
	    (!glGetQueryObjectiv) || (!glGetQueryObjectuiv) || (!glDeleteQueries))
	{
		if (isExtensionSupported("GL_ARB_occlusion_query"))
		{
			glGenQueries	= (PFNGLGENQUERIESPROC)O3D_GET_OPENGL_PROC("glGenQueriesARB");
			glIsQuery		= (PFNGLISQUERYPROC)O3D_GET_OPENGL_PROC("glIsQueryARB");
			glBeginQuery	= (PFNGLBEGINQUERYPROC)O3D_GET_OPENGL_PROC("glBeginQueryARB");
			glEndQuery		= (PFNGLENDQUERYPROC)O3D_GET_OPENGL_PROC("glEndQueryARB");
			glGetQueryiv		= (PFNGLGETQUERYIVPROC)O3D_GET_OPENGL_PROC("glGetQueryivARB");
			glGetQueryObjectiv	= (PFNGLGETQUERYOBJECTIVPROC)O3D_GET_OPENGL_PROC("glGetQueryObjectivARB");
			glGetQueryObjectuiv	= (PFNGLGETQUERYOBJECTUIVPROC)O3D_GET_OPENGL_PROC("glGetQueryObjectuivARB");
			glDeleteQueries		= (PFNGLDELETEQUERIESPROC)O3D_GET_OPENGL_PROC("glDeleteQueriesARB");
		}

		if ((!glGenQueries) || (!glIsQuery) || (!glBeginQuery) || (!glEndQuery) || (!glGetQueryiv) ||
			(!glGetQueryObjectiv) || (!glGetQueryObjectuiv) || (!glDeleteQueries))
				O3D_ERROR(E_NullPointer("OpenGL occlusion query related functions"));
	}
#endif // O3D_GL_VERSION_1_5

#ifdef O3D_GL_VERSION_2_0

	//
	// GLSL
	//

	glCreateShader		= (PFNGLCREATESHADERPROC)	O3D_GET_OPENGL_PROC("glCreateShader");
	glShaderSource		= (PFNGLSHADERSOURCEPROC)	O3D_GET_OPENGL_PROC("glShaderSource");
	glCompileShader		= (PFNGLCOMPILESHADERPROC)	O3D_GET_OPENGL_PROC("glCompileShader");
	glCreateProgram		= (PFNGLCREATEPROGRAMPROC)	O3D_GET_OPENGL_PROC("glCreateProgram");
	glAttachShader		= (PFNGLATTACHSHADERPROC)	O3D_GET_OPENGL_PROC("glAttachShader");
	glLinkProgram		= (PFNGLLINKPROGRAMPROC)	O3D_GET_OPENGL_PROC("glLinkProgram");
	glValidateProgram	= (PFNGLVALIDATEPROGRAMPROC)O3D_GET_OPENGL_PROC("glValidateProgram");
	glUseProgram		= (PFNGLUSEPROGRAMPROC)		O3D_GET_OPENGL_PROC("glUseProgram");

	glDetachShader		= (PFNGLDETACHSHADERPROC)	O3D_GET_OPENGL_PROC("glDetachShader");
	glDeleteShader		= (PFNGLDELETESHADERPROC)	O3D_GET_OPENGL_PROC("glDeleteShader");
	glDeleteProgram		= (PFNGLDELETEPROGRAMPROC)	O3D_GET_OPENGL_PROC("glDeleteProgram");

	glGetProgramiv		= (PFNGLGETPROGRAMIVPROC)	O3D_GET_OPENGL_PROC("glGetProgramiv");
	glGetProgramInfoLog	= (PFNGLGETPROGRAMINFOLOGPROC)	O3D_GET_OPENGL_PROC("glGetProgramInfoLog");
	glGetShaderiv		= (PFNGLGETSHADERIVPROC)	O3D_GET_OPENGL_PROC("glGetShaderiv");
	glGetShaderInfoLog	= (PFNGLGETSHADERINFOLOGPROC)	O3D_GET_OPENGL_PROC("glGetShaderInfoLog");
	glIsProgram		= (PFNGLISPROGRAMPROC)		O3D_GET_OPENGL_PROC("glIsProgram");
	glIsShader		= (PFNGLISSHADERPROC)		O3D_GET_OPENGL_PROC("glIsShader");

	glGetUniformLocation= (PFNGLGETUNIFORMLOCATIONPROC)	O3D_GET_OPENGL_PROC("glGetUniformLocation");
	glGetActiveUniform  = (PFNGLGETACTIVEUNIFORMPROC)   O3D_GET_OPENGL_PROC("glGetActiveUniform");
	glUniform1f		= (PFNGLUNIFORM1FPROC)		O3D_GET_OPENGL_PROC("glUniform1f");
	glUniform2f		= (PFNGLUNIFORM2FPROC)		O3D_GET_OPENGL_PROC("glUniform2f");
	glUniform3f		= (PFNGLUNIFORM3FPROC)		O3D_GET_OPENGL_PROC("glUniform3f");
	glUniform4f		= (PFNGLUNIFORM4FPROC)		O3D_GET_OPENGL_PROC("glUniform4f");
	glUniform1fv	= (PFNGLUNIFORM1FVPROC)		O3D_GET_OPENGL_PROC("glUniform1fv");
	glUniform2fv	= (PFNGLUNIFORM2FVPROC)		O3D_GET_OPENGL_PROC("glUniform2fv");
	glUniform3fv	= (PFNGLUNIFORM3FVPROC)		O3D_GET_OPENGL_PROC("glUniform3fv");
	glUniform4fv	= (PFNGLUNIFORM4FVPROC)		O3D_GET_OPENGL_PROC("glUniform4fv");
	glUniform1i		= (PFNGLUNIFORM1IPROC)		O3D_GET_OPENGL_PROC("glUniform1i");
	glUniform2i		= (PFNGLUNIFORM2IPROC)		O3D_GET_OPENGL_PROC("glUniform2i");
	glUniform3i		= (PFNGLUNIFORM3IPROC)		O3D_GET_OPENGL_PROC("glUniform3i");
	glUniform4i		= (PFNGLUNIFORM4IPROC)		O3D_GET_OPENGL_PROC("glUniform4i");
	glUniformMatrix2fv	= (PFNGLUNIFORMMATRIX2FVPROC)	O3D_GET_OPENGL_PROC("glUniformMatrix2fv");
	glUniformMatrix3fv	= (PFNGLUNIFORMMATRIX3FVPROC)	O3D_GET_OPENGL_PROC("glUniformMatrix3fv");
	glUniformMatrix4fv	= (PFNGLUNIFORMMATRIX4FVPROC)	O3D_GET_OPENGL_PROC("glUniformMatrix4fv");

	glDisableVertexAttribArray	= (PFNGLDISABLEVERTEXATTRIBARRAYPROC)	O3D_GET_OPENGL_PROC("glDisableVertexAttribArray");
	glEnableVertexAttribArray	= (PFNGLENABLEVERTEXATTRIBARRAYPROC)	O3D_GET_OPENGL_PROC("glEnableVertexAttribArray");
	glGetActiveAttrib		= (PFNGLGETACTIVEATTRIBPROC)		O3D_GET_OPENGL_PROC("glGetActiveAttrib");
	glBindAttribLocation		= (PFNGLBINDATTRIBLOCATIONPROC)		O3D_GET_OPENGL_PROC("glBindAttribLocation");
	glGetAttribLocation		= (PFNGLGETATTRIBLOCATIONPROC)		O3D_GET_OPENGL_PROC("glGetAttribLocation");
	glGetVertexAttribdv		= (PFNGLGETVERTEXATTRIBDVPROC)		O3D_GET_OPENGL_PROC("glGetVertexAttribdv");
	glGetVertexAttribfv		= (PFNGLGETVERTEXATTRIBFVPROC)		O3D_GET_OPENGL_PROC("glGetVertexAttribfv");
	glGetVertexAttribiv		= (PFNGLGETVERTEXATTRIBIVPROC)		O3D_GET_OPENGL_PROC("glGetVertexAttribiv");
	glGetVertexAttribPointerv	= (PFNGLGETVERTEXATTRIBPOINTERVPROC)	O3D_GET_OPENGL_PROC("glGetVertexAttribPointerv");

	glVertexAttrib1d		= (PFNGLVERTEXATTRIB1DPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib1d");
	glVertexAttrib1dv		= (PFNGLVERTEXATTRIB1DVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib1dv");
	glVertexAttrib1f		= (PFNGLVERTEXATTRIB1FPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib1f");
	glVertexAttrib1fv		= (PFNGLVERTEXATTRIB1FVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib1fv");
	glVertexAttrib1s		= (PFNGLVERTEXATTRIB1SPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib1s");
	glVertexAttrib1sv		= (PFNGLVERTEXATTRIB1SVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib1sv");
	glVertexAttrib2d		= (PFNGLVERTEXATTRIB2DPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib2d");
	glVertexAttrib2dv		= (PFNGLVERTEXATTRIB2DVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib2dv");
	glVertexAttrib2f		= (PFNGLVERTEXATTRIB2FPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib2f");
	glVertexAttrib2fv		= (PFNGLVERTEXATTRIB2FVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib2fv");
	glVertexAttrib2s		= (PFNGLVERTEXATTRIB2SPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib2s");
	glVertexAttrib2sv		= (PFNGLVERTEXATTRIB2SVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib2sv");
	glVertexAttrib3d		= (PFNGLVERTEXATTRIB3DPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib3d");
	glVertexAttrib3dv		= (PFNGLVERTEXATTRIB3DVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib3dv");
	glVertexAttrib3f		= (PFNGLVERTEXATTRIB3FPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib3f");
	glVertexAttrib3fv		= (PFNGLVERTEXATTRIB3FVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib3fv");
	glVertexAttrib3s		= (PFNGLVERTEXATTRIB3SPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib3s");
	glVertexAttrib3sv		= (PFNGLVERTEXATTRIB3SVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib3sv");
	glVertexAttrib4Nbv		= (PFNGLVERTEXATTRIB4NBVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4Nbv");
	glVertexAttrib4Niv		= (PFNGLVERTEXATTRIB4NIVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4Niv");
	glVertexAttrib4Nsv		= (PFNGLVERTEXATTRIB4NSVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4Nsv");
	glVertexAttrib4Nub		= (PFNGLVERTEXATTRIB4NUBPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4Nub");
	glVertexAttrib4Nubv		= (PFNGLVERTEXATTRIB4NUBVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4Nubv");
	glVertexAttrib4Nuiv		= (PFNGLVERTEXATTRIB4NUIVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4Nuiv");
	glVertexAttrib4Nusv		= (PFNGLVERTEXATTRIB4NUSVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4Nusv");
	glVertexAttrib4bv		= (PFNGLVERTEXATTRIB4BVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4bv");
	glVertexAttrib4d		= (PFNGLVERTEXATTRIB4DPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4d");
	glVertexAttrib4dv		= (PFNGLVERTEXATTRIB4DVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4dv");
	glVertexAttrib4f		= (PFNGLVERTEXATTRIB4FPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4f");
	glVertexAttrib4fv		= (PFNGLVERTEXATTRIB4FVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4fv");
	glVertexAttrib4iv		= (PFNGLVERTEXATTRIB4IVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4iv");
	glVertexAttrib4s		= (PFNGLVERTEXATTRIB4SPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4s");
	glVertexAttrib4sv		= (PFNGLVERTEXATTRIB4SVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4sv");
	glVertexAttrib4ubv		= (PFNGLVERTEXATTRIB4UBVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4ubv");
	glVertexAttrib4uiv		= (PFNGLVERTEXATTRIB4UIVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4uiv");
	glVertexAttrib4usv		= (PFNGLVERTEXATTRIB4USVPROC)		O3D_GET_OPENGL_PROC("glVertexAttrib4usv");
	glVertexAttribPointer	= (PFNGLVERTEXATTRIBPOINTERPROC)		O3D_GET_OPENGL_PROC("glVertexAttribPointer");

	if ((!glCreateShader) || (!glShaderSource) || (!glCompileShader) || (!glCreateProgram) ||
		(!glAttachShader) || (!glLinkProgram) || (!glValidateProgram) || (!glUseProgram) ||
		(!glDetachShader) || (!glDeleteShader) || (!glDeleteProgram) || (!glGetProgramInfoLog) ||
		(!glGetShaderInfoLog) || (!glIsProgram) || (!glIsShader) || (!glGetUniformLocation) ||
		(!glGetProgramiv) || (!glUniform1f) || (!glUniform2f) || (!glUniform3f) || (!glUniform4f) ||
		(!glUniform1fv) || (!glUniform2fv) || (!glUniform3fv) || (!glUniform4fv) ||
		(!glUniform1i) || (!glUniform2i) || (!glUniform3i) || (!glUniform4i) ||
		(!glUniformMatrix2fv) || (!glUniformMatrix3fv) || (!glUniformMatrix4fv) ||
		(!glGetShaderiv) || (!glGetActiveUniform) ||
		(!glBindAttribLocation) || (!glGetAttribLocation) || (!glGetVertexAttribdv) || (!glGetVertexAttribfv) ||
		(!glGetVertexAttribiv) || (!glGetVertexAttribPointerv) || (!glVertexAttrib1d) || (!glVertexAttrib1dv) ||
		(!glVertexAttrib1f) || (!glVertexAttrib1fv) || (!glVertexAttrib1s) || (!glVertexAttrib1sv) ||
		(!glVertexAttrib2d) || (!glVertexAttrib2dv) || (!glVertexAttrib2f) || (!glVertexAttrib2fv) ||
		(!glVertexAttrib2s) || (!glVertexAttrib2sv) || (!glVertexAttrib3d) || (!glVertexAttrib3dv) ||
		(!glVertexAttrib3f) || (!glVertexAttrib3fv) || (!glVertexAttrib3s) || (!glVertexAttrib3sv) ||
		(!glVertexAttrib4Nbv) || (!glVertexAttrib4Niv) || (!glVertexAttrib4Nsv) || (!glVertexAttrib4Nub) ||
		(!glVertexAttrib4Nubv) || (!glVertexAttrib4Nuiv) || (!glVertexAttrib4Nusv) || (!glVertexAttrib4bv) ||
		(!glVertexAttrib4d) || (!glVertexAttrib4dv) || (!glVertexAttrib4f) || (!glVertexAttrib4fv) ||
		(!glVertexAttrib4iv) || (!glVertexAttrib4s) || (!glVertexAttrib4sv) || (!glVertexAttrib4ubv) ||
		(!glVertexAttrib4uiv) || (!glVertexAttrib4usv) || (!glVertexAttribPointer))
	{
		if (isExtensionSupported("GL_ARB_shading_language_100"))
		{
			glCreateShader		= (PFNGLCREATESHADERPROC)	O3D_GET_OPENGL_PROC("glCreateShaderObjectARB");
			glShaderSource		= (PFNGLSHADERSOURCEPROC)	O3D_GET_OPENGL_PROC("glShaderSourceARB");
			glCompileShader		= (PFNGLCOMPILESHADERPROC)	O3D_GET_OPENGL_PROC("glCompileShaderARB");
			glCreateProgram		= (PFNGLCREATEPROGRAMPROC)	O3D_GET_OPENGL_PROC("glCreateProgramObjectARB");
			glAttachShader		= (PFNGLATTACHSHADERPROC)	O3D_GET_OPENGL_PROC("glAttachObjectARB");
			glLinkProgram		= (PFNGLLINKPROGRAMPROC)	O3D_GET_OPENGL_PROC("glLinkProgramARB");
			glValidateProgram	= (PFNGLVALIDATEPROGRAMPROC)O3D_GET_OPENGL_PROC("glValidateProgramARB");
			glUseProgram		= (PFNGLUSEPROGRAMPROC)		O3D_GET_OPENGL_PROC("glUseProgramObjectARB");

			glDetachShader		= (PFNGLDETACHSHADERPROC)	O3D_GET_OPENGL_PROC("glDetachObjectARB");
			glDeleteShader		= (PFNGLDELETESHADERPROC)	O3D_GET_OPENGL_PROC("glDeleteObjectARB");
			glDeleteProgram		= (PFNGLDELETEPROGRAMPROC)	O3D_GET_OPENGL_PROC("glDeleteObjectARB");

			glGetProgramiv		= (PFNGLGETPROGRAMIVPROC)	O3D_GET_OPENGL_PROC("glGetObjectParameterivARB");
			glGetProgramInfoLog	= (PFNGLGETPROGRAMINFOLOGPROC)	O3D_GET_OPENGL_PROC("glGetInfoLogARB");
			glGetShaderiv		= (PFNGLGETSHADERIVPROC)	O3D_GET_OPENGL_PROC("glGetObjectParameterivARB");
			glGetShaderInfoLog	= (PFNGLGETSHADERINFOLOGPROC)	O3D_GET_OPENGL_PROC("glGetInfoLogARB");
			glIsProgram		= (PFNGLISPROGRAMPROC)		O3D_GET_OPENGL_PROC("glIsProgramARB"); // Not sure
			glIsShader		= (PFNGLISSHADERPROC)		O3D_GET_OPENGL_PROC("glIsProgramARB"); // Not sure

			glGetUniformLocation    = (PFNGLGETUNIFORMLOCATIONPROC)	O3D_GET_OPENGL_PROC("glGetUniformLocationARB");
			glGetActiveUniform      = (PFNGLGETACTIVEUNIFORMPROC)   O3D_GET_OPENGL_PROC("glGetActiveUniformARB");
			glUniform1f		= (PFNGLUNIFORM1FPROC)		O3D_GET_OPENGL_PROC("glUniform1fARB");
			glUniform2f		= (PFNGLUNIFORM2FPROC)		O3D_GET_OPENGL_PROC("glUniform2fARB");
			glUniform3f		= (PFNGLUNIFORM3FPROC)		O3D_GET_OPENGL_PROC("glUniform3fARB");
			glUniform4f		= (PFNGLUNIFORM4FPROC)		O3D_GET_OPENGL_PROC("glUniform4fARB");
			glUniform1fv	= (PFNGLUNIFORM1FVPROC)		O3D_GET_OPENGL_PROC("glUniform1fvARB");
			glUniform2fv	= (PFNGLUNIFORM2FVPROC)		O3D_GET_OPENGL_PROC("glUniform2fvARB");
			glUniform3fv	= (PFNGLUNIFORM3FVPROC)		O3D_GET_OPENGL_PROC("glUniform3fvARB");
			glUniform4fv	= (PFNGLUNIFORM4FVPROC)		O3D_GET_OPENGL_PROC("glUniform4fvARB");
			glUniform1i		= (PFNGLUNIFORM1IPROC)		O3D_GET_OPENGL_PROC("glUniform1iARB");
			glUniform2i		= (PFNGLUNIFORM2IPROC)		O3D_GET_OPENGL_PROC("glUniform2iARB");
			glUniform3i		= (PFNGLUNIFORM3IPROC)		O3D_GET_OPENGL_PROC("glUniform3iARB");
			glUniform4i		= (PFNGLUNIFORM4IPROC)		O3D_GET_OPENGL_PROC("glUniform4iARB");
			glUniformMatrix2fv	= (PFNGLUNIFORMMATRIX2FVPROC)	O3D_GET_OPENGL_PROC("glUniformMatrix2fvARB");
			glUniformMatrix3fv	= (PFNGLUNIFORMMATRIX3FVPROC)	O3D_GET_OPENGL_PROC("glUniformMatrix3fvARB");
			glUniformMatrix4fv	= (PFNGLUNIFORMMATRIX4FVPROC)	O3D_GET_OPENGL_PROC("glUniformMatrix4fvARB");

			glDisableVertexAttribArray	= (PFNGLDISABLEVERTEXATTRIBARRAYPROC)	O3D_GET_OPENGL_PROC("glDisableVertexAttribArrayARB");
			glEnableVertexAttribArray	= (PFNGLENABLEVERTEXATTRIBARRAYPROC)	O3D_GET_OPENGL_PROC("glEnableVertexAttribArrayARB");
			glGetActiveAttrib		= (PFNGLGETACTIVEATTRIBPROC)		O3D_GET_OPENGL_PROC("glGetActiveAttribARB");
			glBindAttribLocation		= (PFNGLBINDATTRIBLOCATIONPROC)		O3D_GET_OPENGL_PROC("glBindAttribLocationARB");
			glGetAttribLocation		= (PFNGLGETATTRIBLOCATIONPROC)		O3D_GET_OPENGL_PROC("glGetAttribLocationARB");
			glGetVertexAttribdv		= (PFNGLGETVERTEXATTRIBDVPROC)		O3D_GET_OPENGL_PROC("glGetVertexAttribdvARB");
			glGetVertexAttribfv		= (PFNGLGETVERTEXATTRIBFVPROC)		O3D_GET_OPENGL_PROC("glGetVertexAttribfvARB");
			glGetVertexAttribiv		= (PFNGLGETVERTEXATTRIBIVPROC)		O3D_GET_OPENGL_PROC("glGetVertexAttribivARB");
			glGetVertexAttribPointerv	= (PFNGLGETVERTEXATTRIBPOINTERVPROC)	O3D_GET_OPENGL_PROC("glGetVertexAttribPointervARB");

			glVertexAttrib1d		= (PFNGLVERTEXATTRIB1DPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib1dARB");
			glVertexAttrib1dv		= (PFNGLVERTEXATTRIB1DVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib1dvARB");
			glVertexAttrib1f		= (PFNGLVERTEXATTRIB1FPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib1fARB");
			glVertexAttrib1fv		= (PFNGLVERTEXATTRIB1FVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib1fvARB");
			glVertexAttrib1s		= (PFNGLVERTEXATTRIB1SPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib1sARB");
			glVertexAttrib1sv		= (PFNGLVERTEXATTRIB1SVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib1svARB");
			glVertexAttrib2d		= (PFNGLVERTEXATTRIB2DPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib2dARB");
			glVertexAttrib2dv		= (PFNGLVERTEXATTRIB2DVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib2dvARB");
			glVertexAttrib2f		= (PFNGLVERTEXATTRIB2FPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib2fARB");
			glVertexAttrib2fv		= (PFNGLVERTEXATTRIB2FVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib2fvARB");
			glVertexAttrib2s		= (PFNGLVERTEXATTRIB2SPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib2sARB");
			glVertexAttrib2sv		= (PFNGLVERTEXATTRIB2SVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib2svARB");
			glVertexAttrib3d		= (PFNGLVERTEXATTRIB3DPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib3dARB");
			glVertexAttrib3dv		= (PFNGLVERTEXATTRIB3DVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib3dvARB");
			glVertexAttrib3f		= (PFNGLVERTEXATTRIB3FPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib3fARB");
			glVertexAttrib3fv		= (PFNGLVERTEXATTRIB3FVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib3fvARB");
			glVertexAttrib3s		= (PFNGLVERTEXATTRIB3SPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib3sARB");
			glVertexAttrib3sv		= (PFNGLVERTEXATTRIB3SVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib3svARB");
			glVertexAttrib4Nbv		= (PFNGLVERTEXATTRIB4NBVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4NbvARB");
			glVertexAttrib4Niv		= (PFNGLVERTEXATTRIB4NIVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4NivARB");
			glVertexAttrib4Nsv		= (PFNGLVERTEXATTRIB4NSVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4NsvARB");
			glVertexAttrib4Nub		= (PFNGLVERTEXATTRIB4NUBPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4NubARB");
			glVertexAttrib4Nubv		= (PFNGLVERTEXATTRIB4NUBVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4NubvARB");
			glVertexAttrib4Nuiv		= (PFNGLVERTEXATTRIB4NUIVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4NuivARB");
			glVertexAttrib4Nusv		= (PFNGLVERTEXATTRIB4NUSVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4NusvARB");
			glVertexAttrib4bv		= (PFNGLVERTEXATTRIB4BVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4bvARB");
			glVertexAttrib4d		= (PFNGLVERTEXATTRIB4DPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4dARB");
			glVertexAttrib4dv		= (PFNGLVERTEXATTRIB4DVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4dvARB");
			glVertexAttrib4f		= (PFNGLVERTEXATTRIB4FPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4fARB");
			glVertexAttrib4fv		= (PFNGLVERTEXATTRIB4FVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4fvARB");
			glVertexAttrib4iv		= (PFNGLVERTEXATTRIB4IVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4ivARB");
			glVertexAttrib4s		= (PFNGLVERTEXATTRIB4SPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4sARB");
			glVertexAttrib4sv		= (PFNGLVERTEXATTRIB4SVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4svARB");
			glVertexAttrib4ubv		= (PFNGLVERTEXATTRIB4UBVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4ubvARB");
			glVertexAttrib4uiv		= (PFNGLVERTEXATTRIB4UIVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4uivARB");
			glVertexAttrib4usv		= (PFNGLVERTEXATTRIB4USVPROC)	O3D_GET_OPENGL_PROC("glVertexAttrib4usvARB");
			glVertexAttribPointer	= (PFNGLVERTEXATTRIBPOINTERPROC)O3D_GET_OPENGL_PROC("glVertexAttribPointerARB");
		}

		if ((!glCreateShader) || (!glShaderSource) || (!glCompileShader) || (!glCreateProgram) ||
			(!glAttachShader) || (!glLinkProgram) || (!glValidateProgram) || (!glUseProgram) ||
			(!glDetachShader) || (!glDeleteShader) || (!glDeleteProgram) || (!glGetProgramInfoLog) ||
			(!glGetShaderInfoLog) || (!glIsProgram) || (!glIsShader) || (!glGetUniformLocation) ||
			(!glGetProgramiv) || (!glUniform1f) || (!glUniform2f) || (!glUniform3f) || (!glUniform4f) ||
			(!glUniform1fv) || (!glUniform2fv) || (!glUniform3fv) || (!glUniform4fv) ||
			(!glUniform1i) || (!glUniform2i) || (!glUniform3i) || (!glUniform4i) ||
			(!glUniformMatrix2fv) || (!glUniformMatrix3fv) || (!glUniformMatrix4fv) ||
			(!glGetShaderiv) || (!glGetActiveUniform) ||
			(!glBindAttribLocation) || (!glGetAttribLocation) || (!glGetVertexAttribdv) || (!glGetVertexAttribfv) ||
			(!glGetVertexAttribiv) || (!glGetVertexAttribPointerv) || (!glVertexAttrib1d) || (!glVertexAttrib1dv) ||
			(!glVertexAttrib1f) || (!glVertexAttrib1fv) || (!glVertexAttrib1s) || (!glVertexAttrib1sv) ||
			(!glVertexAttrib2d) || (!glVertexAttrib2dv) || (!glVertexAttrib2f) || (!glVertexAttrib2fv) ||
			(!glVertexAttrib2s) || (!glVertexAttrib2sv) || (!glVertexAttrib3d) || (!glVertexAttrib3dv) ||
			(!glVertexAttrib3f) || (!glVertexAttrib3fv) || (!glVertexAttrib3s) || (!glVertexAttrib3sv) ||
			(!glVertexAttrib4Nbv) || (!glVertexAttrib4Niv) || (!glVertexAttrib4Nsv) || (!glVertexAttrib4Nub) ||
			(!glVertexAttrib4Nubv) || (!glVertexAttrib4Nuiv) || (!glVertexAttrib4Nusv) || (!glVertexAttrib4bv) ||
			(!glVertexAttrib4d) || (!glVertexAttrib4dv) || (!glVertexAttrib4f) || (!glVertexAttrib4fv) ||
			(!glVertexAttrib4iv) || (!glVertexAttrib4s) || (!glVertexAttrib4sv) || (!glVertexAttrib4ubv) ||
			(!glVertexAttrib4uiv) || (!glVertexAttrib4usv) || (!glVertexAttribPointer))
				O3D_ERROR(E_NullPointer("OpenGL GLSL related functions"));
	}

    //
    // Blend equation separate
    //

    glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)  O3D_GET_OPENGL_PROC("glBlendEquationSeparate");

    if (!glBlendEquationSeparate)
        O3D_ERROR(E_NullPointer("OpenGL glBlendEquationSeparate function"));

	//
	// Draw buffers
	//

	glDrawBuffers = (PFNGLDRAWBUFFERSPROC)  O3D_GET_OPENGL_PROC("glDrawBuffers");

	if (!glDrawBuffers)
	{
		if (isExtensionSupported("GL_ARB_draw_buffers"))
		{
			glDrawBuffers = (PFNGLDRAWBUFFERSPROC) O3D_GET_OPENGL_PROC("glDrawBuffersARB");

			if ((!glDrawBuffers))
				O3D_ERROR(E_NullPointer("OpenGL glDrawBuffers function"));
		}
	}

    //
    // Stencil separate
    //

    glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)  O3D_GET_OPENGL_PROC("glStencilOpSeparate");
    glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)  O3D_GET_OPENGL_PROC("glStencilFuncSeparate");
    glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC)  O3D_GET_OPENGL_PROC("glStencilMaskSeparate");

    if (!glStencilOpSeparate || !glStencilFuncSeparate || !glStencilMaskSeparate)
        O3D_ERROR(E_NullPointer("OpenGL stencil separate functions"));

#endif // O3D_GL_VERSION_2_0

#ifdef O3D_GL_ARB_framebuffer_object

	//
	// Frame Buffer Object
	//

    glIsRenderbuffer 		= (PFNGLISRENDERBUFFERPROC) O3D_GET_OPENGL_PROC("glIsRenderbuffer");
    glBindRenderbuffer 		= (PFNGLBINDRENDERBUFFERPROC) O3D_GET_OPENGL_PROC("glBindRenderbuffer");
    glDeleteRenderbuffers 	= (PFNGLDELETERENDERBUFFERSPROC) O3D_GET_OPENGL_PROC("glDeleteRenderbuffers");
    glGenRenderbuffers 		= (PFNGLGENRENDERBUFFERSPROC) O3D_GET_OPENGL_PROC("glGenRenderbuffers");
    glRenderbufferStorage 	= (PFNGLRENDERBUFFERSTORAGEPROC) O3D_GET_OPENGL_PROC("glRenderbufferStorage");
	glGetRenderbufferParameteriv    = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) O3D_GET_OPENGL_PROC("glGetRenderbufferParameteriv");
    glIsFramebuffer 		= (PFNGLISFRAMEBUFFERPROC) O3D_GET_OPENGL_PROC("glIsFramebuffer");
    glBindFramebuffer 		= (PFNGLBINDFRAMEBUFFERPROC) O3D_GET_OPENGL_PROC("glBindFramebuffer");
	glDeleteFramebuffers 	= (PFNGLDELETEFRAMEBUFFERSPROC)	O3D_GET_OPENGL_PROC("glDeleteFramebuffers");
    glGenFramebuffers 		= (PFNGLGENFRAMEBUFFERSPROC) O3D_GET_OPENGL_PROC("glGenFramebuffers");
	glCheckFramebufferStatus 	= (PFNGLCHECKFRAMEBUFFERSTATUSPROC)	O3D_GET_OPENGL_PROC("glCheckFramebufferStatus");
    glFramebufferTexture1D 		= (PFNGLFRAMEBUFFERTEXTURE1DPROC) O3D_GET_OPENGL_PROC("glFramebufferTexture1D");
    glFramebufferTexture2D 		= (PFNGLFRAMEBUFFERTEXTURE2DPROC) O3D_GET_OPENGL_PROC("glFramebufferTexture2D");
    glFramebufferTexture3D 		= (PFNGLFRAMEBUFFERTEXTURE3DPROC) O3D_GET_OPENGL_PROC("glFramebufferTexture3D");
    glFramebufferRenderbuffer 	= (PFNGLFRAMEBUFFERRENDERBUFFERPROC) O3D_GET_OPENGL_PROC("glFramebufferRenderbuffer");
	glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)	O3D_GET_OPENGL_PROC("glGetFramebufferAttachmentParameteriv");
    glBlitFramebuffer 		= (PFNGLBLITFRAMEBUFFERPROC) O3D_GET_OPENGL_PROC("glBlitFramebuffer");
	glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) O3D_GET_OPENGL_PROC("glRenderbufferStorageMultisample");
    glGenerateMipmap 		= (PFNGLGENERATEMIPMAPPROC)	 O3D_GET_OPENGL_PROC("glGenerateMipmap");
    glFramebufferTextureLayer   = (PFNGLFRAMEBUFFERTEXTURELAYERPROC) O3D_GET_OPENGL_PROC("glFramebufferTextureLayer");

	if ((!glIsRenderbuffer) || (!glBindRenderbuffer) || (!glDeleteRenderbuffers) ||
		(!glGenRenderbuffers) || (!glRenderbufferStorage) || (!glGetRenderbufferParameteriv) ||
		(!glIsFramebuffer) || (!glBindFramebuffer) || (!glDeleteFramebuffers) ||
		(!glGenFramebuffers) || (!glCheckFramebufferStatus) || (!glFramebufferTexture1D) ||
		(!glFramebufferTexture2D) || (!glFramebufferTexture3D) || (!glFramebufferRenderbuffer) ||
		(!glGetFramebufferAttachmentParameteriv) || (!glBlitFramebuffer) ||
		(!glRenderbufferStorageMultisample) || (!glGenerateMipmap) ||(!glFramebufferTextureLayer))
	{
		if (isExtensionSupported("GL_ARB_framebuffer_object"))
		{
            glIsRenderbuffer 		= (PFNGLISRENDERBUFFERPROC)	O3D_GET_OPENGL_PROC("glIsRenderbufferARB");
            glBindRenderbuffer 		= (PFNGLBINDRENDERBUFFERPROC) O3D_GET_OPENGL_PROC("glBindRenderbufferARB");
            glDeleteRenderbuffers 	= (PFNGLDELETERENDERBUFFERSPROC) O3D_GET_OPENGL_PROC("glDeleteRenderbuffersARB");
            glGenRenderbuffers 		= (PFNGLGENRENDERBUFFERSPROC) O3D_GET_OPENGL_PROC("glGenRenderbuffersARB");
            glRenderbufferStorage 	= (PFNGLRENDERBUFFERSTORAGEPROC) O3D_GET_OPENGL_PROC("glRenderbufferStorageARB");
            glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) O3D_GET_OPENGL_PROC("glGetRenderbufferParameterivARB");
            glIsFramebuffer 		= (PFNGLISFRAMEBUFFERPROC) O3D_GET_OPENGL_PROC("glIsFramebufferARB");
            glBindFramebuffer 		= (PFNGLBINDFRAMEBUFFERPROC) O3D_GET_OPENGL_PROC("glBindFramebufferARB");
            glDeleteFramebuffers 	= (PFNGLDELETEFRAMEBUFFERSPROC)	O3D_GET_OPENGL_PROC("glDeleteFramebuffersARB");
            glGenFramebuffers 		= (PFNGLGENFRAMEBUFFERSPROC) O3D_GET_OPENGL_PROC("glGenFramebuffersARB");
			glCheckFramebufferStatus 	= (PFNGLCHECKFRAMEBUFFERSTATUSPROC)	O3D_GET_OPENGL_PROC("glCheckFramebufferStatusARB");
			glFramebufferTexture1D 		= (PFNGLFRAMEBUFFERTEXTURE1DPROC)	O3D_GET_OPENGL_PROC("glFramebufferTexture1DARB");
			glFramebufferTexture2D 		= (PFNGLFRAMEBUFFERTEXTURE2DPROC)	O3D_GET_OPENGL_PROC("glFramebufferTexture2DARB");
			glFramebufferTexture3D 		= (PFNGLFRAMEBUFFERTEXTURE3DPROC)	O3D_GET_OPENGL_PROC("glFramebufferTexture3DARB");
			glFramebufferRenderbuffer 	= (PFNGLFRAMEBUFFERRENDERBUFFERPROC)	O3D_GET_OPENGL_PROC("glFramebufferRenderbufferARB");
			glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)	O3D_GET_OPENGL_PROC("glGetFramebufferAttachmentParameterivARB");
			glBlitFramebuffer 		= (PFNGLBLITFRAMEBUFFERPROC)		O3D_GET_OPENGL_PROC("glBlitFramebufferARB");
			glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) O3D_GET_OPENGL_PROC("glRenderbufferStorageMultisampleARB");
			glGenerateMipmap 		= (PFNGLGENERATEMIPMAPPROC)	        O3D_GET_OPENGL_PROC("glGenerateMipmapARB");
			glFramebufferTextureLayer   = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)	O3D_GET_OPENGL_PROC("glFramebufferTextureLayerARB");
		}

		if ((!glIsRenderbuffer) || (!glBindRenderbuffer) || (!glDeleteRenderbuffers) ||
			(!glGenRenderbuffers) || (!glRenderbufferStorage) || (!glGetRenderbufferParameteriv) ||
			(!glIsFramebuffer) || (!glBindFramebuffer) || (!glDeleteFramebuffers) ||
			(!glGenFramebuffers) || (!glCheckFramebufferStatus) || (!glFramebufferTexture1D) ||
			(!glFramebufferTexture2D) || (!glFramebufferTexture3D) || (!glFramebufferRenderbuffer) ||
			(!glGetFramebufferAttachmentParameteriv) || (!glBlitFramebuffer) ||
			(!glRenderbufferStorageMultisample) || (!glGenerateMipmap) ||(!glFramebufferTextureLayer))
		{
			if (isExtensionSupported("GL_EXT_framebuffer_object"))
			{
				glIsRenderbuffer 	= (PFNGLISRENDERBUFFERPROC)		O3D_GET_OPENGL_PROC("glIsRenderbufferEXT");
				glBindRenderbuffer 	= (PFNGLBINDRENDERBUFFERPROC)		O3D_GET_OPENGL_PROC("glBindRenderbufferEXT");
				glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)	O3D_GET_OPENGL_PROC("glDeleteRenderbuffersEXT");
				glGenRenderbuffers 	= (PFNGLGENRENDERBUFFERSPROC)		O3D_GET_OPENGL_PROC("glGenRenderbuffersEXT");
				glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)	O3D_GET_OPENGL_PROC("glRenderbufferStorageEXT");
				glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) O3D_GET_OPENGL_PROC("glGetRenderbufferParameterivEXT");
				glIsFramebuffer 	= (PFNGLISFRAMEBUFFERPROC)		O3D_GET_OPENGL_PROC("glIsFramebufferEXT");
				glBindFramebuffer 	= (PFNGLBINDFRAMEBUFFERPROC)		O3D_GET_OPENGL_PROC("glBindFramebufferEXT");
				glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)		O3D_GET_OPENGL_PROC("glDeleteFramebuffersEXT");
				glGenFramebuffers 	= (PFNGLGENFRAMEBUFFERSPROC)		O3D_GET_OPENGL_PROC("glGenFramebuffersEXT");
				glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)	O3D_GET_OPENGL_PROC("glCheckFramebufferStatusEXT");
				glFramebufferTexture1D 	= (PFNGLFRAMEBUFFERTEXTURE1DPROC)	O3D_GET_OPENGL_PROC("glFramebufferTexture1DEXT");
				glFramebufferTexture2D 	= (PFNGLFRAMEBUFFERTEXTURE2DPROC)	O3D_GET_OPENGL_PROC("glFramebufferTexture2DEXT");
				glFramebufferTexture3D 	= (PFNGLFRAMEBUFFERTEXTURE3DPROC)	O3D_GET_OPENGL_PROC("glFramebufferTexture3DEXT");
				glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)	O3D_GET_OPENGL_PROC("glFramebufferRenderbufferEXT");
				glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)	O3D_GET_OPENGL_PROC("glGetFramebufferAttachmentParameterivEXT");
				glGenerateMipmap 	= (PFNGLGENERATEMIPMAPPROC)		O3D_GET_OPENGL_PROC("glGenerateMipmapEXT");
			}
		
			if (isExtensionSupported("GL_EXT_framebuffer_blit"))
				glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC) O3D_GET_OPENGL_PROC("glBlitFramebufferEXT");

			if ((!glIsRenderbuffer) || (!glBindRenderbuffer) || (!glDeleteRenderbuffers) ||
				(!glGenRenderbuffers) || (!glRenderbufferStorage) || (!glGetRenderbufferParameteriv) ||
				(!glIsFramebuffer) || (!glBindFramebuffer) || (!glDeleteFramebuffers) ||
				(!glGenFramebuffers) || (!glCheckFramebufferStatus) || (!glFramebufferTexture1D) ||
				(!glFramebufferTexture2D) || (!glFramebufferTexture3D) || (!glFramebufferRenderbuffer) ||
				(!glGetFramebufferAttachmentParameteriv) || (!glBlitFramebuffer) || (!glGenerateMipmap))
					O3D_ERROR(E_NullPointer("OpenGL FBO related functions"));
		}
	}
#endif // O3D_GL_ARB_framebuffer_object

	//
	// Check for ARB_texture_float
	//

	if (!isExtensionSupported("GL_ARB_texture_float"))
		O3D_ERROR(E_NullPointer("GL_ARB_texture_float is required"));

#ifdef O3D_GL_VERSION_3_0

	//
	// OpenGL 3.0
	//

	glColorMaski = (PFNGLCOLORMASKIPROC) O3D_GET_OPENGL_PROC("glColorMaski");
	glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC) O3D_GET_OPENGL_PROC("glGetBooleani_v");
	glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC) O3D_GET_OPENGL_PROC("glGetIntegeri_v");
	glEnablei = (PFNGLENABLEIPROC) O3D_GET_OPENGL_PROC("glEnablei");
	glDisablei = (PFNGLDISABLEIPROC) O3D_GET_OPENGL_PROC("glDisablei");
	glIsEnabledi = (PFNGLISENABLEDIPROC)  O3D_GET_OPENGL_PROC("glIsEnabledi");
	glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC) O3D_GET_OPENGL_PROC("glBeginTransformFeedback");
	glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC) O3D_GET_OPENGL_PROC("glEndTransformFeedback");
	glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC) O3D_GET_OPENGL_PROC("glBindBufferRange");
	glBindBufferBase = (PFNGLBINDBUFFERBASEPROC) O3D_GET_OPENGL_PROC("glBindBufferBase");
	glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC) O3D_GET_OPENGL_PROC("glTransformFeedbackVaryings");
	glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC) O3D_GET_OPENGL_PROC("glGetTransformFeedbackVarying");
	glClampColor = (PFNGLCLAMPCOLORPROC) O3D_GET_OPENGL_PROC("glClampColor");
	glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC) O3D_GET_OPENGL_PROC("glBeginConditionalRender");
	glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC) O3D_GET_OPENGL_PROC("glEndConditionalRender");
	glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC) O3D_GET_OPENGL_PROC("glVertexAttribIPointer");
	glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC) O3D_GET_OPENGL_PROC("glGetVertexAttribIiv");
	glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC) O3D_GET_OPENGL_PROC("glGetVertexAttribIuiv");
	glVertexAttribI1i = (PFNGLVERTEXATTRIBI1IPROC) O3D_GET_OPENGL_PROC("glVertexAttribI1i");
	glVertexAttribI2i = (PFNGLVERTEXATTRIBI2IPROC) O3D_GET_OPENGL_PROC("glVertexAttribI2i");
	glVertexAttribI3i = (PFNGLVERTEXATTRIBI3IPROC) O3D_GET_OPENGL_PROC("glVertexAttribI3i");
	glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC) O3D_GET_OPENGL_PROC("glVertexAttribI4i");
	glVertexAttribI1ui = (PFNGLVERTEXATTRIBI1UIPROC) O3D_GET_OPENGL_PROC("glVertexAttribI1ui");
	glVertexAttribI2ui = (PFNGLVERTEXATTRIBI2UIPROC) O3D_GET_OPENGL_PROC("glVertexAttribI2ui");
	glVertexAttribI3ui = (PFNGLVERTEXATTRIBI3UIPROC) O3D_GET_OPENGL_PROC("glVertexAttribI3ui");
	glVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC) O3D_GET_OPENGL_PROC("glVertexAttribI4ui");
	glVertexAttribI1iv = (PFNGLVERTEXATTRIBI1IVPROC) O3D_GET_OPENGL_PROC("glVertexAttribI1iv");
	glVertexAttribI2iv = (PFNGLVERTEXATTRIBI2IVPROC) O3D_GET_OPENGL_PROC("glVertexAttribI2iv");
	glVertexAttribI3iv = (PFNGLVERTEXATTRIBI3IVPROC) O3D_GET_OPENGL_PROC("glVertexAttribI3iv");
	glVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC) O3D_GET_OPENGL_PROC("glVertexAttribI4iv");
	glVertexAttribI1uiv = (PFNGLVERTEXATTRIBI1UIVPROC) O3D_GET_OPENGL_PROC("glVertexAttribI1uiv");
	glVertexAttribI2uiv = (PFNGLVERTEXATTRIBI2UIVPROC) O3D_GET_OPENGL_PROC("glVertexAttribI2uiv");
	glVertexAttribI3uiv = (PFNGLVERTEXATTRIBI3UIVPROC) O3D_GET_OPENGL_PROC("glVertexAttribI3uiv");
	glVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC) O3D_GET_OPENGL_PROC("glVertexAttribI4uiv");
	glVertexAttribI4bv = (PFNGLVERTEXATTRIBI4BVPROC) O3D_GET_OPENGL_PROC("glVertexAttribI4bv");
	glVertexAttribI4sv = (PFNGLVERTEXATTRIBI4SVPROC) O3D_GET_OPENGL_PROC("glVertexAttribI4sv");
	glVertexAttribI4ubv = (PFNGLVERTEXATTRIBI4UBVPROC) O3D_GET_OPENGL_PROC("glVertexAttribI4ubv");
	glVertexAttribI4usv = (PFNGLVERTEXATTRIBI4USVPROC) O3D_GET_OPENGL_PROC("glVertexAttribI4usv");
	glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC) O3D_GET_OPENGL_PROC("glGetUniformuiv");
	glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC) O3D_GET_OPENGL_PROC("glBindFragDataLocation");
	glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC) O3D_GET_OPENGL_PROC("glGetFragDataLocation");
	glUniform1ui = (PFNGLUNIFORM1UIPROC) O3D_GET_OPENGL_PROC("glUniform1ui");
	glUniform2ui = (PFNGLUNIFORM2UIPROC) O3D_GET_OPENGL_PROC("glUniform2ui");
	glUniform3ui = (PFNGLUNIFORM3UIPROC) O3D_GET_OPENGL_PROC("glUniform3ui");
	glUniform4ui = (PFNGLUNIFORM4UIPROC) O3D_GET_OPENGL_PROC("glUniform4ui");
	glUniform1uiv = (PFNGLUNIFORM1UIVPROC) O3D_GET_OPENGL_PROC("glUniform1uiv");
	glUniform2uiv = (PFNGLUNIFORM2UIVPROC) O3D_GET_OPENGL_PROC("glUniform2uiv");
	glUniform3uiv = (PFNGLUNIFORM3UIVPROC) O3D_GET_OPENGL_PROC("glUniform3uiv");
	glUniform4uiv = (PFNGLUNIFORM4UIPROC) O3D_GET_OPENGL_PROC("glUniform4uiv");
	glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC) O3D_GET_OPENGL_PROC("glTexParameterIiv");
	glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC) O3D_GET_OPENGL_PROC("glTexParameterIuiv");
	glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC) O3D_GET_OPENGL_PROC("glGetTexParameterIiv");
	glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC) O3D_GET_OPENGL_PROC("glGetTexParameterIuiv");
	glClearBufferiv = (PFNGLCLEARBUFFERIVPROC) O3D_GET_OPENGL_PROC("glClearBufferiv");
	glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC) O3D_GET_OPENGL_PROC("glClearBufferuiv");
	glClearBufferfv = (PFNGLCLEARBUFFERFVPROC) O3D_GET_OPENGL_PROC("glClearBufferfv");
	glClearBufferfi = (PFNGLCLEARBUFFERFIPROC) O3D_GET_OPENGL_PROC("glClearBufferfi");

	if ((!glColorMaski) || (!glGetBooleani_v) || (!glGetIntegeri_v) ||
		(!glEnablei) || (!glDisablei) || (!glIsEnabledi) ||
		(!glBeginTransformFeedback) || (!glEndTransformFeedback) ||
		(!glBindBufferRange) || (!glBindBufferBase) ||
		(!glTransformFeedbackVaryings) || (!glGetTransformFeedbackVarying) ||
		(!glClampColor) ||
		(!glBeginConditionalRender) || (!glEndConditionalRender) ||
		(!glVertexAttribIPointer) || (!glGetVertexAttribIiv) || (!glGetVertexAttribIuiv) ||
		(!glVertexAttribI1i) || (!glVertexAttribI2i) || (!glVertexAttribI3i) || (!glVertexAttribI4i) ||
		(!glVertexAttribI1ui) || (!glVertexAttribI2ui) || (!glVertexAttribI3ui) || (!glVertexAttribI4ui) ||
		(!glVertexAttribI1iv) || (!glVertexAttribI2iv) || (!glVertexAttribI3iv) || (!glVertexAttribI4iv) ||
		(!glVertexAttribI1uiv) || (!glVertexAttribI2uiv) || (!glVertexAttribI3uiv) || (!glVertexAttribI4uiv) ||
		(!glVertexAttribI4bv) || (!glVertexAttribI4sv) || (!glVertexAttribI4ubv) || (!glVertexAttribI4usv) ||
		(!glGetUniformuiv) || (!glBindFragDataLocation) || (!glGetFragDataLocation) ||
		(!glUniform1ui) || (!glUniform2ui) || (!glUniform3ui) || (!glUniform4ui) ||
		(!glUniform1uiv) || (!glUniform2uiv) || (!glUniform3uiv) || (!glUniform4uiv) ||
		(!glTexParameterIiv) || (!glTexParameterIuiv) ||
		(!glGetTexParameterIiv) || (!glGetTexParameterIuiv) ||
		(!glClearBufferiv) || (!glClearBufferuiv) || (!glClearBufferfv) ||(!glClearBufferfi))
	{
		// Not mandatory for the moment
		O3D_WARNING("Missing OpenGL 3.0 related functions");
		//O3D_ERROR(E_NullPointer("OpenGL 3.0 related functions"));
	}

#endif // O3D_GL_VERSION_3_0

#ifdef O3D_GL_VERSION_3_2

	//
	// OpenGL 3.2 and geometry shader
	//

	//glGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC) O3D_GET_OPENGL_PROC("glGetInteger64i_v");
	//glGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC) O3D_GET_OPENGL_PROC("glGetBufferParameteri64v");
	glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC) O3D_GET_OPENGL_PROC("glProgramParameteri");
	glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC) O3D_GET_OPENGL_PROC("glFramebufferTexture");
	glFramebufferTextureFace = (PFNGLFRAMEBUFFERTEXTUREFACEPROC) O3D_GET_OPENGL_PROC("glFramebufferTextureFace");

	if ((!glProgramParameteri) || (!glFramebufferTexture) || (!glFramebufferTextureFace))
	{
		glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC) O3D_GET_OPENGL_PROC("glProgramParameteriARB");
		glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC) O3D_GET_OPENGL_PROC("glFramebufferTextureARB");
		glFramebufferTextureFace = (PFNGLFRAMEBUFFERTEXTUREFACEPROC) O3D_GET_OPENGL_PROC("glFramebufferTextureFaceARB");

		if ((!glProgramParameteri) || (!glFramebufferTexture) || (!glFramebufferTextureFace))
			O3D_WARNING("Geometry shader functions not founds");
	}

	//
	// GL_ARB_vertex_array_object
	//

	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) O3D_GET_OPENGL_PROC("glBindVertexArray");
	glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) O3D_GET_OPENGL_PROC("glDeleteVertexArrays");
	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) O3D_GET_OPENGL_PROC("glGenVertexArrays");
	glIsVertexArray = (PFNGLISVERTEXARRAYPROC) O3D_GET_OPENGL_PROC("glIsVertexArray");

	if ((!glBindVertexArray) || (!glDeleteVertexArrays) || (!glGenVertexArrays) || (!glIsVertexArray))
		O3D_WARNING("GL_ARB_vertex_array_object functions not founds");

#endif // O3D_GL_VERSION_3_2

#ifdef O3D_GL_VERSION_3_3

    //
    // OpenGL 3.2 and sampler object
    //

    glGenSamplers = (PFNGLGENSAMPLERSPROC) O3D_GET_OPENGL_PROC("glGenSamplers");
    glDeleteSamplers = (PFNGLDELETESAMPLERSPROC) O3D_GET_OPENGL_PROC("glDeleteSamplers");
    glIsSampler = (PFNGLISSAMPLERPROC) O3D_GET_OPENGL_PROC("glIsSampler");
    glBindSampler = (PFNGLBINDSAMPLERPROC) O3D_GET_OPENGL_PROC("glBindSampler");
    glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC) O3D_GET_OPENGL_PROC("glSamplerParameteri");
    glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC) O3D_GET_OPENGL_PROC("glSamplerParameteriv");
    glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC) O3D_GET_OPENGL_PROC("glSamplerParameterf");
    glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC) O3D_GET_OPENGL_PROC("glSamplerParameterfv");
    glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC) O3D_GET_OPENGL_PROC("glSamplerParameterIiv");
    glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC) O3D_GET_OPENGL_PROC("glSamplerParameterIuiv");
    glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC) O3D_GET_OPENGL_PROC("glGetSamplerParameteriv");
    glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC) O3D_GET_OPENGL_PROC("glGetSamplerParameterIiv");
    glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC) O3D_GET_OPENGL_PROC("glGetSamplerParameterfv");
    glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC) O3D_GET_OPENGL_PROC("glGetSamplerParameterIuiv");

    if ((!glGenSamplers) || (!glDeleteSamplers) || (!glIsSampler) || (!glBindSampler) ||
        (!glSamplerParameteri) || (!glSamplerParameteriv) ||
        (!glSamplerParameterf) || (!glSamplerParameterfv) ||
        (!glSamplerParameterIiv) || (!glSamplerParameterIuiv) ||
        (!glGetSamplerParameteriv) || (!glGetSamplerParameterIiv) ||
        (!glGetSamplerParameterfv) || (!glGetSamplerParameterIuiv))
        O3D_WARNING("Geometry shader functions not founds");

#endif // O3D_GL_VERSION_3_3

#ifdef O3D_GL_ARB_map_buffer_range

    //
    // O3D_GL_ARB_map_buffer_range
    //

    glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC) O3D_GET_OPENGL_PROC("glMapBufferRange");
    glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC) O3D_GET_OPENGL_PROC("glFlushMappedBufferRange");

    if ((!glMapBufferRange) || (!glFlushMappedBufferRange))
        O3D_WARNING("Frame buffer object map buffer range functions not founds");

#endif // O3D_GL_ARB_map_buffer_range

#ifdef O3D_GL_ARB_texture_multisample

    //
    // O3D_GL_ARB_texture_multisample
    //

    glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC) O3D_GET_OPENGL_PROC("glTexImage2DMultisample");
    glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC) O3D_GET_OPENGL_PROC("glTexImage3DMultisample");
    glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC) O3D_GET_OPENGL_PROC("glGetMultisamplefv");
    glSampleMaski = (PFNGLSAMPLEMASKIPROC) O3D_GET_OPENGL_PROC("glSampleMaski");

    if ((!glTexImage2DMultisample) || (!glTexImage3DMultisample) ||
        (!glGetMultisamplefv) || (!glSampleMaski))
        O3D_WARNING("Texture multisample fonctions not founds");

#endif // O3D_GL_ARB_texture_multisample

#ifdef O3D_GL_VERSION_4_0
    glBlendEquationi = (PFNGLBLENDEQUATIONIPROC) O3D_GET_OPENGL_PROC("glBlendEquationi");
    glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC) O3D_GET_OPENGL_PROC("glBlendEquationSeparatei");
    glBlendFunci = (PFNGLBLENDFUNCIPROC) O3D_GET_OPENGL_PROC("glBlendFunci");
    glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC) O3D_GET_OPENGL_PROC("glBlendFuncSeparatei");

    if (!glBlendEquationi || !glBlendEquationSeparatei || !glBlendFunci || !glBlendFuncSeparatei)
        O3D_WARNING("Blend index/separate functions not founds");
#endif // O3D_GL_VERSION_4_0

#ifdef O3D_GL_VERSION_4_1
#endif // O3D_GL_VERSION_4_1

#ifdef O3D_GL_VERSION_4_2
#endif // O3D_GL_VERSION_4_2

#ifdef O3D_GL_VERSION_4_3
#endif // O3D_GL_VERSION_4_3

#ifdef O3D_GL_VERSION_4_4
#endif // O3D_GL_VERSION_4_4

#ifdef O3D_GL_VERSION_4_5
#endif // O3D_GL_VERSION_4_5

#endif // O3D_GL_PROTOTYPES
}

// Initialize the extension manager
void GLExtensionManager::initialize()
{
    if (m_valid) {
		return;
    }

	// get extensions functions pointers (only for windows or for non valid OpenGL 2.0 context).
	const GLubyte *version = glGetString(GL_VERSION);
    if (!version) {
		O3D_ERROR(E_InvalidResult("Undefined OpenGL version"));
    }

	// get glGetStringi before
#ifndef O3D_GL_PROTOTYPES
#ifdef O3D_GL_VERSION_3_0
	glGetStringi = (PFNGLGETSTRINGIPROC) O3D_GET_OPENGL_PROC("glGetStringi");
#endif // O3D_GL_VERSION_3_0
#endif // O3D_GL_PROTOTYPES

	getExtFunctions();

	m_valid = True;
}

// Check for an extension
Bool GLExtensionManager::isExtensionSupported(const String &ext)
{
	// check for a valid extension name
    if ((ext.find(' ') > 0) || (ext.length() == 0)) {
		return False;
    }

	const GLubyte *version = glGetString(GL_VERSION);

    if (!version) {
		O3D_ERROR(E_InvalidResult("Undefined OpenGL version"));
    }

    if ((version != nullptr) && ((version[0] - 48) >= 3)) {
		GLint numExts;

		glGetIntegerv(GL_NUM_EXTENSIONS, &numExts);

        for (GLint i = 0; i < numExts; ++i) {
			const Char *extName = reinterpret_cast<const Char*>(glGetStringi(GL_EXTENSIONS, i));
            if (extName) {
				if (ext == extName)
					return True;
			}
		}

		return False;
    } else {
		// get the list of extensions
		const GLubyte *extensions = glGetString(GL_EXTENSIONS);

        if (!extensions) {
			O3D_ERROR(E_InvalidResult("glGetString(GL_EXTENSIONS) is null"));
        }

		String exts = reinterpret_cast<const Char*>(extensions);

		StringTokenizer tokenizer(exts, " ");

        while (tokenizer.hasMoreElements()) {
			if (tokenizer.nextElement() == ext)
				return True;
		}

		return False;
	}
}

void* GLExtensionManager::getFunctionPtr(const CString &foo)
{
    return (void*)(O3D_GET_OPENGL_PROC(foo.getData()));
}
