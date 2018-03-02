/**
 * @file enginetype.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ENGINETYPE_H
#define _O3D_ENGINETYPE_H

namespace o3d {

/**
 * @brief Enumerations for engine scene objects.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2015-12-28
 */
enum EngineObjectType
{
    ENGINE_SCENE = 0x02000000,         //!< scene manager
    ENGINE_ENGINE_ENTITY,
    ENGINE_ENGINE_RESOURCE,
    ENGINE_ENGINE_RESOURCE_MANAGER,
	ENGINE_SCENE_OBJECT,
	ENGINE_ALPHA_PIPELINE,
	ENGINE_PICKING,
	ENGINE_PICKABLE,
	ENGINE_MOVABLE,
	ENGINE_ANIMATABLE,
	ENGINE_DRAWABLE,
	ENGINE_UPDATABLE,
	ENGINE_SHADABLE,
	ENGINE_SHADOWABLE,
	ENGINE_SHADOW_RENDERER,
	ENGINE_SHADOW_VOLUME_FORWARD,
	ENGINE_GBUFFER,
    ENGINE_GBUFFER_MSAA,
	ENGINE_SCENE_DRAWER,
    ENGINE_DEFERRED_DRAWER,
	ENGINE_SCENE_ENTITY,
	ENGINE_SCENE_RESOURCE,
	ENGINE_SCENE_RESOURCE_MANAGER,
	ENGINE_ISO_SPHERE,
	ENGINE_WORLD_LABEL,
	ENGINE_GEOMETRY_DATA,
	ENGINE_PRIMITIVE_MANAGER,
	ENGINE_PRIMITIVE,
	ENGINE_SURFACE,
	ENGINE_SPHERE,
	ENGINE_CYLINDER,
	ENGINE_CUBE,
	ENGINE_DOME,
	ENGINE_SHADABLE_OBJECT,
    ENGINE_TEXT_2D,

	ENGINE_GEOMETRY,                   //!< geometry of a mesh data
	ENGINE_MESH_DATA_MANAGER,          //!< mesh data manager chunk
	ENGINE_MESH_DATA,                  //!< mesh data chunk
	ENGINE_CLOTH_LIST,                 //!< Cloth model manager
	ENGINE_CLOTH_MODEL,                //!< cloth model object
	ENGINE_SHADER_MANAGER,             //!< shader manager
	ENGINE_SHADER,                     //!< a shader object
	ENGINE_ANIMATIONPLAYER_LIST,       //!< animation player manager
	ENGINE_ANIMATION_PLAYER,           //!< animation player
	ENGINE_ANIMATION_PLAYER_QUEUE,     //!< queue of players
	ENGINE_SCENEOBJECT_LIST,           //!< scene object mapper
	ENGINE_GL_CONTEXT,                 //!< opengl context

    ENGINE_ANIMATION_LIST = 0x02010000,//!< anmiation manager
	ENGINE_ANIMATION,                  //!< animation object
	ENGINE_ANIMATION_BLEND,            //!< animation blending object

    ENGINE_LANDSCAPE = 0x02020000,     //!< landscape generator
	ENGINE_ZONE,                       //!< a zone of the landscape
	ENGINE_ZONEDATA,                   //!< a zone date of the landscape
	ENGINE_SKY_OBJECT,
	ENGINE_SKY_OBJECT_BASE,
	ENGINE_SCATTERING_MODEL_BASE,
	ENGINE_SCATTERING_MODEL_DEFAULT,
	ENGINE_SKY_SCATTERING,
	ENGINE_SKY_BASE,

    ENGINE_VIEWPORT_LIST = 0x02030000,   //!< viewport manager
	ENGINE_VIEWPORT,                     //!< base viewport object
	ENGINE_VIEWPORT_SCREEN,              //!< screen render viewport
	ENGINE_VIEWPORT_FEEDBACK,            //!< texture render viewport

