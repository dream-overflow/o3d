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
// draw range element
PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements = nullptr;

// texture_3d
PFNGLTEXIMAGE3DPROC glTexImage3D = nullptr;
PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D = nullptr;
#endif // O3D_GL_VERSION_1_2

#ifdef O3D_GL_VERSION_1_3
// multi-texture extension
PFNGLACTIVETEXTUREPROC glActiveTexture = nullptr;

// glCompressedTexImage
PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D = nullptr;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D = nullptr;
PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D = nullptr;
PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage = nullptr;
#endif // O3D_GL_VERSION_1_3

#ifdef O3D_GL_VERSION_1_4
// point_parameters can be used for particles
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
// occlusion_query
PFNGLGENQUERIESPROC glGenQueries = nullptr;
PFNGLISQUERYPROC glIsQuery = nullptr;
PFNGLBEGINQUERYPROC glBeginQuery = nullptr;
PFNGLENDQUERYPROC glEndQuery = nullptr;
PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv = nullptr;
PFNGLGETQUERYIVPROC glGetQueryiv = nullptr;
PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv = nullptr;
PFNGLDELETEQUERIESPROC glDeleteQueries = nullptr;

// vertex_buffer_object
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

// shading_language
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

// uniform
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

// vertex attributes array
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib = nullptr;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation = nullptr;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = nullptr;
PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv = nullptr;
PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv = nullptr;
PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv = nullptr;
PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv = nullptr;

// vertex attributes
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
#endif O3D_GL_VERSION_3_1

#ifdef O3D_GL_VERSION_3_2
//PFNGLGETINTEGER64I_VPROC glGetInteger64i_v = nullptr;
//PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v = nullptr;
PFNGLPROGRAMPARAMETERIPROC glProgramParameteri = nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture = nullptr;
PFNGLFRAMEBUFFERTEXTUREFACEPROC glFramebufferTextureFace = nullptr;
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
PFNGLDEPTHRANGEFPROC glDepthRangef = nullptr;
PFNGLCLEARDEPTHFPROC glClearDepthf = nullptr;
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
#endif // O3D_GL_VERSION_4_3

#ifdef O3D_GL_VERSION_4_4
#endif // O3D_GL_VERSION_4_4

#ifdef O3D_GL_VERSION_4_5

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

PFNGLGETTEXTURESUBIMAGEPROC glGetTextureSubImage = nullptr;

#endif // O3D_GL_VERSION_4_5

#ifdef O3D_GL_VERSION_4_6
#endif // O3D_GL_VERSION_4_6

#ifdef O3D_GL_ARB_framebuffer_object
// framebuffer_object (render-to-texture)
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
#endif // O3D_GL_ARB_framebuffer_object

// O3D_GL_ARB_vertex_array_object
#ifdef O3D_GL_ARB_vertex_array_object
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = nullptr;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
PFNGLISVERTEXARRAYPROC glIsVertexArray   = nullptr;
#endif // O3D_GL_ARB_vertex_array_object

#ifdef O3D_GL_ARB_map_buffer_range
PFNGLMAPBUFFERRANGEPROC glMapBufferRange = nullptr;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange = nullptr;
#endif // O3D_GL_ARB_map_buffer_range

#ifdef O3D_GL_ARB_texture_multisample
PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample = nullptr;
PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample = nullptr;
PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv = nullptr;
PFNGLSAMPLEMASKIPROC glSampleMaski = nullptr;
#endif // GL_ARB_texture_multisample

#undef Bool

// statics members
Bool GLExtensionManager::ms_valid = False;
DynamicLibrary* GLExtensionManager::ms_openGL = nullptr;

//
// Profile GL 3.0+
//

// Get extensions pointers
void GLExtensionManager::getGLFunctions()
{
    //
    // OpenGL 1.0
    //

#ifdef O3D_GL_VERSION_1_0
    glCullFace = (PFNGLCULLFACEPROC) GL::getProcAddress("glCullFace");
    glFrontFace = (PFNGLFRONTFACEPROC) GL::getProcAddress("glFrontFace");
    glHint = (PFNGLHINTPROC) GL::getProcAddress("glHint");
    glLineWidth = (PFNGLLINEWIDTHPROC) GL::getProcAddress("glLineWidth");
    glPointSize = (PFNGLPOINTSIZEPROC) GL::getProcAddress("glPointSize");
    glPolygonMode = (PFNGLPOLYGONMODEPROC) GL::getProcAddress("glPolygonMode");
    glScissor = (PFNGLSCISSORPROC) GL::getProcAddress("glScissor");
    glTexParameterf = (PFNGLTEXPARAMETERFPROC) GL::getProcAddress("glTexParameterf");
    glTexParameterfv = (PFNGLTEXPARAMETERFVPROC) GL::getProcAddress("glTexParameterfv");
    glTexParameteri = (PFNGLTEXPARAMETERIPROC) GL::getProcAddress("glTexParameteri");
    glTexParameteriv = (PFNGLTEXPARAMETERIVPROC) GL::getProcAddress("glTexParameteriv");
    glTexImage1D = (PFNGLTEXIMAGE1DPROC) GL::getProcAddress("glTexImage1D");
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
    glLogicOp = (PFNGLLOGICOPPROC) GL::getProcAddress("glLogicOp");
    glStencilFunc = (PFNGLSTENCILFUNCPROC) GL::getProcAddress("glStencilFunc");
    glStencilOp = (PFNGLSTENCILOPPROC) GL::getProcAddress("glStencilOp");
    glDepthFunc = (PFNGLDEPTHFUNCPROC) GL::getProcAddress("glDepthFunc");
    glPixelStoref = (PFNGLPIXELSTOREFPROC) GL::getProcAddress("glPixelStoref");
    glPixelStorei = (PFNGLPIXELSTOREIPROC) GL::getProcAddress("glPixelStorei");
    glReadBuffer = (PFNGLREADBUFFERPROC) GL::getProcAddress("glReadBuffer");
    glReadPixels = (PFNGLREADPIXELSPROC) GL::getProcAddress("glReadPixels");
    glGetBooleanv = (PFNGLGETBOOLEANVPROC) GL::getProcAddress("glGetBooleanv");
    glGetDoublev = (PFNGLGETDOUBLEVPROC) GL::getProcAddress("glGetDoublev");
    glGetError = (PFNGLGETERRORPROC) GL::getProcAddress("glGetError");
    glGetFloatv = (PFNGLGETFLOATVPROC) GL::getProcAddress("glGetFloatv");
    _glGetIntegerv = (PFNGLGETINTEGERVPROC) GL::getProcAddress("glGetIntegerv");
    _glGetString = (PFNGLGETSTRINGPROC) GL::getProcAddress("glGetString");
    glGetTexImage = (PFNGLGETTEXIMAGEPROC) GL::getProcAddress("glGetTexImage");
    glGetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC) GL::getProcAddress("glGetTexParameterfv");
    glGetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC) GL::getProcAddress("glGetTexParameteriv");
    glGetTexLevelParameterfv = (PFNGLGETTEXLEVELPARAMETERFVPROC) GL::getProcAddress("glGetTexLevelParameterfv");
    glGetTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIVPROC) GL::getProcAddress("glGetTexLevelParameteriv");
    glIsEnabled = (PFNGLISENABLEDPROC) GL::getProcAddress("glIsEnabled");
    glDepthRange = (PFNGLDEPTHRANGEPROC) GL::getProcAddress("glDepthRange");
    glViewport = (PFNGLVIEWPORTPROC) GL::getProcAddress("glViewport");
#endif

    //
    // OpenGL 1.1
    //

#ifdef O3D_GL_VERSION_1_1
    glDrawArrays = (PFNGLDRAWARRAYSPROC) GL::getProcAddress("glDrawArrays");
    glDrawElements = (PFNGLDRAWELEMENTSPROC) GL::getProcAddress("glDrawElements");
    glGetPointerv = (PFNGLGETPOINTERVPROC) GL::getProcAddress("glGetPointerv");
    glPolygonOffset = (PFNGLPOLYGONOFFSETPROC) GL::getProcAddress("glPolygonOffset");
    glCopyTexImage1D = (PFNGLCOPYTEXIMAGE1DPROC) GL::getProcAddress("glCopyTexImage1D");
    glCopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC) GL::getProcAddress("glCopyTexImage2D");
    glCopyTexSubImage1D = (PFNGLCOPYTEXSUBIMAGE1DPROC) GL::getProcAddress("glCopyTexSubImage1D");
    glCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC) GL::getProcAddress("glCopyTexSubImage2D");
    glTexSubImage1D = (PFNGLTEXSUBIMAGE1DPROC) GL::getProcAddress("glTexSubImage1D");
    glTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC) GL::getProcAddress("glTexSubImage2D");
    glBindTexture = (PFNGLBINDTEXTUREPROC) GL::getProcAddress("glBindTexture");
    glDeleteTextures = (PFNGLDELETETEXTURESPROC) GL::getProcAddress("glDeleteTextures");
    glGenTextures = (PFNGLGENTEXTURESPROC) GL::getProcAddress("glGenTextures");
    glIsTexture = (PFNGLISTEXTUREPROC) GL::getProcAddress("glIsTexture");
#endif

	//
	// draw range element*
	//

#ifdef O3D_GL_VERSION_1_2
    glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC) GL::getProcAddress("glDrawRangeElements");
    if (!glDrawRangeElements) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL draw range elements"));
    }

	//
	// Texture3d
	//

    glTexImage3D = (PFNGLTEXIMAGE3DPROC) GL::getProcAddress("glTexImage3D");
    glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC) GL::getProcAddress("glTexSubImage3D");
    if (!glTexImage3D || !glTexSubImage3D) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL texture 3D"));
    }
#endif // O3D_GL_VERSION_1_2

#ifdef O3D_GL_VERSION_1_3

	//
	// multi-texture extension
	//

    glActiveTexture = (PFNGLACTIVETEXTUREPROC) GL::getProcAddress("glActiveTexture");
    if (!glActiveTexture && isExtensionSupported("GL_ARB_multitexture")) {
        glActiveTexture = (PFNGLACTIVETEXTUREPROC) GL::getProcAddress("glActiveTextureARB");
    }

    if (!glActiveTexture) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL active texture"));
    }

	//
	// Compressed texture image
	//

    glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC) GL::getProcAddress("glCompressedTexImage1D");
    glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) GL::getProcAddress("glCompressedTexImage2D");
    glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC) GL::getProcAddress("glCompressedTexImage3D");
    glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) GL::getProcAddress("glCompressedTexSubImage3D");
    glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) GL::getProcAddress("glCompressedTexSubImage2D");
    glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) GL::getProcAddress("glCompressedTexSubImage1D");
    glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC) GL::getProcAddress("glGetCompressedTexImage");

    if ((!glCompressedTexImage1D || !glCompressedTexImage2D || !glCompressedTexImage3D ||
        !glCompressedTexSubImage1D || !glCompressedTexSubImage2D || !glCompressedTexSubImage3D ||
        !glGetCompressedTexImage) && isExtensionSupported("GL_ARB_texture_compression")) {

        glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC) GL::getProcAddress("glCompressedTexImage1DARB");
        glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) GL::getProcAddress("glCompressedTexImage2DARB");
        glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC) GL::getProcAddress("glCompressedTexImage3DARB");
        glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) GL::getProcAddress("glCompressedTexSubImage3DARB");
        glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) GL::getProcAddress("glCompressedTexSubImage2DARB");
        glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) GL::getProcAddress("glCompressedTexSubImage1DARB");
        glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC) GL::getProcAddress("glGetCompressedTexImageARB");
    }

    if (!glCompressedTexImage1D || !glCompressedTexSubImage3D) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL compressed texture 1D"));
    }

    if (!glCompressedTexImage2D || !glCompressedTexSubImage2D) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL compressed texture 2D"));
    }

    if (!glCompressedTexImage3D || !glCompressedTexSubImage3D) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL compressed texture 3D"));
    }

    if (!glGetCompressedTexImage) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL compressed texture get image"));
    }
#endif // O3D_GL_VERSION_1_3

#ifdef O3D_GL_VERSION_1_4

	//
	// PointParametersSupported
	//

    glPointParameterf  = (PFNGLPOINTPARAMETERFPROC) GL::getProcAddress("glPointParameterf");
    glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC) GL::getProcAddress("glPointParameterfv");
    glPointParameteri  = (PFNGLPOINTPARAMETERIPROC) GL::getProcAddress("glPointParameteri");
    glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC) GL::getProcAddress("glPointParameteriv");

    if ((!glPointParameterf || !glPointParameterfv || !glPointParameteri || !glPointParameteriv) &&
        isExtensionSupported("GL_ARB_point_parameters")) {

        glPointParameterf  = (PFNGLPOINTPARAMETERFPROC) GL::getProcAddress("glPointParameterfARB");
        glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC) GL::getProcAddress("glPointParameterfvARB");
        glPointParameteri  = (PFNGLPOINTPARAMETERIPROC) GL::getProcAddress("glPointParameteriARB");
        glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC) GL::getProcAddress("glPointParameterivARB");
    }

    if (!glPointParameterf || !glPointParameterfv || !glPointParameteri || !glPointParameteriv) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL point parameter"));
    }

    glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) GL::getProcAddress("glBlendFuncSeparate");

    if (!glBlendFuncSeparate) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL blend func separate"));
    }

    glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC) GL::getProcAddress("glMultiDrawArrays");
    glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC) GL::getProcAddress("glMultiDrawElements");

    if (!glMultiDrawArrays || !glMultiDrawElements) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL multi draw"));
    }

    glBlendEquation = (PFNGLBLENDEQUATIONPROC) GL::getProcAddress("glBlendEquation");

    if (!glBlendEquation) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL blend equation"));
    }

#endif // O3D_GL_VERSION_1_4

