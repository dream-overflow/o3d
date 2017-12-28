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
#include "o3d/core/localfile.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/virtualfilelisting.h"
#include "o3d/core/dir.h"
#include "o3d/core/timer.h"
#include "o3d/core/smartpointer.h"
#include "o3d/core/char.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/renderer.h"

#include <set>

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(ShaderManager, ENGINE_SHADER_MANAGER, SceneTemplateManager<Shader>)

ShaderManager::ShaderManager(
		BaseObject *parent,
		const String &defaultPath):
	SceneTemplateManager<Shader>(parent),
    m_activeVersion(VERSION_UNSUPPORTED)
{
    String name;

    // set to the supported GLSL version
    if (getScene()->getRenderer()->isGLES()) {
        switch (getScene()->getContext()->getGLSLVersion())	{
            case 300:
                m_activeVersion = VERSION_300_ES;
                name = "glsles300";
                break;
            case 310:
                m_activeVersion = VERSION_310_ES;
                name = "glsles310";
                break;
            case 320:
                m_activeVersion = VERSION_320_ES;
                name = "glsles320";
                break;
            default:
                break;
        }
    } else {
        switch (getScene()->getContext()->getGLSLVersion())	{
            case 330:
                m_activeVersion = VERSION_330;
                name = "glsl330";
                break;
            case 450:
                m_activeVersion = VERSION_450;
                name = "glsl450";
                break;
            case 460:
                m_activeVersion = VERSION_460;
                name = "glsl460";
                break;
            default:
                break;
        }
    }

    if (m_activeVersion == VERSION_UNSUPPORTED) {
        O3D_ERROR(E_InvalidPrecondition(String("GLSL shaders not found for the architecture ") <<
                                        getScene()->getContext()->getGLSLVersion()));
    }

	// and browse the default path
	Bool valid = False;

    Dir basePath(defaultPath);

    if (!basePath.exists()) {
        O3D_ERROR(E_InvalidPrecondition(String("GLSL shaders base path does not exists ") + defaultPath));
    }

    if (basePath.check(name) == BaseDir::SUCCESS) {
        if (FileManager::instance()->isPath(defaultPath)) {
            O3D_MESSAGE("Found defaults shaders in shaders directory");
            addPath(basePath.makeFullPathName(name));
            valid = True;
        }
    } else if (basePath.check(name + ".zip") == BaseDir::SUCCESS) {
            O3D_MESSAGE(String("Found defaults shaders in ") << basePath.makeFullFileName(name + ".zip"));
            FileManager::instance()->mountAsset("zip://", basePath.makeFullFileName(name + ".zip"));
            addPath(basePath.makeFullPathName(name));
            valid = True;

            m_assets.push_back(basePath.makeFullFileName(name + ".zip"));
    }

    if (!valid) {
        O3D_ERROR(E_InvalidPrecondition(String("GLSL shaders not found for the architecture ") <<
                                        getScene()->getContext()->getGLSLVersion()));
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
    m_programs.clear();

    // destroy shaders
	TemplateManager<Shader>::destroy();

    m_searchPathList.clear();

    // unmount assets of shaders
    for (IT_StringList it = m_assets.begin(); it != m_assets.end(); ++it) {
        FileManager::instance()->umountAsset(*it);
    }

    m_assets.clear();
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
Shader* ShaderManager::addShader(const String &name)
{
	FastMutexLocker locker(m_mutex);

	IT_ProgramMap it = m_programs.find(name);

	// found it
    if (it != m_programs.end()) {
        if (it->second->shader) {
            return it->second->shader;
        }

		// otherwise we have to create it
		Shader *shader = new Shader(this);
		shader->setName(name);
		shader->setProgramName(name);

        if (createShader(shader, *it->second)) {
			// set it into the manager
            it->second->shader = shader;

			// and add it to the template manager
			TemplateManager<Shader>::addElement(shader);

			return shader;
        } else {
			deletePtr(shader);
            return nullptr;
		}
    } else {
        O3D_ERROR(E_InvalidParameter("Program name is not known of the manager : " + name));
    }

    return nullptr;
}

UInt32 ShaderManager::addShader(const T_StringList &programNameArray, std::vector<Shader*> *programPtrArray)
{
	O3D_ASSERT(programPtrArray);

	FastMutexLocker locker(m_mutex);

	Shader *shader;
	UInt32 num = 0;

    for (CIT_StringList cit = programNameArray.begin(); cit != programNameArray.end(); ++cit) {
        shader = addShader(*cit);

        if (shader) {
			++num;

            if (programPtrArray) {
				programPtrArray->push_back(shader);
            }
		}
	}

	return num;
}

Shader* ShaderManager::get(const String &name) const
{
	FastMutexLocker locker(m_mutex);

	CIT_ProgramMap cit = m_programs.find(name);

    if (cit != m_programs.end()) {
        if (cit->second->shader) {
            return cit->second->shader;
        } else {
            O3D_ERROR(E_InvalidParameter("Program name is not loaded. Process to addShader."));
        }
    } else {
		O3D_ERROR(E_InvalidParameter("Program name does not exists"));
    }

    return nullptr;
}

// Return shader by name
UInt32 ShaderManager::find(const String &name, std::vector<Shader*> &programPtrArray) const
{
	FastMutexLocker locker(m_mutex);

	CIT_ProgramMap cit = m_programs.find(name);

	// push all valid program found for a given name
    if (cit != m_programs.end()) {
        if (cit->second->shader) {
            programPtrArray.push_back(cit->second->shader);
        }
    }

	return programPtrArray.size();
}

// Delete a shader.
void ShaderManager::deleteShader(const String &name)
{
	FastMutexLocker locker(m_mutex);

	IT_ProgramMap it = m_programs.find(name);

    if (it != m_programs.end()) {
        Shader *shader = it->second->shader;

        if (shader) {
            TemplateManager<Shader>::deleteElementPtr(shader);
            it->second->shader = nullptr;
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

        if (it->second->shader == shader) {
            TemplateManager<Shader>::deleteElementPtr(shader);
            it->second->shader = nullptr;

            return;
		}

        O3D_ERROR(E_InvalidParameter("Shader appears to be in the manager but it doesn't match"));
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
                // delete any valid program
                if (it->second->shader) {
                    TemplateManager<Shader>::deleteElementPtr(it->second->shader);
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
Bool ShaderManager::isProgramLoaded(const String &name) const
{
	FastMutexLocker locker(m_mutex);

	CIT_ProgramMap cit = m_programs.find(name);

    if (cit != m_programs.end()) {
        if (cit->second->shader) {
            return True;
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

UInt32 ShaderManager::readShaderResource(InStream &is)
{
    UInt32 numPrograms = 0;
    String line;
    String programName;
    String path;

    // magic is #SHADERS
    if (is.readLine(line)) {
        if (!line.startsWith("#SHADERS=")) {
            return 0;
        }
    } else {
        return 0;
    }

    line.remove("#SHADERS=");

    switch (m_activeVersion) {
        case VERSION_300_ES:
            if (line != "300 es") {
                return 0;
            }
            break;
        case VERSION_310_ES:
            if (line != "310 es") {
                return 0;
            }
            break;
        case VERSION_320_ES:
            if (line != "320 es") {
                return 0;
            }
            break;

        case VERSION_330:
            if (line != "330") {
                return 0;
            }
            break;
        case VERSION_450:
            if (line != "450") {
                return 0;
            }
            break;
        case VERSION_460:
            if (line != "460") {
                return 0;
            }
            break;

        default:
            return 0;
    }

    std::set<String> programList;

    while (is.readLine(line) > 0) {
        programName = line;
        programName.trimLeft('/');
        programName.trimRight('/');

        Dir programDir(m_currentBrowseFullPath + '/' + programName);
        if (!programDir.exists()) {
            O3D_WARNING(String("Not found shader program \"{0}\"").arg(programName));
            continue;
        }

        if (programList.find(programName) != programList.end()) {
            O3D_WARNING(String("Program name already exists \"{0}\" at \"{2}\"... ignored second definition")
                        .arg(programName).arg(m_currentBrowseFullPath));
        } else {
            programList.insert(programName);
        }
    }

    for (auto it = programList.begin(); it != programList.end(); ++it) {
        programName = *it;

        T_Program *program = new T_Program;
        program->path = m_currentBrowseFullPath;

        path = program->path + '/' + programName;

        if (browseGLSLDir(path, m_activeVersion, program) > 0) {
            ++numPrograms;

            O3D_MESSAGE("Found shader program \"" + programName + "\"");
            m_programs.insert(std::make_pair(programName, program));
        }
    }

    return numPrograms;
}

// Browse a path to search any programs that it contain and list them into the program map.
UInt32 ShaderManager::browseFolder(const String &path)
{
	FLItem *fileItem;
	UInt32 numPrograms = 0;

	String lPath = path;
    String programName;
	lPath.replace('\\','/');
	lPath.trimRight('/');

    m_currentBrowseFullPath = FileManager::instance()->getFullFileName(lPath);

    // a manifest is present browse with it
    File manifest(path, "manifest");
    if (manifest.exists()) {
        InStream *is = FileManager::instance()->openInStream(manifest.getFullFileName());
        numPrograms = readShaderResource(*is);
        deletePtr(is);
    } else {
        // no manifest found try to browser the folder, but need directory listing capacity
        VirtualFileListing fileListing;
        fileListing.setPath(m_currentBrowseFullPath);
        fileListing.searchFirstFile();

        while ((fileItem = fileListing.searchNextFile()) != nullptr) {
            // a sub directory
            if (fileItem->FileType == FILE_DIR) {
                if ((fileItem->FileName != String(".")) && (fileItem->FileName != String(".."))) {
                    numPrograms += browseSubFolder(fileListing.getFileFullName());
                }
            } else if ((fileItem->FileType == FILE_FILE) && fileItem->FileName.endsWith(".glsl")) {
                T_Program *program = new T_Program;
                program->path = fileListing.getFileFullName();

                programName = program->path;
                programName.remove(m_currentBrowseFullPath);
                programName.trimLeft('/');
                programName.trimRight('/');

                if (browseGLSLDir(path, m_activeVersion, program) > 0) {
                    ++numPrograms;

                    O3D_MESSAGE("Found shader program \"" + programName + "\"");
                    m_programs.insert(std::make_pair(programName, program));
                } else {
                    delete program;
                }

                O3D_MESSAGE("Found shader program \"" + programName + "\"");
            }
        }

        m_searchPathList.push_back(lPath);
    }

	return numPrograms;
}

// Browse a sub directory.
UInt32 ShaderManager::browseSubFolder(const String &path)
{
	VirtualFileListing fileListing;
	fileListing.setPath(path);

	fileListing.searchFirstFile();

    String programName;
	FLItem *fileItem;
	UInt32 numPrograms = 0;

    while ((fileItem = fileListing.searchNextFile()) != nullptr) {
		// a sub directory
        if (fileItem->FileType == FILE_DIR) {
            if ((fileItem->FileName != String(".")) && (fileItem->FileName != String(".."))) {
                numPrograms += browseSubFolder(fileListing.getFileFullName());
            }
        } else if ((fileItem->FileType == FILE_FILE) && fileItem->FileName.endsWith(".glsl")) {
            T_Program *program = new T_Program;
            program->path = fileListing.getFileFullName();

            programName = program->path;
            programName.remove(m_currentBrowseFullPath);
            programName.trimLeft('/');
            programName.trimRight('/');

            if (browseGLSLDir(path, m_activeVersion, program) > 0) {
                ++numPrograms;

                O3D_MESSAGE("Found shader program \"" + programName + "\"");
                m_programs.insert(std::make_pair(programName, program));
            } else {
                delete program;
            }

            O3D_MESSAGE("Found shader program \"" + programName + "\"");
        }
	}

	return numPrograms;
}

// Browse a version direction.
UInt32 ShaderManager::browseGLSLDir(
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
            program->vpList.push_back(fileListing.getFileFullName());
        } else if (fileItem->FileName.endsWith("_fp.glsl")) {
            program->fpList.push_back(fileListing.getFileFullName());
        } else if (fileItem->FileName.endsWith("_gp.glsl")) {
            program->gpList.push_back(fileListing.getFileFullName());
        } else if (fileItem->FileName.endsWith("_tc.glsl")) {
            program->tcList.push_back(fileListing.getFileFullName());
        } else if (fileItem->FileName.endsWith("_te.glsl")) {
            program->teList.push_back(fileListing.getFileFullName());
        }
	}

    program->shader = nullptr;

	// return 1 if one or more GLSL files are found
    return (program->vpList.size() +
            program->fpList.size() +
            program->gpList.size() +
            program->tcList.size()) > 0 ? 1 : 0;
}

// Create a shader.
Bool ShaderManager::createShader(Shader *shader, T_Program &program)
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

        name = FileManager::getFileName(*it);
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

        name = FileManager::getFileName(*it);
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

        name = FileManager::getFileName(*it);
		name.trimRight("_gp.glsl");

		shader->addProgram(Shader::TYPE_GEOMETRY_PROGRAM, name, data.getData(), size+1);

        deletePtr(is);
	}

    // tesselation control programs
    for (it = program.tcList.begin(); it != program.tcList.end(); ++it) {
        is = FileManager::instance()->openInStream(*it);
        size = is->getAvailable();

        data.setSize(size+1);
        is->read(data.getData(), size);
        data[size] = 0;

        name = FileManager::getFileName(*it);
        name.trimRight("_tc.glsl");

        shader->addProgram(Shader::TYPE_TESS_CONTROL_PROGRAM, name, data.getData(), size+1);

        deletePtr(is);
    }

    // tesselation evaluation programs
    for (it = program.teList.begin(); it != program.teList.end(); ++it) {
        is = FileManager::instance()->openInStream(*it);
        size = is->getAvailable();

        data.setSize(size+1);
        is->read(data.getData(), size);
        data[size] = 0;

        name = FileManager::getFileName(*it);
        name.trimRight("_te.glsl");

        shader->addProgram(Shader::TYPE_TESS_EVALUATION_PROGRAM, name, data.getData(), size+1);

        deletePtr(is);
    }

	return True;
}
