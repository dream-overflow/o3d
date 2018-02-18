/**
 * @file console.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CONSOLE_H
#define _O3D_CONSOLE_H

#include "gettext.h"
#include "o3d/engine/scene/sceneentity.h"
#include "o3d/core/stringlist.h"
#include "o3d/gui/widgets/widget.h"

#include "o3d/core/memorydbg.h"

namespace o3d {

class Console;

/**
 * @brief A command for the Console widget
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2015-12-30
 * A command is an object that must be inherited. In the object you can redefine
 * a command and subcommand sets with theirs actions.
 * Finally you only need to send objects that inherit from Command to Console
 * that will manage them.
 */
class O3D_API Command
{
public:

	struct CommandElt
	{
		String Command;
		T_StringList SubCommand;
	};

	typedef std::list<CommandElt> T_CommandEltList;
	typedef T_CommandEltList::iterator IT_CommandEltList;
	typedef T_CommandEltList::const_iterator CIT_CommandEltList;

	//! Default constructor.
	Command(Console *console);

	//! Virtual destructor.
	virtual ~Command();

	//-----------------------------------------------------------------------------------
	// Usage, registered command and subcommand are managed using the AddCommand(...).
	//-----------------------------------------------------------------------------------

	//! get the first command
	const String* findCommand(const String& find);

	//! get the first subcommand
	const String* findSubCommand(const String& command,const String& find);

	//! is there one or more subcommand for a given command
	Bool isSubCommand(const String& command);

	//! apply a command on the object with some args (like 1, true...)
	virtual void applyCommand(const String &args) = 0;

	//! get the found command
	inline const String& getCommand()const { return m_foundCommand; }

	//! get the found subcommand
	inline const String& getSubCommand()const { return m_foundSubCommand; }

	//-----------------------------------------------------------------------------------
	// Initialization
	//-----------------------------------------------------------------------------------

	//! get the command list
	inline T_CommandEltList& getCommandList() { return m_command; }

	//! draw a text into the console
	void putText(const String &text);

	//! Set the owner console
	void setConsole(Console *pConsole) { m_console = pConsole; }

	//! Get the owner console
	inline Console* getConsole() { return m_console; }
	//! Get the owner console (const version)
	inline const Console* getConsole() const { return m_console; }

	//! Add to the defined parent console
	void addToConsole();

protected:

	Console *m_console;       //!< the owner console

	CIT_StringList     m_currentSub;       //!< iterator on current subcommand
	CIT_CommandEltList m_currentCommand;   //!< iterator on current command
    T_CommandEltList   m_command;          //!< the multimap that contain all command and subcommand

	String m_foundCommand;     //!< substring for find next command
	String m_foundSubCommand;  //!< substring for find next subcommand

	//! Add a command element.
	inline void addCommand(const CommandElt &elt) { m_command.push_back(elt); }
};

typedef std::list<Command*> T_CommandList;
typedef T_CommandList::iterator IT_CommandList;
typedef T_CommandList::const_iterator CIT_CommandList;

/**
 * @brief A full options console standalone widget
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-05-13
 */
class O3D_API Console : public Widget
{
public:

    O3D_DECLARE_CLASS(Console)

	//! Default constructor
	Console(
		BaseObject *parent,
        ABCFont *font = nullptr);

	//! destructor
	virtual ~Console();

    //! get the recommended widget default size
    virtual Vector2i getDefaultSize() override;

	//! get the text getter
    inline       GetText& getTextGetter()       { return m_getText; }
    inline const GetText& getTextGetter() const { return m_getText; }

	//! set/get used font
    inline void setFont(ABCFont* pFont) { m_getText.setFont(pFont); }
    inline       ABCFont* getFont()       { return m_getText.font(); }
    inline const ABCFont* getFont() const { return m_getText.font(); }

	//! set the font color
	inline void setFontColor(const Color &color) { m_fontColor = color; }
	//! get the font color
	inline const Color& getFontColor() const { return m_fontColor; }

	//! set the font size
	inline void setFontSize(Int32 size) { m_fontSize = size; }
	//! get the font size
	inline Int32 getFontSize() const { return m_fontSize; }

	//! Key event.
    virtual Bool keyboardToggled(Keyboard *keyboard, KeyEvent event) override;
	//! Character event.
    virtual Bool character(Keyboard *keyboard, CharacterEvent event) override;

	//! add a command to the list
	void addCommand(Command* command);
	//! remove a keyword from the list
	void removeCommand(Command* command);

	//! Enable auto-completion
	inline void enableAutoCompletion() { m_autocomp = True; }
	//! Disable auto-completion
	inline void disableAutoCompletion() { m_autocomp = False; }
	//! Is autocompletion enable.
	inline Bool isAutoCompletion() const { return m_autocomp; }

	//! Clear the console.
	inline void clear() { m_drawlist.clear(); }

	//! get last command line
	inline String getLastCommand() const
	{
		if (m_curcommand != m_commandlist.end())
			return *m_curcommand;
		return String("");
	}

	//! Set the number of max memorised commands.
	inline void setMaxCommand(UInt32 max) { m_maxcommand = max; }
	//! Get the number of max memorised commands.
	inline UInt32 getMaxCommand() const { return m_maxcommand; }

	//! Set the number of max displayed commands
	inline void setDrawCommand(UInt32 draw) { m_drawcommand = draw; }
	//! Get the number of max displayed commands
	inline UInt32 getDrawCommand() const { return m_drawcommand; }

    //! Cut the current selection and send it to the system clipboard.
    //! @param primary To the primary clipboard if True and available.
    //! @return True if the cut success (has content, content sent).
    virtual Bool cut(Bool primary=False) override;
    //! Copy the current selection and send it to the system clipboard.
    //! @param primary To the primary clipboard if True and available.
    //! @return True if the paste success (has content, content sent).
    virtual Bool copy(Bool primary=False) override;
    //! Replace the current selection by the content of the system clipboard.
    //! @param primary Use the primary clipboard if True and available.
    //! @return True if the copy success (has content, compatible content type).
    virtual Bool paste(Bool primary=False) override;

	//! put a line into the console (or a multi line with \n)
	void putText(const String& text);

	//! draw all visible command with the defined font
    virtual void draw() override;
    virtual void updateCache() override;

protected:

	typedef std::map<String,T_StringList> T_StringMapToList;
	typedef T_StringMapToList::iterator IT_StringMapToList;
	typedef T_StringMapToList::const_iterator CIT_StringMapToList;

    GetText m_getText;       //!< the text getter
    Int32 m_fontSize;        //!< the text getter font size
	Color m_fontColor;       //!< the text getter font color

	Bool m_autocomp;        //!< the autocompletion state

	UInt32 m_maxcommand;    //!< max nbr of command whoose are memorised
	UInt32 m_drawcommand;   //!< nbr of drewn command line (1 per line)

	T_StringList m_commandlist;    //!< the list of memorised command
	IT_StringList m_curcommand;    //!< the current selected command in list

	T_StringList m_drawlist;       //!< the list of drewn command

	T_CommandList m_commands;      //!< the list of commands to executes

	IT_StringMapToList m_com;      //!< the found commmand
	IT_StringList m_subcom;        //!< the found subcommand
	T_StringMapToList m_commandMap;//!< the map whose contain all command and subcommand

    String m_memCom;        //!< bit of command or sub-command to search
	String m_memCom2;

	Command *m_clearCommand;
};

} // namespace o3d

#endif // _O3D_CONSOLE_H