#ifdef O3D_GL_VERSION_1_5
	//
	// Vertex Buffer Object
	//

    glBindBuffer		= (PFNGLBINDBUFFERPROC) GL::getProcAddress("glBindBuffer");
    glGenBuffers		= (PFNGLGENBUFFERSPROC) GL::getProcAddress("glGenBuffers");
    glBufferData		= (PFNGLBUFFERDATAPROC) GL::getProcAddress("glBufferData");
    glBufferSubData		= (PFNGLBUFFERSUBDATAPROC) GL::getProcAddress("glBufferSubData");
    glDeleteBuffers		= (PFNGLDELETEBUFFERSPROC) GL::getProcAddress("glDeleteBuffers");
    glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC) GL::getProcAddress("glGetBufferParameteriv");
    glMapBuffer			= (PFNGLMAPBUFFERPROC) GL::getProcAddress("glMapBuffer");
    glUnmapBuffer		= (PFNGLUNMAPBUFFERPROC) GL::getProcAddress("glUnmapBuffer");

    if ((!glBindBuffer || !glDeleteBuffers || !glGenBuffers || !glBufferData ||
        !glGetBufferParameteriv || !glMapBuffer || !glUnmapBuffer) &&
        isExtensionSupported("GL_ARB_vertex_buffer_object")) {

        glBindBuffer			= (PFNGLBINDBUFFERPROC) GL::getProcAddress("glBindBufferARB");
        glGenBuffers			= (PFNGLGENBUFFERSPROC) GL::getProcAddress("glGenBuffersARB");
        glBufferData			= (PFNGLBUFFERDATAPROC) GL::getProcAddress("glBufferDataARB");
        glBufferSubData			= (PFNGLBUFFERSUBDATAPROC) GL::getProcAddress("glBufferSubDataARB");
        glDeleteBuffers			= (PFNGLDELETEBUFFERSPROC) GL::getProcAddress("glDeleteBuffersARB");
        glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC) GL::getProcAddress("glGetBufferParameterivARB");
        glMapBuffer		        = (PFNGLMAPBUFFERPROC) GL::getProcAddress("glMapBufferARB");
        glUnmapBuffer			= (PFNGLUNMAPBUFFERPROC) GL::getProcAddress("glUnmapBufferARB");
    }

    if (!glBindBuffer|| !glDeleteBuffers|| !glGenBuffers|| !glBufferData ||
        !glGetBufferParameteriv|| !glMapBuffer|| !glUnmapBuffer) {

        O3D_ERROR(E_UnsuportedFeature("OpenGL VBO"));
    }

	// OcclusionQuerySupported
    glGenQueries	= (PFNGLGENQUERIESPROC)GL::getProcAddress("glGenQueries");
    glIsQuery		= (PFNGLISQUERYPROC)GL::getProcAddress("glIsQuery");
    glBeginQuery	= (PFNGLBEGINQUERYPROC)GL::getProcAddress("glBeginQuery");
    glEndQuery		= (PFNGLENDQUERYPROC)GL::getProcAddress("glEndQuery");
    glGetQueryiv		= (PFNGLGETQUERYIVPROC)GL::getProcAddress("glGetQueryiv");
    glGetQueryObjectiv	= (PFNGLGETQUERYOBJECTIVPROC)GL::getProcAddress("glGetQueryObjectiv");
    glGetQueryObjectuiv	= (PFNGLGETQUERYOBJECTUIVPROC)GL::getProcAddress("glGetQueryObjectuiv");
    glDeleteQueries		= (PFNGLDELETEQUERIESPROC)GL::getProcAddress("glDeleteQueries");

    if ((!glGenQueries || !glIsQuery || !glBeginQuery || !glEndQuery || !glGetQueryiv ||
        !glGetQueryObjectiv || !glGetQueryObjectuiv || !glDeleteQueries) &&
        isExtensionSupported("GL_ARB_occlusion_query")) {

        glGenQueries	= (PFNGLGENQUERIESPROC)GL::getProcAddress("glGenQueriesARB");
        glIsQuery		= (PFNGLISQUERYPROC)GL::getProcAddress("glIsQueryARB");
        glBeginQuery	= (PFNGLBEGINQUERYPROC)GL::getProcAddress("glBeginQueryARB");
        glEndQuery		= (PFNGLENDQUERYPROC)GL::getProcAddress("glEndQueryARB");
        glGetQueryiv		= (PFNGLGETQUERYIVPROC)GL::getProcAddress("glGetQueryivARB");
        glGetQueryObjectiv	= (PFNGLGETQUERYOBJECTIVPROC)GL::getProcAddress("glGetQueryObjectivARB");
        glGetQueryObjectuiv	= (PFNGLGETQUERYOBJECTUIVPROC)GL::getProcAddress("glGetQueryObjectuivARB");
        glDeleteQueries		= (PFNGLDELETEQUERIESPROC)GL::getProcAddress("glDeleteQueriesARB");
    }

    if (!glGenQueries || !glIsQuery || !glBeginQuery || !glEndQuery || !glGetQueryiv ||
        !glGetQueryObjectiv || !glGetQueryObjectuiv || !glDeleteQueries) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL occlusion query"));
    }
#endif // O3D_GL_VERSION_1_5

