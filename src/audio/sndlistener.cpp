/**
 * @file sndlistener.cpp
 * @brief Implementation of SndListener.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-04-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/audio/precompiled.h"

#include "o3d/audio/sndlistener.h"
#include "o3d/audio/audiotype.h"
#include "o3d/audio/audio.h"
#include "o3d/audio/audiorenderer.h"
#include "o3d/engine/gl.h"
#include "o3d/engine/object/primitivemanager.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/hierarchy/hierarchytree.h"
#include "o3d/engine/object/camera.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(SndListener, AUDIO_SND_LISTENER, SceneObject)

// Get the drawing type
UInt32 SndListener::getDrawType() const { return Scene::DRAW_SND_LISTENER; }

SndListener::SndListener(BaseObject *parent) :
	SceneObject(parent),
	m_gain(1.f),
	m_velocity(0.f,0.f,0.f),
	m_orientationAt(0.f,0.f,-1.f),
	m_orientationUp(0.f,1.f,0.f),
	m_modifiermodel(False),
	m_modifiedEAX(False),
	m_oldGain(-1.f)

{
	setDrawable(True);
	setUpdatable(True);

	setGenericEnv();
}

SndListener::~SndListener()
{
}

// set to generic default environment
void SndListener::setGenericEnv()
{
	// set the generic EAX environment settings
	// there are here for none EAX availability for the compatibilities with EAX availability
	// and for the serialization
#ifdef O3D_EFX
	m_EAX.dwEnvironment = EAX_ENVIRONMENT_GENERIC;
	m_EAX.dwFlags = EAXLISTENERFLAGS_DECAYTIMESCALE | EAXLISTENERFLAGS_REFLECTIONSSCALE |
		EAXLISTENERFLAGS_REFLECTIONSDELAYSCALE | EAXLISTENERFLAGS_REVERBSCALE |
		EAXLISTENERFLAGS_REVERBDELAYSCALE | EAXLISTENERFLAGS_DECAYHFLIMIT;

	m_EAX.flAirAbsorptionHF = EAXLISTENER_DEFAULTAIRABSORPTIONHF;
	m_EAX.flDecayHFRatio = EAXLISTENER_DEFAULTDECAYHFRATIO;
	m_EAX.flDecayTime = EAXLISTENER_DEFAULTDECAYTIME;
	m_EAX.flEnvironmentDiffusion = EAXLISTENER_DEFAULTENVIRONMENTDIFFUSION;
	m_EAX.flEnvironmentSize = EAXLISTENER_DEFAULTENVIRONMENTSIZE;
	m_EAX.flReflectionsDelay = EAXLISTENER_DEFAULTREFLECTIONSDELAY;
	m_EAX.flReverbDelay = EAXLISTENER_DEFAULTREVERBDELAY;
	m_EAX.flRoomRolloffFactor = EAXLISTENER_DEFAULTROOMROLLOFFFACTOR;
	m_EAX.lReflections = EAXLISTENER_DEFAULTREFLECTIONS;
	m_EAX.lReverb = EAXLISTENER_DEFAULTREVERB;
	m_EAX.lRoom = EAXLISTENER_DEFAULTROOM;
	m_EAX.lRoomHF = EAXLISTENER_DEFAULTROOMHF;
#else
	m_EAX.dwEnvironment = 0;
	m_EAX.dwFlags = 0x01 | 0x02 | 0x04 | 0x08 | 0x10 | 0x20;

	m_EAX.flAirAbsorptionHF = -5.0f;
	m_EAX.flDecayHFRatio = 0.83f;
	m_EAX.flDecayTime = 1.49f;
	m_EAX.flEnvironmentDiffusion = 1.0f;
	m_EAX.flEnvironmentSize = 7.5f;
	m_EAX.flReflectionsDelay = 0.007f;
	m_EAX.flReverbDelay = 0.011f;
	m_EAX.flRoomRolloffFactor = 0.0f;
	m_EAX.lReflections = -2602;
	m_EAX.lReverb = 200;
	m_EAX.lRoom = -1000;
	m_EAX.lRoomHF = -100;
#endif

	m_modifiermodel = True;
	m_modifiedEAX = True;
}

// set an environment model
void SndListener::setEnvModel(O3DEAX_EnvModel envmodel)
{
	if (envmodel < EAX_EnvironnementCount)
	{
		if (envmodel != (Int32)m_EAX.dwEnvironment)
		{
	        m_EAX.dwEnvironment = envmodel;
			m_modifiermodel = m_modifiedEAX = True;
		}
	}
	else
	{
		O3D_ERROR(E_InvalidParameter("envmodel"));
	}
}

// update the absolute position of the listener
void SndListener::update()
{
	O3D_ASSERT(m_node);
	clearUpdated();

	if (m_node && m_node->hasUpdated())
	{
		m_orientationUp = m_node->getAbsoluteMatrix().getY();
		m_orientationAt = -m_node->getAbsoluteMatrix().getZ();

		setUpdated();
	}
}

// setup OpenAL parameters
void SndListener::put()
{
	if (hasUpdated())
	{
		if (m_node)
			alListenerfv(AL_POSITION, m_node->getAbsoluteMatrix().getTranslationPtr());

		alListenerfv(AL_VELOCITY, m_velocity.getData());

		Float tmp[6];

		tmp[0] = m_orientationAt[X]; tmp[1] = m_orientationAt[Y]; tmp[2] = m_orientationAt[Z];
		tmp[3] = m_orientationUp[X]; tmp[4] = m_orientationUp[Y]; tmp[5] = m_orientationUp[Z];

		alListenerfv(AL_ORIENTATION, tmp);
		alListenerf(AL_GAIN, m_gain);
	}

#ifdef O3D_EFX
	// put EAX listener settings
	if (m_modifiedEAX && getScene()->getAudio()->renderer()->isEAX())
	{
		if (m_modifiermodel)
		{
			unsigned long EAXVal = m_EAX.dwEnvironment;
			alEAXSet(&DSPROPSETID_EAX_ListenerProperties,DSPROPERTY_EAXLISTENER_ENVIRONMENT,0,&EAXVal,
				sizeof(unsigned long));
		}
		else
		{
			Bool undo = False;

			if (m_EAX.dwEnvironment == EAX_Personalised)
			{
				m_EAX.dwEnvironment = EAX_Generic;
				undo = True;
			}

			alEAXSet(&DSPROPSETID_EAX_ListenerProperties,DSPROPERTY_EAXLISTENER_ALLPARAMETERS,0,&m_EAX,
				sizeof(EAXLISTENERPROPERTIES));

			if (undo)
				m_EAX.dwEnvironment = EAX_Personalised;
		}

		// commit deferred setting to EAX
		//alEAXSet(&DSPROPSETID_EAX20_ListenerProperties,
		//	DSPROPERTY_EAXLISTENER_COMMITDEFERREDSETTINGS,0,NULL,0);

		// get all settings (useful for know value when there is size scale flag active)
		alEAXGet(&DSPROPSETID_EAX_ListenerProperties,DSPROPERTY_EAXLISTENER_ALLPARAMETERS,0,&m_EAX,
			sizeof(EAXLISTENERPROPERTIES));
	}
#endif
}

// Setup the modelview matrix to OpenGL
void SndListener::setUpModelView()
{
	O3D_ASSERT(getScene()->getActiveCamera() != NULL);
	if (m_node)
		getScene()->getContext()->modelView().set(
				getScene()->getActiveCamera()->getModelviewMatrix() *
				m_node->getAbsoluteMatrix());
	else
		getScene()->getContext()->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix());
}

// draw a symbolic representation of a listener
void SndListener::draw(const DrawInfo &drawInfo)
{
	if (!getActivity() || !getVisibility())
		return;

	if ((drawInfo.pass == DrawInfo::AMBIENT_PASS/*SymbolicPass*/) && getScene()->getDrawObject(Scene::DRAW_SND_LISTENER))
	{
		// if it is m_IsVisible draw its symbolic
		setUpModelView();

		PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access();

		if (getScene()->getDrawObject(Scene::DRAW_LOCAL_AXIS))
			primitive->drawLocalAxis();

		primitive->setColor(1.0f,0.0f,1.0f);
		primitive->draw(PrimitiveManager::WIRE_CUBE1, Vector3(3.0f,1.0f,1.0f));
	}
}

