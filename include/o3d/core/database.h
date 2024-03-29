/**
 * @file database.h
 * @brief Database interface.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-11-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DATABASE_H
#define _O3D_DATABASE_H

#include "string.h"
#include "memorydbg.h"
#include "dbvariable.h"
#include "templatearray.h"
#include "smartarray.h"

#include <vector>
#include <map>

namespace o3d {

class DbQuery;
class DbAttribute;
class Date;
class DateTime;
class InStream;

//template<class T>
//class TemplateArray;

//template<class T>
//class SmartArray;

typedef std::map<String,DbQuery*> T_DbQueryMap;
typedef T_DbQueryMap::iterator IT_DbQueryMap;
typedef T_DbQueryMap::const_iterator CIT_DbQueryMap;

//---------------------------------------------------------------------------------------
//! @class Database
//-------------------------------------------------------------------------------------
//! Database interface.
//---------------------------------------------------------------------------------------
class O3D_API Database
{
public:

	//! Default constructor
    Database() : m_isConnected(False) {}

	//! Virtual destructor
	virtual ~Database();

	//! Connect to a database
    virtual Bool connect(
        const String &host,
        o3d::UInt32 port,
		const String &database,
        const String &user = "",
		const String &password = "",
        Bool keepPassord = True) = 0;

	//! Disconnect from the database server
	virtual void disconnect() = 0;

	//! Is database connected
    inline Bool isConnected() { return m_isConnected; }

	//! Try to maintain the connection established
	virtual void pingConnection() = 0;

	//! Register a new DbQuery
    DbQuery* registerQuery(const String &name, const CString &query);

	//! Unregister an existing DbQuery given its pointer
    Bool unregisterQuery(DbQuery *query);

	//! Unregister an existing DbQuery given its name
    Bool unregisterQuery(const String &name);

	//! Find a query by its name
	DbQuery* findQuery(const String &name);

	//! Unregister all existing DbQuery
	void unregisterAll();

    //! Get the host name
    inline const String& getHost() const { return m_host; }

    //! Get the host db port
    inline UInt32 getPort() const { return m_port; }

	//! Get the database name
    inline const String& getDatabase() const { return m_database; }

    //! Get the user name
    inline const String& getUser() const { return m_user; }

    //! Get the user password
    inline const String& getPassword() const { return m_password; }

	//! Get the number of registred queries
    inline UInt32 getNumRegistredQuery() const { return static_cast<UInt32>(m_registredQuery.size()); }

protected:

	//! Instanciate a new DbQuery object
    virtual DbQuery* newDbQuery(const String &/*name*/, const CString &/*query*/) { return nullptr; }

    Bool m_isConnected;

    String m_host;
    UInt32 m_port;
    String m_database;
    String m_user;
    String m_password;

    T_DbQueryMap m_registredQuery;
};

/**
 * @brief The DbQuery interface.
 * Prepared statement.
 */
class O3D_API DbQuery
{
	friend class Database;

public:

	//! Virtual destructor
    virtual ~DbQuery() = 0;

    //! Set an input variable as ArrayUInt8. The array is duplicated.
    virtual void setArrayUInt8(UInt32 attr, const ArrayUInt8 &v) = 0;

    //! Set an input variable as SmartArrayUInt8. The array is duplicated.
    virtual void setSmartArrayUInt8(UInt32 attr, const SmartArrayUInt8 &v) = 0;

    //! Set an input variable as InStream. Data are not duplicated. The stream must stay opened.
    virtual void setInStream(UInt32 attr, const InStream &v) = 0;

    //! Set an input variable as Bool.
    virtual void setBool(UInt32 attr, Bool v) = 0;

    //! Set an input variable as Int32.
    virtual void setInt32(UInt32 attr, Int32 v) = 0;

    //! Set an input variable as UInt32.
    virtual void setUInt32(UInt32 attr, UInt32 v) = 0;

    //! Set an input variable as Int64.
    virtual void setInt64(UInt32 attr, Int64 v) = 0;

    //! Set an input variable as UInt64.
    virtual void setUInt64(UInt32 attr, UInt64 v) = 0;

    //! Set an input variable as Float.
    virtual void setFloat(UInt32 attr, Float v) = 0;

    //! Set an input variable as Double.
    virtual void setDouble(UInt32 attr, Double v) = 0;

    //! Set an input variable as CString.
    virtual void setCString(UInt32 attr, const CString &v) = 0;

    //! Set an input variable as Date.
    virtual void setDate(UInt32 attr, const Date &date) = 0;

    //! Set an input variable as Timestamp.
    virtual void setTimestamp(UInt32 attr, const DateTime &date) = 0;

    //! Get an output attribute id by its name.
    virtual UInt32 getOutAttr(const CString &name) = 0;

    //! Get an result variable by its name.
    virtual const DbVariable& getOut(const CString &name) const = 0;

    //! Get an result variable by its index.
    virtual const DbVariable& getOut(UInt32 attr) const = 0;

    //! Execute the query for a SELECT.
	virtual void execute() = 0;

    //! Execute the query for an UPDATE, INSERT, or DELETE.
    virtual void update() = 0;

    //! Get the number of affected or result rows after an execute or update.
    virtual UInt32 getNumRows() = 0;

    //! Get the result variable (ie for an auto increment).
    virtual UInt64 getGeneratedKey() const = 0;

    /**
     * @brief fetch Fetch the results.
     * Can be called in a while for each entry of the result.
     * @return True until there is results row
     */
    virtual Bool fetch() = 0;

    //! Get the row position when fetching.
    virtual UInt32 tellRow() = 0;

    //! Set the row position when fetching (seek 0 for reset).
    virtual void seekRow(UInt32 row) = 0;

    //! Unbind the current attributes.
	virtual void unbind() = 0;
};

} // namespace o3d

#endif // _O3D_DATABASE_H
