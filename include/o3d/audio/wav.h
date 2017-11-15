/**
 * @file wav.h
 * @brief Read and stream Microsoft Wav format.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-05-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_WAV_H
#define _O3D_WAV_H

#include "sndformat.h"
#include "sndstream.h"

#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Wav
//-------------------------------------------------------------------------------------
//! Open and read a Microsoft WAV file. The sound can be directly decoded, or streamed
//! using an O3DWavStreamer.
//---------------------------------------------------------------------------------------
class O3D_API Wav : public SndFormat
{
	friend class WavStreamer;

public:

	//! Default constructor.
	Wav();

	//! Destructor.
	virtual ~Wav();

    virtual void prepare(InStream &is);

    virtual void prepare(const String &filename);

	virtual void destroy();

	virtual Float getDuration() const;

	virtual Bool decode();

protected:

	Float m_duration;   //!< Sound duration in seconds.
	Int32 m_rewindPos;  //!< Rewind position in the file in byte from start.

    InStream *m_is;     //!< An opened stream in (can be null).
	UInt32 m_dataSize;  //!< Size of the sound data in bytes.
};


//---------------------------------------------------------------------------------------
//! @class WavStreamer
//-------------------------------------------------------------------------------------
//! Stream a WAV sound.
//---------------------------------------------------------------------------------------
class O3D_API WavStreamer : public SndStream
{
public:

	//! Default constructor. Take a valid O3DWav object.
	WavStreamer(Wav &wav);

	//! Destructor.
	virtual ~WavStreamer();

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

	SmartPtr<Wav> m_source;    //!< Wav source data.

	UInt8 *m_stream;       //!< The stream data if opened file.
	UInt32 m_position;     //!< Position in the stream.
};

} // namespace o3d

#endif // _O3D_WAV_H

