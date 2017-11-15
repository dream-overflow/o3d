/**
 * @file sndformat.h
 * @brief Sound format interface.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2009-01-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SNDFORMAT_H
#define _O3D_SNDFORMAT_H

#include "o3d/core/smartcounter.h"
#include "o3d/core/templatearray.h"
#include "o3d/core/file.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class SndFormat
//-------------------------------------------------------------------------------------
//! Sound format interface.
//---------------------------------------------------------------------------------------
class O3D_API SndFormat : public SmartCounter<SndFormat>
{
public:

	//! Default constructor.
	SndFormat() :
		SmartCounter<SndFormat>(),
		m_format(0),
		m_samplingRate(0)
	{
	}

    /**
     * @brief prepare Load sound information from a stream and manage it.
     * @param is Stream is deleted at destructor.
     */
    virtual void prepare(InStream &is) = 0;

    /**
     * @brief prepare Load sound informations from a file name.
     * @param filename Resource filename
     * @return True if success.
     */
    virtual void prepare(const String& filename) = 0;

	//! Destroy the data.
	virtual void destroy() = 0;

    //! Get the sound duration in seconds. This value is available after prepare.
	virtual Float getDuration() const = 0;

    //! Decode the sound data. And delete the stream.
	//! @return True if successfully decoded.
	//! @note Take care when using this method because it can take a lot of time
	//! depending of the size of the sound data and the compression method.
	virtual Bool decode() = 0;

	//! Get the decoded data array (null if not decoded @see Decode).
	inline const UInt8 *getData() const { return m_decodedData.getData(); }

	//! Get the decoded data size (0 if not decoded @see Decoded).
	inline UInt32 getSize() const { return m_decodedData.getSize(); }

	//! Get OpenAL sound format.
	inline Int32 getFormat() const { return m_format; }

	//! Get sampling rate in Hz.
	inline UInt32 getSamplingRate() const { return m_samplingRate; }

	//! Is the sound load method processed.
	inline Bool isValid() const { return m_format != 0; }

protected:

	ArrayUInt8 m_decodedData;  //!< Decoded data array.

	UInt32 m_format;          //!< OpenAL sound format.
	UInt32 m_samplingRate;    //!< Sampling rate in Hz.
};

} // namespace o3d

#endif // _O3D_SNDFORMAT_H

