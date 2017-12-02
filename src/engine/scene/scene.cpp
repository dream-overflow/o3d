/**
 * @file scene.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/scene/scene.h"

#include "o3d/core/objects.h"
#include "o3d/core/appwindow.h"
#include "o3d/core/diskfileinfo.h"
#include "o3d/core/classfactory.h"

#include "o3d/engine/scene/sceneobjectmanager.h"

#include "o3d/engine/audiomanager.h"

#include "o3d/core/classfactory.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/architecture.h"
#include "o3d/engine/gl.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/object/primitive.h"

#include "o3d/engine/viewportmanager.h"
#include "o3d/engine/renderer.h"

#include "o3d/engine/utils/framemanager.h"

#include "o3d/engine/object/camera.h"
#include "o3d/engine/object/mesh.h"
#include "o3d/engine/object/meshdatamanager.h"
#include "o3d/engine/object/pointgizmo.h"
#include "o3d/engine/object/cubegizmo.h"
#include "o3d/engine/object/spheregizmo.h"
#include "o3d/engine/object/cylindergizmo.h"
#include "o3d/engine/object/squaregizmo.h"
#include "o3d/engine/object/vectorgizmo.h"
#include "o3d/engine/object/skin.h"
#include "o3d/engine/object/bones.h"
#include "o3d/engine/object/complexmesh.h"
#include "o3d/engine/object/light.h"

#include "o3d/engine/texture/texturemanager.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/texture/attenuationtex3d.h"
#include "o3d/engine/texture/normalcubemaptex.h"
#include "o3d/engine/alphapipeline.h"
#include "o3d/engine/shader/shadermanager.h"

#include "o3d/engine/effect/specialeffectsmanager.h"

#include "o3d/engine/hierarchy/hierarchytree.h"
#include "o3d/engine/hierarchy/node.h"
#include "o3d/engine/hierarchy/targetnode.h"

#include "o3d/engine/animation/animationmanager.h"
#include "o3d/engine/animation/animationblend.h"
#include "o3d/engine/animation/animationplayermanager.h"

#include "o3d/geom/frustum.h"

#include "o3d/engine/landscape/landscape.h"

#include "o3d/engine/guimanager.h"
#include "o3d/engine/physicmanager.h"
#include "o3d/physic/physicentitymanager.h"

#include "o3d/engine/picking.h"

#include "o3d/engine/utils/ms3d.h"

#include "o3d/engine/visibility/visibilitymanager.h"
#include "o3d/engine/object/primitivemanager.h"

using namespace o3d;

AudioManager::~AudioManager()
{
}

GuiManager::~GuiManager()
{
}

PhysicManager::~PhysicManager()
{
}

O3D_IMPLEMENT_DYNAMIC_CLASS1(Scene, ENGINE_SCENE, SceneEntity)

// Initialize
Scene::Scene(
	BaseObject *parent,
	const String &rootPath,
    Renderer *renderer) :
        SceneEntity(nullptr),  // no parent
        m_renderer(nullptr),
        m_context(nullptr),
        m_picking(nullptr),
        m_globalAmbient(0.3f,0.3f,0.3f),
        m_audio(nullptr),
        m_gui(nullptr),
        m_physicEntityManager(nullptr),
        m_sceneObjectManager(nullptr),
        m_clothManager(nullptr),
        m_primitiveManager(nullptr),
        m_shaderManager(nullptr),
        m_textureManager(nullptr),
        m_specialEffectsManager(nullptr),
        m_viewPortManager(nullptr),
        m_animationManager(nullptr),
        m_animationPlayerManager(nullptr),
        m_meshDataManager(nullptr),
        m_frameManager(nullptr),
        m_frustum(nullptr),
        m_landscape(nullptr),
        m_alphaPipeline(nullptr),
        m_hierarchyTree(nullptr),
        m_visibilityManager(nullptr),
        m_activeCamera(nullptr),
		m_keepArrays(False),
		m_lastUpdateDuration(0.f),
		m_lastDisplayDuration(0.f)
{
	m_topLevelParent = static_cast<BaseObject*>(this);
	m_name = "Scene";

	// set the renderer
    setRenderer(renderer);

	m_rootPath = FileManager::instance()->getFullFileName(rootPath);

    m_shaderManager            = new ShaderManager(this, /*m_rootPath + "/shaders"*/ "shaders");

	m_alphaPipeline	           = new AlphaPipeline(this);
	m_specialEffectsManager    = new SpecialEffectsManager(this);
	m_textureManager	       = new TextureManager(this, m_rootPath + "/textures");
	m_meshDataManager	       = new MeshDataManager(this, m_rootPath + "/models");
	m_sceneObjectManager	   = new SceneObjectManager(this);
	m_viewPortManager	       = new ViewPortManager(this);
	m_animationManager	       = new AnimationManager(this, m_rootPath + "/animations");
    m_frameManager             = new FrameManager;
	m_frustum                  = new Frustum;
	m_landscape                = new Landscape(this);
	m_animationPlayerManager   = new AnimationPlayerManager(this);
    m_physicEntityManager      = new PhysicEntityManager(this);
	m_picking                  = new Picking(this);
	m_clothManager             = new ClothManager(this);
	m_primitiveManager         = new PrimitiveManager(this);
	m_visibilityManager        = new VisibilityManager(this);

    // create the hierarchy tree
    m_hierarchyTree = new HierarchyTree(this);

    // by default draw all objects
    m_objStateDraw[0] = o3d::Limits<UInt32>::max();
    m_objStateDraw[1] = o3d::Limits<UInt32>::max();

    // but no light volume
    setDrawObject(DRAW_LIGHT_VOLUME, False);

	// reshape to 1x1 area
	reshape(1, 1);
}

