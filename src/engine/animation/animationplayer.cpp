/**
 * @file animationplayer.cpp
 * @brief Implementation of AnimationPlayer.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-02-23
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/animation/animationplayer.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/animation/animationmanager.h"
#include "o3d/engine/hierarchy/node.h"
#include "o3d/core/debug.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(AnimationPlayer, ENGINE_ANIMATION_PLAYER, SceneEntity)

/*---------------------------------------------------------------------------------------
  constructor
---------------------------------------------------------------------------------------*/
AnimationPlayer::AnimationPlayer(BaseObject *parent) :
	SceneEntity(parent),
	m_animation(this),
	m_elapsed(0.f),
	m_time(0),
	m_elapsedTime(0),
	m_remainingTime(0),
	m_lastTime(-1),
	m_isActive(False),
	m_playerMode(MODE_PAUSE),
    m_animatable(nullptr),
	m_blendMode(Animation::BLEND_REPLACE),
	m_blendWeight(1.0f),
	m_startTime(0.f),
	m_endTime(1.f),
	m_changeAnim(False),
	m_isBreakable(False),
    m_timeToBreak(0),
    m_range(-1)
{
}

AnimationPlayer::AnimationPlayer(
	BaseObject *parent,
	Animation *animation,
	Animatable *animatable,
	Int32 timesToPlay,
	Float fps,
	Animation::BlendMode blendMode,
	Float blendWeight) :
		SceneEntity(parent),
		m_animation(this),
		m_elapsed(0.f),
		m_time(0),
		m_elapsedTime(0),
		m_remainingTime(timesToPlay),
		m_lastTime(-1),
		m_isActive(False),
		m_playerMode(MODE_PAUSE),
		m_animatable(animatable),
		m_blendMode(blendMode),
		m_blendWeight(blendWeight),
		m_startTime(0.f),
		m_endTime(1.f),
		m_changeAnim(False),
		m_isBreakable(False),
        m_timeToBreak(0),
        m_range(-1)
{
	if (animation)
	{
		setAnimation(animation);
		setFramePerSec(fps);
	}
	
}

// copy constructor
AnimationPlayer::AnimationPlayer(const AnimationPlayer &dup) :
	SceneEntity(dup),
	m_animation(this),
	m_frameSpeed(dup.m_frameSpeed),
	m_elapsed(0.f),
	m_speed(dup.m_speed),
	m_time(0),
	m_elapsedTime(0),
	m_remainingTime(dup.m_remainingTime),
	m_lastTime(-1),
	m_isActive(dup.m_isActive),
	m_playerMode(dup.m_playerMode),
	m_animatable(dup.m_animatable),
	m_blendMode(dup.m_blendMode),
	m_blendWeight(dup.m_blendWeight),
	m_startTime(0.f),
	m_endTime(1.f),
	m_changeAnim(False),
	m_isBreakable(dup.m_isBreakable),
    m_timeToBreak(dup.m_timeToBreak),
    m_range(dup.m_range)
{
	setAnimation(dup.m_animation.get());
}

// destructor
AnimationPlayer::~AnimationPlayer()
{
}

// set the animation
void AnimationPlayer::setAnimation(Animation *animation)
{
	m_animation = animation;
	m_changeAnim = False;
}

// set the animatable
void AnimationPlayer::setAnimatable(Animatable *animatable)
{
	m_animatable = animatable;
	m_changeAnim = False;
}

// is the animation finished, then the player should be removed
Bool AnimationPlayer::isFinished()
{
	// the player can be destroyed only if the animation is stopped
	if (m_playerMode == MODE_STOP)
	{
		if (m_remainingTime < 0)
			return False;
		if (m_time >= m_endTime)
			return (m_elapsedTime+1 >= m_remainingTime);
		else
			return False;
	}
	else
		return False;
}

// Use an animation range with time computation to find key frames
void AnimationPlayer::playAnimRange(UInt32 id)
{
	O3D_ASSERT(m_animation);

	Animation::AnimRange range = m_animation->getAnimRange(id);

	Float inv = 1.f / m_animation->getNumFrames();

	m_startTime = (Float)range.start * inv;
	m_endTime = (Float)range.end * inv;
	m_time = m_startTime;

	m_isBreakable = range.breaking != 0 ? True : False;
	if (m_isBreakable)
		m_timeToBreak = (Float)range.breaking * inv;
	else
		m_timeToBreak = 0.f;

	m_animation->useAnimRange(m_animatable, id);
    m_range = (Int32)id;

    m_isActive = True;
}

String AnimationPlayer::getAnimRangeName() const
{
    if (m_range != -1)
        return m_animation->getAnimRange((UInt32)m_range).name;
    else
        return String();
}

// Stop and go for delete the animation player.
void AnimationPlayer::destroy()
{
	m_playerMode = MODE_STOP;
	m_time = m_endTime;
	m_elapsedTime = m_remainingTime;
}

