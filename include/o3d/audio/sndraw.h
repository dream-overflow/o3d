/**
 * @file sndraw.h
 * @brief Raw sound format reader and streamer.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2009-01-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SNDRAW_H
#define _O3D_SNDRAW_H

#include "sndformat.h"
#include "sndstream.h"

#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class SndRaw
//-------------------------------------------------------------------------------------
//! Raw sound data.
//---------------------------------------------------------------------------------------
class O3D_API SndRaw : public SndFormat
{
	friend class SndRawStreamer;

public:

	//! Default constructor.
	//! @param format OpenAL sound format.
	//! @param samplingRate Sampling rate in Hertz.
	SndRaw(UInt32 format, UInt32 samplingRate);

	//! Destructor.
	virtual ~SndRaw();

    virtual void prepare(InStream &is);

	//! Define data from a valid buffer.
    void prepare(const UInt8 *data, UInt32 size);

	//! Load from a valid filename.
	//! The file stay open until Destroy or Decode method is called.
    virtual void prepare(const String &filename);

	//! Destroy the data.
	virtual void destroy();

	//! Get the sound duration in seconds. Valid is SetFormat was previously called.
	virtual Float getDuration() const;

	//! Decode the sound data.
	//! @return True if successfully decoded.
	//! @note Take care when using this method because it can take a lot of time and memory
	//! depending of the size of the sound data and the compression method.
	virtual Bool decode();

protected:

    InStream *m_is;     //!< Streamed (can be null).
	UInt32 m_dataSize;  //!< Size of the sound data in bytes.
	Float m_duration;   //!< Duration in seconds.
};


//---------------------------------------------------------------------------------------
//! @class SndRawStreamer
//-------------------------------------------------------------------------------------
//! Stream a RAW sound.
//---------------------------------------------------------------------------------------
class O3D_API SndRawStreamer : public SndStream
{
public:

	//! Default constructor. Take a valid O3DSndRaw object.
	//! @param manager If managed it must be valid.
	SndRawStreamer(SndRaw &raw);

	//! Destructor.
	virtual ~SndRawStreamer();

	virtual void destroy();

	virtual Bool isDefined() const;
	virtual Bool isValid() const;
	virtual UInt32 getSize() const;
	virtual UInt32 getFormat() const;
	virtual UInt32 getSamplingRate() const;
	virtual Float getDuration() const;

	virtual const UInt8* getStreamChunk(UInt32 seek, UInt32 &size, Bool &finished);
	virtual UInt32 getStreamPos() const;
	virtual void rewind();
	virtual Bool isFinished() const;

protected:

	SmartPtr<SndRaw> m_source;    //!< Raw source data.

	UInt8 *m_stream;       //!< The stream data if opened file.
	UInt32 m_position;     //!< Position in the stream.
};

} // namespace o3d

#endif // _O3D_SNDRAW_H

