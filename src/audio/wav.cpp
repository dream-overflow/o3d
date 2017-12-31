/**
 * @file wav.cpp
 * @brief Implementation of Wav.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-05-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/audio/precompiled.h"

#include "o3d/audio/wav.h"
#include "o3d/core/debug.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/datainstream.h"
#include "o3d/core/templatemanager.h"

#include "o3d/audio/al.h"
#include "o3d/audio/alc.h"

using namespace o3d;

//! Default constructor.
Wav::Wav() :
	SndFormat(),
	m_duration(0.f),
	m_rewindPos(0),
    m_is(nullptr),
	m_dataSize(0)
{
}

// destructor
Wav::~Wav()
{
    destroy();
}

void Wav::prepare(InStream &is)
{
	if (m_is || m_decodedData.getSize())
		destroy();

	Char   riff[4]; // 'RIFF'
	Int32  riffSize;
	Char   wave[4]; // 'WAVE'
	Char   fmt[4];  // 'fmt '
	UInt32 fmtSize;
	UInt16 Format;
	UInt16 Channels;
	UInt32 SamplesPerSec;
	UInt32 BytesPerSec;
	UInt16 BlockAlign;
	UInt16 BitsPerSample;
	Char   data[4];  // 'data'
	UInt32 dataSize;

	// read the WAV header; (already checked by O3DSnd)
    is.read(riff, 4);
    is >> riffSize;
    is.read(wave, 4);

	if (wave[0] != 'W' || wave[1] != 'A' || wave[2] != 'V' || wave[3] != 'E')
        O3D_ERROR(E_InvalidFormat("Invalid WAV file format"));

    is.read(fmt, 4);

	if (fmt[0] != 'f' || fmt[1] != 'm' || fmt[2] != 't' || fmt[3] != ' ')
        O3D_ERROR(E_InvalidFormat("Invalid WAV fmt format"));

    is >> fmtSize
       >> Format
       >> Channels
       >> SamplesPerSec
       >> BytesPerSec
       >> BlockAlign
       >> BitsPerSample;
	
	if (Format != 1)
        O3D_ERROR(E_InvalidFormat("Compressed WAV formats are not supported"));

    is.read(data, 4);

	// padding
	if ((data[0] == 'P') && (data[1] == 'A') && (data[2] == 'D') && (data[3] == ' '))
	{
		Int32 padding;

        is >> padding;
        is.seek(padding);

        is.read(data, 4);
	}

	if ((data[0] != 'd') || (data[1] != 'a') || (data[2] != 't') || (data[3] != 'a'))
        O3D_ERROR(E_InvalidFormat("Invalid WAV data token"));

    is >> dataSize;

	// Rewind pos
    m_rewindPos = is.getPosition();

	if (Channels == 1 && BitsPerSample == 16)
		m_format = AL_FORMAT_MONO16;
	else if (Channels == 1 && BitsPerSample == 8)
		m_format = AL_FORMAT_MONO8;
	else if (Channels == 2 && BitsPerSample == 8)
		m_format = AL_FORMAT_STEREO8;
	else if (Channels == 2 && BitsPerSample == 16)
		m_format = AL_FORMAT_STEREO16;

	m_samplingRate = SamplesPerSec;

	// compute the duration in seconds
	m_duration = (Float)(dataSize / Channels / (BitsPerSample >> 3)) / (Float)SamplesPerSec;
	m_dataSize = dataSize;

    m_is = &is;
}

void Wav::prepare(const String &filename)
{
    if (m_is || m_decodedData.getSize())
        destroy();

    InStream *pis = FileManager::instance()->openInStream(filename);
    InStream &is = *pis;

    Char   riff[4]; // 'RIFF'
    Int32  riffSize;
    Char   wave[4]; // 'WAVE'
    Char   fmt[4];  // 'fmt '
    UInt32 fmtSize;
    UInt16 Format;
    UInt16 Channels;
    UInt32 SamplesPerSec;
    UInt32 BytesPerSec;
    UInt16 BlockAlign;
    UInt16 BitsPerSample;
    Char   data[4];  // 'data'
    UInt32 dataSize;

    // read the WAV header; (already checked by O3DSnd)
    is.read(riff, 4);
    is >> riffSize;
    is.read(wave, 4);

    if (wave[0] != 'W' || wave[1] != 'A' || wave[2] != 'V' || wave[3] != 'E')
        O3D_ERROR(E_InvalidFormat("Invalid WAV file format"));

    is.read(fmt, 4);

    if (fmt[0] != 'f' || fmt[1] != 'm' || fmt[2] != 't' || fmt[3] != ' ')
        O3D_ERROR(E_InvalidFormat("Invalid WAV fmt format"));

    is >> fmtSize
       >> Format
       >> Channels
       >> SamplesPerSec
       >> BytesPerSec
       >> BlockAlign
       >> BitsPerSample;

    if (Format != 1)
        O3D_ERROR(E_InvalidFormat("Compressed WAV formats are not supported"));

    is.read(data, 4);

    // padding
    if ((data[0] == 'P') && (data[1] == 'A') && (data[2] == 'D') && (data[3] == ' '))
    {
        Int32 padding;

        is >> padding;
        is.seek(padding);

        is.read(data, 4);
    }

    if ((data[0] != 'd') || (data[1] != 'a') || (data[2] != 't') || (data[3] != 'a'))
        O3D_ERROR(E_InvalidFormat("Invalid WAV data token"));

    is >> dataSize;

    // Rewind pos
    m_rewindPos = is.getPosition();

    if (Channels == 1 && BitsPerSample == 16)
        m_format = AL_FORMAT_MONO16;
    else if (Channels == 1 && BitsPerSample == 8)
        m_format = AL_FORMAT_MONO8;
    else if (Channels == 2 && BitsPerSample == 8)
        m_format = AL_FORMAT_STEREO8;
    else if (Channels == 2 && BitsPerSample == 16)
        m_format = AL_FORMAT_STEREO16;

    m_samplingRate = SamplesPerSec;

    // compute the duration in seconds
    m_duration = (Float)(dataSize / Channels / (BitsPerSample >> 3)) / (Float)SamplesPerSec;
    m_dataSize = dataSize;

    m_is = pis;
}

// destroy all content
void Wav::destroy()
{
	deletePtr(m_is);

	m_samplingRate = m_format = 0;
	m_duration = 0.f;
	m_rewindPos = 0;

	m_decodedData.destroy();
}

Float Wav::getDuration() const
{
	return m_duration;
}

Bool Wav::decode()
{
	if (m_is)
	{
		UInt32 result;
        Int32 size = m_is->getAvailable() - m_rewindPos;
        m_is->reset(m_rewindPos);

		m_decodedData.setSize(size);
        result = m_is->read(m_decodedData.getData(), size);

		deletePtr(m_is);
		return ((UInt32)m_decodedData.getSize() == result);
	}
	else
		return (m_decodedData.getSize() > 0);
}


//---------------------------------------------------------------------------------------
// O3DWavStreamer
//---------------------------------------------------------------------------------------

// Default constructor. Take a valid O3DWav object.
WavStreamer::WavStreamer(Wav &wav) :
	SndStream(),
		m_source(&wav),
        m_stream(nullptr),
		m_position(0)
{
	O3D_ASSERT(m_source.isValid());

	// create a stream buffer if the source is a file
	if (m_source->m_is)
		m_stream = new UInt8[SndStream::BUFFER_SIZE];
}

// Destructor.
WavStreamer::~WavStreamer()
{
	destroy();
}

// Destroy.
void WavStreamer::destroy()
{
	deleteArray(m_stream);
}

Bool WavStreamer::isDefined() const
{
	return (isValid() && m_source->isValid());
}

Bool WavStreamer::isValid() const
{
	return m_source.isValid();
}

UInt32 WavStreamer::getSize() const
{
	return 0;
}

UInt32 WavStreamer::getFormat() const
{
	return (m_source.isValid() ? m_source->getFormat() : 0);
}

UInt32 WavStreamer::getSamplingRate() const
{
	return (m_source.isValid() ? m_source->getSamplingRate() : 0);
}

Float WavStreamer::getDuration() const
{
	return (m_source.isValid() ? m_source->getDuration() : 0);
}

const UInt8* WavStreamer::getStreamChunk(UInt32 seek, UInt32 &size, Bool &finished)
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
        if (seek != (UInt32)m_source->m_is->getPosition() - m_source->m_rewindPos)
		{
            m_source->m_is->reset(seek + m_source->m_rewindPos);
			m_position = seek;
		}

		// truncate when the end of data block is reached
		UInt32 bufferSize = o3d::min<UInt32>(SndStream::BUFFER_SIZE, m_source->m_dataSize - m_position);

        size = m_source->m_is->read(m_stream, bufferSize);
		m_position += size;

		// finished
		if (size < (Int32)SndStream::BUFFER_SIZE)
			finished = True;
		else
			finished = False;

		// no data
		if (size == 0)
            return nullptr;

		// some data
		return m_stream;
	}
	// If no file, direct data
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
		if (size < (Int32)SndStream::BUFFER_SIZE)
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
UInt32 WavStreamer::getStreamPos() const
{
	return m_position;
}

// rewind the stream (useful pour looping)
void WavStreamer::rewind()
{
	m_position = 0;
}

// is the stream is had finished to play or not
Bool WavStreamer::isFinished() const
{
	if (m_source->m_is)
		return (m_position >= m_source->m_dataSize);
	else
		return (m_position >= (UInt32)m_source->m_decodedData.getSize());
}