#ifdef O3D_GL_VERSION_2_0

	//
	// GLSL
	//

    glCreateShader		= (PFNGLCREATESHADERPROC) GL::getProcAddress("glCreateShader");
    glShaderSource		= (PFNGLSHADERSOURCEPROC) GL::getProcAddress("glShaderSource");
    glCompileShader		= (PFNGLCOMPILESHADERPROC) GL::getProcAddress("glCompileShader");
    glCreateProgram		= (PFNGLCREATEPROGRAMPROC) GL::getProcAddress("glCreateProgram");
    glAttachShader		= (PFNGLATTACHSHADERPROC) GL::getProcAddress("glAttachShader");
    glLinkProgram		= (PFNGLLINKPROGRAMPROC) GL::getProcAddress("glLinkProgram");
    glValidateProgram	= (PFNGLVALIDATEPROGRAMPROC)GL::getProcAddress("glValidateProgram");
    glUseProgram		= (PFNGLUSEPROGRAMPROC) GL::getProcAddress("glUseProgram");

    glDetachShader		= (PFNGLDETACHSHADERPROC) GL::getProcAddress("glDetachShader");
    glDeleteShader		= (PFNGLDELETESHADERPROC) GL::getProcAddress("glDeleteShader");
    glDeleteProgram		= (PFNGLDELETEPROGRAMPROC) GL::getProcAddress("glDeleteProgram");

    glGetProgramiv		= (PFNGLGETPROGRAMIVPROC) GL::getProcAddress("glGetProgramiv");
    glGetProgramInfoLog	= (PFNGLGETPROGRAMINFOLOGPROC) GL::getProcAddress("glGetProgramInfoLog");
    glGetShaderiv		= (PFNGLGETSHADERIVPROC) GL::getProcAddress("glGetShaderiv");
    glGetShaderInfoLog	= (PFNGLGETSHADERINFOLOGPROC) GL::getProcAddress("glGetShaderInfoLog");
    glIsProgram		= (PFNGLISPROGRAMPROC) GL::getProcAddress("glIsProgram");
    glIsShader		= (PFNGLISSHADERPROC) GL::getProcAddress("glIsShader");

    glGetUniformLocation= (PFNGLGETUNIFORMLOCATIONPROC) GL::getProcAddress("glGetUniformLocation");
    glGetActiveUniform  = (PFNGLGETACTIVEUNIFORMPROC) GL::getProcAddress("glGetActiveUniform");
    glUniform1f		= (PFNGLUNIFORM1FPROC) GL::getProcAddress("glUniform1f");
    glUniform2f		= (PFNGLUNIFORM2FPROC) GL::getProcAddress("glUniform2f");
    glUniform3f		= (PFNGLUNIFORM3FPROC) GL::getProcAddress("glUniform3f");
    glUniform4f		= (PFNGLUNIFORM4FPROC) GL::getProcAddress("glUniform4f");
    glUniform1fv	= (PFNGLUNIFORM1FVPROC) GL::getProcAddress("glUniform1fv");
    glUniform2fv	= (PFNGLUNIFORM2FVPROC) GL::getProcAddress("glUniform2fv");
    glUniform3fv	= (PFNGLUNIFORM3FVPROC) GL::getProcAddress("glUniform3fv");
    glUniform4fv	= (PFNGLUNIFORM4FVPROC) GL::getProcAddress("glUniform4fv");
    glUniform1i		= (PFNGLUNIFORM1IPROC) GL::getProcAddress("glUniform1i");
    glUniform2i		= (PFNGLUNIFORM2IPROC) GL::getProcAddress("glUniform2i");
    glUniform3i		= (PFNGLUNIFORM3IPROC) GL::getProcAddress("glUniform3i");
    glUniform4i		= (PFNGLUNIFORM4IPROC) GL::getProcAddress("glUniform4i");
    glUniformMatrix2fv	= (PFNGLUNIFORMMATRIX2FVPROC) GL::getProcAddress("glUniformMatrix2fv");
    glUniformMatrix3fv	= (PFNGLUNIFORMMATRIX3FVPROC) GL::getProcAddress("glUniformMatrix3fv");
    glUniformMatrix4fv	= (PFNGLUNIFORMMATRIX4FVPROC) GL::getProcAddress("glUniformMatrix4fv");

    glDisableVertexAttribArray	= (PFNGLDISABLEVERTEXATTRIBARRAYPROC) GL::getProcAddress("glDisableVertexAttribArray");
    glEnableVertexAttribArray	= (PFNGLENABLEVERTEXATTRIBARRAYPROC) GL::getProcAddress("glEnableVertexAttribArray");
    glGetActiveAttrib		= (PFNGLGETACTIVEATTRIBPROC) GL::getProcAddress("glGetActiveAttrib");
    glBindAttribLocation		= (PFNGLBINDATTRIBLOCATIONPROC) GL::getProcAddress("glBindAttribLocation");
    glGetAttribLocation		= (PFNGLGETATTRIBLOCATIONPROC) GL::getProcAddress("glGetAttribLocation");
    glGetVertexAttribdv		= (PFNGLGETVERTEXATTRIBDVPROC) GL::getProcAddress("glGetVertexAttribdv");
    glGetVertexAttribfv		= (PFNGLGETVERTEXATTRIBFVPROC) GL::getProcAddress("glGetVertexAttribfv");
    glGetVertexAttribiv		= (PFNGLGETVERTEXATTRIBIVPROC) GL::getProcAddress("glGetVertexAttribiv");
    glGetVertexAttribPointerv	= (PFNGLGETVERTEXATTRIBPOINTERVPROC) GL::getProcAddress("glGetVertexAttribPointerv");

    glVertexAttrib1d		= (PFNGLVERTEXATTRIB1DPROC) GL::getProcAddress("glVertexAttrib1d");
    glVertexAttrib1dv		= (PFNGLVERTEXATTRIB1DVPROC) GL::getProcAddress("glVertexAttrib1dv");
    glVertexAttrib1f		= (PFNGLVERTEXATTRIB1FPROC) GL::getProcAddress("glVertexAttrib1f");
    glVertexAttrib1fv		= (PFNGLVERTEXATTRIB1FVPROC) GL::getProcAddress("glVertexAttrib1fv");
    glVertexAttrib1s		= (PFNGLVERTEXATTRIB1SPROC) GL::getProcAddress("glVertexAttrib1s");
    glVertexAttrib1sv		= (PFNGLVERTEXATTRIB1SVPROC) GL::getProcAddress("glVertexAttrib1sv");
    glVertexAttrib2d		= (PFNGLVERTEXATTRIB2DPROC) GL::getProcAddress("glVertexAttrib2d");
    glVertexAttrib2dv		= (PFNGLVERTEXATTRIB2DVPROC) GL::getProcAddress("glVertexAttrib2dv");
    glVertexAttrib2f		= (PFNGLVERTEXATTRIB2FPROC) GL::getProcAddress("glVertexAttrib2f");
    glVertexAttrib2fv		= (PFNGLVERTEXATTRIB2FVPROC) GL::getProcAddress("glVertexAttrib2fv");
    glVertexAttrib2s		= (PFNGLVERTEXATTRIB2SPROC) GL::getProcAddress("glVertexAttrib2s");
    glVertexAttrib2sv		= (PFNGLVERTEXATTRIB2SVPROC) GL::getProcAddress("glVertexAttrib2sv");
    glVertexAttrib3d		= (PFNGLVERTEXATTRIB3DPROC) GL::getProcAddress("glVertexAttrib3d");
    glVertexAttrib3dv		= (PFNGLVERTEXATTRIB3DVPROC) GL::getProcAddress("glVertexAttrib3dv");
    glVertexAttrib3f		= (PFNGLVERTEXATTRIB3FPROC) GL::getProcAddress("glVertexAttrib3f");
    glVertexAttrib3fv		= (PFNGLVERTEXATTRIB3FVPROC) GL::getProcAddress("glVertexAttrib3fv");
    glVertexAttrib3s		= (PFNGLVERTEXATTRIB3SPROC) GL::getProcAddress("glVertexAttrib3s");
    glVertexAttrib3sv		= (PFNGLVERTEXATTRIB3SVPROC) GL::getProcAddress("glVertexAttrib3sv");
    glVertexAttrib4Nbv		= (PFNGLVERTEXATTRIB4NBVPROC) GL::getProcAddress("glVertexAttrib4Nbv");
    glVertexAttrib4Niv		= (PFNGLVERTEXATTRIB4NIVPROC) GL::getProcAddress("glVertexAttrib4Niv");
    glVertexAttrib4Nsv		= (PFNGLVERTEXATTRIB4NSVPROC) GL::getProcAddress("glVertexAttrib4Nsv");
    glVertexAttrib4Nub		= (PFNGLVERTEXATTRIB4NUBPROC) GL::getProcAddress("glVertexAttrib4Nub");
    glVertexAttrib4Nubv		= (PFNGLVERTEXATTRIB4NUBVPROC) GL::getProcAddress("glVertexAttrib4Nubv");
    glVertexAttrib4Nuiv		= (PFNGLVERTEXATTRIB4NUIVPROC) GL::getProcAddress("glVertexAttrib4Nuiv");
    glVertexAttrib4Nusv		= (PFNGLVERTEXATTRIB4NUSVPROC) GL::getProcAddress("glVertexAttrib4Nusv");
    glVertexAttrib4bv		= (PFNGLVERTEXATTRIB4BVPROC) GL::getProcAddress("glVertexAttrib4bv");
    glVertexAttrib4d		= (PFNGLVERTEXATTRIB4DPROC) GL::getProcAddress("glVertexAttrib4d");
    glVertexAttrib4dv		= (PFNGLVERTEXATTRIB4DVPROC) GL::getProcAddress("glVertexAttrib4dv");
    glVertexAttrib4f		= (PFNGLVERTEXATTRIB4FPROC) GL::getProcAddress("glVertexAttrib4f");
    glVertexAttrib4fv		= (PFNGLVERTEXATTRIB4FVPROC) GL::getProcAddress("glVertexAttrib4fv");
    glVertexAttrib4iv		= (PFNGLVERTEXATTRIB4IVPROC) GL::getProcAddress("glVertexAttrib4iv");
    glVertexAttrib4s		= (PFNGLVERTEXATTRIB4SPROC) GL::getProcAddress("glVertexAttrib4s");
    glVertexAttrib4sv		= (PFNGLVERTEXATTRIB4SVPROC) GL::getProcAddress("glVertexAttrib4sv");
    glVertexAttrib4ubv		= (PFNGLVERTEXATTRIB4UBVPROC) GL::getProcAddress("glVertexAttrib4ubv");
    glVertexAttrib4uiv		= (PFNGLVERTEXATTRIB4UIVPROC) GL::getProcAddress("glVertexAttrib4uiv");
    glVertexAttrib4usv		= (PFNGLVERTEXATTRIB4USVPROC) GL::getProcAddress("glVertexAttrib4usv");
    glVertexAttribPointer	= (PFNGLVERTEXATTRIBPOINTERPROC) GL::getProcAddress("glVertexAttribPointer");

    if ((!glCreateShader || !glShaderSource || !glCompileShader || !glCreateProgram  ||
        !glAttachShader || !glLinkProgram || !glValidateProgram || !glUseProgram  ||
        !glDetachShader || !glDeleteShader || !glDeleteProgram || !glGetProgramInfoLog  ||
        !glGetShaderInfoLog || !glIsProgram || !glIsShader || !glGetUniformLocation  ||
        !glGetProgramiv || !glUniform1f || !glUniform2f || !glUniform3f || !glUniform4f  ||
        !glUniform1fv || !glUniform2fv || !glUniform3fv || !glUniform4fv  ||
        !glUniform1i || !glUniform2i || !glUniform3i || !glUniform4i  ||
        !glUniformMatrix2fv || !glUniformMatrix3fv || !glUniformMatrix4fv  ||
        !glGetShaderiv || !glGetActiveUniform  ||
        !glBindAttribLocation || !glGetAttribLocation || !glGetVertexAttribdv || !glGetVertexAttribfv  ||
        !glGetVertexAttribiv || !glGetVertexAttribPointerv || !glVertexAttrib1d || !glVertexAttrib1dv  ||
        !glVertexAttrib1f || !glVertexAttrib1fv || !glVertexAttrib1s || !glVertexAttrib1sv  ||
        !glVertexAttrib2d || !glVertexAttrib2dv || !glVertexAttrib2f || !glVertexAttrib2fv  ||
        !glVertexAttrib2s || !glVertexAttrib2sv || !glVertexAttrib3d || !glVertexAttrib3dv  ||
        !glVertexAttrib3f || !glVertexAttrib3fv || !glVertexAttrib3s || !glVertexAttrib3sv  ||
        !glVertexAttrib4Nbv || !glVertexAttrib4Niv || !glVertexAttrib4Nsv || !glVertexAttrib4Nub  ||
        !glVertexAttrib4Nubv || !glVertexAttrib4Nuiv || !glVertexAttrib4Nusv || !glVertexAttrib4bv  ||
        !glVertexAttrib4d || !glVertexAttrib4dv || !glVertexAttrib4f || !glVertexAttrib4fv  ||
        !glVertexAttrib4iv || !glVertexAttrib4s || !glVertexAttrib4sv || !glVertexAttrib4ubv  ||
        !glVertexAttrib4uiv || !glVertexAttrib4usv || !glVertexAttribPointer) &&
        isExtensionSupported("GL_ARB_shading_language_100")) {

        glCreateShader		= (PFNGLCREATESHADERPROC) GL::getProcAddress("glCreateShaderObjectARB");
        glShaderSource		= (PFNGLSHADERSOURCEPROC) GL::getProcAddress("glShaderSourceARB");
        glCompileShader		= (PFNGLCOMPILESHADERPROC) GL::getProcAddress("glCompileShaderARB");
        glCreateProgram		= (PFNGLCREATEPROGRAMPROC) GL::getProcAddress("glCreateProgramObjectARB");
        glAttachShader		= (PFNGLATTACHSHADERPROC) GL::getProcAddress("glAttachObjectARB");
        glLinkProgram		= (PFNGLLINKPROGRAMPROC) GL::getProcAddress("glLinkProgramARB");
        glValidateProgram	= (PFNGLVALIDATEPROGRAMPROC)GL::getProcAddress("glValidateProgramARB");
        glUseProgram		= (PFNGLUSEPROGRAMPROC) GL::getProcAddress("glUseProgramObjectARB");

        glDetachShader		= (PFNGLDETACHSHADERPROC) GL::getProcAddress("glDetachObjectARB");
        glDeleteShader		= (PFNGLDELETESHADERPROC) GL::getProcAddress("glDeleteObjectARB");
        glDeleteProgram		= (PFNGLDELETEPROGRAMPROC) GL::getProcAddress("glDeleteObjectARB");

        glGetProgramiv		= (PFNGLGETPROGRAMIVPROC) GL::getProcAddress("glGetObjectParameterivARB");
        glGetProgramInfoLog	= (PFNGLGETPROGRAMINFOLOGPROC) GL::getProcAddress("glGetInfoLogARB");
        glGetShaderiv		= (PFNGLGETSHADERIVPROC) GL::getProcAddress("glGetObjectParameterivARB");
        glGetShaderInfoLog	= (PFNGLGETSHADERINFOLOGPROC) GL::getProcAddress("glGetInfoLogARB");
        glIsProgram		= (PFNGLISPROGRAMPROC) GL::getProcAddress("glIsProgramARB"); // Not sure
        glIsShader		= (PFNGLISSHADERPROC) GL::getProcAddress("glIsProgramARB"); // Not sure

        glGetUniformLocation    = (PFNGLGETUNIFORMLOCATIONPROC) GL::getProcAddress("glGetUniformLocationARB");
        glGetActiveUniform      = (PFNGLGETACTIVEUNIFORMPROC) GL::getProcAddress("glGetActiveUniformARB");
        glUniform1f		= (PFNGLUNIFORM1FPROC) GL::getProcAddress("glUniform1fARB");
        glUniform2f		= (PFNGLUNIFORM2FPROC) GL::getProcAddress("glUniform2fARB");
        glUniform3f		= (PFNGLUNIFORM3FPROC) GL::getProcAddress("glUniform3fARB");
        glUniform4f		= (PFNGLUNIFORM4FPROC) GL::getProcAddress("glUniform4fARB");
        glUniform1fv	= (PFNGLUNIFORM1FVPROC) GL::getProcAddress("glUniform1fvARB");
        glUniform2fv	= (PFNGLUNIFORM2FVPROC) GL::getProcAddress("glUniform2fvARB");
        glUniform3fv	= (PFNGLUNIFORM3FVPROC) GL::getProcAddress("glUniform3fvARB");
        glUniform4fv	= (PFNGLUNIFORM4FVPROC) GL::getProcAddress("glUniform4fvARB");
        glUniform1i		= (PFNGLUNIFORM1IPROC) GL::getProcAddress("glUniform1iARB");
        glUniform2i		= (PFNGLUNIFORM2IPROC) GL::getProcAddress("glUniform2iARB");
        glUniform3i		= (PFNGLUNIFORM3IPROC) GL::getProcAddress("glUniform3iARB");
        glUniform4i		= (PFNGLUNIFORM4IPROC) GL::getProcAddress("glUniform4iARB");
        glUniformMatrix2fv	= (PFNGLUNIFORMMATRIX2FVPROC) GL::getProcAddress("glUniformMatrix2fvARB");
        glUniformMatrix3fv	= (PFNGLUNIFORMMATRIX3FVPROC) GL::getProcAddress("glUniformMatrix3fvARB");
        glUniformMatrix4fv	= (PFNGLUNIFORMMATRIX4FVPROC) GL::getProcAddress("glUniformMatrix4fvARB");

        glDisableVertexAttribArray	= (PFNGLDISABLEVERTEXATTRIBARRAYPROC) GL::getProcAddress("glDisableVertexAttribArrayARB");
        glEnableVertexAttribArray	= (PFNGLENABLEVERTEXATTRIBARRAYPROC) GL::getProcAddress("glEnableVertexAttribArrayARB");
        glGetActiveAttrib		= (PFNGLGETACTIVEATTRIBPROC) GL::getProcAddress("glGetActiveAttribARB");
        glBindAttribLocation		= (PFNGLBINDATTRIBLOCATIONPROC) GL::getProcAddress("glBindAttribLocationARB");
        glGetAttribLocation		= (PFNGLGETATTRIBLOCATIONPROC) GL::getProcAddress("glGetAttribLocationARB");
        glGetVertexAttribdv		= (PFNGLGETVERTEXATTRIBDVPROC) GL::getProcAddress("glGetVertexAttribdvARB");
        glGetVertexAttribfv		= (PFNGLGETVERTEXATTRIBFVPROC) GL::getProcAddress("glGetVertexAttribfvARB");
        glGetVertexAttribiv		= (PFNGLGETVERTEXATTRIBIVPROC) GL::getProcAddress("glGetVertexAttribivARB");
        glGetVertexAttribPointerv	= (PFNGLGETVERTEXATTRIBPOINTERVPROC) GL::getProcAddress("glGetVertexAttribPointervARB");

        glVertexAttrib1d		= (PFNGLVERTEXATTRIB1DPROC) GL::getProcAddress("glVertexAttrib1dARB");
        glVertexAttrib1dv		= (PFNGLVERTEXATTRIB1DVPROC) GL::getProcAddress("glVertexAttrib1dvARB");
        glVertexAttrib1f		= (PFNGLVERTEXATTRIB1FPROC) GL::getProcAddress("glVertexAttrib1fARB");
        glVertexAttrib1fv		= (PFNGLVERTEXATTRIB1FVPROC) GL::getProcAddress("glVertexAttrib1fvARB");
        glVertexAttrib1s		= (PFNGLVERTEXATTRIB1SPROC) GL::getProcAddress("glVertexAttrib1sARB");
        glVertexAttrib1sv		= (PFNGLVERTEXATTRIB1SVPROC) GL::getProcAddress("glVertexAttrib1svARB");
        glVertexAttrib2d		= (PFNGLVERTEXATTRIB2DPROC) GL::getProcAddress("glVertexAttrib2dARB");
        glVertexAttrib2dv		= (PFNGLVERTEXATTRIB2DVPROC) GL::getProcAddress("glVertexAttrib2dvARB");
        glVertexAttrib2f		= (PFNGLVERTEXATTRIB2FPROC) GL::getProcAddress("glVertexAttrib2fARB");
        glVertexAttrib2fv		= (PFNGLVERTEXATTRIB2FVPROC) GL::getProcAddress("glVertexAttrib2fvARB");
        glVertexAttrib2s		= (PFNGLVERTEXATTRIB2SPROC) GL::getProcAddress("glVertexAttrib2sARB");
        glVertexAttrib2sv		= (PFNGLVERTEXATTRIB2SVPROC) GL::getProcAddress("glVertexAttrib2svARB");
        glVertexAttrib3d		= (PFNGLVERTEXATTRIB3DPROC) GL::getProcAddress("glVertexAttrib3dARB");
        glVertexAttrib3dv		= (PFNGLVERTEXATTRIB3DVPROC) GL::getProcAddress("glVertexAttrib3dvARB");
        glVertexAttrib3f		= (PFNGLVERTEXATTRIB3FPROC) GL::getProcAddress("glVertexAttrib3fARB");
        glVertexAttrib3fv		= (PFNGLVERTEXATTRIB3FVPROC) GL::getProcAddress("glVertexAttrib3fvARB");
        glVertexAttrib3s		= (PFNGLVERTEXATTRIB3SPROC) GL::getProcAddress("glVertexAttrib3sARB");
        glVertexAttrib3sv		= (PFNGLVERTEXATTRIB3SVPROC) GL::getProcAddress("glVertexAttrib3svARB");
        glVertexAttrib4Nbv		= (PFNGLVERTEXATTRIB4NBVPROC) GL::getProcAddress("glVertexAttrib4NbvARB");
        glVertexAttrib4Niv		= (PFNGLVERTEXATTRIB4NIVPROC) GL::getProcAddress("glVertexAttrib4NivARB");
        glVertexAttrib4Nsv		= (PFNGLVERTEXATTRIB4NSVPROC) GL::getProcAddress("glVertexAttrib4NsvARB");
        glVertexAttrib4Nub		= (PFNGLVERTEXATTRIB4NUBPROC) GL::getProcAddress("glVertexAttrib4NubARB");
        glVertexAttrib4Nubv		= (PFNGLVERTEXATTRIB4NUBVPROC) GL::getProcAddress("glVertexAttrib4NubvARB");
        glVertexAttrib4Nuiv		= (PFNGLVERTEXATTRIB4NUIVPROC) GL::getProcAddress("glVertexAttrib4NuivARB");
        glVertexAttrib4Nusv		= (PFNGLVERTEXATTRIB4NUSVPROC) GL::getProcAddress("glVertexAttrib4NusvARB");
        glVertexAttrib4bv		= (PFNGLVERTEXATTRIB4BVPROC) GL::getProcAddress("glVertexAttrib4bvARB");
        glVertexAttrib4d		= (PFNGLVERTEXATTRIB4DPROC) GL::getProcAddress("glVertexAttrib4dARB");
        glVertexAttrib4dv		= (PFNGLVERTEXATTRIB4DVPROC) GL::getProcAddress("glVertexAttrib4dvARB");
        glVertexAttrib4f		= (PFNGLVERTEXATTRIB4FPROC) GL::getProcAddress("glVertexAttrib4fARB");
        glVertexAttrib4fv		= (PFNGLVERTEXATTRIB4FVPROC) GL::getProcAddress("glVertexAttrib4fvARB");
        glVertexAttrib4iv		= (PFNGLVERTEXATTRIB4IVPROC) GL::getProcAddress("glVertexAttrib4ivARB");
        glVertexAttrib4s		= (PFNGLVERTEXATTRIB4SPROC) GL::getProcAddress("glVertexAttrib4sARB");
        glVertexAttrib4sv		= (PFNGLVERTEXATTRIB4SVPROC) GL::getProcAddress("glVertexAttrib4svARB");
        glVertexAttrib4ubv		= (PFNGLVERTEXATTRIB4UBVPROC) GL::getProcAddress("glVertexAttrib4ubvARB");
        glVertexAttrib4uiv		= (PFNGLVERTEXATTRIB4UIVPROC) GL::getProcAddress("glVertexAttrib4uivARB");
        glVertexAttrib4usv		= (PFNGLVERTEXATTRIB4USVPROC) GL::getProcAddress("glVertexAttrib4usvARB");
        glVertexAttribPointer	= (PFNGLVERTEXATTRIBPOINTERPROC)GL::getProcAddress("glVertexAttribPointerARB");
    }

    if (!glCreateShader || !glShaderSource || !glCompileShader || !glCreateProgram  ||
        !glAttachShader || !glLinkProgram || !glValidateProgram || !glUseProgram  ||
        !glDetachShader || !glDeleteShader || !glDeleteProgram || !glGetProgramInfoLog  ||
        !glGetShaderInfoLog || !glIsProgram || !glIsShader || !glGetUniformLocation  ||
        !glGetProgramiv || !glUniform1f || !glUniform2f || !glUniform3f || !glUniform4f  ||
        !glUniform1fv || !glUniform2fv || !glUniform3fv || !glUniform4fv  ||
        !glUniform1i || !glUniform2i || !glUniform3i || !glUniform4i  ||
        !glUniformMatrix2fv || !glUniformMatrix3fv || !glUniformMatrix4fv  ||
        !glGetShaderiv || !glGetActiveUniform  ||
        !glBindAttribLocation || !glGetAttribLocation || !glGetVertexAttribdv || !glGetVertexAttribfv  ||
        !glGetVertexAttribiv || !glGetVertexAttribPointerv || !glVertexAttrib1d || !glVertexAttrib1dv  ||
        !glVertexAttrib1f || !glVertexAttrib1fv || !glVertexAttrib1s || !glVertexAttrib1sv  ||
        !glVertexAttrib2d || !glVertexAttrib2dv || !glVertexAttrib2f || !glVertexAttrib2fv  ||
        !glVertexAttrib2s || !glVertexAttrib2sv || !glVertexAttrib3d || !glVertexAttrib3dv  ||
        !glVertexAttrib3f || !glVertexAttrib3fv || !glVertexAttrib3s || !glVertexAttrib3sv  ||
        !glVertexAttrib4Nbv || !glVertexAttrib4Niv || !glVertexAttrib4Nsv || !glVertexAttrib4Nub  ||
        !glVertexAttrib4Nubv || !glVertexAttrib4Nuiv || !glVertexAttrib4Nusv || !glVertexAttrib4bv  ||
        !glVertexAttrib4d || !glVertexAttrib4dv || !glVertexAttrib4f || !glVertexAttrib4fv  ||
        !glVertexAttrib4iv || !glVertexAttrib4s || !glVertexAttrib4sv || !glVertexAttrib4ubv  ||
        !glVertexAttrib4uiv || !glVertexAttrib4usv || !glVertexAttribPointer) {

        O3D_ERROR(E_UnsuportedFeature("OpenGL GLSL"));
	}

    //
    // Blend equation separate
    //

    glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC) GL::getProcAddress("glBlendEquationSeparate");
    if (!glBlendEquationSeparate) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Blend equation separate"));
    }

	//
	// Draw buffers
	//

    glDrawBuffers = (PFNGLDRAWBUFFERSPROC) GL::getProcAddress("glDrawBuffers");
    if (!glDrawBuffers && isExtensionSupported("GL_ARB_draw_buffers")) {
        glDrawBuffers = (PFNGLDRAWBUFFERSPROC) GL::getProcAddress("glDrawBuffersARB");
    }

    if (!glDrawBuffers) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Draw buffers"));
    }

    //
    // Stencil separate
    //

    glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC) GL::getProcAddress("glStencilOpSeparate");
    glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC) GL::getProcAddress("glStencilFuncSeparate");
    glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC) GL::getProcAddress("glStencilMaskSeparate");

    if (!glStencilOpSeparate || !glStencilFuncSeparate || !glStencilMaskSeparate) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL stencil separate functions"));
    }