    ENGINE_TEXTURE_LIST = 0x02040000,    //!< texture manager
	ENGINE_TEXTURE,                      //!< undefined texture
	ENGINE_TEXTURE_1D,                   //!< texture 1D
	ENGINE_TEXTURE_2D,                   //!< texture 2D
	ENGINE_TEXTURE_3D,                   //!< texture 3D
	ENGINE_TEXTURE_CUBEMAP,              //!< cube map texture
	ENGINE_TEXTURE_CUBEMAP_ARRAY,        //!< cube map array texture
	ENGINE_TEXTURE_NORMCUBEMAP,          //!< normal cubemap texture
	ENGINE_TEXTURE_ATTENUATION_3D,       //!< 3d attenuation texture
	ENGINE_TEXTURE_2D_MULTISAMPLE,       //!< texture 2D multisample
	ENGINE_TEXTURE_1D_ARRAY,             //!< texture 1D array
	ENGINE_TEXTURE_2D_ARRAY,             //!< texture 2D array
	ENGINE_TEXTURE_2D_MULTISAMPLE_ARRAY, //!< texture 2D multisample array
	ENGINE_TEXTURE_2D_CACHE,

    ENGINE_MATERIAL_LIST = 0x02050000,   //!< Material manager.
	ENGINE_MATERIAL_PROFILE,             //!< Material profile.
	ENGINE_MATERIAL_TECHNIQUE,
	ENGINE_MATERIAL_PASS,
	ENGINE_MATERIAL,                     //!< Base material object.
	ENGINE_AMBIENT_MATERIAL,
    ENGINE_COLOR_MATERIAL,
	ENGINE_BUMP_MATERIAL,
	ENGINE_DEPTH_PATH_MATERIAL,
	ENGINE_LAMBERT_MATERIAL,
	ENGINE_PICKING_MATERIAL,
	ENGINE_MATERIAL_TEMPLATE,            //!< Programmable template material.
	ENGINE_MATERIAL_COLOR,               //!< Color material.
	ENGINE_MATERIAL_BILLBOARD,           //!< Billboarding.
	ENGINE_MATERIAL_TEXTURE_MAP,         //!< Texture mapping.
	ENGINE_MATERIAL_BUMPMAP_PARALLAX,    //!< Parallax bump mapping.
	ENGINE_MATERIAL_NORMAL_MAPPING,      //!< Simple normal mapping.
	ENGINE_MATERIAL_CUBE_MAP,            //!< Cube mapping.
	ENGINE_MATERIAL_SPHERE_MAP,          //!< Sphere mapping.
	ENGINE_MATERIAL_CARTOON,             //!< Cell shading.
    ENGINE_MATERIAL_UNKNOWN = 0x0205FFFF,//!< Unknown material.

    ENGINE_TRANSFORM = 0x02060000,       //!< base transform object
    ENGINE_ATRANSFORM,                   //!< accumulative SRT transform
    ENGINE_MTRANSFORM,                   //!< accumulative SRT transform
    ENGINE_FTRANSFORM,                   //!< first person view like transform
    ENGINE_DQTRANSFORM,                  //!< dual quaternion based RT transform
    ENGINE_STRANSFORM,                   //!< spacial view transform

    ENGINE_HIERARCHY_TREE = 0x02070000,  //!< hierarchy-tree
	ENGINE_ROOT_NODE,
    ENGINE_BASE_NODE,                    //!< abstract base node object
    ENGINE_NODE,                         //!< transform node object for 3d
    ENGINE_NODE_TARGET,                  //!< target node object
    ENGINE_BONES,                        //!< bones node object
    ENGINE_NODE_END = 0x0207FFFF,        //!< last node identifier

    ENGINE_MESH = 0x02080000,          //!< mesh
	ENGINE_SPRITE2D,                   //!< 2d sprite
	ENGINE_SPRITE2D_INSTANCED,         //!< Multiple sprite in a single object.

    ENGINE_SKIN = 0x02090000,          //!< skin mesh base object
	ENGINE_RIGGING,                    //!< rigging mesh object
	ENGINE_SKINNING,                   //!< skinning mesh object
	ENGINE_HUMANOID,                   //!< humanoid mesh object (complex skinning)
    ENGINE_CLOTH_OBJECT,               //!< cloth object
    ENGINE_SKELETON,                   //!< skeleton container

    ENGINE_COMPLEX_MESH = 0x020A0000,  //!< complex mesh base object
	ENGINE_BEZIERSURFACE_MESH,         //!< bezier surface
	ENGINE_NURBS_RENDERER,             //!< nurbs surface renderer

