/**
 * @file sndstream.cpp
 * @brief Implementation of SndStream.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-10-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/audio/precompiled.h"

#include "o3d/audio/sndstream.h"

using namespace o3d;

// Default constructor
SndStream::SndStream() :
	SndBufferObject(),
		m_frontBuffer(0),
		m_backBuffer(0)
{
	m_type = AUDIO_SND_STREAM;
}

Bool SndStream::isStream() const
{
    return True;
}

void SndStream::load(const Sound &/*snd*/)
{
}
