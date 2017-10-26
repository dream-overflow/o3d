/**
 * @file sndsource.cpp
 * @brief Implementation of SndSource.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2003-04-13
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/audio/precompiled.h"

#include "o3d/core/architecture.h"
#include "o3d/engine/gl.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/hierarchy/hierarchytree.h"
#include "o3d/core/classfactory.h"
#include "o3d/audio/sndsource.h"
#include "o3d/audio/audio.h"
#include "o3d/audio/audiorenderer.h"
#include "o3d/audio/sndbuffermanager.h"
#include "o3d/engine/context.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/object/primitivemanager.h"

#include <algorithm>

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(SndSource, AUDIO_SND_SOURCE, SceneObject)

// Default constructor
SndSource::SndSource(BaseObject *parent) :
	SceneObject(parent),
		m_sourceId(0),
		m_pitch(1),
		m_gain(1),
		m_refDistance(1),
		m_rollOff(1),
		m_maxDistance(1000),
		m_minGain(0),
		m_maxGain(1),
		m_reverbDelay(0),
		m_reverbScale(0),
		m_looping(False),
		m_playing(False),
		m_relative(False),
		m_position(0),
		m_autoPlay(False),
		m_autoRelease(False),
		m_streaming(False),
		m_lastTime(0),
		m_fadeScale(0),
		m_fadeInOut(0),
		m_modifiedEAX(False)
{
	setDrawable(True);
	setUpdatable(True);

	setDefaultEAX();

	// Register the source
	if (getScene())
        ((Audio*)getScene()->getAudio())->getSourceManager()->registerSource(this);
}

// Destructor
SndSource::~SndSource()
{
	stop();
	destroy();

	// Un-register the source
	if (getScene())
        ((Audio*)getScene()->getAudio())->getSourceManager()->unRegisterSource(this);
}

// Set the default EAX setting for the source
void SndSource::setDefaultEAX()
{
#ifdef O3D_EFX
	m_EAX.dwFlags = EAXBUFFER_DEFAULTFLAGS;
	m_EAX.flAirAbsorptionFactor = EAXBUFFER_DEFAULTAIRABSORPTIONFACTOR;
	m_EAX.flObstructionLFRatio = EAXBUFFER_DEFAULTOBSTRUCTIONLFRATIO;
	m_EAX.flOcclusionLFRatio = EAXBUFFER_DEFAULTOCCLUSIONLFRATIO;
	m_EAX.flOcclusionRoomRatio = EAXBUFFER_DEFAULTOCCLUSIONROOMRATIO;
	m_EAX.flRoomRolloffFactor = EAXBUFFER_DEFAULTROOMROLLOFFFACTOR;
	m_EAX.lDirect = EAXBUFFER_DEFAULTDIRECT;
	m_EAX.lDirectHF = EAXBUFFER_DEFAULTDIRECTHF;
	m_EAX.lObstruction = EAXBUFFER_DEFAULTOBSTRUCTION;
	m_EAX.lOcclusion = EAXBUFFER_DEFAULTOCCLUSION;
	m_EAX.lOutsideVolumeHF = EAXBUFFER_DEFAULTOUTSIDEVOLUMEHF;
	m_EAX.lRoom = EAXBUFFER_DEFAULTROOM;
	m_EAX.lRoomHF = EAXBUFFER_DEFAULTROOMHF;
#else
	m_EAX.dwFlags = 0x7;
	m_EAX.flAirAbsorptionFactor = 1.0f;
	m_EAX.flObstructionLFRatio = 0.0f;
	m_EAX.flOcclusionLFRatio = 0.25f;
	m_EAX.flOcclusionRoomRatio = 0.5f;
	m_EAX.flRoomRolloffFactor = 0.0f;
	m_EAX.lDirect = 0;
	m_EAX.lDirectHF = 0;
	m_EAX.lObstruction = 0;
	m_EAX.lOcclusion = 0;
	m_EAX.lOutsideVolumeHF = 0;
	m_EAX.lRoom = 0;
	m_EAX.lRoomHF = 0;
#endif

	m_modifiedEAX = True;
}

// These flags is for compute automatically some properties
#ifndef O3D_EFX
	#define EAXBUFFERFLAGS_DIRECTHFAUTO 0x00000001
	#define EAXBUFFERFLAGS_ROOMAUTO     0x00000002
	#define EAXBUFFERFLAGS_ROOMHFAUTO   0x00000004
#endif

void SndSource::setAutoComputeFlag(EAX_AutoComputeFlags flag, Bool b)
{
	switch (flag)
	{
	case EAX_DirectHFAuto:
		if (b) m_EAX.dwFlags |= EAXBUFFERFLAGS_DIRECTHFAUTO;
		  else m_EAX.dwFlags &= m_EAX.dwFlags ^ EAXBUFFERFLAGS_DIRECTHFAUTO;
		break;

	case EAX_RoomAuto:
		if (b) m_EAX.dwFlags |= EAXBUFFERFLAGS_ROOMAUTO;
		  else m_EAX.dwFlags &= m_EAX.dwFlags ^ EAXBUFFERFLAGS_ROOMAUTO;
		break;

	case EAX_RoomHFAutdo:
		if (b) m_EAX.dwFlags |= EAXBUFFERFLAGS_ROOMHFAUTO;
		  else m_EAX.dwFlags &= m_EAX.dwFlags ^ EAXBUFFERFLAGS_ROOMHFAUTO;
		break;

	default:
		O3D_ERROR(E_InvalidParameter("flag"));
		return;
	}
	m_modifiedEAX = True;
}

Bool SndSource::getAutoComputeFlag(EAX_AutoComputeFlags flag)const
{
	switch (flag)
	{
	case EAX_DirectHFAuto:
		return m_EAX.dwFlags & EAXBUFFERFLAGS_DIRECTHFAUTO;

	case EAX_RoomAuto:
		return m_EAX.dwFlags & EAXBUFFERFLAGS_ROOMAUTO;

	case EAX_RoomHFAutdo:
		return m_EAX.dwFlags & EAXBUFFERFLAGS_ROOMHFAUTO;

	default:
		O3D_ERROR(E_InvalidParameter("flag"));
		return False;
	}
}

// Take a source ID from the manager
Bool SndSource::create()
{
	// take a source ID
	if (m_sourceId == O3D_UNDEFINED)
	{
		// create a new source
        m_sourceId = ((Audio*)getScene()->getAudio())->getSourceManager()->getSourceID(this);

		if (m_sourceId != O3D_UNDEFINED)
		{
			put();
			return True;
		}
		else
		{
			return False;
		}
	}
	return True;
}

// Destroy the source.
void SndSource::destroy()
{
	// delete the source
	if (m_sourceId != O3D_UNDEFINED)
	{
		if (m_playing)
		{
			alSourceStop(m_sourceId);
			m_playing = False;
		}

		// we delete the current source
		// this have the effect to release it too for the source manager
		alDeleteSources(1,&m_sourceId);
		m_sourceId = O3D_UNDEFINED;
	}

	// clear the buffer list and release all buffers
	removeAllBuffer();
}

// Compute the source gain (useless to find the source playing priority).
Float SndSource::computeGain()
{
	Float sourceGain = 0.f;
	// get the current distance model
    Audio::DistanceModel distanceModel = ((Audio*)getScene()->getAudio())->getDistanceModel();

	// compute the new source factor. depend on the distance model
	switch (distanceModel)
	{
		case Audio::DISTMODEL_NONE:
			sourceGain = m_gain;
			break;

		case Audio::DISTMODEL_INVERSE:
			if (m_node)
				sourceGain = m_gain * m_refDistance / (m_refDistance + m_rollOff * (m_node->getAbsoluteMatrix().getTranslation().length() - m_refDistance));
			break;

		case Audio::DISTMODEL_INVERSE_CLAMP:
			if (m_node)
			{
				Float distance = m_node->getAbsoluteMatrix().getTranslation().length();
				distance = o3d::max(distance,m_refDistance);
				distance = o3d::min(distance,m_maxDistance);

				sourceGain = m_gain * m_refDistance / (m_refDistance + m_rollOff * (m_node->getAbsoluteMatrix().getTranslation().length() - m_refDistance));
			}
			break;

		case Audio::DISTMODEL_LINEAR:
			if (m_node)
				sourceGain = m_gain * (1.f - m_rollOff * (m_node->getAbsoluteMatrix().getTranslation().length() - m_refDistance) / (m_maxDistance - m_refDistance));
			break;

		case Audio::DISTMODEL_LINEAR_CLAMP:
			if (m_node)
			{
				Float distance = m_node->getAbsoluteMatrix().getTranslation().length();
				distance = o3d::max(distance,m_refDistance);
				distance = o3d::min(distance,m_maxDistance);

				sourceGain = m_gain * (1.f - m_rollOff * (m_node->getAbsoluteMatrix().getTranslation().length() - m_refDistance) / (m_maxDistance - m_refDistance));
			}
			break;

		case Audio::DISTMODEL_EXPONENT:
			if (m_node)
			{
				Float distance = m_node->getAbsoluteMatrix().getTranslation().length();

				if (distance != 0.0f)
					sourceGain = m_gain * pow(distance,-m_rollOff);
				else
					sourceGain = m_gain;// * 1.0f;
			}
			break;

		case Audio::DISTMODEL_EXPONENT_CLAMP:
			if (m_node)
			{
				Float distance = m_node->getAbsoluteMatrix().getTranslation().length();
				distance = o3d::max(distance,m_refDistance);
				distance = o3d::min(distance,m_maxDistance);

				if (distance != 0.0f)
					sourceGain = m_gain * pow(distance,-m_rollOff);
				else
					sourceGain = m_gain;// * 1.0f;
			}
			break;

		default:
			sourceGain = m_gain;
			break;
	}

	sourceGain = o3d::clamp(sourceGain, m_minGain, m_maxGain);
	return sourceGain;
}

// Set a unique buffer to play.
void SndSource::setUniqueBuffer(SndBuffer *buffer)
{
	m_buffersQueue.clear();

	if (buffer)
	{
		m_buffersQueue.push_back(SmartObject<SndBuffer>(this, buffer));

        // auto-play, and buffer valid
        if (buffer->isValid())
        {
            if (m_autoPlay && !m_playing)
                play();
        }
        else
        {
            // wait for buffer ready
            buffer->onSndBufferValid.connect(this, &SndSource::soundBufferValid, EvtHandler::CONNECTION_ASYNCH);
        }
	}
}

// Push back a buffer to play.
void SndSource::addBufferQueue(SndBuffer *buffer)
{
	if (buffer)
	{
		m_buffersQueue.push_back(SmartObject<SndBuffer>(this, buffer));

        // auto-play, and buffer valid, and buffer is actually the only one in the queue
        if (m_buffersQueue.size() == 1 && buffer->isValid())
        {
            if (m_autoPlay && !m_playing)
                play();
        }
        else
        {
            // wait for buffer ready
            buffer->onSndBufferValid.connect(this, &SndSource::soundBufferValid, EvtHandler::CONNECTION_ASYNCH);
        }
	}
}

// Remove the back buffer of the queue.
void SndSource::removeBufferQueue()
{
	if (!m_buffersQueue.empty())
	{
		disconnect(m_buffersQueue.back().get());
		m_buffersQueue.pop_back();
	}
}

// Remove a buffer from the list (updated only after a stop)
void SndSource::removeBuffer(SndBuffer *buffer)
{
	if (buffer)
	{
		// search for the buffer, and remove it
		IT_SmartBufferList it = std::find(m_buffersQueue.begin(), m_buffersQueue.end(), buffer);
		if (it != m_buffersQueue.end())
		{
			disconnect(buffer);
			m_buffersQueue.erase(it);
		}
	}
}

// Define the input sound buffer list.
void SndSource::setBufferList(const T_BufferList &bufferlist)
{
	// clear the current buffer list
	for (IT_SmartBufferList it = m_buffersQueue.begin(); it != m_buffersQueue.end() ; ++it)
	{
		disconnect(it->get());
	}

	m_buffersQueue.clear();

	// assign the new sound buffers
	for (CIT_BufferList it2 = bufferlist.begin(); it2 != bufferlist.end() ; ++it2)
	{
		addBufferQueue(*it2);
	}
}

// Get the number of played buffer.
UInt32 SndSource::getNumPlayedBuffers() const
{
	UInt32 processed = 0;

	if (m_playing && (m_sourceId != O3D_UNDEFINED))
		alGetSourcei(m_sourceId, AL_BUFFERS_PROCESSED, (ALint*)&processed);

	return processed;
}

// Remove all buffer from the queue.
void SndSource::removeAllBuffer()
{
	setBufferList(T_BufferList());
}

//---------------------------------------------------------------------------------------
// Sound source parameters
//---------------------------------------------------------------------------------------

// Set revert delay.
void SndSource::setReverbDelay(Float delay)
{
    if (((Audio*)getScene()->getAudio())->getRenderer()->isReverbSupported())
	{
		m_reverbDelay = delay;

		if (m_sourceId != O3D_UNDEFINED)
			alReverbDelay(m_sourceId,delay);
	}
}

void SndSource::setReverbRatio(Float ratio)
{
    if (((Audio*)getScene()->getAudio())->getRenderer()->isReverbSupported())
	{
		m_reverbScale = ratio;

		if (m_sourceId != O3D_UNDEFINED)
			alReverbScale(m_sourceId,ratio);
	}
}

void SndSource::setPitch(Float Pitch)
{
	m_pitch = Pitch;

	if (m_sourceId != O3D_UNDEFINED)
		alSourcef(m_sourceId,AL_PITCH,Pitch);
}

void SndSource::setGain(Float Gain)
{
	m_gain = Gain;

	if (m_sourceId != O3D_UNDEFINED)
		alSourcef(m_sourceId,AL_GAIN,Gain);
}

void SndSource::setMaxDistance(Float MaxDistance)
{
	m_maxDistance = MaxDistance;

	if (m_sourceId != O3D_UNDEFINED)
		alSourcef(m_sourceId,AL_MAX_DISTANCE,MaxDistance);
}

void SndSource::setRollOff(Float RollOff)
{
	m_rollOff = RollOff;

	if (m_sourceId != O3D_UNDEFINED)
		alSourcef(m_sourceId,AL_ROLLOFF_FACTOR,RollOff);
}

void SndSource::setRefDistance(Float RefDistance)
{
	m_refDistance = RefDistance;

	if (m_sourceId != O3D_UNDEFINED)
		alSourcef(m_sourceId,AL_REFERENCE_DISTANCE,RefDistance);
}

void SndSource::setRelative(Bool SourceRelative)
{
	m_relative = SourceRelative;

	if (m_sourceId != O3D_UNDEFINED)
		alSourcei(m_sourceId,AL_SOURCE_RELATIVE,SourceRelative);
}

void SndSource::setMinGain(Float MinGain)
{
	m_minGain = MinGain;

	if (m_sourceId != O3D_UNDEFINED)
		alSourcef(m_sourceId,AL_MIN_GAIN,MinGain);
}

void SndSource::setMaxGain(Float MaxGain)
{
	m_maxGain = MaxGain;

	if (m_sourceId != O3D_UNDEFINED)
		alSourcef(m_sourceId,AL_MAX_GAIN,MaxGain);
}

Bool SndSource::enableLooping()
{
	Bool ret = m_looping;
	m_looping = True;

	if (m_sourceId != O3D_UNDEFINED && !m_streaming)
		alSourcei(m_sourceId,AL_LOOPING,1);

	return ret;
}

Bool SndSource::disableLooping()
{
	Bool ret = m_looping;
	m_looping = False;

	if (m_sourceId != O3D_UNDEFINED)
		alSourcei(m_sourceId,AL_LOOPING,0);

	return ret;
}

//---------------------------------------------------------------------------------------
// Transformation parameters
//---------------------------------------------------------------------------------------

void SndSource::setVelocity(Vector3& vec)
{
	m_velocity = vec;

	if (m_sourceId != O3D_UNDEFINED)
		alSourcefv(m_sourceId,AL_VELOCITY,vec.getData());
}

void SndSource::setVelocity(Float x,Float y,Float z)
{
	m_velocity.set(x,y,z);

	if (m_sourceId != O3D_UNDEFINED)
		alSourcefv(m_sourceId,AL_VELOCITY,m_velocity.getData());
}

// play/pause/stop/rewind the source
void SndSource::play()
{
	// TODO queue of streamed sound buffer. Actually there is not check,
	// and only non streamed buffer can be used with a multiple buffers queue.
	// The solution is to play the next sound buffer when the end of the
	// current is reached. updateAutoRelease must be updated to in this
	// way...

	if (!m_playing && !m_buffersQueue.empty() && create())
	{
		// single buffer
		if (m_buffersQueue.size() == 1)
		{
			SndBuffer *sndBuffer = m_buffersQueue.front().get();

			if (!sndBuffer->isValid())
				return;

			alSourceStop(m_sourceId);

			// if it is a stream buffer
			if (sndBuffer->isStream())
			{
				// doesn't work with OpenAL source looping
				if (m_looping)
					alSourcei(m_sourceId, AL_LOOPING, 0);

				if (!sndBuffer->prepareStream(m_sourceId, m_position))
					return;

				m_streaming = True;
			}
			// a simple buffer
			else
			{
				alSourcei(m_sourceId, AL_BUFFER, sndBuffer->getFrontBufferId());
			}

			alSourcePlay(m_sourceId);
			m_playing = True;
		}
		// multiple buffers
		else if (m_buffersQueue.size() > 1)
		{
			UInt32 n = 0;
			UInt32 numBuffers = (UInt32)m_buffersQueue.size();
			UInt32 *buffersList = new UInt32[numBuffers];

			for (IT_SmartBufferList it = m_buffersQueue.begin(); it != m_buffersQueue.end(); ++it)
			{
				buffersList[n++] = (*it)->getFrontBufferId();
			}

			alSourceStop(m_sourceId);
			alSourceQueueBuffers(m_sourceId, numBuffers, buffersList);
			alSourcePlay(m_sourceId);

			m_playing = True;
		}
	}
}

void SndSource::pause()
{
    if (m_playing)
	{
        if (m_sourceId != O3D_UNDEFINED)
            alSourcePause(m_sourceId);

        m_playing = False;
	}
}

void SndSource::stop()
{
    if (m_playing)
	{
        if (m_sourceId != O3D_UNDEFINED)
        {
            alSourceStop(m_sourceId);

            // we delete the current source and release the id to the manager
            alDeleteSources(1,&m_sourceId);
        }

		m_playing = False;
		m_streaming = False;
		m_position = 0;

		m_sourceId = O3D_UNDEFINED;
	}
}

void SndSource::rewind()
{
	if (m_sourceId != O3D_UNDEFINED)
	{
		alSourceRewind(m_sourceId);
		m_playing = False;
		m_position = 0;
	}
}

// send to OpenAL source settings
void SndSource::put()
{
	if (m_sourceId != O3D_UNDEFINED)
	{
		alSourcef(m_sourceId, AL_PITCH, m_pitch);
		alSourcef(m_sourceId, AL_GAIN, m_gain);
		alSourcef(m_sourceId, AL_REFERENCE_DISTANCE, m_refDistance);
		alSourcef(m_sourceId, AL_ROLLOFF_FACTOR, m_rollOff);
		alSourcef(m_sourceId, AL_MAX_DISTANCE, m_maxDistance);
		alSourcef(m_sourceId, AL_MIN_GAIN, m_minGain);
		alSourcef(m_sourceId, AL_MAX_GAIN, m_maxGain);
		alSourcei(m_sourceId, AL_SOURCE_RELATIVE, m_relative);

		if (!m_streaming)
			alSourcei(m_sourceId, AL_LOOPING, m_looping);

        if (((Audio*)getScene()->getAudio())->getRenderer()->isReverbSupported())
		{
			alReverbDelay(m_sourceId, m_reverbDelay);
			alReverbScale(m_sourceId, m_reverbScale);
		}

		if (m_node)
			alSourcefv(m_sourceId, AL_POSITION, m_node->getAbsoluteMatrix().getTranslationPtr());

#ifdef O3D_EFX
		// put EAX source settings
		if (m_modifiedEAX && getScene()->getAudio()->getRenderer()->IsEAX())
		{
			alEAXSet(
					&DSPROPSETID_EAX_SourceProperties,
					DSPROPERTY_EAXBUFFER_ALLPARAMETERS,
					m_sourceId,
					&m_EAX,
					sizeof(EAXBUFFERPROPERTIES));

		// commit deferred setting to EAX
		//alEAXSet(
		//	&DSPROPSETID_EAX_SourceProperties,
		//	DSPROPERTY_EAXBUFFER_COMMITDEFERREDSETTINGS,
		//	0,
		// NULL,
		// 0);

		// get all settings (useful for know value when there is size scale flag active)
		alEAXGet(
				&DSPROPSETID_EAX_SourceProperties,
				DSPROPERTY_EAXBUFFER_ALLPARAMETERS,
				m_sourceId,
				&m_EAX,
				sizeof(EAXBUFFERPROPERTIES));
		}
#endif
	}
}

// set a material model for the source
#ifndef O3D_EFX
	#define EAX_MATERIAL_SINGLEWINDOW          (-2800)
	#define EAX_MATERIAL_SINGLEWINDOWLF        0.71f
	#define EAX_MATERIAL_SINGLEWINDOWROOMRATIO 0.43f
	#define EAX_MATERIAL_DOUBLEWINDOW          (-5000)
	#define EAX_MATERIAL_DOUBLEWINDOWHF        0.40f
	#define EAX_MATERIAL_DOUBLEWINDOWROOMRATIO 0.24f
	#define EAX_MATERIAL_THINDOOR              (-1800)
	#define EAX_MATERIAL_THINDOORLF            0.66f
	#define EAX_MATERIAL_THINDOORROOMRATIO     0.66f
	#define EAX_MATERIAL_THICKDOOR             (-4400)
	#define EAX_MATERIAL_THICKDOORLF           0.64f
	#define EAX_MATERIAL_THICKDOORROOMRTATION  0.27f
	#define EAX_MATERIAL_WOODWALL              (-4000)
	#define EAX_MATERIAL_WOODWALLLF            0.50f
	#define EAX_MATERIAL_WOODWALLROOMRATIO     0.30f
	#define EAX_MATERIAL_BRICKWALL             (-5000)
	#define EAX_MATERIAL_BRICKWALLLF           0.60f
	#define EAX_MATERIAL_BRICKWALLROOMRATIO    0.24f
	#define EAX_MATERIAL_STONEWALL             (-6000)
	#define EAX_MATERIAL_STONEWALLLF           0.68f
	#define EAX_MATERIAL_STONEWALLROOMRATIO    0.20f
	#define EAX_MATERIAL_CURTAIN               (-1200)
	#define EAX_MATERIAL_CURTAINLF             0.15f
	#define EAX_MATERIAL_CURTAINROOMRATIO      1.00f
#endif /* !O3D_EFX */

