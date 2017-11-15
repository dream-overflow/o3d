/**
 * @file albuffer.cpp
 * @brief Implementation of ALBuffer.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-12-31
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/audio/precompiled.h"

#include "o3d/core/debug.h"

#include "o3d/audio/albuffer.h"
#include "o3d/audio/al.h"

using namespace o3d;

//---------------------------------------------------------------------------------------
// SndBufferObject
//---------------------------------------------------------------------------------------

// Default constructor.
SndBufferObject::SndBufferObject()
{
	m_type = AUDIO_SND_BUFFER_OBJECT;
}

// Virtual destructor.
SndBufferObject::~SndBufferObject()
{
}

//---------------------------------------------------------------------------------------
// O3DALBuffer
//---------------------------------------------------------------------------------------

// Default constructor.
ALBuffer::ALBuffer() :
	SndBufferObject(),
		m_bufferId(O3D_UNDEFINED),
		m_duration(0.f),
		m_size(0),
		m_format(0),
		m_samplingRate(0)
{
	m_type = AUDIO_AL_BUFFER;
}

// Virtual destructor.
ALBuffer::~ALBuffer()
{
	if (m_bufferId != 0)
	{
		O3D_SFREE(MemoryManager::SFX_SINGLE_BUFFER, m_bufferId);
		alDeleteBuffers(1, &m_bufferId);
	}
}

// Load a sound sample according the the type previously defined.
void ALBuffer::load(const Sound &snd)
{
	if (snd.isEmpty())
		return;

	UInt32 sndSize;
	Bool realloc = False;

	if (m_bufferId == O3D_UNDEFINED)
		alGenBuffers(1, &m_bufferId);
	else
		realloc = True;

	switch (m_type)
	{
		case AUDIO_AL_BUFFER:
			sndSize = loadSound(m_bufferId, snd);
			break;

        default:
			sndSize = 0;
            O3D_ERROR(E_InvalidFormat("The sound type is not compatible"));
            break;
	}

	if (realloc)
		O3D_SREALLOC(MemoryManager::SFX_SINGLE_BUFFER, m_bufferId, sndSize);
	else
		O3D_SALLOC(MemoryManager::SFX_SINGLE_BUFFER, m_bufferId, sndSize);

	m_size = snd.getSize();
	m_format = snd.getFormat();
	m_samplingRate = snd.getSamplingRate();
	m_duration = snd.getDuration();
}

//! Define the buffer parameters.
Bool ALBuffer::define(
	UInt32 format,
	UInt32 size,
	UInt32 samplingRate)
{
	Bool created = False;

	if (m_bufferId == O3D_UNDEFINED)
	{
		alGenBuffers(1, &m_bufferId);
		created = True;
	}

	m_format = format;
	m_size = size;
	m_samplingRate = samplingRate;

	switch (format)
	{
		case AL_FORMAT_MONO8:
			m_duration = (Float)(m_size) / m_samplingRate;
			break;
		case AL_FORMAT_MONO16:
			m_duration = (Float)(m_size >> 1) / m_samplingRate;
			break;
		case AL_FORMAT_STEREO8:
			m_duration = (Float)(m_size >> 1) / m_samplingRate;
			break;
		case AL_FORMAT_STEREO16:
			m_duration = (Float)(m_size >> 2) / m_samplingRate;
			break;
		default:
			m_duration = 0;
			break;
	}

	return created;
}

// load a a sound to OpenAL.
UInt32 ALBuffer::loadSound(UInt32 id, const Sound &snd)
{
	alBufferData(
		id,
		snd.getFormat(),
		(ALvoid*)snd.getData(),
		snd.getSize(),
		snd.getSamplingRate());

	return snd.getSize();
}