    ENGINE_LIGHT = 0x020B0000,         //!< Light object.

    ENGINE_CAMERA = 0x020C0000,        //!< camera object

    ENGINE_GIZMO = 0x020D0000,         //!< base gizmo object
	ENGINE_GIZMO_CUBE,                 //!< a box gizmo
	ENGINE_GIZMO_CYLINDER,             //!< a cylinder gizmo
	ENGINE_GIZMO_POINT,                //!< a point gizmo
	ENGINE_GIZMO_SQUARE,               //!< a square gizmo
	ENGINE_GIZMO_SPHERE,               //!< a sphere gizmo
	ENGINE_GIZMO_VECTOR,               //!< a vector gizmo

    ENGINE_EFFET_LIST = 0x020E0000,    //!< special effect manager
	ENGINE_EFFECT,                     //!< base special effect object
	ENGINE_EFFECT_FOG,                 //!< fog effect
	ENGINE_EFFECT_VOLUMETRIC_FOG,      //!< volumetric fog effect (not yet implemented)
	ENGINE_EFFECT_LENS_FLARE,          //!< lens effect
	ENGINE_EFFECT_GLOW,                //!< glow effect
	ENGINE_EFFECT_LENS,                //!< lens effect
	ENGINE_EFFECT_SKYBOX,              //!< skybox
	ENGINE_EFFECT_INTENSITY,           //!< intensity effect
	ENGINE_EFFECT_MULTI,               //!< multi effect
	ENGINE_EFFECT_LENS_FLARE_INFINITE, //!< lens flare infinite
	ENGINE_EFFECT_LENS_FLARE_LOCAL,    //!< lens flare on target
	ENGINE_LENSFLARE_MODEL,            //!< lens flare model for lens effect

    ENGINE_PARTICULE_MANAGER = 0x020F0000, //!< Particles manager
	ENGINE_PARTICULE,                      //!< a particle

    ENGINE_VISIBILITY_ABC = 0x02100000,    //!< abstract visibility controller
	ENGINE_VISIBILITY_MANAGER,
	ENGINE_VISIBILITY_BASIC,           //!< basic visibility controller
	ENGINE_VISIBILITY_QUADTREE,        //!< quadtree visibility controller
	ENGINE_VISIBILITY_OCTREE,          //!< octree visibility controller

    ENGINE_TERRAIN_ABC = 0x02110000,    //!< abstract terrain renderer and visibility controller
	ENGINE_TERRAIN_MANAGER,
	ENGINE_TERRAIN_DEF_MANAGER,
	ENGINE_HEIGHT_MAP_SPLATTING,
	ENGINE_TERRAIN_BASE,
	ENGINE_PCLODTERRAIN,               //!< terrain renderer and visibility controller
	ENGINE_PCLOD_CONFIG,
	ENGINE_PCLOD_TEXTURE_MANAGER,
	ENGINE_PCLOD_RENDER_MANAGER,
	ENGINE_PCLOD_TERRAIN,
	ENGINE_PCLOD_ZONE_MANAGER,

    ENGINE_MAP_2D = 0x02120000,
	ENGINE_MAP_2D_LAYER,
	ENGINE_MAP_2D_TILE_LAYER,
	ENGINE_MAP_2D_OBJECT_LAYER,
	ENGINE_MAP_2D_OBJECT,
	ENGINE_MAP_2D_TILER,
	ENGINE_MAP_2D_ISO_TILER,
	ENGINE_MAP_2D_DRAWER,
    ENGIME_MAP_2D_TILE_SET
};

//! Culling mode
enum CullingMode
{
	CULLING_FRONT_FACE = 0,  //!< cull front faces.
	CULLING_BACK_FACE,       //!< cull back faces.
	CULLING_NONE             //!< no culling.
};

//! OpenGL comparison modes.
enum Comparison
{
	COMP_NEVER = 0x0200,
	COMP_LESS = 0x0201,
	COMP_EQUAL = 0x0202,
	COMP_LEQUAL = 0x0203,
	COMP_GREATER = 0x0204,
	COMP_NOTEQUAL = 0x0205,
	COMP_GEQUAL = 0x0206,
	COMP_ALWAYS = 0x0207
};

