/**
 * @file shader.h
 * @brief GLSL shader object.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @author  Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2005-10-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SHADER_H
#define _O3D_SHADER_H

#include "../scene/sceneentity.h"
#include "o3d/core/memorydbg.h"

#include "o3d/core/vector2.h"
#include "o3d/core/vector3.h"
#include "o3d/core/vector4.h"
#include "o3d/core/matrix3.h"
#include "o3d/core/matrix4.h"
#include "o3d/image/color.h"

#include "../object/vertexelement.h"

namespace o3d {

class Texture;
class ShaderInstance;

//---------------------------------------------------------------------------------------
//! @class Shader
//-------------------------------------------------------------------------------------
//! Use Shader when you want to use a GLSL shader program.
//! You can load many vertex, fragment and geometry program in the same object.
//---------------------------------------------------------------------------------------
class O3D_API Shader : public SceneEntity, NonCopyable<>
{
	friend class ShaderInstance;

public:

	O3D_DECLARE_DYNAMIC_CLASS(Shader)

	enum ProgramType {
		TYPE_UNDEFINED = 0,
		TYPE_VERTEX_PROGRAM,
		TYPE_FRAGMENT_PROGRAM,
		TYPE_GEOMETRY_PROGRAM };

	//! Enum used to avoid trying to compile twice the same program if previously attempt failed.
	enum ProgramState {
		PROGRAM_UNDEFINED = 0,	//!< The program was never compiled.
		PROGRAM_INVALID,		//!< the compilation or linkage failed.
		PROGRAM_COMPILED };		//!< The program was successfully compiled.

	enum ShaderState {
		SHADER_UNDEFINED =  0,				//!< Empty shader
		SHADER_LOADED =		0x00000001,		//!< Source codes loaded
		SHADER_DEFINED =	0x00000002,		//!< Programs defined and ready to be compiled
		SHADER_COMPILED =	0x00000004,		//!< Programs defined and compiled
		SHADER_LINKED =		0x00000008,		//!< Linked and ready to be sent to the GPU
		SHADER_INUSE =		0x00000010 };	//!< Currently executed by the GPU

	enum BuildType {
		BUILD_DEFINE = 0,
		BUILD_COMPILE,
		BUILD_COMPILE_AND_LINK };

	typedef std::vector<Int32> T_ProgramIndexArray;

	//! Default constructor.
	//! @param parent Parent owner. It can be NULL if necessary.
	Shader(BaseObject *parent);

	//! Virtual destructor.
	virtual ~Shader();

	//-----------------------------------------------------------------------------------
	// PROGRAM
	//-----------------------------------------------------------------------------------

	//! Load a specific shader from a file.
	//! @param programType the type of the program.
	//! @param name Define the chosen name of the program.
    //! @param is the stream containing the program.
	//! @exception E_FileInvalidFormat for various errors.
	void load(
			ProgramType programType,
			const String &programName,
            InStream &is);

	//! Load a specific shader from a file.
	//! @param programType the type of the program.
	//! @param name Define the chosen name of the program.
	//! @param filename the file name to the shader file.
	//! @exception E_FileInvalidFormat for various errors.
	void load(
			ProgramType programType,
			const String &programName,
			const String &filename);

	//! @brief Compile a specific program
	//! @param _programType the type of the program.
	//! @param _programIndex the index of the program. If _programIndex == 0, the
	//!                      current program will be compiled.
	//! @exception E_InvalidOperation if the shader object is not defined.
	//! @exception E_InvalidOperation if _programIndex == 0 and no programs is currently selected.
	//! @exception E_InvalidOperation if the compilation failed.
	//! @exception E_IndexOutOfRange if the index is not valid.
	void compileProgram(
			ProgramType _programType,
			Int32 _programIndex,
			const String & _options);
/*
	//! @brief Compile all available programs
	//! This function is useful to avoid the over-cost of the real time compilation.
	//! @param _vProgramArray a pointer to an integer array. It will contain the index of
	//!                       all vertex programs which were not compiled.
	//! @param _fProgramArray a pointer to an integer array. It will contain the index of
	//!                       all fragment programs which were not compiled.
	//! @param _gProgramArray a pointer to an integer array. It will contain the index of
	//!                       all geometry programs which were not compiled.
	//! @return FALSE if a least one program was not compiled, TRUE otherwise.
	//! @exception E_InvalidOperation if the shader object is not defined.
	Bool compileAllPrograms(
            T_ProgramIndexArray * _vProgramArray = nullptr,
            T_ProgramIndexArray * _fProgramArray = nullptr,
            T_ProgramIndexArray * _gProgramArray = nullptr);*/

	//! @brief Add a program to the shader
	//! @param _programType is the type of program you want to add : TYPE_VERTEX_PROGRAM or TYPE_FRAGMENT_PROGRAM
	//! @param _programName the name of the vertex/fragment program.
	//! @param _pBuffer a pointer to the source code
	//! @param _bufferSize the size of the source code in bytes, or 0 if it's a null terminated string.
	//! @exception E_InvalidOperation if the name is already used by an other program in the shader.
	void addProgram(
			ProgramType _programType,
			const String & _programName,
			const Char * _pBuffer,
			UInt32 _bufferSize = 0);

	//! @brief Remove a program from the shader.
	//! @param _programType is the type of program you want to remove :
	//!        TYPE_VERTEX_PROGRAM or TYPE_FRAGMENT_PROGRAM or TYPE_GEOMETRY_PROGRAM
	//! @param _programName the name of the vertex/fragment program.
	void removeProgram(ProgramType _programType, const String & _programName);

	//! @brief Destroy the shader
	//! This function restores the object to its initial state. If the shader is currently in use,
	//! it will be unbound first.
	void destroy();

	//! @brief Create a new implementation of the program
	void buildInstance(ShaderInstance &) const;

	//! get shader program name
	inline const String& getProgramName() const { return m_programName; }
	//! force the program name
	inline void setProgramName(const String& name) { m_programName = name; }

	//! Returns the number of vertex programs
	inline UInt32 getVertexProgramCount() const
	{
		return UInt32(m_vertexProgramArray.size());
	}

	//! Returns the number of fragment programs
	inline UInt32 getFragmentProgramCount() const
	{
		return UInt32(m_fragmentProgramArray.size());
	}

	//! Returns the number of geometry programs
	inline UInt32 getGeometryProgramCount() const
	{
		return UInt32(m_geometryProgramArray.size());
	}

	//! Return the name of a given vertex program
	const String& getVertexProgramName(Int32 _vertexIndex) const;
	//! Return the name of a given fragment program
	const String& getFragmentProgramName(Int32 _fragmentIndex) const;
	//! Return the name of a given geometry program
	const String& getGeometryProgramName(Int32 _geometryIndex) const;

	//! Return the source of a given vertex program
	const CString& getVertexProgramSource(Int32 _vertexIndex) const;
	//! Return the source of a given fragment program
	const CString& getFragmentProgramSource(Int32 _fragmentIndex) const;
	//! Return the source of a given geometry program
	const CString& getGeometryProgramSource(Int32 _geometryIndex) const;

	//! Return the state of a given vertex program
	ProgramState getVertexProgramState(Int32 _vertexIndex, const String & _options) const;
	//! Return the state of a given fragment program
	ProgramState getFragmentProgramState(Int32 _fragmentIndex, const String & _options) const;
	//! Return the state of a given geometry program
	ProgramState getGeometryProgramState(Int32 _geometryIndex, const String & _options) const;

	//! Find a vertex program
	//! @return the index of the program if found, else -1
	Int32 findVertexProgram(const String &) const;

	//! Find a fragment program
	//! @return the index of the program if found, else -1
	Int32 findFragmentProgram(const String &) const;

	//! Find a geometry program
	//! @return the index of the program if found, else -1
	Int32 findGeometryProgram(const String &) const;

	//! @brief Whether or not the program is loaded
	inline Bool isLoaded() const
	{
		return (!m_vertexProgramArray.empty() && !m_fragmentProgramArray.empty());
	}

