/**
 * @file shadableobject.h
 * @brief A shadable object model that inherit from SceneObject.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SHADABLEOBJECT_H
#define _O3D_SHADABLEOBJECT_H

#include "../scene/sceneobject.h"

#include "facearray.h"
#include "../material/material.h"
#include "o3d/core/templatearray.h"
#include "o3d/core/smartpointer.h"

namespace o3d {

/**
 * @brief A shadable object model that inherit from SceneObject.
 * @details You can simply inherit from ShadableObject directly rather than SceneObject
 * for shadable object. It is mainly used by O3DMesh and its inherited classes. You
 * can use it as an implementation example or as a base.
 */
class O3D_API ShadableObject : public SceneObject
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(ShadableObject)

	//! Default constructor
	ShadableObject(BaseObject *parent);

	//! Copy constructor
	ShadableObject(const ShadableObject &dup);

	//! Duplicate
	ShadableObject& operator= (const ShadableObject &dup);

	//! get the shadable shader program to use
    virtual VertexProgramType getVertexProgramType() const override;

	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

    //! Set an external face array to process. It is needed for the alpha pipeline.
    //! Only P_TRIANGLE can be processed with the way.
	virtual void useExternalFaceArray(
		FaceArray *faceArray,
		UInt32 numFaces,
		UInt32 firstFace,
        UInt32 lastFace) override;

	//! return true if there is a specified list of faces
    virtual Bool isExternalFaceArray() const override;

	//! get the specified faces list
    virtual FaceArray* getFaceArrayToProcess(UInt32 &first, UInt32 &last) override;

protected:

	//! Shadable data related into a struct.
	struct ShadableInfo
	{
		FaceArray *faceArray;   //!< External face array to process
        UInt32 numFaces;        //!< If m_faceArray the number of faces to process
        UInt32 firstFace;       //!< If m_faceArray the first face index to process
        UInt32 lastFace;        //!< If m_faceArray the last face index to process

		Shadable::VertexProgramType vertexProgram;        //!< Try to use this vertex program.
		Shadable::VertexProgramType activeVertexProgram;  //!< Really used vertex program.
	};

	ShadableInfo m_shadableInfo;
};

} // namespace o3d

#endif // _O3D_SHADABLEOBJECT_H