#endif // O3D_GL_VERSION_2_0

#ifdef O3D_GL_ARB_framebuffer_object

	//
	// Frame Buffer Object
	//

    glIsRenderbuffer 		= (PFNGLISRENDERBUFFERPROC) GL::getProcAddress("glIsRenderbuffer");
    glBindRenderbuffer 		= (PFNGLBINDRENDERBUFFERPROC) GL::getProcAddress("glBindRenderbuffer");
    glDeleteRenderbuffers 	= (PFNGLDELETERENDERBUFFERSPROC) GL::getProcAddress("glDeleteRenderbuffers");
    glGenRenderbuffers 		= (PFNGLGENRENDERBUFFERSPROC) GL::getProcAddress("glGenRenderbuffers");
    glRenderbufferStorage 	= (PFNGLRENDERBUFFERSTORAGEPROC) GL::getProcAddress("glRenderbufferStorage");
    glGetRenderbufferParameteriv    = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) GL::getProcAddress("glGetRenderbufferParameteriv");
    glIsFramebuffer 		= (PFNGLISFRAMEBUFFERPROC) GL::getProcAddress("glIsFramebuffer");
    glBindFramebuffer 		= (PFNGLBINDFRAMEBUFFERPROC) GL::getProcAddress("glBindFramebuffer");
    glDeleteFramebuffers 	= (PFNGLDELETEFRAMEBUFFERSPROC) GL::getProcAddress("glDeleteFramebuffers");
    glGenFramebuffers 		= (PFNGLGENFRAMEBUFFERSPROC) GL::getProcAddress("glGenFramebuffers");
    glCheckFramebufferStatus 	= (PFNGLCHECKFRAMEBUFFERSTATUSPROC) GL::getProcAddress("glCheckFramebufferStatus");
    glFramebufferTexture1D 		= (PFNGLFRAMEBUFFERTEXTURE1DPROC) GL::getProcAddress("glFramebufferTexture1D");
    glFramebufferTexture2D 		= (PFNGLFRAMEBUFFERTEXTURE2DPROC) GL::getProcAddress("glFramebufferTexture2D");
    glFramebufferTexture3D 		= (PFNGLFRAMEBUFFERTEXTURE3DPROC) GL::getProcAddress("glFramebufferTexture3D");
    glFramebufferRenderbuffer 	= (PFNGLFRAMEBUFFERRENDERBUFFERPROC) GL::getProcAddress("glFramebufferRenderbuffer");
    glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) GL::getProcAddress("glGetFramebufferAttachmentParameteriv");
    glBlitFramebuffer 		= (PFNGLBLITFRAMEBUFFERPROC) GL::getProcAddress("glBlitFramebuffer");
    glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) GL::getProcAddress("glRenderbufferStorageMultisample");
    glGenerateMipmap 		= (PFNGLGENERATEMIPMAPPROC) GL::getProcAddress("glGenerateMipmap");
    glFramebufferTextureLayer   = (PFNGLFRAMEBUFFERTEXTURELAYERPROC) GL::getProcAddress("glFramebufferTextureLayer");

    if ((!glIsRenderbuffer || !glBindRenderbuffer || !glDeleteRenderbuffers ||
        !glGenRenderbuffers || !glRenderbufferStorage || !glGetRenderbufferParameteriv ||
        !glIsFramebuffer || !glBindFramebuffer || !glDeleteFramebuffers ||
        !glGenFramebuffers || !glCheckFramebufferStatus || !glFramebufferTexture1D ||
        !glFramebufferTexture2D || !glFramebufferTexture3D || !glFramebufferRenderbuffer ||
        !glGetFramebufferAttachmentParameteriv || !glBlitFramebuffer ||
        !glRenderbufferStorageMultisample || !glGenerateMipmap ||!glFramebufferTextureLayer) &&
        isExtensionSupported("GL_ARB_framebuffer_object")) {

        glIsRenderbuffer 		= (PFNGLISRENDERBUFFERPROC) GL::getProcAddress("glIsRenderbufferARB");
        glBindRenderbuffer 		= (PFNGLBINDRENDERBUFFERPROC) GL::getProcAddress("glBindRenderbufferARB");
        glDeleteRenderbuffers 	= (PFNGLDELETERENDERBUFFERSPROC) GL::getProcAddress("glDeleteRenderbuffersARB");
        glGenRenderbuffers 		= (PFNGLGENRENDERBUFFERSPROC) GL::getProcAddress("glGenRenderbuffersARB");
        glRenderbufferStorage 	= (PFNGLRENDERBUFFERSTORAGEPROC) GL::getProcAddress("glRenderbufferStorageARB");
        glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) GL::getProcAddress("glGetRenderbufferParameterivARB");
        glIsFramebuffer 		= (PFNGLISFRAMEBUFFERPROC) GL::getProcAddress("glIsFramebufferARB");
        glBindFramebuffer 		= (PFNGLBINDFRAMEBUFFERPROC) GL::getProcAddress("glBindFramebufferARB");
        glDeleteFramebuffers 	= (PFNGLDELETEFRAMEBUFFERSPROC) GL::getProcAddress("glDeleteFramebuffersARB");
        glGenFramebuffers 		= (PFNGLGENFRAMEBUFFERSPROC) GL::getProcAddress("glGenFramebuffersARB");
        glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) GL::getProcAddress("glCheckFramebufferStatusARB");
        glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC) GL::getProcAddress("glFramebufferTexture1DARB");
        glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) GL::getProcAddress("glFramebufferTexture2DARB");
        glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) GL::getProcAddress("glFramebufferTexture3DARB");
        glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) GL::getProcAddress("glFramebufferRenderbufferARB");
        glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) GL::getProcAddress("glGetFramebufferAttachmentParameterivARB");
        glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC) GL::getProcAddress("glBlitFramebufferARB");
        glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) GL::getProcAddress("glRenderbufferStorageMultisampleARB");
        glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) GL::getProcAddress("glGenerateMipmapARB");
        glFramebufferTextureLayer  = (PFNGLFRAMEBUFFERTEXTURELAYERPROC) GL::getProcAddress("glFramebufferTextureLayerARB");
    }

    if ((!glIsRenderbuffer || !glBindRenderbuffer || !glDeleteRenderbuffers ||
        !glGenRenderbuffers || !glRenderbufferStorage || !glGetRenderbufferParameteriv ||
        !glIsFramebuffer || !glBindFramebuffer || !glDeleteFramebuffers ||
        !glGenFramebuffers || !glCheckFramebufferStatus || !glFramebufferTexture1D ||
        !glFramebufferTexture2D || !glFramebufferTexture3D || !glFramebufferRenderbuffer ||
        !glGetFramebufferAttachmentParameteriv ||
        !glRenderbufferStorageMultisample || !glGenerateMipmap ||!glFramebufferTextureLayer) &&
        isExtensionSupported("GL_EXT_framebuffer_object")) {

        glIsRenderbuffer 	= (PFNGLISRENDERBUFFERPROC) GL::getProcAddress("glIsRenderbufferEXT");
        glBindRenderbuffer 	= (PFNGLBINDRENDERBUFFERPROC) GL::getProcAddress("glBindRenderbufferEXT");
        glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) GL::getProcAddress("glDeleteRenderbuffersEXT");
        glGenRenderbuffers 	= (PFNGLGENRENDERBUFFERSPROC) GL::getProcAddress("glGenRenderbuffersEXT");
        glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) GL::getProcAddress("glRenderbufferStorageEXT");
        glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) GL::getProcAddress("glGetRenderbufferParameterivEXT");
        glIsFramebuffer 	= (PFNGLISFRAMEBUFFERPROC) GL::getProcAddress("glIsFramebufferEXT");
        glBindFramebuffer 	= (PFNGLBINDFRAMEBUFFERPROC) GL::getProcAddress("glBindFramebufferEXT");
        glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) GL::getProcAddress("glDeleteFramebuffersEXT");
        glGenFramebuffers 	= (PFNGLGENFRAMEBUFFERSPROC) GL::getProcAddress("glGenFramebuffersEXT");
        glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) GL::getProcAddress("glCheckFramebufferStatusEXT");
        glFramebufferTexture1D 	= (PFNGLFRAMEBUFFERTEXTURE1DPROC) GL::getProcAddress("glFramebufferTexture1DEXT");
        glFramebufferTexture2D 	= (PFNGLFRAMEBUFFERTEXTURE2DPROC) GL::getProcAddress("glFramebufferTexture2DEXT");
        glFramebufferTexture3D 	= (PFNGLFRAMEBUFFERTEXTURE3DPROC) GL::getProcAddress("glFramebufferTexture3DEXT");
        glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) GL::getProcAddress("glFramebufferRenderbufferEXT");
        glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) GL::getProcAddress("glGetFramebufferAttachmentParameterivEXT");
        glGenerateMipmap 	= (PFNGLGENERATEMIPMAPPROC) GL::getProcAddress("glGenerateMipmapEXT");
    }

    if (!glBlitFramebuffer && isExtensionSupported("GL_EXT_framebuffer_blit")) {
        glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC) GL::getProcAddress("glBlitFramebufferEXT");
    }

    if (!glIsRenderbuffer || !glBindRenderbuffer || !glDeleteRenderbuffers ||
        !glGenRenderbuffers || !glRenderbufferStorage || !glGetRenderbufferParameteriv ||
        !glIsFramebuffer || !glBindFramebuffer || !glDeleteFramebuffers ||
        !glGenFramebuffers || !glCheckFramebufferStatus || !glFramebufferTexture1D ||
        !glFramebufferTexture2D || !glFramebufferTexture3D || !glFramebufferRenderbuffer ||
        !glGetFramebufferAttachmentParameteriv ||
        !glRenderbufferStorageMultisample || !glGenerateMipmap ||!glFramebufferTextureLayer) {

        O3D_ERROR(E_UnsuportedFeature("OpenGL FBO"));
    }

    if (!glBlitFramebuffer) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL FBO blitting"));
    }
#endif // O3D_GL_ARB_framebuffer_object

	//
	// Check for ARB_texture_float
	//

    if (!isExtensionSupported("GL_ARB_texture_float")) {
        O3D_ERROR(E_UnsuportedFeature("GL_ARB_texture_float"));
    }

#ifdef O3D_GL_VERSION_3_0

	//
	// OpenGL 3.0
	//

    glColorMaski = (PFNGLCOLORMASKIPROC) GL::getProcAddress("glColorMaski");
    glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC) GL::getProcAddress("glGetBooleani_v");
    glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC) GL::getProcAddress("glGetIntegeri_v");
    glEnablei = (PFNGLENABLEIPROC) GL::getProcAddress("glEnablei");
    glDisablei = (PFNGLDISABLEIPROC) GL::getProcAddress("glDisablei");
    glIsEnabledi = (PFNGLISENABLEDIPROC) GL::getProcAddress("glIsEnabledi");
    glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC) GL::getProcAddress("glBeginTransformFeedback");
    glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC) GL::getProcAddress("glEndTransformFeedback");
    glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC) GL::getProcAddress("glBindBufferRange");
    glBindBufferBase = (PFNGLBINDBUFFERBASEPROC) GL::getProcAddress("glBindBufferBase");
    glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC) GL::getProcAddress("glTransformFeedbackVaryings");
    glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC) GL::getProcAddress("glGetTransformFeedbackVarying");
    glClampColor = (PFNGLCLAMPCOLORPROC) GL::getProcAddress("glClampColor");
    glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC) GL::getProcAddress("glBeginConditionalRender");
    glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC) GL::getProcAddress("glEndConditionalRender");
    glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC) GL::getProcAddress("glVertexAttribIPointer");
    glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC) GL::getProcAddress("glGetVertexAttribIiv");
    glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC) GL::getProcAddress("glGetVertexAttribIuiv");
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
    glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC) GL::getProcAddress("glGetUniformuiv");
    glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC) GL::getProcAddress("glBindFragDataLocation");
    glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC) GL::getProcAddress("glGetFragDataLocation");
    glUniform1ui = (PFNGLUNIFORM1UIPROC) GL::getProcAddress("glUniform1ui");
    glUniform2ui = (PFNGLUNIFORM2UIPROC) GL::getProcAddress("glUniform2ui");
    glUniform3ui = (PFNGLUNIFORM3UIPROC) GL::getProcAddress("glUniform3ui");
    glUniform4ui = (PFNGLUNIFORM4UIPROC) GL::getProcAddress("glUniform4ui");
    glUniform1uiv = (PFNGLUNIFORM1UIVPROC) GL::getProcAddress("glUniform1uiv");
    glUniform2uiv = (PFNGLUNIFORM2UIVPROC) GL::getProcAddress("glUniform2uiv");
    glUniform3uiv = (PFNGLUNIFORM3UIVPROC) GL::getProcAddress("glUniform3uiv");
    glUniform4uiv = (PFNGLUNIFORM4UIPROC) GL::getProcAddress("glUniform4uiv");
    glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC) GL::getProcAddress("glTexParameterIiv");
    glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC) GL::getProcAddress("glTexParameterIuiv");
    glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC) GL::getProcAddress("glGetTexParameterIiv");
    glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC) GL::getProcAddress("glGetTexParameterIuiv");
    glClearBufferiv = (PFNGLCLEARBUFFERIVPROC) GL::getProcAddress("glClearBufferiv");
    glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC) GL::getProcAddress("glClearBufferuiv");
    glClearBufferfv = (PFNGLCLEARBUFFERFVPROC) GL::getProcAddress("glClearBufferfv");
    glClearBufferfi = (PFNGLCLEARBUFFERFIPROC) GL::getProcAddress("glClearBufferfi");

    if ((!glColorMaski || !glGetBooleani_v || !glGetIntegeri_v ||
        !glEnablei || !glDisablei || !glIsEnabledi ||
        !glBeginTransformFeedback || !glEndTransformFeedback ||
        !glBindBufferRange || !glBindBufferBase ||
        !glTransformFeedbackVaryings || !glGetTransformFeedbackVarying ||
        !glClampColor ||
        !glBeginConditionalRender || !glEndConditionalRender ||
        !glVertexAttribIPointer || !glGetVertexAttribIiv || !glGetVertexAttribIuiv ||
        !glVertexAttribI1i || !glVertexAttribI2i || !glVertexAttribI3i || !glVertexAttribI4i ||
        !glVertexAttribI1ui || !glVertexAttribI2ui || !glVertexAttribI3ui || !glVertexAttribI4ui ||
        !glVertexAttribI1iv || !glVertexAttribI2iv || !glVertexAttribI3iv || !glVertexAttribI4iv ||
        !glVertexAttribI1uiv || !glVertexAttribI2uiv || !glVertexAttribI3uiv || !glVertexAttribI4uiv ||
        !glVertexAttribI4bv || !glVertexAttribI4sv || !glVertexAttribI4ubv || !glVertexAttribI4usv ||
        !glGetUniformuiv || !glBindFragDataLocation || !glGetFragDataLocation ||
        !glUniform1ui || !glUniform2ui || !glUniform3ui || !glUniform4ui ||
        !glUniform1uiv || !glUniform2uiv || !glUniform3uiv || !glUniform4uiv ||
        !glTexParameterIiv || !glTexParameterIuiv ||
        !glGetTexParameterIiv || !glGetTexParameterIuiv ||
        !glClearBufferiv || !glClearBufferuiv || !glClearBufferfv ||!glClearBufferfi)) {

        O3D_ERROR(E_UnsuportedFeature("OpenGL 3.0 related functions"));
	}

