/**
 * @file shader.cpp
 * @brief Implementation of Shader.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author  Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2005-10-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/glextdefines.h"
#include "o3d/engine/shader/shader.h"

#include "o3d/engine/shader/shadermanager.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/stringtokenizer.h"
#include "o3d/engine/texture/texture.h"
#include "o3d/engine/context.h"
#include "o3d/engine/renderer.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Shader, ENGINE_SHADER, SceneEntity)

Shader::T_ProgramInfo::T_ProgramInfo() :
	programName(),
	programType(TYPE_UNDEFINED),
	programSource()
{
}
/*
Bool hader::T_ProgramInfo::isNull()
{
	return (programName.isNull() && (programType == TYPE_UNDEFINED));
}

void Shader::T_ProgramInfo::reset()
{
	programName.destroy();
	programType = TYPE_UNDEFINED;
	programSource.destroy();
	programs.clear();
}
*/
Shader::T_ProgramInfo::T_Program::T_Program() :
	programId(0),
	programState(PROGRAM_UNDEFINED)
{
}

Shader::Shader(BaseObject *parent) :
	SceneEntity(parent),
	m_vertexProgramArray(),
	m_fragmentProgramArray(),
	m_geometryProgramArray(),
    m_tessControlProgramArray(),
    m_tessEvaluationProgramArray(),
	m_programName(),
	m_instances()
{
}

Shader::~Shader()
{
	destroy();
}

void Shader::compileInstance(T_InstanceInfo & _instance)
{
	O3D_ASSERT((_instance.shaderState & SHADER_COMPILED) != SHADER_COMPILED);

	const Int32 & lVertexId = _instance.vertexProgramId;
	const Int32 & lFragmentId = _instance.fragmentProgramId;
	const Int32 & lGeometryId = _instance.geometryProgramId;
    const Int32 & lTessControlId = _instance.tessControlProgramId;
    const Int32 & lTessEvaluationId = _instance.tessEvaluationProgramId;

	const String & lOptions = _instance.options;

    if ((lVertexId != -1) && (m_vertexProgramArray[lVertexId].programs[lOptions].programState == PROGRAM_UNDEFINED)) {
		compileProgram(TYPE_VERTEX_PROGRAM, lVertexId, lOptions);
    }

    if ((lFragmentId != -1) && (m_fragmentProgramArray[lFragmentId].programs[lOptions].programState == PROGRAM_UNDEFINED)) {
		compileProgram(TYPE_FRAGMENT_PROGRAM, lFragmentId, lOptions);
    }

    if ((lGeometryId != -1) && (m_geometryProgramArray[lGeometryId].programs[lOptions].programState == PROGRAM_UNDEFINED)) {
		compileProgram(TYPE_GEOMETRY_PROGRAM, lGeometryId, lOptions);
    }

    if ((lTessControlId != -1) && (m_tessControlProgramArray[lTessControlId].programs[lOptions].programState == PROGRAM_UNDEFINED)) {
        compileProgram(TYPE_TESS_CONTROL_PROGRAM, lTessControlId, lOptions);
    }

    if ((lTessEvaluationId != -1) && (m_tessEvaluationProgramArray[lTessEvaluationId].programs[lOptions].programState == PROGRAM_UNDEFINED)) {
        compileProgram(TYPE_TESS_EVALUATION_PROGRAM, lTessEvaluationId, lOptions);
    }

    if ((lVertexId != -1) && (m_vertexProgramArray[lVertexId].programs[lOptions].programState != PROGRAM_COMPILED)) {
        return;
    }

    if ((lFragmentId != -1) && (m_fragmentProgramArray[lFragmentId].programs[lOptions].programState != PROGRAM_COMPILED)) {
        return;
    }

    if ((lGeometryId != -1) && (m_geometryProgramArray[lGeometryId].programs[lOptions].programState != PROGRAM_COMPILED)) {
        return;
    }

    if ((lTessControlId != -1) && (m_tessControlProgramArray[lTessControlId].programs[lOptions].programState != PROGRAM_COMPILED)) {
        return;
    }

    if ((lTessEvaluationId != -1) && (m_tessEvaluationProgramArray[lTessEvaluationId].programs[lOptions].programState != PROGRAM_COMPILED)) {
        return;
    }

    _instance.shaderState |= SHADER_COMPILED;
}

