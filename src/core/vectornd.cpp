/**
 * @file vectornd.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2003-07-2
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/vectornd.h"
#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

#include <math.h>

using namespace o3d;

/*---------------------------------------------------------------------------------------
  set tout les élements de V avec val
---------------------------------------------------------------------------------------*/
void VectorND::setValue(Float val)
{
    for (UInt32 i = 0 ; i < dim ; ++i)
		V[i] = val;
}

/*---------------------------------------------------------------------------------------
  renvoie la longueur du vecteur (norme)
---------------------------------------------------------------------------------------*/
Float VectorND::length()const
{
	Float val = 0;

	for (UInt32 i = 0 ; i < dim ; ++i)
		val += (V[i] * V[i]);

	return (Float)sqrtf(val);
}
	
/*---------------------------------------------------------------------------------------
  renvoie la longueur du vecteur au carré (normée)
---------------------------------------------------------------------------------------*/
Float VectorND::squareLength()const
{
	Float val = 0;

	for (UInt32 i = 0 ; i < dim ; ++i)
		val += (V[i] * V[i]);

	return val;
}

/*---------------------------------------------------------------------------------------
  renvoie la norme infinie du vecteur (plus grande valeur absolue)
---------------------------------------------------------------------------------------*/
Float VectorND::normInf()const
{
	Float val = 0;

	for (UInt32 i = 0 ; i < dim ; ++i)
	{
		if (V[i] > val)
			val = V[i];
		if (-V[i] > val)
			val = -V[i];
	}

	return val;
}

/*---------------------------------------------------------------------------------------
  renvoie la norme absolue du vecteur (somme absolue de ses �lements)
---------------------------------------------------------------------------------------*/
Float VectorND::normAbs()const
{
	Float val = 0;

	for (UInt32 i = 0 ; i < dim ; ++i)
		val += o3d::abs(V[i]);

	return val;
}

/*---------------------------------------------------------------------------------------
  ajoute un vecteur � this
---------------------------------------------------------------------------------------*/
VectorND& VectorND::operator+= (const VectorND& vec)
{
	if (dim != vec.getDim()) return (*this);

	for (UInt32 i = 0 ; i < dim ; ++i)
		V[i] += vec.getData(i);

	return (*this);
}

/*---------------------------------------------------------------------------------------
  soustrait un vecteur � this
---------------------------------------------------------------------------------------*/
VectorND& VectorND::operator-= (const VectorND& vec)
{
	if (dim != vec.getDim()) return (*this);

	for (UInt32 i = 0 ; i < dim ; ++i)
		V[i] -= vec.getData(i);

	return (*this);
}

/*---------------------------------------------------------------------------------------
  scalairiser this*=scal
---------------------------------------------------------------------------------------*/
VectorND& VectorND::operator*= (const Float scal)
{
	for (UInt32 i = 0 ; i < dim ; ++i)
		V[i] *= scal;

	return (*this);
}

/*---------------------------------------------------------------------------------------	
  division scalaire this/=scal
---------------------------------------------------------------------------------------*/
VectorND& VectorND::operator/= (const Float scal)
{
	for (UInt32 i = 0 ; i < dim ; ++i)
		V[i] /= scal;

	return (*this);
}

/*---------------------------------------------------------------------------------------
  produit scalaire this*vec
---------------------------------------------------------------------------------------*/
Float VectorND::operator* (const VectorND& v)const
{
	if (dim != v.getDim()) return 0;

	Float val = 0;

	for (UInt32 i = 0 ; i < dim ; ++i)
		val += V[i] * v.getData(i);
    
	return val;
}

/*---------------------------------------------------------------------------------------
  produit scalaire vec*scal
---------------------------------------------------------------------------------------*/
VectorND VectorND::operator* (const Float scal)const
{
	VectorND temp(dim);

	for (UInt32 i = 0 ; i < dim ; ++i)
		temp.setData(i,V[i]*scal);

	return temp;
}

/*---------------------------------------------------------------------------------------
  division scalaire vec/scal
---------------------------------------------------------------------------------------*/
VectorND VectorND::operator/ (const Float scal)const
{
	VectorND temp(dim);

	for (UInt32 i = 0 ; i < dim ; ++i)
		temp.setData(i,V[i]/scal);

	return temp;
}

/*---------------------------------------------------------------------------------------
  addition vectorielle v1 + v2
---------------------------------------------------------------------------------------*/
VectorND VectorND::operator+ (const VectorND& v)const
{
	if (dim != v.getDim()) return VectorND(dim);

	VectorND temp(dim);

	for (UInt32 i = 0 ; i < dim ; ++i)
		temp.setData(i,V[i]+v.getData(i));

	return temp;
}

/*---------------------------------------------------------------------------------------
 soustraction vectorielle v1 - v2
---------------------------------------------------------------------------------------*/
VectorND VectorND::operator- (const VectorND& v)const
{
	if (dim != v.getDim()) return VectorND(dim);

	VectorND temp(dim);

	for (UInt32 i = 0 ; i < dim ; ++i)
		temp.setData(i,V[i]-v.getData(i));

	return temp;
}

/*---------------------------------------------------------------------------------------
 inverse le vecteur this et le renvoi (toutes ses composante sons invers�es (-))
---------------------------------------------------------------------------------------*/
VectorND VectorND::operator- ()const
{
	VectorND temp(dim);

	for (UInt32 i = 0 ; i < dim ; ++i)
		temp.setData(i,-V[i]);

	return temp;
}

/*---------------------------------------------------------------------------------------
  S�rialisation: �crit/lit dans un fichier
---------------------------------------------------------------------------------------*/
Bool VectorND::writeToFile(OutStream &os)const
{
    os << dim;

	for (UInt32 i = 0 ; i < dim ; ++i)
        os << V[i];

	return True;
}

Bool VectorND::readFromFile(InStream &is)
{
    is >> dim;
	reset(dim);

	for (UInt32 i = 0 ; i < dim ; ++i)
        is >> V[i];

	return True;
}