#endif // O3D_GL_VERSION_3_0

#ifdef O3D_GL_VERSION_3_1

    glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC) GL::getProcAddress("glDrawArraysInstanced");
    glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC) GL::getProcAddress("");
    glTexBuffer = (PFNGLTEXBUFFERPROC) GL::getProcAddress("glTexBuffer");
    glPrimitiveRestartIndex = (PFNGLPRIMITIVERESTARTINDEXPROC) GL::getProcAddress("glPrimitiveRestartIndex");
    glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC) GL::getProcAddress("glCopyBufferSubData");
    glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC) GL::getProcAddress("glGetUniformIndices");
    glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC) GL::getProcAddress("glGetActiveUniformsiv");
    glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC) GL::getProcAddress("glGetActiveUniformName");
    glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC) GL::getProcAddress("glGetUniformBlockIndex");
    glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC) GL::getProcAddress("glGetActiveUniformBlockiv");
    glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC) GL::getProcAddress("glGetActiveUniformBlockName");
    glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC) GL::getProcAddress("glUniformBlockBinding");

    if (!glDrawArraysInstanced || !glDrawElementsInstanced) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Draw instanced"));
    }

    if (!glTexBuffer) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Tex buffer"));
    }

    if (!glPrimitiveRestartIndex) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Primitive restart index"));
    }

    if (!glCopyBufferSubData) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Copy buffer sub-data"));
    }

    if (!glGetUniformIndices || !glGetActiveUniformsiv || !glGetActiveUniformName ||
        !glGetUniformBlockIndex || !glGetActiveUniformBlockiv || !glGetActiveUniformBlockName ||
        !glUniformBlockBinding) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Uniform Buffer Object"));
    }

#endif // O3D_GL_VERSION_3_1

#ifdef O3D_GL_VERSION_3_2

	//
	// OpenGL 3.2 and geometry shader
	//

    //glGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC) GL::getProcAddress("glGetInteger64i_v");
    //glGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC) GL::getProcAddress("glGetBufferParameteri64v");
    glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC) GL::getProcAddress("glProgramParameteri");
    glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC) GL::getProcAddress("glFramebufferTexture");
    glFramebufferTextureFace = (PFNGLFRAMEBUFFERTEXTUREFACEPROC) GL::getProcAddress("glFramebufferTextureFace");

    if (!glProgramParameteri || !glFramebufferTexture || !glFramebufferTextureFace) {
        glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC) GL::getProcAddress("glProgramParameteriARB");
        glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC) GL::getProcAddress("glFramebufferTextureARB");
        glFramebufferTextureFace = (PFNGLFRAMEBUFFERTEXTUREFACEPROC) GL::getProcAddress("glFramebufferTextureFaceARB");
    }

    if (!glProgramParameteri || !glFramebufferTexture || !glFramebufferTextureFace) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL 3.2 related functions"));
    }

	//
	// GL_ARB_vertex_array_object
	//

    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) GL::getProcAddress("glBindVertexArray");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) GL::getProcAddress("glDeleteVertexArrays");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) GL::getProcAddress("glGenVertexArrays");
    glIsVertexArray = (PFNGLISVERTEXARRAYPROC) GL::getProcAddress("glIsVertexArray");

    if (!glBindVertexArray || !glDeleteVertexArrays || !glGenVertexArrays || !glIsVertexArray) {
        O3D_WARNING("OpenGL VAO is not available");
    }

#endif // O3D_GL_VERSION_3_2

#ifdef O3D_GL_VERSION_3_3

    //
    // OpenGL 3.2 and sampler object
    //

    glGenSamplers = (PFNGLGENSAMPLERSPROC) GL::getProcAddress("glGenSamplers");
    glDeleteSamplers = (PFNGLDELETESAMPLERSPROC) GL::getProcAddress("glDeleteSamplers");
    glIsSampler = (PFNGLISSAMPLERPROC) GL::getProcAddress("glIsSampler");
    glBindSampler = (PFNGLBINDSAMPLERPROC) GL::getProcAddress("glBindSampler");
    glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC) GL::getProcAddress("glSamplerParameteri");
    glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC) GL::getProcAddress("glSamplerParameteriv");
    glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC) GL::getProcAddress("glSamplerParameterf");
    glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC) GL::getProcAddress("glSamplerParameterfv");
    glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC) GL::getProcAddress("glSamplerParameterIiv");
    glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC) GL::getProcAddress("glSamplerParameterIuiv");
    glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC) GL::getProcAddress("glGetSamplerParameteriv");
    glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC) GL::getProcAddress("glGetSamplerParameterIiv");
    glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC) GL::getProcAddress("glGetSamplerParameterfv");
    glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC) GL::getProcAddress("glGetSamplerParameterIuiv");

    if (!glGenSamplers || !glDeleteSamplers || !glIsSampler || !glBindSampler ||
        !glSamplerParameteri || !glSamplerParameteriv ||
        !glSamplerParameterf || !glSamplerParameterfv ||
        !glSamplerParameterIiv || !glSamplerParameterIuiv ||
        !glGetSamplerParameteriv || !glGetSamplerParameterIiv ||
        !glGetSamplerParameterfv || !glGetSamplerParameterIuiv) {

        O3D_ERROR(E_UnsuportedFeature("OpenGL sampler object is not available"));
    }

#endif // O3D_GL_VERSION_3_3

#ifdef O3D_GL_ARB_map_buffer_range

    //
    // O3D_GL_ARB_map_buffer_range
    //

    glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC) GL::getProcAddress("glMapBufferRange");
    glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC) GL::getProcAddress("glFlushMappedBufferRange");

    if (!glMapBufferRange || !glFlushMappedBufferRange) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL map buffer object range is not available"));
    }

#endif // O3D_GL_ARB_map_buffer_range

#ifdef O3D_GL_ARB_texture_multisample

    //
    // O3D_GL_ARB_texture_multisample
    //

    glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC) GL::getProcAddress("glTexImage2DMultisample");
    glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC) GL::getProcAddress("glTexImage3DMultisample");
    glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC) GL::getProcAddress("glGetMultisamplefv");
    glSampleMaski = (PFNGLSAMPLEMASKIPROC) GL::getProcAddress("glSampleMaski");

    if (!glTexImage2DMultisample || !glTexImage3DMultisample || !glGetMultisamplefv || !glSampleMaski) {
        O3D_WARNING("OpenGL texture multisample fonctions is not available");
    }

#endif // O3D_GL_ARB_texture_multisample

#ifdef O3D_GL_VERSION_4_0
    glBlendEquationi = (PFNGLBLENDEQUATIONIPROC) GL::getProcAddress("glBlendEquationi");
    glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC) GL::getProcAddress("glBlendEquationSeparatei");
    glBlendFunci = (PFNGLBLENDFUNCIPROC) GL::getProcAddress("glBlendFunci");
    glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC) GL::getProcAddress("glBlendFuncSeparatei");

    if (!glBlendEquationi || !glBlendEquationSeparatei || !glBlendFunci || !glBlendFuncSeparatei) {
        O3D_WARNING("OpenGL Blend index/separate is not available");
    }
#endif // O3D_GL_VERSION_4_0

#ifdef O3D_GL_VERSION_4_1
    glDepthRangef = (PFNGLDEPTHRANGEFPROC)GL::getProcAddress("glDepthRangef");
    glClearDepthf = (PFNGLCLEARDEPTHFPROC)GL::getProcAddress("glClearDepthf");
#endif // O3D_GL_VERSION_4_1

#ifdef O3D_GL_VERSION_4_2

    glDrawArraysInstancedBaseInstance = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC) GL::getProcAddress("glDrawArraysInstancedBaseInstance");
    glDrawElementsInstancedBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC) GL::getProcAddress("glDrawElementsInstancedBaseInstance");
    glDrawElementsInstancedBaseVertexBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC) GL::getProcAddress("glDrawElementsInstancedBaseVertexBaseInstance");
    glGetInternalformativ = (PFNGLGETINTERNALFORMATIVPROC) GL::getProcAddress("glGetInternalformativ");
    glGetActiveAtomicCounterBufferiv = (PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC) GL::getProcAddress("glGetActiveAtomicCounterBufferiv");
    glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC) GL::getProcAddress("glBindImageTexture");
    glMemoryBarrier = (PFNGLMEMORYBARRIERPROC) GL::getProcAddress("glMemoryBarrier");
    glTexStorage1D = (PFNGLTEXSTORAGE1DPROC) GL::getProcAddress("glTexStorage1D");
    glTexStorage2D = (PFNGLTEXSTORAGE2DPROC) GL::getProcAddress("glTexStorage2D");
    glTexStorage3D = (PFNGLTEXSTORAGE3DPROC) GL::getProcAddress("glTexStorage3D");
    glDrawTransformFeedbackInstanced = (PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC) GL::getProcAddress("glDrawTransformFeedbackInstanced");
    glDrawTransformFeedbackStreamInstanced = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC) GL::getProcAddress("glDrawTransformFeedbackStreamInstanced");

    if (!glDrawArraysInstancedBaseInstance || !glDrawElementsInstancedBaseInstance ||
        !glDrawElementsInstancedBaseVertexBaseInstance || !glDrawTransformFeedbackInstanced ||
        !glDrawTransformFeedbackStreamInstanced) {
        O3D_WARNING("OpenGL 4.2 draw instanced is not available");
    }

    if (!glGetInternalformativ) {
        O3D_WARNING("OpenGL GetInternalformativ is not available");
    }

    if (!glGetActiveAtomicCounterBufferiv) {
        O3D_WARNING("OpenGL GetActiveAtomicCounterBufferiv is not available");
    }

    if (!glBindImageTexture) {
        O3D_WARNING("OpenGL BindImageTexture is not available");
    }

    if (!glMemoryBarrier) {
        O3D_WARNING("OpenGL Memory Barrier is not available");
    }

    if (!glTexStorage1D || !glTexStorage2D || !glTexStorage3D) {
        O3D_WARNING("OpenGL TexStorage<N>D is not available");
    }

#endif // O3D_GL_VERSION_4_2

#ifdef O3D_GL_VERSION_4_3
#endif // O3D_GL_VERSION_4_3

#ifdef O3D_GL_VERSION_4_4
#endif // O3D_GL_VERSION_4_4

#ifdef O3D_GL_VERSION_4_5

//    PFNGLCREATEBUFFERSPROC glCreateBuffers = nullptr;
//    PFNGLNAMEDBUFFERSTORAGEPROC glNamedBufferStorage = nullptr;
//    PFNGLNAMEDBUFFERDATAPROC glNamedBufferData = nullptr;
//    PFNGLNAMEDBUFFERSUBDATAPROC glNamedBufferSubData = nullptr;
//    PFNGLCOPYNAMEDBUFFERSUBDATAPROC glCopyNamedBufferSubData = nullptr;
//    PFNGLCLEARNAMEDBUFFERDATAPROC glClearNamedBufferData = nullptr;
//    PFNGLCLEARNAMEDBUFFERSUBDATAPROC glClearNamedBufferSubData = nullptr;
//    PFNGLMAPNAMEDBUFFERPROC glMapNamedBuffer = nullptr;
//    PFNGLMAPNAMEDBUFFERRANGEPROC glMapNamedBufferRange = nullptr;
//    PFNGLUNMAPNAMEDBUFFERPROC glUnmapNamedBuffer = nullptr;
//    PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC glFlushMappedNamedBufferRange = nullptr;
//    PFNGLGETNAMEDBUFFERPARAMETERIVPROC glGetNamedBufferParameteriv = nullptr;
//    PFNGLGETNAMEDBUFFERPARAMETERI64VPROC glGetNamedBufferParameteri64v = nullptr;
//    PFNGLGETNAMEDBUFFERPOINTERVPROC glGetNamedBufferPointerv = nullptr;
//    PFNGLGETNAMEDBUFFERSUBDATAPROC glGetNamedBufferSubData = nullptr;
//    PFNGLCREATEFRAMEBUFFERSPROC glCreateFramebuffers = nullptr;
//    PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC glNamedFramebufferRenderbuffer = nullptr;
//    PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC glNamedFramebufferParameteri = nullptr;
//    PFNGLNAMEDFRAMEBUFFERTEXTUREPROC glNamedFramebufferTexture = nullptr;
//    PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC glNamedFramebufferTextureLayer = nullptr;
//    PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC glNamedFramebufferDrawBuffer = nullptr;
//    PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC glNamedFramebufferDrawBuffers = nullptr;
//    PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC glNamedFramebufferReadBuffer = nullptr;
//    PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC glInvalidateNamedFramebufferData = nullptr;
//    PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC glInvalidateNamedFramebufferSubData = nullptr;
//    PFNGLCLEARNAMEDFRAMEBUFFERIVPROC glClearNamedFramebufferiv = nullptr;
//    PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC glClearNamedFramebufferuiv = nullptr;
//    PFNGLCLEARNAMEDFRAMEBUFFERFVPROC glClearNamedFramebufferfv = nullptr;
//    PFNGLCLEARNAMEDFRAMEBUFFERFIPROC glClearNamedFramebufferfi = nullptr;
//    PFNGLBLITNAMEDFRAMEBUFFERPROC glBlitNamedFramebuffer = nullptr;
//    PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC glCheckNamedFramebufferStatus = nullptr;
//    PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC glGetNamedFramebufferParameteriv = nullptr;
//    PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetNamedFramebufferAttachmentParameteriv = nullptr;

    glGetTextureSubImage = (PFNGLGETTEXTURESUBIMAGEPROC)GL::getProcAddress("glGetTextureSubImage");

#endif // O3D_GL_VERSION_4_5

#ifdef O3D_GL_VERSION_4_6
#endif // O3D_GL_VERSION_4_6
}

//
// Profile GLES 3.0+
//