void SndSource::setMaterialModel(EAX_MaterialModels model)
{
	if (model < EAX_MatNumber)
	{
		switch (model)
		{
		case EAX_OcclusionSingleWindow:
			// Single window material preset
			m_EAX.lOcclusion = EAX_MATERIAL_SINGLEWINDOW;
			m_EAX.flOcclusionLFRatio = EAX_MATERIAL_SINGLEWINDOWLF;
			m_EAX.flOcclusionRoomRatio = EAX_MATERIAL_SINGLEWINDOWROOMRATIO;
			break;

		case EAX_OcclusionDoubleWindow:
			// Double window material preset
			m_EAX.lOcclusion = EAX_MATERIAL_DOUBLEWINDOW;
			m_EAX.flOcclusionLFRatio = EAX_MATERIAL_DOUBLEWINDOWHF;
			m_EAX.flOcclusionRoomRatio = EAX_MATERIAL_DOUBLEWINDOWROOMRATIO;
			break;

		case EAX_OcclusionThinDoor:
			// Thin door material preset
			m_EAX.lOcclusion = EAX_MATERIAL_THINDOOR;
			m_EAX.flOcclusionLFRatio = EAX_MATERIAL_THINDOORLF;
			m_EAX.flOcclusionRoomRatio = EAX_MATERIAL_THINDOORROOMRATIO;
			break;

		case EAX_OcclusionThickDoor:
			// Thick door material preset
			m_EAX.lOcclusion = EAX_MATERIAL_THICKDOOR;
			m_EAX.flOcclusionLFRatio = EAX_MATERIAL_THICKDOORLF;
			m_EAX.flOcclusionRoomRatio = EAX_MATERIAL_THICKDOORROOMRTATION;
			break;

		case EAX_OcclusionWoodWall:
			// Wood wall material preset
			m_EAX.lOcclusion = EAX_MATERIAL_WOODWALL;
			m_EAX.flOcclusionLFRatio = EAX_MATERIAL_WOODWALLLF;
			m_EAX.flOcclusionRoomRatio = EAX_MATERIAL_WOODWALLROOMRATIO;
			break;

		case EAX_OcclusionBrickWall:
			// Brick wall material preset
			m_EAX.lOcclusion = EAX_MATERIAL_BRICKWALL;
			m_EAX.flOcclusionLFRatio = EAX_MATERIAL_BRICKWALLLF;
			m_EAX.flOcclusionRoomRatio = EAX_MATERIAL_BRICKWALLROOMRATIO;
			break;

		case EAX_OcclusionStoneWall:
			// Stone wall material preset
			m_EAX.lOcclusion = EAX_MATERIAL_STONEWALL;
			m_EAX.flOcclusionLFRatio = EAX_MATERIAL_STONEWALLLF;
			m_EAX.flOcclusionRoomRatio = EAX_MATERIAL_STONEWALLROOMRATIO;
			break;

		case EAX_OcclusionCurtain:
			// Curtain material preset
			m_EAX.lOcclusion = EAX_MATERIAL_CURTAIN;
			m_EAX.flOcclusionLFRatio = EAX_MATERIAL_CURTAINLF;
			m_EAX.flOcclusionRoomRatio = EAX_MATERIAL_CURTAINROOMRATIO;
			break;

		case EAX_ObstructionSingleWindow:
			// Single window material preset
			m_EAX.lObstruction = EAX_MATERIAL_SINGLEWINDOW;
			m_EAX.flObstructionLFRatio = EAX_MATERIAL_SINGLEWINDOWLF;
			m_EAX.flOcclusionRoomRatio = EAX_MATERIAL_SINGLEWINDOWROOMRATIO;
			break;

		case EAX_ObstructionDoubleWindow:
			// Double window material preset
			m_EAX.lObstruction = EAX_MATERIAL_DOUBLEWINDOW;
			m_EAX.flObstructionLFRatio = EAX_MATERIAL_DOUBLEWINDOWHF;
			m_EAX.flOcclusionRoomRatio = EAX_MATERIAL_DOUBLEWINDOWROOMRATIO;
			break;

		case EAX_ObstructionThinDoor:
			// Thin door material preset
			m_EAX.lObstruction = EAX_MATERIAL_THINDOOR;
			m_EAX.flObstructionLFRatio = EAX_MATERIAL_THINDOORLF;
			m_EAX.flOcclusionRoomRatio = EAX_MATERIAL_THINDOORROOMRATIO;
			break;

		case EAX_ObstructionThickDoor:
			// Thick door material preset
			m_EAX.lObstruction = EAX_MATERIAL_THICKDOOR;
			m_EAX.flObstructionLFRatio = EAX_MATERIAL_THICKDOORLF;
			m_EAX.flOcclusionRoomRatio = EAX_MATERIAL_THICKDOORROOMRTATION;
			break;

		case EAX_ObstructionWoodWall:
			// Wood wall material preset
			m_EAX.lObstruction = EAX_MATERIAL_WOODWALL;
			m_EAX.flObstructionLFRatio = EAX_MATERIAL_WOODWALLLF;
			m_EAX.flOcclusionRoomRatio = EAX_MATERIAL_WOODWALLROOMRATIO;
			break;

		case EAX_ObstructionBrickWall:
			// Brick wall material preset
			m_EAX.lObstruction = EAX_MATERIAL_BRICKWALL;
			m_EAX.flObstructionLFRatio = EAX_MATERIAL_BRICKWALLLF;
			m_EAX.flOcclusionRoomRatio = EAX_MATERIAL_BRICKWALLROOMRATIO;
			break;

		case EAX_ObstructionStoneWall:
			// Stone wall material preset
			m_EAX.lObstruction = EAX_MATERIAL_STONEWALL;
			m_EAX.flObstructionLFRatio = EAX_MATERIAL_STONEWALLLF;
			m_EAX.flOcclusionRoomRatio = EAX_MATERIAL_STONEWALLROOMRATIO;
			break;

		case EAX_ObstructionCurtain:
			// Curtain material preset
			m_EAX.lObstruction = EAX_MATERIAL_CURTAIN;
			m_EAX.flObstructionLFRatio = EAX_MATERIAL_CURTAINLF;
			m_EAX.flOcclusionRoomRatio = EAX_MATERIAL_CURTAINROOMRATIO;
			break;

		default:
			break;
		}
		m_modifiedEAX = True;
	}
	else
	{
		O3D_ERROR(E_InvalidParameter("model"));
	}
}

