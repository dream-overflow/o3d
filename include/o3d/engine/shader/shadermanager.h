/**
 * @file shadermanager.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SHADERMANAGER_H
#define _O3D_SHADERMANAGER_H

#include "../scene/scenetemplatemanager.h"
#include "o3d/core/stringlist.h"
#include "o3d/core/objects.h"
#include "o3d/core/mutex.h"
#include "shader.h"

#include <map>

namespace o3d {

/**
 * @brief GLSL shader manager.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-10-19
 * This manager is responsible to browser directory containing program shader, and
 * to create instance (@see Shader) of these program.
 * Each created shader is kept into the manager, and returned subsequently on another
 * addShader or directly get. A shader directory per targeted version (GLSL 330,
 * 450, 460 or GLSL ES 300 310 320).
 * In a version directory you can have many vertex programs, fragment programs and
 * geometry programs.
 * Notice a program file name should always be of the form:
 *    - "name_vp.glsl", for a vertex program
 *    - "name_vp.o3dsl", for a scripted vertex program
 *    - "name_fp.glsl", for a fragment program
 *    - "name_fp.o3dsl", for a scripted fragment program
 *    - "name_gp.glsl", for a geometry program
 *    - "name_gp.o3dsl", for a scripted geometry program
 *    - "name_tc.glsl", for a tesselation control program
 *    - "name_tc.o3dsl", for a scripted tesselation control program
 *    - "name_te.glsl", for a tesselation evaluation program
 *    - "name_te.o3dsl", for a scripted tesselation evaluation program
 * The content of these files must respect the rules of the GLSL shading language.
 * Never miss to define the GLSL version using #version XXX <ES> needed for the GLSL
 * compiler.
 */
class O3D_API ShaderManager : protected SceneTemplateManager<Shader>
{
public:

	//! Managed version
	enum Version
	{
        VERSION_UNSUPPORTED = 0,
        VERSION_300_ES,       //!< GLSL ES 3.00 (OpenGL ES 3.0)
        VERSION_310_ES,       //!< GLSL ES 3.10 (OpenGL ES 3.1)
        VERSION_320_ES,       //!< GLSL ES 3.20 (OpenGL ES 3.2)
        VERSION_330,          //!< GLSL 3.30 (OpenGL 3.3)
        VERSION_450,          //!< GLSL 4.50 (OpenGL 4.5)
        VERSION_460,          //!< GLSL 4.50 (OpenGL 4.6)       
		NUM_VERSIONS,         //!< Number of versions
		ACTIVE_VERSION,       //!< Apply to the current active version
		ANY_VERSIONS          //!< Apply to any versions
	};

	O3D_DECLARE_CLASS(ShaderManager)

	//! @brief Default constructor.
	//! @param parent Parent object (the scene generally).
    //! @param defaultPath Default search path for a glsl<VERSION><ES><.zip>
	ShaderManager(BaseObject *parent, const String &defaultPath = String());

	//! Virtual Destructor
	virtual ~ShaderManager();

	//! Get the list of search path.
	inline const T_StringList& getSearchPathList() const { return m_searchPathList; }

	//! Change the default version of shader to use. Default is set to the optimal version
	//! that the hardware can manage.
	inline void setActiveVersion(Version version);

	//! Get the current active version that is used.
	inline Version getActiveVersion() const { return m_activeVersion; }

	//! @brief Destroy the manager
	//! This function remove all programs currently stored in the manager. It means the program
	//! will be still valid but no more managed by the manager.
	void destroy();

	//! Add a path containing programs to browse for.
	//! @param path A parent path containing one ore many shader.
	//! @return The number of programs found.
	//! @exception E_FileNotFoundOrInvalidRights
	UInt32 addPath(const String &path);

	//! @brief Load a program from its name.
	//! @param name A valid program name from previously browsed path.
	//! @return A new shader object, or NULL if the path was not found.
	//! @note All file contained into the given path are pre-loaded and kept in memory for a while.
    Shader* addShader(const String &name);

