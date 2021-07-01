/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved.  
* 
* File: zcommandtable.hpp
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
#ifndef __ZCOMMANDTABLE_HPP__
#define __ZCOMMANDTABLE_HPP__ 

#include "zconsole/zconsole.hpp"



class Zconsole::ZcommandTable final : sf::NonCopyable { 
public:  
	ZcommandTable(Zconsole* const _lpconsole) :
		m_lpconsole(_lpconsole) {}
	~ZcommandTable() {}	

	/*
	Initialize the command table*/
	void initialize(); 

	/*
	Try to run a command, if it is registered.
	@_args: command arguments*/
	void run(zstrings& _args); 

	/*
	Register a new command
	@_command: the command to register
	@return  : true if command was registered, else false*/
	bool insert(const zcommand& _command);

	/*
	Unregister a command.
	@_command: the command to unregister
	@return  : true if command was removed, 
	           false if the command could not be found*/
	bool remove(const zcommand& _command);

private: 
	void initHelpTextTable(std::map<zstring, zstrings>& _table) const; 

	std::map<zstring, zcommand> m_commandmap;  
	zstring                     m_longestcmd; 
	Zconsole* const             m_lpconsole;

private: 
    void cmd_cls         (zstrings _cmdargs); 
	void cmd_color       (zstrings _cmdargs); 
	void cmd_delhistory  (zstrings _cmdargs); 
	void cmd_echo        (zstrings _cmdargs);  
    void cmd_exit        (zstrings _cmdargs);  
    void cmd_help        (zstrings _cmdargs); 
    void cmd_loadhistory (zstrings _cmdargs);	 
	void cmd_palette     (zstrings _cmdargs);
    void cmd_savehistory (zstrings _cmdargs);   
	void cmd_scale       (zstrings _cmdargs); 
	void cmd_showhistory (zstrings _cmdargs);  
    void cmd_ver         (zstrings _cmdargs);
}; 
/*****************************************************************************/  
#endif //EOF
/*****************************************************************************/  