// Called when a sound buffer is ready to be played.
void SndSource::soundBufferValid(SndBuffer* sndBuffer)
{
	if (sndBuffer)
	{
		if (m_autoPlay && !m_playing)
			play();
	}
}

// compute the fading
void SndSource::computeFading()
{
	if (m_fadeInOut == 0 || !m_playing)
		return;

	// compute the fade value depend on time
	Int64 curtime = System::getTime();
	Float computedTime = (Float)(curtime - m_lastTime) / (Float)System::getTimeFrequency();
	if (m_lastTime == O3D_UNDEFINED)
		computedTime = 0;

	m_lastTime = curtime;

	// make a fade in
	if (m_fadeInOut == -1)
	{
		if (m_gain <= m_maxGain)
		{
			setGain(m_maxGain);
			m_fadeInOut = 0;
			m_lastTime = 0;
			m_fadeScale = 0.0f;
			m_lastTime = O3D_UNDEFINED;
			return;
		}

		m_gain += (m_fadeScale * computedTime);
		if (m_gain > m_maxGain) m_gain = m_maxGain;

		setGain(m_gain);
	}
	// make a fade out
	else if (m_fadeInOut == 1)
	{
		if (m_gain <= m_minGain)
		{
			setGain(m_minGain);
			m_fadeInOut = 0;
			m_lastTime = 0;
			m_fadeScale = 0.0f;
			m_lastTime = O3D_UNDEFINED;

            // signal
            onFadeOutComplete();

			return;
		}

		m_gain -= (m_fadeScale * computedTime);
		if (m_gain < m_minGain) m_gain = m_minGain;

		setGain(m_gain);
	}
}

