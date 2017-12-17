/**
 * @file shadermanager.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/shader/shadermanager.h"

#include "o3d/core/filemanager.h"
#include "o3d/core/fileinfo.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/virtualfilelisting.h"
#include "o3d/core/dir.h"
#include "o3d/core/timer.h"
#include "o3d/core/smartpointer.h"
#include "o3d/core/char.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/renderer.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(ShaderManager, ENGINE_SHADER_MANAGER, SceneTemplateManager<Shader>)

ShaderManager::ShaderManager(
		BaseObject *parent,
		const String &defaultPath):
	SceneTemplateManager<Shader>(parent),
    m_activeVersion(VERSION_330)
{
	// set to the supported GLSL version
	switch (getScene()->getContext()->getGLSLVersion())	{
		case 330:
			m_activeVersion = VERSION_330;
			break;
		case 400:
			m_activeVersion = VERSION_400;
			break;
		case 410:
			m_activeVersion = VERSION_410;
			break;
		case 420:
			m_activeVersion = VERSION_420;
			break;
        case 430:
            m_activeVersion = VERSION_430;
            break;
        case 440:
            m_activeVersion = VERSION_440;
            break;
        case 450:
            m_activeVersion = VERSION_450;
            break;
        case 460:
            m_activeVersion = VERSION_460;
            break;
		default:
            // fallback to the major supported version
            switch (getScene()->getRenderer()->getVersion()) {
                case Renderer::OGL_460:
                    m_activeVersion = VERSION_460;
                    break;
                case Renderer::OGL_450:
                    m_activeVersion = VERSION_450;
                    break;
                case Renderer::OGL_440:
                    m_activeVersion = VERSION_440;
                    break;
                case Renderer::OGL_430:
                    m_activeVersion = VERSION_430;
                    break;
                case Renderer::OGL_420:
                    m_activeVersion = VERSION_420;
                    break;
                case Renderer::OGL_410:
                    m_activeVersion = VERSION_410;
                    break;
                case Renderer::OGL_400:
                    m_activeVersion = VERSION_400;
                    break;
                case Renderer::OGL_330:
                    m_activeVersion = VERSION_330;
                    break;
                default:
                    O3D_ERROR(E_InvalidPrecondition("Shaders works only with OpenGL 3.3 and greater"));
                    break;
            }
			break;
	}

	// and browse the default path
	Bool valid = False;

    if (defaultPath.isValid()) {
        if (FileManager::instance()->isPath(defaultPath)) {
			O3D_MESSAGE("Found defaults shaders in shaders directory");
            addPath(FileManager::instance()->getFullFileName(defaultPath));
			valid = True;
		}
	}

    if (!valid) {
        // search for a shaders.zip archive in working directory
        if (FileManager::instance()->isFile("shaders.zip")) {
            O3D_MESSAGE("Found defaults shaders in shaders.zip");
            FileManager::instance()->mountAsset("zip://", "shaders.zip");

            addPath(FileManager::instance()->getFullFileName("shaders"));

			valid = True;
		}
	}

    if (!valid) {
        O3D_ERROR(E_InvalidParameter("Missing shaders directory and shaders.zip file"));
	}
}

ShaderManager::~ShaderManager()
{
	destroy();
}

void ShaderManager::destroy()
{
	FastMutexLocker locker(m_mutex);

	// clear any program entry
    for (IT_ProgramMap it = m_programs.begin(); it != m_programs.end(); ++it) {
		deletePtr(it->second);
	}

	TemplateManager<Shader>::destroy();

	m_searchPathList.clear();
}

// Add a path containing programs to browse for.
UInt32 ShaderManager::addPath(const String &path)
{
	String lPath(path);
	lPath.replace('\\','/');
	lPath.trimRight('/');

	lPath = FileManager::instance()->getFullFileName(lPath);

    // @todo look for manifest read it
    {
        FastMutexLocker locker(m_mutex);

        IT_StringList it = std::find(m_searchPathList.begin(), m_searchPathList.end(), lPath);

        // already include path
        if (it != m_searchPathList.end()) {
            O3D_ERROR(E_InvalidParameter(String("The path is already registered <") + lPath + ">"));
        }

        // check if a similar path already contain 'path'
        for (it = m_searchPathList.begin(); it != m_searchPathList.end(); ++it) {
            if (it->startsWith(lPath)) {
                O3D_ERROR(E_InvalidParameter(String("The path is already partially registered <") + lPath + ">"));
            }
        }

        return browseFolder(lPath);
    }
}

// Load a program from a path.
Shader* ShaderManager::addShader(
		const String &name,
		ShaderManager::Version version)
{
	FastMutexLocker locker(m_mutex);

    switch (version) {
		case NUM_VERSIONS:
		case ANY_VERSIONS:
			O3D_ERROR(E_InvalidParameter("A valid version must be specified"));
			break;

		case ACTIVE_VERSION:
			version = m_activeVersion;
			break;

		default:
			break;
	}

	IT_ProgramMap it = m_programs.find(name);

	// found it
    if (it != m_programs.end()) {
		// of the same version
		if (it->second->versions[version].shader)
			return it->second->versions[version].shader;

		// otherwise we have to create it
		Shader *shader = new Shader(this);
		shader->setName(name);
		shader->setProgramName(name);

        if (createShader(shader, it->second->versions[version])) {
			// set it into the manager
			it->second->versions[version].shader = shader;

			// and add it to the template manager
			TemplateManager<Shader>::addElement(shader);

			return shader;
        } else {
			deletePtr(shader);
            return nullptr;
		}
    } else {
		O3D_ERROR(E_InvalidParameter("Program name is not known of the manager"));
    }

    return nullptr;
}

// Load a set of programs.
UInt32 ShaderManager::addShader(
		const T_StringList &programNameArray,
		std::vector<Shader*> *programPtrArray,
		ShaderManager::Version version)
{
	O3D_ASSERT(programPtrArray);

	FastMutexLocker locker(m_mutex);

	Shader *shader;
	UInt32 num = 0;

    for (CIT_StringList cit = programNameArray.begin(); cit != programNameArray.end(); ++cit) {
		shader = addShader(*cit, version);

        if (shader) {
			++num;

            if (programPtrArray) {
				programPtrArray->push_back(shader);
            }
		}
	}

	return num;
}

// Return a pointer to a shader.
Shader* ShaderManager::get(
		const String &name,
		ShaderManager::Version version) const
{
	FastMutexLocker locker(m_mutex);

    switch (version) {
		case NUM_VERSIONS:
		case ANY_VERSIONS:
			O3D_ERROR(E_InvalidParameter("A valid version must be specified"));
			break;

		case ACTIVE_VERSION:
			version = m_activeVersion;
			break;

		default:
			break;
	}

	CIT_ProgramMap cit = m_programs.find(name);

    if (cit != m_programs.end()) {
        if (cit->second->versions[m_activeVersion].shader) {
			return cit->second->versions[m_activeVersion].shader;
        } else {
			O3D_ERROR(E_InvalidParameter("Program name is not loaded. Process a AddShader."));
        }
    } else {
		O3D_ERROR(E_InvalidParameter("Program name does not exists"));
    }

    return nullptr;
}

// Return shader by name
UInt32 ShaderManager::find(
		const String &name,
		std::vector<Shader*> &programPtrArray) const
{
	FastMutexLocker locker(m_mutex);

	CIT_ProgramMap cit = m_programs.find(name);

	// push all valid program found for a given name
    if (cit != m_programs.end()) {
        for (UInt32 i = 0; i < NUM_VERSIONS; ++i) {
            if (cit->second->versions[i].shader) {
				programPtrArray.push_back(cit->second->versions[i].shader);
            }
		}
	}
	return programPtrArray.size();
}

// Delete a shader.
void ShaderManager::deleteShader(
		const String &name,
		ShaderManager::Version version)
{
	FastMutexLocker locker(m_mutex);

    switch (version) {
		case NUM_VERSIONS:
		case ANY_VERSIONS:
			O3D_ERROR(E_InvalidParameter("A valid version must be specified"));
			break;

		case ACTIVE_VERSION:
			version = m_activeVersion;
			break;

		default:
			break;
	}

	IT_ProgramMap it = m_programs.find(name);

    if (it != m_programs.end()) {
        for (UInt32 i = 0; i < NUM_VERSIONS; ++i) {
			Shader *shader = it->second->versions[i].shader;

			// valid for this version
            if (shader) {
				TemplateManager<Shader>::deleteElementPtr(shader);
                it->second->versions[i].shader = nullptr;
			}
		}
    } else {
		O3D_ERROR(E_InvalidParameter("Program name does not exists"));
    }
}

// Delete a shader.
void ShaderManager::deleteShader(Int32 shaderId)
{
	FastMutexLocker locker(m_mutex);

	// search the shader in the template manager, then we can
	// have the shader name to find it into the programs database
	Shader *shader = TemplateManager<Shader>::get(shaderId);
    if (shader) {
		IT_ProgramMap it = m_programs.find(shader->getName());

		// search in any versions
        for (UInt32 i = 0; i < NUM_VERSIONS; ++i) {
            if (it->second->versions[i].shader == shader) {
				TemplateManager<Shader>::deleteElementPtr(shader);
                it->second->versions[i].shader = nullptr;

				return;
			}
		}

		O3D_ERROR(E_InvalidParameter(
				"Shader appears to be in the manager but it doesn't match to one of the versions"));
    } else {
		O3D_ERROR(E_InvalidParameter("Invalid shader identifier"));
    }
}

// Delete recursively all shader of a given path.
UInt32 ShaderManager::deleteAll(const String &path)
{
	T_StringList eraseList;
	IT_ProgramMap it;

	String lPath(path);
	lPath.replace('\\','/');
	lPath.trimRight('/');

	lPath = FileManager::instance()->getFullFileName(lPath);

	UInt32 count = 0;
    {
        FastMutexLocker locker(m_mutex);

        // erase the entry of the browse path list
        IT_StringList strIt = std::find(m_searchPathList.begin(), m_searchPathList.end(), lPath);
        if (strIt != m_searchPathList.end()) {
            m_searchPathList.erase(strIt);
        } else {
            O3D_ERROR(E_InvalidParameter("The path \"" + lPath + "\" is not known of the database"));
        }

        // find any program contained into the given path
        for (it = m_programs.begin(); it != m_programs.end(); ++it) {
            if (it->second && (it->second->path == lPath)) {
                eraseList.push_back(it->first);
            }
        }

        // erase map entries
        for (IT_StringList it2 = eraseList.begin(); it2 != eraseList.end(); ++it2) {
            it = m_programs.find(*it2);
            if (it != m_programs.end()) {
                for (UInt32 i = 0; i < NUM_VERSIONS; ++i) {
                    // delete any valid program
                    if (it->second->versions[i].shader) {
                        TemplateManager<Shader>::deleteElementPtr(it->second->versions[i].shader);
                    }
                }

                // and erase the program entry
                m_programs.erase(it);
                ++count;
            }

        }
    }

	return count;
}

// Specify whether or not a shader is loaded.
Bool ShaderManager::isProgramLoaded(
		const String &name,
		ShaderManager::Version version) const
{
	FastMutexLocker locker(m_mutex);

    switch (version) {
		case NUM_VERSIONS:
			break;

		case ACTIVE_VERSION:
			version = m_activeVersion;
			break;

		default:
			break;
	}

	CIT_ProgramMap cit = m_programs.find(name);

    if (cit != m_programs.end()) {
        if (version == ANY_VERSIONS) {
			// return FALSE if one of the version is not defined
            for (UInt32 i = 0; i < NUM_VERSIONS; ++i) {
                if (cit->second->versions[i].shader == nullptr)
					return False;
			}
        } else {
            if (cit->second->versions[version].shader) {
				return True;
            }
		}
	}

	return False;
}

// Check for the existence of a given program.
Bool ShaderManager::isProgramExists(const String &name) const
{
	FastMutexLocker locker(m_mutex);

	CIT_ProgramMap cit = m_programs.find(name);
	return cit != m_programs.end();
}

// Get the absolute path for a given program.
const String& ShaderManager::getProgramPath(const String &name) const
{
	FastMutexLocker locker(m_mutex);

	CIT_ProgramMap cit = m_programs.find(name);
    if (cit == m_programs.end()) {
		O3D_ERROR(E_InvalidParameter(String("Unknown program name \"") + name + "\""));
    }

    return cit->second->path;
}

UInt32 ShaderManager::readShaderResource(const String &path)
{
    UInt32 count = 0;

    // @todo

    return count;
}

// Browse a path to search any programs that it contain and list them into the program map.
UInt32 ShaderManager::browseFolder(const String &path)
{
	FLItem *fileItem;
	UInt32 numPrograms = 0;

	String lPath = path;
	lPath.replace('\\','/');
	lPath.trimRight('/');

	m_currentBrowseFullPath = FileManager::instance()->getFullFileName(lPath);

	VirtualFileListing fileListing;
	fileListing.setPath(m_currentBrowseFullPath);

	fileListing.searchFirstFile();

    while ((fileItem = fileListing.searchNextFile()) != nullptr) {
		// a sub directory
        if (fileItem->FileType == FILE_DIR) {
            // we cannot directly found a version directory, or a sub directory
            if (WideChar::isStringDigit(fileItem->FileName)) {
                Int32 version = fileItem->FileName.toInt32();

                if (version >= 330 && version <= 450) {
                    O3D_ERROR(E_InvalidParameter("The path must contain programs directories, but the given path contain version directories"));
                }
            } else if ((fileItem->FileName != String(".")) && (fileItem->FileName != String(".."))) {
				numPrograms += browseSubFolder(fileListing.getFileFullName());
			}
		}
	}

	m_searchPathList.push_back(lPath);

	return numPrograms;
}

// Browse a sub directory.
UInt32 ShaderManager::browseSubFolder(const String &path)
{
	VirtualFileListing fileListing;
	fileListing.setPath(path);

	fileListing.searchFirstFile();

	FLItem *fileItem;
	UInt32 numPrograms = 0;

    while ((fileItem = fileListing.searchNextFile()) != nullptr) {
		// a sub directory
        if (fileItem->FileType == FILE_DIR) {
            // we can directly found a version directory, or a sub directory
            if (WideChar::isStringDigit(fileItem->FileName)) {
                Int32 version = fileItem->FileName.toInt32();

                if (version >= 330 && version <= 450) {
                    numPrograms += browseProgramFolder(&fileListing, fileItem);

                    // and return, because when we found a version directory,
                    // it should be a terminating sub folder.
                    return numPrograms;
                }
            } else if ((fileItem->FileName != String(".")) && (fileItem->FileName != String(".."))) {
				numPrograms += browseSubFolder(fileListing.getFileFullName());
			}
		}
	}

	return numPrograms;
}

//! Browse a program directory.
UInt32 ShaderManager::browseProgramFolder(VirtualFileListing *fileListing, FLItem *fileItem)
{
	UInt32 numPrograms = 0;
	T_Program *program = new T_Program;

	String programName = fileListing->getPath();
    if (programName.sub(m_currentBrowseFullPath,0) == 0) {
		programName.remove(m_currentBrowseFullPath);
    }

	programName.trimLeft('/');

	program->path = m_currentBrowseFullPath;

    while (fileItem != nullptr) {
		// a sub directory
        if ((fileItem->FileType == FILE_DIR) && WideChar::isStringDigit(fileItem->FileName)) {
			// we can directly found a version directory, or a sub directory
            Int32 version = fileItem->FileName.toInt32();

            switch (version) {
                case 330:
                    numPrograms += browseVersionFolder(fileListing->getFileFullName(), VERSION_330, program);
                    break;
                case 400:
                    numPrograms += browseVersionFolder(fileListing->getFileFullName(), VERSION_400, program);
                    break;
                case 410:
                    numPrograms += browseVersionFolder(fileListing->getFileFullName(), VERSION_410, program);
                    break;
                case 420:
                    numPrograms += browseVersionFolder(fileListing->getFileFullName(), VERSION_420, program);
                    break;
                case 430:
                    numPrograms += browseVersionFolder(fileListing->getFileFullName(), VERSION_430, program);
                    break;
                case 440:
                    numPrograms += browseVersionFolder(fileListing->getFileFullName(), VERSION_440, program);
                    break;
                case 450:
                    numPrograms += browseVersionFolder(fileListing->getFileFullName(), VERSION_450, program);
                    break;
                case 460:
                    numPrograms += browseVersionFolder(fileListing->getFileFullName(), VERSION_460, program);
                    break;
                default:
                    // unknown version
                    break;
            }
        }

		fileItem = fileListing->searchNextFile();
	}

	// program found
    if (numPrograms > 0) {
        if (m_programs.find(programName) != m_programs.end()) {
			O3D_ERROR(E_InvalidPrecondition("Program name can exists only one time \"" +
					programName + "\" at \"" + fileListing->getPath() + "\""));
        }

		m_programs.insert(std::make_pair(programName, program));

		// fill any greater versions if empty
        for (Int32 i = 0; i < NUM_VERSIONS; ++i) {
			// a version lack
			if (((program->versions[i].vpList.size() +
				 program->versions[i].fpList.size() +
                 program->versions[i].gpList.size()) == 0) && (i > 0)) {

				// duplicate it from a lesser, excepted if it is the
				// lower version
				program->versions[i].vpList = program->versions[i-1].vpList;
				program->versions[i].fpList = program->versions[i-1].fpList;
				program->versions[i].gpList = program->versions[i-1].gpList;
                program->versions[i].shader = nullptr;
			}
		}

		O3D_MESSAGE("Found shader program \"" + programName + "\"");
		return 1;
    } else {
		deletePtr(program);
		return 0;
	}
}

// Browse a version direction.
UInt32 ShaderManager::browseVersionFolder(
		const String &path,
		ShaderManager::Version version,
		ShaderManager::T_Program *program)
{
	VirtualFileListing fileListing;
	fileListing.setPath(path);
	fileListing.setExt("*.glsl");

	fileListing.searchFirstFile();

	FLItem *fileItem;

    while ((fileItem = fileListing.searchNextFile()) != nullptr) {
        if (fileItem->FileName.endsWith("_vp.glsl")) {
			program->versions[version].vpList.push_back(fileListing.getFileFullName());
        } else if (fileItem->FileName.endsWith("_fp.glsl")) {
			program->versions[version].fpList.push_back(fileListing.getFileFullName());
        } else if (fileItem->FileName.endsWith("_gp.glsl")) {
			program->versions[version].gpList.push_back(fileListing.getFileFullName());
        } else if (fileItem->FileName.endsWith("_tp.glsl")) {
            program->versions[version].tpList.push_back(fileListing.getFileFullName());
        }
	}

    program->versions[version].shader = nullptr;

	// return 1 if one or more GLSL files are found
    return (program->versions[version].vpList.size() +
		program->versions[version].fpList.size() +
        program->versions[version].gpList.size() +
        program->versions[version].tpList.size()) > 0 ? 1 : 0;
}

// Create a shader.
Bool ShaderManager::createShader(Shader *shader, T_ProgramToken &program)
{
	O3D_ASSERT(shader);

	IT_StringList it;
	ArrayChar data;
	Int32 size;
	String name;
    InStream *is;

	// vertex programs
    for (it = program.vpList.begin(); it != program.vpList.end(); ++it) {
        is = FileManager::instance()->openInStream(*it);
        size = is->getAvailable();

		data.setSize(size+1);
        is->read(data.getData(), size);
		data[size] = 0;

		FileInfo fileInfo(*it);
		name = fileInfo.getFileName();

		name.trimRight("_vp.glsl");

		shader->addProgram(Shader::TYPE_VERTEX_PROGRAM, name, data.getData(), size+1);

        deletePtr(is);
	}

	// fragment programs
    for (it = program.fpList.begin(); it != program.fpList.end(); ++it) {
        is = FileManager::instance()->openInStream(*it);
        size = is->getAvailable();

		data.setSize(size+1);
        is->read(data.getData(), size);
		data[size] = 0;

		FileInfo fileInfo(*it);
		name = fileInfo.getFileName();

		name.trimRight("_fp.glsl");

		shader->addProgram(Shader::TYPE_FRAGMENT_PROGRAM, name, data.getData(), size+1);

        deletePtr(is);
	}

	// geometry programs
    for (it = program.gpList.begin(); it != program.gpList.end(); ++it) {
        is = FileManager::instance()->openInStream(*it);
        size = is->getAvailable();

		data.setSize(size+1);
        is->read(data.getData(), size);
		data[size] = 0;

		FileInfo fileInfo(*it);
		name = fileInfo.getFileName();

		name.trimRight("_gp.glsl");

		shader->addProgram(Shader::TYPE_GEOMETRY_PROGRAM, name, data.getData(), size+1);

        deletePtr(is);
	}

    // tesselation programs
    for (it = program.tpList.begin(); it != program.tpList.end(); ++it) {
        is = FileManager::instance()->openInStream(*it);
        size = is->getAvailable();

        data.setSize(size+1);
        is->read(data.getData(), size);
        data[size] = 0;

        FileInfo fileInfo(*it);
        name = fileInfo.getFileName();

        name.trimRight("_tp.glsl");

        shader->addProgram(Shader::TYPE_TESSELATION_PROGRAM, name, data.getData(), size+1);

        deletePtr(is);
    }

	return True;
}
