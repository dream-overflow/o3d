/**
 * @file specialeffectsmanager.h
 * @brief Specials effects manager.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-01-31
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SPECIALEFFECTSMANAGER_H
#define _O3D_SPECIALEFFECTSMANAGER_H

#include "../scene/sceneentity.h"
#include "o3d/core/mutex.h"
#include "o3d/core/memorydbg.h"
#include "specialeffects.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class SpecialEffectsManager
//-------------------------------------------------------------------------------------
//! Specials effects manager.
//---------------------------------------------------------------------------------------
class O3D_API SpecialEffectsManager : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(SpecialEffectsManager)

	//! Default constructor.
	SpecialEffectsManager(BaseObject *parent);

	//! Virtual destructor.
	virtual ~SpecialEffectsManager();

    virtual Bool deleteChild(BaseObject *child) override;

	//! Insert an existing special effect in the manager.
	//! @note Once the special effect is inserted into the manager, it become its parent.
	void addSpecialEffects(SpecialEffects *specialEffect);

	//! Remove an existing special effect from the manager.
	void removeSpecialEffects(SpecialEffects *specialEffect);

	//! Delete an existing special effect from the manager.
	void deleteSpecialEffects(SpecialEffects *specialEffect);

	//! Is a special effect exists.
	//! @param name Name of the special effect.
	Bool isSpecialEffects(const String &name);

	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	//! Update all effects.
	void update();

	//! Draw all effects.
	void draw(const DrawInfo &drawInfo);

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	//! Define an imported special effect for a given serialize id.
	inline void setImportedSpecialEffects(UInt32 index, SpecialEffects *specialEffects)
	{
		if (index < m_indexToEffectSize)
			m_indexToEffect[index] = specialEffects;
	}

	//! Return an imported special effect for a given serialize id.
	inline SpecialEffects* getImportedSpecialEffects(UInt32 index) const
	{
		return m_indexToEffect[index];
	}

	//! Resize the list of special effect to import.
	void resizeImportedSpecialEffects(UInt32 size);

	//! Return the number of last imported special effect.
	inline UInt32 getNumImportedSpecialEffects() const { return m_indexToEffectSize; }

    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

private:

	typedef std::map<String, SpecialEffects*> T_FindMap;
	typedef T_FindMap::iterator IT_FindMap;
	typedef T_FindMap::const_iterator CIT_FindMap;

	T_FindMap m_findMap;

	FastMutex m_mutex;

	SpecialEffects* findSpecialEffect(UInt32 type, const String &keyName);

	SpecialEffects **m_indexToEffect; //!< used for import export
	UInt32 m_indexToEffectSize;
};

} // namespace o3d

#endif // _O3D_SPECIALEFFECTSMANAGER_H