// Destructor
Scene::~Scene()
{
    if (m_audio)
        m_audio->release();

    if (m_gui)
        m_gui->release();

    //m_physicEntityManager->release();

	deletePtr(m_landscape);

	deletePtr(m_gui);

    deletePtr(m_physicEntityManager);
	deletePtr(m_picking);
	deletePtr(m_hierarchyTree);
	deletePtr(m_visibilityManager);
	deletePtr(m_animationPlayerManager);
	deletePtr(m_animationManager);
	deletePtr(m_specialEffectsManager);
	deletePtr(m_sceneObjectManager);
	deletePtr(m_meshDataManager);
	deletePtr(m_shaderManager);
	deletePtr(m_viewPortManager);
	deletePtr(m_clothManager);
	deletePtr(m_primitiveManager);
	deletePtr(m_textureManager);

	deletePtr(m_audio);

	// delete misc managers
	deletePtr(m_alphaPipeline);
	deletePtr(m_frustum);
	deletePtr(m_frameManager);

	// remove all pack
	for (IT_StringList it = m_packList.begin(); it != m_packList.end(); ++it)
    {
		FileManager::instance()->removeArchive(m_rootPath + '/' + *it);
    }

	// remove the renderer
	if (m_renderer)
	{
		m_renderer->releaseIt();
        m_renderer = nullptr;
	}

    m_context = nullptr;
    m_activeCamera = nullptr;
}

void Scene::setAudio(AudioManager *audio)
{
    if (m_audio)
    {
        m_audio->release();
        deletePtr(m_audio);
    }

    m_audio = audio;

    if (m_audio)
        m_audio->init();
}

void Scene::setGui(GuiManager *gui)
{
    if (m_gui)
    {
        m_gui->release();
        deletePtr(m_gui);
    }

    m_gui = gui;

    if (m_gui)
        m_gui->init();
}

// Use default connections and attach them to an AppWindow
void Scene::defaultAttachment(AppWindow *appWindow)
{
    if (!appWindow)
        return;

	// Update
    appWindow->onUpdate.connect(this, &Scene::update);
	
	// Draw
    appWindow->onDraw.connect(this, &Scene::display);

	// Reshape
    appWindow->onMaximize.connect(this, &Scene::reshape);
    appWindow->onMinimize.connect(this, &Scene::reshape);
    appWindow->onRestore.connect(this, &Scene::reshape);
    appWindow->onResize.connect(this, &Scene::reshape);

    // synchronize reshape
	reshape(appWindow->getClientWidth(), appWindow->getClientHeight());
}

// Add pack file (ZIP file) that contain texture or others files needed by the scene
Bool Scene::addArchive(const String& archiveName)
{
	if (!FileManager::instance()->isRelativePath(archiveName))
		O3D_ERROR(E_InvalidParameter("packname must be a relative path"));

	if (FileManager::instance()->mountArchive(m_rootPath + '/' + archiveName))
	{
		m_packList.push_back(archiveName);
		return True;
	}
	else
		return False;
}