// update auto release
void SndSource::updateAutoRelease()
{
	// for multiple buffers source
	if (m_autoRelease)
	{
		UInt32 playedSndBuffer = getNumPlayedBuffers();

		if (playedSndBuffer)
		{
			UInt32 *unqueue = new UInt32[playedSndBuffer];

			// un-queue played buffers
			alSourceUnqueueBuffers(m_sourceId, playedSndBuffer, (ALuint*)unqueue);
			deleteArray(unqueue);

			// un-queue played buffers from the list by front
			for (UInt32 i = 0; i < playedSndBuffer; ++i)
			{
				if (!m_buffersQueue.empty())
				{
					disconnect(m_buffersQueue.front().get());
					m_buffersQueue.pop_front();
				}
			}
		}
	}
}

// update buffer stream
void SndSource::updateStream()
{
	if (m_streaming)
	{
		SndBuffer *buffer = m_buffersQueue.front().get();

        if (!buffer || !buffer->updateStream(m_sourceId, m_looping, m_position))
		{
			// stop the streaming update
			m_streaming = False;
		}
	}
}

// update source position/direction
void SndSource::update()
{
	if (m_sourceId != O3D_UNDEFINED)
	{
		ALenum state;
		alGetSourcei(m_sourceId, AL_SOURCE_STATE, &state);

		// if the source have finished to play then we need to release it
		if (m_playing && (state == AL_STOPPED))
		{
			stop();
			return;
		}

		if (hasUpdated() && m_node)
			alSourcefv(m_sourceId, AL_POSITION, m_node->getAbsoluteMatrix().getTranslation().getData());

		if (m_playing)
		{
			computeFading();
			updateAutoRelease();
			updateStream();
		}
	}
}