//! Vertex attributes arrays.
enum VertexAttributeArray
{
    V_VERTICES_ARRAY = 0,         //!< Vertices array (3 float).
    V_NORMALS_ARRAY = 1,          //!< Normals array (3 float).
    V_TANGENT_ARRAY = 2,          //!< Tangent array (3 float).
    V_BITANGENT_ARRAY = 3,        //!< Bi-tangent array (3 float).
    V_COLOR_ARRAY = 4,            //!< Vertex color array (4 float).
    V_UV_MAP_ARRAY = 5,           //!< 2d texture coordinates (uv) (2 float).
    V_UVW_ARRAY = 6,              //!< 3d texture coordinates (uvw) (3 float).
    V_RIGGING_ARRAY = 7,          //!< Rigging bones reference id (1 float).
    V_SKINNING_ARRAY = 8,         //!< Array that contain bones reference id 0..3 (4 float).
    V_SKINNING_EXT_ARRAY = 9,     //!< Array that contain bones reference id 4..7 (4 float).
    V_WEIGHTING_ARRAY = 10,       //!< Array that contain skinning bones weights 0..3 (4 float).
    V_WEIGHTING_EXT_ARRAY = 11,   //!< Array that contain skinning bones weights 4..7 (4 float).
    V_UV_MAP2_ARRAY = 12,         //!< Second 2d texture coordinate unit (2 float).
    V_UVW_2_ARRAY = 13,           //!< Second 3d texture coordinate unit (3 float).
    V_UV_MAP3_ARRAY = 14,         //!< Third 2d texture coordinate unit (2 float).
    V_UVW_3_ARRAY = 15,           //!< Third 3d texture coordinate unit (3 float).
};

//! Number of vertex attributes arrays.
static const Int32 NUM_VERTEX_ATTRIBUTES = 16;

//! OpenGL Data type.
enum DataType
{
	DATA_BYTE              = 0x1400,
	DATA_UNSIGNED_BYTE     = 0x1401,
	DATA_SHORT             = 0x1402,
	DATA_UNSIGNED_SHORT    = 0x1403,
	DATA_INT               = 0x1404,
	DATA_UNSIGNED_INT      = 0x1405,
	DATA_FLOAT             = 0x1406,
	DATA_DOUBLE            = 0x140A,
	DATA_HALF_FLOAT        = 0x140B,
    DATA_UNSIGNED_INT_24_8 = 0x84FA,
    DATA_FLOAT32_UNSIGNED_INT_24_8 = 0x8DAD
};

//! OpenGL texture format
enum TextureFormat
{
	TF_DEPTH_COMPONENT           = 0x1902,
	TF_RED                       = 0x1903,
    TF_RED_INTEGER               = 0x8D94,
    TF_RG                        = 0x8227,
    TF_RG_INTEGER                = 0x8228,
	TF_RGB                       = 0x1907,
    TF_RGB_INTEGER               = 0x8D98,
	TF_RGBA                      = 0x1908,
    TF_RGBA_INTEGER              = 0x8D99,
	TF_COMPRESSED_RGB_S3TC_DXT1  = 0x83F0,
	TF_COMPRESSED_RGBA_S3TC_DXT1 = 0x83F1,
	TF_COMPRESSED_RGBA_S3TC_DXT3 = 0x83F2,
	TF_COMPRESSED_RGBA_S3TC_DXT5 = 0x83F3,
	TF_DEPTH_STENCIL             = 0x84F9
};

