/**
 * @file sndbuffer.cpp
 * @brief Implementation of SndBuffer.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2003-04-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/audio/precompiled.h"

#include "o3d/core/mutex.h"
#include "o3d/audio/sndbuffer.h"
#include "o3d/audio/sndstream.h"
#include "o3d/audio/sound.h"
#include "o3d/core/debug.h"
#include "o3d/core/virtualfilelisting.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/memorydbg.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/audio/audio.h"

#include "o3d/audio/al.h"
#include "o3d/audio/alc.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(SndBuffer, AUDIO_SND_BUFFER, SceneResource)

// Default constructor
SndBuffer::SndBuffer(
	BaseObject *parent,
	Float decodeMaxDuration) :
			SceneResource(parent),
            m_bufferObject(nullptr),
			m_frontBuffer(O3D_UNDEFINED),
			m_backBuffer(O3D_UNDEFINED),
			m_streamPos(0),
			m_decodeMaxDuration(decodeMaxDuration)
{
}

// Construct using a generated and valid sound.
SndBuffer::SndBuffer(
	BaseObject *parent,
	const Sound &sound,
	Float decodeMaxDuration) :
			SceneResource(parent),
            m_bufferObject(nullptr),
			m_frontBuffer(O3D_UNDEFINED),
			m_backBuffer(O3D_UNDEFINED),
			m_streamPos(0),
			m_decodeMaxDuration(decodeMaxDuration)
{
	if (sound.isEmpty())
		O3D_ERROR(E_InvalidParameter("Sound must be valid"));

	m_sound = sound;
}

// Destructor
SndBuffer::~SndBuffer()
{
	destroy();
}

// Get the sound buffer sampling rate (8,11,22,44kHz).
UInt32 SndBuffer::getSamplingRate() const
{
	if (m_sound.isValid())
		return m_sound.getSamplingRate();
	else if (m_bufferObject)
		return m_bufferObject->getSamplingRate();
	else
		return 0;
}

// Get the size of a channel in bits (8 or 16bits).
UInt32 SndBuffer::getChannelFormat() const
{
	if (m_sound.isValid())
		return m_sound.getFormat();
	else if (m_bufferObject)
		return m_bufferObject->getFormat();
	else
		return 0;
}

// Get bit per sample (8,16).
UInt32 SndBuffer::getBitsPerSample() const
{
	UInt32 format = 0;

	if (m_sound.isValid())
		format = m_sound.getFormat();
	else if (m_bufferObject)
		format = m_bufferObject->getFormat();

	switch (format)
	{
		case AL_FORMAT_MONO16:
		case AL_FORMAT_STEREO16:
			return 16;
		case AL_FORMAT_MONO8:
		case AL_FORMAT_STEREO8:
			return 8;
		default:
			return 0;
	}
}

// Get the number of channels (1 or 2).
UInt32 SndBuffer::getNumChannels() const
{
	UInt32 format = 0;

	if (m_sound.isValid())
		format = m_sound.getFormat();
	else if (m_bufferObject)
		format = m_bufferObject->getFormat();

	switch (format)
	{
		case AL_FORMAT_MONO8:
		case AL_FORMAT_MONO16:
			return 1;
		case AL_FORMAT_STEREO8:
		case AL_FORMAT_STEREO16:
			return 2;
		default:
			return 0;
	}
}

// Create OpenAL buffer.
Bool SndBuffer::create(Bool unloadSound)
{
	if (m_bufferObject)
		return True;

	// if the sound data is no longer in memory we try to load it before
	if (m_sound.isEmpty())
	{
        if (getResourceName().startsWith("<"))
			return False;

		// load the sound
        m_sound.load(getResourceName(), m_decodeMaxDuration);
	}

	// single buffer
	if ((m_sound.getDuration() <= m_decodeMaxDuration) ||
		((m_sound.getSize() <= SndStream::BUFFER_SIZE) && (m_sound.getSize() != 0)))
	{
		m_bufferObject = new ALBuffer();

		// single buffer object
		m_bufferObject->load(m_sound);
	}
	else
	{
		// streamed with front and back buffers
		m_bufferObject = m_sound.createStreamer();

		alGenBuffers(1, &m_frontBuffer);
		alGenBuffers(1, &m_backBuffer);
	}

	if (unloadSound)
		m_sound.destroy();

	// Valid sound buffer
	onSndBufferValid(this);

	return True;
}

// Delete the buffer
void SndBuffer::destroy()
{
	m_streamPos = 0;

	deletePtr(m_bufferObject);

	if (m_frontBuffer != O3D_UNDEFINED)
	{
		O3D_SFREE(MemoryManager::SFX_STREAM_BUFFER, m_frontBuffer);

		alDeleteBuffers(1, &m_frontBuffer);
		m_frontBuffer = O3D_UNDEFINED;
	}

	if (m_backBuffer != O3D_UNDEFINED)
	{
		O3D_SFREE(MemoryManager::SFX_STREAM_BUFFER, m_backBuffer);

		alDeleteBuffers(1, &m_backBuffer);
		m_backBuffer = O3D_UNDEFINED;
	}

	m_sound.destroy();
}

// Unload the sound data, but keep it in OpenAL
void SndBuffer::unloadSound()
{
	m_sound.destroy();
}

// Get the front or single buffer id.
UInt32 SndBuffer::getFrontBufferId() const
{
	if (m_bufferObject)
	{
		if (m_bufferObject->isStream())
			return m_frontBuffer;
		else
			return static_cast<ALBuffer*>(m_bufferObject)->getBufferId();
	}
	else
		return 0;
}

// Get the back buffer id.
UInt32 SndBuffer::getBackBufferId() const
{
	if (m_bufferObject)
	{
		if (m_bufferObject->isStream())
			return m_backBuffer;
		else
			return 0;
	}
	else
		return 0;
}

// update the buffer: only for streamed buffer (with a back-buffer)
Bool SndBuffer::updateStream(UInt32 sourceId, Bool loop, UInt32 &position)
{
	// only for a streamed sound
	if (!m_bufferObject->isStream())
		return False;

	Int32 processed;
	Bool finished = False;

	alGetSourcei(sourceId, AL_BUFFERS_PROCESSED, &processed);

	SndStream *sndStream = static_cast<SndStream*>(m_bufferObject);

	m_streamPos = position;

	while (processed--)
	{
		ALuint bufferId;

		// un-queue the read buffer
		alSourceUnqueueBuffers(sourceId, 1, &bufferId);

		// update buffer data if not finished
		if (!finished)
		{
			UInt32 size;
			const UInt8 *data = sndStream->getStreamChunk(m_streamPos, size, finished);

			// is finished and loop needed
			if (finished && loop)
			{
				finished = False;
				m_streamPos = position = 0;

				// need data
				if (!data)
					data = sndStream->getStreamChunk(m_streamPos, size, finished);
			}

			// data to fill in ?
			if (data)
			{
				m_streamPos = position = sndStream->getStreamPos();

				// fill the buffer
				alBufferData(bufferId, m_bufferObject->getFormat(), (ALvoid*)data, size, m_bufferObject->getSamplingRate());

				// and queue the updated buffer
				alSourceQueueBuffers(sourceId, 1, &bufferId);
			}
		}
	}

	return !finished;
}

// prepare the stream to be played
Bool SndBuffer::prepareStream(UInt32 sourceId, UInt32 &position)
{
	// only for a streamed sound
	if (!m_bufferObject->isStream())
		return False;

	UInt32 size;
	Bool finished = True;

	SndStream *sndStream = static_cast<SndStream*>(m_bufferObject);

	m_streamPos = 0;

	// set the front buffer
	if (m_frontBuffer != O3D_UNDEFINED)
	{
		const UInt8* data = sndStream->getStreamChunk(m_streamPos, size, finished);
		if (!data)
			return False;

		O3D_SALLOC(MemoryManager::SFX_STREAM_BUFFER, m_frontBuffer, size);

		alBufferData(
			m_frontBuffer,
			m_bufferObject->getFormat(),
			(ALvoid*)data,
			size,
			m_bufferObject->getSamplingRate());

		m_streamPos = position = sndStream->getStreamPos();

		// and queue the buffer
		alSourceQueueBuffers(sourceId, 1, &m_frontBuffer);
	}

	// set the back buffer
	if ((m_backBuffer != O3D_UNDEFINED) && !finished)
	{
		const UInt8* data = sndStream->getStreamChunk(m_streamPos, size, finished);
		if (!data)
			return False;

		O3D_SALLOC(MemoryManager::SFX_STREAM_BUFFER, m_backBuffer, size);

		alBufferData(
			m_backBuffer,
			m_bufferObject->getFormat(),
			(ALvoid*)data,
			size,
			m_bufferObject->getSamplingRate());

		m_streamPos = position = sndStream->getStreamPos();

		// and queue the buffer
		alSourceQueueBuffers(sourceId, 1, &m_backBuffer);
	}

	return True;
}

// Serialization
Bool SndBuffer::writeToFile(OutStream &os)
{
    BaseObject::writeToFile(os);

    os   << m_resourceName
		 << m_decodeMaxDuration;

	return True;
}

Bool SndBuffer::readFromFile(InStream &is)
{
    BaseObject::readFromFile(is);

    is   >> m_resourceName
		 >> m_decodeMaxDuration;

	return True;
}

//---------------------------------------------------------------------------------------
// SndBufferTask
//---------------------------------------------------------------------------------------

// Default constructor.
SndBufferTask::SndBufferTask(
		SndBuffer *sndBuffer,
		const String &filename,
		Float decodeMaxDuration) :
			m_sndBuffer(sndBuffer),
			m_decodeMaxDuration(decodeMaxDuration)
{
	if (!sndBuffer)
		O3D_ERROR(E_InvalidParameter("The sound buffer must be valid"));

	m_filename = FileManager::instance()->getFullFileName(filename);
}

Bool SndBufferTask::execute()
{
	O3D_ASSERT(m_filename.isValid());
	if (m_filename.isValid())
	{
        try {
            m_sound.load(m_filename, m_decodeMaxDuration);
        }
        catch (E_BaseException &)
        {
            return False;
        }

        return True;
	}
	else
		return False;
}

Bool SndBufferTask::finalize()
{
	if (m_sound.isValid())
	{
		m_sndBuffer->getSound() = m_sound;
		return m_sndBuffer->create();
	}
	else
		return False;
}

