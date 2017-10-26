/**
 * @file animationplayermanager.h
 * @brief The animation player manager.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2006-04-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ANIMATIONPLAYMGER_H
#define _O3D_ANIMATIONPLAYMGER_H

#include "o3d/core/idmanager.h"
#include "animationplayer.h"
#include "animatable.h"
#include <map>
#include <list>

namespace o3d {

typedef std::map<Int32,T_AnimationPlayerList> T_PlayerQueueMap;
typedef T_PlayerQueueMap::iterator IT_PlayerQueueMap;
typedef T_PlayerQueueMap::const_iterator CIT_PlayerQueueMap;

typedef std::multimap<Int32,Int32> T_PlayerLinksMap;
typedef T_PlayerLinksMap::iterator IT_PlayerLinksMap;
typedef T_PlayerLinksMap::const_iterator CIT_PlayerLinksMap;

typedef std::vector<AnimationPlayer*> T_PlayerVector;
typedef T_PlayerVector::iterator IT_PlayerVector;
typedef T_PlayerVector::const_iterator CIT_PlayerVector;

//---------------------------------------------------------------------------------------
//! @class AnimationPlayerManager
//-------------------------------------------------------------------------------------
//! The animation player manager. You define animation players in queue. Each player
//! is played once time the previous is finished. All queue are updated on each frame.
//! You can do linked actions by linking two queues (by their ID) between them
//---------------------------------------------------------------------------------------
class O3D_API AnimationPlayerManager : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(AnimationPlayerManager)

	enum Player_Action{
		PLAYER_PLAY,
		PLAYER_PAUSE,
		PLAYER_TOGGLE_PLAY_PAUSE,
		PLAYER_RESTART,
		PLAYER_STEP_FORWARD,
		PLAYER_STEP_BACKWARD,
		PLAYER_STEP_VAL,
		PLAYER_STEP_BOOL
	};

	//! Default constructor.
	AnimationPlayerManager(BaseObject *parent);

	//! Virtual desctructor.
	virtual ~AnimationPlayerManager();

	//-----------------------------------------------------------------------------------
	// Factory
	//-----------------------------------------------------------------------------------

	//! Create an animation player
	AnimationPlayer* createAnimationPlayer(Animation *animation = NULL);

	//! Delete an animation player.
	//! The player is deleted even if the animation is not finished.
	//! @return True if the player was owned by the manager.
	Bool deleteAnimationPlayer(AnimationPlayer *animationPlayer);

	//-----------------------------------------------------------------------------------
	// Animation queue
	//-----------------------------------------------------------------------------------

	//! Add an animation player queue and return its queue ID, set ownit to true if you want
	//! the manager auto-delete the player at destructor.
	Int32 add(AnimationPlayer &player, Bool ownit = True);

	//! Add an animation player in a queue (by its ID) in the last position
	Bool addLast(Int32 queueId, AnimationPlayer &player, Bool ownit = True);
	//! Add an animation player in a queue (by its ID) in the first position
	Bool addFirst(Int32 queueId, AnimationPlayer &player, Bool ownit = True);

	//! Remove an animation queue
	void remove(Int32 queueId);
	//! Remove all animation player queue
	void removeAll();

	//! link an animation queue with another one, by theirs ID
	void link(Int32 queue1, Int32 queue2);
	//! Unlink two animations queue by theirs ID
	void unLink(Int32 queue1, Int32 queue2);

	//! Get the first animation player for a queue ID
	AnimationPlayer* getFirstPlayer(Int32 queueId);
	//! Get the last animation player for a queue ID
	AnimationPlayer* getLastPlayer(Int32 queueId);
	//! Get an animation player for a queue ID and a a player ID
	AnimationPlayer* getPlayer(Int32 queueId, UInt32 playerId);

	//-----------------------------------------------------------------------------------
	// Processing
	//---------------------------------------------------------------------------------------

	//! Update all animations players
	void update();

	//! Pause the animation player queue by its ID
	inline void pause(Int32 queueId) { doLinkedAction(queueId,PLAYER_PAUSE); }
	//! Play the animation player queue by its ID
	inline void play(Int32 queueId) { doLinkedAction(queueId,PLAYER_PLAY); }
	//! Toggle play/pause the animation player queue by its ID
	inline void togglePlayPause(Int32 queueId) { doLinkedAction(queueId,PLAYER_TOGGLE_PLAY_PAUSE); }
	//! Restart the animation player queue by its ID
	inline void restart(Int32 queueId) { doLinkedAction(queueId,PLAYER_RESTART); }
	//! Add a step to the animation player queue by its ID
    inline void stepBy(Int32 queueId, Float Val) { doLinkedAction(queueId,PLAYER_STEP_VAL,Val); }
	//! Add a step to the animation player queue by its ID, reward or forward mode
	inline void step(Int32 queueId, Bool Forward = True) { doLinkedAction(queueId,PLAYER_STEP_BOOL,(Float)Forward); }
	//! Forward the animation player queue by its ID
	inline void stepForward(Int32 queueId) { doLinkedAction(queueId,PLAYER_STEP_FORWARD); }
	//! Backward the animation player queue by its ID
	inline void stepBackward(Int32 queueId) { doLinkedAction(queueId,PLAYER_STEP_BACKWARD); }

	//! Is the animation player queue playing (by its ID)
	inline Bool isPlaying(Int32 queueId)
	{
		// Find the corresponding queue and return the value for the first player in the queue
		IT_PlayerQueueMap it = m_Map.find(queueId);

		if (it != m_Map.end())
		{
			T_AnimationPlayerList& Queue = (*it).second;

			if (!Queue.empty())
				return Queue.front()->isPlaying();
		}
		return False;
	}

	//! Change the time for an animation player queue by its ID
	inline void setTime(Int32 queueId, Float Time)
	{
		// Find the corresponding queue and return the value for the first player in the queue
		IT_PlayerQueueMap it = m_Map.find(queueId);

		if (it != m_Map.end())
		{
			T_AnimationPlayerList& Queue = (*it).second;

			if (!Queue.empty())
				return Queue.front()->setTime(Time);
		}
	}

	//! Get the time for an animation player queue by its ID
	inline Float getTime(Int32 queueId)
	{
		// Find the corresponding queue and return the value for the first player in the queue
		IT_PlayerQueueMap it = m_Map.find(queueId);

		if (it != m_Map.end())
		{
			T_AnimationPlayerList& Queue = (*it).second;

			if (!Queue.empty())
				return Queue.front()->getTime();
		}
		return 0.f;
	}

	//-----------------------------------------------------------------------------------
	// Serialisation
	//---------------------------------------------------------------------------------------

	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

	inline       T_PlayerVector& getImportedPlayers()      { return m_ImportedPlayer; }
	inline const T_PlayerVector& getImportedPlayers()const { return m_ImportedPlayer; }

private:

	void doLinkedAction(Int32 QueueId,Player_Action action, Float param = 0);

	inline void doAction(AnimationPlayer* player, Player_Action action, Float param)
	{
		switch (action)
		{
			case PLAYER_PLAY:
				player->play();
			break;
			case PLAYER_PAUSE:
				player->pause();
			break;
			case PLAYER_TOGGLE_PLAY_PAUSE:
				player->togglePlayPause();
			break;
			case PLAYER_RESTART:
				player->restart();
			break;
			case PLAYER_STEP_FORWARD:
				player->stepForward();
			break;
			case PLAYER_STEP_BACKWARD:
				player->stepBackward();
			break;
			case PLAYER_STEP_VAL:
                player->stepBy(param);
			break;
			case PLAYER_STEP_BOOL:
				player->step(param > 0);
			break;
		}
	}

	T_PlayerQueueMap m_Map;         //!< the map that contain all player queue
	T_PlayerLinksMap m_Links;       //!< the multimap that contain link between queues maps

	T_AnimationPlayerList m_playerList; //!< used to delete automatically all players at destructor

	IDManager m_IDManager;           //!< id manager and recycler

	T_PlayerVector m_ImportedPlayer;
};

} // namespace o3d

#endif // _O3D_ANIMATIONPLAYMGER_H

