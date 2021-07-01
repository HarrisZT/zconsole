/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved.  
* 
* File: zcommandtable.cpp
* Desc: interface for console commands
*******************************************************************************


This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
******************************************************************************/  
#include "zhelperfuncs.hpp"  
#include "zhistory.hpp"
#include "zconsoledefs.hpp"
#include "zscreen.hpp"
#include "zcommandtable.hpp" 



 

/* Section 1:
** Initialization and Command parsing*/
//***************************************************************************//
void Zconsole::ZcommandTable::initHelpTextTable(
	std::map<zstring, zstrings>& _table) const {  
	
#define CMD_NO_ARGS \
      "If no argument is given, command will be ignored",\
      "or will display the current configuration."

#define VALID_BOOL_ARGS "", \
      "1 		 0      ",  \
      "on 	     off    ",  \
      "set       unset  ",  \
      "true	     false  ",  \
      "enable    disable",  \
      CMD_NO_ARGS,       "" 
	 
	_table["cls"] = {
		"clears the console screen.",
		"Example: \"cls\"",
	};
	_table["color"] = {
		"sets the default console text color.",
		"color [attr] - (attr) specifies color identified by a hex digit",
		"                      digit can be any of the following values:",
		"",
		"0 = Green     6 = Cyan		",
		"1 = Brown     7 = Magenta  ",
		"2 = Amber     8 = Lt Green ",
		"3 = Grey      9 = Blue 	",
		"4 = Red       A = White    ",
		"5 = Purple                 ",
		"",
		CMD_NO_ARGS,
		"Example: \"color 0\" produces green text",
	};  
	_table["delhistory"] = {
	   "clears the command history.",
	   "Example: \"delhistory\"",
	};
	_table["echo"] = {
		"displays messages, or turns command-echoing on or off.",
		"",
		"echo [on | off]",
		"echo [message]",
		"",
		"Or type \"echo\" w/o parameters to display current echo setting.",
	};
	_table["exit"] = {
		"quits the command interpretor",
		"example: \"exit\"",
	}; 
	_table["help"] = {
		"provides help information for console commands.",
		"help [command] - (command) displays help information on command.",
		"",
		"example: \"help color\"",
	};
	_table["loadhistory"] = {
		"loads previously saved command history from disk",
		"example: \"loadhistory\"",
	};
	_table["palette"] = {
		"demonstrates the palette by printing in each color.",
		"example: \"palette\"",
	};
	_table["savehistory"] = {
		"saves the command history to a text file on diskr",
		"example: \"savehistory\"",
	}; 
	_table["scale"] = {
	    "define the scale factor the console is rendered at.",
	    "example: \"scale\"",
	};
	_table["showhistory"] = {
		"displays command history",
		"example: \"showhistory\"",
	};   
	_table["loadstate"] = {
		"load a state into memory.", 
		CMD_NO_ARGS,
		"Example: \"loadstate test\" loads the test program into memory",
	};
	_table["ver"] = {
	   "displays the console version",
	   "example: \"ver\"",
	}; 
#undef CMD_NO_ARGS
#undef VALID_BOOL_ARGS 
} 


void Zconsole::ZcommandTable::initialize() { 
	std::vector<zcommand> commandlist;
	{
		zcommand command;
	    #define BIND(x)\
            std::bind(&ZcommandTable::x, this, std::placeholders::_1)  

		command.name = "cls";
		command.func = BIND(cmd_cls);
		commandlist.push_back(command);

		command.name = "color";
		command.func = BIND(cmd_color);
		commandlist.push_back(command);

		command.name = "delhistory";
		command.func = BIND(cmd_delhistory);
		commandlist.push_back(command);

		command.name = "echo";
		command.func = BIND(cmd_echo);
		commandlist.push_back(command);

		command.name = "exit";
		command.func = BIND(cmd_exit);
		commandlist.push_back(command);

		command.name = "help";
		command.func = BIND(cmd_help);
		commandlist.push_back(command);

		command.name = "loadhistory";
		command.func = BIND(cmd_loadhistory);
		commandlist.push_back(command);		

		command.name = "palette";
		command.func = BIND(cmd_palette);
		commandlist.push_back(command);

		command.name = "savehistory";
		command.func = BIND(cmd_savehistory);
		commandlist.push_back(command);

		command.name = "scale";
		command.func = BIND(cmd_scale);
		commandlist.push_back(command); 

		command.name = "showhistory";
		command.func = BIND(cmd_showhistory);
		commandlist.push_back(command); 

		command.name = "ver";
		command.func = BIND(cmd_ver);
		commandlist.push_back(command);
	#undef BIND
	}
	m_commandmap.clear(); 
	
	std::map<zstring, zstrings> helptext;
	initHelpTextTable(helptext);

	for (auto& it : commandlist) {
		auto help = helptext.find(it.name);
		if (help != helptext.end())
			it.help = help->second;

		if (!insert(it)) { 
			return;
		}
	}  
} 


bool Zconsole::ZcommandTable::insert(const zcommand& _command) {  
	auto entry = m_commandmap.find(_command.name);
	if (entry == m_commandmap.end()) {
		m_commandmap.insert(std::make_pair(_command.name, _command));
		if (m_longestcmd.length() < _command.name.length()) 
			m_longestcmd = _command.name; 
		return true;
	}
	return false; 
}


bool Zconsole::ZcommandTable::remove(const zcommand& _command) {
	auto entry = m_commandmap.find(_command.name);
	if (entry != m_commandmap.end()) {
		m_commandmap.erase(_command.name);
		return true;
	}
	return false;
}