// Setup the modelview matrix to OpenGL
void SndSource::setUpModelView()
{
    O3D_ASSERT(getScene()->getActiveCamera() != nullptr);

	if (m_node)
		getScene()->getContext()->modelView().set(
				getScene()->getActiveCamera()->getModelviewMatrix() *
				m_node->getAbsoluteMatrix());
	else
		getScene()->getContext()->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix());
}

// draw a symbolic representation of a source
void SndSource::draw(const DrawInfo &drawInfo)
{
	if (!getActivity() || !getVisibility())
		return;

	if ((drawInfo.pass == DrawInfo::AMBIENT_PASS/*SymbolicPass*/) && getScene()->getDrawObject(Scene::DRAW_SND_SOURCE_OMNI))
	{
		// if this source is visible draw its symbolic
		setUpModelView();

		PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access();

		if (getScene()->getDrawObject(Scene::DRAW_LOCAL_AXIS))
			primitive->drawLocalAxis();

		primitive->setColor(0.0f,0.0f,1.0f);
		primitive->draw(PrimitiveManager::WIRE_SPHERE1, Vector3(0.3f,0.3f,0.3f));
	}
}

// Serialization
Bool SndSource::writeToFile(OutStream &os)
{
    SceneObject::writeToFile(os);

    os   << m_pitch
		 << m_gain
		 << m_refDistance
		 << m_rollOff
		 << m_maxDistance
		 << m_minGain
		 << m_maxGain
		 << m_looping
		 << m_relative;

	// EAX settings
    os   << (UInt32)m_EAX.dwFlags
		 << m_EAX.flAirAbsorptionFactor
		 << m_EAX.flObstructionLFRatio
		 << m_EAX.flOcclusionLFRatio
		 << m_EAX.flOcclusionRoomRatio
		 << m_EAX.flRoomRolloffFactor
		 << (Int32)m_EAX.lDirect
		 << (Int32)m_EAX.lDirectHF
		 << (Int32)m_EAX.lObstruction
		 << (Int32)m_EAX.lOcclusion
		 << (Int32)m_EAX.lOutsideVolumeHF
		 << (Int32)m_EAX.lRoom
		 << (Int32)m_EAX.lRoomHF;

    os << getNumBuffers();

	for (IT_SmartBufferList it = m_buffersQueue.begin(); it != m_buffersQueue.end(); ++it)
	{
        os << *(*it).get();
	}

	return True;
}

