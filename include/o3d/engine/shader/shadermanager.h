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
 * addShader or directly get.
 * Use AddPath to add a valid search path (virtual in ZIP or real on drive). This given
 * path should contain one or many sub-directories, that define a program.
 * In others words each sub-directory correspond to a program. It is not possible to
 * directly browser a program folder, it always must have a parent one.
 * In each program directory you should have one more directory named with a version
 * number: 110 for GLSL 1.10, 120, 130, 140 or 150...
 * If 110 is defined and not 120, then 120 would use the same files as defined in the
 * previous valid version (in this case 110).
 * In a version directory you can have many vertex programs, fragment programs and
 * geometry programs.
 * Notice a program file name should always be of the form:
 *    - "name_vp.glsl", for a vertex program
 *    - "name_vp.o3dsl", for a scripted vertex program
 *    - "name_fp.glsl", for a fragment program
 *    - "name_fp.o3dsl", for a scripted fragment program
 *    - "name_gp.glsl", for a geometry program
 *    - "name_gp.o3dsl", for a scripted geometry program
 *    - "name_tp.glsl", for a tesselation program
 *    - "name_tp.o3dsl", for a scripted tesselation program
 * otherwise it is impossible to list them correctly.
 * The content of these files must respect the rules of the GLSL shading language.
 * Never miss to define the GLSL version using #version 110 or greater, because this
 * force the compiler to use a specific version of the language/compiler.
 */
class O3D_API ShaderManager : protected SceneTemplateManager<Shader>
{
public:

	//! Managed version
	enum Version
	{
        VERSION_330,          //!< GLSL 3.30 (OpenGL 3.3)
        VERSION_400,          //!< GLSL 4.00 (OpenGL 4.0)
        VERSION_410,          //!< GLSL 4.10 (OpenGL 4.1)
        VERSION_420,          //!< GLSL 4.20 (OpenGL 4.2)
        VERSION_430,          //!< GLSL 4.30 (OpenGL 4.3)
        VERSION_440,          //!< GLSL 4.40 (OpenGL 4.4)
        VERSION_450,          //!< GLSL 4.50 (OpenGL 4.5)
        VERSION_460,          //!< GLSL 4.50 (OpenGL 4.6)
		NUM_VERSIONS,         //!< Number of versions
		ACTIVE_VERSION,       //!< Apply to the current active version
		ANY_VERSIONS          //!< Apply to any versions
	};

	O3D_DECLARE_CLASS(ShaderManager)

	//! @brief Default constructor.
	//! @param parent Parent object (the scene generally).
	//! @param defaultPath Default search path.
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
	//! @param version Version to load. Cannot be AnyVersion (see the AddShader below).
	//! @return A new shader object, or NULL if the path was not found.
	//! @note All file contained into the given path are pre-loaded and kept in memory for a while.
	Shader* addShader(const String &name, Version version = ACTIVE_VERSION);

	//! @brief Load a set of programs from a list of names.
	//! @param programNameArray contains the name of all programs you want to load.
	//! @param programIndexArray will contain the id of each program if the pointer is valid.
	//! @param version Version to load.
	//! @return the number of programs which are created.
    //! @exception E_FileInvalidFormat and E_InvalidOperation
	UInt32 addShader(
			const T_StringList &programNameArray,
            std::vector<Shader*> *programPtrArray = nullptr,
			Version version = ACTIVE_VERSION);

	//! @brief Return a pointer to a shader.
	//! @param name The name of the program you want to get. An case sensitive
	//!             comparison is used.
	//! @param version Version to get (cannot be AnyVersion).
	//! @return NULL if the shader was not found and not loaded.
    //! @exception E_FileNotFoundOrInvalidRights
	Shader* get(const String &name, Version version = ACTIVE_VERSION) const;

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
	void deleteShader(const String &name, Version version = ANY_VERSIONS);

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
	Bool isProgramLoaded(const String &name, Version version = ACTIVE_VERSION) const;

	//! @brief Check for the existence of a given program.
	//! @param name the name of the program.
	//! @return TRUE if the program is known of the manager.
	Bool isProgramExists(const String &name) const;

	//! @brief Get the path for a given program. This is one path used by AddPath.
	//! @param name the name of the program.
	//! @return If exists, the path, otherwise it throw an exception.
	const String& getProgramPath(const String &name) const;

private:

	struct T_ProgramToken
	{
		T_StringList vpList;    //!< List of vertex programs (file_vp.glsl).
		T_StringList fpList;    //!< List of fragment programs (file_fp.glsl).
		T_StringList gpList;    //!< List of geometry programs (file_gp.glsl).
        T_StringList tpList;    //!< List of tesselation programs (file_tp.glsl).

		Shader *shader;  //!< Direct pointer of the Shader object. null if not created.
	};

	struct T_Program
	{
		String path;                           //!< Path containing programs.
		T_ProgramToken versions[NUM_VERSIONS]; //!< Programs arranged by version
	};

	FastMutex m_mutex;

	typedef std::map<String, T_Program*> T_ProgramMap;
	typedef T_ProgramMap::iterator IT_ProgramMap;
	typedef T_ProgramMap::const_iterator CIT_ProgramMap;

	T_StringList m_searchPathList;  //!< Contain the list of searched path

	T_ProgramMap m_programs;    //!< Map containing all programs found at folder browsing.

	Version m_activeVersion;   //!< Active version.

	String m_currentBrowseFullPath;

	//! Browse a path to search any programs that it contain and list them into the program map.
	//! @return The number of found programs.
	UInt32 browseFolder(const String &path);

	//! Browse a sub directory.
	UInt32 browseSubFolder(const String &path);

	//! Browse a program directory.
	UInt32 browseProgramFolder(class VirtualFileListing *fileListing, struct FLItem *fileItem);

	//! Browse a version direction.
	UInt32 browseVersionFolder(const String &path, Version version, T_Program *program);

	//! Create a shader.
	Bool createShader(Shader *shader, T_ProgramToken &program);
};

} // namespace o3d

#endif // _O3D_SHADERMANAGER_H