void Shader::linkInstance(T_InstanceInfo & _instance)
{
	O3D_ASSERT((_instance.shaderState & SHADER_COMPILED) == SHADER_COMPILED);
	O3D_ASSERT((_instance.shaderState & SHADER_LINKED) != SHADER_LINKED);

	const Int32 & lVertexId = _instance.vertexProgramId;
	const Int32 & lFragmentId = _instance.fragmentProgramId;
	const Int32 & lGeometryId = _instance.geometryProgramId;
    const Int32 & lTessControlId = _instance.tessControlProgramId;
    const Int32 & lTessEvaluationId = _instance.tessEvaluationProgramId;

	const String & lOptions = _instance.options;

	const T_ProgramInfo & lVertexProgram = m_vertexProgramArray[lVertexId];
	const T_ProgramInfo & lFragmentProgram = m_fragmentProgramArray[lFragmentId];

    if (_instance.shaderId == 0) {
		_instance.shaderId = glCreateProgram();
    }

    // Bind the attribute's slots @todo layout everywhere and remove that
//    glBindAttribLocation(_instance.shaderId, V_VERTICES_ARRAY, "a_vertex");
//    glBindAttribLocation(_instance.shaderId, V_NORMALS_ARRAY, "a_normal");
//    glBindAttribLocation(_instance.shaderId, V_TANGENT_ARRAY, "a_tangent");
//    glBindAttribLocation(_instance.shaderId, V_BITANGENT_ARRAY, "a_bitangent");
//    glBindAttribLocation(_instance.shaderId, V_COLOR_ARRAY, "a_color");
//    glBindAttribLocation(_instance.shaderId, V_RIGGING_ARRAY, "a_rigging");
//    glBindAttribLocation(_instance.shaderId, V_SKINNING_ARRAY, "a_skinning");
//    glBindAttribLocation(_instance.shaderId, V_WEIGHTING_ARRAY, "a_weighting");
//    glBindAttribLocation(_instance.shaderId, V_UV_MAP_ARRAY, "a_texCoords1");
//    glBindAttribLocation(_instance.shaderId, V_UVW_ARRAY, "a_tex3D_1");
//    glBindAttribLocation(_instance.shaderId, V_UV_MAP2_ARRAY, "a_tex2D_2");
//    glBindAttribLocation(_instance.shaderId, V_UVW_2_ARRAY, "a_tex3D_2");
//    glBindAttribLocation(_instance.shaderId, V_UV_MAP3_ARRAY, "a_tex2D_3");
//    glBindAttribLocation(_instance.shaderId, V_UVW_3_ARRAY, "a_tex3D_3");

    // Since OpenGL 3.3 bind out fragments, but not for GLES
    // @todo Uses layouts everywhere and remove that
//    if (glBindFragDataLocation) {
//        glBindFragDataLocation(_instance.shaderId, 0, "o_finalColor");
//        glBindFragDataLocation(_instance.shaderId, 0, "o_fragData");
//        glBindFragDataLocation(_instance.shaderId, 0, "o_ambient");
//        glBindFragDataLocation(_instance.shaderId, 1, "o_normal");
//        glBindFragDataLocation(_instance.shaderId, 2, "o_position");
//        glBindFragDataLocation(_instance.shaderId, 3, "o_diffuse");
//        glBindFragDataLocation(_instance.shaderId, 4, "o_specular");
//    }

	T_ProgramInfo::CIT_ProgramMap lVpCit = lVertexProgram.programs.find(lOptions);
	T_ProgramInfo::CIT_ProgramMap lFpCit = lFragmentProgram.programs.find(lOptions);
    T_ProgramInfo::CIT_ProgramMap lGpCit;
    T_ProgramInfo::CIT_ProgramMap lTcCit;
    T_ProgramInfo::CIT_ProgramMap lTeCit;

	glAttachShader(_instance.shaderId, lVpCit->second.programId);
	glAttachShader(_instance.shaderId, lFpCit->second.programId);

	// optional geometry program
    if (lGeometryId != -1) {
		const T_ProgramInfo & lGeometryProgram = m_geometryProgramArray[lGeometryId];
        lGpCit = lGeometryProgram.programs.find(lOptions);

		glAttachShader(_instance.shaderId, lGpCit->second.programId);
	}

    // optional tesselation program
    if (lTessControlId != -1 && lTessEvaluationId != -1) {
        const T_ProgramInfo & lTessControlProgram = m_tessControlProgramArray[lTessControlId];
        lTcCit = lTessControlProgram.programs.find(lOptions);

        const T_ProgramInfo & lTessEvaluationProgram = m_tessEvaluationProgramArray[lTessEvaluationId];
        lTcCit = lTessEvaluationProgram.programs.find(lOptions);

        glAttachShader(_instance.shaderId, lTcCit->second.programId);
        glAttachShader(_instance.shaderId, lTeCit->second.programId);
    }

	glLinkProgram(_instance.shaderId);

	GLint lResult = 0;
    glGetProgramiv(_instance.shaderId, GL_LINK_STATUS, (GLint*)&lResult);

    if (lResult == GL_FALSE) {
		GLint lLogSize = 0;
		glGetProgramiv(_instance.shaderId, GL_INFO_LOG_LENGTH, &lLogSize);

		ArrayChar lLogMessage(lLogSize);
        glGetProgramInfoLog(_instance.shaderId, lLogSize, nullptr, lLogMessage.getData());

        glDetachShader(_instance.shaderId, lVpCit->second.programId);
        glDetachShader(_instance.shaderId, lFpCit->second.programId);

        if (lGeometryId != -1) {
            glDetachShader(_instance.shaderId, lGpCit->second.programId);
        }

        if (lTessControlId != -1) {
            glDetachShader(_instance.shaderId, lTcCit->second.programId);
        }

        if (lTessEvaluationId != -1) {
            glDetachShader(_instance.shaderId, lTeCit->second.programId);
        }

		glDeleteProgram(_instance.shaderId);
		_instance.shaderId = 0;

		_instance.shaderState &= ~SHADER_LINKED;

        String vp = String("> using VP <") << lVertexProgram.programName;
        String fp = String("> using FP <") << lFragmentProgram.programName;
        String gp, tc, te;

        if (lGeometryId != -1) {
            const T_ProgramInfo & lGeometryProgram = m_geometryProgramArray[lGeometryId];
            gp = String("> using GP <") << lGeometryProgram.programName;
        }

        if (lTessControlId != -1) {
            const T_ProgramInfo & lTesselationProgram = m_tessControlProgramArray[lTessControlId];
            tc = String("> using TC <") << lTesselationProgram.programName;
        }

        if (lTessEvaluationId != -1) {
            const T_ProgramInfo & lEvaluationProgram = m_tessEvaluationProgramArray[lTessEvaluationId];
            te = String("> using TE <") << lEvaluationProgram.programName;
        }

        O3D_ERROR(E_InvalidOperation(String("Shader : Unable to link the program <") << m_name
                    << vp << fp << gp << tc << te << "> to the object <"
                    << m_name << "> contained in the file : <" << m_programName << "> : " << lLogMessage.getData()));
    } else {
		GLint lLogSize = 0;
		glGetProgramiv(_instance.shaderId, GL_INFO_LOG_LENGTH, &lLogSize);

        if (lLogSize > 1) {
			ArrayChar lLogMessage('\0', lLogSize+1, 0);
            glGetProgramInfoLog(_instance.shaderId, lLogSize, nullptr, lLogMessage.getData());

            String vp = String("> using VP <") << lVertexProgram.programName;
            String fp = String("> using FP <") << lFragmentProgram.programName;
            String gp, tc, te;

            if (lGeometryId != -1) {
                const T_ProgramInfo & lGeometryProgram = m_geometryProgramArray[lGeometryId];
                gp = String("> using GP <") << lGeometryProgram.programName;
            }

            if (lTessControlId != -1) {
                const T_ProgramInfo & lTesselationProgram = m_tessControlProgramArray[lTessControlId];
                tc = String("> using TC <") << lTesselationProgram.programName;
            }

            if (lTessEvaluationId != -1) {
                const T_ProgramInfo & lEvaluationProgram = m_tessEvaluationProgramArray[lTessEvaluationId];
                te = String("> using TE <") << lEvaluationProgram.programName;
            }

            O3D_MESSAGE(String("Shader : Warning when link the program <") << m_name
                        << vp << fp << gp << tc << te << "> to the object <"
                        << m_name << "> contained in the file : <" << m_programName << "> : " << lLogMessage.getData());
		}

		_instance.shaderState |= SHADER_LINKED;
	}

    glDetachShader(_instance.shaderId, lVpCit->second.programId);
    glDetachShader(_instance.shaderId, lFpCit->second.programId);

    if (lGeometryId != -1) {
        glDetachShader(_instance.shaderId, lGpCit->second.programId);
    }

    if (lTessControlId != -1) {
        glDetachShader(_instance.shaderId, lTcCit->second.programId);
    }

    if (lTessEvaluationId != -1) {
        glDetachShader(_instance.shaderId, lTeCit->second.programId);
    }
}

void Shader::refreshInstanceState()
{
    for (IT_InstanceArray it = m_instances.begin(); it != m_instances.end(); it++) {
		String lOptions = (*it)->options;

        if (((*it)->vertexProgramId != -1) && ((m_vertexProgramArray[(*it)->vertexProgramId].programs[lOptions].programState & PROGRAM_COMPILED) != PROGRAM_COMPILED)) {
            continue;
        }

        if (((*it)->fragmentProgramId != -1) && ((m_fragmentProgramArray[(*it)->fragmentProgramId].programs[lOptions].programState & PROGRAM_COMPILED) != PROGRAM_COMPILED)) {
            continue;
        }

        if (((*it)->geometryProgramId != -1) && ((m_geometryProgramArray[(*it)->geometryProgramId].programs[lOptions].programState & PROGRAM_COMPILED) != PROGRAM_COMPILED)) {
            continue;
        }

        if (((*it)->tessControlProgramId != -1) && ((m_tessControlProgramArray[(*it)->tessControlProgramId].programs[lOptions].programState & PROGRAM_COMPILED) != PROGRAM_COMPILED)) {
            continue;
        }

        if (((*it)->tessEvaluationProgramId != -1) && ((m_tessEvaluationProgramArray[(*it)->tessEvaluationProgramId].programs[lOptions].programState & PROGRAM_COMPILED) != PROGRAM_COMPILED)) {
            continue;
        }

        (*it)->shaderState |= SHADER_COMPILED;
	}
}

void Shader::load(ProgramType programType, const String &programName, InStream &is)
{
    if (programName.isEmpty()) {
		O3D_ERROR(E_InvalidParameter("Shader : Program name is missing"));
    }

	ArrayChar data;
    Int32 count = is.getAvailable();

	data.setSize(count);

    is.read(data.getData(), count);

	addProgram(programType, programName, data.getData(), count);
}

void Shader::load(ProgramType programType, const String &programName, const String &filename)
{
    if (programName.isEmpty()) {
		O3D_ERROR(E_InvalidParameter("Shader : Program name is missing"));
    }

    AutoPtr<InStream> lis(FileManager::instance()->openInStream(filename));

    load(programType, programName, *lis);
}

