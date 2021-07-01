/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved.  
* 
* File: zconsole.hpp
* Desc: public interface for console
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
#ifndef __ZCONSOLE_HPP__
#define __ZCONSOLE_HPP__

#include "ztypes.h" 
 
 

class Zconsole final : sf::NonCopyable {  
public:
	static Zconsole* getSingleton() {
		static Zconsole singleton;
		return &singleton;
	}  
public:
	/*
	Initialize the console system. 
	@_directory: where to load/save a history file
	@return    : true on success, false otherwise*/
	bool initialize(const zstring& _directory);

	/*
	Specifies the directory where Zconsole can load/save history
	@_directory: where to load/save a history file*/
	void setDirectory(const zstring& _directory);

	/*
	Shutdown the console and release resources*/
	void release();

	/*
	Register a new command
	@_command: the command to register
	@return  : true if command was registered, false on error*/
	bool insertCommand(const zcommand& _command);

	/*
	Unregister a command.
	@_command: the command to unregister
	@return  : true if command was removed, 
	           false if the command could not be found or error*/
	bool removeCommand(const zcommand& _command);

	/*
	Print a given string to the console screen*/
	void print(const zstring& _str);

	/*
	Returns a pointer to the internal font used by the console.
	(can be used as a default fallback font for example)*/
	const sf::Font* const getFont() const;

	/*
	Returns a string containing the current version of Zconsole*/
	inline zstring getVersion() const { 
		return "Zconsole [ver 210701]"; 
	} 

public:
	/*
	Activate and open the console window*/
	void open();

	/*
	Deactivate and close the console window*/
	void close();

	/*
	Process system events- 
	@_nextevent: the next event in queue*/
	void onEvent(const sf::Event& _event);

	/*
	Update the console internal state.
	@_delta: fixed time step 
	@return: false when the console is ready to close*/
	bool onUpdate(float _delta); 

	/*
	Render contents of the console screen to the given RenderTarget*/
	void onPresent( 
	    sf::RenderTarget& _rendertarget,
	    sf::RenderStates  _renderstates = sf::RenderStates::Default);

private:
	Zconsole();
	~Zconsole();

	void pollInput();
	void processNextCommand();
	void parseLastLine(bool _isCommandEntry);
	void scrollHistory(int _direction); 

	class Zscreen;      
	class Zcursor;      
	class Zhistory;     
	class ZcommandTable; 
	
	zstring                        m_commandbuffer; 
	std::unique_ptr<ZcommandTable> m_commandtable;
	std::unique_ptr<Zcursor>       m_cursor;
	bool                           m_entrykeyflag;   
	std::unique_ptr<sf::Font>      m_font;
	std::unique_ptr<Zhistory>      m_history;
	bool                           m_isOpenFlag;
	zstring                        m_lastline;
    unsigned int                   m_lastlineOffset;
	zstring                        m_partition;
	unsigned int                   m_pendingUpdates;
	float                          m_renderscale; 
	std::unique_ptr<Zscreen>       m_screen; 
	unsigned int                   m_textcolor;
	zvertices                      m_vertices;  
}; 
/*****************************************************************************/  
#endif //EOF
/*****************************************************************************/  
