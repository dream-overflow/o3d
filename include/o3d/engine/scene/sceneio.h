/**
 * @file sceneio.h
 * @brief Scene import/export settings.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-07-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SCENEIO_H
#define _O3D_SCENEIO_H

#include "o3d/core/serialize.h"
#include "o3d/core/debug.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class SceneIO
//-------------------------------------------------------------------------------------
//! Used for import/export options. There is too a default setting for general export.
//! Take care: if you choose to write the tree and not some of object used by this tree
//! the result will be undefined.
//---------------------------------------------------------------------------------------
class O3D_API SceneIO : public Serialize
{
	O3D_DEFINE_SERIALIZATION(SceneIO)

public:

	enum Setting
	{
		MESH = 0,
		SKIN,
		BONES,
		LIGHT,
		CAMERA,
		GIZMO,
		COMPLEX_MESH,
		NODES,
		DRAWING_STATE,
		SPECIAL_EFFECT,
		SND_SOURCE,
		SND_LISTENER,
		PHYSIC,
		MATERIAL,
		ANIMATION,
		ANIMATION_PLAYER,
		GRAPHIC_SETTING,
		AUDIO_SETTING,
		NUM_SETTINGS
	};

	//! default constructor
	SceneIO()
	{
		m_setting = (1 << NUM_SETTINGS) - 1;
	}

	inline SceneIO& operator= (const SceneIO& dup)
	{
		m_setting = dup.m_setting;
		return (*this);
	}

	inline void set(UInt32 setting, Bool value)
	{
		if (setting >= NUM_SETTINGS)
		{
			O3D_ERROR(E_IndexOutOfRange("setting"));
            return;
		}
		if (value) m_setting |= (1 << setting);  // put the bit to 1
		else	   m_setting &= ~(1 << setting); // put the bit to 0
	}

	inline Bool get(UInt32 setting) const
	{
		if (setting >= NUM_SETTINGS)
		{
			O3D_ERROR(E_IndexOutOfRange("setting"));
            return False;
		}
		return (m_setting & (1 << setting));
	}

	//! Is an object can be exported/imported according to this scene IO.
	Bool isIO(const class SceneObject &object) const;

	//! Serialization
    virtual Bool writeToFile(OutStream& os);
    virtual Bool readFromFile(InStream& is);

private:

	UInt32 m_setting;
};

} // namespace o3d

#endif // _O3D_SCENEIO_H

