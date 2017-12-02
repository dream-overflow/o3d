/**
 * @file sndbuffermanager.cpp
 * @brief Implementation of SndBufferManager.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-02-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/audio/precompiled.h"
#include "o3d/audio/sndbuffermanager.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(SndBufferManager, AUDIO_SND_BUFFER_LIST, SceneResourceManager)

// Default constructor
SndBufferManager::SndBufferManager(BaseObject *parent, const String &path) :
    SceneResourceManager(parent, path, "*.mp3|*.ogg|*.wav|*.o3dsfx"),
	m_isAsynchronous(False)
{
	m_garbageManager.setLifeTime(10000);
}

// Virtual destructor
SndBufferManager::~SndBufferManager()
{
	if (!m_findMap.empty())
	{
		String message("Sound buffers still exists into the manager :\n");

		for (IT_FindMap it = m_findMap.begin(); it != m_findMap.end(); ++it)
		{
			for (std::list<SndBuffer*>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			{
                message += "    |- " + (*it2)->getResourceName() + "\n";
				deletePtr(*it2);
			}
		}

		O3D_WARNING(message);
	}
}

// Delete child .
Bool SndBufferManager::deleteChild(BaseObject *child)
{
	if (child)
	{
		if (child->getParent() != this)
			O3D_ERROR(E_InvalidParameter("The parent child differ from this"));
		else
		{
			// is it a material, and is it managed by this
			SndBuffer *sndBuffer = o3d::dynamicCast<SndBuffer*>(child);
			if (sndBuffer && (sndBuffer->getManager() == this))
			{
				deleteSndBuffer(sndBuffer);
				return True;
			}

			// otherwise simply delete the child
			deletePtr(child);
			return True;
		}
	}
	return False;
}

SndBuffer* SndBufferManager::findSndBuffer(
        const String &resourceName,
        UInt32 type,
        Float decodeMaxDuration)
{
	FastMutexLocker locker(m_mutex);

	// search the key name into the map, next the sound buffer given parameters into the element
    CIT_FindMap cit = m_findMap.find(resourceName);
	if (cit != m_findMap.end())
	{
		// search into the list
		for (std::list<SndBuffer*>::const_iterator it2 = cit->second.begin(); it2 != cit->second.end(); ++it2)
		{
			SndBuffer *sndBuffer = *it2;

			if ((sndBuffer->getType() == type) &&
				(sndBuffer->getDecodeMaxDuration() == decodeMaxDuration))
			{
				// found it ?
				return sndBuffer;
			}
		}
	}

	// maybe the asked sound buffer was just deleted, so search it into the garbage collector
    SndBuffer *sndBuffer = nullptr;
	m_garbageManager.remove(
            GarbageKey(resourceName, type, decodeMaxDuration),
			sndBuffer);

	locker.unlock();

	// if found, reinsert it into the manager
	if (sndBuffer)
		addSndBuffer(sndBuffer);

	return sndBuffer;
}

// Add a sound buffer to the manager.
void SndBufferManager::addSndBuffer(SndBuffer *sndBuffer)
{
    if (sndBuffer->getManager() != nullptr)
		O3D_ERROR(E_InvalidOperation("The given sound buffer already have a manager"));

	FastMutexLocker locker(m_mutex);

    // search for the sound buffer
    IT_FindMap it = m_findMap.find(sndBuffer->getResourceName());
	if (it != m_findMap.end())
	{
		it->second.push_back(sndBuffer);
	}
	else
	{
		std::list<SndBuffer*> entry;
		entry.push_back(sndBuffer);

        m_findMap.insert(std::make_pair(sndBuffer->getResourceName(), entry));
	}

    O3D_MESSAGE("Add sound buffer \"" + sndBuffer->getResourceName() + "\"");

	// this is the manager
	sndBuffer->setManager(this);
	sndBuffer->setParent(this);
	sndBuffer->setId(m_IDManager.getID());
}

// Remove a sound buffer from the manager.
void SndBufferManager::removeSndBuffer(SndBuffer *sndBuffer)
{
	if (sndBuffer->getManager() != this)
		O3D_ERROR(E_InvalidParameter("Sound buffer manager is not this"));

	FastMutexLocker locker(m_mutex);

	// remove the sound buffer object from the manager.
    IT_FindMap it = m_findMap.find(sndBuffer->getResourceName());
	if (it != m_findMap.end())
	{
		std::list<SndBuffer*>::iterator it2 =  it->second.begin();

		// search into the list
		for (; it2 != it->second.end(); ++it2)
		{
			// found it ?
			if (*it2 == sndBuffer)
				break;
		}

		// if sound buffer found erase it from the list
		if (it2 != it->second.end())
		{
			it->second.erase(it2);

            sndBuffer->setManager(nullptr);
			sndBuffer->setParent(getScene());

			m_IDManager.releaseID(sndBuffer->getId());
			sndBuffer->setId(-1);

            O3D_MESSAGE("Remove (not delete) existing sound buffer : " + sndBuffer->getResourceName());
		}

		// erase the list if empty
		if (it->second.empty())
			m_findMap.erase(it);
	}
}

// Delete a sound buffer from the manager.
void SndBufferManager::deleteSndBuffer(SndBuffer *sndBuffer)
{
	if (sndBuffer->getManager() != this)
		O3D_ERROR(E_InvalidParameter("Sound buffer manager is not this"));

	FastMutexLocker locker(m_mutex);

	// remove the sound buffer object from the manager.
    IT_FindMap it = m_findMap.find(sndBuffer->getResourceName());
	if (it != m_findMap.end())
	{
		std::list<SndBuffer*>::iterator it2 =  it->second.begin();

		// search into the list
		for (; it2 != it->second.end(); ++it2)
		{
			// found it ?
			if (*it2 == sndBuffer)
				break;
		}

		// if sound buffer found erase it from the list
		// and adding it for a deferred deletion
		if (it2 != it->second.end())
		{
			it->second.erase(it2);
			m_garbageManager.add(GarbageKey(sndBuffer), sndBuffer);

            sndBuffer->setManager(nullptr);

			m_IDManager.releaseID(sndBuffer->getId());
			sndBuffer->setId(-1);

            O3D_MESSAGE("Delete existing sound buffer : " + sndBuffer->getResourceName());
		}

		// erase the list if empty
		if (it->second.empty())
			m_findMap.erase(it);
	}
}

// Purge immediately the garbage manager of its content.
void SndBufferManager::purgeGarbage()
{
	FastMutexLocker locker(m_mutex);

	m_garbageManager.destroy();
}

// Create a new sound buffer.
SndBuffer* SndBufferManager::addSndBuffer(const String &filename, Float decodeMaxDuration)
{
	// search for a similar sound buffer
	SndBuffer *sndBuffer = findSndBuffer(
            filename,
			AUDIO_SND_BUFFER,
			decodeMaxDuration);

	if (sndBuffer)
		return sndBuffer;

    String absFileName = getFullFileName(filename);

	// new sound buffer
    sndBuffer = new SndBuffer(this, decodeMaxDuration);
    sndBuffer->setResourceName(filename);

	// asynchronous loading
	if (m_isAsynchronous)
	{
		SndBufferTask *task = new SndBufferTask(
				sndBuffer,
				absFileName,
				decodeMaxDuration);

		TaskManager::instance()->addTask(task);
	}
	// synchronous loading
    else
    {
        Sound sound;
        sound.load(absFileName, decodeMaxDuration);

        sndBuffer->setSound(sound);
        if (!sndBuffer->create())
        {
            deletePtr(sndBuffer);
            return nullptr;
        }
	}

	addSndBuffer(sndBuffer);
	return sndBuffer;
}

// Search a sound by its path+filename
Bool SndBufferManager::isSndBuffer(const String &filename, Float decodeMaxDuration)
{
    //getResourceName(filename); TODO and same for others rcmgr
    return findSndBuffer(filename, AUDIO_SND_BUFFER, decodeMaxDuration) != nullptr;
}

// Read a buffer class information and create this new buffer or return an existing buffer.
SndBuffer* SndBufferManager::readSndBuffer(InStream &is)
{
	SndBuffer *sndBuffer = new SndBuffer(this);

	// read sound buffer info from file
    if (!sndBuffer->readFromFile(is))
	{
		deletePtr(sndBuffer);
        return nullptr;
	}

	// default sound buffer TODO (silent)
	/*if (sndBuffer->getFileName() == "<o3d::soundbuffer::default>")
	{
		deletePtr(sndBuffer);
		return m_defaultSndBuffer;
	}*/

	// search for a similar sound buffer
	SndBuffer *foundSndBuffer = findSndBuffer(
            sndBuffer->getResourceName(),
			AUDIO_SND_BUFFER,
			sndBuffer->getDecodeMaxDuration());

	// already exists ?
	if (foundSndBuffer)
	{
		deletePtr(sndBuffer);
		return foundSndBuffer;
	}

    String absFileName = getFullFileName(sndBuffer->getResourceName());

	// asynchronous loading
	if (m_isAsynchronous)
	{
		SndBufferTask *task = new SndBufferTask(
				sndBuffer,
                absFileName,
				sndBuffer->getDecodeMaxDuration());

		TaskManager::instance()->addTask(task);
	}
	// synchronous loading
    else
    {
        Sound snd;
        snd.load(absFileName, sndBuffer->getDecodeMaxDuration());

        sndBuffer->setSound(snd);
        if (!sndBuffer->create())
        {
            deletePtr(sndBuffer);
            return nullptr;
        }
	}

	addSndBuffer(sndBuffer);
	return sndBuffer;
}

// Enable an asynchronous sound query manager
void SndBufferManager::enableAsynchronous()
{
	FastMutexLocker locker(m_mutex);

	if (!m_isAsynchronous)
		m_isAsynchronous = True;
}

// Disable an asynchronous sound query manager
void SndBufferManager::disableAsynchronous()
{
	FastMutexLocker locker(m_mutex);

	if (m_isAsynchronous)
		m_isAsynchronous = False;
}

// Is asynchronous texture sound is enabled.
Bool SndBufferManager::isAsynchronous() const
{
	m_mutex.lock();
	Bool result = m_isAsynchronous;
	m_mutex.unlock();

	return result;
}

// Update the manager
void SndBufferManager::update()
{
	FastMutexLocker locker(m_mutex);

	// process garbage checking
	m_garbageManager.update();
}

