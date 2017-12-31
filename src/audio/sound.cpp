/**
 * @file sound.cpp
 * @brief Implementation of Sound.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-04-15
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/audio/precompiled.h"

#include "o3d/audio/sound.h"
#include "o3d/audio/audio.h"
#include "o3d/audio/audiorenderer.h"
#include "o3d/audio/oggvorbis.h"
#include "o3d/audio/sndraw.h"
#include "o3d/audio/wav.h"
#include "o3d/core/debug.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/templatemanager.h"

using namespace o3d;

// Constructor
Sound::Sound() :
	m_format(0),
	m_size(0),
	m_sampling(0),
    m_data(nullptr),
	m_dataType(AUDIO_SOUND)
{
}

// Copy constructor
Sound::Sound(const Sound &dup) :
	m_format(0),
	m_size(0),
	m_sampling(0),
    m_data(nullptr),
	m_dataType(AUDIO_SOUND)
{
	m_format = dup.m_format;
	m_size = dup.m_size;
	m_sampling = dup.m_sampling;
	m_dataType = dup.m_dataType;

	if (dup.m_data.get())
	{
		// shallow copy
		m_data = dup.m_data;
	}
}

// Destructor
Sound::~Sound()
{
	destroy();
}

// Assign
Sound& Sound::operator=(const Sound& dup)
{
	destroy();

	m_format = dup.m_format;
	m_size = dup.m_size;
	m_sampling = dup.m_sampling;
	m_dataType = dup.m_dataType;

	if (dup.m_data.get())
	{
		// shallow copy
		m_data = dup.m_data;
	}

	return *this;
}

// Get the data buffer and size.
const UInt8* Sound::getData() const
{
	if (m_data.get())
		return m_data->getData();
	else
        return nullptr;
}

// Get the sound duration in seconds.
Float Sound::getDuration() const
{
	if (m_data.get())
		return m_data->getDuration();
	else
		return 0.f;
}

// Get the number of channels (1 or 2).
UInt32 Sound::getNumChannels() const
{
	switch (m_format)
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

// Get the sound format size (8 or 16bits)
UInt32 Sound::getBitsPerSample() const
{
	switch (m_format)
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

// Load from a filename
void Sound::load(
	const String& filename,
	Int32 format,
	const UInt8 *data,
	UInt32 size,
	UInt32 sampling)
{
    if (!data || size == 0)
        O3D_ERROR(E_NullPointer("Data must be valid"));

    SndRaw *raw = new SndRaw(format, sampling);
    m_data = raw;

    raw->prepare(data, size);

    m_format = format;
    m_size = size;
    m_sampling = sampling;

    m_dataType = AUDIO_SOUND_RAW;
}

// Load a WAV file from an opened file
void Sound::loadWav(InStream &is)
{
	m_data = new Wav();

    m_data->prepare(is);

	m_format = m_data->getFormat();
	m_sampling = m_data->getSamplingRate();
	m_size = m_data->getSize();

	m_dataType = AUDIO_SOUND_WAV;
}

void Sound::loadOgg(InStream &is)
{
	m_data = new OggVorbis();

    m_data->prepare(is);

	m_format = m_data->getFormat();
	m_sampling = m_data->getSamplingRate();
	m_size = m_data->getSize();

	m_dataType = AUDIO_SOUND_OGG;
}

void Sound::loadMp3(InStream &is)
{
//    m_data = new OggVorbis();

//    m_data->prepare(is);

//    m_format = m_data->getFormat();
//    m_sampling = m_data->getSamplingRate();
//    m_size = m_data->getSize();

//    m_dataType = AUDIO_SOUND_MP3;
	O3D_ASSERT(0);
}

// load from a filename
void Sound::load(const String& filename, Float decodeMaxDuration)
{
    if (m_format)
        destroy();

    InStream *is = FileManager::instance()->openInStream(filename);

    load(*is);

    // fully decode
    if (getDuration() <= decodeMaxDuration)
    {
        m_data->decode();
        m_size = m_data->getSize();
    }
}

// load from an opened file
void Sound::load(InStream &is, Float decodeMaxDuration)
{
	if (m_format)
		destroy();

    UInt8 buf[6] = { 0 };

    is.read(buf, 6);
    is.seek(-6);

	if (buf[0] == 'R' && buf[1] == 'I' && buf[2] == 'F' && buf[3] == 'F')
        loadWav(is); // WAV
	else if (buf[0] == 'O' && buf[1] == 'g' && buf[2] == 'g' && buf[3] == 'S')
        loadOgg(is); // OGG
	else if ((buf[0] == 0xFF && buf[1] == 0xB0) || (buf[0] == 'I' && buf[1] == 'D' && buf[2] == '3' && buf[3] == 0x03))
        loadMp3(is); // MP3
	else // unknown
	{
		m_dataType = AUDIO_SOUND;
        O3D_ERROR(E_InvalidFormat("Unsupported sound format"));
	}
}

// Create a streamer
SndStream* Sound::createStreamer()
{
	switch (m_dataType)
	{
		case AUDIO_SOUND_RAW:
			return new SndRawStreamer(*static_cast<SndRaw*>(m_data.get()));

		case AUDIO_SOUND_WAV:
			return new WavStreamer(*static_cast<Wav*>(m_data.get()));

		case AUDIO_SOUND_OGG:
			return new OggStreamer(*static_cast<OggVorbis*>(m_data.get()));

		case AUDIO_SOUND_MP3:
			//return new O3DMp3Streamer(*static_cast<O3DMp3*>(m_data.get()));
            return nullptr;

		default:
            return nullptr;
	}
}

// destroy the sound
void Sound::destroy()
{
	// sound data
	if (m_data.get())
        m_data = nullptr;

	m_sampling = 0;
	m_format = 0;
	m_size = 0;
	m_dataType = AUDIO_SOUND;
}