private:

	//! Types
	struct T_ProgramInfo
	{
		struct T_Program
		{
			UInt32		programId;
			ProgramState	programState;

			//! Default constructor.
			T_Program();
		};

		typedef std::map<String, T_Program> T_ProgramMap;
		typedef T_ProgramMap::iterator IT_ProgramMap;
		typedef T_ProgramMap::const_iterator CIT_ProgramMap;

		String		programName;

		ProgramType	programType;
		CString		programSource;

		T_ProgramMap programs;

		//! Default constructor
		T_ProgramInfo();
/*
		//! Return TRUE if the object is in default state
		Bool isNull();

		//! Restore the default state
		void reset();*/
	};

	struct T_InstanceInfo
	{
		Int32 vertexProgramId;
		Int32 fragmentProgramId;
		Int32 geometryProgramId;

		String options;      //!< Compilation defines options.

		UInt32 shaderId;    //!< The program object id.
		Int32 shaderState;  //!< State of the shader.

		Int32 refCounter;

		ShaderInstance * pOwner;  //!< Instance which bound the program. NULL if not bound.
	};

	typedef std::vector<T_ProgramInfo>		T_ProgramArray;
	typedef T_ProgramArray::iterator		IT_ProgramArray;
	typedef T_ProgramArray::const_iterator	CIT_ProgramArray;

	typedef std::vector<T_InstanceInfo*>	T_InstanceArray;
	typedef T_InstanceArray::iterator		IT_InstanceArray;
	typedef T_InstanceArray::const_iterator	CIT_InstanceArray;

	typedef std::vector<ShaderInstance*>     T_ReferenceArray;
	typedef T_ReferenceArray::iterator		 IT_ReferenceArray;
	typedef T_ReferenceArray::const_iterator CIT_ReferenceArray;

