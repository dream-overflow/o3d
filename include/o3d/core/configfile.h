/**
 * @file configfile.h
 * @brief Simple config file parser.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2007-11-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CONFIGFILE_H
#define _O3D_CONFIGFILE_H

#include "instream.h"
#include "outstream.h"
#include "stringlist.h"

#include <map>
#include <vector>

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class ConfigFile
//---------------------------------------------------------------------------------------
//! @brief This class implements a simple parser used to read or write config files.
//! This class is used to manage config files. A config file is structured in blocks
//! containing keys. You can assign a value to each keys. Supported type are:
//! bool, int, double, string
//---------------------------------------------------------------------------------------
class O3D_API ConfigFile
{
private:

	// Predefinitions
	class ConfigFileGroup;
	class ConfigFileValue;

	// Internal types
	typedef std::vector<ConfigFileGroup*>			T_GroupArray;
	typedef T_GroupArray::iterator					IT_GroupArray;
	typedef T_GroupArray::const_iterator			CIT_GroupArray;

	typedef std::map<String, ConfigFileValue>		T_KeyMap;
	typedef T_KeyMap::iterator						IT_KeyMap;
	typedef T_KeyMap::const_iterator				CIT_KeyMap;

	// Internal classes
	class ConfigFileValue
	{
	public:

		enum KeyType {
			CONFIG_FILE_KEY_UNDEFINED = 0,
			CONFIG_FILE_KEY_BOOL,
			CONFIG_FILE_KEY_INT,
			CONFIG_FILE_KEY_DOUBLE,
			CONFIG_FILE_KEY_STRING };

        static const Char * const m_enumString[];

	private:

		void * m_pData;

		KeyType m_type;

	public:

		// Constructors
		ConfigFileValue();
        ConfigFileValue(Bool);
        ConfigFileValue(Int32);
        ConfigFileValue(Double);
		ConfigFileValue(const String &);
		ConfigFileValue(const ConfigFileValue &);
		~ConfigFileValue();

		ConfigFileValue & operator = (const ConfigFileValue &);

		void destroy();

		// Accessors
        Bool isType(KeyType _type) const { return (m_type == _type); }
		KeyType getType() const { return m_type; }

        Bool toBool(Bool, Bool * = NULL) const;
        Int32 toInt(Int32, Bool * = NULL) const;
        Double toDouble(Double, Bool * = NULL) const;
        const String toString(const String &, Bool * = NULL) const;

        void setBool(Bool);
        void setInt(Int32);
        void setDouble(Double);
		void setString(const String &);

		// Serialization
        Bool writeToString(String &);
        Bool readFromString(const String &);
	};

	class ConfigFileGroup
	{


	private:

		enum ParserLineType {
			PARSER_LINE_UNDEFINED = 0,
			PARSER_LINE_TEXT,
			PARSER_LINE_KEY,
			PARSER_LINE_GROUP_BEGIN,
			PARSER_LINE_GROUP_END };

		struct ParserLine
		{
			ParserLineType lineType;	//! Type of the line TEXT or KEY

			String linePreData;		//! String before the name of the group/key
			String lineData;			//! Whole line
			String linePostData;		//! String after the name of the group/value
		};

		typedef std::vector<ParserLine>			T_FileStructure;
		typedef T_FileStructure::iterator		IT_FileStructure;
		typedef T_FileStructure::const_iterator	CIT_FileStructure;

		// Members
		String m_name;		//! name of this group (Not the path)

		T_KeyMap m_keys;		//! All keys declared in this block

		T_GroupArray m_groups;	//! All sub groups

		T_FileStructure m_structure;	//! Structure of the file

		// Restricted functions
		ConfigFileGroup * findLocalGroup(const String &);
		const ConfigFileGroup * findLocalGroup(const String &) const;

		ConfigFileValue * findLocalKey(const String &);
		const ConfigFileValue * findLocalKey(const String &) const;

		void recFindKey(const String &, const String &, std::vector<String> &) const;

        Bool parseFile(InStream &, ParserLine &);

	public:

		// Constructors
		explicit ConfigFileGroup(const String & = String());
		ConfigFileGroup(const ConfigFileGroup &);
		~ConfigFileGroup();

		ConfigFileGroup & operator = (const ConfigFileGroup &);

		void destroy();

		/* Accessors */
		void setName(const String &);
		const String & getName() const { return m_name; }

        Bool removeKey(const String &);

        Bool setKey(const String &, const ConfigFileValue &);
        Bool getKey(const String &, ConfigFileValue &) const;

        Bool findKey(const String &) const;
        Bool findKey(const String &, std::vector<String> &) const;

		const T_GroupArray & getGroups() const { return m_groups; }

        Bool isEmpty() const { return (m_groups.empty() && m_keys.empty()); }
        Bool isNull() const { return (isEmpty() && m_name.isEmpty()); }

		ConfigFileGroup * createGroup(const String &);
		ConfigFileGroup * getGroup(const String &);
        Bool removeGroup(const String &);

		// Serialisation
        Bool writeToFile(OutStream &_os);
        Bool readFromFile(InStream &_is);
	};

	// Members
	String m_fileName;				//!< Filename

	ConfigFileGroup m_root;			//!< Base group

	T_GroupArray m_currentGroup;	//!< Current group

