/**
 * @file normalcubemaptex.h
 * @brief Normal cube-map texture.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-04-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_NORMCUBEMAPTEX_H
#define _O3D_NORMCUBEMAPTEX_H

#include "o3d/core/matrix4.h"
#include "o3d/core/smartcounter.h"
#include "o3d/image/image.h"

#include "texture.h"

#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class NormalCubeMapTex
//-------------------------------------------------------------------------------------
//! Normal cube-map texture.
//---------------------------------------------------------------------------------------
class O3D_API NormalCubeMapTex : public Texture
{
public:

	O3D_DECLARE_CLASS(NormalCubeMapTex)

    //! Make a resource name.
    static String makeResourceName(UInt32 size, Float offset);

	//! default constructor (default size 64, offset 0.5)
	NormalCubeMapTex(BaseObject *parent, UInt32 size = 64, Float offset = 0.5f);

	//! destructor
	virtual ~NormalCubeMapTex();

	//! Create the normalization cube map texture.
	//! If the texture already exists it return false immediately.
	Bool create();

	//! Delete the texture
	virtual void destroy();

	//! set the texture parameter, such as enable extra coord for cubemap...
	virtual void set();
	//! bind the texture.
	virtual Bool bind();
	//! unbound the texture
	virtual void unbind();
	//! unset the texture parameter
	virtual void unSet();

	//! Get the size.
	inline UInt32 getSize()const { return m_size; }

	//! Get the offset.
	inline Float getOffset()const { return m_offset; }

	// Serialization
	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

protected:

	UInt32 m_size;     //!< size of the cube-map texture (square)
	Float m_offset;    //!< offset computed

	//! Set the filtering mode to OpenGL.
	virtual void setFilteringMode();
	//! Set the warp mode to OpenGL.
	virtual void setWrapMode();
	//! Set the anisotropy level to OpenGL.
	virtual void setAnisotropyLevel();
};

} // namespace o3d

#endif // _O3D_NORMCUBEMAPTEX_H