void GLExtensionManager::getGLESFunctions()
{
    //
    // OpenGL 1.0
    //

#ifdef O3D_GL_VERSION_1_0
    glCullFace = (PFNGLCULLFACEPROC) GL::getProcAddress("glCullFace");
    glFrontFace = (PFNGLFRONTFACEPROC) GL::getProcAddress("glFrontFace");
    glHint = (PFNGLHINTPROC) GL::getProcAddress("glHint");
    glLineWidth = (PFNGLLINEWIDTHPROC) GL::getProcAddress("glLineWidth");
    glPointSize = (PFNGLPOINTSIZEPROC) GL::getProcAddress("glPointSize");
    glPolygonMode = (PFNGLPOLYGONMODEPROC) GL::getProcAddress("glPolygonMode");
    glScissor = (PFNGLSCISSORPROC) GL::getProcAddress("glScissor");
    glTexParameterf = (PFNGLTEXPARAMETERFPROC) GL::getProcAddress("glTexParameterf");
    glTexParameterfv = (PFNGLTEXPARAMETERFVPROC) GL::getProcAddress("glTexParameterfv");
    glTexParameteri = (PFNGLTEXPARAMETERIPROC) GL::getProcAddress("glTexParameteri");
    glTexParameteriv = (PFNGLTEXPARAMETERIVPROC) GL::getProcAddress("glTexParameteriv");
    glTexImage1D = (PFNGLTEXIMAGE1DPROC) GL::getProcAddress("glTexImage1D");
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
    glLogicOp = (PFNGLLOGICOPPROC) GL::getProcAddress("glLogicOp");
    glStencilFunc = (PFNGLSTENCILFUNCPROC) GL::getProcAddress("glStencilFunc");
    glStencilOp = (PFNGLSTENCILOPPROC) GL::getProcAddress("glStencilOp");
    glDepthFunc = (PFNGLDEPTHFUNCPROC) GL::getProcAddress("glDepthFunc");
    glPixelStoref = (PFNGLPIXELSTOREFPROC) GL::getProcAddress("glPixelStoref");
    glPixelStorei = (PFNGLPIXELSTOREIPROC) GL::getProcAddress("glPixelStorei");
    glReadBuffer = (PFNGLREADBUFFERPROC) GL::getProcAddress("glReadBuffer");
    glReadPixels = (PFNGLREADPIXELSPROC) GL::getProcAddress("glReadPixels");
    glGetBooleanv = (PFNGLGETBOOLEANVPROC) GL::getProcAddress("glGetBooleanv");
    glGetDoublev = (PFNGLGETDOUBLEVPROC) GL::getProcAddress("glGetDoublev");
    glGetError = (PFNGLGETERRORPROC) GL::getProcAddress("glGetError");
    glGetFloatv = (PFNGLGETFLOATVPROC) GL::getProcAddress("glGetFloatv");
    _glGetIntegerv = (PFNGLGETINTEGERVPROC) GL::getProcAddress("glGetIntegerv");
    _glGetString = (PFNGLGETSTRINGPROC) GL::getProcAddress("glGetString");
    glGetTexImage = (PFNGLGETTEXIMAGEPROC) GL::getProcAddress("glGetTexImage");
    glGetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC) GL::getProcAddress("glGetTexParameterfv");
    glGetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC) GL::getProcAddress("glGetTexParameteriv");
    glGetTexLevelParameterfv = (PFNGLGETTEXLEVELPARAMETERFVPROC) GL::getProcAddress("glGetTexLevelParameterfv");
    glGetTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIVPROC) GL::getProcAddress("glGetTexLevelParameteriv");
    glIsEnabled = (PFNGLISENABLEDPROC) GL::getProcAddress("glIsEnabled");
    glDepthRange = (PFNGLDEPTHRANGEPROC) GL::getProcAddress("glDepthRange");
    glViewport = (PFNGLVIEWPORTPROC) GL::getProcAddress("glViewport");
#endif

    //
    // OpenGL 1.1
    //

#ifdef O3D_GL_VERSION_1_1
    glDrawArrays = (PFNGLDRAWARRAYSPROC) GL::getProcAddress("glDrawArrays");
    glDrawElements = (PFNGLDRAWELEMENTSPROC) GL::getProcAddress("glDrawElements");
    glGetPointerv = (PFNGLGETPOINTERVPROC) GL::getProcAddress("glGetPointerv");
    glPolygonOffset = (PFNGLPOLYGONOFFSETPROC) GL::getProcAddress("glPolygonOffset");
    glCopyTexImage1D = (PFNGLCOPYTEXIMAGE1DPROC) GL::getProcAddress("glCopyTexImage1D");
    glCopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC) GL::getProcAddress("glCopyTexImage2D");
    glCopyTexSubImage1D = (PFNGLCOPYTEXSUBIMAGE1DPROC) GL::getProcAddress("glCopyTexSubImage1D");
    glCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC) GL::getProcAddress("glCopyTexSubImage2D");
    glTexSubImage1D = (PFNGLTEXSUBIMAGE1DPROC) GL::getProcAddress("glTexSubImage1D");
    glTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC) GL::getProcAddress("glTexSubImage2D");
    glBindTexture = (PFNGLBINDTEXTUREPROC) GL::getProcAddress("glBindTexture");
    glDeleteTextures = (PFNGLDELETETEXTURESPROC) GL::getProcAddress("glDeleteTextures");
    glGenTextures = (PFNGLGENTEXTURESPROC) GL::getProcAddress("glGenTextures");
    glIsTexture = (PFNGLISTEXTUREPROC) GL::getProcAddress("glIsTexture");
#endif

    //
    // draw range element*
    //

#ifdef O3D_GL_VERSION_1_2
    glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC) GL::getProcAddress("glDrawRangeElements");
    if (!glDrawRangeElements) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Draw range elements"));
    }

    //
    // Texture3d
    //

    glTexImage3D = (PFNGLTEXIMAGE3DPROC) GL::getProcAddress("glTexImage3D");
    glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC) GL::getProcAddress("glTexSubImage3D");
    if (!glTexImage3D || !glTexSubImage3D) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL texture 3D"));
    }
#endif // O3D_GL_VERSION_1_2

#ifdef O3D_GL_VERSION_1_3

    //
    // multi-texture extension
    //

    glActiveTexture = (PFNGLACTIVETEXTUREPROC) GL::getProcAddress("glActiveTexture");

    if (!glActiveTexture && isExtensionSupported("GL_ARB_multitexture")) {
        glActiveTexture = (PFNGLACTIVETEXTUREPROC) GL::getProcAddress("glActiveTextureARB");
    }

    if (!glActiveTexture) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL active texture"));
    }

    //
    // Compressed texture image
    //

    glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC) GL::getProcAddress("glCompressedTexImage1D");
    glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) GL::getProcAddress("glCompressedTexImage2D");
    glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC) GL::getProcAddress("glCompressedTexImage3D");
    glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) GL::getProcAddress("glCompressedTexSubImage1D");
    glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) GL::getProcAddress("glCompressedTexSubImage2D");
    glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) GL::getProcAddress("glCompressedTexSubImage3D");
    glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC) GL::getProcAddress("glGetCompressedTexImage");

    if (!glCompressedTexImage1D || !glCompressedTexSubImage3D) {
        O3D_WARNING("OpenGL compressed texture 1D related are not availables");
    }

    if (!glCompressedTexImage2D || !glCompressedTexSubImage2D) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL compressed texture 2D"));
    }

    if (!glCompressedTexImage3D || !glCompressedTexSubImage3D) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL compressed texture 3D"));
    }

    if (!glGetCompressedTexImage) {
        O3D_WARNING("OpenGL compressed texture get image is not available");
    }
#endif // O3D_GL_VERSION_1_3

#ifdef O3D_GL_VERSION_1_4

    //
    // PointParametersSupported
    //

    glPointParameterf  = (PFNGLPOINTPARAMETERFPROC) GL::getProcAddress("glPointParameterf");
    glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC) GL::getProcAddress("glPointParameterfv");
    glPointParameteri  = (PFNGLPOINTPARAMETERIPROC) GL::getProcAddress("glPointParameteri");
    glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC) GL::getProcAddress("glPointParameteriv");

    if (!glPointParameterf || !glPointParameterfv || !glPointParameteri || !glPointParameteriv) {
        O3D_WARNING("OpenGL point parameter is not available");
    }

    glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) GL::getProcAddress("glBlendFuncSeparate");

    if (!glBlendFuncSeparate) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL blend func separate function"));
    }

    glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC) GL::getProcAddress("glMultiDrawArrays");
    glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC) GL::getProcAddress("glMultiDrawElements");

    if (!glMultiDrawArrays || !glMultiDrawElements) {
        O3D_WARNING("OpenGL multi draw is not available");
    }

    glBlendEquation = (PFNGLBLENDEQUATIONPROC) GL::getProcAddress("glBlendEquation");

    if (!glBlendEquation) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL blend equation"));
    }

#endif // O3D_GL_VERSION_1_4

#ifdef O3D_GL_VERSION_1_5
    //
    // Vertex Buffer Object
    //

    glBindBuffer		= (PFNGLBINDBUFFERPROC) GL::getProcAddress("glBindBuffer");
    glGenBuffers		= (PFNGLGENBUFFERSPROC) GL::getProcAddress("glGenBuffers");
    glBufferData		= (PFNGLBUFFERDATAPROC) GL::getProcAddress("glBufferData");
    glBufferSubData		= (PFNGLBUFFERSUBDATAPROC) GL::getProcAddress("glBufferSubData");
    glDeleteBuffers		= (PFNGLDELETEBUFFERSPROC) GL::getProcAddress("glDeleteBuffers");
    glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC) GL::getProcAddress("glGetBufferParameteriv");
    glMapBuffer			= (PFNGLMAPBUFFERPROC) GL::getProcAddress("glMapBuffer");
    glUnmapBuffer		= (PFNGLUNMAPBUFFERPROC) GL::getProcAddress("glUnmapBuffer");

    if (!glBindBuffer || !glDeleteBuffers || !glGenBuffers || !glBufferData || !glGetBufferParameteriv) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL VBO related"));
    }

    if ((!glMapBuffer || !glUnmapBuffer) && isExtensionSupported("GL_OES_mapbuffer")) {
        glMapBuffer = (PFNGLMAPBUFFERPROC)GL::getProcAddress("glMapBufferOES");
        glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)GL::getProcAddress("glUnmapBufferOES");
        // glGetBufferPointerv = (?)GL::getProcAddress("glGetBufferPointervOES");
    }

    if (!glMapBuffer || !glUnmapBuffer) {
        O3D_WARNING("OpenGL OES mapbuffer is not available");
    }

    // OcclusionQuerySupported
    glGenQueries	= (PFNGLGENQUERIESPROC)GL::getProcAddress("glGenQueries");
    glIsQuery		= (PFNGLISQUERYPROC)GL::getProcAddress("glIsQuery");
    glBeginQuery	= (PFNGLBEGINQUERYPROC)GL::getProcAddress("glBeginQuery");
    glEndQuery		= (PFNGLENDQUERYPROC)GL::getProcAddress("glEndQuery");
    glGetQueryiv		= (PFNGLGETQUERYIVPROC)GL::getProcAddress("glGetQueryiv");
    glGetQueryObjectiv	= (PFNGLGETQUERYOBJECTIVPROC)GL::getProcAddress("glGetQueryObjectiv");
    glGetQueryObjectuiv	= (PFNGLGETQUERYOBJECTUIVPROC)GL::getProcAddress("glGetQueryObjectuiv");
    glDeleteQueries		= (PFNGLDELETEQUERIESPROC)GL::getProcAddress("glDeleteQueries");

    if (!glGenQueries || !glIsQuery || !glBeginQuery || !glEndQuery || !glGetQueryiv ||
        !glGetQueryObjectuiv || !glDeleteQueries) {

        O3D_ERROR(E_UnsuportedFeature("OpenGL occlusion query"));
    }

    if (!glGetQueryObjectiv) {
        O3D_WARNING("OpenGL occlusion query GetQueryObjectiv is not avalaible");
    }

#endif // O3D_GL_VERSION_1_5

