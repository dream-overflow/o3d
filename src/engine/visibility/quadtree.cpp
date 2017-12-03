/**
 * @file quadtree.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/visibility/quadtree.h"

#include "o3d/engine/visibility/visibilitymanager.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/object/camera.h"
#include "o3d/geom/frustum.h"
#include "o3d/core/vector2.h"
#include "o3d/core/vector3.h"
#include "o3d/geom/plane.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/object/primitivemanager.h"

#include <algorithm>

using namespace o3d;

//---------------------------------------------------------------------------------------
// class QuadObject
//---------------------------------------------------------------------------------------

QuadObject::QuadObject(Quadtree * _pQuadTree, SceneObject * _object):
	m_pObject(_object),
	m_pQuadTree(_pQuadTree)
{
}

QuadObject::~QuadObject()
{
    onDestroyed();
}

void QuadObject::addZoneContainer(QuadZone * _zone)
{
	O3D_ASSERT(std::find(m_zoneList.begin(), m_zoneList.end(), _zone) == m_zoneList.end());

	m_zoneList.push_back(_zone);
}

void QuadObject::removeZoneContainer(QuadZone * _zone)
{
	IT_QuadZoneList it = std::find(m_zoneList.begin(), m_zoneList.end(), _zone);

	O3D_ASSERT(it != m_zoneList.end());

	m_zoneList.erase(it);

    if (m_zoneList.size() == 0) {
        onUnused();
    }
}

//---------------------------------------------------------------------------------------
// class QuadZone
//---------------------------------------------------------------------------------------
QuadZone::QuadZone(Quadtree * _quad, Vector2i _position, Float _size):
	m_pQuadTree(_quad),
    m_pParent(nullptr),
	m_position(_position),
	m_size(_size),
	m_subPosition(),
	m_objectList()
{
	memset((void*)m_pChildren, 0, 4*sizeof(QuadZone*));
}

QuadZone::~QuadZone()
{
    for (UInt32 k = 0; k < 4; ++k) {
		deletePtr(m_pChildren[k]);
    }

	m_subPosition.clear();
	m_size = 0.0f;
	m_position.zero();
    m_pQuadTree = nullptr;

    if (m_pParent != nullptr) {
        for (UInt32 k = 0; k < 4 ; ++k) {
            if (m_pParent->m_pChildren[k] == this) {
                m_pParent->m_pChildren[k] = nullptr;
				break;
            } else if (k == 3) {
				O3D_ASSERT(0);
			}
        }
	}

	removeAllObjects();
}

void QuadZone::draw(Scene *scene)
{
	Vector3 absPosition = getAbsolutePosition();
	PrimitiveManager *primitive = scene->getPrimitiveManager();

	primitive->modelView().push();
	primitive->modelView().translate(absPosition);
	primitive->setModelviewProjection();

    // yellow
    primitive->setColor(1.0f, 1.0f, 0.0f);

	// We draw the edges of this zone
    // primitive->drawYAxisAlignedQuad(P_LINE_LOOP, Vector3(m_size, 1, m_size));

    // Daw corners of this zone
    primitive->beginDraw(P_LINES);

    // top left
    primitive->addVertex(Vector3(-m_size, 0, -m_size));
    primitive->addVertex(Vector3(-m_size*0.8, 0, -m_size));

    primitive->addVertex(Vector3(-m_size, 0, -m_size));
    primitive->addVertex(Vector3(-m_size, 0, -m_size*0.8));

    // top right
    primitive->addVertex(Vector3(m_size, 0, -m_size));
    primitive->addVertex(Vector3(m_size*0.8, 0, -m_size));

    primitive->addVertex(Vector3(m_size, 0, -m_size));
    primitive->addVertex(Vector3(m_size, 0, -m_size*0.8));

    // bottom left
    primitive->addVertex(Vector3(-m_size, 0, m_size));
    primitive->addVertex(Vector3(-m_size*0.8, 0, m_size));

    primitive->addVertex(Vector3(-m_size, 0, m_size));
    primitive->addVertex(Vector3(-m_size, 0, m_size*0.8));

    // bottom right
    primitive->addVertex(Vector3(m_size, 0, m_size));
    primitive->addVertex(Vector3(m_size*0.8, 0, m_size));

    primitive->addVertex(Vector3(m_size, 0, m_size));
    primitive->addVertex(Vector3(m_size, 0, m_size*0.8));

    primitive->endDraw();

//    // Now we draw connections
//    Vector3 absCenter = absPosition + Vector3(0.5f * m_size, 0.0f, 0.5f * m_size);
//
//    primitive->beginDraw(P_LINES);
//        for (IT_ZoneObjectList it = m_objectList.begin(); it != m_objectList.end() ; it++)
//        {
//            primitive->addVertex(absCenter.getData());
//            primitive->addVertex((*it)->getSceneObject()->getAbsoluteMatrix().getTranslationPtr());
//        }
//    primitive->endDraw();

	primitive->setColor(1.0f, 0.0f, 0.0f);
	primitive->modelView().pop();

    for (UInt32 k = 0; k < 4 ; ++k) {
        if (m_pChildren[k] != nullptr) {
			m_pChildren[k]->draw(scene);
        }
	}
}

Vector3 QuadZone::getAbsolutePosition() const
{
	return (m_pQuadTree->getQuadCenterOrigin() + Vector3(Float(m_position[QUAD_X]), 0.0f, Float(m_position[QUAD_Z])) * m_size);
}

Vector3 QuadZone::getAbsoluteCenter() const
{
	return (m_pQuadTree->getQuadCenter() + Vector3(m_position[QUAD_X] * m_size, 0.0f, m_position[QUAD_Z] * m_size));
}


Bool QuadZone::hasChildren(QuadZone * _zone) const
{
    if ((m_pChildren[0] != _zone) && (m_pChildren[1] != _zone) && (m_pChildren[2] != _zone) && (m_pChildren[3] != _zone)) {
		return False;
    } else {
		return True;
    }
}

Bool QuadZone::findZone(QuadZone * _zone) const
{
    if ((m_pChildren[0] != _zone) && (m_pChildren[1] != _zone) && (m_pChildren[2] != _zone) && (m_pChildren[3] != _zone)) {
        for (UInt32 k = 0; k < 4; ++k) {
            if ((m_pChildren[k] != nullptr) && m_pChildren[k]->findZone(_zone)) {
				return True;
            }
        }

		return False;
    } else {
		return True;
    }
}

// Add/Remove an object from the zone
void QuadZone::addObject(QuadObject * _object)
{
    O3D_ASSERT(_object != nullptr);

	// We add this zone in the _object
	_object->addZoneContainer(this);

	m_objectList.push_back(_object);

    _object->onDestroyed.connect(this, &QuadZone::onObjectDeletion);
}

void QuadZone::removeObject(QuadObject * _object)
{
    O3D_ASSERT(_object != nullptr);

	IT_ZoneObjectList it = std::find(m_objectList.begin(), m_objectList.end(), _object);
	O3D_ASSERT(it != m_objectList.end());

	m_objectList.erase(it);

	_object->removeZoneContainer(this);
	disconnect(_object);
}

void QuadZone::removeAllObjects()
{
    for (IT_ZoneObjectList it = m_objectList.begin() ; it != m_objectList.end() ; it++) {
		(*it)->removeZoneContainer(this);
		disconnect(*it);
	}

	m_objectList.clear();
}

QuadObject * QuadZone::findObject(SceneObject * _object)
{
    for(IT_ZoneObjectList it = m_objectList.begin() ; it != m_objectList.end() ; it++) {
        if ((*it)->getSceneObject() == _object) {
			return *it;
        }
    }

	QuadObject * pObject;

    for (UInt32 k = 0; k < 4 ; ++k) {
        if ((m_pChildren[k] != nullptr) && ((pObject = m_pChildren[k]->findObject(_object)) != nullptr)) {
			return pObject;
        }
    }

    return nullptr;
}

const QuadObject * QuadZone::findObject(SceneObject * _object) const
{
    for(CIT_ZoneObjectList it = m_objectList.begin() ; it != m_objectList.end() ; it++) {
        if ((*it)->getSceneObject() == _object) {
			return *it;
        }
    }

	QuadObject * pObject;

    for (UInt32 k = 0; k < 4 ; ++k) {
        if ((m_pChildren[k] != nullptr) && ((pObject = m_pChildren[k]->findObject(_object)) != nullptr)) {
			return pObject;
        }
    }

    return nullptr;
}

void QuadZone::setParent(QuadZone * _parent)
{
    O3D_ASSERT(m_pParent == nullptr);

	m_pParent = _parent;
}

void QuadZone::onObjectDeletion()
{
	EvtHandler * lSenderObject = getSender();

	IT_ZoneObjectList it = std::find(m_objectList.begin(), m_objectList.end(), lSenderObject);
	O3D_ASSERT(it != m_objectList.end());

	m_objectList.erase(it);
}

//---------------------------------------------------------------------------------------
// class QuadTree
//---------------------------------------------------------------------------------------

O3D_IMPLEMENT_DYNAMIC_CLASS1(Quadtree, ENGINE_VISIBILITY_QUADTREE, VisibilityABC)

Quadtree::Quadtree(
	BaseObject *parent,
	UInt32 halfSize,
	Float zoneSize) :
		VisibilityABC(parent, Vector3(), Vector3(zoneSize*halfSize, 0.f, zoneSize*halfSize)),
		m_topZone(2*halfSize+1, 2*halfSize+1),
		m_objectMap(),
		m_zoneSize(zoneSize),
		m_center(0.0f, 0.0f, 0.0f),
		m_hysteresis(0.2f * zoneSize),
		m_currentPosition(0.0f, 0.0f, 0.0f)
{
    for (UInt32 j = 0 ; j < m_topZone.height() ; ++j) {
        for (UInt32 i = 0 ; i < m_topZone.width() ; ++i) {
			m_topZone(i,j) = new QuadZone(this, Vector2i(i - halfSize, j - halfSize), m_zoneSize);
		}
	}
}

Quadtree::~Quadtree()
{
	clear();
}

void Quadtree::translate(const Vector2i & _move)
{
	// TODO
	// Optimize with memcpy

	m_center += Vector3(_move[QUAD_X] * m_zoneSize, 0.0f, _move[QUAD_Z] * m_zoneSize);
	m_bbox.setCenter(m_center);

    if (UInt32(_move.normInf()) >= m_topZone.width()) {
		Int32 halfSize = m_topZone.width() / 2;
		Int32 i,j;

		// We must destroy all the array
        for (UInt32 k = 0; k < m_topZone.elt() ; ++k) {
			m_topZone[k]->removeAllObjects();
			m_topZone[k]->setSize(m_zoneSize);

			i = Int32(k % m_topZone.width()) - halfSize;
			j = Int32(k / m_topZone.width()) - halfSize;

			m_topZone[k]->setPosition(Vector2i(i,j));
		}

		return;
	}

    if ((_move[QUAD_Z] > 0) && (_move[QUAD_Z] < Int32(m_topZone.width()))) {
		Int32 halfSize = m_topZone.width() / 2;

        for (UInt32 j = 0; j < m_topZone.height(); ++j) {
            for (UInt32 i = 0 ; i < UInt32(_move[QUAD_Z]) ; ++i) {
				deletePtr(m_topZone(i,j));
            }
        }

        for (UInt32 j = 0; j < m_topZone.height(); ++j) {
            for (UInt32 i = 0 ; i < UInt32(m_topZone.width() - _move[QUAD_Z]) ; ++i) {
				m_topZone(i,j) = m_topZone(i + _move[QUAD_Z], j);
				m_topZone(i,j)->setPosition(Vector2i(i - halfSize, j - halfSize));
			}
        }

        for (UInt32 j = 0; j < m_topZone.height(); ++j) {
            for (UInt32 i = m_topZone.width() - _move[QUAD_Z]; i < m_topZone.width() ; ++i) {
				m_topZone(i,j) = new QuadZone(this, Vector2i(i - halfSize, j - halfSize), m_zoneSize);
            }
        }
    } else if ((_move[QUAD_Z] < 0) && (_move[QUAD_Z] > -Int32(m_topZone.width()))) {
		Int32 halfSize = m_topZone.width() / 2;

        for (UInt32 j = 0; j < m_topZone.height(); ++j) {
            for (UInt32 i = m_topZone.width() + _move[QUAD_Z] ; i < m_topZone.width() ; ++i) {
				deletePtr(m_topZone(i,j));
            }
        }

        for (UInt32 j = 0; j < m_topZone.height(); ++j) {
            for (UInt32 i = m_topZone.width() - 1 ; i >= UInt32(-_move[QUAD_Z]) ; --i) {
				m_topZone(i,j) = m_topZone(i + _move[QUAD_Z], j);
				m_topZone(i,j)->setPosition(Vector2i(i - halfSize, j - halfSize));
			}
        }

        for (UInt32 j = 0; j < m_topZone.height(); ++j) {
            for (UInt32 i = 0; i < UInt32(-_move[QUAD_Z]) ; ++i) {
				m_topZone(i,j) = new QuadZone(this, Vector2i(i - halfSize, j - halfSize), m_zoneSize);
            }
        }
	}

    if ((_move[QUAD_X] > 0) && (_move[QUAD_X] < Int32(m_topZone.height()))) {
		Int32 halfSize = m_topZone.height() / 2;

        for (UInt32 i = 0; i < m_topZone.width(); ++i) {
            for (UInt32 j = 0 ; j < UInt32(_move[QUAD_X]) ; ++j) {
				deletePtr(m_topZone(i,j));
            }
        }

        for (UInt32 i = 0; i < m_topZone.width(); ++i) {
            for (UInt32 j = 0 ; j < UInt32(m_topZone.height() - _move[QUAD_X]) ; ++j) {
				m_topZone(i,j) = m_topZone(i, j + _move[QUAD_X]);
				m_topZone(i,j)->setPosition(Vector2i(i - halfSize, j - halfSize));
			}
        }

        for (UInt32 i = 0; i < m_topZone.width(); ++i) {
            for (UInt32 j = m_topZone.height() - _move[QUAD_X]; j < m_topZone.height() ; ++j) {
				m_topZone(i,j) = new QuadZone(this, Vector2i(i - halfSize, j - halfSize), m_zoneSize);
            }
        }
    } else if ((_move[QUAD_X] < 0) && (_move[QUAD_X] > -Int32(m_topZone.height()))) {
		Int32 halfSize = m_topZone.height() / 2;

        for (UInt32 i = 0; i < m_topZone.width(); ++i) {
            for (UInt32 j = m_topZone.height() + _move[QUAD_X] ; j < m_topZone.height() ; ++j) {
				deletePtr(m_topZone(i,j));
            }
        }

        for (UInt32 i = 0; i < m_topZone.width(); ++i) {
            for (UInt32 j = m_topZone.height() - 1 ; j >= UInt32(-_move[QUAD_X]) ; --j) {
				m_topZone(i,j) = m_topZone(i, j + _move[QUAD_X]);
				m_topZone(i,j)->setPosition(Vector2i(i - halfSize, j - halfSize));
			}
        }

        for (UInt32 i = 0; i < m_topZone.width(); ++i) {
            for (UInt32 j = 0; j < UInt32(-_move[QUAD_X]) ; ++j) {
				m_topZone(i,j) = new QuadZone(this, Vector2i(i - halfSize, j - halfSize), m_zoneSize);
            }
        }
	}
}

void Quadtree::onObjectUnused()
{
	QuadObject * lObject = static_cast<QuadObject*>(getSender());

	IT_ObjectMap it = m_objectMap.find(lObject->getSceneObject());
	O3D_ASSERT(it != m_objectMap.end());

    if (it != m_objectMap.end()) {
		m_objectMap.erase(it);
    }

	deletePtr(lObject);
}

// Clear all objects contained in the quadTree
void Quadtree::clear()
{
    for (UInt32 k = 0 ; k < m_topZone.elt() ; ++k) {
		deletePtr(m_topZone[k]);
    }

	m_topZone.free();

	EvtManager::instance()->processEvent(this);
}

inline UInt32 Quadtree::getNumObjects()const
{
	return UInt32(m_objectMap.size());
}

// Return the neighbor of a zone
QuadZone * Quadtree::getNeighbor(const QuadZone & _zone, QuadDirection _direction)
{
	O3D_ASSERT(!_zone.hasParent());

	Int32 halfSize = m_topZone.width() / 2;

	Vector2i zoneRequested(_zone.getPosition());

    switch (_direction) {
		case NORTH: zoneRequested[QUAD_X]++; break;
		case SOUTH: zoneRequested[QUAD_X]--; break;
		case EAST: zoneRequested[QUAD_Z]++; break;
		case WEST: zoneRequested[QUAD_Z]--; break;
		default: O3D_ASSERT(0); break;
	}

    // If the requested neighbor is outside the quadtree range, we return nullptr
    if (zoneRequested.normInf() > halfSize) {
        return nullptr;
    } else {
		return m_topZone(zoneRequested[QUAD_Z], zoneRequested[QUAD_X]);
    }
}

// Add an object (we suppose that it doesn't exist)
void Quadtree::addObject(SceneObject *object)
{
	Vector3 zoneOrigin(getQuadCenterOrigin());

	Vector3 diff = object->getAbsoluteMatrix().getTranslation() - zoneOrigin;
	diff /= m_zoneSize;

	Vector2i relativPos(Int32(floorf(diff[Z])), Int32(floorf(diff[X])));
	Int32 halfSize = m_topZone.width() / 2;

	// The quadtree is a square so ...
    if (relativPos.normInf() > halfSize) {
		// The object is outside the range of the quadtree
        // O3D_ERROR(E_InvalidParameter("Attempt to add an object outside the quadtree range"));
        O3D_WARNING("Attempt to add an object outside the quadtree range");
		return;
	}

	QuadObject * newQuadObject = new QuadObject(this, object);
	m_topZone(halfSize + relativPos[QUAD_Z], halfSize + relativPos[QUAD_X])->addObject(newQuadObject);

    newQuadObject->onUnused.connect(this, &Quadtree::onObjectUnused, CONNECTION_ASYNCH);

	// Finally we add the object in the map
	m_objectMap[object] = newQuadObject;
}

// Remove an object. Function called by the user
Bool Quadtree::removeObject(SceneObject *object)
{
    if (object) {
		IT_ObjectMap it = m_objectMap.find(object);
		//O3D_ASSERT(it != m_objectMap.end());

        if (it != m_objectMap.end()) {
			deletePtr(it->second);

			m_objectMap.erase(it);
			return True;
		}

		return False;
    } else {
		O3D_ERROR(E_InvalidParameter("object must be non null"));
		return False;
	}
}

// Update an object
void Quadtree::updateObject(SceneObject *object)
{
    if (object) {
		IT_ObjectMap it = m_objectMap.find(object);
		O3D_ASSERT(it != m_objectMap.end());

		// What must be done ?

		QuadObject * pQuadObject = it->second;
		O3D_ASSERT(pQuadObject->getZoneNumber() > 0);

		// Only one owner for each object is supported yet
		//O3DVector3 relative = object->getAbsoluteMatrix().GetTranslation() - pQuadObject->getZoneList()[0]->getAbsoluteCenter();
		Vector3 center = pQuadObject->getZoneList()[0]->getAbsoluteCenter();

		// left plane
		Plane leftPlane(Vector3(-1.f, 0.f, 0.f), Vector3(center.x() - m_zoneSize * 0.5f, 0.f, 0.f));
		Geometry::Clipping left = object->checkBounding(leftPlane);

        if (left != Geometry::CLIP_OUTSIDE) {
			//O3DApps::Message(O3DString("Change zone ") << object->getName(), "");
			// Outside the previous zone
			removeObject(object);	// Can be optimized a lot
			addObject(object);      // Same here
			return;
		}

		// right plane
		Plane rightPlane(Vector3(1.f, 0.f, 0.f), Vector3(center.x() + m_zoneSize * 0.5f, 0.f, 0.f));
		Geometry::Clipping right = object->checkBounding(rightPlane);

        if (right != Geometry::CLIP_OUTSIDE) {
			//O3DApps::Message(O3DString("Change zone ") << object->getName(), "");
			// Outside the previous zone
			removeObject(object);	// Can be optimized a lot
			addObject(object);      // Same here
			return;
		}

		// bottom plane
		Plane bottomPlane(Vector3(0.f, 0.f, -1.f), Vector3(0.f, 0.f, center.z() - m_zoneSize * 0.5f));
		Geometry::Clipping bottom = object->checkBounding(bottomPlane);

        if (bottom != Geometry::CLIP_OUTSIDE) {
			//O3DApps::Message(O3DString("Change zone ") << object->getName(), "");
			// Outside the previous zone
			removeObject(object);	// Can be optimized a lot
			addObject(object);      // Same here
			return;
		}

		// top plane
		Plane topPlane(Vector3(0.f, 0.f, 1.f), Vector3(0.f, 0.f, center.z() + m_zoneSize * 0.5f));
		Geometry::Clipping top = object->checkBounding(topPlane);

        if (top != Geometry::CLIP_OUTSIDE) {
            //System::print(String("Change zone ") << object->getName(), "");
			// Outside the previous zone
			removeObject(object);	// Can be optimized a lot
			addObject(object);      // Same here
			return;
		}

		/*if (((left != Geometry::CLIP_OUTSIDE) ||
			(right != Geometry::CLIP_OUTSIDE) ||
			(top != Geometry::CLIP_OUTSIDE) ||
            (bottom != Geometry::CLIP_OUTSIDE))) {
			// Outside the previous zone
			removeObject(object);	// Can be optimized a lot
            //System::print(String("Change zone ") << object->getName(), "");
			addObject(object);      // Same here
		}*/

        /*if (relative.normInf() > m_zoneSize) {
			// Outside the previous zone
			removeObject(object);	// Can be optimized a lot
			printf("change zone %s\n", object->getName().toUtf8().getData());
			addObject(object);      // Same here
		}*/
	}
}