//! OpenGL texture internal format
enum TextureIntFormat
{
	TIF_R8                        = 0x8229,
    TIF_R8_SNORM                  = 0x8F94,
    TIF_R16                       = 0x822A,   //!< not in ES
    TIF_R16_SNORM                 = 0x8F98,   //!< not in ES
    TIF_R16F                      = 0x822D,
    TIF_R32I                      = 0x8235,
    TIF_R32UI                     = 0x8236,
    TIF_R32F                      = 0x822E,
    TIF_R8UI                      = 0x8232,
    TIF_R8I                       = 0x8231,
    TIF_R16UI                     = 0x8234,
    TIF_R16I                      = 0x8233,
    TIF_RG8                       = 0x822B,
    TIF_RG8_SNORM                 = 0x8F95,
    TIF_RG16                      = 0x822C,
    TIF_RG16_SNORM                = 0x8F99,
    TIF_RG16F                     = 0x822F,
    TIF_RG32F                     = 0x8230,
    TIF_RG8UI                     = 0x8238,
    TIF_RG8I                      = 0x8237,
    TIF_RG16UI                    = 0x823A,
    TIF_RG16I                     = 0x8239,
    TIF_RG32UI                    = 0x823C,
    TIF_RG32I                     = 0x823B,
    TIF_RGB8                      = 0x8051,
    TIF_SRGB8                     = 0x8C41,
    TIF_RGB565                    = 0x8D62,
    TIF_RGB8_SNORM                = 0x8F96,
    TIF_RGB16F                    = 0x881B,
    TIF_RGB32F                    = 0x8815,
    TIF_RGB8UI                    = 0x8D7D,
    TIF_RGB8I                     = 0x8D8F,
    TIF_RGB16UI                   = 0x8D77,
    TIF_RGB16I                    = 0x8D89,
    TIF_RGB16_SNORM               = 0x8F9A,
    TIF_RGB32UI                   = 0x8D71,
    TIF_RGB32I                    = 0x8D83,
    TIF_RGBA8                     = 0x8058,
    TIF_SRGB8_ALPHA8              = 0x8C43,
    TIF_RGBA8_SNORM               = 0x8F97,
    TIF_RGBA16                    = 0x805B,
    TIF_RGBA16F                   = 0x881A,
    TIF_RGBA32F                   = 0x8814,
    TIF_RGBA8UI                   = 0x8D7C,
    TIF_RGBA8I                    = 0x8D8E,
    TIF_RGBA16UI                  = 0x8D76,
    TIF_RGBA16I                   = 0x8D88,
    TIF_RGBA16_SNORM              = 0x8F9B,
    TIF_RGBA32I                   = 0x8D82,
    TIF_RGBA32UI                  = 0x8D70,
	TIF_DEPTH_COMPONENT16         = 0x81A5,
	TIF_DEPTH_COMPONENT24         = 0x81A6,
	TIF_DEPTH_COMPONENT32         = 0x81A7,
	TIF_COMPRESSED_RGB_S3TC_DXT1  = 0x83F0,
	TIF_COMPRESSED_RGBA_S3TC_DXT1 = 0x83F1,
	TIF_COMPRESSED_RGBA_S3TC_DXT3 = 0x83F2,
	TIF_COMPRESSED_RGBA_S3TC_DXT5 = 0x83F3,
    TIF_DEPTH24_STENCIL8          = 0x88F0,  //!< 24 bits depth buffer + 8 bits stencil
    TIF_DEPTH_COMPONENT32F        = 0x8CAC,  //!< 32 bits depth buffer
    TIF_DEPTH32F_STENCIL8         = 0x8CAD,
    TIF_STENCIL_INDEX1            = 0x8D46,
    TIF_STENCIL_INDEX4            = 0x8D47,
    TIF_STENCIL_INDEX8            = 0x8D48,
    TIF_STENCIL_INDEX16           = 0x8D49
};

//! OpenGL Texture type.
enum TextureType
{
	TEXTURE_1D       = 0x0DE0,
	TEXTURE_2D       = 0x0DE1,
	TEXTURE_3D       = 0x806F,
	TEXTURE_CUBE_MAP = 0x8513,
	TEXTURE_1D_ARRAY = 0x8C18,
    TEXTURE_2D_ARRAY = 0x8C1A,
    TEXTURE_2D_MULTISAMPLE = 0x9100,
    TEXTURE_2D_MULTISAMPLE_ARRAY = 0x9102
};

//! Primitive format.
enum PrimitiveFormat
{
	P_POINTS = 0,            //!< Points.
	P_LINES = 1,             //!< Lines.
	P_LINE_LOOP = 2,         //!< Line loop.
	P_LINE_STRIP = 3,        //!< Line strip.
	P_TRIANGLES = 4,         //!< Triangles.
	P_TRIANGLE_STRIP = 5,    //!< Triangle Strip.
	P_TRIANGLE_FAN = 6       //!< Triangle Fans.
};

} // namespace o3d

#endif // _O3D_ENGINETYPE_H