protected:

	T_ProgramArray m_vertexProgramArray;    //!< The vertex shader id.
	T_ProgramArray m_fragmentProgramArray;  //!< The fragment shader id.
	T_ProgramArray m_geometryProgramArray;  //!< The geometry shader id.

	String m_programName;				//!< Only the program name.

	T_InstanceArray m_instances;		//!< List of shader associations

    T_ReferenceArray m_references;		//!< List of ShaderInstance using this object.

	void linkInstance(T_InstanceInfo &);
	void compileInstance(T_InstanceInfo &);

	void refreshInstanceState();
};

//---------------------------------------------------------------------------------------
//! @class ShaderInstance
//-------------------------------------------------------------------------------------
//! Define an instance of an Shader object.
//---------------------------------------------------------------------------------------
class O3D_API ShaderInstance
{
public:

	//! @brief Default constructor
	ShaderInstance();
	//! @brief Copy constructor
	ShaderInstance(const ShaderInstance &);
	//! @brief Destructor
	~ShaderInstance();

	//! @brief Affectation operator
	ShaderInstance & operator = (const ShaderInstance &);

	//! @brief Attach the instance to a shader
	//! If the instance is already attached, it will be detach first.
	//! attach(NULL) has the same effect as detach()
	//! @param _ptr a pointer to a shader language object.
	void attach(Shader* _ptr);
	void detach();

	//! @brief Return the state of the instance
	//! @return TRUE if the instance is attached, FALSE otherwise
    inline Bool isValid() const { return (m_shader != nullptr); }

	//! @brief Return the attached object
	//! @return NULL if the instance is not attached
	inline Shader * getShader() const { return m_shader; }

	//! @brief Return the name of the attached shader.
	inline const String & getShaderName() const;
	//! @brief Return the program name of the attached shader.
	//! @exception E_InvalidOperation if the instance is not loaded
	inline const String & getProgramName() const;

	//! @brief Return the current vertex program name
	//! @exception E_InvalidOperation if the vertex program is not defined
	inline const String & getVertexProgramName() const;
	//! @brief Return the current vertex program index
	//! @exception E_InvalidOperation if the vertex program is not defined
	inline Int32 getVertexProgramIndex() const;

