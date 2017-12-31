/**
 * @file animationplayermanager.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-04-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/animation/animationplayermanager.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(AnimationPlayerManager, ENGINE_ANIMATIONPLAYER_LIST, SceneEntity)

// Constructor
AnimationPlayerManager::AnimationPlayerManager(BaseObject *parent) :
	SceneEntity(parent)
{
}

// desctructor
AnimationPlayerManager::~AnimationPlayerManager()
{
	removeAll();

	for (IT_AnimationPlayerList it = m_playerList.begin() ; it != m_playerList.end() ; ++it)
		deletePtr(*it);

	m_playerList.clear();
}

// Create an animation player
AnimationPlayer* AnimationPlayerManager::createAnimationPlayer(Animation *animation)
{
	AnimationPlayer *animationPlayer = new AnimationPlayer(this, animation);
	return animationPlayer;
}

// Delete an animation player (delete manually an owned player)
Bool AnimationPlayerManager::deleteAnimationPlayer(AnimationPlayer *animationPlayer)
{
	if (animationPlayer)
	{
		animationPlayer->destroy();

		if (animationPlayer->getParent() == this)
		{
			m_playerList.remove(animationPlayer);
			return True;
		}
	}
	return False;
}

// add an animation player queue and return its queue ID
Int32 AnimationPlayerManager::add(AnimationPlayer& Player, Bool ownit)
{
	// Create a new queue and add the player to it
	T_AnimationPlayerList NewQueue;
	NewQueue.push_back(&Player);

	Int32 id = m_IDManager.getID();
	m_Map[id] = NewQueue;
	if (ownit) m_playerList.push_back(&Player);

	return id;
}

// add an animation player in a queue (by its ID) in the last position
Bool AnimationPlayerManager::addLast(
	Int32 QueueId,
	AnimationPlayer& Player,
	Bool ownit)
{
	// Find the queue and add the player to the back
	IT_PlayerQueueMap it = m_Map.find(QueueId);

	if (it != m_Map.end())
	{
		T_AnimationPlayerList& Queue = (*it).second;
		Queue.push_back(&Player);
		if (ownit) m_playerList.push_back(&Player);

		return True;
	}
	return False;
}

// add an animation player in a queue (by its ID) in the first position
Bool AnimationPlayerManager::addFirst(
	Int32 queueId,
	AnimationPlayer &player,
	Bool ownit)
{
	// Find the queue and add the player to the front
	IT_PlayerQueueMap it = m_Map.find(queueId);

	if (it != m_Map.end())
	{
		T_AnimationPlayerList& Queue = (*it).second;
		Queue.push_front(&player);

		if (ownit)
			m_playerList.push_back(&player);

		return True;
	}
	return False;
}

// remove an animation queue
void AnimationPlayerManager::remove(Int32 queueId)
{
	// Remove the queue from the queue map
	IT_PlayerQueueMap it = m_Map.find(queueId);

	if (it != m_Map.end())
	{
		m_Map.erase(it);
		m_IDManager.releaseID(queueId);
	}

	// Remove any link from and to this queue
	IT_PlayerLinksMap itlink;
	while ((itlink = m_Links.find(queueId)) != m_Links.end())
	{
		unLink(queueId,(*itlink).second);
	}
}

// remove all animation player queue
void AnimationPlayerManager::removeAll()
{
	m_Map.clear();
	m_Links.clear();

	m_IDManager.releaseAll();
}

// update all players queues
void AnimationPlayerManager::update()
{
	IT_PlayerQueueMap it;

	for (it = m_Map.begin() ; it != m_Map.end() ; ++it)
	{
		T_AnimationPlayerList& Queue = (*it).second;
        AnimationPlayer* Player = nullptr;

		// pop the queue until we find a player that's not finished
		while (!Queue.empty())
		{
			Player = Queue.front();

			if (!Player->isFinished())
			{
				// Update the player at the front of the queue
				Player->update();
				break;
			}

			Queue.pop_front();
			m_playerList.remove(Player);
			deletePtr(Player);
		}
	}
}

/*---------------------------------------------------------------------------------------
  link an animation queue with another one, by theirs ID
---------------------------------------------------------------------------------------*/
void AnimationPlayerManager::link(Int32 Queue1,Int32 Queue2)
{
	m_Links.insert(std::pair<Int32,Int32>(Queue1,Queue2));
	m_Links.insert(std::pair<Int32,Int32>(Queue2,Queue1));
}

/*---------------------------------------------------------------------------------------
  unlink two animations queue by theirs ID
---------------------------------------------------------------------------------------*/
void AnimationPlayerManager::unLink(Int32 Queue1,Int32 Queue2)
{
	// Find the node of the map with a key equal to (Queue1,Queue2) and erase it
	IT_PlayerLinksMap it = m_Links.find(Queue1);

	while (it != m_Links.end() && (*it).first == Queue1)
	{
		if ((*it).second == Queue2)
		{
			m_Links.erase(it);
			break;
		}
		++it;
	}

	// Find the node of the map with a key equal to (Queue2,Queue1) and erase it
	it = m_Links.find(Queue2);

	while (it != m_Links.end() && (*it).first == Queue2)
	{
		if ((*it).second == Queue1)
		{
			m_Links.erase(it);
			break;
		}
		++it;
	}
}