// Remove pack file (and unload it from the file manager
void Scene::removeArchive(const String& archiveName)
{
	if (!FileManager::instance()->isRelativePath(archiveName))
		O3D_ERROR(E_InvalidParameter("packname must be a relative path"));

	if (FileManager::instance()->removeArchive(m_rootPath + '/' + archiveName))
		m_packList.remove(archiveName);
}

// Set current OpenGL renderer and context.
void Scene::setRenderer(Renderer *renderer)
{
    if (!renderer || !renderer->isValid())
		O3D_ERROR(E_InvalidParameter("Renderer must be valid"));

	if (!renderer->getContext())
		O3D_ERROR(E_InvalidParameter("Renderer context must be valid"));

	if (m_renderer)
		m_renderer->releaseIt();

	m_renderer = renderer;
	m_context = renderer->getContext();

	m_renderer->useIt();
	m_renderer->setCurrent();

    // add apply default parameters from the context
    getContext()->setAllParameters();
}

// Draw an array of elements of short unsigned integer.
void Scene::drawElementsUInt16(PrimitiveFormat primitive, UInt32 count, const UInt16 *indices)
{
	m_frameManager->addPrimitives(primitive, count);
	glDrawElements(primitive, count, GL_UNSIGNED_SHORT, (const GLvoid*)indices);
}

// Draw an array of elements of unsigned integer.
void Scene::drawElementsUInt32(PrimitiveFormat primitive, UInt32 count, const UInt32 *indices)
{
	m_frameManager->addPrimitives(primitive, count);
	glDrawElements(primitive, count, GL_UNSIGNED_INT, (const GLvoid*)indices);
}

// Draw an array of elements of short unsigned integer according to a specified range of vertices.
void Scene::drawRangeElementsUInt16(
		PrimitiveFormat primitive,
		UInt32 startVertex,
		UInt32 endVertex,
		UInt32 count,
		const UInt16 *indices)
{
	m_frameManager->addPrimitives(primitive, count);
	glDrawRangeElements(primitive, startVertex, endVertex, count, GL_UNSIGNED_SHORT, (const GLvoid*)indices);
}

// Draw an array of elements of unsigned integer according to a specified range of vertices.
void Scene::drawRangeElementsUInt32(
		PrimitiveFormat primitive,
		UInt32 startVertex,
		UInt32 endVertex,
		UInt32 count,
		const UInt32 *indices)
{
	m_frameManager->addPrimitives(primitive, count);
	glDrawRangeElements(primitive, startVertex, endVertex, count, GL_UNSIGNED_INT, (const GLvoid*)indices);
}

// Draw an array of primitive.
void Scene::drawArrays(PrimitiveFormat primitive, UInt32 first, UInt32 count)
{
	m_frameManager->addPrimitives(primitive, count - first);
	glDrawArrays(primitive, 0, count);
}

// Enable or disable a DrawObjectType object draw
Bool Scene::getDrawObject(Scene::DrawObjectType type) const
{
	Int32 intType = (Int32)type;

	if (intType >= 64)
		O3D_ERROR(E_IndexOutOfRange("Invalid Object type"));

	if (intType < 32) // symbolic object
		return (m_objStateDraw[0] & (1 << intType));
	else             // physical object
		return (m_objStateDraw[1] & (1 << (intType-32)));
}

void Scene::setDrawObject(Scene::DrawObjectType type, Bool state)
{
	Int32 intType = (Int32)type;

	if (intType >= 64)
		O3D_ERROR(E_IndexOutOfRange("Invalid Object type"));

	if (intType < 32) // symbolic object
	{
		if (state)
			m_objStateDraw[0] |= (1 << intType);
		else
			m_objStateDraw[0] &= m_objStateDraw[0] ^ (1 << intType);
	}
	else              // physical object
	{
		if (state)
			m_objStateDraw[1] |= (1 << (intType-32));
		else
			m_objStateDraw[1] &= m_objStateDraw[1] ^ (1 << (intType-32));
	}
}

//---------------------------------------------------------------------------------------
// Serialization
//---------------------------------------------------------------------------------------

Bool Scene::importScene(const String &filename, SceneIO *whatImported)
{
    InStream *is = FileManager::instance()->openInStream(filename);
    if (!is)
		return False;

	Bool ret;

	try
	{
        ret = importScene(*is, whatImported);
	}
	catch(E_BaseException &)
	{
        deletePtr(is);
		throw;
	}

    deletePtr(is);

	return ret;
}