// update the animatable [0<t<1]
void AnimationPlayer::update()
{
	O3D_ASSERT(m_animation);
	O3D_ASSERT(m_animatable);

	// check if the animation needs to be restarted
	if (m_time > m_endTime) // 1.f
	{
		// reset the time depend on player mode
		switch (m_playerMode)
		{
		case MODE_PAUSE:
			pause();
			m_time = m_endTime; // 1.f
			break;
		case MODE_STOP:
			m_time = m_startTime; // 0.f
			m_elapsedTime++;
			break;
		case MODE_LOOP:
			m_time = m_startTime; // 0.f
			m_elapsedTime++;
			break;
		case MODE_PING_PONG:
			m_time = m_endTime; // 1.f
			m_speed = -m_speed;
			m_elapsedTime++;
			break;
		case MODE_CONTINUE:
			onAnimationFinished();
			break;
		default:
			O3D_WARNING("AnimationPlayer::Update m_player_mode unknown");
			break;
		}
	}
	if (m_time < m_startTime) // 0.f
	{
		// reset the time depend on player mode
		switch (m_playerMode)
		{
		case MODE_PAUSE:
			pause();
			break;
		case MODE_STOP:
			m_time = m_endTime; // 1.f
			m_elapsedTime++;
			break;
		case MODE_LOOP:
			m_time = m_endTime; // 1.f
			m_elapsedTime++;
			break;
		case MODE_PING_PONG:
			m_time = m_startTime; // 0.f
			m_speed = -m_speed;
			m_elapsedTime++;
			break;
		case MODE_CONTINUE:
			onAnimationFinished();
			break;
		default:
			O3D_WARNING("AnimationPlayer::Update m_player_mode unknown");
			break;
		}
	}

	// is there an animation to enqueue
	if (m_queueList.size())
	{
		// first we put animation in continue mode (no looping), this would force the anim to finish
		m_playerMode = MODE_CONTINUE;

		// check if we can break the current animation
		if (m_isBreakable)
		{
			if (m_time > m_timeToBreak)
				m_time = m_endTime; // and force to finish
		}
		else
			m_time = m_endTime;     // not breakable, finish anytime

        // check if the animation is finished, and play the next in queue if finished
		if (m_time >= m_endTime)
		{
			Queue &queue = m_queueList.front();

			m_playerMode = queue.Mode;

			if (queue.Range == -1)
				playFullRange();
			else
				playAnimRange(queue.Range);

			m_queueList.pop_front();
		}
	}

	// if no active so the lastTime is not initialized
	/*if (!m_isActive)
	{
		m_lastTime = 0;
		return;
	}*/

	if (m_lastTime > 0)
	{
		Int64 old = m_lastTime;
		m_lastTime = System::getTime();

		m_elapsed += (Float)(m_lastTime-old)/(System::getTimeFrequency());

		// animation update
		if (m_elapsed >= m_frameSpeed)
		{
			m_animatable->resetAnim(); // Important for animation blending
			m_animation->update(m_animatable, m_time, m_blendMode, m_blendWeight);
			m_elapsed = 0.f;
		}

		// compute the new time
		if (isPlaying())
			m_time += m_speed * ((Float)(m_lastTime-old))/(System::getTimeFrequency());
	}
	else
	{
		// the animation has not yet been updated
		m_lastTime = System::getTime();
		m_elapsed = 0;
	}
}

// draw the animation trajectory
void AnimationPlayer::drawTrajectory(Node *curNode, const DrawInfo &drawInfo)
{
    m_animation->drawTrajectory(curNode, drawInfo);
}

// Serialization
Bool AnimationPlayer::writeToFile(OutStream &os)
{
	if (!m_animation)
        O3D_ERROR(E_InvalidPrecondition("Animation must be defined"));

	if (!m_animatable)
        O3D_ERROR(E_InvalidPrecondition("Animatable must be defined"));

    os   << ENGINE_ANIMATION_PLAYER
		 << m_frameSpeed
		 << m_speed
		 << m_isActive
		 << m_playerMode
		 << m_remainingTime
		 << m_animation->getResourceName();

	// write the animatable information for retrieve the animatable object when reading
    Animatable::writeToFile(getScene(), m_animatable, os);

	return True;
}

Bool AnimationPlayer::readFromFile(InStream &is)
{
	UInt32 tmp;
    is >> tmp;

	if (tmp != ENGINE_ANIMATION_PLAYER)
        O3D_ERROR(E_InvalidFormat("Invalid animation player token"));

	String animationFilename;
	String animationName;

    is   >> m_frameSpeed
		 >> m_speed
		 >> m_isActive
		 >> m_playerMode
		 >> m_remainingTime
		 >> animationName;

	m_animation = getScene()->getAnimationManager()->addAnimation(animationName);

	// get the animatable on this player
    m_animatable = Animatable::readFromFile(getScene(), is);

	if (m_animation && !m_animation->isAnimRangeComputed())
		m_animation->computeAnimRange();

	return True;
}