void Shader::compileProgram(ProgramType _programType, Int32 _programIndex, const String & _options)
{
    if (!isLoaded()) {
		O3D_ERROR(E_InvalidOperation(String("Shader : Attempt to compile an unloaded program.")));
    }

    if (_programIndex < 0) {
		O3D_ERROR(E_IndexOutOfRange(String("Shader : _programIndex < 0")));
    }

	GLint lResult = 0;

	// transform the options string into multiple defines lines
	String options;

    if (_options.isValid()) {
		StringTokenizer optionsTokenizer(_options, ";");

        while (optionsTokenizer.hasMoreElements()) {
			String define = optionsTokenizer.nextElement();

			Int32 pos = define.find('=');
            if (pos != -1) {
				// define with value
				options += String("#define ") + define.sub(0, pos) + ' ' + define.sub(pos+1) + '\n';
            } else {
				// simple define without value
				options += String("#define ") + define + '\n';
			}
		}
	}

    T_ProgramArray *programArray = nullptr;
	GLuint programType = 0;
	String message;

	switch(_programType)
	{
		case TYPE_VERTEX_PROGRAM:
			programArray = &m_vertexProgramArray;
			programType = GL_VERTEX_SHADER;
			message = "VERTEX";
			break;
		case TYPE_FRAGMENT_PROGRAM:
			programArray = &m_fragmentProgramArray;
			programType = GL_FRAGMENT_SHADER;
			message = "FRAGMENT";
			break;
		case TYPE_GEOMETRY_PROGRAM:
			programArray = &m_geometryProgramArray;
            programType = GL_GEOMETRY_SHADER;
			message = "GEOMETRY";
			break;
        case TYPE_TESS_CONTROL_PROGRAM:
            programArray = &m_tessControlProgramArray;
            programType = GL_TESS_CONTROL_SHADER;
            message = "TESSCTRL";
            break;
        case TYPE_TESS_EVALUATION_PROGRAM:
            programArray = &m_tessEvaluationProgramArray;
            programType = GL_TESS_EVALUATION_SHADER;
            message = "TESSEVAL";
            break;
		default:
			O3D_ERROR(E_InvalidParameter(
					String("Shader : Invalid program type <") << UInt32(_programType) << '>'));
			break;
	}

    if (_programIndex >= Int32(programArray->size())) {
		O3D_ERROR(E_IndexOutOfRange(String("Shader : _index (") << _programIndex << ") > " << Int32(programArray->size())));
    }

	T_ProgramInfo & lProgramInfo = (*programArray)[_programIndex];
	T_ProgramInfo::T_Program & lProgram = lProgramInfo.programs[_options];

    if (lProgram.programState != PROGRAM_UNDEFINED) {
		O3D_ERROR(E_InvalidOperation(String("Shader : Trying to compile an already compiled or invalid program")));
    }

	O3D_ASSERT(lProgram.programId == 0);

    // @todo inject optimal version if necessary (@see GLES too)
	String programStr = lProgramInfo.programSource.getData();
	Int32 versionPos = programStr.sub("#version", 0);
    if (versionPos != -1) {
		versionPos = programStr.find('\n', versionPos);
		programStr.insert(options, versionPos+1);
    } else {
		programStr.insert(options, 0);
	}

	CString program = programStr.toAscii();

	const Char *lBufferSource = program.getData();
	const GLint lBufferSize = program.length();

	lProgram.programId = glCreateShader(programType);
	glShaderSource(lProgram.programId, 1, &lBufferSource, &lBufferSize);
	glCompileShader(lProgram.programId);
	glGetShaderiv(lProgram.programId, GL_COMPILE_STATUS, &lResult);

	// If the compilation failed
    if(lResult == GL_FALSE) {
		GLint lLogSize = 0;
		glGetShaderiv(lProgram.programId, GL_INFO_LOG_LENGTH, &lLogSize);

		ArrayChar lLogMessage(lLogSize);
        glGetShaderInfoLog(lProgram.programId, lLogSize, nullptr, lLogMessage.getData());

		glDeleteShader(lProgram.programId);
		lProgram.programId = 0;
		lProgram.programState = PROGRAM_INVALID;

		O3D_ERROR(E_InvalidOperation(String("Shader : Unable to compile the ") << message << " program <"
			<< lProgramInfo.programName << "> of the object <"
			<< m_name << "> contained in the file <" << m_programName << "> : " << lLogMessage.getData()));
    } else {
		GLint lLogSize = 0;
		glGetShaderiv(lProgram.programId, GL_INFO_LOG_LENGTH, &lLogSize);

        if (lLogSize > 1) {
			ArrayChar lLogMessage('\0', lLogSize+1, 0);
            glGetShaderInfoLog(lProgram.programId, lLogSize, nullptr, lLogMessage.getData());

			O3D_WARNING(String("Shader : Warning when compile the ") << message << " program <"
					<< lProgramInfo.programName << "> of the object <"
					<< m_name << "> contained in the file <" << m_programName << "> : " << lLogMessage.getData());
		}

		lProgram.programState = PROGRAM_COMPILED;
	}

	refreshInstanceState();
}

//Bool Shader::CompileAllPrograms(
//		T_ProgramIndexArray * _vProgramArray,
//		T_ProgramIndexArray * _fProgramArray,
//		T_ProgramIndexArray * _gProgramArray)
//{
//	if (!isLoaded()) {
//		O3D_ERROR(O3D_E_InvalidOperation(O3DString("Shader : Attempt to compile an unloaded object.")));
//  }

//	Bool lRet = True;

//	// vertex
//	for (UInt32 k = 0 ; k < UInt32(m_vertexProgramArray.size()) ; ++k)
//		if (m_vertexProgramArray[k].programState == PROGRAM_UNDEFINED)
//		{
//			try
//			{
//				CompileProgram(TYPE_VERTEX_PROGRAM, k, "");
//			}
//			catch(const O3D_E_InvalidOperation &)
//			{
//				if (_vProgramArray != nullptr)
//				{
//					_vProgramArray->push_back(k);
//					lRet = False;
//				}
//			}
//		}
//		else if ((m_vertexProgramArray[k].programState == PROGRAM_INVALID) && (_vProgramArray != nullptr))
//		{
//			_vProgramArray->push_back(k);
//			lRet = False;
//		}

//	// fragment
//	for (UInt32 k = 0 ; k < UInt32(m_fragmentProgramArray.size()) ; ++k)
//		if (m_fragmentProgramArray[k].programState == PROGRAM_UNDEFINED)
//		{
//			try
//			{
//				CompileProgram(TYPE_FRAGMENT_PROGRAM, k, "");
//			}
//			catch(const O3D_E_InvalidOperation &)
//			{
//				if (_fProgramArray != nullptr)
//				{
//					_fProgramArray->push_back(k);
//					lRet = False;
//				}
//			}
//		}
//		else if ((m_fragmentProgramArray[k].programState == PROGRAM_INVALID) && (_fProgramArray != nullptr))
//		{
//			_fProgramArray->push_back(k);
//			lRet = False;
//		}

//	// geometry
//	for (UInt32 k = 0 ; k < UInt32(m_geometryProgramArray.size()) ; ++k)
//		if (m_geometryProgramArray[k].programState == PROGRAM_UNDEFINED)
//		{
//			try
//			{
//				CompileProgram(TYPE_GEOMETRY_PROGRAM, k, "");
//			}
//			catch(const O3D_E_InvalidOperation &)
//			{
//				if (_gProgramArray != nullptr)
//				{
//					_gProgramArray->push_back(k);
//					lRet = False;
//				}
//			}
//		}
//		else if ((m_geometryProgramArray[k].programState == PROGRAM_INVALID) && (_gProgramArray != nullptr))
//		{
//			_gProgramArray->push_back(k);
//			lRet = False;
//		}

//	refreshInstanceState();

//	return lRet;
//}

void Shader::addProgram(
		ProgramType _programType,
		const String & _programName,
		const Char * _pBuffer,
		UInt32 _bufferSize)
{
    O3D_ASSERT(_pBuffer != nullptr);

	T_ProgramInfo lProgramInfo;
	lProgramInfo.programName = _programName;
	lProgramInfo.programType = _programType;

    if (_bufferSize == 0) {
		lProgramInfo.programSource = CString(_pBuffer);
    } else if (_pBuffer != nullptr) {
		lProgramInfo.programSource = CString(_pBuffer, _bufferSize-1);
    }

    switch(_programType) {
        case TYPE_VERTEX_PROGRAM:
            if (findVertexProgram(_programName) != -1) {
                O3D_ERROR(E_InvalidOperation(String("Shader : Attempt to add a program <") << _programName << "> whose name is already used"));
            } else {
                m_vertexProgramArray.push_back(lProgramInfo);
            }
            break;

        case TYPE_FRAGMENT_PROGRAM:
            if (findFragmentProgram(_programName) != -1) {
                O3D_ERROR(E_InvalidOperation(String("Shader : Attempt to add a program <") << _programName << "> whose name is already used"));
            } else {
                m_fragmentProgramArray.push_back(lProgramInfo);
            }
            break;

        case TYPE_GEOMETRY_PROGRAM:
            if (findGeometryProgram(_programName) != -1) {
                O3D_ERROR(E_InvalidOperation(String("Shader : Attempt to add a program <") << _programName << "> whose name is already used"));
            } else {
                m_geometryProgramArray.push_back(lProgramInfo);
            }
            break;

        case TYPE_TESS_CONTROL_PROGRAM:
            if (findTessControlProgram(_programName) != -1) {
                O3D_ERROR(E_InvalidOperation(String("Shader : Attempt to add a program <") << _programName << "> whose name is already used"));
            } else {
                m_tessControlProgramArray.push_back(lProgramInfo);
            }
            break;

        case TYPE_TESS_EVALUATION_PROGRAM:
            if (findTessEvaluationProgram(_programName) != -1) {
                O3D_ERROR(E_InvalidOperation(String("Shader : Attempt to add a program <") << _programName << "> whose name is already used"));
            } else {
                m_tessEvaluationProgramArray.push_back(lProgramInfo);
            }
            break;

        default:
            break;
	}
}