void AnimationPlayerManager::doLinkedAction(Int32 QueueId,Player_Action Action,Float Param)
{
	// Do the action on the specified queue
	IT_PlayerQueueMap it = m_Map.find(QueueId);

	if (it != m_Map.end())
	{
		T_AnimationPlayerList& Queue = (*it).second;

		if (!Queue.empty())
		{
			doAction(Queue.front(),Action,Param);
		}
	}

	// Do the action on all linked queues
	IT_PlayerLinksMap itlink = m_Links.find(QueueId);

	while (itlink != m_Links.end() && (*itlink).first == QueueId)
	{
		Int32 LinkedQueue = (*itlink).second;

		it = m_Map.find(LinkedQueue);

		if (it != m_Map.end())
		{
			T_AnimationPlayerList& Queue = (*it).second;

			if (!Queue.empty())
			{
				doAction(Queue.front(),Action,Param);
			}
		}
		++itlink;
	}
}

// Get the first animation player for a queue ID
AnimationPlayer* AnimationPlayerManager::getFirstPlayer(Int32 QueueId)
{
	// find the corresponding queue and return the value for the first player in the queue
	IT_PlayerQueueMap it = m_Map.find(QueueId);

	if (it != m_Map.end())
	{
		T_AnimationPlayerList& Queue = (*it).second;

		if (Queue.size())
			return Queue.front();
	}

    return nullptr;
}

// Get the last animation player for a queue ID
AnimationPlayer* AnimationPlayerManager::getLastPlayer(Int32 QueueId)
{
	// find the corresponding queue and return the value for the first player in the queue
	IT_PlayerQueueMap it = m_Map.find(QueueId);

	if (it != m_Map.end())
	{
		T_AnimationPlayerList& Queue = (*it).second;

		if (Queue.size())
			return Queue.back();
	}

    return nullptr;
}

// get an animation player by its position in a queue and the queue ID
AnimationPlayer* AnimationPlayerManager::getPlayer(Int32 QueueId,UInt32 PlayerId)
{
	// find the corresponding queue and return the value for the first player in the queue
	IT_PlayerQueueMap it = m_Map.find(QueueId);

	if (it != m_Map.end())
	{
		T_AnimationPlayerList& Queue = (*it).second;
		IT_AnimationPlayerList itqueue = Queue.begin();

		UInt32 i = 0;

		while (i < PlayerId && itqueue != Queue.end())
		{
			++itqueue;
			++i;
		}

		if (itqueue != Queue.end())
		{
			return (*itqueue);
		}
	}

    return nullptr;
}

// serialisation
Bool AnimationPlayerManager::writeToFile(OutStream &os)
{
    BaseObject::writeToFile(os);

    os << (UInt32)m_Map.size();

	// write all queues
	for (IT_PlayerQueueMap it = m_Map.begin() ; it != m_Map.end() ; ++it)
	{
		T_AnimationPlayerList& Queue = (*it).second;
        AnimationPlayer* Player = nullptr;

        os   << ENGINE_ANIMATION_PLAYER_QUEUE
			 << (UInt32)Queue.size();

		// write all players in queue
		for (IT_AnimationPlayerList players = Queue.begin() ; players != Queue.end() ; ++players)
		{
			Player = *players;
			if (Player)
			{
                os << *Player;
			}
			else
                O3D_ERROR(E_InvalidPrecondition("Player must non null"));
		}
	}

	// write all links
    os << (UInt32)m_Links.size();

	for (IT_PlayerLinksMap it = m_Links.begin() ; it != m_Links.end() ; ++it)
	{
        os << (*it).first
           << (*it).second;
	}

	return True;
}

Bool AnimationPlayerManager::readFromFile(InStream &is)
{
    BaseObject::readFromFile(is);

	m_ImportedPlayer.clear();

	UInt32 tmp, nb_queue;
    is >> nb_queue;

	// read all queues
	for (UInt32 i = 0 ; i < nb_queue ; ++i)
	{
        is >> tmp;

		// add a player queue
		if (tmp != ENGINE_ANIMATION_PLAYER_QUEUE)
            O3D_ERROR(E_InvalidFormat("Invalid animation playeyr queue token"));

		// Create a new queue
		T_AnimationPlayerList NewQueue;

		UInt32 nb_player;
        is >> nb_player;

		// read all players
		for (UInt32 j = 0 ; j < nb_player ; ++j)
		{
            AnimationPlayer *pPlayer = new AnimationPlayer(this,nullptr);
            if (!pPlayer->readFromFile(is))
				return False;

			m_playerList.push_back(pPlayer);
			NewQueue.push_back(pPlayer);
			pPlayer->restart();
			pPlayer->play();

			m_ImportedPlayer.push_back(pPlayer);
		}

		m_Map[m_IDManager.getID()] = NewQueue;
	}

	UInt32 nb_link;
    is >> nb_link;

	Int32 queue1,queue2;

	// read all queues
	for (UInt32 i = 0 ; i < nb_link ; ++i)
	{
        is   >> queue1
			 >> queue2;

		link(queue1,queue2);
	}

	return True;
}

