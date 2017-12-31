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

/**
 * @brief Open and read a Microsoft WAV file. The sound can be directly decoded,
 * or streamed using an WavStreamer.
 */
class O3D_API Wav : public SndFormat
{
	friend class WavStreamer;

public:

	//! Default constructor.
	Wav();

	//! Destructor.
	virtual ~Wav();

    virtual void prepare(InStream &is) override;

    virtual void prepare(const String &filename) override;

    virtual void destroy() override;

    virtual Float getDuration() const override;

    virtual Bool decode() override;

protected:

	Float m_duration;   //!< Sound duration in seconds.
	Int32 m_rewindPos;  //!< Rewind position in the file in byte from start.

    InStream *m_is;     //!< An opened stream in (can be null).
	UInt32 m_dataSize;  //!< Size of the sound data in bytes.
};

/**
 * @brief Stream a WAV sound.
 */
class O3D_API WavStreamer : public SndStream
{
public:

	//! Default constructor. Take a valid O3DWav object.
	WavStreamer(Wav &wav);

	//! Destructor.
	virtual ~WavStreamer();

    virtual void destroy() override;

    virtual Bool isDefined() const override;
    virtual Bool isValid() const override;
    virtual UInt32 getSize() const override;
    virtual UInt32 getFormat() const override;
    virtual UInt32 getSamplingRate() const override;
    virtual Float getDuration() const override;

    virtual const UInt8* getStreamChunk(UInt32 seek, UInt32 &size, Bool &finished) override;
    virtual UInt32 getStreamPos() const override;
    virtual void rewind() override;
    virtual Bool isFinished() const override;

protected:

	SmartPtr<Wav> m_source;    //!< Wav source data.

	UInt8 *m_stream;       //!< The stream data if opened file.
	UInt32 m_position;     //!< Position in the stream.
};

} // namespace o3d

#endif // _O3D_WAV_H