#ifdef O3D_GL_VERSION_2_0

    //
    // GLSL
    //

    glCreateShader		= (PFNGLCREATESHADERPROC) GL::getProcAddress("glCreateShader");
    glShaderSource		= (PFNGLSHADERSOURCEPROC) GL::getProcAddress("glShaderSource");
    glCompileShader		= (PFNGLCOMPILESHADERPROC) GL::getProcAddress("glCompileShader");
    glCreateProgram		= (PFNGLCREATEPROGRAMPROC) GL::getProcAddress("glCreateProgram");
    glAttachShader		= (PFNGLATTACHSHADERPROC) GL::getProcAddress("glAttachShader");
    glLinkProgram		= (PFNGLLINKPROGRAMPROC) GL::getProcAddress("glLinkProgram");
    glValidateProgram	= (PFNGLVALIDATEPROGRAMPROC)GL::getProcAddress("glValidateProgram");
    glUseProgram		= (PFNGLUSEPROGRAMPROC) GL::getProcAddress("glUseProgram");

    glDetachShader		= (PFNGLDETACHSHADERPROC) GL::getProcAddress("glDetachShader");
    glDeleteShader		= (PFNGLDELETESHADERPROC) GL::getProcAddress("glDeleteShader");
    glDeleteProgram		= (PFNGLDELETEPROGRAMPROC) GL::getProcAddress("glDeleteProgram");

    glGetProgramiv		= (PFNGLGETPROGRAMIVPROC) GL::getProcAddress("glGetProgramiv");
    glGetProgramInfoLog	= (PFNGLGETPROGRAMINFOLOGPROC) GL::getProcAddress("glGetProgramInfoLog");
    glGetShaderiv		= (PFNGLGETSHADERIVPROC) GL::getProcAddress("glGetShaderiv");
    glGetShaderInfoLog	= (PFNGLGETSHADERINFOLOGPROC) GL::getProcAddress("glGetShaderInfoLog");
    glIsProgram		= (PFNGLISPROGRAMPROC) GL::getProcAddress("glIsProgram");
    glIsShader		= (PFNGLISSHADERPROC) GL::getProcAddress("glIsShader");

    glGetUniformLocation= (PFNGLGETUNIFORMLOCATIONPROC) GL::getProcAddress("glGetUniformLocation");
    glGetActiveUniform  = (PFNGLGETACTIVEUNIFORMPROC) GL::getProcAddress("glGetActiveUniform");
    glUniform1f		= (PFNGLUNIFORM1FPROC) GL::getProcAddress("glUniform1f");
    glUniform2f		= (PFNGLUNIFORM2FPROC) GL::getProcAddress("glUniform2f");
    glUniform3f		= (PFNGLUNIFORM3FPROC) GL::getProcAddress("glUniform3f");
    glUniform4f		= (PFNGLUNIFORM4FPROC) GL::getProcAddress("glUniform4f");
    glUniform1fv	= (PFNGLUNIFORM1FVPROC) GL::getProcAddress("glUniform1fv");
    glUniform2fv	= (PFNGLUNIFORM2FVPROC) GL::getProcAddress("glUniform2fv");
    glUniform3fv	= (PFNGLUNIFORM3FVPROC) GL::getProcAddress("glUniform3fv");
    glUniform4fv	= (PFNGLUNIFORM4FVPROC) GL::getProcAddress("glUniform4fv");
    glUniform1i		= (PFNGLUNIFORM1IPROC) GL::getProcAddress("glUniform1i");
    glUniform2i		= (PFNGLUNIFORM2IPROC) GL::getProcAddress("glUniform2i");
    glUniform3i		= (PFNGLUNIFORM3IPROC) GL::getProcAddress("glUniform3i");
    glUniform4i		= (PFNGLUNIFORM4IPROC) GL::getProcAddress("glUniform4i");
    glUniformMatrix2fv	= (PFNGLUNIFORMMATRIX2FVPROC) GL::getProcAddress("glUniformMatrix2fv");
    glUniformMatrix3fv	= (PFNGLUNIFORMMATRIX3FVPROC) GL::getProcAddress("glUniformMatrix3fv");
    glUniformMatrix4fv	= (PFNGLUNIFORMMATRIX4FVPROC) GL::getProcAddress("glUniformMatrix4fv");

    glDisableVertexAttribArray	= (PFNGLDISABLEVERTEXATTRIBARRAYPROC) GL::getProcAddress("glDisableVertexAttribArray");
    glEnableVertexAttribArray	= (PFNGLENABLEVERTEXATTRIBARRAYPROC) GL::getProcAddress("glEnableVertexAttribArray");
    glGetActiveAttrib		= (PFNGLGETACTIVEATTRIBPROC) GL::getProcAddress("glGetActiveAttrib");
    glBindAttribLocation		= (PFNGLBINDATTRIBLOCATIONPROC) GL::getProcAddress("glBindAttribLocation");
    glGetAttribLocation		= (PFNGLGETATTRIBLOCATIONPROC) GL::getProcAddress("glGetAttribLocation");
    glGetVertexAttribdv		= (PFNGLGETVERTEXATTRIBDVPROC) GL::getProcAddress("glGetVertexAttribdv");
    glGetVertexAttribfv		= (PFNGLGETVERTEXATTRIBFVPROC) GL::getProcAddress("glGetVertexAttribfv");
    glGetVertexAttribiv		= (PFNGLGETVERTEXATTRIBIVPROC) GL::getProcAddress("glGetVertexAttribiv");
    glGetVertexAttribPointerv	= (PFNGLGETVERTEXATTRIBPOINTERVPROC) GL::getProcAddress("glGetVertexAttribPointerv");

    glVertexAttrib1d		= (PFNGLVERTEXATTRIB1DPROC) GL::getProcAddress("glVertexAttrib1d");
    glVertexAttrib1dv		= (PFNGLVERTEXATTRIB1DVPROC) GL::getProcAddress("glVertexAttrib1dv");
    glVertexAttrib1f		= (PFNGLVERTEXATTRIB1FPROC) GL::getProcAddress("glVertexAttrib1f");
    glVertexAttrib1fv		= (PFNGLVERTEXATTRIB1FVPROC) GL::getProcAddress("glVertexAttrib1fv");
    glVertexAttrib1s		= (PFNGLVERTEXATTRIB1SPROC) GL::getProcAddress("glVertexAttrib1s");
    glVertexAttrib1sv		= (PFNGLVERTEXATTRIB1SVPROC) GL::getProcAddress("glVertexAttrib1sv");
    glVertexAttrib2d		= (PFNGLVERTEXATTRIB2DPROC) GL::getProcAddress("glVertexAttrib2d");
    glVertexAttrib2dv		= (PFNGLVERTEXATTRIB2DVPROC) GL::getProcAddress("glVertexAttrib2dv");
    glVertexAttrib2f		= (PFNGLVERTEXATTRIB2FPROC) GL::getProcAddress("glVertexAttrib2f");
    glVertexAttrib2fv		= (PFNGLVERTEXATTRIB2FVPROC) GL::getProcAddress("glVertexAttrib2fv");
    glVertexAttrib2s		= (PFNGLVERTEXATTRIB2SPROC) GL::getProcAddress("glVertexAttrib2s");
    glVertexAttrib2sv		= (PFNGLVERTEXATTRIB2SVPROC) GL::getProcAddress("glVertexAttrib2sv");
    glVertexAttrib3d		= (PFNGLVERTEXATTRIB3DPROC) GL::getProcAddress("glVertexAttrib3d");
    glVertexAttrib3dv		= (PFNGLVERTEXATTRIB3DVPROC) GL::getProcAddress("glVertexAttrib3dv");
    glVertexAttrib3f		= (PFNGLVERTEXATTRIB3FPROC) GL::getProcAddress("glVertexAttrib3f");
    glVertexAttrib3fv		= (PFNGLVERTEXATTRIB3FVPROC) GL::getProcAddress("glVertexAttrib3fv");
    glVertexAttrib3s		= (PFNGLVERTEXATTRIB3SPROC) GL::getProcAddress("glVertexAttrib3s");
    glVertexAttrib3sv		= (PFNGLVERTEXATTRIB3SVPROC) GL::getProcAddress("glVertexAttrib3sv");
    glVertexAttrib4Nbv		= (PFNGLVERTEXATTRIB4NBVPROC) GL::getProcAddress("glVertexAttrib4Nbv");
    glVertexAttrib4Niv		= (PFNGLVERTEXATTRIB4NIVPROC) GL::getProcAddress("glVertexAttrib4Niv");
    glVertexAttrib4Nsv		= (PFNGLVERTEXATTRIB4NSVPROC) GL::getProcAddress("glVertexAttrib4Nsv");
    glVertexAttrib4Nub		= (PFNGLVERTEXATTRIB4NUBPROC) GL::getProcAddress("glVertexAttrib4Nub");
    glVertexAttrib4Nubv		= (PFNGLVERTEXATTRIB4NUBVPROC) GL::getProcAddress("glVertexAttrib4Nubv");
    glVertexAttrib4Nuiv		= (PFNGLVERTEXATTRIB4NUIVPROC) GL::getProcAddress("glVertexAttrib4Nuiv");
    glVertexAttrib4Nusv		= (PFNGLVERTEXATTRIB4NUSVPROC) GL::getProcAddress("glVertexAttrib4Nusv");
    glVertexAttrib4bv		= (PFNGLVERTEXATTRIB4BVPROC) GL::getProcAddress("glVertexAttrib4bv");
    glVertexAttrib4d		= (PFNGLVERTEXATTRIB4DPROC) GL::getProcAddress("glVertexAttrib4d");
    glVertexAttrib4dv		= (PFNGLVERTEXATTRIB4DVPROC) GL::getProcAddress("glVertexAttrib4dv");
    glVertexAttrib4f		= (PFNGLVERTEXATTRIB4FPROC) GL::getProcAddress("glVertexAttrib4f");
    glVertexAttrib4fv		= (PFNGLVERTEXATTRIB4FVPROC) GL::getProcAddress("glVertexAttrib4fv");
    glVertexAttrib4iv		= (PFNGLVERTEXATTRIB4IVPROC) GL::getProcAddress("glVertexAttrib4iv");
    glVertexAttrib4s		= (PFNGLVERTEXATTRIB4SPROC) GL::getProcAddress("glVertexAttrib4s");
    glVertexAttrib4sv		= (PFNGLVERTEXATTRIB4SVPROC) GL::getProcAddress("glVertexAttrib4sv");
    glVertexAttrib4ubv		= (PFNGLVERTEXATTRIB4UBVPROC)GL::getProcAddress("glVertexAttrib4ubv");
    glVertexAttrib4uiv		= (PFNGLVERTEXATTRIB4UIVPROC)GL::getProcAddress("glVertexAttrib4uiv");
    glVertexAttrib4usv		= (PFNGLVERTEXATTRIB4USVPROC)GL::getProcAddress("glVertexAttrib4usv");
    glVertexAttribPointer	= (PFNGLVERTEXATTRIBPOINTERPROC)GL::getProcAddress("glVertexAttribPointer");

    if (!glCreateShader || !glShaderSource || !glCompileShader || !glCreateProgram  ||
        !glAttachShader || !glLinkProgram || !glValidateProgram || !glUseProgram  ||
        !glDetachShader || !glDeleteShader || !glDeleteProgram || !glGetProgramInfoLog  ||
        !glGetShaderInfoLog || !glIsProgram || !glIsShader || !glGetUniformLocation  ||
        !glGetProgramiv || !glUniform1f || !glUniform2f || !glUniform3f || !glUniform4f  ||
        !glUniform1fv || !glUniform2fv || !glUniform3fv || !glUniform4fv  ||
        !glUniform1i || !glUniform2i || !glUniform3i || !glUniform4i  ||
        !glUniformMatrix2fv || !glUniformMatrix3fv || !glUniformMatrix4fv  ||
        !glGetShaderiv || !glGetActiveUniform  ||
        !glBindAttribLocation || !glGetAttribLocation || !glGetVertexAttribfv  ||
        !glGetVertexAttribiv || !glGetVertexAttribPointerv ||
        !glVertexAttrib1f || !glVertexAttrib1fv ||
        !glVertexAttrib2f || !glVertexAttrib2fv ||
        !glVertexAttrib3f || !glVertexAttrib3fv ||
        !glVertexAttrib4f || !glVertexAttrib4fv || !glVertexAttribPointer) {

        O3D_ERROR(E_UnsuportedFeature("OpenGL GLSL"));
    }

    if (!glVertexAttrib1s) {
        O3D_WARNING("OpenGL GLSL short format is not avalaible");
    }

    if (!glVertexAttrib1d) {
        O3D_WARNING("OpenGL GLSL double format is not avalaible");
    }

    //
    // Blend equation separate
    //

    glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC) GL::getProcAddress("glBlendEquationSeparate");

    if (!glBlendEquationSeparate) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL blend equation separate"));
    }

    //
    // Draw buffers
    //

    glDrawBuffers = (PFNGLDRAWBUFFERSPROC) GL::getProcAddress("glDrawBuffers");

    if (!glDrawBuffers && isExtensionSupported("GL_ARB_draw_buffers")) {
        glDrawBuffers = (PFNGLDRAWBUFFERSPROC) GL::getProcAddress("glDrawBuffersARB");
    }

    if (!glDrawBuffers) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL draw buffers"));
    }

    //
    // Stencil separate
    //

    glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC) GL::getProcAddress("glStencilOpSeparate");
    glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC) GL::getProcAddress("glStencilFuncSeparate");
    glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC) GL::getProcAddress("glStencilMaskSeparate");

    if (!glStencilOpSeparate || !glStencilFuncSeparate || !glStencilMaskSeparate) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL stencil separate"));
    }

#endif // O3D_GL_VERSION_2_0

#ifdef O3D_GL_ARB_framebuffer_object

    //
    // Frame Buffer Object
    //

    glIsRenderbuffer 		= (PFNGLISRENDERBUFFERPROC) GL::getProcAddress("glIsRenderbuffer");
    glBindRenderbuffer 		= (PFNGLBINDRENDERBUFFERPROC) GL::getProcAddress("glBindRenderbuffer");
    glDeleteRenderbuffers 	= (PFNGLDELETERENDERBUFFERSPROC) GL::getProcAddress("glDeleteRenderbuffers");
    glGenRenderbuffers 		= (PFNGLGENRENDERBUFFERSPROC) GL::getProcAddress("glGenRenderbuffers");
    glRenderbufferStorage 	= (PFNGLRENDERBUFFERSTORAGEPROC) GL::getProcAddress("glRenderbufferStorage");
    glGetRenderbufferParameteriv    = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) GL::getProcAddress("glGetRenderbufferParameteriv");
    glIsFramebuffer 		= (PFNGLISFRAMEBUFFERPROC) GL::getProcAddress("glIsFramebuffer");
    glBindFramebuffer 		= (PFNGLBINDFRAMEBUFFERPROC) GL::getProcAddress("glBindFramebuffer");
    glDeleteFramebuffers 	= (PFNGLDELETEFRAMEBUFFERSPROC) GL::getProcAddress("glDeleteFramebuffers");
    glGenFramebuffers 		= (PFNGLGENFRAMEBUFFERSPROC) GL::getProcAddress("glGenFramebuffers");
    glCheckFramebufferStatus 	= (PFNGLCHECKFRAMEBUFFERSTATUSPROC) GL::getProcAddress("glCheckFramebufferStatus");
    glFramebufferTexture1D 		= (PFNGLFRAMEBUFFERTEXTURE1DPROC) GL::getProcAddress("glFramebufferTexture1D");
    glFramebufferTexture2D 		= (PFNGLFRAMEBUFFERTEXTURE2DPROC) GL::getProcAddress("glFramebufferTexture2D");
    glFramebufferTexture3D 		= (PFNGLFRAMEBUFFERTEXTURE3DPROC) GL::getProcAddress("glFramebufferTexture3D");
    glFramebufferRenderbuffer 	= (PFNGLFRAMEBUFFERRENDERBUFFERPROC) GL::getProcAddress("glFramebufferRenderbuffer");
    glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) GL::getProcAddress("glGetFramebufferAttachmentParameteriv");
    glBlitFramebuffer 		= (PFNGLBLITFRAMEBUFFERPROC) GL::getProcAddress("glBlitFramebuffer");
    glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) GL::getProcAddress("glRenderbufferStorageMultisample");
    glGenerateMipmap 		= (PFNGLGENERATEMIPMAPPROC) GL::getProcAddress("glGenerateMipmap");
    glFramebufferTextureLayer   = (PFNGLFRAMEBUFFERTEXTURELAYERPROC) GL::getProcAddress("glFramebufferTextureLayer");

    if (!glIsRenderbuffer || !glBindRenderbuffer || !glDeleteRenderbuffers ||
        !glGenRenderbuffers || !glRenderbufferStorage || !glGetRenderbufferParameteriv ||
        !glIsFramebuffer || !glBindFramebuffer || !glDeleteFramebuffers ||
        !glGenFramebuffers || !glCheckFramebufferStatus ||
        !glFramebufferTexture2D || !glFramebufferRenderbuffer ||
        !glGetFramebufferAttachmentParameteriv || !glBlitFramebuffer ||
        !glRenderbufferStorageMultisample || !glGenerateMipmap || !glFramebufferTextureLayer) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL FBO"));
    }

    if (!glFramebufferTexture1D) {
        O3D_WARNING("OpenGL FBO texture 1D is not avalaible");
    }

    if (!glFramebufferTexture3D) {
        O3D_WARNING("OpenGL FBO texture 3D is not avalaible");
    }

#endif // O3D_GL_ARB_framebuffer_object

    //
    // Check for GL_OES_texture_float and GL_OES_texture_float_linear
    //

    if (!isExtensionSupported("GL_OES_texture_float")) {
        O3D_ERROR(E_UnsuportedFeature("GL_OES_texture_float is required"));
    }

    if (!isExtensionSupported("GL_OES_texture_half_float_linear")) {
        O3D_ERROR(E_UnsuportedFeature("GL_OES_texture_half_float_linear is required"));
    }

    if (!isExtensionSupported("GL_OES_texture_float_linear")) {
        O3D_ERROR(E_UnsuportedFeature("GL_OES_texture_float_linear is required"));
    }

