/**
 * @file scene.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SCENE_H
#define _O3D_SCENE_H

#include "o3d/core/stringlist.h"
#include "o3d/core/evt.h"
#include "o3d/core/debug.h"
#include "o3d/core/objects.h"

#include "o3d/image/color.h"
#include "o3d/image/image.h"

#include "sceneio.h"
#include "sceneinfo.h"
#include "sceneentity.h"

#include "o3d/core/memorydbg.h"

namespace o3d {

class AppWindow;
class Camera;
class Node;
class SceneObject;
class Context;
class Matrix;
class TextureManager;
class AnimationManager;
class HierarchyTree;
class Texture;
class Texture2D;
class FrameManager;
class Renderer;
class Frustum;
class AnimationPlayerManager;
class Animatable;
class PhysicEntityManager;
class Picking;
class SceneObjectManager;
class GuiManager;
class AudioRenderer;
class Primitive;
class PrimitiveManager;
class ViewPortManager;
class VisibilityManager;
class AlphaPipeline;
class SpecialEffectsManager;
class Gizmo;
class MeshDataManager;
class ComplexMeshManager;
class Landscape;
class ShaderManager;
class ClothManager;
class SpecialEffects;
class AudioManager;

/**
 * @brief The Scene class
 * The scene is a global instance that manage all 2d/3d objects, rendering, materials,
 * texture, physic, terrain, sound and such more.
 * It is possible to have many instances of scenes.
 * Native O3D scene serialization is provided by using SceneIO settings.
 * It can import O3D natives scenes, and using o3d::Ms3d it is possible to import ms3d
 * objects. To import a Collada it is possible by using o3dCollada library.
 *
 * Resources are shared by theirs specifics directories from the root path, as follow :
 *  - textures/ => contains any textures searched by the TextureManager.
 *  - models/ => contains any o3d mesh data searched by the MeshDataManager.
 *  - animations/ => contains any o3d animations searched by the AnimationManager.
 *  - sounds/ => contains any audio files searched by the SndBufferManager.
 *  - shaders/ => contains shader programs definitions.
 *
 * A scene can have a set of 4 types of files:
 *  - scenename.o3dsc that contain the scene definition and structure.
 *  - meshdataname.o3dms that contain meshes data definitions.
 *  - scenename.o3dan that contain animations data definitions.
 *  - materialname.o3dmt that contain material data definitions.
 *
 * The scene contain all structure but not the meshdatamanager neither the
 * animationmanager and neither the materials definitions. When you export a scene
 * it create at least 1 file. When you import you import only the scene file.
 * If it is a scene (.o3dsc) it will import any related files. You can also
 * preload meshes, animations and materials files with theirs respective managers.
 *
 * @date 2002-09-01
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class O3D_API Scene : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(Scene)

	//-----------------------------------------------------------------------------------
	// Scene
	//-----------------------------------------------------------------------------------

	//! enum for the symbolics objects draw state
	enum DrawObjectType
	{
        DRAW_LOCAL_AXIS = 0,
		DRAW_TRAJECTORY,
		DRAW_BONES,
		DRAW_PORTAL,
		DRAW_MESH_LOCAL_AXIS,
		DRAW_POINT_LIGHT,
		DRAW_SPOT_LIGHT,
		DRAW_DIRECTIONAL_LIGHT,
        DRAW_LIGHT_VOLUME,       //!< light volume symbolic
		DRAW_CAMERA,
		DRAW_CUBE_GIZMO,
		DRAW_CYLINDER_GYZMO,
		DRAW_POINT_GIZMO,
		DRAW_SQUARE_GIZMO,
		DRAW_SPHERE_GIZMO,
		DRAW_VECTOR_GIZMO,
		DRAW_SND_LISTENER,
		DRAW_SND_SOURCE_OMNI,
		DRAW_SND_SOURCE_DIRECT,
		DRAW_QUADTREE,
		DRAW_OCTREE,
		DRAW_CONNECTION,          //!< landscape connections
		DRAW_MATERIALS,           //!< landscape materials
		DRAW_DEBUG,               //!< used to display debugs info
		DRAW_BOUNDING_VOLUME,     //!< bounding volume
		DRAW_COLLIDER_BOUNDING,   //!< collisions bounding
		DRAW_LOCAL_SPACE,         //!< local space (TBN of each vertex)

		DRAW_UNDEFINED = 32,
		DRAW_MESH,
		DRAW_SKIN,
		DRAW_BEZIER_SURFACE,
		DRAW_NURBS_SURFACE,
		DRAW_WORLD_LABEL,
		DRAW_SKY,
		DRAW_SKY_OBJECT,
		DRAW_CLOUD_LAYER,
		DRAW_MAP_2D,
		DRAW_MAP_2D_LAYER,
		DRAW_MAP_2D_OBJECT
	};

    /**
     * @brief Scene Construct a scene.
     * @param parent Parent object, mostly null.
     * @param rootPath Path where to finds data folders.
     * @param renderer Instance to an OpenGL renderer.
     */
	Scene(
		BaseObject *parent,
		const String &rootPath = "",
        Renderer *renderer = nullptr);

	//! Virtual destructor.
	virtual ~Scene();

    //
    // Audio manager
    //

    /**
     * @brief setAudio Defines an audio manager.
     * @param audio
     */
    void setAudio(AudioManager *audio);

    /**
     * @brief getAudio
     * @return Null if not previously set.
     */
    inline AudioManager* getAudio() { return m_audio; }

    /**
     * @brief getAudio
     * @return Null if not previously set.
     */
    inline const AudioManager* getAudio() const { return m_audio; }

    //
    // Gui manager
    //

    /**
     * @brief setGui Defines an GUI manager.
     * @param audio
     */
    void setGui(GuiManager *gui);

    /**
     * @brief getGui
     * @return Null if not previously set.
     */
    inline GuiManager* getGui() { return m_gui; }

    /**
     * @brief getGui
     * @return Null if not previously set.
     */
    inline const GuiManager* getGui() const { return m_gui; }

    //
    // Physic manager
    //

    inline       PhysicEntityManager* getPhysicEntityManager()       { return m_physicEntityManager; }
    inline const PhysicEntityManager* getPhysicEntityManager() const { return m_physicEntityManager; }

    //
	// Signals connections
    //

    /**
     * @brief defaultAttachment Use default connections and attach them to an AppWindow.
     * If you use your own window system you have to connect the scene.
     * to events :
     *  - Connect resizing/minimize/restore/maximize to Scene::reshape.
     *  - Connect draw/paint to Scene::display, and a timer to Scene::update.
     *  - Connect keyboard and mouses events to the Gui::postKeyEvent
     * @param appWindow
     */
	void defaultAttachment(AppWindow *appWindow);

    //
	// Packages management
    //

	//! Add an archive file (ZIP file) that contain texture or others files needed by the
	//! scene and load it in the file manager (path relative to scene dir).
	//! @param packname A pak file with a path relative to the scene root.
	Bool addArchive(const String &archiveName);

	//! Remove an archive file (and unload it from the file manager).
	//! @param packname A pak file with a path relative to the scene root.
	void removeArchive(const String &archiveName);

	//! Get the string list of loaded archives.
	inline const T_StringList& getArchiveList() const { return m_packList; }

    //
	// Accessors
    //

	inline       Context* getContext()       { return m_context; }
	inline const Context* getContext() const { return m_context; }

	inline       AlphaPipeline* getAlphaPipeline()       { return m_alphaPipeline; }
	inline const AlphaPipeline* getAlphaPipeline() const { return m_alphaPipeline; }

	inline       SpecialEffectsManager* getSpecialEffectsManager()       { return m_specialEffectsManager; }
	inline const SpecialEffectsManager* getSpecialEffectsManager() const { return m_specialEffectsManager; }

	inline       ViewPortManager* getViewPortManager()       { return m_viewPortManager; }
	inline const ViewPortManager* getViewPortManager() const { return m_viewPortManager; }

	inline       TextureManager* getTextureManager()       { return m_textureManager; }
	inline const TextureManager* getTextureManager() const { return m_textureManager; }

	inline       AnimationManager* getAnimationManager()       { return m_animationManager; }
	inline const AnimationManager* getAnimationManager() const { return m_animationManager; }

	inline       FrameManager* getFrameManager()       { return m_frameManager; }
	inline const FrameManager* getFrameManager() const { return m_frameManager; }

	inline       MeshDataManager* getMeshDataManager()       { return m_meshDataManager; }
	inline const MeshDataManager* getMeshDataManager() const { return m_meshDataManager; }

	inline       HierarchyTree* getHierarchyTree()       { return m_hierarchyTree; }
	inline const HierarchyTree* getHierarchyTree() const { return m_hierarchyTree; }

	inline       Frustum* getFrustum()       { return m_frustum; }
	inline const Frustum* getFrustum() const { return m_frustum; }

	inline       Landscape* getLandscape()       { return m_landscape; }
	inline const Landscape* getLandscape() const { return m_landscape; }

	inline       AnimationPlayerManager* getAnimationPlayerManager()       { return m_animationPlayerManager; }
	inline const AnimationPlayerManager* getAnimationPlayerManager() const { return m_animationPlayerManager; }

	inline       ShaderManager* getShaderManager()       { return m_shaderManager; }
	inline const ShaderManager* getShaderManager() const { return m_shaderManager; }

	inline       Picking* getPicking()       { return m_picking; }
	inline const Picking* getPicking() const { return m_picking; }

	inline       SceneObjectManager* getSceneObjectManager()       { return m_sceneObjectManager; }
	inline const SceneObjectManager* getSceneObjectManager() const { return m_sceneObjectManager; }

	inline       ClothManager* getClothManager()       { return m_clothManager; }
	inline const ClothManager* getClothManager() const { return m_clothManager; }

	inline       VisibilityManager* getVisibilityManager()       { return m_visibilityManager; }
	inline const VisibilityManager* getVisibilityManager() const { return m_visibilityManager; }

	inline       PrimitiveManager* getPrimitiveManager()       { return m_primitiveManager; }
	inline const PrimitiveManager* getPrimitiveManager() const { return m_primitiveManager; }

    //
	// Global variables accessors
    //

	//! Get the scene info
	inline       SceneInfo& getSceneInfo()       { return m_sceneInfo; }
	//! Get the scene info (const version)
	inline const SceneInfo& getSceneInfo() const { return m_sceneInfo; }

	//! Define global ambient lighting
	inline void setGlobalAmbient(const Color &ambient) { m_globalAmbient = ambient; }
	//! Get global ambient lighting
	inline const Color& getGlobalAmbient() const { return m_globalAmbient; }

	//! Get the absolute root path of the scene.
	inline const String& getRootPath() const { return m_rootPath; }

	//! Define the name of the scene
	inline void setSceneName(const String &name) { m_sceneInfo.setSceneName(name); }
	//! Get the name of the scene
	inline const String& getSceneName() const { return m_sceneInfo.getSceneName(); }

	//! Set current OpenGL renderer and context
    void setRenderer(Renderer *renderer);

	//! Get current OpenGL renderer and context
    inline       Renderer* getRenderer()       { return m_renderer; }
	//! Get current OpenGL renderer and context (const version)
    inline const Renderer* getRenderer() const { return m_renderer; }

	//! Define the active camera.
	void setActiveCamera(Camera* camera) { m_activeCamera = camera; }

	//! Get the active camera. Valid at draw pass
	inline       Camera* getActiveCamera()       { return m_activeCamera; }
	//! Get the active camera. Valid at draw pass (const version)
	inline const Camera* getActiveCamera() const { return m_activeCamera; }

    //
	// Draw methods
    //

	//! Draw an array of elements of short unsigned integer.
	//! @param primitive Primitive format (P_TRIANGLES...).
	//! @param count Number of elements to be rendered.
	//! @param indices Pointer on elements array.
	void drawElementsUInt16(PrimitiveFormat primitive, UInt32 count, const UInt16 *indices);

	//! Draw an array of elements of unsigned integer.
	//! @param primitive Primitive format (P_TRIANGLES...).
	//! @param count Number of elements to be rendered.
	//! @param indices Pointer on elements array.
	void drawElementsUInt32(PrimitiveFormat primitive, UInt32 count, const UInt32 *indices);

	//! Draw an array of elements of short unsigned integer according to a specified range of vertices.
	//! @param primitive Primitive format (P_TRIANGLES...).
	//! @param startVertex First vertex index.
	//! @param endVertex Last vertex index.
	//! @param count Number of elements to be rendered.
	//! @param indices Pointer on elements array.
	void drawRangeElementsUInt16(
			PrimitiveFormat primitive,
			UInt32 startVertex,
			UInt32 endVertex,
			UInt32 count,
			const UInt16 *indices);

	//! Draw an array of elements of unsigned integer according to a specified range of vertices.
	//! @param primitive Primitive format (P_TRIANGLES...).
	//! @param startVertex First vertex index.
	//! @param endVertex Last vertex index.
	//! @param count Number of elements to be rendered.
	//! @param indices Pointer on elements array.
	void drawRangeElementsUInt32(
			PrimitiveFormat primitive,
			UInt32 startVertex,
			UInt32 endVertex,
			UInt32 count,
			const UInt32 *indices);

	//! Draw an array of primitive.
	//! @param primitive Primitive format (P_TRIANGLES...).
	//! @param first First primitive index.
	//! @param count Number of elements to be rendered.
	void drawArrays(PrimitiveFormat primitive, UInt32 first, UInt32 count);

    //
	// Enable or disable a DrawObjectType object draw
    //

	//! Is a given type of object is enable for drawing
	Bool getDrawObject(DrawObjectType type) const;
	//! Set if a given type of object must be drawn or not
	void setDrawObject(DrawObjectType type, Bool state);

	//! Draw all symbolic objects. Useful for debug visualization.
	inline void drawAllSymbolicObject() { m_objStateDraw[0] = Limits<UInt32>::max(); }
	//! Hide all symbolic objects. Normal case.
	inline void hideAllSymbolicObject() { m_objStateDraw[0] = 0; }

	//! Check for one or more symbolic object state enabled
	inline Bool isOneOrMoreSymbolicDrawObject() const
	{
		return (m_objStateDraw[0] || m_objStateDraw[1]);
	}

    //
	// Scene Import/Export
    //

	//! Force to keep importation informations arrays.
	inline void keepImportArrays(Bool set) { m_keepArrays = set; }
	//! Clear importation informations arrays.
	void clearImportArrays();

	//! Import an o3dsc scene file and return imported element.
	//! @param filename The .o3dsc scene file to import.
    //! @param whatImported If non null will contain imported manager type.
	Bool importScene(const String &filename, SceneIO *whatImported);

    //! Import an o3dsc scene file from a istream and return imported element.
    //! @param is An opened input stream
    //! @param whatImported If non null will contain imported manager type.
    Bool importScene(InStream &is, SceneIO *whatImported);

	//! Export the scene files in a specified path and with specified elements.
	//! @param sceneFilename Absolute filename of the scene, or relative to current working directory.
	//! @param whatExport Define what kind of managers to export.
	Bool exportScene(const String &sceneFilename, const SceneIO &whatExport);

	//! Get the current scene import/export data information.
	inline const SceneIO& getCurSceneIO() const { return m_curSceneIO; }

	//-----------------------------------------------------------------------------------
	// scene update/draw/display, when you use this method, the fully active scene
	// is computed, and drew. AppWindow use update and display method for you by
	// default.
	//-----------------------------------------------------------------------------------

	//! Focus. The view retrieve the foreground.
	void focus();

	//! Lost focus. The view is in the background.
	void lostFocus();

	//! Update the scene
	//! @note Default : connected to AppWindow::onUpdate signal
	void update();

	//! Call viewPortManager->display method and measure performances
	//! @note Default : connected to AppWindow::onDraw signal
	void display();

	//! Reshape the viewport manager
	//! @note Default : connected to AppWindow::onMininize/onMaximize/onRestore/onResize signals
    void reshape(UInt32 width, UInt32 height);

	//! Take a screen-shot. The file name is generated using the scene name and current date.
	//! @param path Path where to store the file.
	Bool screenShot(const String &path, Image::FileFormat format = Image::JPEG);

	//! Get the duration of the last scene update.
	//! @note Valid after an update.
	inline Float getLastUpdateDuration() const { return m_lastUpdateDuration; }
	
	//! Get the duration of the last scene display.
	//! @note Valid after a display.
	inline Float getLastDisplayDuration() const { return m_lastDisplayDuration; }	