	//! @brief Return the current fragment program index
	//! @exception E_InvalidOperation if the vertex program is not defined
	inline Int32 getFragmentProgramIndex() const;
	//! @brief Return the current fragment program name
	//! @exception E_InvalidOperation if the fragment program is not defined
	inline const String & getFragmentProgramName() const;

	//! @brief Return the current geometry program index
	//! @exception E_InvalidOperation if the vertex program is not defined
	inline Int32 getGeometryProgramIndex() const;
	//! @brief Return the current geometry program name
	//! @exception E_InvalidOperation if the fragment program is not defined
	inline const String & getGeometryProgramName() const;

	//! @brief Return the current program OpenGL identifier.
	inline Int32 getProgramId() const;

	//! @brief Return a state bit field
	//! @return SHADER_UNDEFINED if the instance is not loaded, the bit field value otherwise.
	inline Int32 getState() const;

	//! @brief Return a state value
	//! A shader instance is loaded if it's currently attached and the attached object is loaded.
	//! @return TRUE if the shader is loaded, FALSE otherwise
	inline Bool isLoaded() const;

	//! @brief Return a state value
	//! A shader is defined if the vertex and fragment program are specified.
	//! @return TRUE if the shader is defined, FALSE otherwise.
	inline Bool isDefined() const ;

	//! @brief Return a state value
	//! @return TRUE if the shader compiled, FALSE otherwise
	inline Bool isCompiled() const;

	//! @brief Return a state value
	//! @return TRUE if the shader is linked and ready to be sent to the GPU, FALSE otherwise
	inline Bool isLinked() const;

	//! @brief Return a state value
	//! Same function as ShaderInstance::isLinked()
	//! @return TRUE if the shader is linked and ready to be sent to the GPU, FALSE otherwise
	inline Bool isOperational() const { return isLinked(); }

	//! @brief Return a state value
	//! @return TRUE if the shader is currently executed by the GPU, FALSE otherwise
	inline Bool isInUse() const;

	//! @brief Change the currently bound shader if necessary and bind the program
	//! @param _vertexProgram the name of the vertex program you want to use. If the name
	//!        is empty, the currently bound program will be use.
	//! @param _fragmentProgram the name of the fragment program you want to use. If the name
	//!        is empty, the currently bound program will be use.
	//! @param _options An option string that contains some defines on how to compile programs.
	inline void bindShader(
			const String & _vertexProgram,
			const String & _fragmentProgram,
			const String & _geometryProgram,
			const String & _options = "");

	//! @brief Change the currently bound shader if necessary and bind the program
	//! @param _vertexIndex the vertex program index. -1 if you want to keep
	//!        the current vertex program.
	//! @param _fragmentIndex the fragment program index. -1 if you want to keep
	//!        the current fragment program.
	//! @param _geometryIndex the geometry program index. -1 if you want to keep
	//!        the current geometry program.
    inline void bindShader(
			Int32 _vertexIndex,
			Int32 _fragmentIndex,
			Int32 _geometryIndex,
			const String & _options);

	//! bind the shader program
	void bindShader();
	//! unbound the shader program
	void unbindShader();

	//! @brief Define the current vertex/fragment program
	//! @param _vertexIndex the vertex program index. -1 if you want to keep
	//!        the current vertex program.
	//! @param _fragmentIndex the fragment program index. -1 if you want to keep
	//!        the current fragment program.
	//! @param _type specify what operations will be apply on the newly assigned shader.
	//!              If you're using a GUI, you won't always be in a openGL context, in this case
	//!	             you'll probably use ASSIGN_ONLY to postpone the compilation and linkage at the
	//!              first call to ShadingLangage::Bind.
	//! @exception E_IndexOutOfRange, E_InvalidOperation, E_InvalidParameter
	void assign(
			Int32 _vertexIndex,
			Int32 _fragmentIndex,
			Int32 _geometryIndex,
			const String & _options,
			Shader::BuildType _type = Shader::BUILD_COMPILE_AND_LINK);

