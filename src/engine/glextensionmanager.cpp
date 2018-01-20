/**
 * @file glextensionmanager.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/glextdefines.h"

// avoid extern keywork during compilation of the library
#ifndef O3D_GLAPI
#define O3D_GLAPI O3D_API
#endif

#include "o3d/engine/glextensionmanager.h"

#include "o3d/core/gl.h"
#include "o3d/core/debug.h"
#include "o3d/core/dynamiclibrary.h"
#include "o3d/core/stringtokenizer.h"

using namespace o3d;

#ifdef O3D_GL_VERSION_1_0
PFNGLCULLFACEPROC glCullFace = nullptr;
PFNGLFRONTFACEPROC glFrontFace = nullptr;
PFNGLHINTPROC glHint = nullptr;
PFNGLLINEWIDTHPROC glLineWidth = nullptr;
PFNGLPOINTSIZEPROC glPointSize = nullptr;
PFNGLPOLYGONMODEPROC glPolygonMode = nullptr;
PFNGLSCISSORPROC glScissor = nullptr;
PFNGLTEXPARAMETERFPROC glTexParameterf = nullptr;
PFNGLTEXPARAMETERFVPROC glTexParameterfv = nullptr;
PFNGLTEXPARAMETERIPROC glTexParameteri = nullptr;
PFNGLTEXPARAMETERIVPROC glTexParameteriv = nullptr;
PFNGLTEXIMAGE1DPROC glTexImage1D = nullptr;
PFNGLTEXIMAGE2DPROC glTexImage2D = nullptr;
PFNGLDRAWBUFFERPROC glDrawBuffer = nullptr;
PFNGLCLEARPROC glClear = nullptr;
PFNGLCLEARCOLORPROC glClearColor = nullptr;
PFNGLCLEARSTENCILPROC glClearStencil = nullptr;
PFNGLCLEARDEPTHPROC glClearDepth = nullptr;
PFNGLSTENCILMASKPROC glStencilMask = nullptr;
PFNGLCOLORMASKPROC glColorMask = nullptr;
PFNGLDEPTHMASKPROC glDepthMask = nullptr;
PFNGLDISABLEPROC glDisable = nullptr;
PFNGLENABLEPROC glEnable = nullptr;
PFNGLFINISHPROC glFinish = nullptr;
PFNGLFLUSHPROC glFlush = nullptr;
PFNGLBLENDFUNCPROC glBlendFunc = nullptr;
PFNGLLOGICOPPROC glLogicOp = nullptr;
PFNGLSTENCILFUNCPROC glStencilFunc = nullptr;
PFNGLSTENCILOPPROC glStencilOp = nullptr;
PFNGLDEPTHFUNCPROC glDepthFunc = nullptr;
PFNGLPIXELSTOREFPROC glPixelStoref = nullptr;
PFNGLPIXELSTOREIPROC glPixelStorei = nullptr;
PFNGLREADBUFFERPROC glReadBuffer = nullptr;
PFNGLREADPIXELSPROC glReadPixels = nullptr;
PFNGLGETBOOLEANVPROC glGetBooleanv = nullptr;
PFNGLGETDOUBLEVPROC glGetDoublev = nullptr;
PFNGLGETERRORPROC glGetError = nullptr;
PFNGLGETFLOATVPROC glGetFloatv = nullptr;
PFNGLGETINTEGERVPROC _glGetIntegerv = nullptr;
PFNGLGETSTRINGPROC _glGetString = nullptr;
PFNGLGETTEXIMAGEPROC glGetTexImage = nullptr;
PFNGLGETTEXPARAMETERFVPROC glGetTexParameterfv = nullptr;
PFNGLGETTEXPARAMETERIVPROC glGetTexParameteriv = nullptr;
PFNGLGETTEXLEVELPARAMETERFVPROC glGetTexLevelParameterfv = nullptr;
PFNGLGETTEXLEVELPARAMETERIVPROC glGetTexLevelParameteriv = nullptr;
PFNGLISENABLEDPROC glIsEnabled = nullptr;
PFNGLDEPTHRANGEPROC glDepthRange = nullptr;
PFNGLVIEWPORTPROC glViewport = nullptr;
#endif // O3D_GL_VERSION_1_0

#ifdef O3D_GL_VERSION_1_1
PFNGLDRAWARRAYSPROC glDrawArrays = nullptr;
PFNGLDRAWELEMENTSPROC glDrawElements = nullptr;
PFNGLGETPOINTERVPROC glGetPointerv = nullptr;
PFNGLPOLYGONOFFSETPROC glPolygonOffset = nullptr;
PFNGLCOPYTEXIMAGE1DPROC glCopyTexImage1D = nullptr;
PFNGLCOPYTEXIMAGE2DPROC glCopyTexImage2D = nullptr;
PFNGLCOPYTEXSUBIMAGE1DPROC glCopyTexSubImage1D = nullptr;
PFNGLCOPYTEXSUBIMAGE2DPROC glCopyTexSubImage2D = nullptr;
PFNGLTEXSUBIMAGE1DPROC glTexSubImage1D = nullptr;
PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D = nullptr;
PFNGLBINDTEXTUREPROC glBindTexture = nullptr;
PFNGLDELETETEXTURESPROC glDeleteTextures = nullptr;
PFNGLGENTEXTURESPROC glGenTextures = nullptr;
PFNGLISTEXTUREPROC glIsTexture = nullptr;
#endif // O3D_GL_VERSION_1_1

#ifdef O3D_GL_VERSION_1_2
PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements = nullptr;
PFNGLTEXIMAGE3DPROC glTexImage3D = nullptr;
PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D = nullptr;
#endif // O3D_GL_VERSION_1_2

#ifdef O3D_GL_VERSION_1_3
PFNGLACTIVETEXTUREPROC glActiveTexture = nullptr;
PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D = nullptr;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D = nullptr;
PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D = nullptr;
PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage = nullptr;
#endif // O3D_GL_VERSION_1_3

#ifdef O3D_GL_VERSION_1_4
PFNGLPOINTPARAMETERFPROC glPointParameterf = nullptr;
PFNGLPOINTPARAMETERFVPROC glPointParameterfv = nullptr;
PFNGLPOINTPARAMETERIPROC glPointParameteri = nullptr;
PFNGLPOINTPARAMETERIVPROC glPointParameteriv = nullptr;
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate = nullptr;
PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays = nullptr;
PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements = nullptr;
PFNGLBLENDEQUATIONPROC glBlendEquation = nullptr;
#endif // O3D_GL_VERSION_1_4

#ifdef O3D_GL_VERSION_1_5
PFNGLGENQUERIESPROC glGenQueries = nullptr;
PFNGLISQUERYPROC glIsQuery = nullptr;
PFNGLBEGINQUERYPROC glBeginQuery = nullptr;
PFNGLENDQUERYPROC glEndQuery = nullptr;
PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv = nullptr;
PFNGLGETQUERYIVPROC glGetQueryiv = nullptr;
PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv = nullptr;
PFNGLDELETEQUERIESPROC glDeleteQueries = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLBUFFERSUBDATAPROC glBufferSubData = nullptr;
PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData = nullptr;
PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv = nullptr;
PFNGLMAPBUFFERPROC glMapBuffer = nullptr;
PFNGLUNMAPBUFFERPROC glUnmapBuffer = nullptr;
#endif // O3D_GL_VERSION_1_5

#ifdef O3D_GL_VERSION_2_0
PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate = nullptr;
PFNGLDRAWBUFFERSPROC glDrawBuffers = nullptr;
PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate = nullptr;
PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate = nullptr;
PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate = nullptr;
PFNGLCREATESHADERPROC glCreateShader = nullptr;
PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
PFNGLVALIDATEPROGRAMPROC glValidateProgram = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
PFNGLDETACHSHADERPROC glDetachShader = nullptr;
PFNGLDELETESHADERPROC glDeleteShader = nullptr;
PFNGLDELETEPROGRAMPROC glDeleteProgram = nullptr;
PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = nullptr;
PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
PFNGLISPROGRAMPROC glIsProgram = nullptr;
PFNGLISSHADERPROC glIsShader = nullptr;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform = nullptr;
PFNGLUNIFORM1FPROC glUniform1f = nullptr;
PFNGLUNIFORM2FPROC glUniform2f = nullptr;
PFNGLUNIFORM3FPROC glUniform3f = nullptr;
PFNGLUNIFORM4FPROC glUniform4f = nullptr;
PFNGLUNIFORM1FVPROC glUniform1fv = nullptr;
PFNGLUNIFORM2FVPROC glUniform2fv = nullptr;
PFNGLUNIFORM3FVPROC glUniform3fv = nullptr;
PFNGLUNIFORM4FVPROC glUniform4fv = nullptr;
PFNGLUNIFORM1IPROC glUniform1i = nullptr;
PFNGLUNIFORM2IPROC glUniform2i = nullptr;
PFNGLUNIFORM3IPROC glUniform3i = nullptr;
PFNGLUNIFORM4IPROC glUniform4i = nullptr;
PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv = nullptr;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv = nullptr;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib = nullptr;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation = nullptr;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = nullptr;
PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv = nullptr;
PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv = nullptr;
PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv = nullptr;
PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv = nullptr;
PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d = nullptr;
PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv = nullptr;
PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f = nullptr;
PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv = nullptr;
PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s = nullptr;
PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv = nullptr;
PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d = nullptr;
PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv = nullptr;
PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f = nullptr;
PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv = nullptr;
PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s = nullptr;
PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv = nullptr;
PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d = nullptr;
PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv = nullptr;
PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f = nullptr;
PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv = nullptr;
PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s = nullptr;
PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv = nullptr;
PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv = nullptr;
PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv = nullptr;
PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv = nullptr;
PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub = nullptr;
PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv = nullptr;
PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv = nullptr;
PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv = nullptr;
PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv = nullptr;
PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d = nullptr;
PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv = nullptr;
PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f = nullptr;
PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv = nullptr;
PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv = nullptr;
PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s = nullptr;
PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4sv = nullptr;
PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv = nullptr;
PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv = nullptr;
PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
#endif // O3D_GL_VERSION_2_0

#ifdef O3D_GL_VERSION_3_0
PFNGLCOLORMASKIPROC glColorMaski = nullptr;
PFNGLGETBOOLEANI_VPROC glGetBooleani_v = nullptr;
PFNGLGETINTEGERI_VPROC glGetIntegeri_v = nullptr;
PFNGLENABLEIPROC glEnablei = nullptr;
PFNGLDISABLEIPROC glDisablei = nullptr;
PFNGLISENABLEDIPROC glIsEnabledi = nullptr;
PFNGLBEGINTRANSFORMFEEDBACKPROC glBeginTransformFeedback = nullptr;
PFNGLENDTRANSFORMFEEDBACKPROC glEndTransformFeedback = nullptr;
PFNGLBINDBUFFERRANGEPROC glBindBufferRange = nullptr;
PFNGLBINDBUFFERBASEPROC glBindBufferBase = nullptr;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC glTransformFeedbackVaryings = nullptr;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying = nullptr;
PFNGLCLAMPCOLORPROC glClampColor = nullptr;
PFNGLBEGINCONDITIONALRENDERPROC glBeginConditionalRender = nullptr;
PFNGLENDCONDITIONALRENDERPROC glEndConditionalRender = nullptr;
PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer = nullptr;
PFNGLGETVERTEXATTRIBIIVPROC glGetVertexAttribIiv = nullptr;
PFNGLGETVERTEXATTRIBIUIVPROC glGetVertexAttribIuiv = nullptr;
PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i = nullptr;
PFNGLVERTEXATTRIBI2IPROC glVertexAttribI2i = nullptr;
PFNGLVERTEXATTRIBI3IPROC glVertexAttribI3i = nullptr;
PFNGLVERTEXATTRIBI4IPROC glVertexAttribI4i = nullptr;
PFNGLVERTEXATTRIBI1UIPROC glVertexAttribI1ui = nullptr;
PFNGLVERTEXATTRIBI2UIPROC glVertexAttribI2ui = nullptr;
PFNGLVERTEXATTRIBI3UIPROC glVertexAttribI3ui = nullptr;
PFNGLVERTEXATTRIBI4UIPROC glVertexAttribI4ui = nullptr;
PFNGLVERTEXATTRIBI1IVPROC glVertexAttribI1iv = nullptr;
PFNGLVERTEXATTRIBI2IVPROC glVertexAttribI2iv = nullptr;
PFNGLVERTEXATTRIBI3IVPROC glVertexAttribI3iv = nullptr;
PFNGLVERTEXATTRIBI4IVPROC glVertexAttribI4iv = nullptr;
PFNGLVERTEXATTRIBI1UIVPROC glVertexAttribI1uiv = nullptr;
PFNGLVERTEXATTRIBI2UIVPROC glVertexAttribI2uiv = nullptr;
PFNGLVERTEXATTRIBI3UIVPROC glVertexAttribI3uiv = nullptr;
PFNGLVERTEXATTRIBI4UIVPROC glVertexAttribI4uiv = nullptr;
PFNGLVERTEXATTRIBI4BVPROC glVertexAttribI4bv = nullptr;
PFNGLVERTEXATTRIBI4SVPROC glVertexAttribI4sv = nullptr;
PFNGLVERTEXATTRIBI4UBVPROC glVertexAttribI4ubv = nullptr;
PFNGLVERTEXATTRIBI4USVPROC glVertexAttribI4usv = nullptr;
PFNGLGETUNIFORMUIVPROC glGetUniformuiv = nullptr;
PFNGLBINDFRAGDATALOCATIONPROC glBindFragDataLocation = nullptr;
PFNGLGETFRAGDATALOCATIONPROC glGetFragDataLocation = nullptr;
PFNGLUNIFORM1UIPROC glUniform1ui = nullptr;
PFNGLUNIFORM2UIPROC glUniform2ui = nullptr;
PFNGLUNIFORM3UIPROC glUniform3ui = nullptr;
PFNGLUNIFORM4UIPROC glUniform4ui = nullptr;
PFNGLUNIFORM1UIVPROC glUniform1uiv = nullptr;
PFNGLUNIFORM2UIVPROC glUniform2uiv = nullptr;
PFNGLUNIFORM3UIVPROC glUniform3uiv = nullptr;
PFNGLUNIFORM4UIPROC glUniform4uiv = nullptr;
PFNGLTEXPARAMETERIIVPROC glTexParameterIiv = nullptr;
PFNGLTEXPARAMETERIUIVPROC glTexParameterIuiv = nullptr;
PFNGLGETTEXPARAMETERIIVPROC glGetTexParameterIiv = nullptr;
PFNGLGETTEXPARAMETERIUIVPROC glGetTexParameterIuiv = nullptr;
PFNGLCLEARBUFFERIVPROC glClearBufferiv = nullptr;
PFNGLCLEARBUFFERUIVPROC glClearBufferuiv = nullptr;
PFNGLCLEARBUFFERFVPROC glClearBufferfv = nullptr;
PFNGLCLEARBUFFERFIPROC glClearBufferfi = nullptr;
PFNGLGETSTRINGIPROC glGetStringi = nullptr;
PFNGLMAPBUFFERRANGEPROC glMapBufferRange = nullptr;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange = nullptr;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = nullptr;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
PFNGLISVERTEXARRAYPROC glIsVertexArray = nullptr;
PFNGLISRENDERBUFFERPROC glIsRenderbuffer = nullptr;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = nullptr;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers = nullptr;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers = nullptr;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage = nullptr;
PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv = nullptr;
PFNGLISFRAMEBUFFERPROC glIsFramebuffer = nullptr;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = nullptr;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = nullptr;
PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D = nullptr;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = nullptr;
PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D = nullptr;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = nullptr;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv = nullptr;
PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer = nullptr;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample = nullptr;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap = nullptr;
PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer = nullptr;
#endif // O3D_GL_VERSION_3_0

#ifdef O3D_GL_VERSION_3_1
PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced = nullptr;
PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced = nullptr;
PFNGLTEXBUFFERPROC glTexBuffer = nullptr;
PFNGLPRIMITIVERESTARTINDEXPROC glPrimitiveRestartIndex = nullptr;
PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData = nullptr;
PFNGLGETUNIFORMINDICESPROC glGetUniformIndices = nullptr;
PFNGLGETACTIVEUNIFORMSIVPROC glGetActiveUniformsiv = nullptr;
PFNGLGETACTIVEUNIFORMNAMEPROC glGetActiveUniformName = nullptr;
PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex = nullptr;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv = nullptr;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName = nullptr;
PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding = nullptr;
#endif // O3D_GL_VERSION_3_1

#ifdef O3D_GL_VERSION_3_2
PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex = nullptr;
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex = nullptr;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex = nullptr;
PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glMultiDrawElementsBaseVertex = nullptr;
PFNGLPROVOKINGVERTEXPROC glProvokingVertex = nullptr;
PFNGLFENCESYNCPROC glFenceSync = nullptr;
PFNGLISSYNCPROC glIsSync = nullptr;
PFNGLDELETESYNCPROC glDeleteSync = nullptr;
PFNGLCLIENTWAITSYNCPROC glClientWaitSync = nullptr;
PFNGLWAITSYNCPROC glWaitSync = nullptr;
PFNGLGETINTEGER64VPROC glGetInteger64v = nullptr;
PFNGLGETSYNCIVPROC glGetSynciv = nullptr;
PFNGLGETINTEGER64I_VPROC glGetInteger64i_v = nullptr;
PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v = nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture = nullptr;
PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample = nullptr;
PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample = nullptr;
PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv = nullptr;
PFNGLSAMPLEMASKIPROC glSampleMaski = nullptr;
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
PFNGLRELEASESHADERCOMPILERPROC glReleaseShaderCompiler = nullptr;
PFNGLSHADERBINARYPROC glShaderBinary = nullptr;
PFNGLGETSHADERPRECISIONFORMATPROC glGetShaderPrecisionFormat = nullptr;
PFNGLDEPTHRANGEFPROC glDepthRangef = nullptr;
PFNGLCLEARDEPTHFPROC glClearDepthf = nullptr;
PFNGLGETPROGRAMBINARYPROC glGetProgramBinary = nullptr;
PFNGLPROGRAMBINARYPROC glProgramBinary = nullptr;
PFNGLPROGRAMPARAMETERIPROC glProgramParameteri = nullptr;
PFNGLUSEPROGRAMSTAGESPROC glUseProgramStages = nullptr;
PFNGLACTIVESHADERPROGRAMPROC glActiveShaderProgram = nullptr;
PFNGLCREATESHADERPROGRAMVPROC glCreateShaderProgramv = nullptr;
PFNGLBINDPROGRAMPIPELINEPROC glBindProgramPipeline = nullptr;
PFNGLDELETEPROGRAMPIPELINESPROC glDeleteProgramPipelines = nullptr;
PFNGLGENPROGRAMPIPELINESPROC glGenProgramPipelines = nullptr;
PFNGLISPROGRAMPIPELINEPROC glIsProgramPipeline = nullptr;
PFNGLGETPROGRAMPIPELINEIVPROC glGetProgramPipelineiv = nullptr;
PFNGLPROGRAMUNIFORM1IPROC glProgramUniform1i = nullptr;
PFNGLPROGRAMUNIFORM1IVPROC glProgramUniform1iv = nullptr;
PFNGLPROGRAMUNIFORM1FPROC glProgramUniform1f = nullptr;
PFNGLPROGRAMUNIFORM1FVPROC glProgramUniform1fv = nullptr;
PFNGLPROGRAMUNIFORM1DPROC glProgramUniform1d = nullptr;
PFNGLPROGRAMUNIFORM1DVPROC glProgramUniform1dv = nullptr;
PFNGLPROGRAMUNIFORM1UIPROC glProgramUniform1ui = nullptr;
PFNGLPROGRAMUNIFORM1UIVPROC glProgramUniform1uiv = nullptr;
PFNGLPROGRAMUNIFORM2IPROC glProgramUniform2i = nullptr;
PFNGLPROGRAMUNIFORM2IVPROC glProgramUniform2iv = nullptr;
PFNGLPROGRAMUNIFORM2FPROC glProgramUniform2f = nullptr;
PFNGLPROGRAMUNIFORM2FVPROC glProgramUniform2fv = nullptr;
PFNGLPROGRAMUNIFORM2DPROC glProgramUniform2d = nullptr;
PFNGLPROGRAMUNIFORM2DVPROC glProgramUniform2dv = nullptr;
PFNGLPROGRAMUNIFORM2UIPROC glProgramUniform2ui = nullptr;
PFNGLPROGRAMUNIFORM2UIVPROC glProgramUniform2uiv = nullptr;
PFNGLPROGRAMUNIFORM3IPROC glProgramUniform3i = nullptr;
PFNGLPROGRAMUNIFORM3IVPROC glProgramUniform3iv = nullptr;
PFNGLPROGRAMUNIFORM3FPROC glProgramUniform3f = nullptr;
PFNGLPROGRAMUNIFORM3FVPROC glProgramUniform3fv = nullptr;
PFNGLPROGRAMUNIFORM3DPROC glProgramUniform3d = nullptr;
PFNGLPROGRAMUNIFORM3DVPROC glProgramUniform3dv = nullptr;
PFNGLPROGRAMUNIFORM3UIPROC glProgramUniform3ui = nullptr;
PFNGLPROGRAMUNIFORM3UIVPROC glProgramUniform3uiv = nullptr;
PFNGLPROGRAMUNIFORM4IPROC glProgramUniform4i = nullptr;
PFNGLPROGRAMUNIFORM4IVPROC glProgramUniform4iv = nullptr;
PFNGLPROGRAMUNIFORM4FPROC glProgramUniform4f = nullptr;
PFNGLPROGRAMUNIFORM4FVPROC glProgramUniform4fv = nullptr;
PFNGLPROGRAMUNIFORM4DPROC glProgramUniform4d = nullptr;
PFNGLPROGRAMUNIFORM4DVPROC glProgramUniform4dv = nullptr;
PFNGLPROGRAMUNIFORM4UIPROC glProgramUniform4ui = nullptr;
PFNGLPROGRAMUNIFORM4UIVPROC glProgramUniform4uiv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2FVPROC glProgramUniformMatrix2fv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3FVPROC glProgramUniformMatrix3fv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC glProgramUniformMatrix4fv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2DVPROC glProgramUniformMatrix2dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3DVPROC glProgramUniformMatrix3dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4DVPROC glProgramUniformMatrix4dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glProgramUniformMatrix2x3fv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glProgramUniformMatrix3x2fv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glProgramUniformMatrix2x4fv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glProgramUniformMatrix4x2fv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glProgramUniformMatrix3x4fv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glProgramUniformMatrix4x3fv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC glProgramUniformMatrix2x3dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC glProgramUniformMatrix3x2dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC glProgramUniformMatrix2x4dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC glProgramUniformMatrix4x2dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC glProgramUniformMatrix3x4dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC glProgramUniformMatrix4x3dv = nullptr;
PFNGLVALIDATEPROGRAMPIPELINEPROC glValidateProgramPipeline = nullptr;
PFNGLGETPROGRAMPIPELINEINFOLOGPROC glGetProgramPipelineInfoLog = nullptr;
PFNGLVERTEXATTRIBL1DPROC glVertexAttribL1d = nullptr;
PFNGLVERTEXATTRIBL2DPROC glVertexAttribL2d = nullptr;
PFNGLVERTEXATTRIBL3DPROC glVertexAttribL3d = nullptr;
PFNGLVERTEXATTRIBL4DPROC glVertexAttribL4d = nullptr;
PFNGLVERTEXATTRIBL1DVPROC glVertexAttribL1dv = nullptr;
PFNGLVERTEXATTRIBL2DVPROC glVertexAttribL2dv = nullptr;
PFNGLVERTEXATTRIBL3DVPROC glVertexAttribL3dv = nullptr;
PFNGLVERTEXATTRIBL4DVPROC glVertexAttribL4dv = nullptr;
PFNGLVERTEXATTRIBLPOINTERPROC glVertexAttribLPointer = nullptr;
PFNGLGETVERTEXATTRIBLDVPROC glGetVertexAttribLdv = nullptr;
PFNGLVIEWPORTARRAYVPROC glViewportArrayv = nullptr;
PFNGLVIEWPORTINDEXEDFPROC glViewportIndexedf = nullptr;
PFNGLVIEWPORTINDEXEDFVPROC glViewportIndexedfv = nullptr;
PFNGLSCISSORARRAYVPROC glScissorArrayv = nullptr;
PFNGLSCISSORINDEXEDPROC glScissorIndexed = nullptr;
PFNGLSCISSORINDEXEDVPROC glScissorIndexedv = nullptr;
PFNGLDEPTHRANGEARRAYVPROC glDepthRangeArrayv = nullptr;
PFNGLDEPTHRANGEINDEXEDPROC glDepthRangeIndexed = nullptr;
PFNGLGETFLOATI_VPROC glGetFloati_v = nullptr;
PFNGLGETDOUBLEI_VPROC glGetDoublei_v = nullptr;
#endif // O3D_GL_VERSION_4_1

#ifdef O3D_GL_VERSION_4_2
PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC glDrawArraysInstancedBaseInstance = nullptr;
PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC glDrawElementsInstancedBaseInstance = nullptr;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glDrawElementsInstancedBaseVertexBaseInstance = nullptr;
PFNGLGETINTERNALFORMATIVPROC glGetInternalformativ = nullptr;
PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC glGetActiveAtomicCounterBufferiv = nullptr;
PFNGLBINDIMAGETEXTUREPROC glBindImageTexture = nullptr;
PFNGLMEMORYBARRIERPROC glMemoryBarrier = nullptr;
PFNGLTEXSTORAGE1DPROC glTexStorage1D = nullptr;
PFNGLTEXSTORAGE2DPROC glTexStorage2D = nullptr;
PFNGLTEXSTORAGE3DPROC glTexStorage3D = nullptr;
PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC glDrawTransformFeedbackInstanced = nullptr;
PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC glDrawTransformFeedbackStreamInstanced = nullptr;
#endif // O3D_GL_VERSION_4_2

#ifdef O3D_GL_VERSION_4_3
PFNGLCLEARBUFFERDATAPROC glClearBufferData = nullptr;
PFNGLCLEARBUFFERSUBDATAPROC glClearBufferSubData = nullptr;
PFNGLDISPATCHCOMPUTEPROC glDispatchCompute = nullptr;
PFNGLDISPATCHCOMPUTEINDIRECTPROC glDispatchComputeIndirect = nullptr;
PFNGLCOPYIMAGESUBDATAPROC glCopyImageSubData = nullptr;
PFNGLFRAMEBUFFERPARAMETERIPROC glFramebufferParameteri = nullptr;
PFNGLGETFRAMEBUFFERPARAMETERIVPROC glGetFramebufferParameteriv = nullptr;
PFNGLGETINTERNALFORMATI64VPROC glGetInternalformati64v = nullptr;
PFNGLINVALIDATETEXSUBIMAGEPROC glInvalidateTexSubImage = nullptr;
PFNGLINVALIDATETEXIMAGEPROC glInvalidateTexImage = nullptr;
PFNGLINVALIDATEBUFFERSUBDATAPROC glInvalidateBufferSubData = nullptr;
PFNGLINVALIDATEBUFFERDATAPROC glInvalidateBufferData = nullptr;
PFNGLINVALIDATEFRAMEBUFFERPROC glInvalidateFramebuffer = nullptr;
PFNGLINVALIDATESUBFRAMEBUFFERPROC glInvalidateSubFramebuffer = nullptr;
PFNGLMULTIDRAWARRAYSINDIRECTPROC glMultiDrawArraysIndirect = nullptr;
PFNGLMULTIDRAWELEMENTSINDIRECTPROC glMultiDrawElementsIndirect = nullptr;
PFNGLGETPROGRAMINTERFACEIVPROC glGetProgramInterfaceiv = nullptr;
PFNGLGETPROGRAMRESOURCEINDEXPROC glGetProgramResourceIndex = nullptr;
PFNGLGETPROGRAMRESOURCENAMEPROC glGetProgramResourceName = nullptr;
PFNGLGETPROGRAMRESOURCEIVPROC glGetProgramResourceiv = nullptr;
PFNGLGETPROGRAMRESOURCELOCATIONPROC glGetProgramResourceLocation = nullptr;
PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC glGetProgramResourceLocationIndex = nullptr;
PFNGLSHADERSTORAGEBLOCKBINDINGPROC glShaderStorageBlockBinding = nullptr;
PFNGLTEXBUFFERRANGEPROC glTexBufferRange = nullptr;
PFNGLTEXSTORAGE2DMULTISAMPLEPROC glTexStorage2DMultisample = nullptr;
PFNGLTEXSTORAGE3DMULTISAMPLEPROC glTexStorage3DMultisample = nullptr;
PFNGLTEXTUREVIEWPROC glTextureView = nullptr;
PFNGLBINDVERTEXBUFFERPROC glBindVertexBuffer = nullptr;
PFNGLVERTEXATTRIBFORMATPROC glVertexAttribFormat = nullptr;
PFNGLVERTEXATTRIBIFORMATPROC glVertexAttribIFormat = nullptr;
PFNGLVERTEXATTRIBLFORMATPROC glVertexAttribLFormat = nullptr;
PFNGLVERTEXATTRIBBINDINGPROC glVertexAttribBinding = nullptr;
PFNGLVERTEXBINDINGDIVISORPROC glVertexBindingDivisor = nullptr;
PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl = nullptr;
PFNGLDEBUGMESSAGEINSERTPROC glDebugMessageInsert = nullptr;
PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback = nullptr;
PFNGLGETDEBUGMESSAGELOGPROC glGetDebugMessageLog = nullptr;
PFNGLPUSHDEBUGGROUPPROC glPushDebugGroup = nullptr;
PFNGLPOPDEBUGGROUPPROC glPopDebugGroup = nullptr;
PFNGLOBJECTLABELPROC glObjectLabel = nullptr;
PFNGLGETOBJECTLABELPROC glGetObjectLabel = nullptr;
PFNGLOBJECTPTRLABELPROC glObjectPtrLabel = nullptr;
PFNGLGETOBJECTPTRLABELPROC glGetObjectPtrLabel = nullptr;
#endif // O3D_GL_VERSION_4_3

#ifdef O3D_GL_VERSION_4_4
PFNGLBUFFERSTORAGEPROC glBufferStorage = nullptr;
PFNGLCLEARTEXIMAGEPROC glClearTexImage = nullptr;
PFNGLCLEARTEXSUBIMAGEPROC glClearTexSubImage = nullptr;
PFNGLBINDBUFFERSBASEPROC glBindBuffersBase = nullptr;
PFNGLBINDBUFFERSRANGEPROC glBindBuffersRange = nullptr;
PFNGLBINDTEXTURESPROC glBindTextures = nullptr;
PFNGLBINDSAMPLERSPROC glBindSamplers = nullptr;
PFNGLBINDIMAGETEXTURESPROC glBindImageTextures = nullptr;
PFNGLBINDVERTEXBUFFERSPROC glBindVertexBuffers = nullptr;
#endif // O3D_GL_VERSION_4_4

#ifdef O3D_GL_VERSION_4_5
PFNGLCLIPCONTROLPROC glClipControl = nullptr;
PFNGLCREATETRANSFORMFEEDBACKSPROC glCreateTransformFeedbacks = nullptr;
PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC glTransformFeedbackBufferBase = nullptr;
PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC glTransformFeedbackBufferRange = nullptr;
PFNGLGETTRANSFORMFEEDBACKIVPROC glGetTransformFeedbackiv = nullptr;
PFNGLGETTRANSFORMFEEDBACKI_VPROC glGetTransformFeedbacki_v = nullptr;
PFNGLGETTRANSFORMFEEDBACKI64_VPROC glGetTransformFeedbacki64_v = nullptr;
PFNGLCREATEBUFFERSPROC glCreateBuffers = nullptr;
PFNGLNAMEDBUFFERSTORAGEPROC glNamedBufferStorage = nullptr;
PFNGLNAMEDBUFFERDATAPROC glNamedBufferData = nullptr;
PFNGLNAMEDBUFFERSUBDATAPROC glNamedBufferSubData = nullptr;
PFNGLCOPYNAMEDBUFFERSUBDATAPROC glCopyNamedBufferSubData = nullptr;
PFNGLCLEARNAMEDBUFFERDATAPROC glClearNamedBufferData = nullptr;
PFNGLCLEARNAMEDBUFFERSUBDATAPROC glClearNamedBufferSubData = nullptr;
PFNGLMAPNAMEDBUFFERPROC glMapNamedBuffer = nullptr;
PFNGLMAPNAMEDBUFFERRANGEPROC glMapNamedBufferRange = nullptr;
PFNGLUNMAPNAMEDBUFFERPROC glUnmapNamedBuffer = nullptr;
PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC glFlushMappedNamedBufferRange = nullptr;
PFNGLGETNAMEDBUFFERPARAMETERIVPROC glGetNamedBufferParameteriv = nullptr;
PFNGLGETNAMEDBUFFERPARAMETERI64VPROC glGetNamedBufferParameteri64v = nullptr;
PFNGLGETNAMEDBUFFERPOINTERVPROC glGetNamedBufferPointerv = nullptr;
PFNGLGETNAMEDBUFFERSUBDATAPROC glGetNamedBufferSubData = nullptr;
PFNGLCREATEFRAMEBUFFERSPROC glCreateFramebuffers = nullptr;
PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC glNamedFramebufferRenderbuffer = nullptr;
PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC glNamedFramebufferParameteri = nullptr;
PFNGLNAMEDFRAMEBUFFERTEXTUREPROC glNamedFramebufferTexture = nullptr;
PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC glNamedFramebufferTextureLayer = nullptr;
PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC glNamedFramebufferDrawBuffer = nullptr;
PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC glNamedFramebufferDrawBuffers = nullptr;
PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC glNamedFramebufferReadBuffer = nullptr;
PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC glInvalidateNamedFramebufferData = nullptr;
PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC glInvalidateNamedFramebufferSubData = nullptr;
PFNGLCLEARNAMEDFRAMEBUFFERIVPROC glClearNamedFramebufferiv = nullptr;
PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC glClearNamedFramebufferuiv = nullptr;
PFNGLCLEARNAMEDFRAMEBUFFERFVPROC glClearNamedFramebufferfv = nullptr;
PFNGLCLEARNAMEDFRAMEBUFFERFIPROC glClearNamedFramebufferfi = nullptr;
PFNGLBLITNAMEDFRAMEBUFFERPROC glBlitNamedFramebuffer = nullptr;
PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC glCheckNamedFramebufferStatus = nullptr;
PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC glGetNamedFramebufferParameteriv = nullptr;
PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetNamedFramebufferAttachmentParameteriv = nullptr;
PFNGLCREATERENDERBUFFERSPROC glCreateRenderbuffers = nullptr;
PFNGLNAMEDRENDERBUFFERSTORAGEPROC glNamedRenderbufferStorage = nullptr;
PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC glNamedRenderbufferStorageMultisample = nullptr;
PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC glGetNamedRenderbufferParameteriv = nullptr;
PFNGLCREATETEXTURESPROC glCreateTextures = nullptr;
PFNGLTEXTUREBUFFERPROC glTextureBuffer = nullptr;
PFNGLTEXTUREBUFFERRANGEPROC glTextureBufferRange = nullptr;
PFNGLTEXTURESTORAGE1DPROC glTextureStorage1D = nullptr;
PFNGLTEXTURESTORAGE2DPROC glTextureStorage2D = nullptr;
PFNGLTEXTURESTORAGE3DPROC glTextureStorage3D = nullptr;
PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC glTextureStorage2DMultisample = nullptr;
PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC glTextureStorage3DMultisample = nullptr;
PFNGLTEXTURESUBIMAGE1DPROC glTextureSubImage1D = nullptr;
PFNGLTEXTURESUBIMAGE2DPROC glTextureSubImage2D = nullptr;
PFNGLTEXTURESUBIMAGE3DPROC glTextureSubImage3D = nullptr;
PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC glCompressedTextureSubImage1D = nullptr;
PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC glCompressedTextureSubImage2D = nullptr;
PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC glCompressedTextureSubImage3D = nullptr;
PFNGLCOPYTEXTURESUBIMAGE1DPROC glCopyTextureSubImage1D = nullptr;
PFNGLCOPYTEXTURESUBIMAGE2DPROC glCopyTextureSubImage2D = nullptr;
PFNGLCOPYTEXTURESUBIMAGE3DPROC glCopyTextureSubImage3D = nullptr;
PFNGLTEXTUREPARAMETERFPROC glTextureParameterf = nullptr;
PFNGLTEXTUREPARAMETERFVPROC glTextureParameterfv = nullptr;
PFNGLTEXTUREPARAMETERIPROC glTextureParameteri = nullptr;
PFNGLTEXTUREPARAMETERIIVPROC glTextureParameterIiv = nullptr;
PFNGLTEXTUREPARAMETERIUIVPROC glTextureParameterIuiv = nullptr;
PFNGLTEXTUREPARAMETERIVPROC glTextureParameteriv = nullptr;
PFNGLGENERATETEXTUREMIPMAPPROC glGenerateTextureMipmap = nullptr;
PFNGLBINDTEXTUREUNITPROC glBindTextureUnit = nullptr;
PFNGLGETTEXTUREIMAGEPROC glGetTextureImage = nullptr;
PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC glGetCompressedTextureImage = nullptr;
PFNGLGETTEXTURELEVELPARAMETERFVPROC glGetTextureLevelParameterfv = nullptr;
PFNGLGETTEXTURELEVELPARAMETERIVPROC glGetTextureLevelParameteriv = nullptr;
PFNGLGETTEXTUREPARAMETERFVPROC glGetTextureParameterfv = nullptr;
PFNGLGETTEXTUREPARAMETERIIVPROC glGetTextureParameterIiv = nullptr;
PFNGLGETTEXTUREPARAMETERIUIVPROC glGetTextureParameterIuiv = nullptr;
PFNGLGETTEXTUREPARAMETERIVPROC glGetTextureParameteriv = nullptr;
PFNGLCREATEVERTEXARRAYSPROC glCreateVertexArrays = nullptr;
PFNGLDISABLEVERTEXARRAYATTRIBPROC glDisableVertexArrayAttrib = nullptr;
PFNGLENABLEVERTEXARRAYATTRIBPROC glEnableVertexArrayAttrib = nullptr;
PFNGLVERTEXARRAYELEMENTBUFFERPROC glVertexArrayElementBuffer = nullptr;
PFNGLVERTEXARRAYVERTEXBUFFERPROC glVertexArrayVertexBuffer = nullptr;
PFNGLVERTEXARRAYVERTEXBUFFERSPROC glVertexArrayVertexBuffers = nullptr;
PFNGLVERTEXARRAYATTRIBBINDINGPROC glVertexArrayAttribBinding = nullptr;
PFNGLVERTEXARRAYATTRIBFORMATPROC glVertexArrayAttribFormat = nullptr;
PFNGLVERTEXARRAYATTRIBIFORMATPROC glVertexArrayAttribIFormat = nullptr;
PFNGLVERTEXARRAYATTRIBLFORMATPROC glVertexArrayAttribLFormat = nullptr;
PFNGLVERTEXARRAYBINDINGDIVISORPROC glVertexArrayBindingDivisor = nullptr;
PFNGLGETVERTEXARRAYIVPROC glGetVertexArrayiv = nullptr;
PFNGLGETVERTEXARRAYINDEXEDIVPROC glGetVertexArrayIndexediv = nullptr;
PFNGLGETVERTEXARRAYINDEXED64IVPROC glGetVertexArrayIndexed64iv = nullptr;
PFNGLCREATESAMPLERSPROC glCreateSamplers = nullptr;
PFNGLCREATEPROGRAMPIPELINESPROC glCreateProgramPipelines = nullptr;
PFNGLCREATEQUERIESPROC glCreateQueries = nullptr;
PFNGLGETQUERYBUFFEROBJECTI64VPROC glGetQueryBufferObjecti64v = nullptr;
PFNGLGETQUERYBUFFEROBJECTIVPROC glGetQueryBufferObjectiv = nullptr;
PFNGLGETQUERYBUFFEROBJECTUI64VPROC glGetQueryBufferObjectui64v = nullptr;
PFNGLGETQUERYBUFFEROBJECTUIVPROC glGetQueryBufferObjectuiv = nullptr;
PFNGLMEMORYBARRIERBYREGIONPROC glMemoryBarrierByRegion = nullptr;
PFNGLGETTEXTURESUBIMAGEPROC glGetTextureSubImage = nullptr;
PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC glGetCompressedTextureSubImage = nullptr;
PFNGLGETGRAPHICSRESETSTATUSPROC glGetGraphicsResetStatus = nullptr;
PFNGLGETNCOMPRESSEDTEXIMAGEPROC glGetnCompressedTexImage = nullptr;
PFNGLGETNTEXIMAGEPROC glGetnTexImage = nullptr;
PFNGLGETNUNIFORMDVPROC glGetnUniformdv = nullptr;
PFNGLGETNUNIFORMFVPROC glGetnUniformfv = nullptr;
PFNGLGETNUNIFORMIVPROC glGetnUniformiv = nullptr;
PFNGLGETNUNIFORMUIVPROC glGetnUniformuiv = nullptr;
PFNGLREADNPIXELSPROC glReadnPixels = nullptr;
PFNGLGETNMAPDVPROC glGetnMapdv = nullptr;
PFNGLGETNMAPFVPROC glGetnMapfv = nullptr;
PFNGLGETNMAPIVPROC glGetnMapiv = nullptr;
PFNGLGETNPIXELMAPFVPROC glGetnPixelMapfv = nullptr;
PFNGLGETNPIXELMAPUIVPROC glGetnPixelMapuiv = nullptr;
PFNGLGETNPIXELMAPUSVPROC glGetnPixelMapusv = nullptr;
PFNGLGETNPOLYGONSTIPPLEPROC glGetnPolygonStipple = nullptr;
PFNGLGETNCOLORTABLEPROC glGetnColorTable = nullptr;
PFNGLGETNCONVOLUTIONFILTERPROC glGetnConvolutionFilter = nullptr;
PFNGLGETNSEPARABLEFILTERPROC glGetnSeparableFilter = nullptr;
PFNGLGETNHISTOGRAMPROC glGetnHistogram = nullptr;
PFNGLGETNMINMAXPROC glGetnMinmax = nullptr;
PFNGLTEXTUREBARRIERPROC glTextureBarrier = nullptr;
#endif // O3D_GL_VERSION_4_5

#ifdef O3D_GL_VERSION_4_6
PFNGLSPECIALIZESHADERPROC glSpecializeShader = nullptr;
PFNGLMULTIDRAWARRAYSINDIRECTCOUNTPROC glMultiDrawArraysIndirectCount = nullptr;
PFNGLMULTIDRAWELEMENTSINDIRECTCOUNTPROC glMultiDrawElementsIndirectCount = nullptr;
PFNGLPOLYGONOFFSETCLAMPPROC glPolygonOffsetClamp = nullptr;
#endif // O3D_GL_VERSION_4_6

#ifdef O3D_GL_ARB_ES3_2_compatibility
PFNGLPRIMITIVEBOUNDINGBOXARBPROC glPrimitiveBoundingBox = nullptr;
#endif // O3D_GL_ARB_ES3_2_compatibility

#undef Bool

// statics members
Bool GLExtensionManager::ms_valid = False;
DynamicLibrary* GLExtensionManager::ms_openGL = nullptr;

#define REQUIRED_EXT(EXT) if (!isExtensionSupported(String("GL_") + #EXT)) { O3D_ERROR(E_UnsuportedFeature(String("OpenGL ") + #EXT)); }

void GLExtensionManager::getGLCommonFunctions()
{
#ifdef O3D_GL_VERSION_1_0  // GL COMMON
    _glGetIntegerv = (PFNGLGETINTEGERVPROC) GL::getProcAddress("glGetIntegerv");
    _glGetString = (PFNGLGETSTRINGPROC) GL::getProcAddress("glGetString");

    glCullFace = (PFNGLCULLFACEPROC) GL::getProcAddress("glCullFace");
    glFrontFace = (PFNGLFRONTFACEPROC) GL::getProcAddress("glFrontFace");
    glHint = (PFNGLHINTPROC) GL::getProcAddress("glHint");
    glLineWidth = (PFNGLLINEWIDTHPROC) GL::getProcAddress("glLineWidth");
    glScissor = (PFNGLSCISSORPROC) GL::getProcAddress("glScissor");
    glTexParameterf = (PFNGLTEXPARAMETERFPROC) GL::getProcAddress("glTexParameterf");
    glTexParameterfv = (PFNGLTEXPARAMETERFVPROC) GL::getProcAddress("glTexParameterfv");
    glTexParameteri = (PFNGLTEXPARAMETERIPROC) GL::getProcAddress("glTexParameteri");
    glTexParameteriv = (PFNGLTEXPARAMETERIVPROC) GL::getProcAddress("glTexParameteriv");
    glTexImage2D = (PFNGLTEXIMAGE2DPROC) GL::getProcAddress("glTexImage2D");
    glDrawBuffer = (PFNGLDRAWBUFFERPROC) GL::getProcAddress("glDrawBuffer");
    glClear = (PFNGLCLEARPROC) GL::getProcAddress("glClear");
    glClearColor = (PFNGLCLEARCOLORPROC) GL::getProcAddress("glClearColor");
    glClearStencil = (PFNGLCLEARSTENCILPROC) GL::getProcAddress("glClearStencil");
    glClearDepth = (PFNGLCLEARDEPTHPROC) GL::getProcAddress("glClearDepth");
    glStencilMask = (PFNGLSTENCILMASKPROC) GL::getProcAddress("glStencilMask");
    glColorMask = (PFNGLCOLORMASKPROC) GL::getProcAddress("glColorMask");
    glDepthMask = (PFNGLDEPTHMASKPROC) GL::getProcAddress("glDepthMask");
    glDisable = (PFNGLDISABLEPROC) GL::getProcAddress("glDisable");
    glEnable = (PFNGLENABLEPROC) GL::getProcAddress("glEnable");
    glFinish = (PFNGLFINISHPROC) GL::getProcAddress("glFinish");
    glFlush = (PFNGLFLUSHPROC) GL::getProcAddress("glFlush");
    glBlendFunc = (PFNGLBLENDFUNCPROC) GL::getProcAddress("glBlendFunc");
    glStencilFunc = (PFNGLSTENCILFUNCPROC) GL::getProcAddress("glStencilFunc");
    glStencilOp = (PFNGLSTENCILOPPROC) GL::getProcAddress("glStencilOp");
    glDepthFunc = (PFNGLDEPTHFUNCPROC) GL::getProcAddress("glDepthFunc");
    glPixelStorei = (PFNGLPIXELSTOREIPROC) GL::getProcAddress("glPixelStorei");
    glReadBuffer = (PFNGLREADBUFFERPROC) GL::getProcAddress("glReadBuffer");
    glReadPixels = (PFNGLREADPIXELSPROC) GL::getProcAddress("glReadPixels");
    glGetBooleanv = (PFNGLGETBOOLEANVPROC) GL::getProcAddress("glGetBooleanv");
    glGetError = (PFNGLGETERRORPROC) GL::getProcAddress("glGetError");
    glGetFloatv = (PFNGLGETFLOATVPROC) GL::getProcAddress("glGetFloatv");
    glGetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC) GL::getProcAddress("glGetTexParameterfv");
    glGetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC) GL::getProcAddress("glGetTexParameteriv");
    glIsEnabled = (PFNGLISENABLEDPROC) GL::getProcAddress("glIsEnabled");
    glDepthRange = (PFNGLDEPTHRANGEPROC) GL::getProcAddress("glDepthRange");
    glViewport = (PFNGLVIEWPORTPROC) GL::getProcAddress("glViewport");
#endif

#ifdef O3D_GL_VERSION_1_1  // GL COMMON
    glDrawArrays = (PFNGLDRAWARRAYSPROC) GL::getProcAddress("glDrawArrays");
    glDrawElements = (PFNGLDRAWELEMENTSPROC) GL::getProcAddress("glDrawElements");
    glPolygonOffset = (PFNGLPOLYGONOFFSETPROC) GL::getProcAddress("glPolygonOffset");
    glCopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC) GL::getProcAddress("glCopyTexImage2D");
    glCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC) GL::getProcAddress("glCopyTexSubImage2D");
    glTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC) GL::getProcAddress("glTexSubImage2D");
    glBindTexture = (PFNGLBINDTEXTUREPROC) GL::getProcAddress("glBindTexture");
    glDeleteTextures = (PFNGLDELETETEXTURESPROC) GL::getProcAddress("glDeleteTextures");
    glGenTextures = (PFNGLGENTEXTURESPROC) GL::getProcAddress("glGenTextures");
    glIsTexture = (PFNGLISTEXTUREPROC) GL::getProcAddress("glIsTexture");
#endif

#ifdef O3D_GL_VERSION_1_2  // GL COMMON
    glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC) GL::getProcAddress("glDrawRangeElements");
    if (!glDrawRangeElements) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL draw range elements"));
    }

    glTexImage3D = (PFNGLTEXIMAGE3DPROC) GL::getProcAddress("glTexImage3D");
    glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC) GL::getProcAddress("glTexSubImage3D");
    if (!glTexImage3D || !glTexSubImage3D) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL texture 3D"));
    }
#endif // O3D_GL_VERSION_1_2

#ifdef O3D_GL_VERSION_1_3  // GL COMMON
    glActiveTexture = (PFNGLACTIVETEXTUREPROC) GL::getProcAddress("glActiveTexture");
    if (!glActiveTexture && isExtensionSupported("GL_ARB_multitexture")) {
        glActiveTexture = (PFNGLACTIVETEXTUREPROC) GL::getProcAddress("glActiveTextureARB");
    }
    if (!glActiveTexture) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL active texture"));
    }

    glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) GL::getProcAddress("glCompressedTexImage2D");
    glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC) GL::getProcAddress("glCompressedTexImage3D");
    glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) GL::getProcAddress("glCompressedTexSubImage2D");
    glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) GL::getProcAddress("glCompressedTexSubImage3D");

    if ((!glCompressedTexImage2D || !glCompressedTexImage3D ||
         !glCompressedTexSubImage2D || !glCompressedTexSubImage3D) &&
         isExtensionSupported("GL_ARB_texture_compression")) {

        glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) GL::getProcAddress("glCompressedTexImage2DARB");
        glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC) GL::getProcAddress("glCompressedTexImage3DARB");
        glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) GL::getProcAddress("glCompressedTexSubImage2DARB");
        glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) GL::getProcAddress("glCompressedTexSubImage3DARB");
    }

    if (!glCompressedTexImage2D || !glCompressedTexSubImage2D) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL compressed texture 2D"));
    }

    if (!glCompressedTexImage3D || !glCompressedTexSubImage3D) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL compressed texture 3D"));
    }
#endif // O3D_GL_VERSION_1_3

#ifdef O3D_GL_VERSION_1_4  // GL COMMON
    glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) GL::getProcAddress("glBlendFuncSeparate");
    if (!glBlendFuncSeparate) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL blend func separate"));
    }

    glBlendEquation = (PFNGLBLENDEQUATIONPROC) GL::getProcAddress("glBlendEquation");
    if (!glBlendEquation) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL blend equation"));
    }
#endif // O3D_GL_VERSION_1_4

#ifdef O3D_GL_VERSION_1_5  // GL COMMON
    glBindBuffer = (PFNGLBINDBUFFERPROC) GL::getProcAddress("glBindBuffer");
    glGenBuffers = (PFNGLGENBUFFERSPROC) GL::getProcAddress("glGenBuffers");
    glBufferData = (PFNGLBUFFERDATAPROC) GL::getProcAddress("glBufferData");
    glBufferSubData = (PFNGLBUFFERSUBDATAPROC) GL::getProcAddress("glBufferSubData");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) GL::getProcAddress("glDeleteBuffers");
    glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC) GL::getProcAddress("glGetBufferParameteriv");
    glUnmapBuffer = (PFNGLUNMAPBUFFERPROC) GL::getProcAddress("glUnmapBuffer");

    if ((!glBindBuffer || !glDeleteBuffers || !glGenBuffers || !glBufferData ||
         !glGetBufferParameteriv || !glUnmapBuffer) &&
         isExtensionSupported("GL_ARB_vertex_buffer_object")) {

        glBindBuffer = (PFNGLBINDBUFFERPROC) GL::getProcAddress("glBindBufferARB");
        glGenBuffers = (PFNGLGENBUFFERSPROC) GL::getProcAddress("glGenBuffersARB");
        glBufferData = (PFNGLBUFFERDATAPROC) GL::getProcAddress("glBufferDataARB");
        glBufferSubData = (PFNGLBUFFERSUBDATAPROC) GL::getProcAddress("glBufferSubDataARB");
        glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) GL::getProcAddress("glDeleteBuffersARB");
        glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC) GL::getProcAddress("glGetBufferParameterivARB");
        glUnmapBuffer = (PFNGLUNMAPBUFFERPROC) GL::getProcAddress("glUnmapBufferARB");
    }

    if (!glBindBuffer|| !glDeleteBuffers|| !glGenBuffers|| !glBufferData ||
        !glGetBufferParameteriv|| !glUnmapBuffer) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Buffer Object"));
    }

    // OcclusionQuery (optional)
    glGenQueries = (PFNGLGENQUERIESPROC)GL::getProcAddress("glGenQueries");
    glIsQuery = (PFNGLISQUERYPROC)GL::getProcAddress("glIsQuery");
    glBeginQuery = (PFNGLBEGINQUERYPROC)GL::getProcAddress("glBeginQuery");
    glEndQuery = (PFNGLENDQUERYPROC)GL::getProcAddress("glEndQuery");
    glGetQueryiv = (PFNGLGETQUERYIVPROC)GL::getProcAddress("glGetQueryiv");
    glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)GL::getProcAddress("glGetQueryObjectuiv");
    glDeleteQueries = (PFNGLDELETEQUERIESPROC)GL::getProcAddress("glDeleteQueries");

    if ((!glGenQueries || !glIsQuery || !glBeginQuery || !glEndQuery || !glGetQueryiv ||
         !glGetQueryObjectuiv || !glDeleteQueries) &&
         isExtensionSupported("GL_ARB_occlusion_query")) {

        glGenQueries = (PFNGLGENQUERIESPROC)GL::getProcAddress("glGenQueriesARB");
        glIsQuery = (PFNGLISQUERYPROC)GL::getProcAddress("glIsQueryARB");
        glBeginQuery = (PFNGLBEGINQUERYPROC)GL::getProcAddress("glBeginQueryARB");
        glEndQuery = (PFNGLENDQUERYPROC)GL::getProcAddress("glEndQueryARB");
        glGetQueryiv = (PFNGLGETQUERYIVPROC)GL::getProcAddress("glGetQueryivARB");
        glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)GL::getProcAddress("glGetQueryObjectivARB");
        glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)GL::getProcAddress("glGetQueryObjectuivARB");
        glDeleteQueries = (PFNGLDELETEQUERIESPROC)GL::getProcAddress("glDeleteQueriesARB");
    }

    if (!glGenQueries || !glIsQuery || !glBeginQuery || !glEndQuery || !glGetQueryiv ||
        !glGetQueryObjectuiv || !glDeleteQueries) {
        O3D_WARNING("OpenGL occlusion query is not available");
    }
#endif // O3D_GL_VERSION_1_5

#ifdef O3D_GL_VERSION_2_0  // GL COMMON
    glCreateShader = (PFNGLCREATESHADERPROC) GL::getProcAddress("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC) GL::getProcAddress("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC) GL::getProcAddress("glCompileShader");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC) GL::getProcAddress("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC) GL::getProcAddress("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC) GL::getProcAddress("glLinkProgram");
    glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)GL::getProcAddress("glValidateProgram");
    glUseProgram = (PFNGLUSEPROGRAMPROC) GL::getProcAddress("glUseProgram");

    glDetachShader = (PFNGLDETACHSHADERPROC) GL::getProcAddress("glDetachShader");
    glDeleteShader = (PFNGLDELETESHADERPROC) GL::getProcAddress("glDeleteShader");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC) GL::getProcAddress("glDeleteProgram");

    glGetProgramiv = (PFNGLGETPROGRAMIVPROC) GL::getProcAddress("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) GL::getProcAddress("glGetProgramInfoLog");
    glGetShaderiv = (PFNGLGETSHADERIVPROC) GL::getProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) GL::getProcAddress("glGetShaderInfoLog");
    glIsProgram = (PFNGLISPROGRAMPROC) GL::getProcAddress("glIsProgram");
    glIsShader = (PFNGLISSHADERPROC) GL::getProcAddress("glIsShader");

    glGetUniformLocation= (PFNGLGETUNIFORMLOCATIONPROC) GL::getProcAddress("glGetUniformLocation");
    glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC) GL::getProcAddress("glGetActiveUniform");
    glUniform1f = (PFNGLUNIFORM1FPROC) GL::getProcAddress("glUniform1f");
    glUniform2f = (PFNGLUNIFORM2FPROC) GL::getProcAddress("glUniform2f");
    glUniform3f = (PFNGLUNIFORM3FPROC) GL::getProcAddress("glUniform3f");
    glUniform4f = (PFNGLUNIFORM4FPROC) GL::getProcAddress("glUniform4f");
    glUniform1fv = (PFNGLUNIFORM1FVPROC) GL::getProcAddress("glUniform1fv");
    glUniform2fv = (PFNGLUNIFORM2FVPROC) GL::getProcAddress("glUniform2fv");
    glUniform3fv = (PFNGLUNIFORM3FVPROC) GL::getProcAddress("glUniform3fv");
    glUniform4fv = (PFNGLUNIFORM4FVPROC) GL::getProcAddress("glUniform4fv");
    glUniform1i = (PFNGLUNIFORM1IPROC) GL::getProcAddress("glUniform1i");
    glUniform2i = (PFNGLUNIFORM2IPROC) GL::getProcAddress("glUniform2i");
    glUniform3i = (PFNGLUNIFORM3IPROC) GL::getProcAddress("glUniform3i");
    glUniform4i = (PFNGLUNIFORM4IPROC) GL::getProcAddress("glUniform4i");
    glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC) GL::getProcAddress("glUniformMatrix2fv");
    glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC) GL::getProcAddress("glUniformMatrix3fv");
    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) GL::getProcAddress("glUniformMatrix4fv");

    glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) GL::getProcAddress("glDisableVertexAttribArray");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) GL::getProcAddress("glEnableVertexAttribArray");
    glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC) GL::getProcAddress("glGetActiveAttrib");
    glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) GL::getProcAddress("glBindAttribLocation");
    glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) GL::getProcAddress("glGetAttribLocation");
    glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC) GL::getProcAddress("glGetVertexAttribfv");
    glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC) GL::getProcAddress("glGetVertexAttribiv");
    glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC) GL::getProcAddress("glGetVertexAttribPointerv");

    glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC) GL::getProcAddress("glVertexAttrib1f");
    glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC) GL::getProcAddress("glVertexAttrib1fv");
    glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC) GL::getProcAddress("glVertexAttrib2f");
    glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC) GL::getProcAddress("glVertexAttrib2fv");
    glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC) GL::getProcAddress("glVertexAttrib3f");
    glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC) GL::getProcAddress("glVertexAttrib3fv");
    glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC) GL::getProcAddress("glVertexAttrib4f");
    glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC) GL::getProcAddress("glVertexAttrib4fv");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) GL::getProcAddress("glVertexAttribPointer");

    if ((!glCreateShader || !glShaderSource || !glCompileShader || !glCreateProgram  ||
        !glAttachShader || !glLinkProgram || !glValidateProgram || !glUseProgram  ||
        !glDetachShader || !glDeleteShader || !glDeleteProgram || !glGetProgramInfoLog ||
        !glGetShaderInfoLog || !glIsProgram || !glIsShader || !glGetUniformLocation  ||
        !glGetProgramiv || !glUniform1f || !glUniform2f || !glUniform3f || !glUniform4f ||
        !glUniform1fv || !glUniform2fv || !glUniform3fv || !glUniform4fv ||
        !glUniform1i || !glUniform2i || !glUniform3i || !glUniform4i ||
        !glUniformMatrix2fv || !glUniformMatrix3fv || !glUniformMatrix4fv ||
        !glGetShaderiv || !glGetActiveUniform ||
        !glBindAttribLocation || !glGetAttribLocation || !glGetVertexAttribfv  ||
        !glGetVertexAttribiv || !glGetVertexAttribPointerv ||
        !glVertexAttrib1f || !glVertexAttrib1fv || !glVertexAttrib2f || !glVertexAttrib2fv ||
        !glVertexAttrib3f || !glVertexAttrib3fv || !glVertexAttrib4f || !glVertexAttrib4fv ||
        !glVertexAttribPointer) && isExtensionSupported("GL_ARB_shading_language_100")) {

        glCreateShader = (PFNGLCREATESHADERPROC) GL::getProcAddress("glCreateShaderObjectARB");
        glShaderSource = (PFNGLSHADERSOURCEPROC) GL::getProcAddress("glShaderSourceARB");
        glCompileShader = (PFNGLCOMPILESHADERPROC) GL::getProcAddress("glCompileShaderARB");
        glCreateProgram = (PFNGLCREATEPROGRAMPROC) GL::getProcAddress("glCreateProgramObjectARB");
        glAttachShader = (PFNGLATTACHSHADERPROC) GL::getProcAddress("glAttachObjectARB");
        glLinkProgram = (PFNGLLINKPROGRAMPROC) GL::getProcAddress("glLinkProgramARB");
        glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)GL::getProcAddress("glValidateProgramARB");
        glUseProgram = (PFNGLUSEPROGRAMPROC) GL::getProcAddress("glUseProgramObjectARB");

        glDetachShader = (PFNGLDETACHSHADERPROC) GL::getProcAddress("glDetachObjectARB");
        glDeleteShader = (PFNGLDELETESHADERPROC) GL::getProcAddress("glDeleteObjectARB");
        glDeleteProgram = (PFNGLDELETEPROGRAMPROC) GL::getProcAddress("glDeleteObjectARB");

        glGetProgramiv = (PFNGLGETPROGRAMIVPROC) GL::getProcAddress("glGetObjectParameterivARB");
        glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) GL::getProcAddress("glGetInfoLogARB");
        glGetShaderiv = (PFNGLGETSHADERIVPROC) GL::getProcAddress("glGetObjectParameterivARB");
        glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) GL::getProcAddress("glGetInfoLogARB");
        glIsProgram = (PFNGLISPROGRAMPROC) GL::getProcAddress("glIsProgramARB"); // Not sure
        glIsShader = (PFNGLISSHADERPROC) GL::getProcAddress("glIsProgramARB"); // Not sure

        glGetUniformLocation   = (PFNGLGETUNIFORMLOCATIONPROC) GL::getProcAddress("glGetUniformLocationARB");
        glGetActiveUniform     = (PFNGLGETACTIVEUNIFORMPROC) GL::getProcAddress("glGetActiveUniformARB");
        glUniform1f = (PFNGLUNIFORM1FPROC) GL::getProcAddress("glUniform1fARB");
        glUniform2f = (PFNGLUNIFORM2FPROC) GL::getProcAddress("glUniform2fARB");
        glUniform3f = (PFNGLUNIFORM3FPROC) GL::getProcAddress("glUniform3fARB");
        glUniform4f = (PFNGLUNIFORM4FPROC) GL::getProcAddress("glUniform4fARB");
        glUniform1fv = (PFNGLUNIFORM1FVPROC) GL::getProcAddress("glUniform1fvARB");
        glUniform2fv = (PFNGLUNIFORM2FVPROC) GL::getProcAddress("glUniform2fvARB");
        glUniform3fv = (PFNGLUNIFORM3FVPROC) GL::getProcAddress("glUniform3fvARB");
        glUniform4fv = (PFNGLUNIFORM4FVPROC) GL::getProcAddress("glUniform4fvARB");
        glUniform1i = (PFNGLUNIFORM1IPROC) GL::getProcAddress("glUniform1iARB");
        glUniform2i = (PFNGLUNIFORM2IPROC) GL::getProcAddress("glUniform2iARB");
        glUniform3i = (PFNGLUNIFORM3IPROC) GL::getProcAddress("glUniform3iARB");
        glUniform4i = (PFNGLUNIFORM4IPROC) GL::getProcAddress("glUniform4iARB");
        glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC) GL::getProcAddress("glUniformMatrix2fvARB");
        glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC) GL::getProcAddress("glUniformMatrix3fvARB");
        glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) GL::getProcAddress("glUniformMatrix4fvARB");

        glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) GL::getProcAddress("glDisableVertexAttribArrayARB");
        glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) GL::getProcAddress("glEnableVertexAttribArrayARB");
        glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC) GL::getProcAddress("glGetActiveAttribARB");
        glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) GL::getProcAddress("glBindAttribLocationARB");
        glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) GL::getProcAddress("glGetAttribLocationARB");
        glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC) GL::getProcAddress("glGetVertexAttribdvARB");
        glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC) GL::getProcAddress("glGetVertexAttribfvARB");
        glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC) GL::getProcAddress("glGetVertexAttribivARB");
        glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC) GL::getProcAddress("glGetVertexAttribPointervARB");

        glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC) GL::getProcAddress("glVertexAttrib1dARB");
        glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC) GL::getProcAddress("glVertexAttrib1dvARB");
        glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC) GL::getProcAddress("glVertexAttrib1fARB");
        glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC) GL::getProcAddress("glVertexAttrib1fvARB");
        glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC) GL::getProcAddress("glVertexAttrib1sARB");
        glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC) GL::getProcAddress("glVertexAttrib1svARB");
        glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC) GL::getProcAddress("glVertexAttrib2dARB");
        glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC) GL::getProcAddress("glVertexAttrib2dvARB");
        glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC) GL::getProcAddress("glVertexAttrib2fARB");
        glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC) GL::getProcAddress("glVertexAttrib2fvARB");
        glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC) GL::getProcAddress("glVertexAttrib2sARB");
        glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC) GL::getProcAddress("glVertexAttrib2svARB");
        glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC) GL::getProcAddress("glVertexAttrib3dARB");
        glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC) GL::getProcAddress("glVertexAttrib3dvARB");
        glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC) GL::getProcAddress("glVertexAttrib3fARB");
        glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC) GL::getProcAddress("glVertexAttrib3fvARB");
        glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC) GL::getProcAddress("glVertexAttrib3sARB");
        glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC) GL::getProcAddress("glVertexAttrib3svARB");
        glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC) GL::getProcAddress("glVertexAttrib4NbvARB");
        glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC) GL::getProcAddress("glVertexAttrib4NivARB");
        glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC) GL::getProcAddress("glVertexAttrib4NsvARB");
        glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC) GL::getProcAddress("glVertexAttrib4NubARB");
        glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC) GL::getProcAddress("glVertexAttrib4NubvARB");
        glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC) GL::getProcAddress("glVertexAttrib4NuivARB");
        glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC) GL::getProcAddress("glVertexAttrib4NusvARB");
        glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC) GL::getProcAddress("glVertexAttrib4bvARB");
        glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC) GL::getProcAddress("glVertexAttrib4dARB");
        glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC) GL::getProcAddress("glVertexAttrib4dvARB");
        glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC) GL::getProcAddress("glVertexAttrib4fARB");
        glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC) GL::getProcAddress("glVertexAttrib4fvARB");
        glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC) GL::getProcAddress("glVertexAttrib4ivARB");
        glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC) GL::getProcAddress("glVertexAttrib4sARB");
        glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC) GL::getProcAddress("glVertexAttrib4svARB");
        glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC) GL::getProcAddress("glVertexAttrib4ubvARB");
        glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC) GL::getProcAddress("glVertexAttrib4uivARB");
        glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC) GL::getProcAddress("glVertexAttrib4usvARB");
        glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)GL::getProcAddress("glVertexAttribPointerARB");
    }

    if (!glCreateShader || !glShaderSource || !glCompileShader || !glCreateProgram  ||
        !glAttachShader || !glLinkProgram || !glValidateProgram || !glUseProgram  ||
        !glDetachShader || !glDeleteShader || !glDeleteProgram || !glGetProgramInfoLog  ||
        !glGetShaderInfoLog || !glIsProgram || !glIsShader || !glGetUniformLocation ||
        !glGetProgramiv || !glUniform1f || !glUniform2f || !glUniform3f || !glUniform4f  ||
        !glUniform1fv || !glUniform2fv || !glUniform3fv || !glUniform4fv  ||
        !glUniform1i || !glUniform2i || !glUniform3i || !glUniform4i ||
        !glUniformMatrix2fv || !glUniformMatrix3fv || !glUniformMatrix4fv  ||
        !glGetShaderiv || !glGetActiveUniform  ||
        !glBindAttribLocation || !glGetAttribLocation || !glGetVertexAttribfv  ||
        !glGetVertexAttribiv || !glGetVertexAttribPointerv ||
        !glVertexAttrib1f || !glVertexAttrib1fv || !glVertexAttrib2f || !glVertexAttrib2fv ||
        !glVertexAttrib3f || !glVertexAttrib3fv || !glVertexAttrib4f || !glVertexAttrib4fv ||
        !glVertexAttribPointer) {

        O3D_ERROR(E_UnsuportedFeature("OpenGL GLSL"));
    }

    // Blend equation separate
    glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC) GL::getProcAddress("glBlendEquationSeparate");
    if (!glBlendEquationSeparate) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Blend equation separate"));
    }

    // Draw buffers
    glDrawBuffers = (PFNGLDRAWBUFFERSPROC) GL::getProcAddress("glDrawBuffers");
    if (!glDrawBuffers && isExtensionSupported("GL_ARB_draw_buffers")) {
        glDrawBuffers = (PFNGLDRAWBUFFERSPROC) GL::getProcAddress("glDrawBuffersARB");
    }

    if (!glDrawBuffers) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Draw buffers"));
    }

    // Stencil separate
    glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC) GL::getProcAddress("glStencilOpSeparate");
    glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC) GL::getProcAddress("glStencilFuncSeparate");
    glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC) GL::getProcAddress("glStencilMaskSeparate");

    if (!glStencilOpSeparate || !glStencilFuncSeparate || !glStencilMaskSeparate) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL stencil separate functions"));
    }
#endif // O3D_GL_VERSION_2_0

#ifdef O3D_GL_VERSION_3_0  // GL COMMON
    glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC) GL::getProcAddress("glGetBooleani_v");
    glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC) GL::getProcAddress("glGetIntegeri_v");
    glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC) GL::getProcAddress("glBeginTransformFeedback");
    glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC) GL::getProcAddress("glEndTransformFeedback");
    glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC) GL::getProcAddress("glBindBufferRange");
    glBindBufferBase = (PFNGLBINDBUFFERBASEPROC) GL::getProcAddress("glBindBufferBase");
    glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC) GL::getProcAddress("glTransformFeedbackVaryings");
    glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC) GL::getProcAddress("glGetTransformFeedbackVarying");
    glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC) GL::getProcAddress("glVertexAttribIPointer");
    glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC) GL::getProcAddress("glGetVertexAttribIiv");
    glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC) GL::getProcAddress("glGetVertexAttribIuiv");
    glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC) GL::getProcAddress("glGetUniformuiv");
    glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC) GL::getProcAddress("glGetFragDataLocation");
    glUniform1ui = (PFNGLUNIFORM1UIPROC) GL::getProcAddress("glUniform1ui");
    glUniform2ui = (PFNGLUNIFORM2UIPROC) GL::getProcAddress("glUniform2ui");
    glUniform3ui = (PFNGLUNIFORM3UIPROC) GL::getProcAddress("glUniform3ui");
    glUniform4ui = (PFNGLUNIFORM4UIPROC) GL::getProcAddress("glUniform4ui");
    glUniform1uiv = (PFNGLUNIFORM1UIVPROC) GL::getProcAddress("glUniform1uiv");
    glUniform2uiv = (PFNGLUNIFORM2UIVPROC) GL::getProcAddress("glUniform2uiv");
    glUniform3uiv = (PFNGLUNIFORM3UIVPROC) GL::getProcAddress("glUniform3uiv");
    glUniform4uiv = (PFNGLUNIFORM4UIPROC) GL::getProcAddress("glUniform4uiv");
    glClearBufferiv = (PFNGLCLEARBUFFERIVPROC) GL::getProcAddress("glClearBufferiv");
    glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC) GL::getProcAddress("glClearBufferuiv");
    glClearBufferfv = (PFNGLCLEARBUFFERFVPROC) GL::getProcAddress("glClearBufferfv");
    glClearBufferfi = (PFNGLCLEARBUFFERFIPROC) GL::getProcAddress("glClearBufferfi");

    if (!glGetBooleani_v || !glGetIntegeri_v ||
        !glBeginTransformFeedback || !glEndTransformFeedback ||
        !glBindBufferRange || !glBindBufferBase ||
        !glTransformFeedbackVaryings || !glGetTransformFeedbackVarying ||
        !glGetUniformuiv || !glGetFragDataLocation ||
        !glUniform1ui || !glUniform2ui || !glUniform3ui || !glUniform4ui ||
        !glUniform1uiv || !glUniform2uiv || !glUniform3uiv || !glUniform4uiv ||
        !glClearBufferiv || !glClearBufferuiv || !glClearBufferfv ||!glClearBufferfi) {

        O3D_ERROR(E_UnsuportedFeature("OpenGL 3.0 related functions"));
    }

    // GL_ARB_map_buffer_range
    glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC) GL::getProcAddress("glMapBufferRange");
    glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC) GL::getProcAddress("glFlushMappedBufferRange");

    if ((!glMapBufferRange || !glFlushMappedBufferRange) &&
         isExtensionSupported("GL_ARB_map_buffer_range")) {
        glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC) GL::getProcAddress("glMapBufferRangeARB");
        glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC) GL::getProcAddress("glFlushMappedBufferRangeARB");
    }

    if (!glMapBufferRange || !glFlushMappedBufferRange) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL map buffer object range is not available"));
    }

    // GL_ARB_vertex_array_object
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) GL::getProcAddress("glBindVertexArray");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) GL::getProcAddress("glDeleteVertexArrays");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) GL::getProcAddress("glGenVertexArrays");
    glIsVertexArray = (PFNGLISVERTEXARRAYPROC) GL::getProcAddress("glIsVertexArray");

    if ((!glBindVertexArray || !glDeleteVertexArrays || !glGenVertexArrays || !glIsVertexArray) &&
        isExtensionSupported("GL_ARB_vertex_array_object")) {

        glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) GL::getProcAddress("glBindVertexArrayARB");
        glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) GL::getProcAddress("glDeleteVertexArraysARB");
        glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) GL::getProcAddress("glGenVertexArraysARB");
        glIsVertexArray = (PFNGLISVERTEXARRAYPROC) GL::getProcAddress("glIsVertexArrayARB");
    }

    if (!glBindVertexArray || !glDeleteVertexArrays || !glGenVertexArrays || !glIsVertexArray) {
        O3D_WARNING("OpenGL VAO is not available");
    }

    // GL_ARB_framebuffer_object
    glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) GL::getProcAddress("glIsRenderbuffer");
    glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) GL::getProcAddress("glBindRenderbuffer");
    glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) GL::getProcAddress("glDeleteRenderbuffers");
    glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) GL::getProcAddress("glGenRenderbuffers");
    glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) GL::getProcAddress("glRenderbufferStorage");
    glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) GL::getProcAddress("glGetRenderbufferParameteriv");
    glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) GL::getProcAddress("glIsFramebuffer");
    glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) GL::getProcAddress("glBindFramebuffer");
    glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) GL::getProcAddress("glDeleteFramebuffers");
    glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) GL::getProcAddress("glGenFramebuffers");
    glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) GL::getProcAddress("glCheckFramebufferStatus");
    glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) GL::getProcAddress("glFramebufferTexture2D");
    glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) GL::getProcAddress("glFramebufferRenderbuffer");
    glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) GL::getProcAddress("glGetFramebufferAttachmentParameteriv");
    glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC) GL::getProcAddress("glBlitFramebuffer");
    glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) GL::getProcAddress("glRenderbufferStorageMultisample");
    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) GL::getProcAddress("glGenerateMipmap");
    glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC) GL::getProcAddress("glFramebufferTextureLayer");

    if ((!glIsRenderbuffer || !glBindRenderbuffer || !glDeleteRenderbuffers ||
         !glGenRenderbuffers || !glRenderbufferStorage || !glGetRenderbufferParameteriv ||
         !glIsFramebuffer || !glBindFramebuffer || !glDeleteFramebuffers ||
         !glGenFramebuffers || !glCheckFramebufferStatus ||
         !glFramebufferTexture2D || !glFramebufferRenderbuffer ||
         !glGetFramebufferAttachmentParameteriv || !glBlitFramebuffer ||
         !glRenderbufferStorageMultisample || !glGenerateMipmap ||!glFramebufferTextureLayer) &&
         isExtensionSupported("GL_ARB_framebuffer_object")) {

        glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) GL::getProcAddress("glIsRenderbufferARB");
        glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) GL::getProcAddress("glBindRenderbufferARB");
        glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) GL::getProcAddress("glDeleteRenderbuffersARB");
        glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) GL::getProcAddress("glGenRenderbuffersARB");
        glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) GL::getProcAddress("glRenderbufferStorageARB");
        glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) GL::getProcAddress("glGetRenderbufferParameterivARB");
        glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) GL::getProcAddress("glIsFramebufferARB");
        glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) GL::getProcAddress("glBindFramebufferARB");
        glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) GL::getProcAddress("glDeleteFramebuffersARB");
        glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) GL::getProcAddress("glGenFramebuffersARB");
        glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) GL::getProcAddress("glCheckFramebufferStatusARB");
        glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) GL::getProcAddress("glFramebufferTexture1DARB");
        glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) GL::getProcAddress("glFramebufferTexture2DARB");
        glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) GL::getProcAddress("glFramebufferTexture3DARB");
        glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) GL::getProcAddress("glFramebufferRenderbufferARB");
        glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) GL::getProcAddress("glGetFramebufferAttachmentParameterivARB");
        glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC) GL::getProcAddress("glBlitFramebufferARB");
        glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) GL::getProcAddress("glRenderbufferStorageMultisampleARB");
        glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) GL::getProcAddress("glGenerateMipmapARB");
        glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC) GL::getProcAddress("glFramebufferTextureLayerARB");
    }

    if ((!glIsRenderbuffer || !glBindRenderbuffer || !glDeleteRenderbuffers ||
         !glGenRenderbuffers || !glRenderbufferStorage || !glGetRenderbufferParameteriv ||
         !glIsFramebuffer || !glBindFramebuffer || !glDeleteFramebuffers ||
         !glGenFramebuffers || !glCheckFramebufferStatus ||
         !glFramebufferTexture2D || !glFramebufferRenderbuffer ||
         !glGetFramebufferAttachmentParameteriv ||
         !glRenderbufferStorageMultisample || !glGenerateMipmap ||!glFramebufferTextureLayer) &&
         isExtensionSupported("GL_EXT_framebuffer_object")) {

        glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) GL::getProcAddress("glIsRenderbufferEXT");
        glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) GL::getProcAddress("glBindRenderbufferEXT");
        glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) GL::getProcAddress("glDeleteRenderbuffersEXT");
        glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) GL::getProcAddress("glGenRenderbuffersEXT");
        glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) GL::getProcAddress("glRenderbufferStorageEXT");
        glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) GL::getProcAddress("glGetRenderbufferParameterivEXT");
        glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) GL::getProcAddress("glIsFramebufferEXT");
        glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) GL::getProcAddress("glBindFramebufferEXT");
        glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) GL::getProcAddress("glDeleteFramebuffersEXT");
        glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) GL::getProcAddress("glGenFramebuffersEXT");
        glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) GL::getProcAddress("glCheckFramebufferStatusEXT");
        glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) GL::getProcAddress("glFramebufferTexture1DEXT");
        glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) GL::getProcAddress("glFramebufferTexture2DEXT");
        glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) GL::getProcAddress("glFramebufferTexture3DEXT");
        glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) GL::getProcAddress("glFramebufferRenderbufferEXT");
        glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) GL::getProcAddress("glGetFramebufferAttachmentParameterivEXT");
        glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) GL::getProcAddress("glGenerateMipmapEXT");
    }

    if (!glBlitFramebuffer && isExtensionSupported("GL_EXT_framebuffer_blit")) {
        glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC) GL::getProcAddress("glBlitFramebufferEXT");
    }

    if (!glIsRenderbuffer || !glBindRenderbuffer || !glDeleteRenderbuffers ||
        !glGenRenderbuffers || !glRenderbufferStorage || !glGetRenderbufferParameteriv ||
        !glIsFramebuffer || !glBindFramebuffer || !glDeleteFramebuffers ||
        !glGenFramebuffers || !glCheckFramebufferStatus ||
        !glFramebufferTexture2D || !glFramebufferRenderbuffer ||
        !glGetFramebufferAttachmentParameteriv ||
        !glRenderbufferStorageMultisample || !glGenerateMipmap ||!glFramebufferTextureLayer) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL FBO"));
    }

    if (!glBlitFramebuffer) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL FBO blitting"));
    }
#endif // O3D_GL_VERSION_3_0

#ifdef O3D_GL_VERSION_3_1  // GL COMMON
    glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC) GL::getProcAddress("glDrawArraysInstanced");
    glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC) GL::getProcAddress("");
    glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC) GL::getProcAddress("glCopyBufferSubData");
    glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC) GL::getProcAddress("glGetUniformIndices");
    glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC) GL::getProcAddress("glGetActiveUniformsiv");
    glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC) GL::getProcAddress("glGetUniformBlockIndex");
    glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC) GL::getProcAddress("glGetActiveUniformBlockiv");
    glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC) GL::getProcAddress("glGetActiveUniformBlockName");
    glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC) GL::getProcAddress("glUniformBlockBinding");

    if (!glDrawArraysInstanced || !glDrawElementsInstanced) {
        O3D_WARNING("Unavailable OpenGL Draw array/element instanced");
    }

    if (!glCopyBufferSubData) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Copy buffer sub-data"));
    }

    if (!glGetUniformIndices || !glGetActiveUniformsiv ||
        !glGetUniformBlockIndex || !glGetActiveUniformBlockiv ||
        !glGetActiveUniformBlockName || !glUniformBlockBinding) {
        // O3D_ERROR(E_UnsuportedFeature("OpenGL Uniform Buffer Object"));
        O3D_WARNING("Unavailable OpenGL Uniform buffer object");
    }
#endif // O3D_GL_VERSION_3_1

#ifdef O3D_GL_VERSION_3_2  // GL COMMON
    glGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC) GL::getProcAddress("glGetInteger64i_v");
    glGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC) GL::getProcAddress("glGetBufferParameteri64v");

    // GL_ARB_texture_multisample
    glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC) GL::getProcAddress("glGetMultisamplefvARB");
    glSampleMaski = (PFNGLSAMPLEMASKIPROC) GL::getProcAddress("glSampleMaskiARB");

    if ((!glGetMultisamplefv || !glSampleMaski) && isExtensionSupported("GL_ARB_texture_multisample")) {
        glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC) GL::getProcAddress("glTexImage2DMultisampleARB");
        glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC) GL::getProcAddress("glTexImage3DMultisampleARB");
        glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC) GL::getProcAddress("glGetMultisamplefvARB");
        glSampleMaski = (PFNGLSAMPLEMASKIPROC) GL::getProcAddress("glSampleMaskiARB");
    }

    if (!glGetMultisamplefv || !glSampleMaski) {
        O3D_WARNING("OpenGL texture multisample fonctions is not available");
    }
#endif // O3D_GL_VERSION_3_2

#ifdef O3D_GL_VERSION_3_3  // GL COMMON
    glGenSamplers = (PFNGLGENSAMPLERSPROC) GL::getProcAddress("glGenSamplers");
    glDeleteSamplers = (PFNGLDELETESAMPLERSPROC) GL::getProcAddress("glDeleteSamplers");
    glIsSampler = (PFNGLISSAMPLERPROC) GL::getProcAddress("glIsSampler");
    glBindSampler = (PFNGLBINDSAMPLERPROC) GL::getProcAddress("glBindSampler");
    glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC) GL::getProcAddress("glSamplerParameteri");
    glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC) GL::getProcAddress("glSamplerParameteriv");
    glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC) GL::getProcAddress("glSamplerParameterf");
    glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC) GL::getProcAddress("glSamplerParameterfv");
    glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC) GL::getProcAddress("glGetSamplerParameteriv");
    glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC) GL::getProcAddress("glGetSamplerParameterfv");

    if (!glGenSamplers || !glDeleteSamplers || !glIsSampler || !glBindSampler ||
        !glSamplerParameteri || !glSamplerParameteriv ||
        !glSamplerParameterf || !glSamplerParameterfv ||
        !glGetSamplerParameteriv || !glGetSamplerParameterfv) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL sampler object is not available"));
    }
#endif // O3D_GL_VERSION_3_3

#ifdef O3D_GL_VERSION_4_1  // GL COMMON
    glDepthRangef = (PFNGLDEPTHRANGEFPROC)GL::getProcAddress("glDepthRangef");
    glClearDepthf = (PFNGLCLEARDEPTHFPROC)GL::getProcAddress("glClearDepthf");
    glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC) GL::getProcAddress("glProgramParameteri");

    if (!glProgramParameteri) {
        glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC) GL::getProcAddress("glProgramParameteriARB");
    }
#endif // O3D_GL_VERSION_4_1

#ifdef O3D_GL_VERSION_4_2  // GL COMMON
    glGetInternalformativ = (PFNGLGETINTERNALFORMATIVPROC) GL::getProcAddress("glGetInternalformativ");
    glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC) GL::getProcAddress("glBindImageTexture");
    glMemoryBarrier = (PFNGLMEMORYBARRIERPROC) GL::getProcAddress("glMemoryBarrier");
    glTexStorage2D = (PFNGLTEXSTORAGE2DPROC) GL::getProcAddress("glTexStorage2D");
    glTexStorage3D = (PFNGLTEXSTORAGE3DPROC) GL::getProcAddress("glTexStorage3D");

    if (!glGetInternalformativ) {
        O3D_WARNING("OpenGL GetInternalformativ is not available");
    }

    if (!glBindImageTexture) {
        O3D_WARNING("OpenGL BindImageTexture is not available");
    }

    if (!glMemoryBarrier) {
        O3D_WARNING("OpenGL Memory Barrier is not available");
    }

    if (!glTexStorage2D || !glTexStorage3D) {
        O3D_WARNING("OpenGL TexStorage<N>D is not available");
    }
#endif // O3D_GL_VERSION_4_2

#ifdef O3D_GL_VERSION_4_3  // GL COMMON
    glClearBufferData = (PFNGLCLEARBUFFERDATAPROC)GL::getProcAddress("glClearBufferData");
    glDispatchCompute = (PFNGLDISPATCHCOMPUTEPROC)GL::getProcAddress("glDispatchCompute");
    glDispatchComputeIndirect = (PFNGLDISPATCHCOMPUTEINDIRECTPROC)GL::getProcAddress("glDispatchComputeIndirect");
    glCopyImageSubData = (PFNGLCOPYIMAGESUBDATAPROC)GL::getProcAddress("glCopyImageSubData");
    glFramebufferParameteri = (PFNGLFRAMEBUFFERPARAMETERIPROC)GL::getProcAddress("glFramebufferParameteri");
    glGetFramebufferParameteriv = (PFNGLGETFRAMEBUFFERPARAMETERIVPROC)GL::getProcAddress("glGetFramebufferParameteriv");
    glInvalidateFramebuffer = (PFNGLINVALIDATEFRAMEBUFFERPROC)GL::getProcAddress("glInvalidateFramebuffer");
    glInvalidateSubFramebuffer = (PFNGLINVALIDATESUBFRAMEBUFFERPROC)GL::getProcAddress("glInvalidateSubFramebuffer");
    glGetProgramInterfaceiv = (PFNGLGETPROGRAMINTERFACEIVPROC)GL::getProcAddress("glGetProgramInterfaceiv");
    glGetProgramResourceIndex = (PFNGLGETPROGRAMRESOURCEINDEXPROC)GL::getProcAddress("glGetProgramResourceIndex");
    glGetProgramResourceName = (PFNGLGETPROGRAMRESOURCENAMEPROC)GL::getProcAddress("glGetProgramResourceName");
    glGetProgramResourceiv = (PFNGLGETPROGRAMRESOURCEIVPROC)GL::getProcAddress("glGetProgramResourceiv");
    glGetProgramResourceLocation = (PFNGLGETPROGRAMRESOURCELOCATIONPROC)GL::getProcAddress("glGetProgramResourceLocation");
    glTexBufferRange = (PFNGLTEXBUFFERRANGEPROC)GL::getProcAddress("glTexBufferRange");
    glTexStorage2DMultisample = (PFNGLTEXSTORAGE2DMULTISAMPLEPROC)GL::getProcAddress("glTexStorage2DMultisample");
    glTexStorage3DMultisample = (PFNGLTEXSTORAGE3DMULTISAMPLEPROC)GL::getProcAddress("glTexStorage3DMultisample");
    glBindVertexBuffer = (PFNGLBINDVERTEXBUFFERPROC)GL::getProcAddress("glBindVertexBuffer");
    glVertexAttribFormat = (PFNGLVERTEXATTRIBFORMATPROC)GL::getProcAddress("glVertexAttribFormat");
    glVertexAttribIFormat = (PFNGLVERTEXATTRIBIFORMATPROC)GL::getProcAddress("glVertexAttribIFormat");
    glVertexAttribBinding = (PFNGLVERTEXATTRIBBINDINGPROC)GL::getProcAddress("glVertexAttribBinding");
    glVertexBindingDivisor = (PFNGLVERTEXBINDINGDIVISORPROC)GL::getProcAddress("glVertexBindingDivisor");
    glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC)GL::getProcAddress("glDebugMessageControl");
    glDebugMessageInsert = (PFNGLDEBUGMESSAGEINSERTPROC)GL::getProcAddress("glDebugMessageInsert");
    glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)GL::getProcAddress("glDebugMessageCallback");
    glGetDebugMessageLog = (PFNGLGETDEBUGMESSAGELOGPROC)GL::getProcAddress("glGetDebugMessageLog");
    glPushDebugGroup = (PFNGLPUSHDEBUGGROUPPROC)GL::getProcAddress("glPushDebugGroup");
    glPopDebugGroup = (PFNGLPOPDEBUGGROUPPROC)GL::getProcAddress("glPopDebugGroup");
    glObjectLabel = (PFNGLOBJECTLABELPROC)GL::getProcAddress("glObjectLabel");
    glGetObjectLabel = (PFNGLGETOBJECTLABELPROC)GL::getProcAddress("glGetObjectLabel");
    glObjectPtrLabel = (PFNGLOBJECTPTRLABELPROC)GL::getProcAddress("glObjectPtrLabel");
    glGetObjectPtrLabel = (PFNGLGETOBJECTPTRLABELPROC)GL::getProcAddress("glGetObjectPtrLabel");
#endif // O3D_GL_VERSION_4_3
}

// Get extensions pointers
void GLExtensionManager::getGLFunctions()
{
    REQUIRED_EXT(ARB_framebuffer_object)
    REQUIRED_EXT(ARB_texture_float)
    // REQUIRED_EXT(ARB_texture_half_float)
    // REQUIRED_EXT(ARB_texture_float_linear)
    // REQUIRED_EXT(ARB_texture_half_float_linear)

#ifdef O3D_GL_VERSION_1_0  // GL
    glPointSize = (PFNGLPOINTSIZEPROC) GL::getProcAddress("glPointSize");
    glPolygonMode = (PFNGLPOLYGONMODEPROC) GL::getProcAddress("glPolygonMode");
    glLogicOp = (PFNGLLOGICOPPROC) GL::getProcAddress("glLogicOp");
    glPixelStoref = (PFNGLPIXELSTOREFPROC) GL::getProcAddress("glPixelStoref");
    glGetDoublev = (PFNGLGETDOUBLEVPROC) GL::getProcAddress("glGetDoublev");
    glGetTexImage = (PFNGLGETTEXIMAGEPROC) GL::getProcAddress("glGetTexImage");
    glGetTexLevelParameterfv = (PFNGLGETTEXLEVELPARAMETERFVPROC) GL::getProcAddress("glGetTexLevelParameterfv");
    glGetTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIVPROC) GL::getProcAddress("glGetTexLevelParameteriv");
#endif

#ifdef O3D_GL_VERSION_1_1  // GL
    glGetPointerv = (PFNGLGETPOINTERVPROC) GL::getProcAddress("glGetPointerv");
    glPolygonOffset = (PFNGLPOLYGONOFFSETPROC) GL::getProcAddress("glPolygonOffset");
    glCopyTexImage1D = (PFNGLCOPYTEXIMAGE1DPROC) GL::getProcAddress("glCopyTexImage1D");
    glCopyTexSubImage1D = (PFNGLCOPYTEXSUBIMAGE1DPROC) GL::getProcAddress("glCopyTexSubImage1D");
    glTexSubImage1D = (PFNGLTEXSUBIMAGE1DPROC) GL::getProcAddress("glTexSubImage1D");
#endif

#ifdef O3D_GL_VERSION_1_3  // GL
    glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC) GL::getProcAddress("glCompressedTexImage1D");
    glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) GL::getProcAddress("glCompressedTexSubImage1D");
    glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC) GL::getProcAddress("glGetCompressedTexImage");

    if ((!glCompressedTexImage1D || !glCompressedTexSubImage1D ||
         !glGetCompressedTexImage) && isExtensionSupported("GL_ARB_texture_compression")) {

        glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC) GL::getProcAddress("glCompressedTexImage1DARB");
        glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) GL::getProcAddress("glCompressedTexSubImage1DARB");
        glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC) GL::getProcAddress("glGetCompressedTexImageARB");
    }

    if (!glCompressedTexImage1D) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL compressed texture 1D"));
    }

    if (!glGetCompressedTexImage) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL compressed texture get image"));
    }
#endif // O3D_GL_VERSION_1_3

#ifdef O3D_GL_VERSION_1_4  // GL
    glPointParameterf = (PFNGLPOINTPARAMETERFPROC) GL::getProcAddress("glPointParameterf");
    glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC) GL::getProcAddress("glPointParameterfv");
    glPointParameteri = (PFNGLPOINTPARAMETERIPROC) GL::getProcAddress("glPointParameteri");
    glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC) GL::getProcAddress("glPointParameteriv");

    if ((!glPointParameterf || !glPointParameterfv || !glPointParameteri || !glPointParameteriv) &&
        isExtensionSupported("GL_ARB_point_parameters")) {

        glPointParameterf = (PFNGLPOINTPARAMETERFPROC) GL::getProcAddress("glPointParameterfARB");
        glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC) GL::getProcAddress("glPointParameterfvARB");
        glPointParameteri = (PFNGLPOINTPARAMETERIPROC) GL::getProcAddress("glPointParameteriARB");
        glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC) GL::getProcAddress("glPointParameterivARB");
    }

    if (!glPointParameterf || !glPointParameterfv || !glPointParameteri || !glPointParameteriv) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL point parameter"));
    }

    glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC) GL::getProcAddress("glMultiDrawArrays");
    glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC) GL::getProcAddress("glMultiDrawElements");

    if (!glMultiDrawArrays || !glMultiDrawElements) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL multi draw"));
    }
#endif // O3D_GL_VERSION_1_4

#ifdef O3D_GL_VERSION_1_5  // GL
    glMapBuffer = (PFNGLMAPBUFFERPROC) GL::getProcAddress("glMapBuffer");

    if ((!glMapBuffer) && isExtensionSupported("GL_ARB_vertex_buffer_object")) {
        glMapBuffer = (PFNGLMAPBUFFERPROC) GL::getProcAddress("glMapBufferARB");
    }

    if (!glMapBuffer) {
        O3D_WARNING("OpenGL glMapBuffer is not available");
    }

    // OcclusionQuery (optional)
    glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)GL::getProcAddress("glGetQueryObjectiv");

    if ((!glGetQueryObjectiv) && isExtensionSupported("GL_ARB_occlusion_query")) {
        glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)GL::getProcAddress("glGetQueryObjectivARB");
    }
    if (!glGetQueryObjectiv) {
        O3D_WARNING("OpenGL occlusion query is not available");
    }
#endif // O3D_GL_VERSION_1_5

#ifdef O3D_GL_VERSION_2_0  // GL
    glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC) GL::getProcAddress("glGetVertexAttribdv");

    glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC) GL::getProcAddress("glVertexAttrib1d");
    glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC) GL::getProcAddress("glVertexAttrib1dv");
    glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC) GL::getProcAddress("glVertexAttrib1s");
    glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC) GL::getProcAddress("glVertexAttrib1sv");
    glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC) GL::getProcAddress("glVertexAttrib2d");
    glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC) GL::getProcAddress("glVertexAttrib2dv");
    glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC) GL::getProcAddress("glVertexAttrib2s");
    glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC) GL::getProcAddress("glVertexAttrib2sv");
    glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC) GL::getProcAddress("glVertexAttrib3d");
    glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC) GL::getProcAddress("glVertexAttrib3dv");
    glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC) GL::getProcAddress("glVertexAttrib3s");
    glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC) GL::getProcAddress("glVertexAttrib3sv");
    glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC) GL::getProcAddress("glVertexAttrib4Nbv");
    glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC) GL::getProcAddress("glVertexAttrib4Niv");
    glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC) GL::getProcAddress("glVertexAttrib4Nsv");
    glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC) GL::getProcAddress("glVertexAttrib4Nub");
    glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC) GL::getProcAddress("glVertexAttrib4Nubv");
    glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC) GL::getProcAddress("glVertexAttrib4Nuiv");
    glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC) GL::getProcAddress("glVertexAttrib4Nusv");
    glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC) GL::getProcAddress("glVertexAttrib4bv");
    glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC) GL::getProcAddress("glVertexAttrib4d");
    glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC) GL::getProcAddress("glVertexAttrib4dv");
    glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC) GL::getProcAddress("glVertexAttrib4iv");
    glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC) GL::getProcAddress("glVertexAttrib4s");
    glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC) GL::getProcAddress("glVertexAttrib4sv");
    glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC) GL::getProcAddress("glVertexAttrib4ubv");
    glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC) GL::getProcAddress("glVertexAttrib4uiv");
    glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC) GL::getProcAddress("glVertexAttrib4usv");

    if ((!glGetVertexAttribdv || !glVertexAttrib1d || !glVertexAttrib1dv  ||
         !glVertexAttrib1s || !glVertexAttrib1sv  || !glVertexAttrib2d || !glVertexAttrib2dv ||
         !glVertexAttrib2s || !glVertexAttrib2sv || !glVertexAttrib3d || !glVertexAttrib3dv  ||
         !glVertexAttrib3s || !glVertexAttrib3sv  ||
         !glVertexAttrib4Nbv || !glVertexAttrib4Niv || !glVertexAttrib4Nsv || !glVertexAttrib4Nub  ||
         !glVertexAttrib4Nubv || !glVertexAttrib4Nuiv || !glVertexAttrib4Nusv || !glVertexAttrib4bv  ||
         !glVertexAttrib4d || !glVertexAttrib4dv ||
         !glVertexAttrib4iv || !glVertexAttrib4s || !glVertexAttrib4sv || !glVertexAttrib4ubv  ||
         !glVertexAttrib4uiv || !glVertexAttrib4usv) &&
         isExtensionSupported("GL_ARB_shading_language_100")) {

        glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC) GL::getProcAddress("glGetVertexAttribdvARB");

        glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC) GL::getProcAddress("glVertexAttrib1dARB");
        glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC) GL::getProcAddress("glVertexAttrib1dvARB");
        glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC) GL::getProcAddress("glVertexAttrib1sARB");
        glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC) GL::getProcAddress("glVertexAttrib1svARB");
        glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC) GL::getProcAddress("glVertexAttrib2dARB");
        glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC) GL::getProcAddress("glVertexAttrib2dvARB");
        glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC) GL::getProcAddress("glVertexAttrib2sARB");
        glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC) GL::getProcAddress("glVertexAttrib2svARB");
        glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC) GL::getProcAddress("glVertexAttrib3dARB");
        glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC) GL::getProcAddress("glVertexAttrib3dvARB");
        glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC) GL::getProcAddress("glVertexAttrib3sARB");
        glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC) GL::getProcAddress("glVertexAttrib3svARB");
        glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC) GL::getProcAddress("glVertexAttrib4NbvARB");
        glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC) GL::getProcAddress("glVertexAttrib4NivARB");
        glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC) GL::getProcAddress("glVertexAttrib4NsvARB");
        glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC) GL::getProcAddress("glVertexAttrib4NubARB");
        glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC) GL::getProcAddress("glVertexAttrib4NubvARB");
        glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC) GL::getProcAddress("glVertexAttrib4NuivARB");
        glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC) GL::getProcAddress("glVertexAttrib4NusvARB");
        glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC) GL::getProcAddress("glVertexAttrib4bvARB");
        glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC) GL::getProcAddress("glVertexAttrib4dARB");
        glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC) GL::getProcAddress("glVertexAttrib4dvARB");
        glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC) GL::getProcAddress("glVertexAttrib4ivARB");
        glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC) GL::getProcAddress("glVertexAttrib4sARB");
        glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC) GL::getProcAddress("glVertexAttrib4svARB");
        glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC) GL::getProcAddress("glVertexAttrib4ubvARB");
        glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC) GL::getProcAddress("glVertexAttrib4uivARB");
        glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC) GL::getProcAddress("glVertexAttrib4usvARB");
    }

    if (!glGetVertexAttribdv || !glVertexAttrib1d || !glVertexAttrib1dv  ||
        !glVertexAttrib1s || !glVertexAttrib1sv  || !glVertexAttrib2d || !glVertexAttrib2dv ||
        !glVertexAttrib2s || !glVertexAttrib2sv || !glVertexAttrib3d || !glVertexAttrib3dv  ||
        !glVertexAttrib3s || !glVertexAttrib3sv  ||
        !glVertexAttrib4Nbv || !glVertexAttrib4Niv || !glVertexAttrib4Nsv || !glVertexAttrib4Nub  ||
        !glVertexAttrib4Nubv || !glVertexAttrib4Nuiv || !glVertexAttrib4Nusv || !glVertexAttrib4bv  ||
        !glVertexAttrib4d || !glVertexAttrib4dv ||
        !glVertexAttrib4iv || !glVertexAttrib4s || !glVertexAttrib4sv || !glVertexAttrib4ubv  ||
        !glVertexAttrib4uiv || !glVertexAttrib4usv) {

        O3D_ERROR(E_UnsuportedFeature("OpenGL 2.0 GLSL"));
    }
#endif // O3D_GL_VERSION_2_0

#ifdef O3D_GL_VERSION_3_0  // GL
    glColorMaski = (PFNGLCOLORMASKIPROC) GL::getProcAddress("glColorMaski");
    glEnablei = (PFNGLENABLEIPROC) GL::getProcAddress("glEnablei");
    glDisablei = (PFNGLDISABLEIPROC) GL::getProcAddress("glDisablei");
    glIsEnabledi = (PFNGLISENABLEDIPROC) GL::getProcAddress("glIsEnabledi");
    glClampColor = (PFNGLCLAMPCOLORPROC) GL::getProcAddress("glClampColor");
    glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC) GL::getProcAddress("glBeginConditionalRender");
    glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC) GL::getProcAddress("glEndConditionalRender");
    glVertexAttribI1i = (PFNGLVERTEXATTRIBI1IPROC) GL::getProcAddress("glVertexAttribI1i");
    glVertexAttribI2i = (PFNGLVERTEXATTRIBI2IPROC) GL::getProcAddress("glVertexAttribI2i");
    glVertexAttribI3i = (PFNGLVERTEXATTRIBI3IPROC) GL::getProcAddress("glVertexAttribI3i");
    glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC) GL::getProcAddress("glVertexAttribI4i");
    glVertexAttribI1ui = (PFNGLVERTEXATTRIBI1UIPROC) GL::getProcAddress("glVertexAttribI1ui");
    glVertexAttribI2ui = (PFNGLVERTEXATTRIBI2UIPROC) GL::getProcAddress("glVertexAttribI2ui");
    glVertexAttribI3ui = (PFNGLVERTEXATTRIBI3UIPROC) GL::getProcAddress("glVertexAttribI3ui");
    glVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC) GL::getProcAddress("glVertexAttribI4ui");
    glVertexAttribI1iv = (PFNGLVERTEXATTRIBI1IVPROC) GL::getProcAddress("glVertexAttribI1iv");
    glVertexAttribI2iv = (PFNGLVERTEXATTRIBI2IVPROC) GL::getProcAddress("glVertexAttribI2iv");
    glVertexAttribI3iv = (PFNGLVERTEXATTRIBI3IVPROC) GL::getProcAddress("glVertexAttribI3iv");
    glVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC) GL::getProcAddress("glVertexAttribI4iv");
    glVertexAttribI1uiv = (PFNGLVERTEXATTRIBI1UIVPROC) GL::getProcAddress("glVertexAttribI1uiv");
    glVertexAttribI2uiv = (PFNGLVERTEXATTRIBI2UIVPROC) GL::getProcAddress("glVertexAttribI2uiv");
    glVertexAttribI3uiv = (PFNGLVERTEXATTRIBI3UIVPROC) GL::getProcAddress("glVertexAttribI3uiv");
    glVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC) GL::getProcAddress("glVertexAttribI4uiv");
    glVertexAttribI4bv = (PFNGLVERTEXATTRIBI4BVPROC) GL::getProcAddress("glVertexAttribI4bv");
    glVertexAttribI4sv = (PFNGLVERTEXATTRIBI4SVPROC) GL::getProcAddress("glVertexAttribI4sv");
    glVertexAttribI4ubv = (PFNGLVERTEXATTRIBI4UBVPROC) GL::getProcAddress("glVertexAttribI4ubv");
    glVertexAttribI4usv = (PFNGLVERTEXATTRIBI4USVPROC) GL::getProcAddress("glVertexAttribI4usv");
    glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC) GL::getProcAddress("glBindFragDataLocation");
    glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC) GL::getProcAddress("glTexParameterIiv");
    glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC) GL::getProcAddress("glTexParameterIuiv");
    glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC) GL::getProcAddress("glGetTexParameterIiv");
    glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC) GL::getProcAddress("glGetTexParameterIuiv");

    if (!glColorMaski ||
        !glEnablei || !glDisablei || !glIsEnabledi ||
        !glClampColor ||
        !glBeginConditionalRender || !glEndConditionalRender ||
        !glVertexAttribI1i || !glVertexAttribI2i || !glVertexAttribI3i || !glVertexAttribI4i ||
        !glVertexAttribI1ui || !glVertexAttribI2ui || !glVertexAttribI3ui || !glVertexAttribI4ui ||
        !glVertexAttribI1iv || !glVertexAttribI2iv || !glVertexAttribI3iv || !glVertexAttribI4iv ||
        !glVertexAttribI1uiv || !glVertexAttribI2uiv || !glVertexAttribI3uiv || !glVertexAttribI4uiv ||
        !glVertexAttribI4bv || !glVertexAttribI4sv || !glVertexAttribI4ubv || !glVertexAttribI4usv ||
        !glBindFragDataLocation ||
        !glUniform1ui || !glUniform2ui || !glUniform3ui || !glUniform4ui ||
        !glUniform1uiv || !glUniform2uiv || !glUniform3uiv || !glUniform4uiv ||
        !glTexParameterIiv || !glTexParameterIuiv ||
        !glGetTexParameterIiv || !glGetTexParameterIuiv) {

        O3D_ERROR(E_UnsuportedFeature("OpenGL 3.0 related functions"));
	}

    // GL_ARB_framebuffer_object (case for 1D and 3D)
    glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC) GL::getProcAddress("glFramebufferTexture1D");
    glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) GL::getProcAddress("glFramebufferTexture3D");

    if ((!glFramebufferTexture1D || !glFramebufferTexture3D) &&
        isExtensionSupported("GL_ARB_framebuffer_object")) {
        glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC) GL::getProcAddress("glFramebufferTexture1DARB");
        glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) GL::getProcAddress("glFramebufferTexture3DARB");
    }

    if ((!glFramebufferTexture1D || !glFramebufferTexture3D) &&
        isExtensionSupported("GL_EXT_framebuffer_object")) {
        glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC) GL::getProcAddress("glFramebufferTexture1DEXT");
        glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) GL::getProcAddress("glFramebufferTexture3DEXT");
    }
#endif // O3D_GL_VERSION_3_0

#ifdef O3D_GL_VERSION_3_1  // GL
    glTexBuffer = (PFNGLTEXBUFFERPROC) GL::getProcAddress("glTexBuffer");
    glPrimitiveRestartIndex = (PFNGLPRIMITIVERESTARTINDEXPROC) GL::getProcAddress("glPrimitiveRestartIndex");
    glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC) GL::getProcAddress("glGetActiveUniformName");

    if (!glTexBuffer) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Tex buffer"));
    }

    if (!glPrimitiveRestartIndex) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Primitive restart index"));
    }

    if (!glGetActiveUniformName) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Get Active Uniform Name"));
    }
#endif // O3D_GL_VERSION_3_1

#ifdef O3D_GL_VERSION_3_2  // GL
    glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC) GL::getProcAddress("glFramebufferTexture");

    if (!glFramebufferTexture) {
        glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC) GL::getProcAddress("glFramebufferTextureARB");
    }

    if (!glFramebufferTexture) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL FramebufferTexture function"));
    }

    // GL_ARB_texture_multisample (TexImage{2,3}DMultisample functions)
    glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC) GL::getProcAddress("glTexImage2DMultisample");
    glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC) GL::getProcAddress("glTexImage3DMultisample");

    if ((!glTexImage2DMultisample || !glTexImage3DMultisample ||
         !glGetMultisamplefv || !glSampleMaski) && isExtensionSupported("GL_ARB_texture_multisample")) {

        glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC) GL::getProcAddress("glTexImage2DMultisampleARB");
        glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC) GL::getProcAddress("glTexImage3DMultisampleARB");
        glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC) GL::getProcAddress("glGetMultisamplefvARB");
        glSampleMaski = (PFNGLSAMPLEMASKIPROC) GL::getProcAddress("glSampleMaskiARB");
    }

    if (!glTexImage2DMultisample || !glTexImage3DMultisample || !glGetMultisamplefv || !glSampleMaski) {
        O3D_WARNING("OpenGL texture multisample fonctions is not available");
    }
#endif // O3D_GL_VERSION_3_2

#ifdef O3D_GL_VERSION_3_3  // GL
    glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC) GL::getProcAddress("glSamplerParameterIiv");
    glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC) GL::getProcAddress("glSamplerParameterIuiv");
    glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC) GL::getProcAddress("glGetSamplerParameterIiv");
    glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC) GL::getProcAddress("glGetSamplerParameterIuiv");

    if (!glSamplerParameterIiv || !glSamplerParameterIuiv ||
        !glGetSamplerParameterIiv || !glGetSamplerParameterIuiv) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL sampler object is not available"));
    }
#endif // O3D_GL_VERSION_3_3

#ifdef O3D_GL_VERSION_4_0  // GL
    glBlendEquationi = (PFNGLBLENDEQUATIONIPROC) GL::getProcAddress("glBlendEquationi");
    glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC) GL::getProcAddress("glBlendEquationSeparatei");
    glBlendFunci = (PFNGLBLENDFUNCIPROC) GL::getProcAddress("glBlendFunci");
    glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC) GL::getProcAddress("glBlendFuncSeparatei");

    if (!glBlendEquationi || !glBlendEquationSeparatei || !glBlendFunci || !glBlendFuncSeparatei) {
        O3D_WARNING("OpenGL Blend index/separate is not available");
    }
#endif // O3D_GL_VERSION_4_0

#ifdef O3D_GL_VERSION_4_1  // GL
    glReleaseShaderCompiler = (PFNGLRELEASESHADERCOMPILERPROC)GL::getProcAddress("glReleaseShaderCompiler");
    glShaderBinary = (PFNGLSHADERBINARYPROC)GL::getProcAddress("glShaderBinary");
    glGetShaderPrecisionFormat = (PFNGLGETSHADERPRECISIONFORMATPROC)GL::getProcAddress("glGetShaderPrecisionFormat");
    glDepthRangef = (PFNGLDEPTHRANGEFPROC)GL::getProcAddress("glDepthRangef");
    glClearDepthf = (PFNGLCLEARDEPTHFPROC)GL::getProcAddress("glClearDepthf");
    glGetProgramBinary = (PFNGLGETPROGRAMBINARYPROC)GL::getProcAddress("glGetProgramBinary");
    glProgramBinary = (PFNGLPROGRAMBINARYPROC)GL::getProcAddress("glProgramBinary");
    glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)GL::getProcAddress("glProgramParameteri");
    glUseProgramStages = (PFNGLUSEPROGRAMSTAGESPROC)GL::getProcAddress("glUseProgramStages");
    glActiveShaderProgram = (PFNGLACTIVESHADERPROGRAMPROC)GL::getProcAddress("glActiveShaderProgram");
    glCreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC)GL::getProcAddress("glCreateShaderProgramv");
    glBindProgramPipeline = (PFNGLBINDPROGRAMPIPELINEPROC)GL::getProcAddress("glBindProgramPipeline");
    glDeleteProgramPipelines = (PFNGLDELETEPROGRAMPIPELINESPROC)GL::getProcAddress("glDeleteProgramPipelines");
    glGenProgramPipelines = (PFNGLGENPROGRAMPIPELINESPROC)GL::getProcAddress("glGenProgramPipelines");
    glIsProgramPipeline = (PFNGLISPROGRAMPIPELINEPROC)GL::getProcAddress("glIsProgramPipeline");
    glGetProgramPipelineiv = (PFNGLGETPROGRAMPIPELINEIVPROC)GL::getProcAddress("glGetProgramPipelineiv");
    glProgramUniform1i = (PFNGLPROGRAMUNIFORM1IPROC)GL::getProcAddress("glProgramUniform1i");
    glProgramUniform1iv = (PFNGLPROGRAMUNIFORM1IVPROC)GL::getProcAddress("glProgramUniform1iv");
    glProgramUniform1f = (PFNGLPROGRAMUNIFORM1FPROC)GL::getProcAddress("glProgramUniform1f");
    glProgramUniform1fv = (PFNGLPROGRAMUNIFORM1FVPROC)GL::getProcAddress("glProgramUniform1fv");
    glProgramUniform1d = (PFNGLPROGRAMUNIFORM1DPROC)GL::getProcAddress("glProgramUniform1d");
    glProgramUniform1dv = (PFNGLPROGRAMUNIFORM1DVPROC)GL::getProcAddress("glProgramUniform1dv");
    glProgramUniform1ui = (PFNGLPROGRAMUNIFORM1UIPROC)GL::getProcAddress("glProgramUniform1ui");
    glProgramUniform1uiv = (PFNGLPROGRAMUNIFORM1UIVPROC)GL::getProcAddress("glProgramUniform1uiv");
    glProgramUniform2i = (PFNGLPROGRAMUNIFORM2IPROC)GL::getProcAddress("glProgramUniform2i");
    glProgramUniform2iv = (PFNGLPROGRAMUNIFORM2IVPROC)GL::getProcAddress("glProgramUniform2iv");
    glProgramUniform2f = (PFNGLPROGRAMUNIFORM2FPROC)GL::getProcAddress("glProgramUniform2f");
    glProgramUniform2fv = (PFNGLPROGRAMUNIFORM2FVPROC)GL::getProcAddress("glProgramUniform2fv");
    glProgramUniform2d = (PFNGLPROGRAMUNIFORM2DPROC)GL::getProcAddress("glProgramUniform2d");
    glProgramUniform2dv = (PFNGLPROGRAMUNIFORM2DVPROC)GL::getProcAddress("glProgramUniform2dv");
    glProgramUniform2ui = (PFNGLPROGRAMUNIFORM2UIPROC)GL::getProcAddress("glProgramUniform2ui");
    glProgramUniform2uiv = (PFNGLPROGRAMUNIFORM2UIVPROC)GL::getProcAddress("glProgramUniform2uiv");
    glProgramUniform3i = (PFNGLPROGRAMUNIFORM3IPROC)GL::getProcAddress("glProgramUniform3i");
    glProgramUniform3iv = (PFNGLPROGRAMUNIFORM3IVPROC)GL::getProcAddress("glProgramUniform3iv");
    glProgramUniform3f = (PFNGLPROGRAMUNIFORM3FPROC)GL::getProcAddress("glProgramUniform3f");
    glProgramUniform3fv = (PFNGLPROGRAMUNIFORM3FVPROC)GL::getProcAddress("glProgramUniform3fv");
    glProgramUniform3d = (PFNGLPROGRAMUNIFORM3DPROC)GL::getProcAddress("glProgramUniform3d");
    glProgramUniform3dv = (PFNGLPROGRAMUNIFORM3DVPROC)GL::getProcAddress("glProgramUniform3dv");
    glProgramUniform3ui = (PFNGLPROGRAMUNIFORM3UIPROC)GL::getProcAddress("glProgramUniform3ui");
    glProgramUniform3uiv = (PFNGLPROGRAMUNIFORM3UIVPROC)GL::getProcAddress("glProgramUniform3uiv");
    glProgramUniform4i = (PFNGLPROGRAMUNIFORM4IPROC)GL::getProcAddress("glProgramUniform4i");
    glProgramUniform4iv = (PFNGLPROGRAMUNIFORM4IVPROC)GL::getProcAddress("glProgramUniform4iv");
    glProgramUniform4f = (PFNGLPROGRAMUNIFORM4FPROC)GL::getProcAddress("glProgramUniform4f");
    glProgramUniform4fv = (PFNGLPROGRAMUNIFORM4FVPROC)GL::getProcAddress("glProgramUniform4fv");
    glProgramUniform4d = (PFNGLPROGRAMUNIFORM4DPROC)GL::getProcAddress("glProgramUniform4d");
    glProgramUniform4dv = (PFNGLPROGRAMUNIFORM4DVPROC)GL::getProcAddress("glProgramUniform4dv");
    glProgramUniform4ui = (PFNGLPROGRAMUNIFORM4UIPROC)GL::getProcAddress("glProgramUniform4ui");
    glProgramUniform4uiv = (PFNGLPROGRAMUNIFORM4UIVPROC)GL::getProcAddress("glProgramUniform4uiv");
    glProgramUniformMatrix2fv = (PFNGLPROGRAMUNIFORMMATRIX2FVPROC)GL::getProcAddress("glProgramUniformMatrix2fv");
    glProgramUniformMatrix3fv = (PFNGLPROGRAMUNIFORMMATRIX3FVPROC)GL::getProcAddress("glProgramUniformMatrix3fv");
    glProgramUniformMatrix4fv = (PFNGLPROGRAMUNIFORMMATRIX4FVPROC)GL::getProcAddress("glProgramUniformMatrix4fv");
    glProgramUniformMatrix2dv = (PFNGLPROGRAMUNIFORMMATRIX2DVPROC)GL::getProcAddress("glProgramUniformMatrix2dv");
    glProgramUniformMatrix3dv = (PFNGLPROGRAMUNIFORMMATRIX3DVPROC)GL::getProcAddress("glProgramUniformMatrix3dv");
    glProgramUniformMatrix4dv = (PFNGLPROGRAMUNIFORMMATRIX4DVPROC)GL::getProcAddress("glProgramUniformMatrix4dv");
    glProgramUniformMatrix2x3fv = (PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC)GL::getProcAddress("glProgramUniformMatrix2x3fv");
    glProgramUniformMatrix3x2fv = (PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC)GL::getProcAddress("glProgramUniformMatrix3x2fv");
    glProgramUniformMatrix2x4fv = (PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC)GL::getProcAddress("glProgramUniformMatrix2x4fv");
    glProgramUniformMatrix4x2fv = (PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC)GL::getProcAddress("glProgramUniformMatrix4x2fv");
    glProgramUniformMatrix3x4fv = (PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC)GL::getProcAddress("glProgramUniformMatrix3x4fv");
    glProgramUniformMatrix4x3fv = (PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC)GL::getProcAddress("glProgramUniformMatrix4x3fv");
    glProgramUniformMatrix2x3dv = (PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC)GL::getProcAddress("glProgramUniformMatrix2x3dv");
    glProgramUniformMatrix3x2dv = (PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC)GL::getProcAddress("glProgramUniformMatrix3x2dv");
    glProgramUniformMatrix2x4dv = (PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC)GL::getProcAddress("glProgramUniformMatrix2x4dv");
    glProgramUniformMatrix4x2dv = (PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC)GL::getProcAddress("glProgramUniformMatrix4x2dv");
    glProgramUniformMatrix3x4dv = (PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC)GL::getProcAddress("glProgramUniformMatrix3x4dv");
    glProgramUniformMatrix4x3dv = (PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC)GL::getProcAddress("glProgramUniformMatrix4x3dv");
    glValidateProgramPipeline = (PFNGLVALIDATEPROGRAMPIPELINEPROC)GL::getProcAddress("glValidateProgramPipeline");
    glGetProgramPipelineInfoLog = (PFNGLGETPROGRAMPIPELINEINFOLOGPROC)GL::getProcAddress("glGetProgramPipelineInfoLog");
    glVertexAttribL1d = (PFNGLVERTEXATTRIBL1DPROC)GL::getProcAddress("glVertexAttribL1d");
    glVertexAttribL2d = (PFNGLVERTEXATTRIBL2DPROC)GL::getProcAddress("glVertexAttribL2d");
    glVertexAttribL3d = (PFNGLVERTEXATTRIBL3DPROC)GL::getProcAddress("glVertexAttribL3d");
    glVertexAttribL4d = (PFNGLVERTEXATTRIBL4DPROC)GL::getProcAddress("glVertexAttribL4d");
    glVertexAttribL1dv = (PFNGLVERTEXATTRIBL1DVPROC)GL::getProcAddress("glVertexAttribL1dv");
    glVertexAttribL2dv = (PFNGLVERTEXATTRIBL2DVPROC)GL::getProcAddress("glVertexAttribL2dv");
    glVertexAttribL3dv = (PFNGLVERTEXATTRIBL3DVPROC)GL::getProcAddress("glVertexAttribL3dv");
    glVertexAttribL4dv = (PFNGLVERTEXATTRIBL4DVPROC)GL::getProcAddress("glVertexAttribL4dv");
    glVertexAttribLPointer = (PFNGLVERTEXATTRIBLPOINTERPROC)GL::getProcAddress("glVertexAttribLPointer");
    glGetVertexAttribLdv = (PFNGLGETVERTEXATTRIBLDVPROC)GL::getProcAddress("glGetVertexAttribLdv");
    glViewportArrayv = (PFNGLVIEWPORTARRAYVPROC)GL::getProcAddress("glViewportArrayv");
    glViewportIndexedf = (PFNGLVIEWPORTINDEXEDFPROC)GL::getProcAddress("glViewportIndexedf");
    glViewportIndexedfv = (PFNGLVIEWPORTINDEXEDFVPROC)GL::getProcAddress("glViewportIndexedfv");
    glScissorArrayv = (PFNGLSCISSORARRAYVPROC)GL::getProcAddress("glScissorArrayv");
    glScissorIndexed = (PFNGLSCISSORINDEXEDPROC)GL::getProcAddress("glScissorIndexed");
    glScissorIndexedv = (PFNGLSCISSORINDEXEDVPROC)GL::getProcAddress("glScissorIndexedv");
    glDepthRangeArrayv = (PFNGLDEPTHRANGEARRAYVPROC)GL::getProcAddress("glDepthRangeArrayv");
    glDepthRangeIndexed = (PFNGLDEPTHRANGEINDEXEDPROC)GL::getProcAddress("glDepthRangeIndexed");
    glGetFloati_v = (PFNGLGETFLOATI_VPROC)GL::getProcAddress("glGetFloati_v");
    glGetDoublei_v = (PFNGLGETDOUBLEI_VPROC)GL::getProcAddress("glGetDoublei_v");
#endif // O3D_GL_VERSION_4_1

#ifdef O3D_GL_VERSION_4_2  // GL
    glDrawArraysInstancedBaseInstance = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC) GL::getProcAddress("glDrawArraysInstancedBaseInstance");
    glDrawElementsInstancedBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC) GL::getProcAddress("glDrawElementsInstancedBaseInstance");
    glDrawElementsInstancedBaseVertexBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC) GL::getProcAddress("glDrawElementsInstancedBaseVertexBaseInstance");
    glGetActiveAtomicCounterBufferiv = (PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC) GL::getProcAddress("glGetActiveAtomicCounterBufferiv");
    glTexStorage1D = (PFNGLTEXSTORAGE1DPROC) GL::getProcAddress("glTexStorage1D");
    glDrawTransformFeedbackInstanced = (PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC) GL::getProcAddress("glDrawTransformFeedbackInstanced");
    glDrawTransformFeedbackStreamInstanced = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC) GL::getProcAddress("glDrawTransformFeedbackStreamInstanced");

    if (!glDrawArraysInstancedBaseInstance || !glDrawElementsInstancedBaseInstance ||
        !glDrawElementsInstancedBaseVertexBaseInstance || !glDrawTransformFeedbackInstanced ||
        !glDrawTransformFeedbackStreamInstanced) {
        O3D_WARNING("Unavailable OpenGL Draw array/element instanced");
    }

    if (!glGetActiveAtomicCounterBufferiv) {
        O3D_WARNING("OpenGL GetActiveAtomicCounterBufferiv is not available");
    }

    if (!glTexStorage1D) {
        O3D_WARNING("OpenGL TexStorage1D is not available");
    }
#endif // O3D_GL_VERSION_4_2

#ifdef O3D_GL_VERSION_4_3  // GL
    glClearBufferSubData = (PFNGLCLEARBUFFERSUBDATAPROC)GL::getProcAddress("glClearBufferSubData");
    glGetFramebufferParameteriv = (PFNGLGETFRAMEBUFFERPARAMETERIVPROC)GL::getProcAddress("glGetFramebufferParameteriv");
    glGetInternalformati64v = (PFNGLGETINTERNALFORMATI64VPROC)GL::getProcAddress("glGetInternalformati64v");
    glInvalidateTexSubImage = (PFNGLINVALIDATETEXSUBIMAGEPROC)GL::getProcAddress("glInvalidateTexSubImage");
    glInvalidateTexImage = (PFNGLINVALIDATETEXIMAGEPROC)GL::getProcAddress("glInvalidateTexImage");
    glInvalidateBufferSubData = (PFNGLINVALIDATEBUFFERSUBDATAPROC)GL::getProcAddress("glInvalidateBufferSubData");
    glInvalidateBufferData = (PFNGLINVALIDATEBUFFERDATAPROC)GL::getProcAddress("glInvalidateBufferData");
    glMultiDrawArraysIndirect = (PFNGLMULTIDRAWARRAYSINDIRECTPROC)GL::getProcAddress("glMultiDrawArraysIndirect");
    glMultiDrawElementsIndirect = (PFNGLMULTIDRAWELEMENTSINDIRECTPROC)GL::getProcAddress("glMultiDrawElementsIndirect");
    glGetProgramResourceLocationIndex = (PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC)GL::getProcAddress("glGetProgramResourceLocationIndex");
    glShaderStorageBlockBinding = (PFNGLSHADERSTORAGEBLOCKBINDINGPROC)GL::getProcAddress("glShaderStorageBlockBinding");
    glTextureView = (PFNGLTEXTUREVIEWPROC)GL::getProcAddress("glTextureView");
    glVertexAttribLFormat = (PFNGLVERTEXATTRIBLFORMATPROC)GL::getProcAddress("glVertexAttribLFormat");
#endif // O3D_GL_VERSION_4_3

#ifdef O3D_GL_VERSION_4_4  // GL
    glBufferStorage = (PFNGLBUFFERSTORAGEPROC)GL::getProcAddress("glBufferStorage");
    glClearTexImage = (PFNGLCLEARTEXIMAGEPROC)GL::getProcAddress("glClearTexImage");
    glClearTexSubImage = (PFNGLCLEARTEXSUBIMAGEPROC)GL::getProcAddress("glClearTexSubImage");
    glBindBuffersBase = (PFNGLBINDBUFFERSBASEPROC)GL::getProcAddress("glBindBuffersBase");
    glBindBuffersRange = (PFNGLBINDBUFFERSRANGEPROC)GL::getProcAddress("glBindBuffersRange");
    glBindTextures = (PFNGLBINDTEXTURESPROC)GL::getProcAddress("glBindTextures");
    glBindSamplers = (PFNGLBINDSAMPLERSPROC)GL::getProcAddress("glBindSamplers");
    glBindImageTextures = (PFNGLBINDIMAGETEXTURESPROC)GL::getProcAddress("glBindImageTextures");
    glBindVertexBuffers = (PFNGLBINDVERTEXBUFFERSPROC)GL::getProcAddress("glBindVertexBuffers");
#endif // O3D_GL_VERSION_4_4

#ifdef O3D_GL_VERSION_4_5  // GL
    glClipControl = (PFNGLCLIPCONTROLPROC)GL::getProcAddress("glClipControl");
    glCreateTransformFeedbacks = (PFNGLCREATETRANSFORMFEEDBACKSPROC)GL::getProcAddress("glCreateTransformFeedbacks");
    glTransformFeedbackBufferBase = (PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC)GL::getProcAddress("glTransformFeedbackBufferBase");
    glTransformFeedbackBufferRange = (PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC)GL::getProcAddress("glTransformFeedbackBufferRange");
    glGetTransformFeedbackiv = (PFNGLGETTRANSFORMFEEDBACKIVPROC)GL::getProcAddress("glGetTransformFeedbackiv");
    glGetTransformFeedbacki_v = (PFNGLGETTRANSFORMFEEDBACKI_VPROC)GL::getProcAddress("glGetTransformFeedbacki_v");
    glGetTransformFeedbacki64_v = (PFNGLGETTRANSFORMFEEDBACKI64_VPROC)GL::getProcAddress("glGetTransformFeedbacki64_v");
    glCreateBuffers = (PFNGLCREATEBUFFERSPROC)GL::getProcAddress("glCreateBuffers");
    glNamedBufferStorage = (PFNGLNAMEDBUFFERSTORAGEPROC)GL::getProcAddress("glNamedBufferStorage");
    glNamedBufferData = (PFNGLNAMEDBUFFERDATAPROC)GL::getProcAddress("glNamedBufferData");
    glNamedBufferSubData = (PFNGLNAMEDBUFFERSUBDATAPROC)GL::getProcAddress("glNamedBufferSubData");
    glCopyNamedBufferSubData = (PFNGLCOPYNAMEDBUFFERSUBDATAPROC)GL::getProcAddress("glCopyNamedBufferSubData");
    glClearNamedBufferData = (PFNGLCLEARNAMEDBUFFERDATAPROC)GL::getProcAddress("glClearNamedBufferData");
    glClearNamedBufferSubData = (PFNGLCLEARNAMEDBUFFERSUBDATAPROC)GL::getProcAddress("glClearNamedBufferSubData");
    glMapNamedBuffer = (PFNGLMAPNAMEDBUFFERPROC)GL::getProcAddress("glMapNamedBuffer");
    glMapNamedBufferRange = (PFNGLMAPNAMEDBUFFERRANGEPROC)GL::getProcAddress("glMapNamedBufferRange");
    glUnmapNamedBuffer = (PFNGLUNMAPNAMEDBUFFERPROC)GL::getProcAddress("glUnmapNamedBuffer");
    glFlushMappedNamedBufferRange = (PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC)GL::getProcAddress("glFlushMappedNamedBufferRange");
    glGetNamedBufferParameteriv = (PFNGLGETNAMEDBUFFERPARAMETERIVPROC)GL::getProcAddress("glGetNamedBufferParameteriv");
    glGetNamedBufferParameteri64v = (PFNGLGETNAMEDBUFFERPARAMETERI64VPROC)GL::getProcAddress("glGetNamedBufferParameteri64v");
    glGetNamedBufferPointerv = (PFNGLGETNAMEDBUFFERPOINTERVPROC)GL::getProcAddress("glGetNamedBufferPointerv");
    glGetNamedBufferSubData = (PFNGLGETNAMEDBUFFERSUBDATAPROC)GL::getProcAddress("glGetNamedBufferSubData");
    glCreateFramebuffers = (PFNGLCREATEFRAMEBUFFERSPROC)GL::getProcAddress("glCreateFramebuffers");
    glNamedFramebufferRenderbuffer = (PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC)GL::getProcAddress("glNamedFramebufferRenderbuffer");
    glNamedFramebufferParameteri = (PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC)GL::getProcAddress("glNamedFramebufferParameteri");
    glNamedFramebufferTexture = (PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)GL::getProcAddress("glNamedFramebufferTexture");
    glNamedFramebufferTextureLayer = (PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC)GL::getProcAddress("glNamedFramebufferTextureLayer");
    glNamedFramebufferDrawBuffer = (PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC)GL::getProcAddress("glNamedFramebufferDrawBuffer");
    glNamedFramebufferDrawBuffers = (PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)GL::getProcAddress("glNamedFramebufferDrawBuffers");
    glNamedFramebufferReadBuffer = (PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC)GL::getProcAddress("glNamedFramebufferReadBuffer");
    glInvalidateNamedFramebufferData = (PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC)GL::getProcAddress("glInvalidateNamedFramebufferData");
    glInvalidateNamedFramebufferSubData = (PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC)GL::getProcAddress("glInvalidateNamedFramebufferSubData");
    glClearNamedFramebufferiv = (PFNGLCLEARNAMEDFRAMEBUFFERIVPROC)GL::getProcAddress("glClearNamedFramebufferiv");
    glClearNamedFramebufferuiv = (PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC)GL::getProcAddress("glClearNamedFramebufferuiv");
    glClearNamedFramebufferfv = (PFNGLCLEARNAMEDFRAMEBUFFERFVPROC)GL::getProcAddress("glClearNamedFramebufferfv");
    glClearNamedFramebufferfi = (PFNGLCLEARNAMEDFRAMEBUFFERFIPROC)GL::getProcAddress("glClearNamedFramebufferfi");
    glBlitNamedFramebuffer = (PFNGLBLITNAMEDFRAMEBUFFERPROC)GL::getProcAddress("glBlitNamedFramebuffer");
    glCheckNamedFramebufferStatus = (PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC)GL::getProcAddress("glCheckNamedFramebufferStatus");
    glGetNamedFramebufferParameteriv = (PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC)GL::getProcAddress("glGetNamedFramebufferParameteriv");
    glGetNamedFramebufferAttachmentParameteriv = (PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC)GL::getProcAddress("glGetNamedFramebufferAttachmentParameteriv");
    glCreateRenderbuffers = (PFNGLCREATERENDERBUFFERSPROC)GL::getProcAddress("glCreateRenderbuffers");
    glNamedRenderbufferStorage = (PFNGLNAMEDRENDERBUFFERSTORAGEPROC)GL::getProcAddress("glNamedRenderbufferStorage");
    glNamedRenderbufferStorageMultisample = (PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC)GL::getProcAddress("glNamedRenderbufferStorageMultisample");
    glGetNamedRenderbufferParameteriv = (PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC)GL::getProcAddress("glGetNamedRenderbufferParameteriv");
    glCreateTextures = (PFNGLCREATETEXTURESPROC)GL::getProcAddress("glCreateTextures");
    glTextureBuffer = (PFNGLTEXTUREBUFFERPROC)GL::getProcAddress("glTextureBuffer");
    glTextureBufferRange = (PFNGLTEXTUREBUFFERRANGEPROC)GL::getProcAddress("glTextureBufferRange");
    glTextureStorage1D = (PFNGLTEXTURESTORAGE1DPROC)GL::getProcAddress("glTextureStorage1D");
    glTextureStorage2D = (PFNGLTEXTURESTORAGE2DPROC)GL::getProcAddress("glTextureStorage2D");
    glTextureStorage3D = (PFNGLTEXTURESTORAGE3DPROC)GL::getProcAddress("glTextureStorage3D");
    glTextureStorage2DMultisample = (PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC)GL::getProcAddress("glTextureStorage2DMultisample");
    glTextureStorage3DMultisample = (PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC)GL::getProcAddress("glTextureStorage3DMultisample");
    glTextureSubImage1D = (PFNGLTEXTURESUBIMAGE1DPROC)GL::getProcAddress("glTextureSubImage1D");
    glTextureSubImage2D = (PFNGLTEXTURESUBIMAGE2DPROC)GL::getProcAddress("glTextureSubImage2D");
    glTextureSubImage3D = (PFNGLTEXTURESUBIMAGE3DPROC)GL::getProcAddress("glTextureSubImage3D");
    glCompressedTextureSubImage1D = (PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC)GL::getProcAddress("glCompressedTextureSubImage1D");
    glCompressedTextureSubImage2D = (PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC)GL::getProcAddress("glCompressedTextureSubImage2D");
    glCompressedTextureSubImage3D = (PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC)GL::getProcAddress("glCompressedTextureSubImage3D");
    glCopyTextureSubImage1D = (PFNGLCOPYTEXTURESUBIMAGE1DPROC)GL::getProcAddress("glCopyTextureSubImage1D");
    glCopyTextureSubImage2D = (PFNGLCOPYTEXTURESUBIMAGE2DPROC)GL::getProcAddress("glCopyTextureSubImage2D");
    glCopyTextureSubImage3D = (PFNGLCOPYTEXTURESUBIMAGE3DPROC)GL::getProcAddress("glCopyTextureSubImage3D");
    glTextureParameterf = (PFNGLTEXTUREPARAMETERFPROC)GL::getProcAddress("glTextureParameterf");
    glTextureParameterfv = (PFNGLTEXTUREPARAMETERFVPROC)GL::getProcAddress("glTextureParameterfv");
    glTextureParameteri = (PFNGLTEXTUREPARAMETERIPROC)GL::getProcAddress("glTextureParameteri");
    glTextureParameterIiv = (PFNGLTEXTUREPARAMETERIIVPROC)GL::getProcAddress("glTextureParameterIiv");
    glTextureParameterIuiv = (PFNGLTEXTUREPARAMETERIUIVPROC)GL::getProcAddress("glTextureParameterIuiv");
    glTextureParameteriv = (PFNGLTEXTUREPARAMETERIVPROC)GL::getProcAddress("glTextureParameteriv");
    glGenerateTextureMipmap = (PFNGLGENERATETEXTUREMIPMAPPROC)GL::getProcAddress("glGenerateTextureMipmap");
    glBindTextureUnit = (PFNGLBINDTEXTUREUNITPROC)GL::getProcAddress("glBindTextureUnit");
    glGetTextureImage = (PFNGLGETTEXTUREIMAGEPROC)GL::getProcAddress("glGetTextureImage");
    glGetCompressedTextureImage = (PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC)GL::getProcAddress("glGetCompressedTextureImage");
    glGetTextureLevelParameterfv = (PFNGLGETTEXTURELEVELPARAMETERFVPROC)GL::getProcAddress("glGetTextureLevelParameterfv");
    glGetTextureLevelParameteriv = (PFNGLGETTEXTURELEVELPARAMETERIVPROC)GL::getProcAddress("glGetTextureLevelParameteriv");
    glGetTextureParameterfv = (PFNGLGETTEXTUREPARAMETERFVPROC)GL::getProcAddress("glGetTextureParameterfv");
    glGetTextureParameterIiv = (PFNGLGETTEXTUREPARAMETERIIVPROC)GL::getProcAddress("glGetTextureParameterIiv");
    glGetTextureParameterIuiv = (PFNGLGETTEXTUREPARAMETERIUIVPROC)GL::getProcAddress("glGetTextureParameterIuiv");
    glGetTextureParameteriv = (PFNGLGETTEXTUREPARAMETERIVPROC)GL::getProcAddress("glGetTextureParameteriv");
    glCreateVertexArrays = (PFNGLCREATEVERTEXARRAYSPROC)GL::getProcAddress("glCreateVertexArrays");
    glDisableVertexArrayAttrib = (PFNGLDISABLEVERTEXARRAYATTRIBPROC)GL::getProcAddress("glDisableVertexArrayAttrib");
    glEnableVertexArrayAttrib = (PFNGLENABLEVERTEXARRAYATTRIBPROC)GL::getProcAddress("glEnableVertexArrayAttrib");
    glVertexArrayElementBuffer = (PFNGLVERTEXARRAYELEMENTBUFFERPROC)GL::getProcAddress("glVertexArrayElementBuffer");
    glVertexArrayVertexBuffer = (PFNGLVERTEXARRAYVERTEXBUFFERPROC)GL::getProcAddress("glVertexArrayVertexBuffer");
    glVertexArrayVertexBuffers = (PFNGLVERTEXARRAYVERTEXBUFFERSPROC)GL::getProcAddress("glVertexArrayVertexBuffers");
    glVertexArrayAttribBinding = (PFNGLVERTEXARRAYATTRIBBINDINGPROC)GL::getProcAddress("glVertexArrayAttribBinding");
    glVertexArrayAttribFormat = (PFNGLVERTEXARRAYATTRIBFORMATPROC)GL::getProcAddress("glVertexArrayAttribFormat");
    glVertexArrayAttribIFormat = (PFNGLVERTEXARRAYATTRIBIFORMATPROC)GL::getProcAddress("glVertexArrayAttribIFormat");
    glVertexArrayAttribLFormat = (PFNGLVERTEXARRAYATTRIBLFORMATPROC)GL::getProcAddress("glVertexArrayAttribLFormat");
    glVertexArrayBindingDivisor = (PFNGLVERTEXARRAYBINDINGDIVISORPROC)GL::getProcAddress("glVertexArrayBindingDivisor");
    glGetVertexArrayiv = (PFNGLGETVERTEXARRAYIVPROC)GL::getProcAddress("glGetVertexArrayiv");
    glGetVertexArrayIndexediv = (PFNGLGETVERTEXARRAYINDEXEDIVPROC)GL::getProcAddress("glGetVertexArrayIndexediv");
    glGetVertexArrayIndexed64iv = (PFNGLGETVERTEXARRAYINDEXED64IVPROC)GL::getProcAddress("glGetVertexArrayIndexed64iv");
    glCreateSamplers = (PFNGLCREATESAMPLERSPROC)GL::getProcAddress("glCreateSamplers");
    glCreateProgramPipelines = (PFNGLCREATEPROGRAMPIPELINESPROC)GL::getProcAddress("glCreateProgramPipelines");
    glCreateQueries = (PFNGLCREATEQUERIESPROC)GL::getProcAddress("glCreateQueries");
    glGetQueryBufferObjecti64v = (PFNGLGETQUERYBUFFEROBJECTI64VPROC)GL::getProcAddress("glGetQueryBufferObjecti64v");
    glGetQueryBufferObjectiv = (PFNGLGETQUERYBUFFEROBJECTIVPROC)GL::getProcAddress("glGetQueryBufferObjectiv");
    glGetQueryBufferObjectui64v = (PFNGLGETQUERYBUFFEROBJECTUI64VPROC)GL::getProcAddress("glGetQueryBufferObjectui64v");
    glGetQueryBufferObjectuiv = (PFNGLGETQUERYBUFFEROBJECTUIVPROC)GL::getProcAddress("glGetQueryBufferObjectuiv");
    glMemoryBarrierByRegion = (PFNGLMEMORYBARRIERBYREGIONPROC)GL::getProcAddress("glMemoryBarrierByRegion");
    glGetTextureSubImage = (PFNGLGETTEXTURESUBIMAGEPROC)GL::getProcAddress("glGetTextureSubImage");
    glGetCompressedTextureSubImage = (PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC)GL::getProcAddress("glGetCompressedTextureSubImage");
    glGetGraphicsResetStatus = (PFNGLGETGRAPHICSRESETSTATUSPROC)GL::getProcAddress("glGetGraphicsResetStatus");
    glGetnCompressedTexImage = (PFNGLGETNCOMPRESSEDTEXIMAGEPROC)GL::getProcAddress("glGetnCompressedTexImage");
    glGetnTexImage = (PFNGLGETNTEXIMAGEPROC)GL::getProcAddress("glGetnTexImage");
    glGetnUniformdv = (PFNGLGETNUNIFORMDVPROC)GL::getProcAddress("glGetnUniformdv");
    glGetnUniformfv = (PFNGLGETNUNIFORMFVPROC)GL::getProcAddress("glGetnUniformfv");
    glGetnUniformiv = (PFNGLGETNUNIFORMIVPROC)GL::getProcAddress("glGetnUniformiv");
    glGetnUniformuiv = (PFNGLGETNUNIFORMUIVPROC)GL::getProcAddress("glGetnUniformuiv");
    glReadnPixels = (PFNGLREADNPIXELSPROC)GL::getProcAddress("glReadnPixels");
    glGetnMapdv = (PFNGLGETNMAPDVPROC)GL::getProcAddress("glGetnMapdv");
    glGetnMapfv = (PFNGLGETNMAPFVPROC)GL::getProcAddress("glGetnMapfv");
    glGetnMapiv = (PFNGLGETNMAPIVPROC)GL::getProcAddress("glGetnMapiv");
    glGetnPixelMapfv = (PFNGLGETNPIXELMAPFVPROC)GL::getProcAddress("glGetnPixelMapfv");
    glGetnPixelMapuiv = (PFNGLGETNPIXELMAPUIVPROC)GL::getProcAddress("glGetnPixelMapuiv");
    glGetnPixelMapusv = (PFNGLGETNPIXELMAPUSVPROC)GL::getProcAddress("glGetnPixelMapusv");
    glGetnPolygonStipple = (PFNGLGETNPOLYGONSTIPPLEPROC)GL::getProcAddress("glGetnPolygonStipple");
    glGetnColorTable = (PFNGLGETNCOLORTABLEPROC)GL::getProcAddress("glGetnColorTable");
    glGetnConvolutionFilter = (PFNGLGETNCONVOLUTIONFILTERPROC)GL::getProcAddress("glGetnConvolutionFilter");
    glGetnSeparableFilter = (PFNGLGETNSEPARABLEFILTERPROC)GL::getProcAddress("glGetnSeparableFilter");
    glGetnHistogram = (PFNGLGETNHISTOGRAMPROC)GL::getProcAddress("glGetnHistogram");
    glGetnMinmax = (PFNGLGETNMINMAXPROC)GL::getProcAddress("glGetnMinmax");
    glTextureBarrier = (PFNGLTEXTUREBARRIERPROC)GL::getProcAddress("glTextureBarrier");
#endif // O3D_GL_VERSION_4_5

#ifdef O3D_GL_VERSION_4_6  // GL
    glSpecializeShader = (PFNGLSPECIALIZESHADERPROC)GL::getProcAddress("glSpecializeShader");
    glMultiDrawArraysIndirectCount = (PFNGLMULTIDRAWARRAYSINDIRECTCOUNTPROC)GL::getProcAddress("glMultiDrawArraysIndirectCount");
    glMultiDrawElementsIndirectCount = (PFNGLMULTIDRAWELEMENTSINDIRECTCOUNTPROC)GL::getProcAddress("glMultiDrawElementsIndirectCount");
    glPolygonOffsetClamp = (PFNGLPOLYGONOFFSETCLAMPPROC)GL::getProcAddress("glPolygonOffsetClamp");
#endif // O3D_GL_VERSION_4_6

#ifdef O3D_GL_ARB_ES3_2_compatibility  // GL
    glPrimitiveBoundingBox = (PFNGLPRIMITIVEBOUNDINGBOXARBPROC)GL::getProcAddress("glPrimitiveBoundingBoxARB");
#endif // O3D_GL_ARB_ES3_2_compatibility
}

void GLExtensionManager::getGLESFunctions()
{
    REQUIRED_EXT(OES_framebuffer_object)
    REQUIRED_EXT(OES_texture_float)
    REQUIRED_EXT(OES_texture_half_float)
    REQUIRED_EXT(OES_texture_float_linear)
    REQUIRED_EXT(OES_texture_half_float_linear)
    // REQUIRED_EXT(OES_texture_3D)

#ifdef O3D_GLES_VERSION_200
#endif // O3D_GLESS_VERSION_200

#ifdef O3D_GLES_VERSION_300
#endif // O3D_GLESS_VERSION_300

#ifdef O3D_GLES_VERSION_310
#endif // O3D_GLESS_VERSION_310

#ifdef O3D_GLES_VERSION_320
#endif // O3D_GLESS_VERSION_320

#ifdef O3D_GL_VERSION_3_0  // GL ES
    if (!glVertexAttribIPointer || !glGetVertexAttribIiv || !glGetVertexAttribIuiv) {
        O3D_WARNING("OpenGL VAO Index is not available");
    }
#endif // O3D_GL_VERSION_3_0

#ifdef O3D_GL_ARB_ES3_2_compatibility  // GL ES
    glPrimitiveBoundingBox = (PFNGLPRIMITIVEBOUNDINGBOXARBPROC)GL::getProcAddress("glPrimitiveBoundingBox");
#endif // O3D_GL_ARB_ES3_2_compatibility
}

#include "objective3dconfig.h"

// Initialize the extension manager
void GLExtensionManager::init()
{
    if (ms_valid) {
		return;
    }

    _glGetString = (PFNGLGETSTRINGPROC) GL::getProcAddress("glGetString");
    if (!_glGetString) {
    #ifdef O3D_ANDROID
        ms_openGL = DynamicLibrary::load("libGL.so");
    #elif defined(O3D_LINUX)
        ms_openGL = DynamicLibrary::load("libGL.so");
    #elif defined(O3D_MACOSX)
        ms_openGL = DynamicLibrary::load("libGL.so");
    #elif defined(O3D_WINDOWS)
        ms_openGL = DynamicLibrary::load("Opengl32.dll");
    #endif
    }

    if (!_glGetString) {
        O3D_ERROR(E_InvalidResult("Undefined OpenGL glGetString function"));
    }

	// get extensions functions pointers (only for windows or for non valid OpenGL 2.0 context).
    const GLubyte *version = _glGetString(GL_VERSION);
    if (!version) {
        O3D_ERROR(E_InvalidResult("Undefined OpenGL version"));
    }

	// get glGetStringi before
    glGetStringi = (PFNGLGETSTRINGIPROC) GL::getProcAddress("glGetStringi");

    // common functions
    getGLCommonFunctions();

    // specifics functions
    if (GL::getType() == GL::API_GL || GL::getType() == GL::API_CUSTOM) {
        getGLFunctions();
    } else if (GL::getType() == GL::API_GLES_3) {
        getGLESFunctions();
    }

    ms_valid = True;
}

// Check for an extension
Bool GLExtensionManager::isExtensionSupported(const String &ext)
{
	// check for a valid extension name
    if ((ext.find(' ') > 0) || (ext.length() == 0)) {
		return False;
    }

    const GLubyte *version = _glGetString(GL_VERSION);
    if ((version != nullptr) && ((version[0] - 48) >= 3)) {
		GLint numExts;

        _glGetIntegerv(GL_NUM_EXTENSIONS, &numExts);

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
        const GLubyte *extensions = _glGetString(GL_EXTENSIONS);

        if (!extensions) {
            O3D_ERROR(E_InvalidResult("glGetString(GL_EXTENSIONS) is null"));
        }

		String exts = reinterpret_cast<const Char*>(extensions);
		StringTokenizer tokenizer(exts, " ");

        while (tokenizer.hasMoreElements()) {
            if (tokenizer.nextElement() == ext) {
				return True;
            }
		}

		return False;
	}
}

void* GLExtensionManager::getFunctionPtr(const Char *foo)
{
    return (void*)(GL::getProcAddress(foo));
}

void* GLExtensionManager::getFunctionPtr(const CString &foo)
{
    return (void*)(GL::getProcAddress(foo.getData()));
}
