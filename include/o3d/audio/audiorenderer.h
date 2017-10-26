/**
 * @file audiorenderer.h
 * @brief Audio rendering device. Use of the OpenAL capabilities.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2003-04-12
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_AUDIORENDERER_H
#define _O3D_AUDIORENDERER_H

#include "o3d/core/string.h"
#include "o3d/core/memorydbg.h"

#include "al.h"
#include "alc.h"

// EAX is only supported on WIN32 architecture with Visual C++
#ifdef O3D_EFX
	#include "o3d/core/architecture.h"
	#include "eax.h"
#endif // O3D_EFX

namespace o3d {

#ifdef O3D_EFX
	extern EAXSet alEAXSet;
	extern EAXGet alEAXGet;
#endif // O3D_EFX

extern void (*alReverbScale)(ALuint sid, ALfloat param);
extern void (*alReverbDelay)(ALuint sid, ALfloat param);

//---------------------------------------------------------------------------------------
//! @class AudioRenderer
//-------------------------------------------------------------------------------------
//! Sound rendering device. Use of the OpenAL capabilities.
//---------------------------------------------------------------------------------------
class O3D_API AudioRenderer
{
public:

	//! Default constructor.
	//! @param device Device name, or "AutoDetect" to use the first working device, starting
	//!        by trying with the default device.
	//! @param useEAX Enable the EAX support if available.
	AudioRenderer(const String &device = "AutoDetect", Bool useEAX = True);

	//! Destructor.
	virtual ~AudioRenderer();

	//! Create the OpenAL sound renderer.
	//! @param device Device name, or "AutoDetect" to use the first working device, starting
	//!        by trying with the default device.
	//! @param useEAX Enable the EAX support if available.
	Bool create(const String &device = "AutoDetect", Bool useEAX = True);

	//! Delete the sound renderer.
	void destroy();

	//! Get the default device name.
	String getDefaultDevice() const { return m_devicesList[m_defaultDevice]; }

	//! Get number of detected devices.
	UInt32 getNumDevice() const { return m_numDevices; }

	//! Get the name of a device given its index.
	String getDeviceName(UInt32 num) const
	{
		if (num < 12)
			return m_devicesList[num];
		else
			return String("Invalid device value");
	}

	//! Suspend the context (on alt-tab for example).
	void suspendContext();
	//! Re-acquire the context (on alt-tab for example).
	void processContext();

    //! Is LOKI REVERB EXT supported.
	Bool isReverbSupported() const { return m_reverb; }

	//! force to disable EAX extension (need to recreated source)
	void forceDisableEAX();
	//! enable EAX after a force disable or a non enable at creation
	Bool enableEAX();

    //
    // Getters
    //

	//! Is EAX support enabled.
	inline Bool isEAX() const { return m_isEAX; }
	//! Get the OpenAL device.
	inline ALCdevice* getDevice() { return m_alDevice; }
	//! Get the OpenAL context.
	inline ALCcontext* getProcessContext() { return m_alContext; }

	//! Get the sound card vendor name.
	String getVendorName() const;
	//! Get the sound card version number.
	String getVersionNum() const;
	//! Get the renderer name.
	String getRendererName() const;
	//! Get the extensions list string.
	String getExtensionString() const;
	//! Get the devices and drivers strings informations
	String getSoundCardString() const;

	//! Check for an error from the OpenAL device.
	//! The error string can be obtain using GetDeviceError().
	Bool isDeviceError();

	//! Check for an error from OpenAL.
	//! The error string can be obtain using GetError().
	Bool isError();

	//! Get an error string from the OpenAL device.
	//! @return An empty string if no error.
	String getDeviceError() const;

	//! Get an error string from OpenAL.
	//! @return An empty string if no error.
	String getError() const;

	//! Enable sound usage (for serialization only).
	inline void enableSound() { m_isSound = True; }
	//! Disable sound usage (for serialization only).
	inline void disableSound() { m_isSound = False; }

    //! Is the renderer exists.
    inline Bool isValid() const { return m_state; }

    //! Increment the renderer reference usage.
    void useIt();

    //! Decrement the renderer reference usage.
    void releaseIt();

    //! Check if the renderer is used by one or many Scene.
    inline Bool isUsed() const { return (m_refCount > 0); }

protected:

	Int32 m_alErrno;  //!< Last OpenAL error code.
	Int32 m_alcErrno; //!< Last OpenaL device error code.

    Int32 m_refCount; //!< Reference counter.

	Bool m_reverb;    //!< LOKI Reverb support is present

	Bool m_isSound;	  //!< sound enabled
	Bool m_useEAX;	  //!< is use EAX (only for serialization)

	Bool m_state;	  //!< is render initialized
	Bool m_isEAX;     //!< is EAX enable

	String m_deviceName;  //!< OpenAL device name
	String m_EAXVersion;  //!< EAX used version

	ALCdevice *m_alDevice;    //!< OpenAL device
	ALCcontext *m_alContext;  //!< OpenAL device context

	//! Is EAX extension supported.
	Bool getEAXExtension();

	String m_devicesList[12]; //!< list of outputs devices name
	UInt32 m_numDevices;
	UInt32 m_defaultDevice;
};

} // namespace o3d

#endif // _O3D_AUDIORENDERER_H