	//! @brief Define the current vertex/fragment program
	//! @param _vertexProgram the name of the vertex program you want to use. If the name
	//!        is empty, the currently used program will not changed.
	//! @param _fragmentProgram the name of the fragment program you want to use. If the name
	//!        is empty, the currently used program will not changed.
	//! @param _type specify what operations will be apply on the newly assigned shader.
	//!              If you're using a GUI, you won't always be in a openGL context, in this case
	//!	             you'll probably use ASSIGN_ONLY to postpone the compilation and linkage at the
    //!              first call to ShadingLangage::Bind.
	//! @exception E_IndexOutOfRange, E_InvalidOperation, E_InvalidParameter
	void assign(
			const String & _vertexProgram,
			const String & _fragmentProgram,
			const String & _geometryProgram,
			const String & _options,
			Shader::BuildType _type = Shader::BUILD_COMPILE_AND_LINK);

	//! @brief Compile the shader instance
	//! @param _type specify what action must be performed:
	//!	             - BUILD_DEFINE : is not a valid action and will be silently ignored
	//!              - BUILD_COMPIL : the shader will be compiled if necessary
    //!              - BUILD_COMPILE_AND_LINK : the shader will be compiled and link if necessary
	//! @exception E_InvalidOperation
	void build(Shader::BuildType _type = Shader::BUILD_COMPILE_AND_LINK);

	//-----------------------------------------------------------------------------------
	// UNIFORMS
	//-----------------------------------------------------------------------------------

	//! get the GL location of a user defined uniform
	//! This function must be called after program link and before program execution.
	inline Int32 getUniformLocation(const Char * name) const;

	//! Define an uniform int32 constant
	void setConstInt(const Char* name,const Int32 constant);
	inline void setConstInt(Int32 location,const Int32 constant);

	//! Define an uniform boolean constant
	void setConstBool(const Char* name,const Bool constant);
	inline void setConstBool(Int32 location,const Bool constant);

	//! Define an uniform float constant
	void setConstFloat(const Char* name,const Float constant);
	inline void setConstFloat(Int32 location,const Float constant);

	//! Define an uniform O3DVector2f constant
	void setConstVector2(const Char* name,const Vector2f& constant);
	inline void setConstVector2(Int32 location,const Vector2f& constant);

	//! Define an uniform O3DColor (4f) constant
	void setConstColor(const Char* name,const Color& constant);
	inline void setConstColor(Int32 location,const Color& constant);

	//! Define an uniform O3DVector3 (3f) constant
	void setConstVector3(const Char* name,const Vector3& constant);
	inline void setConstVector3(Int32 location,const Vector3& constant);

	//! Define an uniform O3DVector3 (4f) with w to 1.0 constant
	void setConstVector4(const Char* name,const Vector3& constant);
	inline void setConstVector4(Int32 location,const Vector3& constant);

	//! Define an uniform O3DVector4 (4f) constant
	void setConstVector4(const Char* name,const Vector4& constant);
	inline void setConstVector4(Int32 location,const Vector4& constant);

	//! Define an uniform O3DMatrix4 (9f) constant
	void setConstMatrix3(const Char* name,const Bool transpose,const Matrix3& constant);
	inline void setConstMatrix3(Int32 location,const Bool transpose,const Matrix3& constant);

	//! Define an uniform O3DMatrix4 (16f) constant
	void setConstMatrix4(const Char* name,const Bool transpose,const Matrix4& constant);
	inline void setConstMatrix4(Int32 location,const Bool transpose,const Matrix4& constant);

	//! Define n uniforms O3DMatrix4 (16f) constant from an array of O3DObject
	void setNConstMatrix4(const Char* name, Int32 num, const Bool transpose,const Float* constant);
	inline void setNConstMatrix4(Int32 location, Int32 num, const Bool transpose,const Float* constant);

