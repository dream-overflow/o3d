/**
 * @file albuffer.h
 * @brief OpenAL internal sound buffer object and creation.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-12-31
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ALBUFFER_H
#define _O3D_ALBUFFER_H

#include "sound.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class SndStream;

//---------------------------------------------------------------------------------------
//! @class SndBufferObject
//-------------------------------------------------------------------------------------
//! Abstract internal sound buffer object.
//---------------------------------------------------------------------------------------
class O3D_API SndBufferObject : NonCopyable<>
{
public:

	//! Default constructor.
	SndBufferObject();

	//! Virtual destructor.
	virtual ~SndBufferObject();

	//! Get the OpenAL sound buffer type.
	inline UInt32 getType() const { return m_type; }

	//! Load from a valid sound object.
	virtual void load(const Sound &snd) = 0;

	//! Is the object defined.
	virtual Bool isDefined() const = 0;

	//! Is a streamed buffer.
	virtual Bool isStream() const = 0;

	//! Is the sound buffer is valid (for usage).
	virtual Bool isValid() const = 0;

	//! Get the size of the buffer (in bytes).
	virtual UInt32 getSize() const = 0;

	//! Get the sound OpenAL format.
	virtual UInt32 getFormat() const = 0;

	//! Get the sound sampling rate frequency in Hz.
	virtual UInt32 getSamplingRate() const = 0;

	//! Get the buffer duration in seconds.
	virtual Float getDuration() const = 0;

protected:

	UInt32 m_type;
};

//---------------------------------------------------------------------------------------
//! @class ALBuffer
//-------------------------------------------------------------------------------------
//! Store and load a sound in OpenAL memory in a single buffer.
//---------------------------------------------------------------------------------------
class O3D_API ALBuffer : public SndBufferObject
{
public:

	//! Default constructor.
	ALBuffer();

	//! Virtual destructor.
	virtual ~ALBuffer();

	//! Load from a valid sound object.
	virtual void load(const Sound &snd);

	//! Define the sound parameters.
	//! @param format OpenAL buffer format.
	//! @param size Buffer length in bytes.
	//! @param samplingRate Buffer sampling rate frequency in Hz.
	//! @return True if the buffer is created or recreated.
	Bool define(UInt32 format, UInt32 size, UInt32 samplingRate);

	//! Is the sound data defined.
	virtual Bool isDefined() const { return m_format != 0; }

	//! Is a streamed buffer.
	virtual Bool isStream() const { return False; }

	//! Is the sound buffer is valid.
	virtual Bool isValid() const { return m_bufferId != O3D_UNDEFINED; }

	//! Get the OpenAL buffer id.
	inline UInt32 getBufferId() const { return m_bufferId; }

	//! Get the size of the buffer (in bytes).
	virtual UInt32 getSize() const { return m_size; }

	//! Get the sound OpenAL format.
	virtual UInt32 getFormat() const { return m_format; }

	//! Get the sound sampling rate frequency in Hz.
	virtual UInt32 getSamplingRate() const { return m_samplingRate; }

	//! Get the buffer duration in seconds.
	virtual Float getDuration() const { return m_duration; }

	//! Load a sound to OpenAL.
	static UInt32 loadSound(UInt32 id, const Sound &snd);

protected:

	UInt32 m_bufferId;     //!< OpenAL buffer identifier.

	Float m_duration;      //!< Buffer duration in seconds.
	UInt32 m_size;         //!< Size of the buffer in bytes.
	UInt32 m_format;       //!< OpenAL buffer format.
	UInt32 m_samplingRate; //!< Buffer sampling rate (in Hz).
};

} // namespace o3d

#endif // _O3D_ALBUFFER_H

