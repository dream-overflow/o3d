/**
 * @file commandline.h
 * @brief Command line parser and helper.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-09-28
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_COMMANDLINE_H
#define _O3D_COMMANDLINE_H

#include "stringlist.h"
#include <vector>

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class CommandLine
//-------------------------------------------------------------------------------------
//! Command line parser and helper. Short name use '-' prefix and long name '--' prefix.
//! For example, --verbose or -V. Use of " " for value containing space and \" for add
//! a ". In example, --text="This \"is\" my text".
//!
//! Switch:
//! A switch is an optional parameter (like --verbose) and is the command line parser
//! found it so it will be set to true, otherwise to false.
//!
//! Argument:
//! An argument is a plain text token like a file name. Argument
//!
//! Option:
//! Option that can be found 0 or 1 time, but not more. For example --mode=debug or
//! -Wall.
//!
//! Optional option:
//! Option that can be found 0 or 1 time, but not more and that have a default value if
//! it is not specified by the user.
//!
//! Repeatable option:
//! Option that can be repeated many times like -D/usr/include -D/usr/local/include.
//! In this case it returns a list of strings.
//!
//! Var length option:
//! Option that can support many value. For example -O value1 value2 value3. In this
//! case it returns a list of strings. This option cannot be defined in last position if
//! a required argument is registered. Because the var length will take the argument as
//! an additional value.
//---------------------------------------------------------------------------------------
class O3D_API CommandLine
{
public:

	//! Default constructor.
	//! @param argc Number of arguments in argv (-1 if unknown).
	//! @param argv Array of arguments in char* utf8 or ASCII.
	//! @note If argc is set to -1 so argv should contain one single argument (argv[0] != null).
	CommandLine(Int32 argc, Char **argv);

	//! Default constructor using a single string as command line.
	//! If the first argument is the process name then it is automatically removed..
	CommandLine(const String &commandLine);

	//! Destructor
	~CommandLine();

	//! Return the full command line
	inline const String& getFullLine() const { return m_fullCmdLine; }

    //! Get the argument list of the command line (to use directly without parsing)
    const std::vector<String>& getArgs() const { return m_argsList; }


	//-----------------------------------------------------------------------------------
	// Pre-parsing methods
	//-----------------------------------------------------------------------------------

	//! Registers a required command line argument @a name.
	//! @note If no argument is found the parse will return an error. The name is used
	//! as reference with IsSet() or GetArgumentValue(...).
	//! @see CommandLine::GetArgumentValue
	void registerArgument(const String &name);

	//! Registers an optional command line argument @a name.
	//! @note If the argument is found more than one time, then Parse() will return False.
	//! @see O3DCommandLine::GetArgumentValue
	void registerOptionalArgument(const String &name);


	//! Add search for a switch with a short name (ie. -v). If the switch is found a
	//! further call to IsSwitch(...) will return true, else false
	//! @param shortName The short name of the switch
	//! @param longName The long name of the switch or empty string if none
	void addSwitch(Char shortName, const String &longName = String());

	//! Add search for a switch with a long name (ie. --verbose). If the switch is found
	//! a further call to IsSwitch(...) will return true, else false.
	//! @param longName The long name of the switch
	void addSwitch(const String &longName);


	//! Add search for a single occurrence option.
	//! @param shortName The short name of the option
	//! @param longName The long name of the argument or empty string if none
	//! @note If the option is found more than one time, then Parse() will return False.
	//! @see CommandLine::GetOptionValue
	void addOption(Char shortName, const String &longName = String());

	//! Add search for a single occurrence option.
	//! @note If the argument is found more than one time, then Parse() will return False.
	//! @see CommandLine::GetOptionValue
	void addOption(const String &longName);


	//! Add search for a single occurrence option that has an optional value.
	//! @param shortName The short name of the option
	//! @param longName The long name of the argument or empty string if none
	//! @param defaultValue The default value if the option is not found
	//! @note If the argument is found more than one time, then Parse() will return False.
	//! @see CommandLine::GetOptionValue
	void addOptionalOption(
		Char shortName,
		const String &longName,
		const String &defaultValue);

	//! Add search for a single occurrence option that has an optional value.
	//! @param defaultValue The default value if the option is not found
	//! @note If the argument is found more than one time, then Parse() will return False.
	//! @see CommandLine::GetOptionValue
	void addOptionalOption(const String &longName, const String &defaultValue);


	//! Add search for a argument that can be repeated many times into the command line.
	//! @param shortName The short name of the option
	//! @param longName The long name of the argument or empty string if none
	//! @see O3DCommandLine::GetOptionValues
	void addRepeatableOption(Char shortName, const String &longName = String());

	//! Add search for a argument that can be repeated many times into the command line.
	//! @param longName The long name of the option
	//! @note If the argument is found more than one time, then Parse() will return False.
	//! @see CommandLine::GetOptionValues
	void addRepeatableOption(const String &longName);


	//! Add search for an option that contain a variable number of values.
	//! @param longName The long name of the option
	//! @see CommandLine::GetVarLenOptionValues
	//! @warning Never use it in last option if there is a registered argument.
	void addVarLenOption(const String &longName);


	//! Parse the command line.
	//! @return False if the parsing doesn't found any registered arguments, or found many
	//! time a single/varlen argument. Otherwise it returns true.
	Bool parse();


	//-----------------------------------------------------------------------------------
	// Post-parsing methods
	//-----------------------------------------------------------------------------------

	//! Return true if an option or a switch has been found.
	Bool isSet(Char shortName, const String &longName) const;

	//! Return true if an option or a switch long name has been found.
	Bool isSet(const String &longName) const;


	//! Get the value of the required argument.
	//! @param name Name of the registered argument
	String getArgumentValue(const String &name) const;

	//! Get the value of the optional argument.
	//! @param name Name of the registered optional argument
	String getOptionalArgumentValue(const String &name) const;


	//! Get the value of a switch.
	//! @param shortName The short name of the switch
	//! @param longName The long name of the switch or empty string if none
	Bool getSwitch(Char shortName, const String &longName = String()) const;

	//! Get the value of a switch.
	//! @param longName The long name of the switch
	Bool getSwitch(const String &longName) const;


	//! Get the value of a single option.
	//! @param shortName The short name of the option
	//! @param longName The long name of the option or empty string if none
	String getOptionValue(
		Char shortName,
		const String &longName = String()) const;

	//! Get the value of a single option with a short name
	String getOptionValue(const String &longName) const;


	//! Get the values of a multiple occurrence option.
	//! @param shortName The short name of the option
	//! @param longName The long name of the option or empty string if none
	T_StringList getOptionValues(
		Char shortName,
		const String &longName = String()) const;

	//! Get the values of a multiple occurrence option.
	//! @param longName The long name of the option
	T_StringList getOptionValues(const String &longName) const;


	//! Get the values of an option that contain a variable number of values.
	//! @param longName The long name of the option
	T_StringList getVarLenOptionValues(const String &longName) const;

private:

	enum OptionType
	{
		O_UNKNOWN,
		O_ARG1,
		O_OPT,
		O_SWITCH,
		O_REPEAT,
		O_VARLEN
	};

	//! Internal structure of an argument
	struct Option
	{
		Option() :
			m_type(O_UNKNOWN),
			m_shortName(0),
			m_longName(),
			m_defaultValue(),
			m_optional(False),
			m_switch(False)
		{}

		Option(
			OptionType type,
			Char shortName,
			const String &longName,
			Bool optional = True,
			const String &defaultValue = String()) :
				m_type(type),
				m_shortName(shortName),
				m_longName(longName),
				m_defaultValue(defaultValue),
				m_optional(optional),
				m_switch(False)
		{}

		OptionType m_type;

		Char m_shortName;
		String m_longName;

		String m_defaultValue;
		Bool m_optional;

		Bool m_switch;
		T_StringList m_values;
	};

    std::vector<String> m_argsList;

	String m_fullCmdLine;

	Int32 m_reqArgsNum;
	Int32 m_optArgsNum;

	T_StringList m_foundLongName;
	String m_foundShortName;

	Option m_reqArg;                 //!< Required argument
	Option m_optArg;                 //!< Optional argument

	std::list<Option> m_options;     //!< List of options

	typedef std::list<Option>::iterator IT_Opt;
	typedef std::list<Option>::const_iterator CIT_Opt;

	//! Prepare the data before parsing.
	void prepareData();
};

} // namespace o3d

#endif // _O3D_COMMANDLINE_H

