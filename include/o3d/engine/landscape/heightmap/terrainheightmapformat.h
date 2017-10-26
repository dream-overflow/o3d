/**
 * @file terrainheightmapformat.h
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2009-09-05
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef __O3D_TERRAINHEIGHTMAPFORMAT_H
#define __O3D_TERRAINHEIGHTMAPFORMAT_H

#include "o3d/core/types.h"

namespace o3d {

namespace TerrainHeightmap
{
struct FileHeader
{
	Char	header[8];
	Char	version[8];
	UInt32	crc32;

	UInt32	heightmapSize[2];
	Float	heightmapUnits[3];
	UInt8	attribCount;
	UInt8	materialCount;
};

struct AttribHeader
{
	Char	name[64];
	UInt64	position;
	UInt64	size;
	Char	format[32];
};

struct MaterialHeader
{
	Char	name[64];
	Char	type[64];
	UInt8	textureCount;
};

struct TextureHeader
{
	Char		name[64];
	Char		format[64];
	UInt16		size[2];
	UInt8		bufferLocation;
	UInt64		bufferPosition;
	UInt64		bufferSize;
	Char		path[512];
};

} // namespace TerrainHeightmap
} // namespace o3d

#endif // __O3D_TERRAINHEIGHTMAPFORMAT_H

/* 
TerrainHeightmap Format

==================== FILE HEADER ============================
Title					Type					Description
-----------------------------------------------
Header 			:		char[8]					= "O3DHMP  "
Version			:		char[8] 				= "1.0"
Checksum		:		uint32					= (crc32)
Size			:		uint32[2]				= (width,height)
Units			:		float[3]				= (unitx,unity,unitz)
Attribute count	:		uint8					= (attribute count)
Material count	:		uint8					= (material count)
Attribu headers	:		ATTRIB_HEADER[attribute count]
Material headers:		MATERIAL_HEADER[material count]

==================== MATERIAL HEADER ============================
Title					Type		Description
-----------------------------------------------
Attribute name	:		char[64]				= (name)
Buffer position	:		uint64					= (position from the beginning of the header)
Buffer size		:		uint64					= (size in byte)
Buffer format	:		char[32]				= (format descriptor) : raw/zip...)

==================== MATERIAL HEADER ============================
Title					Type		Description
-----------------------------------------------
Material name	:		char[64]	= (name)
Material type	:		char[64]	= (texture|normalmap|parallax|etc...)
Texture count	:		uint8		= (texture count)
Texture headers:		TEXTURE_HEADER[texture count]

==================== TEXTURE HEADER ============================
Title					Type		Description
-----------------------------------------------
Texture name	:		char[64]	= (color|normal|heightmap|etc...)
Texture format	:		char[64]	= (format description : bmp|png|dds|...)
Texture size	:		uint16[2]	= (width,height)
Texture location :		uint8		= (0:extern file|1:included)
Texture pos		:		uint64		= (texture_1 position) = 0 if location is 0
Texture size	:		uint64		= (size in byte) = 0 if location is 0
Texture file	:		char[512]	= (file path) not used if location is 1

Notes:
1) Texture names must match with the shader the material uses
2) Texture types are not relevant, you just have to be sure that O3D will be able to load them
*/

