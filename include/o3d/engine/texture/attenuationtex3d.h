/**
 * @file attenuationtex3d.h
 * @brief Attenuation texture which use of a 3d texture.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
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

/**
 * @brief Attenuation texture which use of a 3d texture
 */
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
    virtual void destroy() override;

	//! Get the size
	inline UInt32 getSize() const { return m_size; }

	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	//! set the texture parameter, such as enable extra coord for cubemap...
    virtual void set() override;
	//! bind an opengl texture
    virtual Bool bind() override;
	//! unbind the texture
    virtual void unbind() override;
	//! unset the texture parameter
    virtual void unSet() override;

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

protected:

	UInt32 m_size;       //!< size of the cubemap texture (square)

	//! Set the filtering mode to OpenGL.
    virtual void setFilteringMode() override;
	//! Set the warp mode to OpenGL.
    virtual void setWrapMode() override;
	//! Set the anisotropy level to OpenGL.
    virtual void setAnisotropyLevel() override;
};

} // namespace o3d

#endif // _O3D_ATTENUATIONTEX3D_H