#ifdef O3D_GL_VERSION_3_0

    //
    // OpenGL 3.0
    //

    glColorMaski = (PFNGLCOLORMASKIPROC) GL::getProcAddress("glColorMaski");
    glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC) GL::getProcAddress("glGetBooleani_v");
    glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC) GL::getProcAddress("glGetIntegeri_v");
    glEnablei = (PFNGLENABLEIPROC) GL::getProcAddress("glEnablei");
    glDisablei = (PFNGLDISABLEIPROC) GL::getProcAddress("glDisablei");
    glIsEnabledi = (PFNGLISENABLEDIPROC) GL::getProcAddress("glIsEnabledi");
    glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC) GL::getProcAddress("glBeginTransformFeedback");
    glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC) GL::getProcAddress("glEndTransformFeedback");
    glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC) GL::getProcAddress("glBindBufferRange");
    glBindBufferBase = (PFNGLBINDBUFFERBASEPROC) GL::getProcAddress("glBindBufferBase");
    glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC) GL::getProcAddress("glTransformFeedbackVaryings");
    glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC) GL::getProcAddress("glGetTransformFeedbackVarying");
    glClampColor = (PFNGLCLAMPCOLORPROC) GL::getProcAddress("glClampColor");
    glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC) GL::getProcAddress("glBeginConditionalRender");
    glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC) GL::getProcAddress("glEndConditionalRender");
    glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC) GL::getProcAddress("glVertexAttribIPointer");
    glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC) GL::getProcAddress("glGetVertexAttribIiv");
    glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC) GL::getProcAddress("glGetVertexAttribIuiv");
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
    glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC) GL::getProcAddress("glGetUniformuiv");
    glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC) GL::getProcAddress("glBindFragDataLocation");
    glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC) GL::getProcAddress("glGetFragDataLocation");
    glUniform1ui = (PFNGLUNIFORM1UIPROC) GL::getProcAddress("glUniform1ui");
    glUniform2ui = (PFNGLUNIFORM2UIPROC) GL::getProcAddress("glUniform2ui");
    glUniform3ui = (PFNGLUNIFORM3UIPROC) GL::getProcAddress("glUniform3ui");
    glUniform4ui = (PFNGLUNIFORM4UIPROC) GL::getProcAddress("glUniform4ui");
    glUniform1uiv = (PFNGLUNIFORM1UIVPROC) GL::getProcAddress("glUniform1uiv");
    glUniform2uiv = (PFNGLUNIFORM2UIVPROC) GL::getProcAddress("glUniform2uiv");
    glUniform3uiv = (PFNGLUNIFORM3UIVPROC) GL::getProcAddress("glUniform3uiv");
    glUniform4uiv = (PFNGLUNIFORM4UIPROC) GL::getProcAddress("glUniform4uiv");
    glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC) GL::getProcAddress("glTexParameterIiv");
    glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC) GL::getProcAddress("glTexParameterIuiv");
    glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC) GL::getProcAddress("glGetTexParameterIiv");
    glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC) GL::getProcAddress("glGetTexParameterIuiv");
    glClearBufferiv = (PFNGLCLEARBUFFERIVPROC) GL::getProcAddress("glClearBufferiv");
    glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC) GL::getProcAddress("glClearBufferuiv");
    glClearBufferfv = (PFNGLCLEARBUFFERFVPROC) GL::getProcAddress("glClearBufferfv");
    glClearBufferfi = (PFNGLCLEARBUFFERFIPROC) GL::getProcAddress("glClearBufferfi");

    if (!glColorMaski || !glGetBooleani_v || !glGetIntegeri_v ||
        !glEnablei || !glDisablei || !glIsEnabledi ||
        !glBeginTransformFeedback || !glEndTransformFeedback ||
        !glBindBufferRange || !glBindBufferBase ||
        !glTransformFeedbackVaryings || !glGetTransformFeedbackVarying ||
        !glGetUniformuiv || !glGetFragDataLocation ||
        !glUniform1ui || !glUniform2ui || !glUniform3ui || !glUniform4ui ||
        !glUniform1uiv || !glUniform2uiv || !glUniform3uiv || !glUniform4uiv ||
        !glTexParameterIiv || !glTexParameterIuiv ||
        !glGetTexParameterIiv || !glGetTexParameterIuiv ||
        !glClearBufferiv || !glClearBufferuiv || !glClearBufferfv ||!glClearBufferfi) {

        // Not mandatory for the moment
        //O3D_WARNING("Missing OpenGL 3.0 related functions");
        O3D_ERROR(E_UnsuportedFeature("OpenGL 3.0 related functions"));
    }

    if (!glBindFragDataLocation) {
        O3D_WARNING("OpenGL frag data location binding is not available");
    }

    if (!glVertexAttribIPointer ||
        !glGetVertexAttribIiv || !glGetVertexAttribIuiv || !glVertexAttribI1i ||
        !glVertexAttribI2i || !glVertexAttribI3i || !glVertexAttribI4i ||
        !glVertexAttribI1ui || !glVertexAttribI2ui || !glVertexAttribI3ui || !glVertexAttribI4ui ||
        !glVertexAttribI1iv || !glVertexAttribI2iv || !glVertexAttribI3iv || !glVertexAttribI4iv ||
        !glVertexAttribI1uiv || !glVertexAttribI2uiv || !glVertexAttribI3uiv || !glVertexAttribI4uiv ||
        !glVertexAttribI4bv || !glVertexAttribI4sv || !glVertexAttribI4ubv || !glVertexAttribI4usv) {
        O3D_WARNING("OpenGL VAO Index is not available");
    }

    if (!glClampColor) {
        O3D_WARNING("OpenGL ClampColor is not available");
    }

    if (!glBeginConditionalRender || !glEndConditionalRender) {
        O3D_WARNING("OpenGL ConditionalRender is not available");
    }

#endif // O3D_GL_VERSION_3_0

#ifdef O3D_GL_VERSION_3_1

    glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC) GL::getProcAddress("glDrawArraysInstanced");
    glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC) GL::getProcAddress("");
    // glTexBuffer = (PFNGLTEXBUFFERPROC) GL::getProcAddress("glTexBuffer");
    // glPrimitiveRestartIndex = (PFNGLPRIMITIVERESTARTINDEXPROC) GL::getProcAddress("glPrimitiveRestartIndex");
    glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC) GL::getProcAddress("glCopyBufferSubData");
    glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC) GL::getProcAddress("glGetUniformIndices");
    glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC) GL::getProcAddress("glGetActiveUniformsiv");
    glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC) GL::getProcAddress("glGetActiveUniformName");
    glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC) GL::getProcAddress("glGetUniformBlockIndex");
    glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC) GL::getProcAddress("glGetActiveUniformBlockiv");
    glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC) GL::getProcAddress("glGetActiveUniformBlockName");
    glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC) GL::getProcAddress("glUniformBlockBinding");

    if (!glDrawArraysInstanced || !glDrawElementsInstanced) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Draw instanced"));
    }

    if (!glCopyBufferSubData) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Copy buffer sub-data"));
    }

    if (!glGetUniformIndices || !glGetActiveUniformsiv || !glGetActiveUniformName ||
        !glGetUniformBlockIndex || !glGetActiveUniformBlockiv || !glGetActiveUniformBlockName ||
        !glUniformBlockBinding) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL Uniform Buffer Object"));
    }

#endif // O3D_GL_VERSION_3_1

#ifdef O3D_GL_VERSION_3_2

    //
    // OpenGL 3.2 and geometry shader
    //

    //glGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC) GL::getProcAddress("glGetInteger64i_v");
    //glGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC) GL::getProcAddress("glGetBufferParameteri64v");
    glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC) GL::getProcAddress("glProgramParameteri");
    glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC) GL::getProcAddress("glFramebufferTexture");
    glFramebufferTextureFace = (PFNGLFRAMEBUFFERTEXTUREFACEPROC) GL::getProcAddress("glFramebufferTextureFace");

    if (!glProgramParameteri || !glFramebufferTexture) {
        O3D_WARNING("Geometry shader functions not founds");
    }

    if (!glFramebufferTextureFace) {
        O3D_WARNING("OpenGL FBO texture face is not avalaibled");
    }

    //
    // GL_ARB_vertex_array_object
    //

    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) GL::getProcAddress("glBindVertexArray");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) GL::getProcAddress("glDeleteVertexArrays");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) GL::getProcAddress("glGenVertexArrays");
    glIsVertexArray = (PFNGLISVERTEXARRAYPROC) GL::getProcAddress("glIsVertexArray");

    if (!glBindVertexArray || !glDeleteVertexArrays || !glGenVertexArrays || !glIsVertexArray) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL VAO"));
    }

#endif // O3D_GL_VERSION_3_2

#ifdef O3D_GL_VERSION_3_3

    //
    // OpenGL 3.2 and sampler object
    //

    glGenSamplers = (PFNGLGENSAMPLERSPROC) GL::getProcAddress("glGenSamplers");
    glDeleteSamplers = (PFNGLDELETESAMPLERSPROC) GL::getProcAddress("glDeleteSamplers");
    glIsSampler = (PFNGLISSAMPLERPROC) GL::getProcAddress("glIsSampler");
    glBindSampler = (PFNGLBINDSAMPLERPROC) GL::getProcAddress("glBindSampler");
    glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC) GL::getProcAddress("glSamplerParameteri");
    glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC) GL::getProcAddress("glSamplerParameteriv");
    glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC) GL::getProcAddress("glSamplerParameterf");
    glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC) GL::getProcAddress("glSamplerParameterfv");
    glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC) GL::getProcAddress("glSamplerParameterIiv");
    glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC) GL::getProcAddress("glSamplerParameterIuiv");
    glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC) GL::getProcAddress("glGetSamplerParameteriv");
    glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC) GL::getProcAddress("glGetSamplerParameterIiv");
    glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC) GL::getProcAddress("glGetSamplerParameterfv");
    glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC) GL::getProcAddress("glGetSamplerParameterIuiv");

    if (!glGenSamplers || !glDeleteSamplers || !glIsSampler || !glBindSampler ||
        !glSamplerParameteri || !glSamplerParameteriv ||
        !glSamplerParameterf || !glSamplerParameterfv) {

        O3D_WARNING("Geometry shader is not available");
    }

    if (!glSamplerParameterIiv || !glSamplerParameterIuiv ||
        !glGetSamplerParameteriv || !glGetSamplerParameterIiv ||
        !glGetSamplerParameterfv || !glGetSamplerParameterIuiv) {

        O3D_WARNING("Geometry shader indexed is not available");
    }

#endif // O3D_GL_VERSION_3_3

#ifdef O3D_GL_ARB_map_buffer_range

    //
    // O3D_GL_ARB_map_buffer_range
    //

    glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC) GL::getProcAddress("glMapBufferRange");
    glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC) GL::getProcAddress("glFlushMappedBufferRange");

    if (!glMapBufferRange || !glFlushMappedBufferRange) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL map buffer object range is not available"));
    }

#endif // O3D_GL_ARB_map_buffer_range

#ifdef O3D_GL_ARB_texture_multisample

    //
    // O3D_GL_ARB_texture_multisample
    //

    glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC) GL::getProcAddress("glTexImage2DMultisample");
    glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC) GL::getProcAddress("glTexImage3DMultisample");
    glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC) GL::getProcAddress("glGetMultisamplefv");
    glSampleMaski = (PFNGLSAMPLEMASKIPROC) GL::getProcAddress("glSampleMaski");

    if (!glTexImage2DMultisample || !glTexImage3DMultisample || !glGetMultisamplefv || !glSampleMaski) {
        O3D_WARNING("OpenGL texture multisample is not available");
    }

#endif // O3D_GL_ARB_texture_multisample

#ifdef O3D_GL_VERSION_4_0
    glBlendEquationi = (PFNGLBLENDEQUATIONIPROC) GL::getProcAddress("glBlendEquationi");
    glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC) GL::getProcAddress("glBlendEquationSeparatei");
    glBlendFunci = (PFNGLBLENDFUNCIPROC) GL::getProcAddress("glBlendFunci");
    glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC) GL::getProcAddress("glBlendFuncSeparatei");

    if (!glBlendEquationi || !glBlendEquationSeparatei || !glBlendFunci || !glBlendFuncSeparatei) {
        O3D_WARNING("OpenGL blend index/separate is not available");
    }
#endif // O3D_GL_VERSION_4_0

#ifdef O3D_GL_VERSION_4_1
    glDepthRangef = (PFNGLDEPTHRANGEFPROC)GL::getProcAddress("glDepthRangef");
    glClearDepthf = (PFNGLCLEARDEPTHFPROC)GL::getProcAddress("glClearDepthf");
#endif // O3D_GL_VERSION_4_1

#ifdef O3D_GL_VERSION_4_2

//    glDrawArraysInstancedBaseInstance = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC) GL::getProcAddress("glDrawArraysInstancedBaseInstance");
//    glDrawElementsInstancedBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC) GL::getProcAddress("glDrawElementsInstancedBaseInstance");
//    glDrawElementsInstancedBaseVertexBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC) GL::getProcAddress("glDrawElementsInstancedBaseVertexBaseInstance");
    glGetInternalformativ = (PFNGLGETINTERNALFORMATIVPROC) GL::getProcAddress("glGetInternalformativ");
//    glGetActiveAtomicCounterBufferiv = (PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC) GL::getProcAddress("glGetActiveAtomicCounterBufferiv");
    glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC) GL::getProcAddress("glBindImageTexture");
    glMemoryBarrier = (PFNGLMEMORYBARRIERPROC) GL::getProcAddress("glMemoryBarrier");
//    glTexStorage1D = (PFNGLTEXSTORAGE1DPROC) GL::getProcAddress("glTexStorage1D");
    glTexStorage2D = (PFNGLTEXSTORAGE2DPROC) GL::getProcAddress("glTexStorage2D");
    glTexStorage3D = (PFNGLTEXSTORAGE3DPROC) GL::getProcAddress("glTexStorage3D");
//    glDrawTransformFeedbackInstanced = (PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC) GL::getProcAddress("glDrawTransformFeedbackInstanced");
//    glDrawTransformFeedbackStreamInstanced = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC) GL::getProcAddress("glDrawTransformFeedbackStreamInstanced");

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

#ifdef O3D_GL_VERSION_4_3
#endif // O3D_GL_VERSION_4_3

#ifdef O3D_GL_VERSION_4_4
#endif // O3D_GL_VERSION_4_4

#ifdef O3D_GL_VERSION_4_5
#endif // O3D_GL_VERSION_4_5

#ifdef O3D_GL_VERSION_4_6
#endif // O3D_GL_VERSION_4_6

    //
    // Enumerated extensions (can depends of GL ES version)
    //

    if (!isExtensionSupported("OES_framebuffer_object")) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL OES_framebuffer_object"));
    }

    if (!isExtensionSupported("GL_OES_texture_3D")) {
        O3D_WARNING("Unavailable OpenGL GL_OES_texture_3D");
    }

    if (!isExtensionSupported("GL_EXT_texture_filter_anisotropic")) {
        O3D_WARNING("Unavailable OpenGL GL_EXT_texture_filter_anisotropic");
    }

    if (!isExtensionSupported("GL_OES_packed_depth_stencil")) {

    }

    if (!isExtensionSupported("GL_OES_depth_texture")) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL GL_OES_depth_texture"));
    }

    if (!isExtensionSupported("GL_OES_depth24")) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL GL_OES_depth24"));
    }

    if (!isExtensionSupported("GL_OES_depth32")) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL GL_OES_depth32"));
    }

    if (!isExtensionSupported("GL_OES_texture_float")) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL GL_OES_texture_float"));
    }

    if (!isExtensionSupported("GL_OES_depth_texture_cube_map")) {

    }

    if (!isExtensionSupported("GL_OES_compressed_ETC1_RGB8_texture")) {

    }

    if (!isExtensionSupported("GL_AMD_compressed_ATC_texture")) {

    }

    if (!isExtensionSupported("GL_KHR_texture_compression_astc_ldr")) {

    }

    if (!isExtensionSupported("GL_EXT_geometry_shader")) {

    }

    if (!isExtensionSupported("GL_EXT_tesselation_shader")) {

    }

    if (!isExtensionSupported("GL_EXT_multisampled_render_to_texture")) {

    }

    if (!isExtensionSupported("GL_EXT_draw_buffer_indexed")) {

    }

    if (!isExtensionSupported("GL_OES_vertex_array_object")) {
        O3D_ERROR(E_UnsuportedFeature("OpenGL GL_OES_vertex_array_object"));
    }
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

    if (!version) {
        //O3D_ERROR(E_InvalidResult("Undefined OpenGL version"));
    }

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
			if (tokenizer.nextElement() == ext)
				return True;
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
