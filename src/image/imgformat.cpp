/**
 * @file imgformat.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-10-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/image/precompiled.h"
#include "o3d/image/imgformat.h"

using namespace o3d;

// copy constructor
ImgFormat::ImgFormat() :
	SmartCounter<ImgFormat>(),
	m_state(False),
	m_width(0),
	m_height(0),
    m_data(nullptr),
	m_size(0)
{
}

// Copy constructor
ImgFormat::ImgFormat(const ImgFormat &dup) :
	SmartCounter<ImgFormat>(dup),
	m_state(dup.m_state),
	m_width(dup.m_width),
	m_height(dup.m_height),
    m_data(nullptr),
	m_size(dup.m_size)
{
	if (dup.m_data && dup.m_state)
	{
		m_data = new UInt8[dup.m_size];
		memcpy(m_data,dup.m_data,m_size);

		m_state = True;
	}
}

// Virtual destructor. Make a call to Destroy().
ImgFormat::~ImgFormat()
{
	destroy();
}

// Detach its data array. The internal data member is then NULL.
void ImgFormat::detach()
{
    m_data = nullptr;
}

// Get picture informations
void ImgFormat::getInfo(
	UInt32 &sizeX_,
	UInt32 &sizeY_,
	UInt8 *&data_,
	UInt32 &size_)
{
	sizeX_ = m_width;
	sizeY_ = m_height;

	data_ = m_data;
	size_ = m_size;
}

// destroy all
void ImgFormat::destroy()
{
	if (m_state)
	{
        deleteArray(m_data);//m_data = nullptr;
		m_size = 0;

		m_state = False;

		m_width = 0;
		m_height = 0;
	}
	else
	{
        O3D_ASSERT(m_data == nullptr);
	}
}

// is a complex picture
Bool ImgFormat::isComplex() const
{
	return False;
}

// is a compressed picture
Bool ImgFormat::isCompressed() const
{
	return False;
}

// Complex methods
Bool ImgFormat::isMipMap() const
{
	return False;
}

Bool ImgFormat::isCubeMap() const
{
	return False;
}

Bool ImgFormat::isVolumeTexture() const
{
	return False;
}

UInt32 ImgFormat::getNumMipMapLvl() const
{
	return 0;
}

UInt32 ImgFormat::getNumDepthLayer() const
{
	return 0;
}

Bool ImgFormat::bindMipMapLvl(UInt32 lvl)
{
	if (lvl == 0)
		return True;

	return False;
}

void ImgFormat::bindCubeMapSide(CubeMapSide side)
{
	// Nothing
}

Bool ImgFormat::bindVolumeLayer(UInt32 layer)
{
	if (layer == 0)
		return True;

	return False;
}

UInt32 ImgFormat::getCurrentMipMapLvl() const
{
	return 0;
}

UInt32 ImgFormat::getCurrentCubeMapSide() const
{
	return 0;
}

UInt32 ImgFormat::getCurrentDepthLayer() const
{
	return 0;
}

