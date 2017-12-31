/**
 * @file audio.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_AUDIO_H
#define _O3D_AUDIO_H

#include "o3d/engine/scene/sceneentity.h"
#include "o3d/engine/audiomanager.h"

#include "al.h"
#include "alc.h"

namespace o3d {

// EFX support
#ifdef O3D_EFX
	#include "o3d/core/architecture.h"
	#include <eax.h>
#endif // O3D_EFX

#include "o3d/core/memorydbg.h"

class SndListener;
class SndBufferManager;
class SndSourceManager;
class AudioRenderer;

/**
 * @brief Audio manager based on OpenAL library (www.openal.org).
 * It contains the sound buffers, control the available hardware sound source and
 * the current listener. It also control globals OpenAL parameters.
 * @date 2003-04-15
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class O3D_API Audio : public SceneEntity, public AudioManager
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(Audio)

	//! OpenAL distance model.
	enum DistanceModel
	{
		DISTMODEL_NONE = 0,         //!< No distance model.
		DISTMODEL_INVERSE,          //!< Default distance model.
		DISTMODEL_INVERSE_CLAMP,    //!< Inverse and clamped.
		DISTMODEL_LINEAR,
		DISTMODEL_LINEAR_CLAMP,
		DISTMODEL_EXPONENT,
		DISTMODEL_EXPONENT_CLAMP
	};

	//-----------------------------------------------------------------------------------
	// Setup
	//-----------------------------------------------------------------------------------

    /**
     * @brief Audio
     * @param parent Mostly the Scene.
     * @param rootPath Path where to find data folders.
     * @param renderer Instance of an OpenAL renderer.
     */
    Audio(BaseObject *parent,
            const String &rootPath = "",
            AudioRenderer* renderer = nullptr);

	virtual ~Audio();

    virtual void init() override;
    virtual void release() override;

	//-----------------------------------------------------------------------------------
	// General sound settings
	//-----------------------------------------------------------------------------------

	//! Set the Doppler factor effect (1.0 is default)
	void setDopplerFactor(Float f);
	//! Get the Doppler factor effect (1.0 is default)
	inline Float getDopplerFactor() const { return m_dopplerFactor; }

	//! Set the sound speed propagation (343.3 is default)
	void setSoundSpeed(Float f);
	//! Get the sound speed propagation (343.3 is default)
	inline Float getSoundSpeed() const { return m_soundSpeed; }

    //! set the distance attenuation model (DistanceModel_Inverse is default)
	void setDistanceModel(DistanceModel m);
    //! get the distance attenuation model (DistanceModel_Inverse is default)
	inline DistanceModel getDistanceModel() const { return m_distanceModel; }

	//-----------------------------------------------------------------------------------
	// Variables global access
	//-----------------------------------------------------------------------------------

    //! Set current OpenAL renderer and context.
    void setRenderer(AudioRenderer *renderer);

    //! Get the audio renderer.
    inline       AudioRenderer* getRenderer()       { return m_renderer; }
    //! Get the audio renderer (read only).
    inline const AudioRenderer* getRenderer() const { return m_renderer; }

	//! Define the active listener
	inline void setActiveListener(SndListener* listener) { m_activeListener = listener; }

	//! Get the active listener
	inline SndListener* getActiveListener() { return m_activeListener; }

	//-----------------------------------------------------------------------------------
	// Accessors
	//-----------------------------------------------------------------------------------

	//! Get the sound buffer manager.
	inline SndBufferManager* getBufferManager() { return m_bufferManager; }
	//! Get the sound buffer manager (read only).
	inline const SndBufferManager* getBufferManager() const { return m_bufferManager; }

	//! Get the sound source manager.
	inline SndSourceManager* getSourceManager() { return m_sourceManager; }
	//! Get the sound source manager (read only).
	inline const SndSourceManager* getSourceManager() const { return m_sourceManager; }

	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

    /**
     * @brief update Update managers.
     */
    virtual void update() override;

    /**
     * @brief focus Called when get the focus.
     */
    virtual void focus() override;

    /**
     * @brief lostFocus Call when the focus is lost.
     */
    virtual void lostFocus() override;

	//-----------------------------------------------------------------------------------
	// serialization (processed by Scene)
	//-----------------------------------------------------------------------------------

    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

protected:

    AudioRenderer* m_renderer;      //!< the OpenAL renderer

	DistanceModel m_distanceModel;  //!< used distance model calculation
    Float m_soundSpeed;             //!< the sound propagation
    Float m_dopplerFactor;          //!< Doppler factor

	SndListener* m_activeListener;  //!< Current active listener

	//-----------------------------------------------------------------------------------
	// managers
	//-----------------------------------------------------------------------------------

	SndBufferManager* m_bufferManager;  //!< Sound buffer manager
	SndSourceManager* m_sourceManager;  //!< Sound sources manager
};

} // namespace o3d

#endif // _O3D_AUDIO_H