public:

	//! @brief Default constructor
	ConfigFile();

	//! @brief A constructor
	//! @param _filename the absolute path of the config file or an empty
	//!        string to create an empty object. If the file does not exist, nothing
	//!        will be loaded, but this file will be used when you call Save.
	//! @return a constructor returns no value, but you can check whether or
	//!         not your file was successfully loaded with the function
	//!         isNull()
	explicit ConfigFile(const String & _filename);
	//! @brief Copy constructor
	ConfigFile(const ConfigFile &);
	//! @brief A destructor
	virtual ~ConfigFile();

	//! Assignment operator
	ConfigFile & operator = (const ConfigFile &);

	//! @brief Releases all data stored in the object
	//! This function releases all the memory used by this object.
	//! After a call to this function, the function IsNull() returns TRUE.
	//!
	void destroy();

	//! @brief Define the file used to save the data
	//! Set the filepath which will be used by the function
    //! Bool save(const O3DString & _filePath = O3DString())
	//! @param _file the file name
	void setFileName(const String & _file) { m_fileName = _file; }

	//! @brief Return the current filename used to save the data
	//! @return the filename
	const String & getFileName() const { return m_fileName; }

	//! @brief Check the filename existence
	//! @return TRUE if the fileName exists but doesn't check if it's valid
    Bool hasFileName() { return (m_fileName.length() > 0); }

	//! @brief Return whether or not the object is empty
	//! This function return the state of the root group.
	//! @return TRUE if no key/group is defined in the object. But an empty
	//!         object can be valid. "Empty" doesn't mean "Null".
	//! @see O3DConfigFile::isNull()
    Bool isEmpty() const { return (m_root.isEmpty()); }

	//! @brief Return whether or not the object is valid
	//! This function return the state of the object. An object will be null
	//! if it was created with the default constructor or loaded from a invalid
	//! file. An empty object is not necessarily null but an null object is always
	//! null.
	//! @return TRUE if the object is not valid.
    Bool isNull() const { return ((m_root.isNull()) && (m_fileName.isEmpty())); }

	//! @brief Return the name of the current group
	//! @return the name of the current group or an empty string if the current
	//! group is the main one or if this object is empty.
	const String getGroup() const;

	//! @brief Return the name of the current group
	//! @return the name of the current group or an empty string if the current
	//! group is the main one or if this object is empty.
	//! Examples: "/" , "/network/"
	const String getFullPath() const;

	//! @brief Get a list of the direct children groups name.
	T_StringList getGroups() const;

	//! @brief define the value (boolean) of a key
	//! This function allow you to change the type or the value of a key.
	//! setBool takes a key name or a path+key name, this mean relative path
	//! is supported.
	//! @param _keyName the name of the key you want to change
	//! @param _value the value you want to give to the key
	//! @return TRUE if the key was successfully changed
    Bool setBool(const String & _keyName, Bool _value);

	//! @brief define the value (integer) of a key
    //! @see ConfigFile::setValue(const O3DString & _keyName, Bool _value)
    Bool setInt(const String & _keyName, Int32 _value);

	//! @brief define the value (double) of a key
    //! @see ConfigFile::setValue(const O3DString & _keyName, Bool _value)
    Bool setDouble(const String & _keyName, Double _value);

	//! @brief define the value (string) of a key
    //! @see ConfigFile::setValue(const O3DString & _keyName, Bool _value)
    Bool setString(const String & _keyName, const String & _value);

	//! @brief return the value of a key (boolean)
	//! This function takes a key name or a path+key name, this mean relative path
	//! is supported. When using this function, you must provide a default value
	//! which will be returned if the key is not found or has an incorrect type.
	//! @param _keyName the name of the key
	//! @param _defaultValue the value which will be return if the key is not valid.
	//! @param _ok a pointer to a boolean if you want to handle errors. if _ok is defined
	//!        _ok will be FALSE, if the key is invalid (not found or not correct)
	//! @return the value of the key or the default value
    Bool getBool(
			const String & _keyName,
            Bool _defaultValue,
            Bool * _ok = NULL) const;

	//! @brief return the value of a key (integer)
    //! @see ConfigFile::getInt(const O3DString &_keyName, Bool _defaultValue, Bool * _ok = NULL) const;
    Int32 getInt(const String & _keyName,
            Int32 _defaultValue,
            Bool * _ok = NULL) const;

	//! @brief return the value of a key (double)
    //! @see ConfigFile::getDouble(const O3DString &_keyName, Bool _defaultValue, Bool * _ok = NULL) const;
    Double getDouble(
			const String & _keyName,
            Double _defaultValue,
            Bool * _ok = NULL) const;

	//! @brief return the value of a key (string)
    //! @see ConfigFile::getString(const O3DString &_keyName, Bool _defaultValue, Bool * _ok = NULL) const;
	const String getString(
			const String & _keyName,
			const String & _defaultValue,
            Bool * _ok = NULL) const;

	//! @brief Find a key
	//! This function try to find a key in the current group only. Only a name
	//! is handle by the function (not a path).
	//! @param _keyName the key's name
	//! @return TRUE if the key exists
    Bool findKey(const String & _keyName) const;

	//! @brief Find a key
	//! This function will try to find a key in all sub groups including
	//! the current one. Only a name is handle by the function (not a path).
	//! If the key was found in the current group, you'll find an empty string
	//! in the list of path, not a null one.
	//! Example: "/", "my_group/my_sub_group/"
	//! @param _keyName the key's name
	//! @param _pathArray the array in which will be stored all relative path
	//!                   in which the key was found.
	//! @return TRUE if the returned array is not empty.
    Bool findKey(const String & _keyName, std::vector<String> & _pathArray) const;

	//! @brief Remove a existing key
	//! You can only remove a key in the current group. So you must use enter()
	//! and Leave() to change the current group.
	//! @param _keyName the name of the key to remove
	//! @return TRUE if the key was successfully removed, FALSE if it doesn't exist.
    Bool removeKey(const String & _keyName);

	//! @brief Create a new group
	//! This function creates a new group in the current one. You can provide
	//! a path instead of just a name but it must be a relative path from
	//! the current group. All group but the last one, must exist.
	//! Example:
	//! Let be a file with the group: /group1
	//! O3DConfigFile::insertGroup("/group1/my_group") will succeed
	//! O3DConfigFile::insertGroup("/group2/my_group") will fail
	//! @param _group the name or the path of the group to create
	//! @return TRUE if the group was successful created or if it exists already
    Bool insertGroup(const String & _group);

	//! @brief Remove a group
	//! This function remove a group in the current group. You can provide
	//! a path relative to the current group instead of just a name.
	//! Example:
	//! Let be a file with the group: /group1/my_group
	//! O3DConfigFile::insertGroup("/group1/my_group") will remove the group "my_group"
	//! @param _group the name or the path of the group to create
	//! @return TRUE if the group was successfully removed
    Bool removeGroup(const String & _group);

	//! @brief Find a group
	//! You can provide a relative path to the current group or just the name
	//! of the group you want to check.
	//! example: FindGroup("/group1/subgroup1") will return FALSE if "group1" or
	//!          "subgroup1" doesn't exist.
	//! @return TRUE if the group exist
    Bool findGroup(const String & _group);

	//! @brief Change the current group
	//! This function allow you to enter in a sub group. You can provide just the
	//! name of the group or a relative path.
	//! example: - Enter("my_group"), enter("/my_group/")
	//!          - enter("my_group/my_sub") or enter("/ my_group/ / my_sub") are
	//!            equivalent.
	//! @param _path the relative path or the name of the group
	//! @return TRUE if the current group changed
    Bool enter(const String & _path);

	//! @brief Change the current group
	//! This function just go up in the group hierarchy.
	//! @param _level the number of level you want to go up. If the number exceeds
	//!               the current level, the current group will be the root one.
    void leave(UInt32 _level = 1);

	//! @brief Set the current group as the root one
	void toRoot();

	//! @brief Save the whole data into an ASCII file
	//! @param _filePath the file in which you want to save the object. By default
	//!                  the file used to load this object or the filename set by
	//!                  the function setFileName(const O3DString & _file) will be used
	//!                  instead.
	//! @return TRUE if the object was successfully saved. In this case, the internal
	//!              filename has changed. Future call to const O3DString & getFileName()
	//!              will return the file you gave.
    Bool save(const String & _filePath = String());

    //! Save the whole data into a stream.
	//! @param _file the file in which you want to save the object
	//! @return TRUE if the file was written successfully.
    virtual Bool writeToFile(OutStream & os);

    //! Extract all data from a stream.
	//! @param _file the file you want to load. The previous data stored in this object
	//!              will be released.
	//! @return TRUE if the file was loaded successfully.
    virtual Bool readFromFile(InStream & is);
};

} // namespace o3d

#endif // _O3D_CONFIGFILE_H

