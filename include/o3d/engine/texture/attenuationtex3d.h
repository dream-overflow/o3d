/**
 * @file attenuationtex3d.h
 * @brief Attenuation texture which use of a 3d texture.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2007-07-31
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ATTENUATIONTEX3D_H
#define _O3D_ATTENUATIONTEX3D_H

#include "o3d/core/matrix4.h"
#include "o3d/core/smartcounter.h"
#include "o3d/image/image.h"

#include "texture.h"

#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class AttenuationTex3D
//-------------------------------------------------------------------------------------
//! Attenuation texture which use of a 3d texture
//---------------------------------------------------------------------------------------
class O3D_API AttenuationTex3D : public Texture
{
public:

	O3D_DECLARE_CLASS(AttenuationTex3D)

    //! Make a resource name.
    static String makeResourceName(UInt32 size);

	//! Default constructor (default size is 64).
	//! @param parent Owner.
	AttenuationTex3D(BaseObject *parent, UInt32 size = 64);

	//! desctructor
	virtual ~AttenuationTex3D();

	//! Create the attenuation texture.
	//! If the texture already exists it return false immediatly.
	Bool create();

	//! Delete the texture
	virtual void destroy();

	//! Get the size
	inline UInt32 getSize() const { return m_size; }

	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	//! set the texture parameter, such as enable extra coord for cubemap...
	virtual void set();
	//! bind an opengl texture
	virtual Bool bind();
	//! unbind the texture
	virtual void unbind();
	//! unset the texture parameter
	virtual void unSet();

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

protected:

	UInt32 m_size;       //!< size of the cubemap texture (square)

	//! Set the filtering mode to OpenGL.
	virtual void setFilteringMode();
	//! Set the warp mode to OpenGL.
	virtual void setWrapMode();
	//! Set the anisotropy level to OpenGL.
	virtual void setAnisotropyLevel();
};

} // namespace o3d

#endif // _O3D_ATTENUATIONTEX3D_H

