/**
 * @file hierarchytree.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/hierarchy/hierarchytree.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine//scene/sceneobjectmanager.h"
#include "o3d/core/classfactory.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/context.h"
#include "o3d/engine/object/primitivemanager.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(RootNode, ENGINE_ROOT_NODE, Node)
O3D_IMPLEMENT_DYNAMIC_CLASS1(HierarchyTree, ENGINE_HIERARCHY_TREE, SceneEntity)

//---------------------------------------------------------------------------------------
// RootNode
//---------------------------------------------------------------------------------------

RootNode::RootNode(BaseObject *parent) :
	Node(parent)
{
	m_name = "RootNode";
}

void RootNode::setUpModelView()
{
	getScene()->getContext()->modelView().identity();
}

// Draw the branch
void RootNode::draw(const DrawInfo &drawInfo)
{
	if (!getActivity() || !getVisibility())
		return;

	setUpModelView();

	getScene()->getPrimitiveManager()->bind();
	getScene()->getPrimitiveManager()->drawLocalAxis();
	getScene()->getPrimitiveManager()->unbind();

	// draw the chidren recursively
	for (IT_SonList it = m_objectList.begin() ; it != m_objectList.end() ; ++it)
	{
		(*it)->draw(drawInfo);
	}
}

/*---------------------------------------------------------------------------------------
  Constructor/destructor
---------------------------------------------------------------------------------------*/
HierarchyTree::HierarchyTree(BaseObject *parent) :
	SceneEntity(parent),
	m_numObject(0),
    m_root(nullptr)
{
	// create the root node
	m_root = new RootNode(parent);
}

HierarchyTree::~HierarchyTree()
{
	deletePtr(m_root);
}

/*---------------------------------------------------------------------------------------
  ajoute un nouveau noeud avec un objet
---------------------------------------------------------------------------------------*/
Node* HierarchyTree::addNode(SceneObject *object)
{
	// ajoute un fils à la racine de la hierarchie
	Node* node = new Node(m_root);
	node->addSonLast(object);

	m_root->addSonLast(node);

	return node;
}

//Node* HierarchyTree::dupTree(const Node *dup, Node *father)
//{
//	O3D_ASSERT(0);
//	return nullptr;
//	// @todo

//	// le nouveau noeud
//	Node* lnode = new Node(*dup);

//	// liste des fils du noeud à dupliquer
//	const T_SonList& SonList = dup->getSonList();

//	// copie tout les fils récursivement
//	for (CIT_SonList it = SonList.begin() ; it != SonList.end() ; ++it)
//	{
//		Node* NewSon = dupTree((*it),lnode);
//		NewSon->setFather(lnode);
//		lnode->addSonLast(*NewSon);
//	}

//	if (father)
//		lnode->setFather(father);

//	return lnode;
//}

/*---------------------------------------------------------------------------------------
  met à jour tous les objet de l'arbre
---------------------------------------------------------------------------------------*/
void HierarchyTree::update()
{
	m_root->update();
}

//! Get a scene object by its name (read only)
const SceneObject* HierarchyTree::findSon(const String &name) const
{
	if (!m_root)
        return nullptr;

	return m_root->findSon(name);
}

//! Get a scene object by its name
SceneObject* HierarchyTree::findSon(const String &name)
{
	if (!m_root)
        return nullptr;

	return m_root->findSon(name);
}

// Find a scene object and return true if found
Bool HierarchyTree::findSon(SceneObject *object) const
{
	if (!m_root)
        return False;

	return m_root->findSon(object);
}

// Get the number of element (count recursively any object)
UInt32 HierarchyTree::getNumElt() const
{
	const T_SonList &sonList = m_root->getSonList();
	UInt32 count = sonList.size();

	for (CIT_SonList it = sonList.begin() ; it != sonList.end() ; ++it)
	{
		if ((*it)->isNodeObject())
			count += ((Node*)(*it))->getNumElt();
	}

	return count;
}

// serialisation
Bool HierarchyTree::writeToFile(OutStream &os)
{
    os << ENGINE_HIERARCHY_TREE;

	T_SonList *sonList = const_cast<T_SonList*>(&m_root->getSonList());
    os << (UInt32)sonList->size();

	// write each son (recursivly if other nodes)
	for (IT_SonList it = sonList->begin() ; it != sonList->end() ; ++it)
	{
		SceneObject *object = (*it);

		if (getScene()->getCurSceneIO().isIO(object))
            ClassFactory::writeToFile(os,*object);
	}

	return True;
}

Bool HierarchyTree::writeToFile(OutStream &os, Node* startNode)
{
    os << ENGINE_HIERARCHY_TREE;

	// one root object
    os << (UInt32)1;

	// export each son recursively
    return startNode->writeToFile(os);
}

Bool HierarchyTree::readFromFile(InStream &is)
{
	UInt32 tmp;
    is >> tmp;

	if (tmp != ENGINE_HIERARCHY_TREE)
        O3D_ERROR(E_InvalidFormat("Invalid hierarchy tree token"));

	// importe le nombre d'objet
	UInt32 num = 0;
    is >> num;
	m_numObject += num;

	m_lastImportedObjects.clear();

	// import each son recursivly
	for (UInt32 i = 0; i < num; ++i)
	{
		// import class name
		String className;
        is >> className;

		// clone a class instance
        BaseObject* object = ClassFactory::getInstanceOfClassInfo(className)->createInstance(this);

		if (typeOf<SceneObject>(object))
		{
			m_root->addSonLast(dynamicCast<SceneObject*>(object));

			// and read the object
            is >> *object;

			// set imported object
			getScene()->getSceneObjectManager()->setImportedSceneObject(
					object->getSerializeId(),
					reinterpret_cast<SceneObject*>(object));

			m_lastImportedObjects.push_back(reinterpret_cast<SceneObject*>(object));
		}
		else
            O3D_ERROR(E_InvalidFormat("Invalid object type, must be a scene object"));
	}

	return True;
}

void HierarchyTree::preExportPass()
{
	T_SonList *sonList = const_cast<T_SonList*>(&m_root->getSonList());

	for (IT_SonList it = sonList->begin() ; it != sonList->end() ; ++it)
	{
		(*it)->preExportPass();
	}
}
