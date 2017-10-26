/**
 * @file database.cpp
 * @brief Implementation of Database.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2007-11-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/database.h"
#include "o3d/core/objects.h"
#include "o3d/core/smartarray.h"

using namespace o3d;

// Virtual destructor
Database::~Database()
{
	unregisterAll();
}

// Register a new ODbQuery
DbQuery* Database::registerQuery(const String &name, const CString &query)
{
	// Existing query name
	IT_DbQueryMap it = m_RegistredQuery.find(name);
	if (it != m_RegistredQuery.end())
	{
		O3D_ERROR(E_ValueRedefinition(name));
        return nullptr;
	}

	if (!m_IsConnected)
	{
		O3D_ERROR(E_InvalidPrecondition("Connection asked"));
        return nullptr;
	}

    DbQuery *newQuery = newDbQuery(name, query);
	if (!newQuery)
	{
		O3D_ERROR(E_InvalidAllocation("newQuery"));
        return nullptr;
	}

	// Register the query
	m_RegistredQuery[name] = newQuery;
	return newQuery;
}

// Unregister an existing O3DDbQuery given its pointer
Bool Database::unregisterQuery(DbQuery *query)
{
    O3D_ASSERT(query != nullptr);

	for (IT_DbQueryMap it = m_RegistredQuery.begin(); it != m_RegistredQuery.end(); ++it)
	{
		if (it->second && (it->second == query))
		{
			deletePtr(it->second);
			m_RegistredQuery.erase(it);

            return True;
		}
	}

	O3D_WARNING("Query not found");
    return False;
}

// Unregister an existing DbQuery given its name
Bool Database::unregisterQuery(const String &name)
{
	// Existing query name
	IT_DbQueryMap it = m_RegistredQuery.find(name);
	if (it != m_RegistredQuery.end())
	{
		deletePtr(it->second);
		m_RegistredQuery.erase(it);

        return True;
	}
	else
	{
		O3D_WARNING(String("Query \'") << name << "\' not found");
        return False;
	}
}

// Unregister all existing O3DDbQuery
void Database::unregisterAll()
{
	for (IT_DbQueryMap it = m_RegistredQuery.begin(); it != m_RegistredQuery.end(); ++it)
	{
		deletePtr(it->second);
	}

	m_RegistredQuery.clear();
}

// Find a query by its name
DbQuery* Database::findQuery(const String &name)
{
	// Existing query name
	IT_DbQueryMap it = m_RegistredQuery.find(name);
	if (it != m_RegistredQuery.end())
	{
		return (it->second);
	}
	else
	{
        return nullptr;
	}
}

DbQuery::~DbQuery()
{

}