void Shader::removeProgram(ProgramType _programType, const String & _programName)
{
	Int32 lProgramId = -1;

    switch(_programType) {
        case TYPE_VERTEX_PROGRAM:
            if ((lProgramId = findVertexProgram(_programName)) == -1) {
                O3D_ERROR(E_InvalidOperation(String("Shader : Attempt to remove a unknown program <") << _programName << ">"));
            } else {
                T_ReferenceArray lToDetach;

                for (IT_ReferenceArray it = m_references.begin(); it != m_references.end(); it++) {
                    if ((*it)->getVertexProgramIndex() == lProgramId) {
                        lToDetach.push_back(*it);
                    }
                }

                for (IT_ReferenceArray it = lToDetach.begin() ; it != lToDetach.end() ; it++) {
                    (*it)->detach();
                }

                IT_ProgramArray programIt = m_vertexProgramArray.begin() + lProgramId;

                // delete any shader
                for (T_ProgramInfo::IT_ProgramMap it = programIt->programs.begin(); it != programIt->programs.end(); it++) {
                    if (it->second.programId != 0) {
                        glDeleteShader(it->second.programId);
                    }
                }

                // and the structure of the program
                m_vertexProgramArray.erase(programIt);
            }
            break;

        case TYPE_FRAGMENT_PROGRAM:
            if ((lProgramId = findFragmentProgram(_programName)) == -1) {
                O3D_ERROR(E_InvalidOperation(String("Shader : Attempt to remove a unknown program <") << _programName << ">"));
            } else {
                T_ReferenceArray lToDetach;

                for (IT_ReferenceArray it = m_references.begin(); it != m_references.end(); it++) {
                    if ((*it)->getFragmentProgramIndex() == lProgramId) {
                        lToDetach.push_back(*it);
                    }
                }

                for (IT_ReferenceArray it = lToDetach.begin() ; it != lToDetach.end() ; it++) {
                    (*it)->detach();
                }

                IT_ProgramArray programIt = m_fragmentProgramArray.begin() + lProgramId;

                // delete any shader
                for (T_ProgramInfo::IT_ProgramMap it = programIt->programs.begin(); it != programIt->programs.end(); it++) {
                    if (it->second.programId != 0) {
                        glDeleteShader(it->second.programId);
                    }
                }

                // and the structure of the program
                m_fragmentProgramArray.erase(programIt);
            }
            break;

        case TYPE_GEOMETRY_PROGRAM:
            if ((lProgramId = findGeometryProgram(_programName)) == -1) {
                O3D_ERROR(E_InvalidOperation(String("Shader : Attempt to remove a unknown program <") << _programName << ">"));
            } else {
                T_ReferenceArray lToDetach;

                for (IT_ReferenceArray it = m_references.begin(); it != m_references.end(); it++) {
                    if ((*it)->getGeometryProgramIndex() == lProgramId) {
                        lToDetach.push_back(*it);
                    }
                }

                for (IT_ReferenceArray it = lToDetach.begin() ; it != lToDetach.end() ; it++) {
                    (*it)->detach();
                }

                IT_ProgramArray programIt = m_geometryProgramArray.begin() + lProgramId;

                // delete any shader
                for (T_ProgramInfo::IT_ProgramMap it = programIt->programs.begin(); it != programIt->programs.end(); it++) {
                    if (it->second.programId != 0) {
                        glDeleteShader(it->second.programId);
                    }
                }

                // and the structure of the program
                m_geometryProgramArray.erase(programIt);
            }
            break;

        case TYPE_TESS_CONTROL_PROGRAM:
            if ((lProgramId = findTessControlProgram(_programName)) == -1) {
                O3D_ERROR(E_InvalidOperation(String("Shader : Attempt to remove a unknown program <") << _programName << ">"));
            } else {
                T_ReferenceArray lToDetach;

                for (IT_ReferenceArray it = m_references.begin(); it != m_references.end(); it++) {
                    if ((*it)->getTessControlProgramIndex() == lProgramId) {
                        lToDetach.push_back(*it);
                    }
                }

                for (IT_ReferenceArray it = lToDetach.begin() ; it != lToDetach.end() ; it++) {
                    (*it)->detach();
                }

                IT_ProgramArray programIt = m_tessControlProgramArray.begin() + lProgramId;

                // delete any shader
                for (T_ProgramInfo::IT_ProgramMap it = programIt->programs.begin(); it != programIt->programs.end(); it++) {
                    if (it->second.programId != 0) {
                        glDeleteShader(it->second.programId);
                    }
                }

                // and the structure of the program
                m_tessControlProgramArray.erase(programIt);
            }
            break;

        case TYPE_TESS_EVALUATION_PROGRAM:
            if ((lProgramId = findTessEvaluationProgram(_programName)) == -1) {
                O3D_ERROR(E_InvalidOperation(String("Shader : Attempt to remove a unknown program <") << _programName << ">"));
            } else {
                T_ReferenceArray lToDetach;

                for (IT_ReferenceArray it = m_references.begin(); it != m_references.end(); it++) {
                    if ((*it)->getTessEvaluationProgramIndex() == lProgramId) {
                        lToDetach.push_back(*it);
                    }
                }

                for (IT_ReferenceArray it = lToDetach.begin() ; it != lToDetach.end() ; it++) {
                    (*it)->detach();
                }

                IT_ProgramArray programIt = m_tessEvaluationProgramArray.begin() + lProgramId;

                // delete any shader
                for (T_ProgramInfo::IT_ProgramMap it = programIt->programs.begin(); it != programIt->programs.end(); it++) {
                    if (it->second.programId != 0) {
                        glDeleteShader(it->second.programId);
                    }
                }

                // and the structure of the program
                m_tessEvaluationProgramArray.erase(programIt);
            }
            break;

        default:
        {
            O3D_ASSERT(0);
            break;
        }
	}
}

void Shader::destroy()
{
    m_programName.destroy();
	m_name.destroy();

    for (IT_ProgramArray it = m_vertexProgramArray.begin(); it != m_vertexProgramArray.end(); it++) {
        for (T_ProgramInfo::IT_ProgramMap it2 = it->programs.begin(); it2 != it->programs.end(); it2++) {
            if (it2->second.programId != 0) {
				glDeleteShader(it2->second.programId);
            }
        }
	}

    for (IT_ProgramArray it = m_fragmentProgramArray.begin() ; it != m_fragmentProgramArray.end() ; it++) {
        for (T_ProgramInfo::IT_ProgramMap it2 = it->programs.begin(); it2 != it->programs.end(); it2++) {
            if (it2->second.programId != 0) {
				glDeleteShader(it2->second.programId);
            }
        }
	}

    for (IT_ProgramArray it = m_geometryProgramArray.begin() ; it != m_geometryProgramArray.end() ; it++) {
        for (T_ProgramInfo::IT_ProgramMap it2 = it->programs.begin(); it2 != it->programs.end(); it2++) {
            if (it2->second.programId != 0) {
				glDeleteShader(it2->second.programId);
            }
        }
	}

    for (IT_ProgramArray it = m_tessControlProgramArray.begin() ; it != m_tessControlProgramArray.end() ; it++) {
        for (T_ProgramInfo::IT_ProgramMap it2 = it->programs.begin(); it2 != it->programs.end(); it2++) {
            if (it2->second.programId != 0) {
                glDeleteShader(it2->second.programId);
            }
        }
    }

    for (IT_ProgramArray it = m_tessEvaluationProgramArray.begin() ; it != m_tessEvaluationProgramArray.end() ; it++) {
        for (T_ProgramInfo::IT_ProgramMap it2 = it->programs.begin(); it2 != it->programs.end(); it2++) {
            if (it2->second.programId != 0) {
                glDeleteShader(it2->second.programId);
            }
        }
    }

	m_vertexProgramArray.clear();
	m_fragmentProgramArray.clear();
    m_geometryProgramArray.clear();
    m_tessControlProgramArray.clear();
    m_tessEvaluationProgramArray.clear();

	T_ReferenceArray lArray = m_references;

    for (std::vector<ShaderInstance*>::iterator itInstance = lArray.begin(); itInstance != lArray.end(); itInstance++) {
		((*itInstance)->detach());
    }

	O3D_ASSERT(m_instances.empty());
}