void Scene::clearImportArrays()
{
	getSceneObjectManager()->resizeImportedSceneObject(0);
	getSpecialEffectsManager()->resizeImportedSpecialEffects(0);
}

Bool Scene::importScene(InStream &is, SceneIO *whatImported)
{
    O3D_MESSAGE("Import scene...");

	String str;
	SceneIO sceneio;

    is >> str;     // file id
	if (str != "Scene")
        O3D_ERROR(E_InvalidFormat("Invalid scene token"));

	// version
	UInt32 tmpUInt32;
    is >> tmpUInt32;

	if (tmpUInt32 < O3D_VERSION_FILE_MIN)
	{
		str.print("Bad file version : %u -/-> %u", O3D_VERSION_FILE_MIN, tmpUInt32);
        O3D_ERROR(E_InvalidFormat("Unsupported scene version : " + str));
	}

    is >> m_sceneInfo;

	// sceneimport setting
    is >> sceneio;

	// backup the content of SceneIO
	if (whatImported)
		*whatImported = sceneio;

	// packs list
    is >> tmpUInt32;
	for (UInt32 i = 0; i < tmpUInt32; ++i)
	{
        is >> str;

		if (!addArchive(str))
		{
			return False;
		}
	}

	// draw object states
	if (sceneio.get(SceneIO::DRAWING_STATE))
        is >> m_objStateDraw[0]
		     >> m_objStateDraw[1];

	// total number of objects, and global ambient color
	UInt32 numObj;
    is >> numObj
       >> m_globalAmbient;

	getSceneObjectManager()->resizeImportedSceneObject(numObj);

    // sound settings TODO
//	if (sceneio.get(SceneIO::AUDIO_SETTING))
//        if (!getAudio()->readFromFile(is))
//			return False;

	// read special effects that are not into the hierarchy tree
	if (sceneio.get(SceneIO::SPECIAL_EFFECT))
        if (!getSpecialEffectsManager()->readFromFile(is))
			return False;

	// scene hierarchy
	if (sceneio.get(SceneIO::NODES))
        if (!getHierarchyTree()->readFromFile(is))
			return False;

	// process to the post importation pass for imported objects
	for (UInt32 i = 0 ; i < getSceneObjectManager()->getNumImportedSceneObjects() ; ++i)
		getSceneObjectManager()->getImportedSceneObject(i)->postImportPass();

	// read the player manager
	if (sceneio.get(SceneIO::ANIMATION_PLAYER))
        if (!getAnimationPlayerManager()->readFromFile(is))
			return False;

	// read physic manager
	if (sceneio.get(SceneIO::PHYSIC))
        if (!getPhysicEntityManager()->readFromFile(is))
			return False;

	// we don't need this now then delete it
	if (!m_keepArrays)
	{
		getSceneObjectManager()->resizeImportedSceneObject(0);
		getSpecialEffectsManager()->resizeImportedSpecialEffects(0);
	}

	return True;
}

Bool Scene::exportScene(
		const String &sceneFilename,
		const SceneIO &whatExport)
{
	SceneIO sceneio = whatExport;

	// Absolute output path
	String absSceneFilename = m_rootPath + '/' + sceneFilename;

	// start with id 0 for scene object IO
	getSceneObjectManager()->resetSingleId();

	// exported object counter
	UInt32 numObj = 0;

	// number of son contained into the root
	numObj += getHierarchyTree()->getNumElt();

	// Compute object cross references before serialize
	getHierarchyTree()->preExportPass();

	// first export all meshdata
	if (getMeshDataManager()->exportMeshData() < 0)
		return False;

	// next export all animations
	if (getAnimationManager()->exportAnimation() < 0)
		return False;

	// next export all cloth
	//if (getClothManager()->exportClothModel(path) < 0)
	//	return False;

	// and now really export the scene
    FileOutStream *os = FileManager::instance()->openOutStream(absSceneFilename, FileOutStream::CREATE);
    FileOutStream& ros = *os;

	String fileTag("Scene");

    ros << fileTag
        << UInt32(O3D_VERSION);

    ros << m_sceneInfo;

	// if we export skinning bones but not bones...
	if (sceneio.get(SceneIO::SKIN) && !sceneio.get(SceneIO::BONES))
		sceneio.set(SceneIO::BONES,True);

	// sceneimport setting
    ros << sceneio;

	// packs list
    ros << (Int32)m_packList.size();
	for (IT_StringList it = m_packList.begin() ; it != m_packList.end() ; ++it)
        ros << (*it);

	// draw object states
	if (sceneio.get(SceneIO::DRAWING_STATE))
        ros << m_objStateDraw[0]
            << m_objStateDraw[1];

    ros << numObj
        << m_globalAmbient;

    // before write the hierarchy tree we need to check if we have to export some sounds data TODO
//	if (sceneio.get(SceneIO::AUDIO_SETTING))
//        if (!getAudio()->writeToFile(ros))
//			return False;

	// write effect that are not into the hierarchy tree structure
	if (sceneio.get(SceneIO::SPECIAL_EFFECT))
        if (!getSpecialEffectsManager()->writeToFile(ros))
			return False;

	// write scene hierarchy
	if (sceneio.get(SceneIO::NODES))
        if (!getHierarchyTree()->writeToFile(ros))
			return False;

	// write animation players
	if (sceneio.get(SceneIO::ANIMATION_PLAYER))
        if (!getAnimationPlayerManager()->writeToFile(ros))
			return False;

	// write physic entities
	if (sceneio.get(SceneIO::PHYSIC))
        if (!getPhysicEntityManager()->writeToFile(ros))
			return False;

    deletePtr(os);

	return True;
}

