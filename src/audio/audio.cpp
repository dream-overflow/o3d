/**
 * @file audio.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/audio/precompiled.h"

#include "o3d/audio/audio.h"
#include "o3d/audio/sndlistener.h"
#include "o3d/audio/sndbuffermanager.h"
#include "o3d/audio/sndsource.h"
#include "o3d/audio/audiorenderer.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/classfactory.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Audio, AUDIO_AUDIO, SceneEntity)

// Default constructor
Audio::Audio(
		BaseObject *parent,
		const String &rootPath,
        AudioRenderer *renderer) :
	SceneEntity(parent),
    m_renderer(nullptr),
	m_distanceModel(DISTMODEL_INVERSE),
	m_soundSpeed(343.3f),
	m_dopplerFactor(1.0f),
    m_activeListener(nullptr),
    m_bufferManager(nullptr),
    m_sourceManager(nullptr)
{
	m_bufferManager = new SndBufferManager(this, rootPath + "/sounds");
	m_sourceManager = new SndSourceManager(this);

    setRenderer(renderer);
}

// termine OpenAL
Audio::~Audio()
{
	deletePtr(m_sourceManager);
	deletePtr(m_bufferManager);

    m_activeListener = nullptr;

    // remove the renderer
    if (m_renderer)
    {
        m_renderer->releaseIt();
        m_renderer = nullptr;
    }
}

void Audio::init()
{
    // nothing
}

void Audio::release()
{
    // nothing
}

// set/get the Doppler factor effect
void Audio::setDopplerFactor(Float f)
{
	m_dopplerFactor = f;

    if (m_renderer)
		alDopplerFactor(f);
}

// set/get the sound speed propagation
void Audio::setSoundSpeed(Float f)
{
	m_soundSpeed = f;

#ifndef AL_BYTE_OFFSET // OpenAL < 1.1
	if (m_sndRenderer)
		alDopplerVelocity(f);
#else
	//if (pSndRenderer) alSpeedOfSound(f); (only commented for testing)
    if (m_renderer)
		alDopplerVelocity(f);
#endif
}

void Audio::setDistanceModel(DistanceModel m)
{
	m_distanceModel = m;
    if (m_renderer)
	{
		switch (m)
		{
			case DISTMODEL_NONE:
				alDistanceModel(AL_NONE);
				break;
			case DISTMODEL_INVERSE:
				alDistanceModel(AL_INVERSE_DISTANCE);
				break;
			case DISTMODEL_INVERSE_CLAMP:
				alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
				break;

		#ifdef AL_BYTE_OFFSET /* OpenAL 1.1 */
			case DISTMODEL_LINEAR:
				alDistanceModel(AL_LINEAR_DISTANCE);
				break;
			case DISTMODEL_LINEAR_CLAMP:
				alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
				break;
			case DISTMODEL_EXPONENT:
				alDistanceModel(AL_EXPONENT_DISTANCE);
				break;
			case DISTMODEL_EXPONENT_CLAMP:
				alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED);
				break;
		#endif // OpenAL 1.1

			default:
				alDistanceModel(AL_INVERSE_DISTANCE);
				break;
		}
    }
}

void Audio::setRenderer(AudioRenderer *renderer)
{
    if (!renderer)
        O3D_ERROR(E_InvalidParameter("AudioRenderer must be valid"));

    if (m_renderer)
        m_renderer->releaseIt();

    m_renderer = renderer;

    m_renderer->useIt();
    //m_renderer->setCurrent();
}

// update some managers
void Audio::update()
{
	m_bufferManager->update();

	if (getActiveListener())
        getActiveListener()->put();
}

void Audio::focus()
{
    if (getActiveListener())
        getActiveListener()->unMute();
}

void Audio::lostFocus()
{
    if (getActiveListener())
        getActiveListener()->mute();
}

// Serialization
Bool Audio::readFromFile(InStream &is)
{
	// global openal settings
    is   >> m_distanceModel
		 >> m_dopplerFactor
		 >> m_soundSpeed;

	return True;
}

Bool Audio::writeToFile(OutStream &os)
{
	// global openal settings
    os   << m_distanceModel
		 << m_dopplerFactor
 		 << m_soundSpeed;

	return True;
}

