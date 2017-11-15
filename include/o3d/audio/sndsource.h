/**
 * @file sndsource.h
 * @brief 2D or 3D sound source.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-04-13
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SOURCE_H
#define _O3D_SOURCE_H

#include "o3d/engine/scene/sceneobject.h"
#include "o3d/core/templatemanager.h"
#include "o3d/core/vector3.h"
#include "sndbuffer.h"

#include "al.h"
#include "alc.h"

#include "o3d/core/hashset.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

// EAX only supported on WIN32 architecture with Visual C++
#ifdef O3D_EFX
	#include "o3d/core/architecture.h"
	#include "eax.h"
	typedef EAXBUFFERPROPERTIES O3D_EAXBUFFER;
#else
	//! here for have the settings even if we have not EAX available (useful for file IO)
	struct O3D_EAXBUFFER
	{
		long lDirect;                //!< direct path level
		long lDirectHF;              //!< direct path level at high frequencies
		long lRoom;                  //!< room effect level
		long lRoomHF;                //!< room effect level at high frequencies
		float flRoomRolloffFactor;   //!< like DS3D flRolloffFactor but for room effect
		long lObstruction;           //!< main obstruction control (attenuation at high frequencies)
		float flObstructionLFRatio;  //!< obstruction low-frequency level re. main control
		long lOcclusion;             //!< main occlusion control (attenuation at high frequencies)
		float flOcclusionLFRatio;    //!< occlusion low-frequency level re. main control
		float flOcclusionRoomRatio;  //!< occlusion room effect level re. main control
		long lOutsideVolumeHF;       //!< outside sound cone level at high frequencies
		float flAirAbsorptionFactor; //!< multiplies DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF
		unsigned long dwFlags;       //!< modifies the behavior of properties
	};
#endif // O3D_EFX

#include "o3d/core/memorydbg.h"
#include <list>

#define O3D_SOUND_MAX_SOURCES 128 //!< only used for define a max check value (not the real max source)

//---------------------------------------------------------------------------------------
//! @class SndSource
//-------------------------------------------------------------------------------------
//! 2d or 3d sound source.
//---------------------------------------------------------------------------------------
class O3D_API SndSource : public SceneObject
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(SndSource)

	//! Some obstruction materials :
	//! - occlusion work when the source in a separate room than the listener
	//! - obstruction work when the source in a the same source than the listener
	enum EAX_MaterialModels
	{
		EAX_OcclusionSingleWindow,
		EAX_OcclusionDoubleWindow,
		EAX_OcclusionThinDoor,
		EAX_OcclusionThickDoor,
		EAX_OcclusionWoodWall,
		EAX_OcclusionBrickWall,
		EAX_OcclusionStoneWall,
		EAX_OcclusionCurtain,
		EAX_ObstructionSingleWindow,
		EAX_ObstructionDoubleWindow,
		EAX_ObstructionThinDoor,
		EAX_ObstructionThickDoor,
		EAX_ObstructionWoodWall,
		EAX_ObstructionBrickWall,
		EAX_ObstructionStoneWall,
		EAX_ObstructionCurtain,
		EAX_MatNumber
	};

	enum EAX_AutoComputeFlags
	{
		EAX_DirectHFAuto,           //!< affects DSPROPERTY_EAXBUFFER_DIRECTHF
		EAX_RoomAuto,               //!< affects DSPROPERTY_EAXBUFFER_ROOM
        EAX_RoomHFAutdo             //!< affects DSPROPERTY_EAXBUFFER_ROOMHF
	};

	//! Default constructor
	SndSource(BaseObject *parent);

	//! Destructor
	virtual ~SndSource();


	//! create the m_sourceId (OpenAL source id) if it can take a source ID from the manager.
	//! Generally you don't need to call it, but if you want to get an ID in priority this can
	//! be a good way for.
	Bool create();

	//! Destruct the OpenAL source object.
	void destroy();


	//-----------------------------------------------------------------------------------
	//! @note Buffers settings
	//!
	//! You can AddBufferQueue when playing but you can't remove a source from
	//! playing buffer queue. If you try to remove a buffer when playing, the update
	//! will be available only after you stop and play again the source.
	//!
	//! The buffer queue is generally used for play streamed data. If you use a streamed
	//! buffer buffer queue doesn't works.
	//-----------------------------------------------------------------------------------

	//! Set the unique buffer
	//! the back buffer is automatically created for streamed buffer, it is transparent
	//! for you, such as a single buffer.
	void setUniqueBuffer(SndBuffer *buffer);
	//! add a buffer to the back of the list
	void addBufferQueue(SndBuffer *buffer);
	//! remove the back buffer of the list
	void removeBufferQueue();
	//! remove a buffer from the list
	void removeBuffer(SndBuffer *bufsfer);
	//! remove all buffer from the queue
	void removeAllBuffer();
	//! set a list of buffer (copy bufferList)
	void setBufferList(const T_BufferList &bufferList);

	//! Auto release played buffer from the queue, doesn't work with loop enable
	//! the source is stop once all buffers are played and the queue is cleared at stop
	//! @note disable loop mode when you enable the auto-release.
	//! work only if you call update(); which is auto called by the hierarchy tree
	inline void setAutoReleaseBufferList(Bool autoRelease)
	{
		m_autoRelease = autoRelease;
		if (m_autoRelease)
			setLooping(False);
	}

	//! Get the number of buffers used by this source.
	inline Int32 getNumBuffers() const { return (Int32)m_buffersQueue.size(); }
	//! Get the number of played buffer.
	UInt32 getNumPlayedBuffers() const;
	//! Get the number of buffer to process.
	inline UInt32 getNumBufferToPlay() const { return getNumBuffers() - getNumPlayedBuffers(); }
	//! Is a single's buffer source.
	inline Bool isUniqueBuffer() const
	{
		return (Int32)m_buffersQueue.size() == 1 ? True : False;
	}


	//-----------------------------------------------------------------------------------
	// Source playing
	//-----------------------------------------------------------------------------------

	//! play the source
	void play();
	//! pause playing
	void pause();
	//! stop playing
	void stop();
	//! rewind the source
	void rewind();
	//! is the source playing
	inline Bool isPlaying() { return m_playing; }

	//! Set if auto start playing the source when buffer are ready or income.
	inline void setAutoPlay(Bool autoplay) { m_autoPlay = autoplay; }
	//! Is auto start playing the source when buffer are ready or income.
	inline Bool isAutoPlay()const { return m_autoPlay; }


	//-----------------------------------------------------------------------------------
	//! @note source immediate fade (work only when playing)
	//! Fading settings are put to 0 once the fade has been finished
	//! NOTE: work only if you call update(); Auto called by the hierarchy tree
	//! The is make to the min/max gain of the source. Then for a complete fade out
	//! you need to set mingain to 0.
	//-----------------------------------------------------------------------------------

	//! Disable the fading
	inline void noFade() { m_fadeInOut = 0; m_fadeScale = 0.0f; }

	//! Define a fade in depending on a given fade gain per second
	inline void fadeIn(Float fadegain)
	{
		m_fadeInOut = -1;
		m_fadeScale = fadegain;
	}

	//! Define a fade out depending on a given fade gain per second
	inline void fadeOut(Float fadegain)
	{
		m_fadeInOut =  1;
		m_fadeScale = fadegain;
	}

	//! Is fade in is in process
	inline Bool isFadeIn()const { return (m_fadeInOut == -1); }
	//! Is fade out is in process
	inline Bool isFadeOut()const { return (m_fadeInOut == 1); }
	//! Is no fade processing
	inline Bool isFade()const { return (m_fadeInOut != 0); }


	//-----------------------------------------------------------------------------------
	// source properties
	//-----------------------------------------------------------------------------------

	//! change/renvoie le pitch
	void setPitch(Float Pitch);
	inline Float getPitch()const { return m_pitch; }
	//! change/renvoie le gain
	void setGain(Float Gain);
	inline Float getGain()const { return m_gain; }
	//! change/renvoie la distance maximum
	void setMaxDistance(Float MaxDistance);
	inline Float getMaxDistance()const { return m_maxDistance; }
	//! change/renvoie la facteur de rolloff
	void setRollOff(Float RollOff);
	inline Float getRollOff()const { return m_rollOff; }
	//! change/renvoie la distance de référence
	void setRefDistance(Float RefDistance);
	inline Float getRefDistance()const { return m_refDistance; }
	//! change/renvoie le gain minimum
	void setMinGain(Float MinGain);
	inline Float getMinGain()const { return m_minGain; }
	//! change/renvoie le gain maximum
	void setMaxGain(Float MaxGain);
	inline Float getMaxGain()const { return m_maxGain; }
	//! change/renvoie la position si relative ou non
	void setRelative(Bool SourceRelative);
	inline Bool getRelative()const { return m_relative; }
	//! change/renvoie le looping
	Bool enableLooping();
	Bool disableLooping();
	inline Bool setLooping(Bool looping) { if (looping) return enableLooping(); return disableLooping(); }
	inline Bool toggleLooping() { if (m_looping) return disableLooping(); return enableLooping(); }
	inline Bool isLooping()const { return m_looping; }
	inline Bool getLooping()const { return m_looping; }
	//! set/get the reverb delay
	void setReverbDelay(Float delay);
	inline Float getReverbDelay()const { return m_reverbDelay; }
	//! set/get the reverb ratio
	void setReverbRatio(Float ratio);
	inline Float getReverbRatio()const { return m_reverbScale; }

	//! compute the source gain (useless for determine the source playing priority)
	Float computeGain();

	//-----------------------------------------------------------------------------------
	// Transformation parameters
	//-----------------------------------------------------------------------------------

	//! set velocity
	void setVelocity(Vector3& vec);
	//! set velocity
	void setVelocity(Float x,Float y,Float z);
	//! get velocity
	inline Vector3 getVelocity()const { return m_velocity; }

	virtual void setUpModelView();

	//! Send OpenAL source settgins.
	virtual void put();

	//! Update the source position and direction.
	virtual void update();

	//! Draw a symbolic representation of the source.
	virtual void draw(const DrawInfo &drawInfo);


	//-----------------------------------------------------------------------------------
	// EAX properties
	//-----------------------------------------------------------------------------------

	//! set the default EAX setting for the source
	void setDefaultEAX();

	//! set a material model for the source
	void setMaterialModel(EAX_MaterialModels model);

	//! set/get the direct path level
	inline void setDirectPathLvl(Int32 l)
	{
		#ifdef O3D_EFX
		l = o3d::clamp(l,EAXBUFFER_MINDIRECT,EAXBUFFER_MAXDIRECT);
		#else
		l = o3d::clamp(l,-10000,1000);
		#endif

		m_EAX.lDirect = (Int32)l;
		m_modifiedEAX = True;
	}
	inline Int32 getDirectPathLvl()const { return (Int32)m_EAX.lDirect; }

	//! set/get the direct path level at high frequencies
	inline void setDirectPathLvlHF(Int32 l)
	{
		#ifdef O3D_EFX
		l = o3d::clamp(l,EAXBUFFER_MINDIRECTHF,EAXBUFFER_MAXDIRECTHF);
		#else
		l = o3d::clamp(l,-10000,0);
		#endif

		m_EAX.lDirectHF = (Int32)l;
		m_modifiedEAX = True;
	}
	inline Int32 getDirectPathLvlHF()const { return (Int32)m_EAX.lDirectHF; }

	//! set/get the room effect level for the source
	inline void setRoomEffect(Int32 l)
	{
		#ifdef O3D_EFX
		l = o3d::clamp(l,EAXBUFFER_MINROOM,EAXBUFFER_MAXROOM);
		#else
		l = o3d::clamp(l,-10000,1000);
		#endif

		m_EAX.lRoom = (Int32)l;
		m_modifiedEAX = True;
	}
	inline Int32 getRoomEffect()const { return (Int32)m_EAX.lRoom; }

	//! set/get the room effect level for the source at high frequencies
	inline void setRoomEffectHF(Int32 l)
	{
		#ifdef O3D_EFX
		l = o3d::clamp(l,EAXBUFFER_MINROOMHF,EAXBUFFER_MAXROOMHF);
		#else
		l = o3d::clamp(l,-10000,0);
		#endif

		m_EAX.lRoomHF = (Int32)l;
		m_modifiedEAX = True;
	}
	inline Int32 getRoomEffectHF()const { return (Int32)m_EAX.lRoomHF; }

	//! set/get the rolloff factor (like DS3D flRolloffFactor but for room effect)
	inline void setRolloffFactor(Float f)
	{
		#ifdef O3D_EFX
		f = o3d::clamp(f,EAXBUFFER_MINROOMROLLOFFFACTOR,EAXBUFFER_MAXROOMROLLOFFFACTOR);
		#else
		f = o3d::clamp(f,0.0f,10.0f);
		#endif

		m_EAX.flRoomRolloffFactor = f;
		m_modifiedEAX = True;
	}
	inline Float getRolloffFactor()const { return m_EAX.flRoomRolloffFactor; }

	//! set/get the main obstruction control (attenuation at high frequencies)
	inline void setObstruction(Int32 l)
	{
		#ifdef O3D_EFX
		l = o3d::clamp(l,EAXBUFFER_MINOBSTRUCTION,EAXBUFFER_MAXOBSTRUCTION);
		#else
		l = o3d::clamp(l,-10000,0);
		#endif

		m_EAX.lObstruction = (Int32)l;
		m_modifiedEAX = True;
	}
	inline Int32 getObstruction()const { return (Int32)m_EAX.lObstruction; }

	//! set/get the obstruction low-frequency level re. main control
	inline void setObstructionLFRatio(Float f)
	{
		#ifdef O3D_EFX
		f = o3d::clamp(f,EAXBUFFER_MINOBSTRUCTIONLFRATIO,EAXBUFFER_MAXOBSTRUCTIONLFRATIO);
		#else
		f = o3d::clamp(f,0.0f,1.0f);
		#endif

		m_EAX.flObstructionLFRatio = f;
		m_modifiedEAX = True;
	}
	inline Float getObstructionLFRatio()const { return m_EAX.flObstructionLFRatio; }

	//! set/get the main occlusion control (attenuation at high frequencies)
	inline void setOcclusion(Int32 l)
	{
		#ifdef O3D_EFX
		l = o3d::clamp(l,EAXBUFFER_MINOCCLUSION,EAXBUFFER_MAXOCCLUSION);
		#else
		l = o3d::clamp(l,-10000,0);
		#endif

		m_EAX.lOcclusion = (Int32)l;
		m_modifiedEAX = True;
	}
	inline Int32 getOcclusion()const { return (Int32)m_EAX.lOcclusion; }

	//! set/get the occlusion low-frequency level re. main control
	inline void setOcclusionLFRatio(Float f)
	{
		#ifdef O3D_EFX
		f = o3d::clamp(f,EAXBUFFER_MINOCCLUSIONLFRATIO,EAXBUFFER_MAXOCCLUSIONLFRATIO);
		#else
		f = o3d::clamp(f,0.0f,1.0f);
		#endif

		m_EAX.flOcclusionLFRatio = f;
		m_modifiedEAX = True;
	}
	inline Float getOcclusionLFRatio()const { return m_EAX.flOcclusionLFRatio; }

	//! set/get the occlusion room effect level re. main control
	inline void setOcclusionRoomRatio(Float f)
	{
		#ifdef O3D_EFX
		f = o3d::clamp(f,EAXBUFFER_MINOCCLUSIONROOMRATIO,EAXBUFFER_MAXOCCLUSIONROOMRATIO);
		#else
		f = o3d::clamp(f,0.0f,10.0f);
		#endif

		m_EAX.flOcclusionRoomRatio = f;
		m_modifiedEAX = True;
	}
	inline Float getOcclusionRoomRatio()const { return m_EAX.flOcclusionRoomRatio; }

	//! set/get the outside sound cone level at high frequencies
	inline void setOutsideVolumeHF(Int32 l)
	{
		#ifdef O3D_EFX
		l = o3d::clamp(l,EAXBUFFER_MINOUTSIDEVOLUMEHF,EAXBUFFER_MAXOUTSIDEVOLUMEHF);
		#else
		l = o3d::clamp(l,-10000,0);
		#endif

		m_EAX.lOutsideVolumeHF = (Int32)l;
		m_modifiedEAX = True;
	}
	inline Int32 getOutsideVolumeHF()const { return (Int32)m_EAX.lOutsideVolumeHF; }

	//! set/get the DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF scale
	inline void setAirAbsorptionFactor(Float f)
	{
		#ifdef O3D_EFX
		f = o3d::clamp(f,EAXBUFFER_MINAIRABSORPTIONFACTOR,EAXBUFFER_MAXAIRABSORPTIONFACTOR);
		#else
		f = o3d::clamp(f,0.0f,10.0f);
		#endif

		m_EAX.flAirAbsorptionFactor = f;
		m_modifiedEAX = True;
	}
	inline Float getAirAbsorptionFactor()const { return m_EAX.flAirAbsorptionFactor; }

	//! Set these flags to compute automatically some properties
	void setAutoComputeFlag(EAX_AutoComputeFlags flag,Bool b);
	//! Is auto compute flag
	Bool getAutoComputeFlag(EAX_AutoComputeFlags flag)const;

	//! force update EAX (used by the listener manager)
	inline void forceUpdateEAX() { m_modifiedEAX = True; }

	// Serialization
    virtual Bool writeToFile(OutStream &os);
    virtual Bool readFromFile(InStream &is);

	//! nothing for post import pass
	virtual void postImportPass() {}

    //
    // Signals
    //

    //! signal on fade out complete
    Signal<> onFadeOutComplete{this};

protected:

	UInt32 m_sourceId;    //!< OpenAL source identifier.

	Float m_pitch;        //!< pitch shift [50% = 12d half-tones = 1 octaves] (default:1.0).
	Float m_gain;         //!< global gain [0.5 = 6dB] (default:1.0).

	Float m_refDistance;  //!< used to compute the attenuation according to the distance (@see m_rollOff) (default:1.0).
	Float m_rollOff;      //!< attenuation factor (default:1.0).
	Float m_maxDistance;  //!< clipping distance, default is o3d::limits<Float>::max().

	Float m_minGain;      //!< minimal sound gain (for attenuation) (default:0.0).
	Float m_maxGain;      //!< maximal sound gain (for attenuation) (default:1.0).

	Float m_reverbDelay;  //!< the reverb delay in ms.
	Float m_reverbScale;  //!< the reverb feedback ratio.

	Bool m_looping;       //!< Play in loop if TRUE, default is FALSE.

	Bool m_playing;       //!< TRUE if the source is playing.

	Bool m_relative;      //!< if TRUE coordinates are relative to the listener (constant distance), default is FALSE.

	Vector3 m_velocity;    //!< sound velocity for doppler effect (default:{0,0,0})

	T_SmartBufferList m_buffersQueue; //!< list of buffers to play (one at time)

	UInt32 m_position;    //!< Position of the next chunk of sound buffer to ready

	Bool m_autoPlay;      //!< auto play the source when the sound is loaded
	Bool m_autoRelease;   //!< auto pop the buffer list when playing

	Bool m_streaming;     //!< are this source play a streamed buffer

	UInt64 m_lastTime;    //!< time counter for fading
	Float m_fadeScale;    //!< fade in/out speed (in gain per second [0.5 = 6dB])
	Int32 m_fadeInOut;    //!< 0 for no fade, 1 for fade out, -1 for fade in

	O3D_EAXBUFFER m_EAX;      //!< EAX buffer(source) settings
	Bool m_modifiedEAX;   //!< is an EAX setting have been modified
	EAX_MaterialModels m_materialEAX;

	//! Called when a sound buffer is ready to be played.
    void soundBufferValid(SndBuffer* sndBuffer);

	//! compute the fading
	void computeFading();
	//! update auto release
	void updateAutoRelease();
	//! update buffer stream
	void updateStream();
};


//---------------------------------------------------------------------------------------
//! @class OmniSource
//-------------------------------------------------------------------------------------
//! Omni-directional sound source (no direction). This can be used for 3d sound or music.
//---------------------------------------------------------------------------------------
class O3D_API OmniSource : public SndSource
{
public:

	O3D_DECLARE_CLASS(OmniSource)

	//! Default constructor
	OmniSource(BaseObject *parent);

	//! Get the drawing type
	virtual UInt32 getDrawType() const;
};


//---------------------------------------------------------------------------------------
//! @class DirectSource
//-------------------------------------------------------------------------------------
//! Directional sound source with cutoff.
//---------------------------------------------------------------------------------------
class O3D_API DirectSource : public SndSource
{
public:

	O3D_DECLARE_CLASS(DirectSource)

	//! Default constructor
	DirectSource(BaseObject *parent);

	//! Destructor
	virtual ~DirectSource() {}

	//! Get the drawing type
	virtual UInt32 getDrawType() const;


	//-----------------------------------------------------------------------------------
	// Source parameters
	//-----------------------------------------------------------------------------------

	//! Change the cone outer gain.
	void setConeOuterGain(Float coneOuterGain);
	//! Get the cone outer gain.
	inline Float getConeOuterGain() const { return m_coneOuterGain; }

	//! Change the cone inner angle of the source,
	void setConeInnerAngle(Float coneInnerAngle);
	//! Get the cone inner angle of the source.
	inline Float getConeInnerAngler() const { return m_coneInnerAngle; }

	//! Change the cone outer angle of the source, used to compute the attenuation.
	void setConeOuterAngle(Float coneOuterAngle);
	//! Get the cone outer angle of the source, used to compute the attenuation.
	inline Float getConeOuterAngler() const { return m_coneOuterAngle; }


	//-----------------------------------------------------------------------------------
	// Transformation settings
	//-----------------------------------------------------------------------------------

	//! Send OpenAL source settings.
	virtual void put();

	//! Update the source position and direction.
	virtual void update();

	//! Draw a symbolic representation of the source.
	virtual void draw(const DrawInfo &drawInfo);

	// serialization
    virtual Bool writeToFile(OutStream &os);
    virtual Bool readFromFile(InStream &is);

protected:

	Float m_coneInnerAngle;	//!< emissive inner angle (default:360° for omni-directional)
	Float m_coneOuterAngle;	//!< emissive outer angle (default:360° for omni-directional)

	Float m_coneOuterGain;	//!< multiplied by gain to determine the effective gain at the outer angle.
};

//---------------------------------------------------------------------------------------
//! @class SndSourceManager
//---------------------------------------------------------------------------------------
//! Manage sound source.
//---------------------------------------------------------------------------------------
class O3D_API SndSourceManager : public SceneEntity, NonCopyable<>
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(SndSourceManager)

	//! constructor
	SndSourceManager(BaseObject *parent);

	//! get the number maximal of source that can play OpenAL at the same time
	//! this information is relative to the others running application that use hardware buffers
	inline UInt32 getMaxSource() const { return m_maxSources; }

	//! get a source id, depend on source gain, ref-distance and position and current world position
	//! for compute the priority and then return or not an id (0 for no source available)
	//! by this way relatives sources are always priority
	UInt32 getSourceID(SndSource *source);

	//---------------------------------------------------------------------------------------
	// Factory
	//---------------------------------------------------------------------------------------

	//! Register a source
	void registerSource(SndSource *source);

	//! Unregister a source
	void unRegisterSource(SndSource *source);

protected:

	typedef stdext::hash_set<SndSource*> T_SourceSet;
	typedef T_SourceSet::iterator IT_SourceSet;

	UInt32 m_maxSources;
	T_SourceSet m_registredSources;
};

} // namespace o3d

#endif // _O3D_SOURCE_H