	//! set to shader program a texture unit to a sampler
	//! @note this method call glActiveTexture(GL_TEXTURE0 + textUnit) and bind the texture onto
	void setConstTexture(const Char* name,Texture* pTexture,Int32 texUnit);
	void setConstTexture(Int32 Location,Texture* pTexture,Int32 texUnit);

	//-----------------------------------------------------------------------------------
	// VERTEX ATTRIBUTES
	//-----------------------------------------------------------------------------------

	//! get the GL location of a user defined vertex attribute
	//! This function must be called after program link and before program execution.
	Int32 getAttributeLocation(const Char * name) const;

	//-----------------------------------------------------------------------------------
	// LOCATION CONTAINER
	//-----------------------------------------------------------------------------------

	void setUniform(Int32 _key, Int32 _value);
	Int32 getUniform(Int32 _key) const;

	void setAttribute(Int32 _key, UInt32 _value);
	UInt32 getAttribute(Int32 _key) const;

	void removeUniform(Int32 _key);
	void removeUniforms();

	void removeAttribute(Int32 _key);
	void removeAttributes();

private:

	Shader *m_shader;                    //!< currently attached shading language object
	Shader::T_InstanceInfo *m_pInstance; //!< pointer to the association used by this object

	std::vector<Int32> m_uniformLocations;
	std::vector<UInt32> m_attribLocations;
};

//---------------------------------------------------------------------------------------
// inline methods definition
//---------------------------------------------------------------------------------------

const String & ShaderInstance::getShaderName() const
{
	if (!isValid())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : No object attached")));

	return m_shader->getName();
}

const String & ShaderInstance::getProgramName() const
{
	if (!isValid())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : No object attached")));

	return m_shader->getProgramName();
}

const String &  ShaderInstance::getVertexProgramName() const
{
	if (!isValid())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : No object attached")));

	if ((m_pInstance == NULL) || (m_pInstance->vertexProgramId == -1))
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Vertex program is still not defined")));

	return m_shader->m_vertexProgramArray[m_pInstance->vertexProgramId].programName;
}

const String &  ShaderInstance::getFragmentProgramName() const
{
	if (!isValid())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : No object attached")));

	if ((m_pInstance == NULL) || (m_pInstance->fragmentProgramId == -1))
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Fragment program is still not defined")));

	return m_shader->m_fragmentProgramArray[m_pInstance->fragmentProgramId].programName;
}

const String &  ShaderInstance::getGeometryProgramName() const
{
	if (!isValid())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : No object attached")));

	if ((m_pInstance == NULL) || (m_pInstance->geometryProgramId == -1))
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Geometry program is still not defined")));

	return m_shader->m_geometryProgramArray[m_pInstance->geometryProgramId].programName;
}

Int32 ShaderInstance::getProgramId() const
{
	if (!isLinked())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not get the shader id because the shader is not linked")));

	return m_pInstance->shaderId;
}

Int32 ShaderInstance::getVertexProgramIndex() const
{
	if (!isValid())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : No object attached")));

	if (!isDefined())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Vertex program is still not defined")));

	return m_pInstance->vertexProgramId;
}

Int32 ShaderInstance::getFragmentProgramIndex() const
{
	if (!isValid())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : No object attached")));

	if (!isDefined())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Fragment program is still not defined")));

	return m_pInstance->fragmentProgramId;
}

Int32 ShaderInstance::getGeometryProgramIndex() const
{
	if (!isValid())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : No object attached")));

	if (!isDefined())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Geometry program is still not defined")));

	return m_pInstance->geometryProgramId;
}

Int32 ShaderInstance::getState() const
{
	if (!isLoaded())
		return Shader::SHADER_UNDEFINED;
	else
		return m_pInstance->shaderState;
}

Bool ShaderInstance::isLoaded() const
{
	return (isValid() && m_shader->isLoaded());
}

Bool ShaderInstance::isDefined() const
{
    return ((m_pInstance != nullptr) && ((m_pInstance->shaderState & Shader::SHADER_DEFINED) == Shader::SHADER_DEFINED));
}