protected:

    //
	// global objects
    //

	SceneInfo m_sceneInfo;                      //!< Global scene information
	Renderer *m_renderer;                       //!< linked OpenGL renderer
	Context *m_context;					        //!< OpenGL context
	Picking *m_picking;                         //!< picking manager
	Color m_globalAmbient;                      //!< Global ambient lighting

    //
	// Objects managers
    //

    AudioManager *m_audio;  //!< global audio manager
    GuiManager *m_gui;      //!< global gui manager

    PhysicEntityManager *m_physicEntityManager; //!< the physic manager

    SceneObjectManager *m_sceneObjectManager;   //!< scene objects manager
    ClothManager *m_clothManager;               //!< cloth objects manager

    PrimitiveManager *m_primitiveManager;       //!< Primitive and helpers draw manager.

    //
	// Miscellaneous managers
    //

    ShaderManager *m_shaderManager;             //!< shader manager
    TextureManager *m_textureManager;           //!< textures manager

	SpecialEffectsManager *m_specialEffectsManager;  //!< special effects manager

    ViewPortManager *m_viewPortManager;         //!< viewports manager

	AnimationManager *m_animationManager;             //!< animations manager
	AnimationPlayerManager *m_animationPlayerManager; //!< animation player controllers

	MeshDataManager *m_meshDataManager;    //!< mesh data manager

	FrameManager *m_frameManager;          //!< frame manager (fps, time...)

    //
	// Rendering managers
    //

	Frustum *m_frustum;                      //!< frustum calculation
	Landscape *m_landscape;                  //!< terrain rendering

    AlphaPipeline *m_alphaPipeline;		     //!< alpha blended face sorter
    HierarchyTree *m_hierarchyTree;		     //!< scene graph

	VisibilityManager *m_visibilityManager;  //!< visibility manager

    //
	// Current bound objects
    //

	Camera *m_activeCamera;         //!< active Camera (valid at draw pass)

    //
	// Serialization helpers and IO
    //

	T_StringList m_packList;       //!< opened packages list linked to the scene.
	String m_rootPath;             //!< absolute scene root path.
    Bool m_keepArrays;             //!< if true doesn't delete some importation arrays.
	SceneIO m_curSceneIO;          //!< current scene IO

    //
	// Others
    //

	UInt32 m_objStateDraw[2];      //!< Drawing objects state.

	Float m_lastUpdateDuration;    //!< Duration of the last scene update.
	Float m_lastDisplayDuration;   //!< Duration of the last scene display.
};

} // namespace o3d

#endif // _O3D_SCENE_H
