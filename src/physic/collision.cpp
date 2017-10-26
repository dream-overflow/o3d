/**
 * @file collision.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/physic/precompiled.h"
#include "o3d/physic/collision.h"

using namespace o3d;

//---------------------------------------------------------------------------------------
// constructor
//---------------------------------------------------------------------------------------
Collision::Collision() :
    m_pBodyA(nullptr),
    m_pBodyB(nullptr),
	m_IsvertexFace(False)
{

}

//---------------------------------------------------------------------------------------
// destructor
//---------------------------------------------------------------------------------------
Collision::~Collision()
{

}

//---------------------------------------------------------------------------------------
// Add an impulse on both rigid body to avoid penetration
//---------------------------------------------------------------------------------------
void Collision::resolve()
{
	// TODO
}

