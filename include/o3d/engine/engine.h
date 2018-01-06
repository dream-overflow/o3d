/**
 * @file engine.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ENGINE_H
#define _O3D_ENGINE_H

#include "o3d/core/baseobject.h"

namespace o3d {

class Context;
class Renderer;
class PrimitiveManager;
class ShaderManager;
class TextureManager;
class AnimationManager;
class MeshDataManager;
class FrameManager;
class ViewPortManager;

/**
 * @brief Engine entry point.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2016-02-17
 * Engine class maintains the resources (texture, shaders, materials, models...)
 * that can be shared with one ore many scene objects.
 */
class O3D_API Engine : public BaseObject
{
public:

    //! Constructor. Then engine have not parent, but it can be managed by
    //! the application.
    //! @param rootPath Root file path where to find the resources and packages.
    //! @param renderer A valid and initialized OpenGL renderer.
    Engine(const String &rootPath = "", Renderer *renderer = nullptr);

    //! Virtual destructor.
    virtual ~Engine();

    //! Update all managers.
    virtual void update();

    //! Get the absolute root path of the engine, where the data are looked from.
    inline const String& getRootPath() const { return m_rootPath; }

    //! Set current OpenGL renderer and context
    void setRenderer(Renderer *renderer);

    //! Get current OpenGL renderer and context
    inline       Renderer* getRenderer()       { return m_renderer; }
    //! Get current OpenGL renderer and context (const version)
    inline const Renderer* getRenderer() const { return m_renderer; }

    //
    // Managers
    //

    inline Context* getContext() { return m_context; }
    inline const Context* getContext() const { return m_context; }

    inline ViewPortManager* getViewPortManager() { return m_viewPortManager; }
    inline const ViewPortManager* getViewPortManager() const { return m_viewPortManager; }

    inline TextureManager* getTextureManager() { return m_textureManager; }
    inline const TextureManager* getTextureManager() const { return m_textureManager; }

    inline AnimationManager* getAnimationManager() { return m_animationManager; }
    inline const AnimationManager* getAnimationManager() const { return m_animationManager; }

    inline FrameManager* getFrameManager() { return m_frameManager; }
    inline const FrameManager* getFrameManager() const { return m_frameManager; }

    inline MeshDataManager* getMeshDataManager() { return m_meshDataManager; }
    inline const MeshDataManager* getMeshDataManager() const { return m_meshDataManager; }

    inline ShaderManager* getShaderManager() { return m_shaderManager; }
    inline const ShaderManager* getShaderManager() const { return m_shaderManager; }

    inline PrimitiveManager* getPrimitiveManager() { return m_primitiveManager; }
    inline const PrimitiveManager* getPrimitiveManager() const { return m_primitiveManager; }

protected:

    String m_rootPath;         //!< absolute engine root path.

    Renderer *m_renderer;      //!< linked OpenGL renderer
    Context *m_context;        //!< OpenGL context

    ViewPortManager *m_viewPortManager;     //!< View port (for displaying) manager.
    PrimitiveManager *m_primitiveManager;   //!< Primitive and helpers draw manager.
    ShaderManager *m_shaderManager;         //!< GLSL shader manager.
    TextureManager *m_textureManager;       //!< Textures resource manager.
    AnimationManager *m_animationManager;   //!< Animations resource manager.
    MeshDataManager *m_meshDataManager;     //!< Mesh data (models) resource manager.

    FrameManager *m_frameManager;   //!< Frame statistics.
};


} // namespace o3d

#endif // _O3D_ENGINE_H
