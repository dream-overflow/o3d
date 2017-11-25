/**
 * @file hierarchytree.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_HIERARCHYTREE_H
#define _O3D_HIERARCHYTREE_H

#include "o3d/engine/scene/sceneobject.h"
#include "o3d/core/memorydbg.h"
#include "node.h"

namespace o3d {

typedef std::list<SceneObject*> T_SceneObjectList;
typedef T_SceneObjectList::iterator IT_SceneObjectList;
typedef T_SceneObjectList::const_iterator CIT_SceneObjectList;

/**
 * @brief Root node is a special case of node.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-02-25
 */
class O3D_API RootNode : public Node
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(RootNode)

	//! constructor
	RootNode(BaseObject *parent);

	virtual void setUpModelView();

	//! draw
	virtual void draw(const DrawInfo &drawInfo);
};


/**
 * @brief Hierarchical object tree for any SceneObject and nodes.
 * @date 2003-02-25
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class O3D_API HierarchyTree : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(HierarchyTree)

	//! ctor
	HierarchyTree(BaseObject *parent);

	//! dtor
	virtual ~HierarchyTree();

    //! Create a new node containing a given object and return this new node or null.
	Node* addNode(SceneObject *object);

	//! Duplicate a subtree and return it
    //Node* dupTree(const Node *dup, Node *newFather = nullptr);

	//! update all the tree
	void update();


	//! Get a scene object by its name (read only)
	const SceneObject* findSon(const String &name) const;
	//! Get a scene object by its name
	SceneObject* findSon(const String &name);

	//! Find a scene object and return true if found
	Bool findSon(SceneObject *object) const;


	//! get the root tree node
	inline       RootNode* getRootNode()       { return m_root; }
	//! get the root tree node (read only)
	inline const RootNode* getRootNode() const { return m_root; }

	//! Get the number of element (count recursively any object)
	UInt32 getNumElt() const;

	//! get the number of imported object of the tree
	inline UInt32 getNumObject() { return m_numObject; }


	// Serialization

	//! Export from the root
	virtual Bool writeToFile(OutStream &os);

	//! Export from a specified starting node
	Bool writeToFile(OutStream &os, Node *startNode);

	//! Import the tree as child of the root
	virtual Bool readFromFile(InStream &is);

	//! get the last imported root objects list
	inline const T_SceneObjectList& getLastImportedObjects() const { return m_lastImportedObjects; }

	//! clear the the last imported root objects list
	inline void clearLastImportedObjects() { m_lastImportedObjects.clear(); }

	void preExportPass();

protected:

	UInt32 m_numObject;  //!< num of object in the tree
    RootNode* m_root;    //!< root of the tree

	T_SceneObjectList m_lastImportedObjects;
};

} // namespace o3d

#endif // _O3D_HIERARCHYTREE_H