void Zconsole::ZcommandTable::run(zstrings& _args) {
	static zstring lastcommand;
	lastcommand = "";

	const auto it = m_commandmap.find(_args[0]);
	if (it != m_commandmap.end()) {
		/*
		Remove command name and empty args:*/
		_args.erase(_args.begin());
		for (auto it = _args.begin(); it != _args.end();) {
			if (!it->empty()) ++it;
			else it = _args.erase(it);
		}
		//lowercase:
		for (auto& it : _args) {
			it = Zconsole_StringToLower(it);
		}
		lastcommand = it->second.name;
		it->second.func(_args);
	}
	else {
		m_lpconsole->print("unknown command: " + _args[0]);
	}
	m_lpconsole->print(" ");
} 


 


/* Section 2:
** Command function definitions*/
//***************************************************************************//
void Zconsole::ZcommandTable::cmd_cls(zstrings _args) {
	m_lpconsole->m_screen->clear();
	m_lpconsole->print("console screen cleared"); 
} 
 

void Zconsole::ZcommandTable::cmd_color(zstrings _args) {
	unsigned int rgba = m_lpconsole->m_textcolor; //rgba color code
	zstring      cstr = "";                       //color string 
	 
	if (_args.empty()) { 
		Zconsole_GetColorCodes(rgba, rgba, cstr);
		m_lpconsole->print("console font color is set to: " + cstr);
		return;
	} 
	rgba = Zconsole_HexStringToUint(_args[0].c_str());
	if (rgba > 0xA) 
		m_lpconsole->print("invalid command arguments"); 
	else {
		Zconsole_GetColorCodes(rgba, rgba, cstr);
		m_lpconsole->m_textcolor = rgba;
		m_lpconsole->print("console font color was set to: " + cstr);
	}
} 
 

void Zconsole::ZcommandTable::cmd_delhistory(zstrings _args) { 
	m_lpconsole->m_history->clear();
} 
 

void Zconsole::ZcommandTable::cmd_echo(zstrings _args) {  
	if (!_args.empty() && !_args[0].empty()) 
		m_lpconsole->print(_args[0]); 
	else m_lpconsole->print("echo is enabled"); 
	m_lpconsole->print(" ");
}
 

void Zconsole::ZcommandTable::cmd_exit(zstrings _args) {
	m_lpconsole->close(); 
}  

void Zconsole::ZcommandTable::cmd_help(zstrings _args) { 
	zstring      dash, cnum, line;
	unsigned int index, longest, numspaces, n;

	m_lpconsole->print(" "); 
	m_lpconsole->print(m_lpconsole->m_partition);  
	 
	if (!_args.empty()) { 
		auto entry = m_commandmap.find(_args[0]);
		if (entry != m_commandmap.end()) {
			for (const auto& line : entry->second.help)
				m_lpconsole->print(line);
			m_lpconsole->print(m_lpconsole->m_partition); 
			return;
		}
	} 	
	index   = 0; 
	longest = m_longestcmd.length();  
	
	for (auto& it : m_commandmap) { 
		dash.clear();
		numspaces = longest - it.first.length();
		for (n = 0; n < numspaces; ++n) 
			dash += " "; 
		dash += " - ";

		cnum = Zconsole_UintToHexString(index, "fmat2p");
		line = cnum + " " + it.first + dash + it.second.help[0];
		m_lpconsole->print(line); 
		index++;
	} 
	m_lpconsole->print(m_lpconsole->m_partition);
	line = 
		"number of available commands: " +
		std::to_string(index); 
	m_lpconsole->print(line); 
} 
 

void Zconsole::ZcommandTable::cmd_loadhistory(zstrings _args) { 
	m_lpconsole->m_history->load();  
} 


void Zconsole::ZcommandTable::cmd_palette(zstrings _args) { 
	zstring      cstr;
	unsigned int index, tmp;

	tmp = m_lpconsole->m_textcolor; 

	m_lpconsole->print("");
	m_lpconsole->print(m_lpconsole->m_partition);

	for (index = 0x0; index < 0xB; ++index) { 
		Zconsole_GetColorCodes(index, m_lpconsole->m_textcolor, cstr); 
		m_lpconsole->print(
			"This is text printed in color: " + 
			Zconsole_UintToHexString(index,"fmat1np") + " " + cstr);
	}
	m_lpconsole->m_textcolor = tmp;
	m_lpconsole->print(m_lpconsole->m_partition);
} 
  

void Zconsole::ZcommandTable::cmd_savehistory(zstrings _args) { 
	m_lpconsole->m_history->save(false); 
} 


void Zconsole::ZcommandTable::cmd_scale(zstrings _args) {  
	zstring str;
	float   fValue;

	str    = "0.25";
	fValue = 0.25f; 
	 
	if (_args.empty()) 
		m_lpconsole->print("scale set to default value"); 
	else {
		if (!_args.empty() && !_args[0].empty())
			str = _args[0];

		if (str != "default" && str != "0.25") {
			fValue = Zconsole_StringToFloat(str);
			if (fValue < 0.15f) {
				fValue = 0.15f;
				str = "value scale range is between 0.15 - 1.00";
			}
			if (fValue > 1.f) {
				fValue = 1.f;
				str = "value scale range is between 0.15 - 1.00";
			}
		}
	}
	m_lpconsole->m_renderscale = fValue;

	m_lpconsole->print(str);  
	m_lpconsole->print(" ");  
} 


void Zconsole::ZcommandTable::cmd_showhistory(zstrings _args) { 
	m_lpconsole->m_history->print();
} 


void Zconsole::ZcommandTable::cmd_ver(zstrings _args) {
	m_lpconsole->print(m_lpconsole->getVersion());
} 
/*****************************************************************************/  
//EOF
/*****************************************************************************/  