//---------------------------------------------------------------------------------------
// scene update/draw/display
//---------------------------------------------------------------------------------------

void Scene::focus()
{
    if (m_audio)
        m_audio->focus();
}

void Scene::lostFocus()
{
    if (m_audio)
        m_audio->lostFocus();
}

void Scene::update()
{
	TimeMesure mesure(m_lastUpdateDuration);

	// compute the duration since the last update
	m_frameManager->computeFrameDuration();

	// deferred deletion (this is not the best place to delete texture, glcontext, better deletion mechanism...)
	m_textureManager->update();
	m_meshDataManager->update();
	m_animationManager->update();

	// physic update (and synchronization maybe...)
    if (m_physicEntityManager) {
        m_physicEntityManager->update();
    }

	// animation players
	m_animationPlayerManager->update();

	// scene graph update
 	m_hierarchyTree->update();

	// special effects
	m_specialEffectsManager->update();

	// landscapes
	m_landscape->update();

	// process deferred sound buffer deletion, and current sound listener put
    if (m_audio) {
        m_audio->update();
    }

	// update some gui objects
    if (m_gui) {
        m_gui->update();
    }

	//
	// Optional picking pass
	//

	// we always process the picking after an update
    if (m_picking->isPickingToProcess()) {
		// TODO Actually GUI cannot mask the scene from picking
		// this can be a problem, so an option is needed.
		// and maybe the GUI picking could be improved using color picking,
		// much easier, best precision...
		m_picking->preReDraw();

        if (m_gui) {
            m_gui->getViewPort()->disable();
        }

		m_viewPortManager->setDrawPickingMode();

		// the display is done in picking draw mode
		display();

		m_viewPortManager->setDrawMode();

        if (m_gui) {
            m_gui->getViewPort()->enable();
        }

		m_picking->postReDraw();
	}
}