// Check for visible object and add it to visibility manager
void Quadtree::checkVisibleObject(const VisibilityInfos & _infos)
{
	m_currentPosition = _infos.cameraPosition;

	Vector2f relativPos(m_currentPosition[Z] - m_center[Z], m_currentPosition[X] - m_center[X]);

    if (relativPos.normInf() > 0.5f*m_zoneSize + m_hysteresis) {
		// We need to change the quadtree
		// Which parts of the quadtree need to be changed

		relativPos /= m_zoneSize;
		relativPos[QUAD_Z] = floorf(relativPos[QUAD_Z] + 0.5f);
		relativPos[QUAD_X] = floorf(relativPos[QUAD_X] + 0.5f);

		Vector2i translation((Int32)relativPos[QUAD_Z], (Int32)relativPos[QUAD_X]);

		//if (translation.norm1() > 0)
			translate(translation);
	}

    SceneObject * object = nullptr;

    for (UInt32 k = 0; k < m_topZone.elt() ; ++k) {
		// Visibility checks ...

        for (CIT_ZoneObjectList it = m_topZone[k]->getObjectList().begin() ; it != m_topZone[k]->getObjectList().end() ; it++) {
			object = (*it)->getSceneObject();

            if (_infos.viewUseMaxDistance) {
				Float length = (object->getAbsoluteMatrix().getTranslation() - m_currentPosition).length();

                if (length > _infos.viewMaxDistance) {
					continue;
                }
			}

			getScene()->getVisibilityManager()->addObjectToDraw(object);
		}
	}
}

void Quadtree::draw()
{
    if (getScene()->getDrawObject(Scene::DRAW_QUADTREE)) {
		PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access();

		// setup modelview
		primitive->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix());

        primitive->setColor(1.f, 1.f, 1.f);

        for (UInt32 j = 0 ; j < m_topZone.height() ; ++j) {
            for (UInt32 i = 0 ; i < m_topZone.width() ; ++i) {
				m_topZone(i,j)->draw(getScene());
			}
		}

		primitive->setColor(0.0f, 1.0f, 0.0f);

		//primitive->modelView().push();
		//	Vector3 absPos(GetQuadOrigin());
		//	primitive->modelView().translate(absPos);
		//	primitive->wireSphere1(Vector3(20.f,20.f,20.f));
		//primitive->modelView().pop()
	}
}