void Shader::buildInstance(ShaderInstance & _instance) const
{
	_instance.attach(const_cast<Shader*>(this));
}

const String& Shader::getVertexProgramName(Int32 _vertexIndex) const
{
    if ((_vertexIndex < 0) || (_vertexIndex >= Int32(m_vertexProgramArray.size()))) {
		O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid vertex index")));
    }

	return m_vertexProgramArray[_vertexIndex].programName;
}

const String& Shader::getFragmentProgramName(Int32 _fragmentIndex) const
{
    if ((_fragmentIndex < 0) || (_fragmentIndex >= Int32(m_fragmentProgramArray.size()))) {
		O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid fragment index")));
    }

	return m_fragmentProgramArray[_fragmentIndex].programName;
}

const String& Shader::getGeometryProgramName(Int32 _geometryIndex) const
{
    if ((_geometryIndex < 0) || (_geometryIndex >= Int32(m_geometryProgramArray.size()))) {
		O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid geometry index")));
    }

    return m_geometryProgramArray[_geometryIndex].programName;
}

const String &Shader::getTessControlProgramName(Int32 _tessControlIndex) const
{
    if ((_tessControlIndex < 0) || (_tessControlIndex >= Int32(m_tessControlProgramArray.size()))) {
        O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid tesselation control index")));
    }

    return m_tessControlProgramArray[_tessControlIndex].programName;
}

const String &Shader::getTessEvaluationProgramName(Int32 _tessEvaluationIndex) const
{
    if ((_tessEvaluationIndex < 0) || (_tessEvaluationIndex >= Int32(m_tessEvaluationProgramArray.size()))) {
        O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid tesselation evaluation index")));
    }

    return m_tessEvaluationProgramArray[_tessEvaluationIndex].programName;
}

//! Return the source of a given vertex program
const CString& Shader::getVertexProgramSource(Int32 _vertexIndex) const
{
    if ((_vertexIndex < 0) || (_vertexIndex >= Int32(m_vertexProgramArray.size()))) {
		O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid vertex index")));
    }

	return m_vertexProgramArray[_vertexIndex].programSource;
}

//! Return the source of a given fragment program
const CString& Shader::getFragmentProgramSource(Int32 _fragmentIndex) const
{
    if ((_fragmentIndex < 0) || (_fragmentIndex >= Int32(m_fragmentProgramArray.size()))) {
		O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid fragment index")));
    }

	return m_fragmentProgramArray[_fragmentIndex].programSource;
}

const CString& Shader::getGeometryProgramSource(Int32 _geometryIndex) const
{
    if ((_geometryIndex < 0) || (_geometryIndex >= Int32(m_geometryProgramArray.size()))) {
		O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid geometry index")));
    }

	return m_geometryProgramArray[_geometryIndex].programSource;
}

const CString& Shader::getTessControlProgramSource(Int32 _tessControlIndex) const
{
    if ((_tessControlIndex < 0) || (_tessControlIndex >= Int32(m_tessControlProgramArray.size()))) {
        O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid tesselation control index")));
    }

    return m_tessControlProgramArray[_tessControlIndex].programSource;
}

const CString& Shader::getTessEvaluationProgramSource(Int32 _tessEvaluationIndex) const
{
    if ((_tessEvaluationIndex < 0) || (_tessEvaluationIndex >= Int32(m_tessEvaluationProgramArray.size()))) {
        O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid tesselation evaluation index")));
    }

    return m_tessEvaluationProgramArray[_tessEvaluationIndex].programSource;
}

Shader::ProgramState Shader::getVertexProgramState(Int32 _vertexIndex, const String & _options) const
{
    if ((_vertexIndex < 0) || (_vertexIndex >= Int32(m_vertexProgramArray.size()))) {
		O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid vertex index")));
    }

	T_ProgramInfo::CIT_ProgramMap cit = m_vertexProgramArray[_vertexIndex].programs.find(_options);

    if (cit == m_vertexProgramArray[_vertexIndex].programs.end()) {
		O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid vertex options")));
    }

	return cit->second.programState;
}

Shader::ProgramState Shader::getFragmentProgramState(Int32 _fragmentIndex, const String & _options) const
{
    if ((_fragmentIndex < 0) || (_fragmentIndex >= Int32(m_fragmentProgramArray.size()))) {
		O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid fragment index")));
    }

	T_ProgramInfo::CIT_ProgramMap cit = m_fragmentProgramArray[_fragmentIndex].programs.find(_options);

    if (cit == m_fragmentProgramArray[_fragmentIndex].programs.end()) {
		O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid fragment options")));
    }

	return cit->second.programState;
}

Shader::ProgramState Shader::getGeometryProgramState(Int32 _geometryIndex, const String & _options) const
{
    if ((_geometryIndex < 0) || (_geometryIndex >= Int32(m_geometryProgramArray.size()))) {
		O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid geometry index")));
    }

	T_ProgramInfo::CIT_ProgramMap cit = m_geometryProgramArray[_geometryIndex].programs.find(_options);

    if (cit == m_geometryProgramArray[_geometryIndex].programs.end()) {
		O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid geometry options")));
    }

	return cit->second.programState;
}

Shader::ProgramState Shader::getTessControlProgramState(Int32 _tessControlIndex, const String & _options) const
{
    if ((_tessControlIndex < 0) || (_tessControlIndex >= Int32(m_tessControlProgramArray.size()))) {
        O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid tesselation control index")));
    }

    T_ProgramInfo::CIT_ProgramMap cit = m_tessControlProgramArray[_tessControlIndex].programs.find(_options);

    if (cit == m_tessControlProgramArray[_tessControlIndex].programs.end()) {
        O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid tesselation control options")));
    }

    return cit->second.programState;
}

Shader::ProgramState Shader::getTessEvaluationProgramState(Int32 _tessEvaluationIndex, const String & _options) const
{
    if ((_tessEvaluationIndex < 0) || (_tessEvaluationIndex >= Int32(m_tessEvaluationProgramArray.size()))) {
        O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid tesselation evaluation index")));
    }

    T_ProgramInfo::CIT_ProgramMap cit = m_tessEvaluationProgramArray[_tessEvaluationIndex].programs.find(_options);

    if (cit == m_tessEvaluationProgramArray[_tessEvaluationIndex].programs.end()) {
        O3D_ERROR(E_IndexOutOfRange(String("Shader : Invalid tesselation evaluation options")));
    }

    return cit->second.programState;
}

Int32 Shader::findVertexProgram(const String & _programName) const
{
	Int32 lIndex = 0;

    for (CIT_ProgramArray it = m_vertexProgramArray.begin() ; it != m_vertexProgramArray.end() ; it++, ++lIndex) {
        if (it->programName.compare(_programName, String::CASE_INSENSITIVE) == 0) {
			return lIndex;
        }
	}

	return -1;
}

Int32 Shader::findFragmentProgram(const String & _programName) const
{
	Int32 lIndex = 0;

    for (CIT_ProgramArray it = m_fragmentProgramArray.begin() ; it != m_fragmentProgramArray.end() ; it++, ++lIndex) {
        if (it->programName.compare(_programName, String::CASE_INSENSITIVE) == 0) {
			return lIndex;
        }
	}

	return -1;
}

Int32 Shader::findGeometryProgram(const String & _programName) const
{
	Int32 lIndex = 0;

    for (CIT_ProgramArray it = m_geometryProgramArray.begin() ; it != m_geometryProgramArray.end() ; it++, ++lIndex) {
        if (it->programName.compare(_programName, String::CASE_INSENSITIVE) == 0) {
			return lIndex;
        }
	}

	return -1;
}

Int32 Shader::findTessControlProgram(const String & _programName) const
{
    Int32 lIndex = 0;

    for (CIT_ProgramArray it = m_tessControlProgramArray.begin() ; it != m_tessControlProgramArray.end() ; it++, ++lIndex) {
        if (it->programName.compare(_programName, String::CASE_INSENSITIVE) == 0) {
            return lIndex;
        }
    }

    return -1;
}

