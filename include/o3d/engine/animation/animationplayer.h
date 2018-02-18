/**
 * @file animationplayer.h
 * @brief An animation player.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-02-23
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ANIMATIONPLAYER_H
#define _O3D_ANIMATIONPLAYER_H

#include "o3d/core/file.h"
#include "o3d/core/memorydbg.h"

#include "animation.h"
#include "../scene/sceneentity.h"

namespace o3d {

class Node;

//---------------------------------------------------------------------------------------
//! @class AnimationPlayer
//-------------------------------------------------------------------------------------
//! An animation player is used for each node of a hierarchy animation tree.
//! The player control the way how animation is played
//---------------------------------------------------------------------------------------
class O3D_API AnimationPlayer : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(AnimationPlayer)

	//! Animation player playing mode.
	enum PlayerMode
	{
		MODE_PAUSE,
		MODE_STOP,
		MODE_LOOP,
		MODE_PING_PONG,
		MODE_CONTINUE,
		MODE_UNDEFINED
	};

	//! Constructor
	AnimationPlayer(
		BaseObject *parent,
		Animation *animation,
        Animatable *animatable = nullptr,
		Int32 timesToPlay = 1,
		Float fps = 30,
		Animation::BlendMode blendMode = Animation::BLEND_REPLACE,
		Float blendWeight = 1.0f);

	//! Copy constructor
	AnimationPlayer(const AnimationPlayer &dup);

	//! Destructor
	virtual ~AnimationPlayer();

	//! Set the animation
	void setAnimation(Animation *animation);
	//! Get the animation
	inline Animation* getAnimation() { return m_animation.get(); }

	//! Set the animatable
	void setAnimatable(Animatable* animatable);
	//! Get the animatable
	inline Animatable* getAnimatable() { return m_animatable; }

	//! Set the number of time the animation must be played
	inline void setRemaningTime(Int32 nbrtime) { m_remainingTime = nbrtime; }
	//! Get the number of time the animation must be played
	inline Int32 getRemaningTime() const { return m_remainingTime; }

	//! Use an animation range with time computation to find key frames
    void playAnimRange(UInt32 id);

	//! See PlayAnimRange (same with a name)
	inline void playAnimRange(const String &name)
	{
		O3D_ASSERT(m_animation);
		Int32 id;
		if ((id = m_animation->getAnimRangeId(name)) >= 0)
            playAnimRange((UInt32)id);
	}

	//! Full range animation (default played animation is in full range)
	inline void playFullRange()
	{
		O3D_ASSERT(m_animation);
		O3D_ASSERT(m_animatable);

		m_startTime = 0.f; m_endTime = 1.f;
		m_animation->useFullRange(m_animatable);
		m_timeToBreak = 0.f;
		m_time = 0.f;

		m_isBreakable = False;
		m_isActive = True;

        m_range = -1;
	}

    //! Get the currently played or paused animation range (or -1 if full range)
    inline Int32 getAnimRange() const { return m_range; }

    //! Get the currently played or paused animation range (or empty string if full range)
    String getAnimRangeName() const;

	//! Put an animation in queue. wait the current animation ending before start.
	//! The current animation can be broken before the end if it has a special breakable key */
	inline void enqueueAnimRange(UInt32 id, PlayerMode mode)
	{
		Queue queue;
		queue.Mode = mode;
		queue.Range = id;

		m_queueList.push_back(queue);
	}

	//! See EnqueueAnimRange (same with name)
	inline void enqueueAnimRange(const String &name, PlayerMode mode)
	{
		O3D_ASSERT(m_animation);
		Int32 id;
		if ((id = m_animation->getAnimRangeId(name)) != -1)
			enqueueAnimRange(id, mode);
	}


	//! Is the animation finished, then the player should be removed
	Bool isFinished();
	//! Update the animatable [0<t<1]
	void update();

	//! Set nbr of update per sec (in FPS : frame per second)
	inline void setFramePerSec(Float framepersec)
	{
		O3D_ASSERT(m_animation);
		m_frameSpeed = 1.f / framepersec;
		m_speed = 1.f / m_animation->getDuration();
	}
	//! Get nbr of update per sec (in FPS : frame per second)
    inline Float getFramePerSec() const { return (1.f / m_frameSpeed); }

	//! Set the animation speed in time
	inline void setSpeed(Float speed) { m_speed = speed; }

	//! Get the animation speed in time
	inline Float getSpeed()const { return m_speed; }

	//! Modify the animation speed by increment/decrement value
	inline Float modifySpeed(Float val)
	{
		m_speed += val;
		return m_speed;
	}

	//! Set the animation player mode
	inline void setPlayerMode(PlayerMode mode) { m_playerMode = mode; }
	//! Get the animation player mode
	inline PlayerMode getPlayerMode() const { return m_playerMode; }

	//! Set the playing time
	inline void setTime(Float time) { m_time = time; }
	//! Get the playing time
	inline Float getTime() const { return m_time; }

	//! Play the animation playing
	inline void play() { m_isActive = True; }
	//! Pause the animation playing
	inline void pause() { m_isActive = False; }
	//! Toggle the animation playing
	inline Bool togglePlayPause() { return m_isActive = !m_isActive; }

	//! Restart the animation
	inline void restart() { m_time = m_startTime; }

	//! Stop and go for delete the animation player.
	//! If the animation player manager use it, it will delete this player.
	void destroy();

	//! Add a step to the animation player
    inline Float stepBy(Float val) { m_time += val; return m_time; }

	//! Play the animation step in forward/backward
	inline Float step(Bool forward = True) { if (forward) m_time += m_speed; else m_time -= m_speed; return m_time; }

	//! Play the animation step in forward
	inline Float stepForward() { m_time += m_speed; return m_time; }
	//! Play the animation step in backward
	inline Float stepBackward() { m_time -= m_speed; return m_time; }

	//! Is the animation player is playing
	inline Bool isPlaying()const { return m_isActive; }

	//! Draw the animation trajectory
	void drawTrajectory(Node *curNode);

	// Serialization
    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