Bool SndSource::readFromFile(InStream &is)
{
    SceneObject::readFromFile(is);

    is   >> m_pitch
		 >> m_gain
		 >> m_refDistance
		 >> m_rollOff
		 >> m_maxDistance
		 >> m_minGain
		 >> m_maxGain
		 >> m_looping
		 >> m_relative;

	// EAX settings
	UInt32 uintulong;
	Int32 intlong;

	// EAX settings
    is >> uintulong; m_EAX.dwFlags = (UInt32)uintulong;

    is   >> m_EAX.flAirAbsorptionFactor
		 >> m_EAX.flObstructionLFRatio
		 >> m_EAX.flOcclusionLFRatio
		 >> m_EAX.flOcclusionRoomRatio
		 >> m_EAX.flRoomRolloffFactor;

    is >> intlong; m_EAX.lDirect = (Int32)intlong;
    is >> intlong; m_EAX.lDirectHF = (Int32)intlong;
    is >> intlong; m_EAX.lObstruction = (Int32)intlong;
    is >> intlong; m_EAX.lOcclusion = (Int32)intlong;
    is >> intlong; m_EAX.lOutsideVolumeHF = (Int32)intlong;
    is >> intlong; m_EAX.lRoom = (Int32)intlong;
    is >> intlong; m_EAX.lRoomHF = (Int32)intlong;

	Int32 size;
    is >> size;

	for (Int32 i = 0; i < size; ++i)
	{
        SndBuffer *sndBuffer = ((Audio*)getScene()->getAudio())->getBufferManager()->readSndBuffer(is);
		addBufferQueue(sndBuffer);
	}

	return True;
}