/*void Scene::draw()
{
	for (UInt32 i = 0; i < 3; ++i)
	{
		Light *light = dynamicCast<Light*>(m_sceneObjectManager->searchName(lights[i]));

		// Shadow map
		else
		{
			if (light && light->getActivity())
			{
				if (light->getShadowCast())
				{
					//
					// Shadow pass.
					//

					// TODO Project the shadow map from light space with any object that the light can affect
					// And then next the complete scene with shadow receiver object only using the shadow map.

					getContext()->setDepthFunc(GL_LESS);
					getContext()->setCullingMode(CULLING_FRONT_FACE);

					Box2i oldViewPort = getContext()->getViewPort();
					Box2i shadowMapViewPort(oldViewPort.x(), oldViewPort.y(), oldViewPort.width()*2, oldViewPort.height()*2);

					UInt32 shadowMapWidth = (UInt32)shadowMapViewPort.width();
					UInt32 shadowMapHeight = (UInt32)shadowMapViewPort.height();

					getContext()->setViewPort(shadowMapViewPort);

					// shadow map is drawn in light space
					getContext()->modelView().set(light->getAbsoluteMatrix().invertStd());

					// TODO one time
					Texture2D shadowMap(this);
					shadowMap.setFiltering(Texture::NO_FILTERING);
					shadowMap.setWarp(Texture::CLAMP);
					shadowMap.create(False, shadowMapWidth, shadowMapHeight, PF_DEPTH);
					shadowMap.setPersistant(True);

					// TODO one time
					FrameBuffer shadowMapFBO(this);
					shadowMapFBO.create(shadowMapWidth, shadowMapHeight, 0);
					shadowMapFBO.bindBuffer();
					shadowMapFBO.attachTexture2D(&shadowMap, FrameBuffer::DEPTH_ATTACHMENT);
					shadowMapFBO.isCompleteness();
					shadowMapFBO.clearDepth();

					// TODO one time
					Shader *shader = getShaderManager()->addShader("shadowMap/hardShadow");
					ShaderInstance shaderInstance;
					shader->buildInstance(shaderInstance);
					shaderInstance.assign(0, 0);
					shaderInstance.build(O3DShader::BUILD_COMPILE_AND_LINK);

					shaderInstance.bindShader();

					Int32 a_vertex = shaderInstance.getAttributeLocation("a_vertex");
					Int32 u_modelViewProjectionMatrix = shaderInstance.getUniformLocation("u_modelViewProjectionMatrix");

					shaderInstance.enableVertexAttribArray(a_vertex);

					getContext()->disableColorWrite();
                    getContext()->blending().setFunc(Blending::DISABLED);
					getContext()->enableDepthTest();
					getContext()->disableDoubleSide();

					drawInfo.setFromLight(light);
					drawInfo.Pass = DrawInfo::SHADOW_PASS;
					drawInfo.ShadowTechnique = DrawInfo::SHADOW_MAP_HARD;
					drawInfo.ShaderInstance = &shaderInstance;

					// world objects
					m_visibilityManager->draw(drawInfo);

					shaderInstance.disableVertexAttribArray(a_vertex);
					shaderInstance.unBindShader();

					// Light POV matrix
					Matrix4 shadowMapMatrix(
								0.5f, 0.0f, 0.0f, 0.0f,
								0.0f, 0.5f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.5f, 0.0f,
								0.5f, 0.5f, 0.5f, 1.0f);

                    drawInfo.Light.matrixShadowMapProjection = shadowMapMatrix * getScene()->getContext()->projection().get();
                    drawInfo.Light.matrixShadowMapProjection *= light->getAbsoluteMatrix().invertStd();

					getContext()->enableColorWrite();
					getContext()->setDefaultCullingMode();

					shadowMapFBO.unBindBuffer();

					getContext()->setViewPort(oldViewPort);

					//
					// Draw the next pass with light
					//

					// reset to the active camera modelview
					getContext()->modelView().Set(getActiveCamera()->getModelviewMatrix());

					// only replace fragments
					getContext()->setDepthFunc(GL_EQUAL);

					drawInfo.setFromLight(light);
					// TODO shadow mode, vis à vis du material def à utiliser...
                    drawInfo.ShaderInstance = nullptr;

					// world objects
					m_visibilityManager->draw(drawInfo);
				}
				// no shadow light
				else
				{
					//
					// Draw the next pass with light
					//

					// reset to the active camera modelview
					getContext()->modelView().set(getActiveCamera()->getModelviewMatrix());

					// only replace fragments
					getContext()->setDepthFunc(COMP_EQUAL);

					drawInfo.setFromLight(light);

					// world objects
					m_visibilityManager->draw(drawInfo);
				}
			}
		}
	}
}*/

void Scene::display()
{
	TimeMesure mesure(m_lastDisplayDuration);

	// Set the renderer as current if necessary
    if (!m_renderer->isCurrent()) {
		m_renderer->setCurrent();
    }

	// update display statistics
	m_frameManager->update(m_lastDisplayDuration);
	
	// draw all scene viewports
	m_viewPortManager->display();
}

void Scene::reshape(UInt32 width, UInt32 height)
{
	m_viewPortManager->reshape(width, height);

    if (m_gui)
        m_gui->reshape(width, height);
}

// take a screen-shot
Bool Scene::screenShot(const String &path, Image::FileFormat format)
{
	String fileName = path + '/' + m_sceneInfo.getSceneName();

	Date date(True);
    fileName += "-" + date.buildString("%D.%M.%y-%h.%i.%s.%l");

	if (format == Image::BMP)
		fileName += ".bmp";
	else if (format == Image::JPEG)
		fileName += ".jpg";
	else if (format == Image::PNG)
		fileName += ".png";
	else
        O3D_ERROR(E_InvalidFormat("Invalid screenshot image format"));

	return m_renderer->screenShot(fileName, format);
}
