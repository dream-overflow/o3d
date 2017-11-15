/**
 * @file sndraw.cpp
 * @brief Implementation of SndRaw.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2009-01-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/audio/precompiled.h"

#include "o3d/core/filemanager.h"
#include "o3d/core/datainstream.h"
#include "o3d/audio/sndraw.h"

using namespace o3d;

//! Default constructor.
SndRaw::SndRaw(UInt32 format, UInt32 samplingRate) :
	SndFormat(),
    m_is(nullptr),
	m_dataSize(0)
{
	m_format = format;
	m_samplingRate = samplingRate;
}

// Destructor.
SndRaw::~SndRaw()
{
    destroy();
}

void SndRaw::prepare(InStream &is)
{
    if (m_is)
        destroy();

    Int32 size = is.getAvailable();

    switch (m_format)
    {
        case AL_FORMAT_MONO8:
            m_duration = (Float)size / m_samplingRate;
            break;
        case AL_FORMAT_STEREO8:
            m_duration = (Float)(size >> 1) / m_samplingRate;
            break;
        case AL_FORMAT_MONO16:
            m_duration = (Float)(size >> 1) / m_samplingRate;
            break;
        case AL_FORMAT_STEREO16:
            m_duration = (Float)(size >> 2) / m_samplingRate;
            break;
        default:
            m_duration = 0.f;
            break;
    }

    m_dataSize = size;
    m_is = &is;
}

void SndRaw::prepare(const UInt8 *data, UInt32 size)
{
    O3D_ASSERT(data != nullptr);

	if (data && size)
	{
        if (m_is || m_decodedData.getSize())
			destroy();

		m_decodedData.pushArray(data, size);

		switch (m_format)
		{
			case AL_FORMAT_MONO8:
				m_duration = (Float)size / m_samplingRate;
				break;
			case AL_FORMAT_STEREO8:
				m_duration = (Float)(size >> 1) / m_samplingRate;
				break;
			case AL_FORMAT_MONO16:
				m_duration = (Float)(size >> 1) / m_samplingRate;
				break;
			case AL_FORMAT_STEREO16:
				m_duration = (Float)(size >> 2) / m_samplingRate;
				break;
			default:
				m_duration = 0.f;
				break;
		}
	}
}

// Load from a valid filename.
void SndRaw::prepare(const String &filename)
{
    if (m_is)
        destroy();

    InStream *is = FileManager::instance()->openInStream(filename);

    Int32 size = is->getAvailable();

    switch (m_format)
    {
        case AL_FORMAT_MONO8:
            m_duration = (Float)size / m_samplingRate;
            break;
        case AL_FORMAT_STEREO8:
            m_duration = (Float)(size >> 1) / m_samplingRate;
            break;
        case AL_FORMAT_MONO16:
            m_duration = (Float)(size >> 1) / m_samplingRate;
            break;
        case AL_FORMAT_STEREO16:
            m_duration = (Float)(size >> 2) / m_samplingRate;
            break;
        default:
            m_duration = 0.f;
            break;
    }

    m_dataSize = size;
    m_is = is;
}

// Destroy the data.
void SndRaw::destroy()
{
    deletePtr(m_is);

	m_samplingRate = m_format = 0;
	m_dataSize = 0;
	m_duration = 0.f;

	m_decodedData.destroy();
}

// Get the sound duration in seconds.
Float SndRaw::getDuration() const
{
	return m_duration;
}

// Decode the sound data.
Bool SndRaw::decode()
{
	// if the file is defined decode data consist to simply read raw data
    if (m_is)
	{
		UInt32 result;

        m_is->reset(0);

        m_decodedData.setSize(m_is->getAvailable());
        result = m_is->read(m_decodedData.getData(), m_is->getAvailable());

        deletePtr(m_is);
		return (result == (UInt32)m_decodedData.getSize());
	}
	else
		return (m_decodedData.getSize() > 0);
}


//---------------------------------------------------------------------------------------
// RawStreamer
//---------------------------------------------------------------------------------------

// Default constructor. Take a valid SndRaw object.
SndRawStreamer::SndRawStreamer(SndRaw &raw) :
	SndStream(),
		m_source(&raw),
        m_stream(nullptr),
		m_position(0)
{
	O3D_ASSERT(m_source.isValid());

	// create a stream buffer if the source is a file
    if (m_source->m_is)
		m_stream = new UInt8[SndStream::BUFFER_SIZE];
}

// Destructor.
SndRawStreamer::~SndRawStreamer()
{
	destroy();
}

// Destructor.
void SndRawStreamer::destroy()
{
	deleteArray(m_stream);
}

Bool SndRawStreamer::isDefined() const
{
	return (isValid() && m_source->isValid());
}

Bool SndRawStreamer::isValid() const
{
	return m_source.isValid();
}

UInt32 SndRawStreamer::getSize() const
{
	return 0;
}

UInt32 SndRawStreamer::getFormat() const
{
	return (m_source ? m_source->getFormat() : 0);
}

UInt32 SndRawStreamer::getSamplingRate() const
{
	return (m_source ? m_source->getSamplingRate() : 0);
}

Float SndRawStreamer::getDuration() const
{
	return (m_source ? m_source->getDuration() : 0);
}

const UInt8* SndRawStreamer::getStreamChunk(UInt32 seek, UInt32 &size, Bool &finished)
{
	size = 0;

	// If file
    if (m_source->m_is)
	{
		// out of memory
		if (seek >= m_source->m_dataSize)
		{
			size = 0;
			finished = True;
            return nullptr;
		}

		// is need to seek
        if (seek != (UInt32)m_source->m_is->getPosition())
		{
            m_source->m_is->reset(seek);
			m_position = seek;
		}

		// truncate when the end of data block is reached
		UInt32 bufferSize = o3d::min<UInt32>(SndStream::BUFFER_SIZE, m_source->m_dataSize - m_position);

        size = m_source->m_is->read(m_stream, bufferSize);
		m_position += size;

		// finished
		if (size < (UInt32)SndStream::BUFFER_SIZE)
			finished = True;
		else
			finished = False;

		// no data
		if (size == 0)
            return nullptr;

		// some data
		return m_stream;
	}
	// If no file
	else if (m_source->m_decodedData.getSize())
	{
		if (seek >= (UInt32)m_source->m_decodedData.getSize())
		{
			size = 0;
			finished = True;

            return nullptr;
		}

		// is need to seek
		if (seek != m_position)
		{
			m_position = seek;
		}

		// truncate when the end of data block is reached
		size = o3d::min<UInt32>(SndStream::BUFFER_SIZE, m_source->m_decodedData.getSize() - m_position);

		UInt32 offset = m_position;
		m_position += size;

		// finished
		if (size < (UInt32)SndStream::BUFFER_SIZE)
			finished = True;
		else
			finished = False;

		// no data
		if (size == 0)
            return nullptr;

		// some data
		return m_source->m_decodedData.getData() + offset;
	}
	else
        return nullptr;
}

// Get the current stream position.
UInt32 SndRawStreamer::getStreamPos() const
{
	return m_position;
}

// rewind the stream (useful pour looping)
void SndRawStreamer::rewind()
{
	m_position = 0;
}

// is the stream is had finished to play or not
Bool SndRawStreamer::isFinished() const
{
    if (m_source->m_is)
		return (m_position >= m_source->m_dataSize);
	else
		return (m_position >= (UInt32)m_source->m_decodedData.getSize());
}