//---------------------------------------------------------------------------------------
// class OmniSource
//---------------------------------------------------------------------------------------

O3D_IMPLEMENT_DYNAMIC_CLASS1(OmniSource, AUDIO_SND_SOURCE_OMNI, SndSource)

// Default constructor.
OmniSource::OmniSource(BaseObject *parent) :
	SndSource(parent)
{
}

// Get the drawing type.
UInt32 OmniSource::getDrawType() const { return Scene::DRAW_SND_SOURCE_OMNI; }

//---------------------------------------------------------------------------------------
// class DirectSource
//---------------------------------------------------------------------------------------

O3D_IMPLEMENT_DYNAMIC_CLASS1(DirectSource, AUDIO_SND_SOURCE_DIRECT, SndSource)

// Default constructor.
DirectSource::DirectSource(BaseObject *parent) :
	SndSource(parent)
{
	m_coneInnerAngle = 360.f;
	m_coneOuterAngle = 360.f;

	m_coneOuterGain = 0.0f;
}

// Get the drawing type
UInt32 DirectSource::getDrawType() const { return Scene::DRAW_SND_SOURCE_DIRECT; }

// send OpenAL source settings
void DirectSource::put()
{
	SndSource::put();

	if (m_sourceId != O3D_UNDEFINED)
	{
		alSourcef(m_sourceId,AL_CONE_INNER_ANGLE,m_coneInnerAngle);
		alSourcef(m_sourceId,AL_CONE_OUTER_ANGLE,m_coneOuterAngle);
		alSourcef(m_sourceId,AL_CONE_OUTER_GAIN,m_coneOuterGain);

		if (m_node)
			alSourcefv(m_sourceId, AL_DIRECTION, m_node->getAbsoluteMatrix().getZ().getData());
	}
}

// Update source position/direction
void DirectSource::update()
{
	SndSource::update();

	if (hasUpdated())
	{
		if ((m_sourceId != O3D_UNDEFINED) && m_node)
			alSourcefv(m_sourceId, AL_DIRECTION, m_node->getAbsoluteMatrix().getZ().getData());
	}
}

