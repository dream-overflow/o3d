/**
 * @file sndbuffer.h
 * @brief An hardware or software streamed or not sound buffer
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-04-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SNDBUFFER_H
#define _O3D_SNDBUFFER_H

#include "o3d/engine/scene/sceneentity.h"
#include "o3d/core/taskmanager.h"
#include "o3d/core/stringlist.h"
#include "sound.h"

#include "o3d/core/smartpointer.h"
#include "o3d/core/garbagemanager.h"
#include "o3d/core/idmanager.h"
#include "o3d/core/memorydbg.h"

#include "o3d/audio/albuffer.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class SndBuffer
//-------------------------------------------------------------------------------------
//! A sound buffer represent an hardware (or software) buffer that contain a sound
//! sampler with a certain sampling rate and one or two channels. Only single channel
//! buffer are supported by 3d sources. Stereo channels source are mainly use for global
//! sound like ambient music. It can be streamed or not.
//---------------------------------------------------------------------------------------
class O3D_API SndBuffer : public SceneResource
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(SndBuffer)

	//! Construct using a sound filename.
	//! @param parent Owner.
    //! @param decodeMaxDuration If the sound duration is greater than this value the
    //! the sound file is reopen from streaming, otherwise it is fully decoded.
	SndBuffer(
		BaseObject *parent,
		Float decodeMaxDuration = 2.0f);

	//! Construct using a generated and valid sound.
	//! @param parent Owner.
	//! @param filename Sound filename to use for the call to Load. The sound name
	//! must be valid. That's mean an existing filename or a name enclosed into < and >.
	//! @param decodeMaxDuration If the sound duration is greater than this value the
	//! the sound file is reopen from streaming, otherwise it is fully decoded.
	SndBuffer(
		BaseObject *parent,
		const Sound &sound,
		Float decodeMaxDuration = 2.0f);

	//! Virtual destructor.
	virtual ~SndBuffer();

	//! Is the buffer operational.
	inline Bool isValid() const { return m_bufferObject && m_bufferObject->isValid(); }

	//! Is the buffer streamed.
	inline Bool isStream() const { return m_bufferObject && m_bufferObject->isStream(); }

	//! Get the front or single buffer id.
	UInt32 getFrontBufferId() const;

	//! Get the back buffer id.
	UInt32 getBackBufferId() const;


	//-----------------------------------------------------------------------------------
	// Loading/unloading/reloading of the sound
	//-----------------------------------------------------------------------------------

	//! Create the sound OpenAL buffer. This make a single buffer sound (non streamed).
	//! A stream is automatically initialized by its source object.
	//! @param unloadSound If true delete the sound from memory, otherwise keep it.
	//! If the sound is streamed the stream object of the sound is not removed.
	Bool create(Bool unloadSound = True);

	//! Delete the buffer data and release the sound.
	void destroy();

	//! Unload the sound data, but keep it in OpenAL.
	void unloadSound();

	//! Update the buffer: only for streamed buffer (with a back-buffer).
	//! @param sourceId Identifier of the target source.
	//! @param loop If TRUE the streaming is looped.
	//! @param position Will set with the next start position.
	//! @return TRUE while the stream end is not reached (always TRUE if loop is TRUE).
	Bool updateStream(UInt32 sourceId, Bool loop, UInt32 &position);

	//! Prepare the stream to be played.
	//! @param sourceID Identifier of the target source.
	//! @param position Will set with the next start position.
	Bool prepareStream(UInt32 sourceId, UInt32 &position);

	//! Get the sound buffer sampling rate (8,11,22,44kHz).
	UInt32 getSamplingRate() const;

	//! Get the size of a channel in bits (8 or 16bits).
	UInt32 getChannelFormat() const;

	//! Get bit per sample (8,16).
	UInt32 getBitsPerSample() const;

	//! Get the number of channels (1 or 2).
	UInt32 getNumChannels() const;

	//! Get the sound object (read only).
	inline const Sound& getSound() const { return m_sound; }
	//! Get the sound object.
	inline Sound& getSound() { return m_sound; }

    //! Set the sound object.
    inline void setSound(Sound &sound) { m_sound = sound; }

	//! Get the decode max duration time.
	inline Float getDecodeMaxDuration() const { return m_decodeMaxDuration; }

	// Serialization
	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

public:

	//! Called when the sound buffer is created.
    Signal<SndBuffer*> onSndBufferValid{this};

protected:

	SndBufferObject *m_bufferObject;

	UInt32 m_frontBuffer;     //!< Front-buffer if streaming, otherwise single buffer.
	UInt32 m_backBuffer;      //!< Back-buffer if streaming.

	Int32 m_streamPos;        //!< Position in the streamer (only if streamed).

	Sound m_sound;             //!< Sound object (can be empty).

	Float m_decodeMaxDuration;   //!< For serialization only, maximal time to entirely store a buffer.
};

//---------------------------------------------------------------------------------------
//! @class SndBufferTask
//-------------------------------------------------------------------------------------
//! Task responsible of the loading of a sound buffer.
//---------------------------------------------------------------------------------------
class O3D_API SndBufferTask : public Task
{
public:

	//! Default constructor.
	//! @param sndBuffer Sound buffer target.
	//! @param filename Filename of the sound to load.
	//! @param decodeMaxDuration If the sound duration is greater than this value the
	//! the sound file is reopen from streaming, otherwise it is fully decoded.
	SndBufferTask(
			SndBuffer *sndBuffer,
			const String &filename,
			Float decodeMaxDuration);

	virtual Bool execute();

	virtual Bool finalize();

private:

	SndBuffer *m_sndBuffer;  //!< Sound buffer to load with the sound.

	String m_filename;       //!< Absolute filename.
	Sound m_sound;           //!< Sound container.

	Float m_decodeMaxDuration;
};

typedef std::list<SndBuffer*> T_BufferList;
typedef T_BufferList::iterator IT_BufferList;
typedef T_BufferList::const_iterator CIT_BufferList;

typedef std::list<SmartObject<SndBuffer> > T_SmartBufferList;
typedef T_SmartBufferList::iterator IT_SmartBufferList;
typedef T_SmartBufferList::const_iterator CIT_SmartBufferList;

} // namespace o3d

#endif // _O3D_SNDBUFFER_H

