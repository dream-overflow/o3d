/**
 * @file sndbuffermanager.h
 * @brief Sound buffer manager
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-02-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SNDBUFFERMANAGER_H
#define _O3D_SNDBUFFERMANAGER_H

#include "sndbuffer.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class SndBufferManager
//-------------------------------------------------------------------------------------
//! Sound buffer manager.
//---------------------------------------------------------------------------------------
class O3D_API SndBufferManager : public SceneResourceManager
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(SndBufferManager)

	//! Default constructor.
	SndBufferManager(BaseObject *parent, const String &path = "");

	//! Virtual destructor.
	virtual ~SndBufferManager();

	Bool deleteChild(BaseObject *child);

	//! Insert an existing sound buffer object into the manager.
	void addSndBuffer(SndBuffer *sndBuffer);

	//! Remove an existing sound buffer from the manager.
	//! The sound buffer is not pushed in the garbage manager, it is simply removed of this manager.
	void removeSndBuffer(SndBuffer *sndBuffer);

	//! Delete an existing sound buffer object into the manager
	void deleteSndBuffer(SndBuffer *sndBuffer);

	//! Purge immediately the garbage manager of its content.
	void purgeGarbage();


	//-----------------------------------------------------------------------------------
	// Factory
	//-----------------------------------------------------------------------------------

	//! Create/Return a sound buffer given a file name and load it in OpenAL.
	//! @param filename Sound filename to load.
	//! @param decodeMaxDuration If the sound duration is greater than this value the
	//! the sound file is reopen from streaming, otherwise it is fully decoded.
	//! @return A new sound buffer object. The alBuffer or sound stream can be shared with
	//! others buffers.
	SndBuffer* addSndBuffer(const String &filename, Float decodeMaxDuration = 2.0);

	//! Is a sound buffer existing, according to its filename.
	//! @param filename Sound buffer filename to search for.
	//! @param decodeMaxDuration Decoded max duration in seconds.
	Bool isSndBuffer(const String &filename, Float decodeMaxDuration);

	//! Create/Return a sound buffer according to the informations read into the file.
    SndBuffer* readSndBuffer(InStream &is);


	//-----------------------------------------------------------------------------------
	// Sound query
	//-----------------------------------------------------------------------------------

	//! Enable an asynchronous sound loading.
	void enableAsynchronous();

	//! Disable an asynchronous sound loading.
	void disableAsynchronous();

	//! Is asynchronous sound loading enabled.
	Bool isAsynchronous() const;

	//! Update the manager
	void update();

private:

	struct GarbageKey
	{
        String resourceName;
        UInt32 type;
        Float decodeMaxDuration;

		GarbageKey() :
                    type(0),
                    decodeMaxDuration(0.f)
		{
		}

		GarbageKey(
                const String &_resourceName,
				UInt32 type,
				Float decodeMaxDuration) :
                    resourceName(_resourceName),
                    type(type),
                    decodeMaxDuration(decodeMaxDuration)
		{
		}

		GarbageKey(SndBuffer *sndBuffer)
		{
            O3D_ASSERT(sndBuffer != nullptr);

            resourceName = sndBuffer->getResourceName();
            type = sndBuffer->getType();
            decodeMaxDuration = sndBuffer->getDecodeMaxDuration();
		}

		GarbageKey(const GarbageKey &dup) :
            resourceName(dup.resourceName),
            type(dup.type),
            decodeMaxDuration(dup.decodeMaxDuration)
		{
		}

		inline Bool operator==(const GarbageKey &cmp) const
		{
            return ((resourceName == cmp.resourceName) &&
                    (type == cmp.type) &&
                    (decodeMaxDuration == cmp.decodeMaxDuration));
		}

		inline Bool operator<(const GarbageKey &cmp) const
		{
            if (resourceName < cmp.resourceName)
				return True;
            if (type < cmp.type)
				return True;
            if (decodeMaxDuration < cmp.decodeMaxDuration)
				return True;
			return False;
		}
	};

	//! Manage removed sound buffer object.
	GarbageManager<GarbageKey, SndBuffer*> m_garbageManager;

	typedef std::map<String, std::list<SndBuffer*> > T_FindMap;

	typedef T_FindMap::iterator IT_FindMap;
	typedef T_FindMap::const_iterator CIT_FindMap;

	T_FindMap m_findMap;

	IDManager m_IDManager;

	Bool m_isAsynchronous;

	//! Find a sound buffer.
	SndBuffer* findSndBuffer(
            const String &resourceName,
			UInt32 type,
			Float decodeMaxDuration);
};

} // namespace o3d

#endif // _O3D_SNDBUFFERMANAGER_H