Int32 Shader::findTessEvaluationProgram(const String & _programName) const
{
    Int32 lIndex = 0;

    for (CIT_ProgramArray it = m_tessEvaluationProgramArray.begin() ; it != m_tessEvaluationProgramArray.end() ; it++, ++lIndex) {
        if (it->programName.compare(_programName, String::CASE_INSENSITIVE) == 0) {
            return lIndex;
        }
    }

    return -1;
}

//---------------------------------------------------------------------------------------
// ShaderInstance
//---------------------------------------------------------------------------------------

ShaderInstance::ShaderInstance():
    m_shader(nullptr),
    m_pInstance(nullptr),
	m_uniformLocations(),
	m_attribLocations()
{
}

ShaderInstance::ShaderInstance(const ShaderInstance & _which):
    m_shader(nullptr),
    m_pInstance(nullptr),
	m_uniformLocations(_which.m_uniformLocations),
	m_attribLocations(_which.m_attribLocations)
{
	attach(_which.m_shader);

    if ((m_pInstance = _which.m_pInstance) != nullptr) {
		++(m_pInstance->refCounter);
    }
}

ShaderInstance::~ShaderInstance()
{
	detach();
}

ShaderInstance & ShaderInstance::operator = (const ShaderInstance & _which)
{
    if (this == &_which) {
		return *this;
    }

	detach();

	m_shader = _which.m_shader;
	m_pInstance = _which.m_pInstance;

    if (m_shader != nullptr) {
		//m_pShader->useIt();
		m_shader->m_references.push_back(this);
	}

    if (m_pInstance != nullptr) {
		++m_pInstance->refCounter;
    }

	m_uniformLocations = _which.m_uniformLocations;
	m_attribLocations = _which.m_attribLocations;

	return *this;
}

void ShaderInstance::attach(Shader* _pShading)
{
    if (m_shader != _pShading) {
		detach();

		m_shader = _pShading;

        if (m_shader != nullptr) {
			//m_pShader->useIt();
			m_shader->m_references.push_back(this);
		}
	}
}

void ShaderInstance::detach()
{
    if (m_pInstance != nullptr) {
        if (m_pInstance->pOwner == this) {
            // It means this instance has bound the program
            O3D_ASSERT(isInUse());

			O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Attempt to detach a currently used shader")));

			unbindShader();
		}

        if (--(m_pInstance->refCounter) == 0) {
			Shader::IT_InstanceArray itInstance =  std::find(m_shader->m_instances.begin(), m_shader->m_instances.end(), m_pInstance);
			O3D_ASSERT(itInstance != m_shader->m_instances.end());

            if (m_pInstance->shaderId != 0) {
				glDeleteProgram(m_pInstance->shaderId);
            }

			deletePtr(*itInstance);
			m_shader->m_instances.erase(itInstance);
		}

        m_pInstance = nullptr;
	}

    if (m_shader != nullptr) {
		std::vector<ShaderInstance*>::iterator it = std::find(m_shader->m_references.begin(), m_shader->m_references.end(), this);
		O3D_ASSERT(it != m_shader->m_references.end());

		m_shader->m_references.erase(it);
		//m_pShader->releaseIt();
        m_shader = nullptr;
    }
}

//! bind the shader program
void ShaderInstance::bindShader()
{
    if (!isLoaded()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Object not loaded")));
    } else if (!isDefined()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Attempt to bind a shader, but you need to select a vertex/fragment program first")));
    } else if (isInUse()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Attempt to bind twice the same shader")));
    } else {
        if (!isCompiled()) {
			m_shader->compileInstance(*m_pInstance);
        }

        if (!isLinked()) {
			m_shader->linkInstance(*m_pInstance);
        }

		#ifdef _DEBUG
            if (m_shader->getScene()->getContext()->getCurrentShader() != 0) {
				O3D_ERROR(E_InvalidOperation(String("ShaderInstance : You must first unbound the current shader")));
            }
		#endif

        O3D_ASSERT(m_pInstance->pOwner == nullptr);
		
		m_shader->getScene()->getContext()->bindShader(m_pInstance->shaderId);

		m_pInstance->shaderState |= Shader::SHADER_INUSE;
		m_pInstance->pOwner = this;
	}
}

//! unbound the shader program
void ShaderInstance::unbindShader()
{
    if (!isInUse()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Attempt to unbound a shader which is not currently running")));
    } else {
		#ifdef _DEBUG
            if (m_shader->getScene()->getContext()->getCurrentShader() != m_pInstance->shaderId) {
				O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Shader's state is not synchronized with GLContext")));
            }
		#endif

		O3D_ASSERT(m_pInstance->pOwner == this);

		m_shader->getScene()->getContext()->bindShader(0);

		m_pInstance->shaderState &= ~Shader::SHADER_INUSE;
        m_pInstance->pOwner = nullptr;
	}
}

void ShaderInstance::assign(
        Int32 _vertexIndex,
        Int32 _fragmentIndex,
        Int32 _geometryIndex,
        Int32 _tessControlIndex,
        Int32 _tessEvaluationIndex,
        const String & _options,
        Shader::BuildType _type)
{
    if (!isLoaded()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Object not loaded")));
    }

    if ((_vertexIndex >= Int32(m_shader->m_vertexProgramArray.size())) || (_vertexIndex < 0)) {
        O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid vertex program index")));
    }

    if ((_fragmentIndex >= Int32(m_shader->m_fragmentProgramArray.size())) || (_fragmentIndex < 0)) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid fragment program index")));
    }

    if (_geometryIndex >= Int32(m_shader->m_geometryProgramArray.size())) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid geometry program index")));
    }

    if (_tessControlIndex >= Int32(m_shader->m_tessControlProgramArray.size())) {
        O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid tesselation program index")));
    }

    if (_tessEvaluationIndex >= Int32(m_shader->m_tessEvaluationProgramArray.size())) {
        O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid evaluation program index")));
    }

    if (isDefined()) {
        if (--(m_pInstance->refCounter) == 0) {
			Shader::IT_InstanceArray itInstance =  std::find(m_shader->m_instances.begin(), m_shader->m_instances.end(), m_pInstance);
			O3D_ASSERT(itInstance != m_shader->m_instances.end());

            if (m_pInstance->shaderId != 0) {
				glDeleteProgram(m_pInstance->shaderId);
            }

			deletePtr(*itInstance);
			m_shader->m_instances.erase(itInstance);
		}

        m_pInstance = nullptr;
	}

    for (Shader::IT_InstanceArray it = m_shader->m_instances.begin() ; it != m_shader->m_instances.end() ; it++) {
        if (((*it)->vertexProgramId == _vertexIndex) &&
            ((*it)->fragmentProgramId == _fragmentIndex) &&
            ((*it)->geometryProgramId == _geometryIndex) &&
            ((*it)->tessControlProgramId == _tessControlIndex) &&
            ((*it)->tessEvaluationProgramId == _tessEvaluationIndex) &&
            ((*it)->options == _options)) {

			m_pInstance = (*it);
			++(m_pInstance->refCounter);
		}
    }

    if (m_pInstance == nullptr) {
		Shader::T_InstanceInfo * lInfo = new Shader::T_InstanceInfo;
		lInfo->vertexProgramId = _vertexIndex;
		lInfo->fragmentProgramId = _fragmentIndex;
		lInfo->geometryProgramId = _geometryIndex;
        lInfo->tessControlProgramId = _tessControlIndex;
        lInfo->tessEvaluationProgramId = _tessEvaluationIndex;
		lInfo->options = _options;
		lInfo->shaderId = 0;
		lInfo->shaderState = Shader::SHADER_DEFINED | Shader::SHADER_LOADED;
		lInfo->refCounter = 0;
        lInfo->pOwner = nullptr;

		m_shader->m_instances.push_back(lInfo);

		m_pInstance = m_shader->m_instances.back();
		m_pInstance->refCounter++;
	}

    if (_type != Shader::BUILD_DEFINE) {
		build(_type);
    }
}