// These flags determine what properties are affected by environment size
// set/get flags state
#ifndef O3D_EFX
	#define EAXLISTENERFLAGS_DECAYTIMESCALE        0x00000001
	#define EAXLISTENERFLAGS_REFLECTIONSSCALE      0x00000002
	#define EAXLISTENERFLAGS_REFLECTIONSDELAYSCALE 0x00000004
	#define EAXLISTENERFLAGS_REVERBSCALE           0x00000008
	#define EAXLISTENERFLAGS_REVERBDELAYSCALE      0x00000010
	#define EAXLISTENERFLAGS_DECAYHFLIMIT          0x00000020
#endif // !O3D_EFX

void SndListener::setEnvSizeFlag(O3DEAX_EnvSizeFlags flag,Bool b)
{
	switch (flag)
	{
	case EAX_DecayTime:
		if (b) m_EAX.dwFlags |= EAXLISTENERFLAGS_DECAYTIMESCALE;
		  else m_EAX.dwFlags &= m_EAX.dwFlags ^ EAXLISTENERFLAGS_DECAYTIMESCALE;
		break;

	case EAX_Reflection:
		if (b) m_EAX.dwFlags |= EAXLISTENERFLAGS_REFLECTIONSSCALE;
		  else m_EAX.dwFlags &= m_EAX.dwFlags ^ EAXLISTENERFLAGS_REFLECTIONSSCALE;
		break;

	case EAX_ReflectionDelay:
		if (b) m_EAX.dwFlags |= EAXLISTENERFLAGS_REFLECTIONSDELAYSCALE;
		  else m_EAX.dwFlags &= m_EAX.dwFlags ^ EAXLISTENERFLAGS_REFLECTIONSDELAYSCALE;
		break;

	case EAX_Reverb:
		if (b) m_EAX.dwFlags |= EAXLISTENERFLAGS_REVERBSCALE;
		  else m_EAX.dwFlags &= m_EAX.dwFlags ^ EAXLISTENERFLAGS_REVERBSCALE;
		break;

	case EAX_ReverbDelay:
		if (b) m_EAX.dwFlags |= EAXLISTENERFLAGS_REVERBDELAYSCALE;
		  else m_EAX.dwFlags &= m_EAX.dwFlags ^ EAXLISTENERFLAGS_REVERBDELAYSCALE;
		break;

	case EAX_DecayLimit:
		if (b) m_EAX.dwFlags |= EAXLISTENERFLAGS_DECAYHFLIMIT;
		  else m_EAX.dwFlags &= m_EAX.dwFlags ^ EAXLISTENERFLAGS_DECAYHFLIMIT;
		break;

	default:
		O3D_ERROR(E_InvalidParameter("flag"));
		return;
	}
	m_modifiedEAX = True;
	m_EAX.dwEnvironment = (Int32)EAX_Personalised;
}