Bool ShaderInstance::isCompiled() const
{
    return ((m_pInstance != nullptr) && ((m_pInstance->shaderState & Shader::SHADER_COMPILED) == Shader::SHADER_COMPILED));
}

Bool ShaderInstance::isLinked() const
{
    return ((m_pInstance != nullptr) && ((m_pInstance->shaderState & Shader::SHADER_LINKED) == Shader::SHADER_LINKED));
}

Bool ShaderInstance::isInUse() const
{
    return ((m_pInstance != nullptr) && ((m_pInstance->shaderState & Shader::SHADER_INUSE) == Shader::SHADER_INUSE));
}

// Change the currently bound shader if necessary and bind the program
void ShaderInstance::bindShader(
		const String & _vertexProgram,
		const String & _fragmentProgram,
		const String & _geometryProgram,
		const String & _options)
{
	if (!isLoaded())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Object not loaded")));

	assign(_vertexProgram, _fragmentProgram, _geometryProgram, _options);

	bindShader();
}

// Change the currently bound shader if necessary and bind the program
void ShaderInstance::bindShader(
		Int32 _vertexIndex,
		Int32 _fragmentIndex,
		Int32 _geometryIndex,
		const String & _options)
{
	if (!isLoaded())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Object not loaded")));

	assign(_vertexIndex, _fragmentIndex, _geometryIndex, _options);

	bindShader();
}

//---------------------------------------------------------------------------------------
// Inline uniforms
//---------------------------------------------------------------------------------------

// get the GL location of a user defined uniform
// This function must be called after program link and before program execution.
Int32 ShaderInstance::getUniformLocation(const Char * name) const
{
	if (!isLinked())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not get a uniform location because the shader is not linked")));

	return glGetUniformLocation(m_pInstance->shaderId, name);
}

void ShaderInstance::setConstInt(Int32 location, const Int32 constant)
{
	if (!isInUse())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));

	glUniform1i(location, constant);
}

void ShaderInstance::setConstBool(Int32 location, const Bool constant)
{
	if (!isInUse())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));

	glUniform1i(location, Int32(constant));
}

void ShaderInstance::setConstFloat(Int32 location, const Float constant)
{
	if (!isInUse())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));

	glUniform1f(location, constant);
}

void ShaderInstance::setConstVector2(Int32 location, const Vector2f& constant)
{
	if (!isInUse())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));

	glUniform2fv(location, 1, constant.getData());
}

void ShaderInstance::setConstColor(Int32 location, const Color& constant)
{
	if (!isInUse())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));

	glUniform4fv(location, 1, constant.getData());
}

void ShaderInstance::setConstVector3(Int32 location, const Vector3& constant)
{
	if (!isInUse())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));

	glUniform3fv(location, 1, constant.getData());
}

void ShaderInstance::setConstVector4(Int32 location, const Vector3& constant)
{
	if (!isInUse())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));

	glUniform4f(location, constant.x(), constant.y(), constant.z(), 1.0f);
}

void ShaderInstance::setConstVector4(Int32 location, const Vector4& constant)
{
	if (!isInUse())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));

	glUniform4fv(location, 1, constant.getData());
}

void ShaderInstance::setConstMatrix3(
		Int32 location,
		const Bool transpose,
		const Matrix3& constant)
{
	if (!isInUse())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));

	glUniformMatrix3fv(location, 1, transpose, constant.getData());
}

void ShaderInstance::setConstMatrix4(
		Int32 location,
		const Bool transpose,
		const Matrix4& constant)
{
	if (!isInUse())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));

	glUniformMatrix4fv(location, 1, transpose, constant.getData());
}

void ShaderInstance::setNConstMatrix4(
		Int32 location,
		Int32 num,
		const Bool transpose,
		const Float* constant)
{
	if (!isInUse())
		O3D_ERROR(E_InvalidOperation(String("ShaderInstance : Can not define a uniform location if the shader is not bound")));

	glUniformMatrix4fv(location, num, transpose, constant);
}

} // namespace o3d

#endif // _O3D_SHADER_H