void ShaderInstance::assign(
        Int32 _vertexIndex,
        Int32 _fragmentIndex,
        const String & _options,
        Shader::BuildType _type)
{
    if (!isLoaded()) {
        O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Object not loaded")));
    }

    if ((_vertexIndex >= Int32(m_shader->m_vertexProgramArray.size())) || (_vertexIndex < 0)) {
        O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid vertex program index")));
    }

    if ((_fragmentIndex >= Int32(m_shader->m_fragmentProgramArray.size())) || (_fragmentIndex < 0)) {
        O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid fragment program index")));
    }

    if (isDefined()) {
        if (--(m_pInstance->refCounter) == 0) {
            Shader::IT_InstanceArray itInstance =  std::find(m_shader->m_instances.begin(), m_shader->m_instances.end(), m_pInstance);
            O3D_ASSERT(itInstance != m_shader->m_instances.end());

            if (m_pInstance->shaderId != 0) {
                glDeleteProgram(m_pInstance->shaderId);
            }

            deletePtr(*itInstance);
            m_shader->m_instances.erase(itInstance);
        }

        m_pInstance = nullptr;
    }

    for (Shader::IT_InstanceArray it = m_shader->m_instances.begin() ; it != m_shader->m_instances.end() ; it++) {
        if (((*it)->vertexProgramId == _vertexIndex) &&
            ((*it)->fragmentProgramId == _fragmentIndex) &&
            ((*it)->options == _options)) {

            m_pInstance = (*it);
            ++(m_pInstance->refCounter);
        }
    }

    if (m_pInstance == nullptr) {
        Shader::T_InstanceInfo * lInfo = new Shader::T_InstanceInfo;
        lInfo->vertexProgramId = _vertexIndex;
        lInfo->fragmentProgramId = _fragmentIndex;
        lInfo->geometryProgramId = -1;
        lInfo->tessControlProgramId = -1;
        lInfo->tessEvaluationProgramId = -1;
        lInfo->options = _options;
        lInfo->shaderId = 0;
        lInfo->shaderState = Shader::SHADER_DEFINED | Shader::SHADER_LOADED;
        lInfo->refCounter = 0;
        lInfo->pOwner = nullptr;

        m_shader->m_instances.push_back(lInfo);

        m_pInstance = m_shader->m_instances.back();
        m_pInstance->refCounter++;
    }

    if (_type != Shader::BUILD_DEFINE) {
        build(_type);
    }
}

void ShaderInstance::assign(
		const String & _vertexProgram,
		const String & _fragmentProgram,
		const String & _geometryProgram,
        const String & _tessControlProgram,
        const String & _tessEvaluationProgram,
		const String & _options,
		Shader::BuildType _type)
{
    if (!isLoaded()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Object not loaded")));
    }

    if (_vertexProgram.isEmpty()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid vertex program name")));
    }

    if (_fragmentProgram.isEmpty()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid fragment program name")));
    }

	Int32 lVertexIndex = m_shader->findVertexProgram(_vertexProgram);

    if (lVertexIndex == -1) {
        O3D_ERROR(E_InvalidParameter(String("ShaderInstance : Unknown vertex program name <") << _vertexProgram << '>'));
    }

	Int32 lFragmentIndex = m_shader->findFragmentProgram(_fragmentProgram);

    if (lFragmentIndex == -1) {
		O3D_ERROR(E_InvalidParameter(String("ShaderInstance : Unknown fragment program name <") << _fragmentProgram << '>'));
    }

	// optional geometry program
	Int32 lGeometryIndex = -1;

    if (_geometryProgram.isValid()) {
		lGeometryIndex = m_shader->findGeometryProgram(_geometryProgram);

        if (lGeometryIndex == -1) {
			O3D_ERROR(E_InvalidParameter(String("ShaderInstance : Unknown geometry program name <") << _geometryProgram << '>'));
        }
	}

    // optional tesselation program
    Int32 lTessControlIndex = -1;
    Int32 lTessEvaluationIndex = -1;

    if (_tessControlProgram.isValid() && _tessEvaluationProgram.isValid()) {
        lTessControlIndex = m_shader->findTessControlProgram(_tessControlProgram);
        lTessEvaluationIndex = m_shader->findTessEvaluationProgram(_tessEvaluationProgram);

        if (lTessControlIndex == -1) {
            O3D_ERROR(E_InvalidParameter(String("ShaderInstance : Unknown tesselation control program name <") << _tessControlProgram << '>'));
        }

        if (lTessEvaluationIndex == -1) {
            O3D_ERROR(E_InvalidParameter(String("ShaderInstance : Unknown tesselation evaluation program name <") << _tessEvaluationProgram << '>'));
        }
    }

    assign(lVertexIndex, lFragmentIndex, lGeometryIndex, lTessControlIndex, lTessEvaluationIndex, _options, _type);
}

void ShaderInstance::assign(
        const String & _vertexProgram,
        const String & _fragmentProgram,
        const String & _options,
        Shader::BuildType _type)
{
    if (!isLoaded()) {
        O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Object not loaded")));
    }

    if (_vertexProgram.isEmpty()) {
        O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid vertex program name")));
    }

    if (_fragmentProgram.isEmpty()) {
        O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid fragment program name")));
    }

    Int32 lVertexIndex = m_shader->findVertexProgram(_vertexProgram);

    if (lVertexIndex == -1) {
        O3D_ERROR(E_InvalidParameter(String("ShaderInstance : Unknown vertex program name <") << _vertexProgram << '>'));
    }

    Int32 lFragmentIndex = m_shader->findFragmentProgram(_fragmentProgram);

    if (lFragmentIndex == -1) {
        O3D_ERROR(E_InvalidParameter(String("ShaderInstance : Unknown fragment program name <") << _fragmentProgram << '>'));
    }

    assign(lVertexIndex, lFragmentIndex, -1, -1, -1, _options, _type);
}

void ShaderInstance::build(Shader::BuildType _type)
{
    if (!isLoaded()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Object not loaded")));
    }

    if (!isDefined()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Object not defined")));
    }

    if ((_type == Shader::BUILD_COMPILE) && (!isCompiled())) {
		m_shader->compileInstance(*m_pInstance);
    } else if (_type == Shader::BUILD_COMPILE_AND_LINK) {
        if (!isCompiled()) {
			m_shader->compileInstance(*m_pInstance);
        }

        if (!isLinked()) {
			m_shader->linkInstance(*m_pInstance);
        }
	}
}

void ShaderInstance::setConstInt(const Char* name,const Int32 constant)
{
    if (!isInUse()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));
    }

	Int32 location = glGetUniformLocation(m_pInstance->shaderId,name);
    if (location >= 0) {
		glUniform1i(location,constant);
    } else {
        O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid uniform variable <") << name << " >"));
    }
}

void ShaderInstance::setConstUInt(const Char* name,const UInt32 constant)
{
    if (!isInUse()) {
        O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));
    }

    Int32 location = glGetUniformLocation(m_pInstance->shaderId,name);
    if (location >= 0) {
        glUniform1ui(location,constant);
    } else {
        O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid uniform variable <") << name << " >"));
    }
}

void ShaderInstance::setConstBool(const Char* name,const Bool constant)
{
    if (!isInUse()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));
    }

	Int32 location = glGetUniformLocation(m_pInstance->shaderId,name);
    if (location >= 0) {
		glUniform1i(location,Int32(constant));
    } else {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid uniform variable <") << name << " >"));
    }
}

void ShaderInstance::setConstFloat(const Char* name,const Float constant)
{
	if (!isInUse())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));

	Int32 location = glGetUniformLocation(m_pInstance->shaderId,name);
	if (location >= 0)
		glUniform1f(location,constant);
	else
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid uniform variable <") << name << " >"));
}

void ShaderInstance::setConstVector2(const Char* name,const Vector2f& constant)
{
    if (!isInUse()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));
    }

	Int32 location = glGetUniformLocation(m_pInstance->shaderId,name);
    if (location >= 0) {
		glUniform2fv(location,1,constant.getData());
    } else {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid uniform variable <") << name << " >"));
    }
}

void ShaderInstance::setConstColor(const Char* name,const Color& constant)
{
    if (!isInUse()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));
    }

	Int32 location = glGetUniformLocation(m_pInstance->shaderId,name);
    if (location >= 0) {
		glUniform4fv(location,1,constant.getData());
    } else {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid uniform variable <") << name << " >"));
    }
}

void ShaderInstance::setConstVector3(const Char* name,const Vector3& constant)
{
    if (!isInUse()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));
    }

	Int32 location = glGetUniformLocation(m_pInstance->shaderId,name);
    if (location >= 0) {
		glUniform3fv(location,1,constant.getData());
    } else {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid uniform variable <") << name << " >"));
    }
}

