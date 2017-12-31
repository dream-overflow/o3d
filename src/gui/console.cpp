/**
 * @file console.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/console.h"

#include "o3d/engine/renderer.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/widgetmanager.h"

#include "o3d/core/appwindow.h"

using namespace o3d;

//---------------------------------------------------------------------------------------
// Command
//---------------------------------------------------------------------------------------

// constructor
Command::Command(Console *pConsole) :
	m_console(pConsole)
{
	O3D_ASSERT(m_console);
}

// destructor
Command::~Command()
{
	m_command.clear();

	// only if used
	if (m_console)
	{
        if (((Gui*)m_console->getScene()->getGui())->getWidgetManager())
            ((Gui*)m_console->getScene()->getGui())->getWidgetManager()->getConsole().removeCommand(this);
	}
}

// get the first command
const String* Command::findCommand(const String &find)
{
	for (
		m_currentCommand = m_command.begin();
		m_currentCommand != m_command.end();
		++m_currentCommand)
	{
		if ((*m_currentCommand).Command == find)
			break;
	}

	if (m_currentCommand == m_command.end())
        return nullptr;

	m_foundCommand = find;
	return &((*m_currentCommand).Command);
}

// get the first subcommand
const String* Command::findSubCommand(
	const String &command,
	const String &find)
{
	if (m_currentCommand == m_command.end())
        return nullptr;

	if ((*m_currentCommand).Command != command)
	{
		if (!findCommand(command))
            return nullptr;
	}

	for (	m_currentSub = (*m_currentCommand).SubCommand.begin();
		m_currentSub != (*m_currentCommand).SubCommand.end();
		++m_currentSub)
	{
		if ((*m_currentSub) == find)
			break;
	}

	if (m_currentSub == (*m_currentCommand).SubCommand.end())
        return nullptr;

	m_foundSubCommand = find;
	return &(*m_currentSub);
}

// is there one or more subcommand for a given command
Bool Command::isSubCommand(const String &command)
{
	if (m_currentCommand == m_command.end())
		return False;

	if ((*m_currentCommand).Command != command)
	{
		if (!findCommand(command))
			return False;
	}

	return ((*m_currentCommand).SubCommand.size() > 0);
}

// draw a text into the console
void Command::putText(const String &text)
{
	if (m_console)
		m_console->putText(text);
}

// Add to the defined parent console
void Command::addToConsole()
{
	if (m_console)
	{
		m_console->addCommand(this);
	}
}

//---------------------------------------------------------------------------------------
// DefaultCommand
//---------------------------------------------------------------------------------------

class DefaultCommand : public Command
{
public:

	DefaultCommand(Console *console) :
		Command(console)
	{
		CommandElt clear;
		clear.Command = "clear";

		addCommand(clear);

		CommandElt options;
		options.Command = "console";
		options.SubCommand.push_back("textcolor");

		addCommand(options);

		addToConsole();
	}

	virtual void applyCommand(const String &args)
	{
		if (getCommand() == "console")
		{
			if (getSubCommand() == "textcolor")
			{
				Color color;
				color.parse(args);
				color.alpha(1.f);

				m_console->setFontColor(color);
			}
		}
		else if (getCommand() == "clear")
		{
			m_console->clear();
		}
	}
};

//---------------------------------------------------------------------------------------
// Console
//---------------------------------------------------------------------------------------

O3D_IMPLEMENT_DYNAMIC_CLASS1(Console, GUI_CONSOLE, Widget)

// constructor
Console::Console(
	BaseObject *parent,
	ABCFont *font) :
        Widget(parent),
        m_getText(this, font),
		m_fontSize(12),
		m_fontColor(0.f, 1.f, 0.f),
		m_autocomp(True),
		m_maxcommand(16),
		m_drawcommand(10),
        m_clearCommand(nullptr)
{
	m_name = "Scene::Gui::WidgetManager::Console";
	m_com = m_commandMap.end();
	m_clearCommand = new DefaultCommand(this);
    m_pos.set(5, 150);
}

Console::~Console()
{
    m_getText.setFont(nullptr);
	m_commands.clear();
	deletePtr(m_clearCommand);
}

Vector2i Console::getDefaultSize()
{
    return getUsedTheme()->getDefaultWidgetSize(Theme::WINDOW_NORMAL);
}

// add a command to the list
void Console::addCommand(Command *command)
{
	if (!command)
		return;

	IT_StringMapToList elt;

	// find for command
	for (Command::IT_CommandEltList it = command->getCommandList().begin();
		it != command->getCommandList().end();
		++it)
	{
		elt = m_commandMap.find((*it).Command);

		// not existing so add the command
		if (elt == m_commandMap.end())
		{
			m_commandMap[(*it).Command] = (*it).SubCommand;
			m_commandMap[(*it).Command].sort();
		}
		// otherwise add all subcommand
		else
		{
			elt->second.insert(
					elt->second.begin(),
					(*it).SubCommand.begin(),
					(*it).SubCommand.end());

			elt->second.sort();
		}
	}

	m_commands.push_back(command);
	command->setConsole(this);
}

// remove a command from the list
void Console::removeCommand(Command* command)
{
	if (!command)
		return;

	IT_StringMapToList elt;

	// find for commands
	for (Command::IT_CommandEltList it = command->getCommandList().begin();
		it != command->getCommandList().end();
		++it)
	{
		elt = m_commandMap.find((*it).Command);

		// found command so remove all subcommand of the subtree
		if (elt != m_commandMap.end())
		{
			for (IT_StringList strit = (*it).SubCommand.begin();
				strit != (*it).SubCommand.end();
				++strit)
			{
				elt->second.remove(*strit);
			}
		}
	}

	m_commands.remove(command);
    command->setConsole(nullptr);
}

Bool Console::keyboardToggled(Keyboard *keyboard, KeyEvent event)
{
    if (!isActive())
        return False;

    m_getText.keyboardToggled(keyboard, event);

	// only key pressed and repeat
	if (event.isReleased())
        return False;

	if (m_commandlist.size())
	{
		// prev command line
		if ((event.key() == KEY_UP) && (m_curcommand != m_commandlist.begin()))
		{
			if (m_curcommand == m_commandlist.end())
                m_memCom2 = m_getText.text();

			m_curcommand--;
		}
		// next command line
		else if ((event.key() == KEY_DOWN) && (m_curcommand != m_commandlist.end()))
		{
			m_curcommand++;
			if (m_curcommand == m_commandlist.end())
			{
				m_memCom = m_memCom2;
                m_getText.setText(m_memCom2);
                m_getText.setCursorPos(m_memCom2.length());

                return True;
			}
		}
		else if ((event.key() == KEY_DOWN) && (m_curcommand == m_commandlist.end()))
		{
            return True;
		}
		else
            return True;

		if (m_curcommand != m_commandlist.end())
		{
			m_memCom = *m_curcommand;
            m_getText.setText(*m_curcommand);
            m_getText.setCursorPos((*m_curcommand).length());
		}
	}

    return True;
}

Bool Console::character(Keyboard *keyboard, CharacterEvent event)
{
    if (!isActive())
        return False;

	//Bool ctrl = keyboard->isKeyDown(KEY_LCONTROL) || keyboard->isKeyDown(KEY_RCONTROL);
    m_getText.character(keyboard, event);

	// valid the command
    if ((event.unicode() == '\n') && (m_getText.text().length() > 0))
	{
        if (m_commandlist.size() && (m_commandlist.back() != m_getText.text()))
		{
            m_commandlist.push_back(m_getText.text());
		}
		else if (m_commandlist.size() == 0)
		{
            m_commandlist.push_back(m_getText.text());
		}
		m_curcommand = m_commandlist.end();

        m_drawlist.push_back(m_getText.text());

		if (m_commandlist.size() > m_maxcommand)
			m_commandlist.pop_front();

		if (m_drawlist.size() > m_drawcommand)
			m_drawlist.pop_front();

		// the command
        String command = m_getText.text();
		Int32 pos = command.find(' ');

		if (pos != -1)
			command.truncate(pos);

		// find the O3DCommand object
		IT_CommandList it;

		String args;
		String subcommand;

		for (it = m_commands.begin(); it != m_commands.end(); ++it)
		{
			// search for the command
			if ((*it)->findCommand(command))
			{
				// is there some subcommand for this command ?
				// If yes and no subcommand typed, so incomplete command...
				if ((*it)->isSubCommand(command))
				{
					// the subcommand ...
                    subcommand = m_getText.text().sub(pos+1);

					if (subcommand.length() == 0)
					{
						putText("incomplete command");
						break;
					}

					// and the argument
					pos = subcommand.find(' ');
					if (pos != -1)
					{
						if (pos+1 < (Int32)subcommand.length())
						{
							args = subcommand.sub(pos+1);
							args.trimRight(' ');
						}

						subcommand.truncate(pos);
					}

					// invalid subcommand
					if (!(*it)->findSubCommand(command,subcommand))
					{
						putText("invalid or bad subcommand");
						break;
					}
				}
				else
				// only the argument
				{
                    args = m_getText.text().sub(pos+1);
				}

				(*it)->applyCommand(args);
				break;
			}
		}

        m_getText.setText("");
        m_getText.setCursorPos(0);

		m_com = m_commandMap.end();
		m_memCom = "";

		if (it == m_commands.end())
			putText("invalid or bad command");

        return True;
	}

	// auto-completion
	if (event.unicode() == '\t')
	{
		String command = m_memCom;
		Int32 pos = command.find(' ');
		String subcommand;

		// find the sub-command
		if (pos != -1)
		{
			// is there already a bit of subcommand
			if (pos+1 < (Int32)m_memCom.length())
			{
				subcommand = m_memCom.sub(pos+1);
				subcommand.trimRight(' ');
				command.truncate(pos);
			}
			command.trimRight(' ');

			UInt32 count = 0;

			// is there a command found
			if (m_com == m_commandMap.end() || m_com->first != command)
			{
				m_com = m_commandMap.find(command);

				// no command found then can't auto-complete the subcommand
				if (m_com == m_commandMap.end())
                    return True;

				// no sub
				if (m_com->second.size() == 0)
                    return True;

				m_subcom = m_com->second.begin();
			}

			// no sub
			if (m_com->second.size() == 0)
                return True;

			// sub-command path
			if (m_subcom == m_com->second.end())
				m_subcom = m_com->second.begin();

			// no subcommand so give all
			if (subcommand.length())
			{
				// turn in loop
                while (((*m_subcom).sub(subcommand,0) != 0) && (count < m_com->second.size()))
				{
					m_subcom++;
					count++;

					if (m_subcom == m_com->second.end())
						m_subcom = m_com->second.begin();
				}

				// nothing found
				if (count == m_com->second.size())
                    return True;
			}

            m_getText.setText(m_com->first + ' ' + *m_subcom + ' ');
            m_getText.setCursorPos(m_getText.text().length());

			m_subcom++;
            return True;
		}
		// find the command
		else
		{
			UInt32 count = 0;

			// no command
			if (m_commandMap.size() == 0)
                return True;

			if (m_com == m_commandMap.end())
				m_com = m_commandMap.begin();

			// no command so give all
			if (command.length())
			{
				// turn in loop
                while ((m_com->first.sub(command,0) != 0) && (count < m_commandMap.size()))
				{
					m_com++;
					count++;

					if (m_com == m_commandMap.end())
						m_com = m_commandMap.begin();
				}

				// nothing found
				if (count == m_commandMap.size())
                    return True;
			}

			m_subcom = m_com->second.begin();
            m_getText.setText(m_com->first + ' ');
            m_getText.setCursorPos(m_getText.text().length());
			m_com++;

            return True;
		}
	}
	// others char
    else if ((event.unicode() != 0) || (event.key() == KEY_DELETE))
	{
        m_memCom = m_getText.text();
		m_com = m_commandMap.end();
	}

    return True;
}

Bool Console::cut(Bool primary)
{
    if (m_getText.text().isValid())
    {
        Bool result = getScene()->getRenderer()->getAppWindow()->pasteToClipboard(m_getText.text(), primary);

        if (result)
        {
            m_getText.setText("");
        }

        return result;
    }

    return False;
}

Bool Console::copy(Bool primary)
{
    if (m_getText.text().isValid())
    {
        return getScene()->getRenderer()->getAppWindow()->pasteToClipboard(m_getText.text(), primary);
    }

    return False;
}

Bool Console::paste(Bool primary)
{
    String text = getScene()->getRenderer()->getAppWindow()->copyFromClipboard(primary);
    if (text.isValid())
    {
         m_getText.setText(text);
         return True;
    }

    return False;
}

// put a line into the console (or a multi line with \n)
void Console::putText(const String &text)
{
	// simple line
	if (text.find('\n') == -1)
	{
		m_drawlist.push_back(text);

		if (m_drawlist.size() > m_drawcommand)
			m_drawlist.pop_front();
	}
	else
	{
		Int32 pos;
		UInt32 oldpos=0;

		while (oldpos < text.length() && (pos = text.find('\n',oldpos)) != -1)
		{
			String line = text.sub(oldpos);

			line.truncate(pos);
			oldpos = pos;
			oldpos++;

			m_drawlist.push_back(line);

			if (m_drawlist.size() > m_drawcommand)
				m_drawlist.pop_front();
		}

		if (oldpos < text.length())
		{
			m_drawlist.push_back(text.sub(oldpos));

			if (m_drawlist.size() > m_drawcommand)
				m_drawlist.pop_front();
		}
	}
}

// draw all visible command with the defined font
void Console::draw()
{
    if (isShown() && m_getText.font())
	{
        processCache();

        Vector2i curPos(m_pos);

        m_getText.font()->setColor(m_fontColor);
        m_getText.font()->setTextHeight(m_fontSize);

		// top-alignment
        curPos.y() += m_getText.font()->getTextHeight();

		// draw old command
		for (IT_StringList it = m_drawlist.begin(); it != m_drawlist.end(); ++it)
		{
            m_getText.font()->write(curPos, *it, -1);
            curPos[Y] += m_getText.font()->getTextHeight();
		}

		// draw current command with cursor
        if (isActive())
            m_getText.draw(curPos);
		else
            m_getText.font()->write(curPos,m_getText.text());
    }
}

void Console::updateCache()
{
    //Theme *theme = getUsedTheme();
    // TODO
}

