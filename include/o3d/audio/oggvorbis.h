/**
 * @file oggvorbis.h
 * @brief OggVorbis reader and streamer.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-10-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_OGGVORBIS_H
#define _O3D_OGGVORBIS_H

#include "sndformat.h"
#include "sndstream.h"

namespace o3d {

#ifdef __GNUC__
	#include <ogg/ogg.h>
	#include <vorbis/codec.h>
	#include <vorbis/vorbisenc.h>
	#include <vorbis/vorbisfile.h>
#else
	#include <ogg/ogg.h>
	#include <vorbis/codec.h>
	#include <vorbis/vorbisenc.h>
	#include <vorbis/vorbisfile.h>
#endif

#include "o3d/core/memorydbg.h"

//---------------------------------------------------------------------------------------
//! @class OggVorbis
//-------------------------------------------------------------------------------------
//! Open an Ogg Vorbis file and decode it if necessary.
//---------------------------------------------------------------------------------------
class O3D_API OggVorbis : public SndFormat
{
	friend class OggStreamer;

public:

	//! Default constructor.
	OggVorbis();

	//! Destructor.
	virtual ~OggVorbis();

    virtual void prepare(InStream &is);

    virtual void prepare(const String &filename);

	virtual void destroy();

	virtual Float getDuration() const;

	virtual Bool decode();

protected:

	OggVorbis_File m_oggStream;
    InStream *m_is;

	UInt32 m_position;
	Float m_duration;
};


//---------------------------------------------------------------------------------------
//! @class OggStreamer
//-------------------------------------------------------------------------------------
//! Stream a Ogg-Vorbis sound.
//---------------------------------------------------------------------------------------
class O3D_API OggStreamer : public SndStream
{
public:

	//! Default constructor. Take a valid O3DOggVorbis object.
	OggStreamer(OggVorbis &ogg);

	//! Destructor.
	virtual ~OggStreamer();

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

	SmartPtr<OggVorbis> m_source;    //!< Ogg source data.

	UInt8 *m_stream;
	UInt32 m_position;
};

} // namespace o3d

#endif // _O3D_OGGVORBIS_H

