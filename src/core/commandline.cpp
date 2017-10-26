/**
 * @file commandline.cpp
 * @brief Implementation of CommandLine.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-09-28
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/commandline.h"

#include "o3d/core/architecture.h"
#include "o3d/core/debug.h"
#include "o3d/core/application.h"

#include <algorithm>

using namespace o3d;

// Default constructor
CommandLine::CommandLine(Int32 argc, Char **argv) :
	m_reqArgsNum(0),
	m_optArgsNum(0)
{
	if ((argc == -1) && argv && argv[0])
	{
		m_fullCmdLine.fromUtf8(argv[0]);

		if (m_fullCmdLine.length())
		{
			m_fullCmdLine.trimRight(' ');
			m_fullCmdLine += ' ';

			// remove the process name from the command line
			if (m_fullCmdLine.sub(Application::getAppName(), 0) == 0)
			{
				m_fullCmdLine.remove(0, Application::getAppName().length());
				m_fullCmdLine.trimLeft(' ', True);
			}
		}
	}
	else if (argc > 1)
	{
		String arg;

		// don't take the process name
		for (Int32 i = 1; i < argc; ++i)
		{
			arg.fromUtf8(argv[i]);
			arg.trimRight(' ');

			m_fullCmdLine += arg;
			m_fullCmdLine.concat(" ");
		}
	}

	prepareData();
}

// Default constructor
CommandLine::CommandLine(const String &commandLine) :
	m_reqArgsNum(0),
	m_optArgsNum(0)
{
	m_fullCmdLine = commandLine;
	m_fullCmdLine.trimRight(' ');
	m_fullCmdLine.concat(" ");

	// remove the process name from the command line
	if (m_fullCmdLine.sub(Application::getAppName(), 0) == 0)
	{
		m_fullCmdLine.remove(0, Application::getAppName().length());
		m_fullCmdLine.trimLeft(' ', True);
	}
	else if (m_fullCmdLine.sub(String("\"") + Application::getAppName() + "\"", 0) == 0)
	{
		m_fullCmdLine.remove(0, Application::getAppName().length() + 2);
		m_fullCmdLine.trimLeft(' ', True);
	}

	prepareData();
}

void CommandLine::prepareData()
{
	if (m_fullCmdLine.length())
	{
		WChar currChar = 0;
		WChar prevChar = 0;

		Int32 startPos = 0;
		Int32 oldStartPos = -1;
		Int32 pos = 0;

		Bool inString = False;
		String tmp;

		while ((currChar = m_fullCmdLine[pos]) != 0)
		{
            if ((currChar == '\"') && (prevChar != '\\'))
			{
				// enter or leave a string area
				inString = !inString;

				if (inString && (prevChar != '='))
				{
					startPos = pos;
				}
				else if (!inString)
				{
					// one more arg
					m_argsList.push_back(m_fullCmdLine.sub(startPos,pos+1));
					oldStartPos = startPos;
				}
			}

			if (!inString && (currChar == ' ') && (prevChar != ' ') && (startPos != oldStartPos))
			{
				// one more arg
				m_argsList.push_back(m_fullCmdLine.sub(startPos,pos));
				oldStartPos = startPos;
			}

			if (!inString && (currChar == ' '))
				startPos = pos+1;

			prevChar = currChar;
			++pos;
		}
	}
}

// Destructor
CommandLine::~CommandLine()
{
}

// Registers a required command line argument
void CommandLine::registerArgument(const String &name)
{
	m_reqArg = Option(O_UNKNOWN,0,name);
	++m_reqArgsNum;
}

// Registers an optional command line argument
void CommandLine::registerOptionalArgument(const String &name)
{
	m_optArg = Option(O_UNKNOWN,0,name);
    ++m_optArgsNum;
}

// Add search for a switch with a short name (ie. -v).
void CommandLine::addSwitch(Char shortName, const String &longName)
{
	m_options.push_back(Option(O_SWITCH,shortName,longName));
}

// Add search for a switch with a long name (ie. --verbose).
void CommandLine::addSwitch(const String &longName)
{
	m_options.push_back(Option(O_SWITCH,0,longName));
}

// Add search for a single occurrence option.
void CommandLine::addOption(Char shortName, const String &longName)
{
	m_options.push_back(Option(O_ARG1,shortName,longName));
}

// Add search for a single occurrence option.
void CommandLine::addOption(const String &longName)
{
	m_options.push_back(Option(O_ARG1,0,longName));
}

// Add search for a single occurrence optional option.
void CommandLine::addOptionalOption(
	Char shortName,
	const String &longName,
	const String &defaultValue)
{
	m_options.push_back(Option(O_OPT,shortName,longName,True,defaultValue));
}

// Add search for a single occurrence optional option.
void CommandLine::addOptionalOption(const String &longName, const String &defaultValue)
{
	m_options.push_back(Option(O_OPT,0,longName,True,defaultValue));
}

// Add search for a argument that can be repeated many times into the command line.
void CommandLine::addRepeatableOption(Char shortName, const String &longName)
{
	m_options.push_back(Option(O_REPEAT,shortName,longName));
}

// Add search for a argument that can be repeated many times into the command line.
void CommandLine::addRepeatableOption(const String &longName)
{
	m_options.push_back(Option(O_REPEAT,0,longName));
}

// Add search for an option contain a variable number of value, given its long name.
void CommandLine::addVarLenOption(const String &longName)
{
	m_options.push_back(Option(O_VARLEN,0,longName));
}

// Parse the command line.
Bool CommandLine::parse()
{
	std::stack<String> stack;
    for (std::vector<String>::reverse_iterator rit = m_argsList.rbegin(); rit != m_argsList.rend(); ++rit)
	{
		stack.push(*rit);
	}

	enum State
	{
		StartState,
		ExpectingState,
		OptionalState
	} state = StartState;

	enum TokenType
	{
		LongOpt,
		ShortOpt,
		Arg,
		End
	} t, currType = End;

	Int32 currArg = 0;
	IT_Opt currOpt = m_options.end();

	Bool extraLoop = True;
	while (!stack.empty() || extraLoop)
	{
		String a;
		String origA;

		if (!stack.empty())
		{
			a = stack.top();
			stack.pop();

			++currArg;
			origA = a;

			if (a.startsWith("--"))
			{
				// long name option
				a.set(a,2);
				if (a.isEmpty())
				{
					O3D_ERROR(E_InvalidParameter("'--' feature not supported, yet"));
					return False;
				}
				t = LongOpt;

				Int32 equal = a.find('=');
				if (equal != -1)
				{
					stack.push(a.sub(equal+1));
					--currArg;
					a.truncate(equal);
				}
				else if (a.length() == 1)
				{
					t = Arg;
				}
			}
			else if (a[0] == '-')
			{
				// short option
				t = ShortOpt;
				// followed by an argument ? push it for later processing.
				if (a.length() > 2)
				{
					stack.push(a.sub(2));
					--currArg;
				}
				a = a[1];
			}
			else
			{
				t = Arg;
			}
		}
		else // if (!stack.empty())
		{
			// faked closing argument
			t = End;
		}

		// remove '"' at left and right if arg
		if (t == Arg)
		{
            if (a.trimLeft('\"'))
			{
                a.trimRight('\"');

                // replace '\"' by '"'
                a.replace("\\\"","\"");
			}
		}

		// look up among known list of options
		Bool ending = False;
		IT_Opt opt = m_options.end();

		if (t != End)
		{
			IT_Opt oit = m_options.begin();
			for (; oit != m_options.end(); ++oit)
			{
				if (((t == LongOpt) && (oit->m_longName == a)) ||
					((t == ShortOpt) && (oit->m_shortName == a[0])))
				{
					opt = oit;
					break;
				}
			}

			if ((t == LongOpt) && (oit->m_type == O_UNKNOWN))
			{
				if ((currOpt != m_options.end()) && (currOpt->m_type != O_VARLEN))
				{
					O3D_WARNING(String("Unknown option --") << a);
					return False;
				}
				else
				{
				  // VarLength options support arguments starting with '-'
				  t = Arg;
				}
			}
			else if ((t == ShortOpt) && (oit->m_type == O_UNKNOWN))
			{
				if ((currOpt != m_options.end()) && (currOpt->m_type != O_VARLEN))
				{
					O3D_WARNING(String("Unknown option -") << a[0]);
					return False;
				}
				else
				{
					// VarLength options support arguments starting with '-'
					t = Arg;
				}
			}
			else
			{
				ending = True;
			}
		}

		// interpret result
		switch (state)
		{
			case StartState:
				if (opt != m_options.end())
				{
					if (opt->m_type == O_SWITCH)
					{
						opt->m_switch = True;
					}
					else if ((opt->m_type == O_ARG1) || (opt->m_type == O_REPEAT))
					{
						state = ExpectingState;
						currOpt = opt;
						currType = t;
					}
					else if ((opt->m_type == O_OPT) || (opt->m_type == O_VARLEN))
					{
						state = OptionalState;
						currOpt = opt;
						currType = t;
					}
				}
				else if (ending && !stack.empty())
				{
					// we're done
				}
				else if ((opt == m_options.end()) && (t == Arg))
				{
					if (m_reqArgsNum > 0)
					{
						if (m_reqArg.m_values.size() == 0)
						{
                            m_foundLongName.push_back(m_reqArg.m_longName);
							m_reqArg.m_values.push_back(a);
						}
						else
						{
							O3D_WARNING("Too many arguments");
							return false;
						}
					}
					else if (m_optArgsNum > 0)
					{
						if (m_optArg.m_values.size() == 0)
						{
                            m_foundLongName.push_back(m_reqArg.m_longName);
							m_optArg.m_values.push_back(a);
						}
						else
						{
							O3D_WARNING("Too many arguments");
							return false;
						}
					 }
				}
				else if (opt != m_options.end())
				{
					O3D_ERROR(E_InvalidOperation(String("Unhandled StartState case ") << (Int32)opt->m_type));
				}
				break;

			case ExpectingState:
				if (t == Arg)
				{
					if (currOpt->m_type == O_ARG1)
					{
						if (currOpt != m_options.end())
                        {
							currOpt->m_values.push_back(a);
                            m_foundLongName.push_back(currOpt->m_longName);
                            m_foundShortName.append(currOpt->m_shortName);
                        }

						state = StartState;
					}
					else if (currOpt->m_type == O_REPEAT)
					{
						if (currOpt != m_options.end())
                        {
							currOpt->m_values.push_back(a);
                            m_foundLongName.push_back(currOpt->m_longName);
                            m_foundShortName.append(currOpt->m_shortName);
                        }

						state = StartState;
					}
					else
					{
						O3D_ERROR(E_InvalidOperation("Undefined ExpectingState"));
					}
				}
				else
				{
					if (currOpt != m_options.end())
					{
						String n = currType == LongOpt ? currOpt->m_longName : String() << currOpt->m_shortName;
						O3D_WARNING(String("Expected an argument after the option ") + n);
					}
					else
					{
						O3D_WARNING(String("Expected an argument after an unknown option"));
					}
					return False;
				}
				break;

			case OptionalState:
				if (t == Arg)
				{
					if ((currOpt != m_options.end()) && (currOpt->m_type == O_OPT))
					{
						currOpt->m_values.push_back(a);
						state = StartState;
					}
					else if ((currOpt != m_options.end()) && (currOpt->m_type == O_VARLEN))
					{
						currOpt->m_values.push_back(origA);
						// remain in this state
					}
					else
					{
						O3D_ERROR(E_InvalidOperation("Undefined OptionalState"));
					}
				}
				else
				{
					// optional argument not specified
					if ((currOpt != m_options.end()) && (currOpt->m_type == O_OPT))
						currOpt->m_values.push_back(currOpt->m_defaultValue);

					if (t != End)
					{
						// re-evaluate current argument
						stack.push(origA);
						--currArg;
					}
					state = StartState;
				}
				break;
		}

		// are we in the extra loop ? if so, flag the final end
		if (t == End)
			extraLoop = False;
	}

	if ((m_reqArgsNum > 0) && (m_reqArg.m_values.size() == 0))
	{
		O3D_WARNING("Missing required argument");
		return False;
	}

	return True;
}

// Return true if an option or a switch has been found.
Bool CommandLine::isSet(Char shortName, const String &longName) const
{
	if ((shortName != 0) && (m_foundShortName.find(shortName) != -1))
		return True;

	if (longName.length())
	{
		std::list<String>::const_iterator cit = std::find(
			m_foundLongName.begin(),
			m_foundLongName.end(),
			longName);

		if (cit != m_foundLongName.end())
			return True;
	}

	return False;
}

// Return true if an option or a switch long name has been found.
Bool CommandLine::isSet(const String &longName) const
{
	if (longName.length())
	{
		std::list<String>::const_iterator cit = std::find(
			m_foundLongName.begin(),
			m_foundLongName.end(),
			longName);

		if (cit != m_foundLongName.end())
			return True;
	}

	return False;
}

// Get the argument value.
String CommandLine::getArgumentValue(const String &name) const
{
	if ((m_reqArg.m_longName == name) && m_reqArg.m_values.size())
		return m_reqArg.m_values.front();

	O3D_ERROR(E_InvalidParameter("Unregistered argument"));
	return String();
}

// Get the value of the optional argument value.
String CommandLine::getOptionalArgumentValue(const String &name) const
{
	if ((m_optArg.m_longName == name) && m_optArg.m_values.size())
		return m_optArg.m_values.front();

	O3D_ERROR(E_InvalidParameter("Unregistered optional argument"));
	return String();
}

// Get the value of a switch.
Bool CommandLine::getSwitch(Char shortName, const String &longName) const
{
	for (CIT_Opt cit = m_options.begin(); cit != m_options.end(); ++cit)
	{
		if ((longName.length() && (cit->m_longName == longName)) ||
			((shortName != 0) && (cit->m_shortName == shortName)))
		{
			return cit->m_switch;
		}
	}

	O3D_ERROR(E_InvalidParameter("Unregistered switch"));
	return False;
}

// Get the value of a switch.
Bool CommandLine::getSwitch(const String &longName) const
{
	for (CIT_Opt cit = m_options.begin(); cit != m_options.end(); ++cit)
	{
		if (longName.length() && (cit->m_longName == longName))
		{
			return cit->m_switch;
		}
	}

	O3D_ERROR(E_InvalidParameter("Unregistered switch"));
	return False;
}

// Get the value of a single option.
String CommandLine::getOptionValue(
	Char shortName,
	const String &longName) const
{
	for (CIT_Opt cit = m_options.begin(); cit != m_options.end(); ++cit)
	{
		if ((longName.length() && (cit->m_longName == longName)) ||
			((shortName != 0) && (cit->m_shortName == shortName)))
		{
			if (cit->m_values.size())
				return cit->m_values.front();
			else
                return cit->m_defaultValue;
		}
	}

	O3D_ERROR(E_InvalidParameter("Unregistered option"));
	return String();
}

// Get the value of a single option with a short name
String CommandLine::getOptionValue(const String &longName) const
{
	for (CIT_Opt cit = m_options.begin(); cit != m_options.end(); ++cit)
	{
		if (longName.length() && (cit->m_longName == longName))
		{
			if (cit->m_values.size())
				return cit->m_values.front();
			else
                return cit->m_defaultValue;
		}
	}

	O3D_ERROR(E_InvalidParameter("Unregistered option"));
	return String();
}

// Get the values of a multiple occurrence option.
T_StringList CommandLine::getOptionValues(
	Char shortName,
	const String &longName) const
{
	for (CIT_Opt cit = m_options.begin(); cit != m_options.end(); ++cit)
	{
		if ((longName.length() && (cit->m_longName == longName)) ||
			((shortName != 0) && (cit->m_shortName == shortName)))
		{
			return cit->m_values;
		}
	}

	O3D_ERROR(E_InvalidParameter("Unregistered repeatable option"));
	return T_StringList();
}

// Get the values of a multiple occurrence option.
T_StringList CommandLine::getOptionValues(const String &longName) const
{
	for (CIT_Opt cit = m_options.begin(); cit != m_options.end(); ++cit)
	{
		if (longName.length() && (cit->m_longName == longName))
		{
			return cit->m_values;
		}
	}

	O3D_ERROR(E_InvalidParameter("Unregistered repeatable option"));
	return T_StringList();
}

// Get the values of an option that contain a variable number of values.
T_StringList CommandLine::getVarLenOptionValues(const String &longName) const
{
	for (CIT_Opt cit = m_options.begin(); cit != m_options.end(); ++cit)
	{
		if (longName.length() && (cit->m_longName == longName))
		{
			return cit->m_values;
		}
	}

	O3D_ERROR(E_InvalidParameter("Unregistered variable length option"));
	return T_StringList();
}