void ShaderInstance::setConstVector4(const Char* name,const Vector3& constant)
{
    if (!isInUse()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));
    }

	Int32 location = glGetUniformLocation(m_pInstance->shaderId,name);
    if (location >= 0) {
		glUniform4f(location,constant[X],constant[Y],constant[Z],1.0f);
    } else {
        O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid uniform variable <") << name << " >"));
    }
}

void ShaderInstance::setConstVector4(const Char* name,const Vector4& constant)
{
    if (!isInUse()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));
    }

	Int32 location = glGetUniformLocation(m_pInstance->shaderId,name);
    if (location >= 0) {
		glUniform4fv(location,1,constant.getData());
    } else {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid uniform variable <") << name << " >"));
    }
}

void ShaderInstance::setConstMatrix3(
	const Char* name,
	const Bool transpose,
	const Matrix3& constant)
{
    if (!isInUse()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));
    }

	Int32 location = glGetUniformLocation(m_pInstance->shaderId,name);
    if (location >= 0) {
		glUniformMatrix3fv(location,1,transpose,constant.getData());
    } else {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid uniform variable <") << name << " >"));
    }
}

void ShaderInstance::setConstMatrix4(
	const Char* name,
	const Bool transpose,
	const Matrix4& constant)
{
    if (!isInUse()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));
    }

	Int32 location = glGetUniformLocation(m_pInstance->shaderId,name);
    if (location >= 0) {
		glUniformMatrix4fv(location,1,transpose,constant.getData());
    } else {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid uniform variable <") << name << " >"));
    }
}

void ShaderInstance::setNConstMatrix4(
	const Char* name,
	Int32 num,
	const Bool transpose,
	const Float* constant)
{
    if (!isInUse()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));
    }

	Int32 location = glGetUniformLocation(m_pInstance->shaderId,name);
    if (location >= 0) {
		glUniformMatrix4fv(location,num,transpose,constant);
    } else {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid uniform variable <") << name << " >"));
    }
}

void ShaderInstance::setConstTexture(const Char* name, Texture* pTexture, Int32 texUnit)
{
    if (!isInUse()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define define a texture if the shader is not bound")));
    }

    if (pTexture != nullptr) {
		Int32 location = glGetUniformLocation(m_pInstance->shaderId,name);

        if (location >= 0) {
			pTexture->getScene()->getContext()->setActiveTextureUnit(texUnit);
			pTexture->bind();
			glUniform1i(location,texUnit);
        } else {
			O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid texture name <") << name << " >"));
        }
    } else {
		O3D_ERROR(E_InvalidParameter(String("ShaderInstance : Null texture")));
    }
}

void ShaderInstance::setConstTexture(Int32 Location, Texture* pTexture, Int32 texUnit)
{
    if (!isInUse()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));
    }

    if (pTexture != nullptr) {
        if (Location >= 0) {
			pTexture->getScene()->getContext()->setActiveTextureUnit(texUnit);
			pTexture->bind();
			glUniform1i(Location,texUnit);
        } else {
			O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Invalid texture location <") << pTexture->getName() << " >"));
        }
    } else {
		O3D_ERROR(E_InvalidParameter(String("ShaderInstance : Null texture")));
    }
}

#include <o3d/engine/uniformbuffer.h>

UInt32 ShaderInstance::getUniformBlockIndex(const Char *name)
{
    if (!isLinked()) {
        O3D_ERROR(E_InvalidOperation(String("ShaderInstance : The shader must be linked to be able to get uniform block index")));
    }

    UInt32 lIndex = glGetUniformBlockIndex(m_pInstance->shaderId, name);
    return lIndex;
}

void ShaderInstance::setUniformBlock(const Char *name, UniformBuffer &uniformBuffer, UInt32 bindingPoint)
{
    if (!isInUse()) {
        O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define define an uniform block if the shader is not bound")));
    }

    UInt32 blockIndex = glGetUniformBlockIndex(m_pInstance->shaderId, name);
    uniformBuffer.bindBufferBase(bindingPoint);
    glUniformBlockBinding(m_pInstance->shaderId, blockIndex, bindingPoint);
}

void ShaderInstance::setUniformBlock(UInt32 blockIndex, UniformBuffer &uniformBuffer, UInt32 bindingPoint)
{
    if (!isInUse()) {
        O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define define an uniform block if the shader is not bound")));
    }

    uniformBuffer.bindBufferBase(bindingPoint);
    glUniformBlockBinding(m_pInstance->shaderId, blockIndex, bindingPoint);
}

Int32 ShaderInstance::getAttributeLocation(const Char *name) const
{
    if (!isLinked()) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : The shader must be linked to be able to get uniform variable location")));
    }

    Int32 lLocation = glGetAttribLocation(m_pInstance->shaderId, name);

    if (lLocation == -1) {
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : There is not attribute called <") << name << "> in the program <" << getProgramName() << ">"));
    } else {
		return lLocation;
    }
}

//-----------------------------------------------------------------------------------
// LOCATION CONTAINER
//-----------------------------------------------------------------------------------

void ShaderInstance::setUniform(Int32 _key, Int32 _value)
{
	O3D_ASSERT((_key >= 0) && (_key < 128));

    if ((_key < 0) || (_key >= 128)) {
		O3D_ERROR(E_InvalidOperation("Invalid key : must be <= 128"));
    }

    if (UInt32(_key) >= m_uniformLocations.size()) {
		m_uniformLocations.resize(1+_key, Int32(O3D_INFINITE));
    }

	m_uniformLocations[_key] = _value;
}

Int32 ShaderInstance::getUniform(Int32 _key) const
{
	O3D_ASSERT((_key >= 0) && (_key < 128));

    if ((_key < 0) || (_key >= 128)) {
		O3D_ERROR(E_InvalidOperation("Invalid uniform key : must be <= 128"));
    }

    if ((UInt32(_key) >= m_uniformLocations.size()) || (m_uniformLocations[_key] == Int32(O3D_INFINITE))) {
		O3D_ERROR(E_InvalidOperation(String::print("Uniform key %d is not assigned", _key)));
    }

	return m_uniformLocations[_key];
}

void ShaderInstance::setAttribute(Int32 _key, UInt32 _value)
{
	O3D_ASSERT((_key >= 0) && (_key < 128));

    if ((_key < 0) || (_key >= 128)) {
		O3D_ERROR(E_InvalidOperation("Invalid attribute key : must be <= 128"));
    }

    if (UInt32(_key) >= m_attribLocations.size()) {
		m_attribLocations.resize(1+_key, UInt32(O3D_INFINITE));
    }

	m_attribLocations[_key] = _value;
}

UInt32 ShaderInstance::getAttribute(Int32 _key) const
{
	O3D_ASSERT((_key >= 0) && (_key < 128));

    if ((_key < 0) || (_key >= 128)) {
		O3D_ERROR(E_InvalidOperation("Invalid attribute key : must be <= 128"));
    }

    if ((UInt32(_key) >= m_attribLocations.size()) || (m_attribLocations[_key] == O3D_INFINITE)) {
		O3D_ERROR(E_InvalidOperation(String::print("Attribute key %d is not assigned", _key)));
    }

	return m_attribLocations[_key];
}

void ShaderInstance::removeUniform(Int32 _key)
{
	O3D_ASSERT((_key >= 0) && (_key < 128));

    if ((_key < 0) || (_key >= 128)) {
		O3D_ERROR(E_InvalidOperation("Invalid uniform key : must be <= 128"));
    }

    if (UInt32(_key) < m_uniformLocations.size()) {
		m_uniformLocations[_key] = O3D_INFINITE;
    }
}

void ShaderInstance::removeUniforms()
{
	m_uniformLocations.clear();
}

void ShaderInstance::removeAttribute(Int32 _key)
{
	O3D_ASSERT((_key >= 0) && (_key < 128));

    if ((_key < 0) || (_key >= 128)) {
		O3D_ERROR(E_InvalidOperation("Invalid attribute key : must be <= 128"));
    }

    if (UInt32(_key) < m_attribLocations.size()) {
		m_attribLocations[_key] = O3D_INFINITE;
    }
}

void ShaderInstance::removeAttributes()
{
	m_attribLocations.clear();
}