	//! @brief Load a set of programs from a list of names.
	//! @param programNameArray contains the name of all programs you want to load.
	//! @param programIndexArray will contain the id of each program if the pointer is valid.
	//! @param version Version to load.
	//! @return the number of programs which are created.
    //! @exception E_FileInvalidFormat and E_InvalidOperation
    UInt32 addShader(const T_StringList &programNameArray, std::vector<Shader*> *programPtrArray = nullptr);

	//! @brief Return a pointer to a shader.
	//! @param name The name of the program you want to get. An case sensitive
	//!             comparison is used.
	//! @param version Version to get (cannot be AnyVersion).
	//! @return NULL if the shader was not found and not loaded.
    //! @exception E_FileNotFoundOrInvalidRights
    Shader* get(const String &name) const;

	//! @brief Return shader by name
	//! This function returns pointers to all loaded program whose name match the specified name.
	//! Many program have commonly the same name when you create a same program for many versions.
	//! @param name The name of the program.
	//! @param programPtrArray The array in which pointers will be stored.
	//! @return The number of program found.
	UInt32 find(const String &name, std::vector<Shader*> &programPtrArray) const;

	//! @brief Delete a shader.
	//! @param name the name of the programs to delete.
	//! @param version Version to delete.
	//! @exception E_FileNotFoundOrInvalidRights
    void deleteShader(const String &name);

	//! @brief Delete a shader.
	//! @param shaderId the id to delete.
	//! @exception E_InvalidOperation.
	void deleteShader(Int32 shaderId);

	//! @brief Delete recursively all shader of a given path.
	//! @param path the parent path containing all the programs to delete.
	//! @return the number of deleted programs (not the number of deleted shader).
	//! @exception E_FileNotFoundOrInvalidRights
	UInt32 deleteAll(const String &path);

	//! @brief Check whether or not a shader is loaded.
	//! @param name the name of the program.
	//! @param version Version to check for.
	//! @return TRUE if the program is loaded, FALSE otherwise.
    Bool isProgramLoaded(const String &name) const;

	//! @brief Check for the existence of a given program.
	//! @param name the name of the program.
	//! @return TRUE if the program is known of the manager.
	Bool isProgramExists(const String &name) const;

	//! @brief Get the path for a given program. This is one path used by AddPath.
	//! @param name the name of the program.
	//! @return If exists, the path, otherwise it throw an exception.
	const String& getProgramPath(const String &name) const;

private:

	struct T_Program
	{
        String path;            //!< Path containing programs.

        T_StringList vpList;    //!< List of vertex programs (file_vp.glsl).
        T_StringList fpList;    //!< List of fragment programs (file_fp.glsl).
        T_StringList gpList;    //!< List of geometry programs (file_gp.glsl).
        T_StringList tcList;    //!< List of tesselation control programs (file_tc.glsl).
        T_StringList teList;    //!< List of tesselation evaluation programs (file_te.glsl).

        Shader *shader;         //!< Direct pointer of the Shader object. null if not created.
	};

	FastMutex m_mutex;

	typedef std::map<String, T_Program*> T_ProgramMap;
	typedef T_ProgramMap::iterator IT_ProgramMap;
	typedef T_ProgramMap::const_iterator CIT_ProgramMap;

	T_StringList m_searchPathList;  //!< Contain the list of searched path
    T_ProgramMap m_programs;        //!< Map containing all programs found at folder browsing.
    Version m_activeVersion;        //!< Active version.
	String m_currentBrowseFullPath;

    T_StringList m_assets;          //!< Mounted assets

    //! Read a path containing shader with a dedicated manifest file and specific structure.
    UInt32 readShaderResource(InStream &is);

    //! Browse a path to search any programs that it contains and list them into the program map.
	//! @return The number of found programs.
	UInt32 browseFolder(const String &path);

	//! Browse a sub directory.
	UInt32 browseSubFolder(const String &path);

    //! Browse a directory containing GLSL files.
    UInt32 browseGLSLDir(const String &path, Version version, T_Program *program);

	//! Create a shader.
    Bool createShader(Shader *shader, T_Program &program);
};

} // namespace o3d

#endif // _O3D_SHADERMANAGER_H
