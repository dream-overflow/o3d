/**
 * @file sound.h
 * @brief Sound data container and loader.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-04-15
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SOUND_H
#define _O3D_SOUND_H

#include "o3d/core/file.h"
#include "o3d/core/smartpointer.h"
#include "o3d/core/memorydbg.h"

#include "al.h"
#include "alc.h"
#include "audiotype.h"

namespace o3d {

class SndStream;
class SndFormat;
class SndBufferManager;

//---------------------------------------------------------------------------------------
//! @class Sound
//-------------------------------------------------------------------------------------
//! Sound file loading and streaming. Supported formats are :
//! - WAV
//! - OGG
//! - MP3 (is not natively supported by O3D, you must develop your own reader).
//---------------------------------------------------------------------------------------
class O3D_API Sound
{
public:

	//! Audio format.
	enum AudioFormat
	{
		MONO_8 = AL_FORMAT_MONO8,
		MONO_16 = AL_FORMAT_MONO16,
		STEREO_8 = AL_FORMAT_STEREO8,
		STEREO_16 = AL_FORMAT_STEREO16
	};

	//! Default constructor
	Sound();

	//! Copy constructor. Make a deep copy.
	Sound(const Sound &dup);

	//! Destructor
	virtual ~Sound();

	//! Assign. Make a deep copy.
	Sound& operator=(const Sound &dup);

	//! Load from a RAW buffer data.
	//! @param filename Name of the sound (not necessary).
	//! @param format OpenAL sound data format.
	//! @param data Sound data.
	//! @param size Size of the data buffer in bytes.
	//! @param sampling Sampling rate in Hz.
    void load(
		const String &filename,
		Int32 format,
		const UInt8 *data,
		UInt32 size,
		UInt32 sampling);

	//! Load from a filename.
	//! @param filename Sound file name to read from.
	//! @param decodeMaxDuration If the sound duration is greater than this value the
    //! the sound stream stay in memory for streaming.
    void load(const String &filename, Float decodeMaxDuration = 2.0f);

	//! Load from an opened file.
    //! @param file A valid stream to read from.
	//! @param decodeMaxDuration If the sound duration is greater than this value the
    //! the sound stream stay in memory for streaming. You can give a memory stream to
    //! avoid disk access or to use a stream from a mounted Zip.
    void load(InStream &is, Float decodeMaxDuration = 2.0f);

	//! Destroy the sound
	void destroy();

	//! Is the sound is empty.
	inline Bool isEmpty() const { return m_dataType==AUDIO_SOUND?True:False; }

	//! Is the sound is valid.
	inline Bool isValid() const { return m_dataType!=AUDIO_SOUND?True:False; }

	//! Create a streamer.
	//! @param sndBufferManager Pointer on the scene sound buffer manager.
	//! @param keyName Unique name of the new stream object.
	SndStream* createStreamer();

	//! Get the sound duration in seconds.
	Float getDuration() const;

	//! Get type.
	inline UInt32 getType() const { return m_dataType; }

	//! Get the sound format.
	inline Int32 getFormat() const { return m_format; }

	//! Get the number of channels (1 or 2).
	UInt32 getNumChannels() const;

	//! get the sound format size (8 or 16bits)
	UInt32 getBitsPerSample() const;

	//! Get the buffer size (in bytes).
	inline UInt32 getSize() const { return m_size; }

	//! Get the sampling rate.
	inline UInt32 getSamplingRate() const { return m_sampling; }

	//! Get the data buffer and size.
	const UInt8* getData() const;

protected:

	Int32 m_format;        //!< sound format
	UInt32 m_size;         //!< size in bytes
	UInt32 m_sampling;     //!< sound sampling

	SmartPtr<SndFormat> m_data;  //!< sharable sound format

	UInt32 m_dataType;     //!< WAV, OGG, MP3, RAW...

    void loadWav(InStream &is);
    void loadOgg(InStream &is);
    void loadMp3(InStream &is);
};

} // namespace o3d

#endif // _O3D_SOUND_H

