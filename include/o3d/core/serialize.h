/**
 * @file serialize.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SERIALIZE_H
#define _O3D_SERIALIZE_H

#include "instream.h"
#include "outstream.h"

namespace o3d {

//---------------------------------------------------------------------------------------
// class name
//---------------------------------------------------------------------------------------

//! Define for no parent class whose want to be serialized
#define O3D_DEFINE_SERIALIZATION(class)                                       \
    public:                                                                   \
    virtual String getClassName() const override { return String(#class); }   \
    static Serialize* createInstance() { return new class; }                  \
    virtual Serialize* makeInstance() const override { return new class(); }

//! Define for no parent template class whose want to be serialized
#define O3D_DEFINE_SERIALIZATION_TEMPLATE(class,T)                                \
    public:                                                                       \
    virtual String getClassName() const override { return String(#class); }       \
    static Serialize* createInstance() { return new class<T>; }                   \
    virtual Serialize* makeInstance() const override { return (new class<T>()); }

/**
 * @brief Used for serialize classes data to files
 */
class O3D_API Serialize
{
public:

    //! Virtual destructor.
    virtual ~Serialize() = 0;

    //! Serialize the object from a file.
    virtual Bool readFromFile(InStream &istream) = 0;
    //! Serialize the object to a file.
    virtual Bool writeToFile(OutStream &ostream) = 0;

    //! Return the name of the class.
    virtual String getClassName() const = 0;

    //! Make an instance of the object.
    virtual Serialize* makeInstance() const = 0;
};

} // namespace o3d

#endif // _O3D_SERIALIZE_H
