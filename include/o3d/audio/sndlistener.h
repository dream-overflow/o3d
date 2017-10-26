/**
 * @file sndlistener.h
 * @brief position and orientation.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2003-04-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_LISTENER_H
#define _O3D_LISTENER_H

#include "o3d/engine/scene/sceneobject.h"
#include "o3d/core/smartcounter.h"
#include "o3d/core/templatemanager.h"
#include "o3d/core/vector3.h"
#include "o3d/core/memorydbg.h"

#include "al.h"
#include "alc.h"

namespace o3d {

#ifdef O3D_EFX
	#include "o3d/core/architecture.h"
	#include "eax.h"
	typedef EAXLISTENERPROPERTIES O3D_EAXLISTENER;
#else
	//! here for have the settings even if we have not EAX available (useful for file IO)
	struct O3D_EAXLISTENER
	{
		long lRoom;                    //!< room effect level at low frequencies
		long lRoomHF;                  //!< room effect high-frequency level re. low frequency level
		float flRoomRolloffFactor;     //!< like DS3D flRolloffFactor but for room effect
		float flDecayTime;             //!< reverberation decay time at low frequencies
		float flDecayHFRatio;          //!< high-frequency to low-frequency decay time ratio
		long lReflections;             //!< early reflections level relative to room effect
		float flReflectionsDelay;      //!< initial reflection delay time
		long lReverb;                  //!< late reverberation level relative to room effect
		float flReverbDelay;           //!< late reverberation delay time relative to initial reflection
		unsigned long dwEnvironment;   //!< sets all listener properties
		float flEnvironmentSize;       //!< environment size in meters
		float flEnvironmentDiffusion;  //!< environment diffusion
		float flAirAbsorptionHF;       //!< change in level per meter at 5 kHz
		unsigned long dwFlags;         //!< modifies the behavior of properties
	};
#endif // O3D_EFX

//---------------------------------------------------------------------------------------
//! @class SndListener
//-------------------------------------------------------------------------------------
//! A sound listener. Like a scene camera. It is defined into a node, and listen at its
//! position and orientation.
//---------------------------------------------------------------------------------------
class O3D_API SndListener : public SceneObject
{
public:

	O3D_DECLARE_CLASS(SndListener)

	//! eax listener environnement models
	enum O3DEAX_EnvModel
	{
		EAX_Generic,
		EAX_PaddedCell,
		EAX_Room,
		EAX_BathRoom,
		EAX_LivingRoom,
		EAX_StoneRoom,
		EAX_Auditorium,
		EAX_ConcertHall,
		EAX_Cave,
		EAX_Arena,
		EAX_Hangar,
		EAX_CarpetedHallway,
		EAX_Hallway,
		EAX_StoneCorridor,
		EAX_Alley,
		EAX_Forest,
		EAX_City,
		EAX_Mountains,
		EAX_Quarry,
		EAX_Plain,
		EAX_ParkingLot,
		EAX_SewerPipe,
		EAX_UnderWater,
		EAX_Drugged,
		EAX_Dizzy,
		EAX_Psychotic,
		EAX_EnvironnementCount,
		EAX_Personalised
	};

	enum O3DEAX_EnvSizeFlags
	{
        EAX_DecayTime,           //!< reverberation decay time
		EAX_Reflection,          //!< reflection level
		EAX_ReflectionDelay,     //!< initial reflection delay time
		EAX_Reverb,              //!< reflections level
		EAX_ReverbDelay,         //!< late reverberation delay time
		EAX_DecayLimit           //!< limits high-frequency decay time according to air absorption
	};

	//! Default constructor
	SndListener(BaseObject *parent);

	//! Destructor
	virtual ~SndListener();

	//! Get the drawing type
	virtual UInt32 getDrawType() const;


	//! Set the gain
	inline void setGain(Float gain) { m_gain = gain; }
	//! Get the gain
	inline Float getGain() const { return m_gain; }

	//! Mute the listener
	inline void mute() { if (m_oldGain == -1.f) { m_oldGain = m_gain; m_gain = 0.f; } }

	//! UnMute the last mute
	inline void unMute() { if (m_oldGain != -1.f) { m_gain = m_oldGain; m_oldGain = -1.f; } }

	//! Check if the listener has been muted
	inline Bool hasMute() const { return (m_oldGain != -1.f); }

	//! Set the velocity
	inline void setVelocity(Vector3& vec) { m_velocity = vec; }
	//! Set the velocity
	inline void SetVelocity(Float x,Float y,Float z) { m_velocity.set(x,y,z); }
	//! Get the velocity
	inline Vector3 getVelocity()const { return m_velocity; }


	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	virtual void setUpModelView();

	//! Send OpenAL listener setting.
	virtual void put();

	//! Update listener position and direction.
	virtual void update();

	//! Draw a symbolic representation of the listener.
	virtual void draw(const DrawInfo &drawInfo);


	//-----------------------------------------------------------------------------------
	// EAX properties
	//-----------------------------------------------------------------------------------

	//! set to generic default environnement
	void setGenericEnv();
	//! set an environnement model
	void setEnvModel(O3DEAX_EnvModel envmodel);
	//! get the used environnement model
	inline UInt32 getEnvModel()const { return m_EAX.dwEnvironment; }

	//! set/get the room effect at low frequencies
	inline void setRoomEffectLF(Int32 l)
	{
		#ifdef O3D_EFX
		l = o3d::clamp(l,EAXLISTENER_MINROOM,EAXLISTENER_MAXROOM);
		#else
		l = o3d::clamp(l,-10000,0);
		#endif

		m_EAX.lRoom = (Int32)l;
		m_modifiedEAX = True;
		m_EAX.dwEnvironment = (Int32)EAX_Personalised;
	}
	inline Int32 getRoomEffectLF()const { return (Int32)m_EAX.lRoom; }

	//! set/get the room effect high-frequency level re. low frequency level
	inline void setRoomEffectLvlHF(Int32 l)
	{
		#ifdef O3D_EFX
		l = o3d::clamp(l,EAXLISTENER_MINROOMHF,EAXLISTENER_MAXROOMHF);
		#else
		l = o3d::clamp(l,-10000,0);
		#endif

		m_EAX.lRoomHF = (Int32)l;
		m_modifiedEAX = True;
		m_EAX.dwEnvironment = (Int32)EAX_Personalised;
	}
	inline Int32 getRoomEffectLvlHF()const { return (Int32)m_EAX.lRoomHF; }

	//! set/get the room rolloff factor (like DS3D flRolloffFactor but for room effect)
	inline void setRoomRolloffFactor(Float f)
	{
		#ifdef O3D_EFX
		f = o3d::clamp(f,EAXLISTENER_MINROOMROLLOFFFACTOR,EAXLISTENER_MAXROOMROLLOFFFACTOR);
		#else
		f = o3d::clamp(f,0.0f,10.0f);
		#endif

		m_EAX.flRoomRolloffFactor = f;
		m_modifiedEAX = True;
		m_EAX.dwEnvironment = (Int32)EAX_Personalised;
	}
	inline Float getRoomRolloffFactor()const { return m_EAX.flRoomRolloffFactor; }

	//! set/get the reverberation decay time at low frequencies
	inline void setDecayTime(Float f)
	{
		#ifdef O3D_EFX
		f = o3d::clamp(f,EAXLISTENER_MINDECAYTIME,EAXLISTENER_MAXDECAYTIME);
		#else
		f = o3d::clamp(f,0.1f,20.f);
		#endif

		m_EAX.flDecayTime = f;
		m_modifiedEAX = True;
		m_EAX.dwEnvironment = (Int32)EAX_Personalised;
	}
	inline Float getDecayTime()const { return m_EAX.flDecayTime; }

	//! set/get the high-frequency to low-frequency decay time ratio
	inline void setDecayHFRatio(Float f)
	{
		#ifdef O3D_EFX
		f = o3d::clamp(f,EAXLISTENER_MINDECAYHFRATIO,EAXLISTENER_MAXDECAYHFRATIO);
		#else
		f = o3d::clamp(f,0.1f,2.0f);
		#endif

		m_EAX.flDecayHFRatio = f;
		m_modifiedEAX = True;
		m_EAX.dwEnvironment = (Int32)EAX_Personalised;
	}
	inline Float getDecayHFRatio()const { return m_EAX.flDecayHFRatio; }

	//! set/get the early reflections level relative to room effect
	inline void setReflections(Int32 l)
	{
		#ifdef O3D_EFX
		l = o3d::clamp(l,EAXLISTENER_MINREFLECTIONS,EAXLISTENER_MAXREFLECTIONS);
		#else
		l = o3d::clamp(l,-10000,1000);
		#endif

		m_EAX.lReflections = (Int32)l;
		m_modifiedEAX = True;
		m_EAX.dwEnvironment = (Int32)EAX_Personalised;
	}
	inline Int32 getReflections()const { return (Int32)m_EAX.lReflections; }

	//! set/get the initial reflection delay time
	inline void setReflectionsDelay(Float f)
	{
		#ifdef O3D_EFX
		f = o3d::clamp(f,EAXLISTENER_MINREFLECTIONSDELAY,EAXLISTENER_MAXREFLECTIONSDELAY);
		#else
		f = o3d::clamp(f,0.0f,0.3f);
		#endif

		m_EAX.flReflectionsDelay = f;
		m_modifiedEAX = True;
		m_EAX.dwEnvironment = (Int32)EAX_Personalised;
	}
	inline Float getReflectionsDelay()const { return m_EAX.flReflectionsDelay; }

	//! set/get the late reverberation level relative to room effect
	inline void setReverb(Int32 l)
	{
		#ifdef O3D_EFX
		l = o3d::clamp(l,EAXLISTENER_MINREVERB,EAXLISTENER_MAXREVERB);
		#else
		l = o3d::clamp(l,-10000,2000);
		#endif

		m_EAX.lReverb = (Int32)l;
		m_modifiedEAX = True;
		m_EAX.dwEnvironment = (Int32)EAX_Personalised;
	}
	inline Int32 getReverb()const { return (Int32)m_EAX.lReverb; }

	//! set/get the late reverberation delay time relative to initial reflection
	inline void setReverbDelay(Float f)
	{
		#ifdef O3D_EFX
		f = o3d::clamp(f,EAXLISTENER_MINREVERBDELAY,EAXLISTENER_MAXREVERBDELAY);
		#else
		f = o3d::clamp(f,0.0f,0.1f);
		#endif

		m_EAX.flReverbDelay = f;
		m_modifiedEAX = True;
		m_EAX.dwEnvironment = (Int32)EAX_Personalised;
	}
	inline Float getReverbDelay()const { return m_EAX.flReverbDelay; }

	//! set/get the environment size in meters
	inline void setEnvironmentSize(Float f)
	{
		#ifdef O3D_EFX
		f = o3d::clamp(f,EAXLISTENER_MINENVIRONMENTSIZE,EAXLISTENER_MAXENVIRONMENTSIZE);
		#else
		f = o3d::clamp(f,1.0f,100.0f);
		#endif

		m_EAX.flEnvironmentSize = f;
		m_modifiedEAX = True;
		m_EAX.dwEnvironment = (Int32)EAX_Personalised;
	}
	inline Float getEnvironmentSize()const { return m_EAX.flEnvironmentSize; }

	//! set/get the environment diffusion
	inline void setEnvironmentDiffusion(Float f)
	{
		#ifdef O3D_EFX
		f = o3d::clamp(f,EAXLISTENER_MINENVIRONMENTDIFFUSION,EAXLISTENER_MAXENVIRONMENTDIFFUSION);
		#else
		f = o3d::clamp(f,0.0f,1.0f);
		#endif

		m_EAX.flEnvironmentDiffusion = f;
		m_modifiedEAX = True;
		m_EAX.dwEnvironment = (Int32)EAX_Personalised;
	}
	inline Float getEnvironmentDiffusion()const { return m_EAX.flEnvironmentDiffusion; }

	//! set/get the change in level per meter at 5 kHz
	inline void setAirAbsorptionHF(Float f)
	{
		#ifdef O3D_EFX
		f = o3d::clamp(f,EAXLISTENER_MINAIRABSORPTIONHF,EAXLISTENER_MAXAIRABSORPTIONHF);
		#else
		f = o3d::clamp(f,-100.f,0.0f);
		#endif

		m_EAX.flAirAbsorptionHF = f;
		m_modifiedEAX = True;
		m_EAX.dwEnvironment = (Int32)EAX_Personalised;
	}
	inline Float getAirAbsorptionHF()const { return m_EAX.flAirAbsorptionHF; }

	//! Set these flags to determine what properties are affected by environment size
	void setEnvSizeFlag(O3DEAX_EnvSizeFlags flag,Bool b);
	//! Get flags state
	Bool getEnvSizeFlag(O3DEAX_EnvSizeFlags flag)const;

	//! force update EAX (used by the listener manager)
	inline void forceUpdateEAX() { m_modifiedEAX = True; }


	// serialisation
	Bool writeToFile(OutStream &os);
	Bool readFromFile(InStream &is);

	virtual void postImportPass() {}

protected:

	Float m_gain;		//!< Source gain [0.5 for 6bD] (default is 1.0).

	Vector3 m_velocity;	    //!< Velocity for Doppler effect.

	Vector3 m_orientationAt;	//!< Listener direction (Z+ vector) (default is {0,0,-1.0}).
	Vector3 m_orientationUp;	//!< Listener direction (Y+ vector) (default is {0,1.0,0}).

	O3D_EAXLISTENER m_EAX;      //!< EAX listener properties.
	Bool m_modifiermodel;
	Bool m_modifiedEAX;     //!< Is an EAX setting have been modified.

	Float m_oldGain;        //!< Old gain factor for mute/unmute feature.
};

} // namespace o3d

#endif // _O3D_LISTENER_H

