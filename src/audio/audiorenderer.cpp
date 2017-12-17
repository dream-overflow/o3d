/**
 * @file audiorenderer.cpp
 * @brief Implementation of AudioRenderer.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-04-12
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/audio/precompiled.h"

#include "o3d/audio/audiorenderer.h"
#include "o3d/core/debug.h"
#include "o3d/audio/al.h"

using namespace o3d;

// EAX is only supported on WIN32 architecture with Visual C++
#ifdef O3D_EFX
	EAXSet o3d::alEAXSet;
	EAXGet o3d::alEAXGet;
#endif // O3D_EFX

void (*o3d::alReverbScale)(ALuint sid, ALfloat param) = nullptr;
void (*o3d::alReverbDelay)(ALuint sid, ALfloat param) = nullptr;

// Constructor
AudioRenderer::AudioRenderer(const String& device, Bool useEAX) :
		m_alErrno(AL_NO_ERROR),
		m_alcErrno(ALC_NO_ERROR)
{
	m_state = False;
	m_isEAX = False;
	m_isSound = True;
	m_useEAX = True;
    m_refCount = 0;

	m_EAXVersion = "EAX2.0";
	m_deviceName = device;

    m_alContext = nullptr;
    m_alDevice = nullptr;

	m_numDevices = 0;
	m_defaultDevice = 0;

	// enum output devices
	Char *defaultDevice;
	Char *devicesList;
	Char *devices[12];

    O3D_MESSAGE("Sound output devices :");

    if (alcIsExtensionPresent(nullptr, (ALchar*)"ALC_ENUMERATION_EXT") == AL_TRUE) {
		defaultDevice = (Char*)alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
		devicesList = (Char*)alcGetString(NULL, ALC_DEVICE_SPECIFIER);

		Int32 ndevices;
        for (ndevices = 0 ; ndevices < 12 ; ++ndevices) {
            devices[ndevices] = nullptr;
		}

        for (ndevices = 0 ; ndevices < 12 ; ++ndevices) {
			devices[ndevices] = devicesList;

            if (strcmp(devices[ndevices],defaultDevice) == 0) {
				m_defaultDevice = ndevices;
			}

			devicesList += strlen(devicesList);

            if (devicesList[0] == 0) {
                if (devicesList[1] == 0) {
					break;
                } else {
					devicesList += 1;
                }
			}
		}

        if (devices[ndevices] != nullptr) {
			m_numDevices = ++ndevices;

            for (Int32 i = 0 ; i < ndevices ; ++i) {
				m_devicesList[i] = devices[i];
                O3D_MESSAGE(String("- ") + m_devicesList[i]);
			}
		}
    }

	create(device, useEAX);
}

// Destructor
AudioRenderer::~AudioRenderer()
{
	destroy();
}

void AudioRenderer::useIt()
{
    m_refCount++;
}

void AudioRenderer::releaseIt()
{
    m_refCount--;

    if (m_refCount < 0)
        O3D_ERROR(E_IndexOutOfRange("AudioRenderer usage reference counter reach a negative value"));
}

// Get EAX extension
Bool AudioRenderer::getEAXExtension()
{
#ifdef O3D_EFX

    if (alIsExtensionPresent((ALchar*)"EAX2.0"))
	{
		alEAXSet = (EAXSet)alGetProcAddress((ALchar*)"EAXSet");
        if (alEAXSet == nullptr) return False;

		alEAXGet = (EAXGet)alGetProcAddress((ALchar*)"EAXGet");
        if (alEAXGet == nullptr) return False;
	}

	m_isEAX = True;
	return True;

#else
	return False;
#endif // O3D_EFX
}

// create the sound renderer
Bool AudioRenderer::create(const String &device, Bool useEAX)
{
	if (m_state)
		return False;

	// open the default device
	if (device.length() == 0 || device == "AutoDetect")
	{
		// try with the default device
		m_alDevice = alcOpenDevice((const ALchar*)m_devicesList[m_defaultDevice].toAscii().getData());

        if (m_alDevice == nullptr)
		{
			UInt32 idevice = 0;

			// try in order with all others detected devices
			while (((m_alDevice = alcOpenDevice((const ALchar*)m_devicesList[idevice].toAscii().getData())) == NULL) &&
					(idevice < m_numDevices))
			{
				idevice++;
			}

			if (idevice < m_numDevices)
				m_deviceName = m_devicesList[idevice];
		}
		else
		{
			m_deviceName = m_devicesList[m_defaultDevice];
		}
	}
	// open a specific device
	else
	{
		m_alDevice = alcOpenDevice((const ALchar*)device.toAscii().getData());
		m_deviceName = device;
	}

    if (m_alDevice == nullptr)
		O3D_ERROR(E_InvalidResult("Unable to found the OpenAL device " + device));

	// create OpenAL context
    m_alContext = alcCreateContext(m_alDevice, nullptr);

    if ((m_alContext == nullptr) || (alcGetError(m_alDevice) != ALC_NO_ERROR))
		O3D_ERROR(E_InvalidResult("Unable to create the OpenAL context " + device));

	// set as current
	alcMakeContextCurrent(m_alContext);

	if (alcGetError(m_alDevice) != ALC_NO_ERROR)
		O3D_ERROR(E_InvalidResult("Unable to set the new OpenAL context as current " + device));

	m_isSound = True;

	// Loki reverb extension
	alReverbScale = (void(*)(ALuint sid,ALfloat param))alGetProcAddress((ALchar*)"alReverbScale_LOKI");
	alReverbDelay = (void(*)(ALuint sid,ALfloat param))alGetProcAddress((ALchar*)"alReverbDelay_LOKI");

	m_state = True;

    O3D_MESSAGE(String("Sound card : ") + getSoundCardString() + " on " + m_deviceName + " device");

    if(alReverbScale && alReverbDelay) {
        O3D_MESSAGE("- OpenAL LOKI reverb: YES");
		m_reverb = True;
    } else {
        O3D_MESSAGE("OpenAL LOKI reverb: NO");
		m_reverb = False;
	}

	return True;
}

// force to disable EAX extension (need to recreated source)
void AudioRenderer::forceDisableEAX()
{
#ifdef O3D_EFX
	if (m_isEAX)
	{
		// reset the listener settings
		unsigned long GlobalReverb = (unsigned long)-1000;

		alEAXSet(&DSPROPSETID_EAX_ListenerProperties,DSPROPERTY_EAXLISTENER_ROOM,0,&GlobalReverb,
			sizeof(unsigned long));
		unsigned long EAXVal = EAX_ENVIRONMENT_GENERIC;
		alEAXSet(&DSPROPSETID_EAX_ListenerProperties,DSPROPERTY_EAXLISTENER_ENVIRONMENT,0,&EAXVal,
			sizeof(unsigned long));

		m_isEAX = False;
	}
#endif
}

// enable EAX after a force disable or a non enable at creation
Bool AudioRenderer::enableEAX()
{
#ifdef O3D_EFX
	if (!m_isEAX)
	{
		m_isEAX = GetEAXExtension();
		if (!m_isEAX)
			O3D_ERROR(E_InvalidPrecondition("You try to enable EAX but your device doesn't support it"));

//		unsigned long GlobalReverb = -1000;

//		alEAXSet(&DSPROPSETID_EAX_ListenerProperties,DSPROPERTY_EAXLISTENER_ROOM,0,&GlobalReverb,
//			sizeof(unsigned long));
		unsigned long EAXVal = EAX_ENVIRONMENT_GENERIC;
		alEAXSet(&DSPROPSETID_EAX_ListenerProperties,DSPROPERTY_EAXLISTENER_ENVIRONMENT,0,&EAXVal,
			sizeof(unsigned long));
	}
	return True;
#else
	return False;
#endif
}

// destroy the OpenAL context
void AudioRenderer::destroy()
{
	if (m_state)
	{
        if (m_refCount > 0)
            O3D_ERROR(E_InvalidPrecondition("Unable to destroy a referenced AudioRenderer"));

#ifdef O3D_EFX
		if (m_isEAX)
		{
			unsigned long EAXVal = EAX_ENVIRONMENT_GENERIC;
			alEAXSet(&DSPROPSETID_EAX_ListenerProperties,DSPROPERTY_EAXLISTENER_ENVIRONMENT,0,&EAXVal,
				sizeof(unsigned long));
		}
#endif

        alcMakeContextCurrent(nullptr);
		alcDestroyContext(m_alContext);
		alcCloseDevice(m_alDevice);

		m_state = False;
		m_isEAX = False;

        m_alContext = nullptr;
        m_alDevice = nullptr;

		m_alErrno = AL_NO_ERROR;
		m_alcErrno = ALC_NO_ERROR;
	}
}

// suspend the OpenAL context
void AudioRenderer::suspendContext()
{
    if (m_state && (m_alContext != nullptr))
		alcSuspendContext(m_alContext);
}

// restore the OpenAL context
void AudioRenderer::processContext()
{
    if (m_state && (m_alContext != nullptr))
		alcProcessContext(m_alContext);
}

// device information
String AudioRenderer::getVendorName() const
{
	Char *str;

	str = (Char*)alGetString(AL_VENDOR);
    return String(str);
}

String AudioRenderer::getVersionNum() const
{
	Char *str;

	str = (Char*)alGetString(AL_VERSION);
    return String(str);
}

String AudioRenderer::getRendererName() const
{
	Char *str;

	str = (Char*)alGetString(AL_RENDERER);
	return String(str);
}

String AudioRenderer::getExtensionString() const
{
	Char *str;

	str = (Char*)alGetString(AL_EXTENSIONS);
	return String(str);
}

String AudioRenderer::getSoundCardString() const
{
	String result;

	result = getVendorName() + getRendererName() + " (" + getVersionNum() + ")";
    return result;
}

Bool AudioRenderer::isDeviceError()
{
	if (m_alDevice)
	{
		m_alcErrno = alcGetError(m_alDevice);
		return (m_alcErrno != ALC_NO_ERROR);
	}
	else
		return False;
}

Bool AudioRenderer::isError()
{
	if (m_state)
	{
		m_alErrno = alGetError();
		return (m_alErrno != AL_NO_ERROR);
	}
	else
		return False;
}

String AudioRenderer::getDeviceError() const
{
	switch (m_alcErrno)
	{
		case ALC_NO_ERROR:
			return String("OpenALC: There is not current error.");

		case ALC_INVALID_DEVICE:
			return String("OpenALC: The device handle or specifier names an accessible driver/server.");

		case ALC_INVALID_CONTEXT:
			return String("OpenALC: The Context argument does not name a valid context.");

		case ALC_INVALID_ENUM:
			return String("OpenALC:A token used is not valid, or not applicable.");

		case ALC_INVALID_VALUE:
			return String("OpenALC: A value (e.g. Attribute) is not valid, or not applicable.");

		case ALC_OUT_OF_MEMORY:
			return String("OpenALC: Unable to allocate memory.");

		default:
			return String("OpenALC: Undefined error.");
	}
}

String AudioRenderer::getError() const
{
	switch (m_alErrno)
	{
		case AL_NO_ERROR:
			return String("OpenAL: There is no current error.");

		case AL_INVALID_NAME:
			return String("OpenAL: Invalid name parameter.");

		case AL_INVALID_ENUM:
			return String("OpenAL: Invalid parameter.");

		case AL_INVALID_VALUE:
			return String("OpenAL: Invalid enum parameter value.");

		case AL_INVALID_OPERATION:
			return String("OpenAL: Illegal call.");

		case AL_OUT_OF_MEMORY:
			return String("OpenAL: Unable to allocate memory.");

		default:
			return String("OpenAL: Undefined error.");
	}
}

