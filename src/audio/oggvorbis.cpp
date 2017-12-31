/**
 * @file oggvorbis.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/audio/precompiled.h"

#include "o3d/audio/oggvorbis.h"
#include "o3d/core/debug.h"
#include "o3d/core/datainstream.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/templatemanager.h"

#include "o3d/audio/al.h"
#include "o3d/audio/alc.h"

using namespace o3d;

// buffer of max
#define MAX_OGG_FILE_SIZE 1024*1024*1024

// vorbis callback
static size_t VorbisRead(void *ptr, size_t byteSize, size_t sizeToRead, void *datasource)
{
    InStream* is = (InStream*)datasource;
	UInt32 len = 0;

    len = is->reader((UInt8*)ptr, (UInt32)byteSize, (UInt32)sizeToRead);

	return len;
}

static Int32 VorbisSeek(void *datasource, ogg_int64_t offset, Int32 whence)
{
    InStream* is = (InStream*)datasource;

    switch (whence) {
		case SEEK_SET:
            is->reset((UInt64)offset);
			break;
		case SEEK_CUR:
            is->seek((Int64)offset);
			break;
		case SEEK_END:
            is->end((Int64)offset);
			break;
	}
	return 0;
}

static Int32 VorbisClose(void *datasource)
{
	return 1;
}

static long VorbisTell(void *datasource)
{
    InStream* file = (InStream*)datasource;
    return file->getPosition();
}

// Default constructor
OggVorbis::OggVorbis() :
	SndFormat(),
    m_is(nullptr),
	m_position(0),
	m_duration(0.f)
{
}

// destructor
OggVorbis::~OggVorbis()
{
    destroy();
}

void OggVorbis::prepare(InStream &is)
{
    if (m_is)
        destroy();

    vorbis_info* vorbisInfo;         // some formatting data
    //vorbis_comment* vorbisComment;   // user comments
    ov_callbacks vorbisCallbacks;

    // vorbis callbacks
    vorbisCallbacks.read_func  = VorbisRead;
    vorbisCallbacks.close_func = VorbisClose;
    vorbisCallbacks.seek_func  = VorbisSeek;
    vorbisCallbacks.tell_func  = VorbisTell;

    // open the memory file
    if(ov_open_callbacks(&is, &m_oggStream, nullptr, 0, vorbisCallbacks) != 0)
        O3D_ERROR(E_InvalidAllocation("Invalid OGG callbacks"));

    vorbisInfo = ov_info(&m_oggStream, -1);
    //vorbisComment = ov_comment(&m_oggStream, -1);

    if (vorbisInfo->channels == 1)
        m_format = AL_FORMAT_MONO16;
    else
        m_format = AL_FORMAT_STEREO16;

    m_samplingRate = vorbisInfo->rate;
    m_duration = (Float)ov_time_total(&m_oggStream, -1);

    m_is = &is;
}

// load an OGG file
void OggVorbis::prepare(const String &filename)
{
    if (m_is)
        destroy();

    InStream *is = FileManager::instance()->openInStream(filename);

	vorbis_info* vorbisInfo;         // some formatting data
	//vorbis_comment* vorbisComment;   // user comments
	ov_callbacks vorbisCallbacks;

	// vorbis callbacks
	vorbisCallbacks.read_func  = VorbisRead;
	vorbisCallbacks.close_func = VorbisClose;
	vorbisCallbacks.seek_func  = VorbisSeek;
	vorbisCallbacks.tell_func  = VorbisTell;

	// open the memory file
    if(ov_open_callbacks(m_is, &m_oggStream, nullptr, 0, vorbisCallbacks) != 0)
	{
        deletePtr(is);
        O3D_ERROR(E_InvalidAllocation("Invalid OGG callbacks"));
	}

	vorbisInfo = ov_info(&m_oggStream, -1);
	//vorbisComment = ov_comment(&m_oggStream, -1);

	if (vorbisInfo->channels == 1)
		m_format = AL_FORMAT_MONO16;
	else
		m_format = AL_FORMAT_STEREO16;

	m_samplingRate = vorbisInfo->rate;
	m_duration = (Float)ov_time_total(&m_oggStream, -1);
}

// destroy all content
void OggVorbis::destroy()
{
    if (m_is)
	{
		ov_clear(&m_oggStream);
        deletePtr(m_is);
	}

	m_samplingRate = m_format = 0;
	m_duration = 0.f;

	m_decodedData.destroy();
}

Bool OggVorbis::decode()
{
    if (m_is)
	{
		UInt8 *data = new UInt8[16384];

		Int32 section;
		Int32 result = 0;

		ov_raw_seek(&m_oggStream, 0);

		do
		{
			result = ov_read(&m_oggStream, (char*)data, 16384, 0, 2, 1, &section);
			m_decodedData.pushArray(data, result);
		} while (result > 0);

		// invalid
		if (result < 0)
			O3D_ERROR(E_InvalidResult("Streaming result<0"));

		ov_clear(&m_oggStream);

        deletePtr(m_is);
		deletePtr(data);

		return (m_decodedData.getSize() > 0);
	}
	else
		return (m_decodedData.getSize() > 0);
}

Float OggVorbis::getDuration() const
{
	return m_duration;
}

//---------------------------------------------------------------------------------------
// O3DOggStreamer
//---------------------------------------------------------------------------------------

// Default constructor. Take a valid O3DSndRaw object.
OggStreamer::OggStreamer(OggVorbis &ogg) :
	SndStream(),
		m_source(&ogg),
        m_stream(nullptr),
		m_position(0)
{
	O3D_ASSERT(m_source.isValid());

	// create a stream buffer if the source is a file
    if (m_source->m_is)
		m_stream = new UInt8[SndStream::BUFFER_SIZE];
}

// Destructor.
OggStreamer::~OggStreamer()
{
	destroy();
}

// Destructor.
void OggStreamer::destroy()
{
	deleteArray(m_stream);
}

Bool OggStreamer::isDefined() const
{
	return (isValid() && m_source->isValid());
}

Bool OggStreamer::isValid() const
{
	return (m_source.isValid());
}

UInt32 OggStreamer::getSize() const
{
	return 0;
}

UInt32 OggStreamer::getFormat() const
{
	return (m_source ? m_source->getFormat() : 0);
}

UInt32 OggStreamer::getSamplingRate() const
{
	return (m_source ? m_source->getSamplingRate() : 0);
}

Float OggStreamer::getDuration() const
{
	return (m_source ? m_source->getDuration() : 0);
}

const UInt8* OggStreamer::getStreamChunk(UInt32 seek, UInt32 &size, Bool &finished)
{
	// stream from a file
    if (m_source->m_is)
	{
		Int32 section;
		Int32 result;

		// out of memory
		if (seek >= ov_raw_total(&m_source->m_oggStream, -1))
		{
			size = 0;
			finished = True;
            return nullptr;
		}

		size = 0;

		if (seek != ov_raw_tell(&m_source->m_oggStream))
		{
			ov_raw_seek(&m_source->m_oggStream, seek);
			m_position = seek;
		}

		while (size < SndStream::BUFFER_SIZE)
		{
			result = ov_read(&m_source->m_oggStream, (char*)m_stream + size, SndStream::BUFFER_SIZE - size, 0, 2, 1, &section);

			// more data
			if (result > 0)
				size += result;
			// invalid
			else if(result < 0)
				O3D_ERROR(E_InvalidResult("Streaming result<0"));
			// finished
			else // if result == 0
				break;
		}

		m_position = (UInt32)ov_raw_tell(&m_source->m_oggStream);

		// finished
		if (size < SndStream::BUFFER_SIZE)
			finished = True;
		else
			finished = False;

		// no data
		if (size == 0)
            return nullptr;

		// some data
		return m_stream;
	}
	// stream from decoded data
	else if (m_source->m_decodedData.getData())
	{
		// out of memory
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
UInt32 OggStreamer::getStreamPos() const
{
	return m_position;
}

// rewind the stream (useful pour looping)
void OggStreamer::rewind()
{
	m_position = 0;
}

// is the stream is had finished to play or not
Bool OggStreamer::isFinished() const
{
    if (m_source->m_is)
		return (ov_raw_total(&m_source->m_oggStream, -1) >= m_position);
	else
		return (m_position >= (UInt32)m_source->m_decodedData.getSize());
}
