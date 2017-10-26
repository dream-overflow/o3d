/**
 * @file sndstream.h
 * @brief Abstract sound streamer.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-10-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SNDSTREAM_H
#define _O3D_SNDSTREAM_H

#include "albuffer.h"

#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! class SndStream
//-------------------------------------------------------------------------------------
//! Internal sound buffer streamer object.
//---------------------------------------------------------------------------------------
class O3D_API SndStream : public SndBufferObject
{
public:

	//! Buffer size for streaming. A sound with a size lesser cannot be streamed,
	//! even thought its duration in second is lesser than the request minimal duration.
	static const UInt32 BUFFER_SIZE = 4096 * 32;

	//! Default constructor.
	SndStream();

	//! Is a streamed buffer.
	virtual Bool isStream() const { return True; }

	//! Load from a valid sound object. Does mean nothing for O3DSndStream.
	virtual void load(const Sound &snd) {}

	//! destroy all content
	virtual void destroy() = 0;

	//! Get the next chunk of the stream.
	//! @param seek Position where to read the chunk.
	//! @param size Number of read bytes.
	//! @param finished True mean end of the sound stream. In this case seek is set to 0.
	virtual const UInt8* getStreamChunk(UInt32 seek, UInt32 &size, Bool &finished) = 0;

	//! Get the current position in the stream.
	virtual UInt32 getStreamPos() const = 0;

	//! Rewind the stream (useful for looping).
	virtual void rewind() = 0;

	//! Is the stream is finished to be played or not.
	virtual Bool isFinished() const = 0;

protected:

	UInt32 m_frontBuffer;
	UInt32 m_backBuffer;
};

} // namespace o3d

#endif // _O3D_SNDSTREAM_H