public:

	//! Called each time an animation reach the end.
    Signal<> onAnimationFinished{this};

protected:

	struct Queue
	{
		PlayerMode Mode;
		Int32 Range;
	};

	typedef std::list<Queue> T_QueueList;
	typedef T_QueueList::iterator IT_QueueList;
	typedef T_QueueList::const_iterator CIT_QueueList;

	T_QueueList m_queueList;             //!< List of animation in queue.

	SmartObject<Animation> m_animation; //!< the animation that is played

	Float m_frameSpeed;              //!< the animation speed in frame per sec
	Float m_elapsed;                 //!< the time elapsed since the last frame update

	Float m_speed;                   //!< the animation speed
	Float m_time;                    //!< the current time in the animation [0..1]

	Int32   m_elapsedTime;           //!< the number of times the animation has been played
	Int32   m_remainingTime;         //!< the number of time the animation must be played, infinite loop if <0

	Int64  m_lastTime;               //!< the time of the last animation update

	Bool  m_isActive;                //!< play/pause the animation

	PlayerMode m_playerMode;             //!< the animation player behavior (loop,stop,ping-pong...)

	Animatable *m_animatable;            //!< the object that is animated

	Animation::BlendMode m_blendMode;    //!< blending mode used to apply the animation
	Float m_blendWeight;             //!< Blending weight used to apply the animation

	Float m_startTime;               //!< start time of the animation for range
	Float m_endTime;                 //!< end time of the animation for range

	Bool m_changeAnim;               //!< need to reset the animatable key frame struct

	Bool m_isBreakable;              //!< is the current animation is breakable.
	Float m_timeToBreak;             //!< if breakable the time when the break is possible.

    Int32 m_range;                   //!< currently played/paused animation range id.

	//! restricted default constructor
	AnimationPlayer(BaseObject *parent);
};

typedef std::list<AnimationPlayer*> T_AnimationPlayerList;
typedef T_AnimationPlayerList::iterator IT_AnimationPlayerList;
typedef T_AnimationPlayerList::const_iterator CIT_AnimationPlayerList;

} // namespace o3d

#endif // _O3D_ANIMATIONPLAYER_H