void DirectSource::draw(const DrawInfo &drawInfo)
{
	if (!getActivity() || !getVisibility())
		return;

	if ((drawInfo.pass == DrawInfo::AMBIENT_PASS/*SymbolicPass*/) && getScene()->getDrawObject(Scene::DRAW_SND_SOURCE_DIRECT))
	{
		// if this source is visible draw its symbolic
		setUpModelView();

		PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access();

		if (getScene()->getDrawObject(Scene::DRAW_LOCAL_AXIS))
			primitive->drawLocalAxis();

		primitive->setColor(0.0f,0.0f,1.0f);

		if (m_coneInnerAngle < 180.f)
		{
			primitive->beginDraw(P_LINES);
				primitive->addVertex(0,0,0);
				primitive->addVertex(0,0,-2);
			primitive->endDraw();

			primitive->modelView().translate(Vector3(0,0,-2));
			primitive->draw(PrimitiveManager::SOLID_SPHERE1, Vector3(0.1f,0.1f,0.1f));
			primitive->draw(PrimitiveManager::WIRE_CONE1, Vector3(
					2.f*tanf(o3d::toRadian(m_coneInnerAngle)),
					2.f*tanf(o3d::toRadian(m_coneInnerAngle)),
					2.f));
		}
		else
			primitive->draw(PrimitiveManager::WIRE_SPHERE1, Vector3(0.3f,0.3f,0.3f));
	}
}

//---------------------------------------------------------------------------------------
// Sound source parameters.
//---------------------------------------------------------------------------------------

void DirectSource::setConeOuterGain(Float ConeOuterGain)
{
	m_coneOuterGain = ConeOuterGain;

	if (m_sourceId != O3D_UNDEFINED)
		alSourcef(m_sourceId,AL_CONE_OUTER_GAIN,ConeOuterGain);
}

void DirectSource::setConeInnerAngle(Float ConeInnerAngle)
{
	m_coneInnerAngle = ConeInnerAngle;

	if (m_sourceId != O3D_UNDEFINED)
		alSourcef(m_sourceId,AL_CONE_INNER_ANGLE,ConeInnerAngle);
}

void DirectSource::setConeOuterAngle(Float ConeOuterAngle)
{
	m_coneOuterAngle = ConeOuterAngle;

	if (m_sourceId != O3D_UNDEFINED)
		alSourcef(m_sourceId,AL_CONE_OUTER_ANGLE,ConeOuterAngle);
}

// Serialization
Bool DirectSource::writeToFile(OutStream &os)
{
    if (!SndSource::writeToFile(os))
		return False;

    os   << m_coneInnerAngle
		 << m_coneOuterAngle
		 << m_coneOuterGain;

	return True;
}

Bool DirectSource::readFromFile(InStream &is)
{
    if (!SndSource::readFromFile(is))
		return False;

    is   >> m_coneInnerAngle
		 >> m_coneOuterAngle
		 >> m_coneOuterGain;

	return True;
}

//---------------------------------------------------------------------------------------
// class SndSourceManager
//---------------------------------------------------------------------------------------

O3D_IMPLEMENT_DYNAMIC_CLASS1(SndSourceManager, AUDIO_SND_SOURCE_LIST, SceneEntity)

SndSourceManager::SndSourceManager(BaseObject *parent) :
	SceneEntity(parent),
	m_maxSources(0)
{
	UInt32 source_id[O3D_SOUND_MAX_SOURCES];

	// create all sources objects for check how many the hardware support
	// i know it's not the better way but i've not found anyway
	while (m_maxSources < O3D_SOUND_MAX_SOURCES)
	{
		alGenSources(1,&source_id[m_maxSources]);
		if (alGetError() == AL_NO_ERROR)
		{
			m_maxSources++;
		}
		else
			break;
	}

	O3D_MESSAGE(String("OpenAL support currently a maximum of ") << m_maxSources << String(" sources"));

	alGetError();
	// delete all created sources
	alDeleteSources(m_maxSources,source_id);
}

// get a source id, depend on source gain, refdistance and position and current world
// position for compute the priority and then return or not an id
UInt32 SndSourceManager::getSourceID(SndSource* sndSource)
{
	UInt32 sourceId;

	// create a new source
	alGenSources(1, &sourceId);

	// else check all playing sources for take the less significant one and
	// then to stop it and to get it's ID.
	if (alGetError() != AL_NO_ERROR)
	{
		Float compSourceGain;
		Float minorPriority = o3d::Limits<Float>::max();
		Float newSourceGain = sndSource->computeGain();
		SndSource *sndTempSource;
		SndSource *sndMinorSource = NULL;

		// first pass we compute for the lesser source priority
	 	for (IT_SourceSet it = m_registredSources.begin(); it != m_registredSources.end(); ++it)
		{
			sndTempSource = (*it);

			// we take only the non relatives and playing sources
			if (sndTempSource->isPlaying() && !sndTempSource->getRelative())
			{
				compSourceGain = sndTempSource->computeGain();

				if (compSourceGain < minorPriority)
				{
					minorPriority = compSourceGain;
					sndMinorSource = sndTempSource;
				}
			}
		}

		// if the new source priority is greater than the lesser playing source
		// we take it's id for the new source
		if (newSourceGain > minorPriority)
		{
			sndMinorSource->stop();
			alGenSources(1,&sourceId);

			if (alGetError() == AL_NO_ERROR)
				return sourceId;
			else
				O3D_ERROR(E_InvalidResult("Unable to create a new sound source"));
		}
		else
			return O3D_UNDEFINED;
	}

	return sourceId;
}

// Register a source
void SndSourceManager::registerSource(SndSource *sndSource)
{
	if (sndSource)
	{
		IT_SourceSet it = m_registredSources.find(sndSource);
		if (it == m_registredSources.end())
		{
			m_registredSources.insert(sndSource);
		}
		else
		{
			O3D_ERROR(E_InvalidParameter("Source is already registered"));
		}
	}
}

// Un-register a source
void SndSourceManager::unRegisterSource(SndSource *sndSource)
{
	if (sndSource)
	{
		IT_SourceSet it = m_registredSources.find(sndSource);
		if (it != m_registredSources.end())
		{
			m_registredSources.erase(it);
		}
		else
		{
			O3D_ERROR(E_InvalidParameter("Source was not previously registered"));
		}
	}
}

