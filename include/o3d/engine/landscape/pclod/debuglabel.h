/**
 * @file debuglabel.h
 * @brief Contains the definition of world labels.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2008-04-29
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PCLODDEBUGLABEL_H
#define _O3D_PCLODDEBUGLABEL_H

#include "o3d/engine/object/worldlabel.h"
#include "o3d/engine/shader/shader.h"

namespace o3d {

class Texture2D;

class Text2D;
class FrameBuffer;

class PCLODColormap;
class PCLODLightmap;

//---------------------------------------------------------------------------------------
//! @class PCLODDebugLabel
//-------------------------------------------------------------------------------------
//! PCLOD debug label.
//---------------------------------------------------------------------------------------
class PCLODDebugLabel : public WorldLabel, NonCopyable<>
{
public:

	//! Default constructor
	PCLODDebugLabel(BaseObject * _pParent);
	//! A destructor
	virtual ~PCLODDebugLabel();

	//! Update the content of the label
    //! The text and the FBO are shared among all debug label and provided by the texture
	//! manager.
    void mtUpdateContent(Text2D*, FrameBuffer*, PCLODColormap *, PCLODLightmap *);

	virtual void drawContent();

	void setPosition(const Vector3 & _position) { m_position = _position; }
	const Vector3 getPosition() const { return m_position; }

private:

	Texture2D *m_pTexture;

	String m_titleText;
	String m_lightmapText;
	String m_colormapText;
	Int32 m_lightCount;

	Vector3 m_position;

	VertexBufferObjf m_drawVerticesVbo;
	VertexBufferObjf m_drawTexCoordsVbo;

	ShaderInstance m_ambientColor;
	ShaderInstance m_ambientMap;

	void initShaders();
};

} // namespace o3d

#endif // _O3D_PCLODDEBUGLABEL_H