Bool SndListener::getEnvSizeFlag(O3DEAX_EnvSizeFlags flag)const
{
	switch (flag)
	{
	case EAX_DecayTime:
		return m_EAX.dwFlags & EAXLISTENERFLAGS_DECAYTIMESCALE;

	case EAX_Reflection:
		return m_EAX.dwFlags & EAXLISTENERFLAGS_REFLECTIONSSCALE;

	case EAX_ReflectionDelay:
		return m_EAX.dwFlags & EAXLISTENERFLAGS_REFLECTIONSDELAYSCALE;

	case EAX_Reverb:
		return m_EAX.dwFlags & EAXLISTENERFLAGS_REVERBSCALE;

	case EAX_ReverbDelay:
		return m_EAX.dwFlags & EAXLISTENERFLAGS_REVERBDELAYSCALE;

	case EAX_DecayLimit:
		return m_EAX.dwFlags & EAXLISTENERFLAGS_DECAYHFLIMIT;

	default:
		O3D_ERROR(E_InvalidParameter("flag"));
		return False;
	}
}

// serialization
Bool SndListener::writeToFile(OutStream &os)
{
    SceneObject::writeToFile(os);

    os  << m_gain
		<< m_velocity;

    if (((Audio*)getScene()->getAudio())->getActiveListener() == this)
        os << True;

	// EAX settings
    os  << (UInt32)m_EAX.dwEnvironment
		<< (UInt32)m_EAX.dwFlags
		<< m_EAX.flAirAbsorptionHF
		<< m_EAX.flDecayHFRatio
		<< m_EAX.flDecayTime
		<< m_EAX.flEnvironmentDiffusion
		<< m_EAX.flEnvironmentSize
		<< m_EAX.flReflectionsDelay
		<< m_EAX.flReverbDelay
		<< m_EAX.flRoomRolloffFactor
		<< (Int32)m_EAX.lReflections
		<< (Int32)m_EAX.lReverb
		<< (Int32)m_EAX.lRoom
		<< (Int32)m_EAX.lRoomHF;

	return True;
}

Bool SndListener::readFromFile(InStream &is)
{
    SceneObject::readFromFile(is);

    is  >> m_gain
		>> m_velocity;

	Bool current;
    is >> current;

	if (current)
        ((Audio*)getScene()->getAudio())->setActiveListener(this);

	// eax settings
	UInt32 uintulong;
	Int32 intlong;

    is >> uintulong; m_EAX.dwEnvironment = (UInt32)uintulong;
    is >> uintulong; m_EAX.dwFlags       = (UInt32)uintulong;

    is  >> m_EAX.flAirAbsorptionHF
		>> m_EAX.flDecayHFRatio
		>> m_EAX.flDecayTime
		>> m_EAX.flEnvironmentDiffusion
		>> m_EAX.flEnvironmentSize
		>> m_EAX.flReflectionsDelay
		>> m_EAX.flReverbDelay
		>> m_EAX.flRoomRolloffFactor;

    is >> intlong; m_EAX.lReflections = (Int32)intlong;
    is >> intlong; m_EAX.lReverb      = (Int32)intlong;
    is >> intlong; m_EAX.lRoom        = (Int32)intlong;
    is >> intlong; m_EAX.lRoomHF      = (Int32)intlong;

	m_modifiedEAX = True;
	if (m_EAX.dwEnvironment != EAX_Personalised)
		m_modifiermodel = True;

	return True;
}

